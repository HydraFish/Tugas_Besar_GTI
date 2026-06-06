#ifndef BMPLOADER_H
#define BMPLOADER_H

#include <iostream>
#include <cstdio>
#include <cstdint>
#include <GL/glut.h>

#pragma pack(push, 1)
struct BMPHeader {
    uint16_t type; 
    uint32_t size; 
    uint16_t reserved1, reserved2; 
    uint32_t offset;
    uint32_t dib_header_size; 
    int32_t width, height; 
    uint16_t planes; 
    uint16_t bpp;
    uint32_t compression; 
    uint32_t image_size; 
    int32_t x_res, y_res;
    uint32_t colors, important_colors;
};
#pragma pack(pop)

// Fungsi Membaca file gambar BMP
inline GLuint loadBMP(const char* filepath) {
    FILE* f = fopen(filepath, "rb");
    if (!f) {
        std::cerr << "Warning: Gambar " << filepath << " tidak ditemukan!\n";
        return 0;
    }
    
    BMPHeader header;
    fread(&header, sizeof(BMPHeader), 1, f);
    if (header.type != 0x4D42) { 
        fclose(f); 
        return 0; 
    }
    
    fseek(f, header.offset, SEEK_SET);
    int imgSize = header.image_size ? header.image_size : header.width * header.height * 3;
    unsigned char* data = new unsigned char[imgSize];
    fread(data, 1, imgSize, f);
    fclose(f);
    
    // Konversi format BGR ke RGB
    for (int i = 0; i < imgSize; i += 3) {
        unsigned char temp = data[i];
        data[i] = data[i + 2];
        data[i + 2] = temp;
    }
    
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, header.width, header.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    
    delete[] data;
    return tex;
}

#endif