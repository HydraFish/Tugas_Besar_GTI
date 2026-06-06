#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>

#include "BmpLoader.h"
#include "Kamera.h"
#include "Object.h"

// ==========================================
// KONFIGURASI BIDANG & VARIABEL GLOBAL
// ==========================================
const int GRID_W = 30; 
const int GRID_D = 50; 

GLuint wallTexture;
GLuint roadTexture;
GLuint woodTexture; 
GLuint oilTexture;  

// STATUS GAME (State Machine)
enum GameState { MAIN_MENU, KEYS, PLAYING, WIN, GAME_OVER };
GameState state = MAIN_MENU; 

float bgR = 0.2f, bgG = 0.1f, bgB = 0.3f;
const float GAME_TIME_LIMIT = 60.0f; 
float timeRemaining = GAME_TIME_LIMIT;
int lastTimeMs = 0;

// Variabel Mouse & Terrain
bool isMouseDown = false;
int currentMouseX = 0, currentMouseY = 0;
float mouseHitX = -1.0f, mouseHitZ = -1.0f;
const float BUMP_RADIUS = 3.0f;
const float BUMP_HEIGHT = 1.5f;  

const float FINISH_X_MIN = 13.0f, FINISH_X_MAX = 17.0f;
const float FINISH_Z_MIN = 0.0f,  FINISH_Z_MAX = 2.0f;

// ==========================================
// DEKLARASI FUNGSI (FORWARD DECLARATION)
// ==========================================
void resetGame();
void updateMouse3D(int mouseX, int mouseY);

// ==========================================
// LOGIKA FISIKA TERRAIN
// ==========================================
float getGridHeight(float x, float z) {
    if (!isMouseDown) return 0.0f;
    float dist = sqrt(pow(x - mouseHitX, 2) + pow(z - mouseHitZ, 2));
    if (dist < BUMP_RADIUS) {
        return (cos(dist / BUMP_RADIUS * 3.14159f) + 1.0f) * 0.5f * BUMP_HEIGHT;
    }
    return 0.0f;
}

Point3D calculateNormal(float x, float z) {
    float delta = 0.1f;
    float hL = getGridHeight(x - delta, z);
    float hR = getGridHeight(x + delta, z);
    float hD = getGridHeight(x, z - delta);
    float hU = getGridHeight(x, z + delta);

    Point3D normal = { hL - hR, 2.0f * delta, hD - hU };
    float length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    return { normal.x / length, normal.y / length, normal.z / length };
}

// ==========================================
// UNPROJECT MOUSE
// ==========================================
void updateMouse3D(int mouseX, int mouseY) {
    GLint viewport[4]; 
    GLdouble modelview[16], projection[16];
    GLfloat winX, winY; 
    GLdouble nearX, nearY, nearZ, farX, farY, farZ;

    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);

    winX = (float)mouseX; 
    winY = (float)viewport[3] - (float)mouseY;

    gluUnProject(winX, winY, 0.0, modelview, projection, viewport, &nearX, &nearY, &nearZ);
    gluUnProject(winX, winY, 1.0, modelview, projection, viewport, &farX, &farY, &farZ);

    float dirX = farX - nearX; 
    float dirY = farY - nearY; 
    float dirZ = farZ - nearZ;

    if (dirY != 0.0f) {
        float t = -nearY / dirY;
        mouseHitX = nearX + t * dirX;
        mouseHitZ = nearZ + t * dirZ;
    }
}

// ==========================================
// CALLBACK INPUT MOUSE & KEYBOARD
// ==========================================
void mouseFunc(int button, int state_m, int x, int y) {
    if (state == PLAYING && button == GLUT_LEFT_BUTTON) {
        if (state_m == GLUT_DOWN) {
            isMouseDown = true;
            currentMouseX = x; currentMouseY = y;
        } else if (state_m == GLUT_UP) {
            isMouseDown = false;
        }
    }
}

void motionFunc(int x, int y) {
    if (isMouseDown && state == PLAYING) {
        currentMouseX = x; currentMouseY = y;
    }
}

void keyboardFunc(unsigned char key, int x, int y) {
    if (state == MAIN_MENU) {
        if (key == 's' || key == 'S') { resetGame(); state = PLAYING; }
        else if (key == 'k' || key == 'K') { state = KEYS; }
        else if (key == 'q' || key == 'Q') { exit(0); }
    } 
    else if (state == KEYS) {
        if (key == 'c' || key == 'C') { state = MAIN_MENU; }
        else if (key == 'q' || key == 'Q') { exit(0); }
    } 
    else if (state == WIN || state == GAME_OVER) {
        if (key == 'r' || key == 'R') { resetGame(); state = PLAYING; }
        else if (key == 'm' || key == 'M') { state = MAIN_MENU; }
        else if (key == 'q' || key == 'Q') { exit(0); }
    }
}

void resetGame() {
    bgR = 0.2f; bgG = 0.1f; bgB = 0.3f;
    ball = initialBall; 
    keys = initialKeys; 
    isMouseDown = false;
    mouseHitX = -1.0f; mouseHitZ = -1.0f;
    timeRemaining = GAME_TIME_LIMIT;
    lastTimeMs = glutGet(GLUT_ELAPSED_TIME);
}

// ==========================================
// UPDATE MESIN FISIKA GAME
// ==========================================
void update(int value) {
    int currentTimeMs = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (currentTimeMs - lastTimeMs) / 1000.0f;
    lastTimeMs = currentTimeMs;

    if (deltaMove) moveMeFlat(deltaMove);
    if (deltaAngle) { angle += deltaAngle; orientMe(angle); }

    if (state == PLAYING) {
        timeRemaining -= deltaTime;
        if (timeRemaining <= 0.0f) { timeRemaining = 0.0f; state = GAME_OVER; }

        if (state == PLAYING && !ball.isFalling) {
            float delta = 0.1f;
            float hL = getGridHeight(ball.x - delta, ball.z);
            float hR = getGridHeight(ball.x + delta, ball.z);
            float hU = getGridHeight(ball.x, ball.z - delta);
            float hD = getGridHeight(ball.x, ball.z + delta);

            ball.vx += (hL - hR) * 0.15f; 
            ball.vz += (hU - hD) * 0.15f;

            // Efek Oli
            float friction = 0.95f; 
            for (const auto& o : oils) {
                if (ball.x > o.xMin && ball.x < o.xMax && ball.z > o.zMin && ball.z < o.zMax) {
                    friction = 0.85f; break;
                }
            }
            ball.vx *= friction; ball.vz *= friction;

            // Sumbu X Tembok
            ball.x += ball.vx;
            for (const auto& w : walls) {
                if (ball.x > w.xMin - ball.radius && ball.x < w.xMax + ball.radius &&
                    ball.z > w.zMin - ball.radius && ball.z < w.zMax + ball.radius) {
                    ball.x -= ball.vx; ball.vx *= -0.5f; break;
                }
            }

            // Sumbu Z Tembok
            ball.z += ball.vz;
            for (const auto& w : walls) {
                if (ball.x > w.xMin - ball.radius && ball.x < w.xMax + ball.radius &&
                    ball.z > w.zMin - ball.radius && ball.z < w.zMax + ball.radius) {
                    ball.z -= ball.vz; ball.vz *= -0.5f; break;
                }
            }

            // Sapuan Pendorong Kayu
            for (auto& w : woods) {
                w.x += w.vx;
                if (w.x < w.limitMin || w.x > w.limitMax) w.vx *= -1.0f;

                if (ball.x > w.x - (w.width/2) - ball.radius && ball.x < w.x + (w.width/2) + ball.radius &&
                    ball.z > w.z - (w.depth/2) - ball.radius && ball.z < w.z + (w.depth/2) + ball.radius) {
                    ball.vx += w.vx * 1.5f; 
                    if (ball.z > w.z) ball.vz += 0.2f; else ball.vz -= 0.2f;
                }
            }

            ball.y = getGridHeight(ball.x, ball.z) + ball.radius;

            // Logika Mengambil Kunci
            for (size_t i = 0; i < keys.size(); i++) {
                float dist = sqrt(pow(ball.x - keys[i].x, 2) + pow(ball.y - keys[i].y, 2) + pow(ball.z - keys[i].z, 2));
                if (dist < ball.radius + 0.8f) {
                    keys.erase(keys.begin() + i);
                    break;
                }
            }

            // Pemeriksaan Finish
            if (keys.empty()) {
                bgR = 0.1f; bgG = 0.4f; bgB = 0.3f; 
                if (ball.x > FINISH_X_MIN && ball.x < FINISH_X_MAX &&
                    ball.z > FINISH_Z_MIN && ball.z < FINISH_Z_MAX) {
                    state = WIN;
                }
            }

            // Jatuh Luar Batas
            if (ball.x < 0 || ball.x > GRID_W || ball.z < 0 || ball.z > GRID_D) {
                ball.isFalling = true;
            }
        } else if (ball.isFalling) {
            ball.y -= 0.2f;
            if (ball.y < -10.0f) state = GAME_OVER;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// ==========================================
// RENDER JALAN MAP UTAMA
// ==========================================
void drawGrid() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, roadTexture);
    GLfloat mat_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient);

    glBegin(GL_QUADS);
    for (int z = 0; z < GRID_D; z++) {
        for (int x = 0; x < GRID_W; x++) {
            Point3D p1 = { (float)x, getGridHeight(x, z), (float)z };
            Point3D p2 = { (float)x + 1, getGridHeight(x + 1, z), (float)z };
            Point3D p3 = { (float)x + 1, getGridHeight(x + 1, z + 1), (float)z + 1 };
            Point3D p4 = { (float)x, getGridHeight(x, z + 1), (float)z + 1 };

            Point3D n1 = calculateNormal(p1.x, p1.z); glNormal3f(n1.x, n1.y, n1.z); glTexCoord2f(x, z); glVertex3f(p1.x, p1.y, p1.z);
            Point3D n2 = calculateNormal(p2.x, p2.z); glNormal3f(n2.x, n2.y, n2.z); glTexCoord2f(x + 1, z); glVertex3f(p2.x, p2.y, p2.z);
            Point3D n3 = calculateNormal(p3.x, p3.z); glNormal3f(n3.x, n3.y, n3.z); glTexCoord2f(x + 1, z + 1); glVertex3f(p3.x, p3.y, p3.z);
            Point3D n4 = calculateNormal(p4.x, p4.z); glNormal3f(n4.x, n4.y, n4.z); glTexCoord2f(x, z + 1); glVertex3f(p4.x, p4.y, p4.z);
        }
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    float boxSize = 0.5f; int colIndex = 0;

    for (float x = FINISH_X_MIN; x < FINISH_X_MAX; x += boxSize) {
        int rowIndex = 0;
        for (float z = FINISH_Z_MIN; z < FINISH_Z_MAX; z += boxSize) {
            bool isEven = ((colIndex + rowIndex) % 2 == 0);
            GLfloat mat_color[4];

            if (!keys.empty()) {
                if (isEven) { mat_color[0]=0.8f; mat_color[1]=0.1f; mat_color[2]=0.1f; mat_color[3]=1.0f; } 
                else        { mat_color[0]=0.9f; mat_color[1]=0.9f; mat_color[2]=0.9f; mat_color[3]=1.0f; }
            } else {
                if (isEven) { mat_color[0]=0.15f; mat_color[1]=0.15f; mat_color[2]=0.15f; mat_color[3]=1.0f; } 
                else        { mat_color[0]=0.9f; mat_color[1]=0.9f; mat_color[2]=0.9f; mat_color[3]=1.0f; }
            }

            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_color);
            glBegin(GL_QUADS);
            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f(x, 0.02f, z);
            glVertex3f(x + boxSize, 0.02f, z);
            glVertex3f(x + boxSize, 0.02f, z + boxSize);
            glVertex3f(x, 0.02f, z + boxSize);
            glEnd();
            rowIndex++;
        }
        colIndex++;
    }
}

// ==========================================
// RENDER TEXT & PANEL OVERLAY
// ==========================================
void drawText(float x, float y, const std::string& text) {
    glRasterPos2f(x, y);
    for (char c : text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

void drawTextLarge(float x, float y, const std::string& text) {
    glRasterPos2f(x, y);
    for (char c : text) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
}

void drawUIOverlay(float r, float g, float b, float a) {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glColor4f(r, g, b, a); 
    glBegin(GL_QUADS);
    glVertex2f(0, 0);       glVertex2f(800, 0);
    glVertex2f(800, 600);   glVertex2f(0, 600);
    glEnd();
    
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ==========================================
// SCENE SCREEN RENDERING MANAGEMENT
// ==========================================
void display() {
    glClearColor(bgR, bgG, bgB, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Posisi Kamera Diperbarui
    gluLookAt(camX, camY, camZ, camX + lx, camY + ly, camZ + lz, 0.0f, 1.0f, 0.0f);
    if (isMouseDown && state == PLAYING) updateMouse3D(currentMouseX, currentMouseY);

    glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);

    // Mengganti Aset Lingkungan 3D
    drawGrid();
    drawOils(oilTexture);    
    drawWalls(wallTexture); 
    drawWoods(woodTexture);  

    for (size_t i = 0; i < keys.size(); i++) {
        float floatY = sin(glutGet(GLUT_ELAPSED_TIME) * 0.003f) * 0.3f;
        float currentKeyY = keys[i].y + floatY;

        glPushMatrix();
        float shadowY = 0.04f;
        if (keys[i].z >= 30.0f && keys[i].z <= 32.0f && keys[i].x >= 8.0f && keys[i].x <= 22.0f) shadowY = 1.52f; 
        glTranslatef(keys[i].x, shadowY, keys[i].z);
        float shadowScale = 0.25f * (1.0f - (floatY * 0.4f));
        glScalef(shadowScale, 0.01f, shadowScale); 
        glDisable(GL_LIGHTING); glColor3f(0.12f, 0.12f, 0.12f); glutSolidSphere(1.5f, 16, 16); glEnable(GL_LIGHTING);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(keys[i].x, currentKeyY, keys[i].z);
        glRotatef(glutGet(GLUT_ELAPSED_TIME) * 0.1f, 0, 1, 0);
        glScalef(0.28f, 0.28f, 0.28f); 
        drawKey(); 
        glPopMatrix();
    }

    drawBall(ball);
    
    // ==========================================
    // PENGATURAN TAMPILAN INTERFACE HUD
    // ==========================================
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION); 
    glPushMatrix(); 
    glLoadIdentity(); 
    gluOrtho2D(0, 800, 0, 600);
    
    glMatrixMode(GL_MODELVIEW); 
    glPushMatrix(); 
    glLoadIdentity();

    if (state == MAIN_MENU) {
        drawUIOverlay(0.0f, 0.0f, 0.0f, 0.65f); 
        
        glColor3f(1.0f, 0.84f, 0.0f); 
        drawTextLarge(270, 420, "BALL NGGLINDING");
        
        // Start berada pas di tengah
        glColor3f(1.0f, 1.0f, 1.0f);
        drawTextLarge(285, 300, "PRESS [S] TO START");
        
        // Options dan Quit
        glColor3f(0.85f, 0.85f, 0.85f);
        drawText(520, 560, "-> Press [K] for Key Controls");
        drawText(520, 530, "-> Press [Q] to Quit Game");

    } else if (state == KEYS) {
        drawUIOverlay(0.05f, 0.05f, 0.1f, 0.85f); 
        
        glColor3f(0.2f, 1.0f, 1.0f); 
        drawTextLarge(295, 500, "KEY CONTROLS");
        
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(120, 390, "[ARROW UP / DOWN]       : Move Camera Forward / Backward");
        drawText(120, 350, "[ARROW LEFT / RIGHT]    : Rotate Camera Orientation");
        drawText(120, 310, "[LEFT MOUSE CLICK]      : Deform Terrain (Raise Bump Floor)");
        drawText(120, 270, "[KEYBOARD R]            : Force Restart Mission (In-Game)");
        
        glColor3f(0.0f, 1.0f, 0.4f);
        drawTextLarge(245, 170, ">> Press [C] to Continue to Menu <<");
        glColor3f(1.0f, 0.2f, 0.2f);
        drawTextLarge(275, 120, ">> Press [Q] to Quit Game <<");

    } else if (state == PLAYING) {
        glColor3f(1.0f, 1.0f, 1.0f);
        int displayTime = (int)ceil(timeRemaining);
        drawTextLarge(20, 560, "Time: " + std::to_string(displayTime));

    } else if (state == WIN) {
        drawUIOverlay(0.0f, 0.35f, 0.15f, 0.75f); 
        
        glColor3f(0.0f, 1.0f, 0.5f);
        drawTextLarge(345, 360, "YOU WIN!");
        
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(290, 290, "Press [R] to Restart Match");
        drawText(280, 250, "Press [M] to return to Main Menu");
        drawText(280, 210, "Press [Q] to Quit Application");

    } else if (state == GAME_OVER) {
        drawUIOverlay(0.45f, 0.0f, 0.05f, 0.75f); 
        
        glColor3f(1.0f, 0.2f, 0.2f);
        drawTextLarge(330, 360, "GAME OVER!");
        
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(290, 290, "Press [R] to Try Again");
        drawText(280, 250, "Press [M] to return to Main Menu");
        drawText(280, 210, "Press [Q] to Quit Application");
    }

    glMatrixMode(GL_PROJECTION); 
    glPopMatrix(); 
    glMatrixMode(GL_MODELVIEW); 
    glPopMatrix();
    
    glEnable(GL_DEPTH_TEST); 
    glutSwapBuffers();
}

void initGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    GLfloat light_pos[] = { 10.0f, 20.0f, 10.0f, 0.0f };
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 1.0, 150.0);
    glMatrixMode(GL_MODELVIEW);

    wallTexture   = loadBMP("BMP/Bricks.bmp");    
    roadTexture   = loadBMP("BMP/RUMPUT.bmp");  
    woodTexture   = loadBMP("BMP/Kayu.bmp");  
    oilTexture    = loadBMP("BMP/lumpur.bmp");   
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("NGGLINDING");

    initGL();
    
    bgR = 0.2f; bgG = 0.1f; bgB = 0.3f;
    ball = initialBall; 
    keys = initialKeys; 

    glutDisplayFunc(display);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(motionFunc);
    glutKeyboardFunc(keyboardFunc);
    glutSpecialFunc(pressKey);
    glutSpecialUpFunc(releaseKey);
    
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}