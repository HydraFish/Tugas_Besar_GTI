#ifndef KAMERA_H
#define KAMERA_H

#include <cmath>       
#include <GL/glut.h>   

// Deklarasi Variabel Global Kamera
extern float angle;
extern float deltaAngle;
extern float camX, camY, camZ;
extern float lx, ly, lz;
extern int deltaMove;

// Deklarasi Prosedur Fungsi Pergerakan Kamera
void orientMe(float ang);
void moveMeFlat(int i);
void pressKey(int key, int x, int y);
void releaseKey(int key, int x, int y);

#endif