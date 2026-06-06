#include "Object.h"
// 1. Dinding Pembatas - Ditambah tembok baru untuk mempersempit jalan menuju finish
std::vector<Wall> walls = {
    {8.0f, 22.0f, 30.0f, 32.0f, 1.5f}, // Tembok Penghalang Tengah (Tinggi 1.5)
    
    // TEMBOK BARU: Mempersepit jalan dari Z = 0 sampai Z = 15
    {0.0f, 12.0f, 0.0f, 15.0f, 2.0f},  // Menutup sisi kiri depan
    {18.0f, 30.0f, 0.0f, 15.0f, 2.0f}  // Menutup sisi kanan depan
    // Menyisakan celah sempit di X = 12 sampai X = 18 menuju garis finish
};

// 2. Area Oli (Tantangan Awal: Memperlambat)
std::vector<Oil> oils = {
    {2.0f, 28.0f, 35.0f, 42.0f} // Membentang lebar di area awal
};

// 3. Kayu Bergerak (Tantangan Akhir: Menendang Bola)
// Format: {x, z, width, depth, height, vx (kecepatan), batasKiri, batasKanan}
std::vector<Wood> woods = {
    {15.0f, 26.0f, 6.0f, 1.5f, 1.0f, -0.1f, 4.0f, 26.0f}, // Kayu 1
    {10.0f, 18.0f, 6.0f, 1.5f, 1.0f, -0.2f, 4.0f, 26.0f}, // Kayu 2
    {20.0f, 10.0f, 8.0f, 1.5f, 1.0f, 0.2f, 4.0f, 26.0f}  // Kayu 3
};

// 4. Kunci Tersebar 
std::vector<Point3D> initialKeys = {
    {15.0f, 1.0f, 38.0f},        // Kunci 1: Di area awal (dekat oli)
    {15.0f, 1.0f, 8.0f}          // Kunci 2: Di dalam lorong sempit sebelum finish
};
std::vector<Point3D> keys = initialKeys;


// Bola dimulai di ujung peta (Z = 48)
const Ball initialBall = { 15.0f, 0.5f, 48.0f, 0.0f, 0.0f, 0.4f, false };
Ball ball = initialBall;

// ==========================================
// FUNGSI MENGGAMBAR OBJEK BARU
// ==========================================
void drawOils(GLuint textureID) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLfloat mat_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f }; // Gelap (Oli)
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient);

    for (const auto& o : oils) {
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        // Digambar sedikit di atas tanah (y = 0.03f) agar tidak tertumpuk (Z-fighting)
        glTexCoord2f(0.0f, 0.0f); glVertex3f(o.xMin, 0.03f, o.zMin);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(o.xMax, 0.03f, o.zMin);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(o.xMax, 0.03f, o.zMax);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(o.xMin, 0.03f, o.zMax);
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}

void drawWoods(GLuint textureID) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLfloat mat_ambient[] = { 0.6f, 0.4f, 0.2f, 1.0f }; // Warna dasar kayu
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient);

    for (const auto& w : woods) {
        glPushMatrix();
        // Memindahkan kayu ke titik tengahnya
        glTranslatef(w.x, w.height / 2.0f, w.z);
        // Membentuk kotak memanjang
        glScalef(w.width, w.height, w.depth);
        glutSolidCube(1.0);
        glPopMatrix();
    }
    glDisable(GL_TEXTURE_2D);
}


// Fungsi Menggambar Kunci Emas
// ==========================================
// FUNGSI PEMBANTU UNTUK KUNCI (Silinder Tertutup)
// ==========================================
void drawCylinder(float radius, float height) {
    GLUquadric* obj = gluNewQuadric();
    glPushMatrix();
        glRotatef(90, 1, 0, 0); // Putar ke bawah sumbu Y
        gluCylinder(obj, radius, radius, height, 32, 32);
        // Tutup silinder atas dan bawah
        gluDisk(obj, 0, radius, 32, 1);
        glTranslatef(0, 0, height);
        gluDisk(obj, 0, radius, 32, 1);
    glPopMatrix();
    gluDeleteQuadric(obj);
}

// ==========================================
// FUNGSI MENGGAMBAR KUNCI UTAMA
// ==========================================
void drawKey() {
    // Pengaturan Material Emas Solid & Mewah
    GLfloat ambient[]  = { 0.3f, 0.25f, 0.1f, 1.0f };
    GLfloat diffuse[]  = { 0.9f, 0.7f, 0.1f, 1.0f };
    GLfloat specular[] = { 1.0f, 0.9f, 0.5f, 1.0f };
    GLfloat shininess  = 80.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    // 1. KEPALA KUNCI (Torus Oval)
    glPushMatrix();
        glTranslatef(0, 2.0f, 0);
        glScalef(1.4f, 1.0f, 0.5f); 
        glutSolidTorus(0.25, 0.8, 32, 32);
    glPopMatrix();

    // 2. LEHER (Sambungan yang menyatukan kepala ke batang)
    glPushMatrix();
        glTranslatef(0, 1.35f, 0); 
        drawCylinder(0.35f, 0.45f);
    glPopMatrix();

    // 3. BATANG UTAMA (Lurus ke bawah)
    glPushMatrix();
        glTranslatef(0, 0.9f, 0); 
        drawCylinder(0.22f, 2.5f);

        // 4. GIGI KUNCI (2 buah di kanan)
        // Gigi Atas
        glPushMatrix();
            glTranslatef(0.35f, -1.5f, 0);
            glScalef(0.5f, 0.35f, 0.3f);
            glutSolidCube(1.0);
        glPopMatrix();

        // Gigi Bawah (lebih panjang sedikit)
        glPushMatrix();
            glTranslatef(0.45f, -2.0f, 0);
            glScalef(0.7f, 0.35f, 0.3f);
            glutSolidCube(1.0);
        glPopMatrix();
    glPopMatrix();
}

// Fungsi Menggambar Dinding dengan Tekstur
void drawWalls(GLuint textureID) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLfloat mat_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient);

    for (const auto& w : walls) {
        glBegin(GL_QUADS);
        // Depan
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(w.xMin, 0.0f, w.zMax);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(w.xMax, 0.0f, w.zMax);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(w.xMax, w.height, w.zMax);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(w.xMin, w.height, w.zMax);
        // Belakang
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(w.xMin, 0.0f, w.zMin);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(w.xMin, w.height, w.zMin);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(w.xMax, w.height, w.zMin);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(w.xMax, 0.0f, w.zMin);
        // Atas
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(w.xMin, w.height, w.zMin);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(w.xMin, w.height, w.zMax);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(w.xMax, w.height, w.zMax);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(w.xMax, w.height, w.zMin);
        // Kiri
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(w.xMin, 0.0f, w.zMin);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(w.xMin, 0.0f, w.zMax);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(w.xMin, w.height, w.zMax);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(w.xMin, w.height, w.zMin);
        // Kanan
        glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(w.xMax, 0.0f, w.zMin);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(w.xMax, w.height, w.zMin);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(w.xMax, w.height, w.zMax);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(w.xMax, 0.0f, w.zMax);
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}

// Fungsi Menggambar Bola
void drawBall(const Ball& b) {
    GLfloat ball_mat[] = { 0.2f, 0.8f, 1.0f, 1.0f }; 
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ball_mat);
    
    glPushMatrix();
    glTranslatef(b.x, b.y, b.z);
    glutSolidSphere(b.radius, 20, 20);
    glPopMatrix();
}