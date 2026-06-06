#include "Kamera.h"

// Inisialisasi awal koordinat kamera di belakang bidang permainan
float angle = 0.0f;
float deltaAngle = 0.0f;
float camX = 10.0f, camY = 12.0f, camZ = 45.0f; // Posisi awal mata melihat
float lx = 0.0f, ly = -0.25f, lz = -1.0f;       // Arah hadap sudut pandang (sedikit menunduk ke bawah)
int deltaMove = 0;

// Fungsi memutar arah sudut pandang kamera
void orientMe(float ang) {
    lx = sin(ang);
    lz = -cos(ang);
}

// Fungsi memindahkan posisi kamera maju / mundur secara mendatar
void moveMeFlat(int i) {
    camX = camX + i * lx * 0.4f; // Kecepatan gerak kamera
    camZ = camZ + i * lz * 0.4f;
}

// Callback saat tombol panah pada keyboard ditekan dan ditahan 
void pressKey(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:  deltaAngle = -0.03f; break; // Rotasi kiri
        case GLUT_KEY_RIGHT: deltaAngle = 0.03f;  break; // Rotasi kanan
        case GLUT_KEY_UP:    deltaMove = 1;       break; // Maju
        case GLUT_KEY_DOWN:  deltaMove = -1;      break; // Mundur
    }
}

// Callback saat tekanan tombol panah pada keyboard dilepas 
void releaseKey(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:  if (deltaAngle < 0.0f) deltaAngle = 0.0f; break;
        case GLUT_KEY_RIGHT: if (deltaAngle > 0.0f) deltaAngle = 0.0f; break;
        case GLUT_KEY_UP:    if (deltaMove > 0) deltaMove = 0;         break;
        case GLUT_KEY_DOWN:  if (deltaMove < 0) deltaMove = 0;         break;
    }
}