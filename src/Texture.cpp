#include "Texture.h"
#include <stdio.h>
#include <iostream>
#include <gl/glew.h>
#include <gl/glut.h>

unsigned char *Texture::LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
    FILE *filePtr;
    BITMAPFILEHEADER bitmapFileHeader;
    unsigned char *bitmapImage;
    int imageIdx = 0;
    unsigned char tempRGB;

    fopen_s(&filePtr, filename, "rb");
    if (filePtr == NULL) {
        std::cerr << "Error in LoadBitmapFile: could not open file " << filename << std::endl;
        return NULL;
    }
    fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
    if (bitmapFileHeader.bfType != BITMAP_ID) {
        std::cerr << "Error in LoadBitmapFile: the file is not a bitmap file" << std::endl;
        fclose(filePtr);
        return NULL;
    }

    fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
    bitmapImage = new unsigned char[bitmapInfoHeader->biSizeImage];
    if (!bitmapImage) {
        std::cerr << "Error in LoadBitmapFile: memory error" << std::endl;
        fclose(filePtr);
        return NULL;
    }

    fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);
    if (bitmapImage == NULL) {
        std::cerr << "Error in LoadBitmapFile: memory error" << std::endl;
        delete[] bitmapImage;
        fclose(filePtr);
        return NULL;
    }

    for (size_t imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3) {
        tempRGB = bitmapImage[imageIdx];
        bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
        bitmapImage[imageIdx + 2] = tempRGB;
    }
    fclose(filePtr);
    return bitmapImage;
}

void Texture::texload(unsigned int textureID, char *filename)
{
    BITMAPINFOHEADER bitmapInfoHeader;
    unsigned char*   bitmapData;

    bitmapData = LoadBitmapFile(filename, &bitmapInfoHeader);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGB,
        bitmapInfoHeader.biWidth,
        bitmapInfoHeader.biHeight,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        bitmapData);
}