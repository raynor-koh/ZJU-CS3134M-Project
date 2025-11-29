#pragma once
#include <windows.h>
#define BITMAP_ID 0x4D42
#define imageweight 128
#define imageheight 128

class Texture {
public:
    unsigned int ID;
    Texture(char *filename) { texload(ID, filename);}
    void rebind(char *filename) { texload(ID, filename); }
private:
    unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader);
    void texload(unsigned int textureID, char *filename);
};