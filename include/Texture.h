#pragma once
#include <windows.h>
#include <string>
#define BITMAP_ID 0x4D42
#define imageweight 128
#define imageheight 128

class Texture {
public:
    Texture(char *filename) { texload(ID, filename);}
    void rebind(char *filename) { texload(ID, filename); }
    unsigned int getID() { return ID; }
    std::string getName() { return name; }
private:
    unsigned int ID;
    std::string name;
    unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader);
    void texload(unsigned int textureID, char *filename);
};