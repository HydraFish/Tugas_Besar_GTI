#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <GL/glut.h>

struct Point3D { float x, y, z; };

struct Wall { float xMin, xMax, zMin, zMax, height; };

// Struktur Area Oli (Memperlambat bola)
struct Oil { float xMin, xMax, zMin, zMax; };

// Struktur Kayu Bergerak (Menendang bola)
struct Wood { 
    float x, z;
    float width, depth, height;
    float vx; // Kecepatan gerak kayu
    float limitMin, limitMax; // Batas pergerakan kanan-kiri
};

struct Ball {
    float x, y, z;
    float vx, vz; 
    float radius;
    bool isFalling;
};

// Deklarasi Variabel Global
extern std::vector<Wall> walls;
extern std::vector<Oil> oils;
extern std::vector<Wood> woods;
extern std::vector<Point3D> initialKeys;
extern std::vector<Point3D> keys;
extern Ball ball;
extern const Ball initialBall;

// Deklarasi Fungsi Penggambaran
void drawKey();
void drawWalls(GLuint textureID);
void drawOils(GLuint textureID);
void drawWoods(GLuint textureID);
void drawBall(const Ball& b);

#endif