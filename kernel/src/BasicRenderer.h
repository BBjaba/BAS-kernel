#pragma once
#include "math.h"
#include "Framebuffer.h"
#include "simpleFonts.h" 
#include <stdint.h>
#include "cstr.h"

#define Black   0x000000
#define White 	0xFFFFFF
#define Red 	0xFF0000
#define Green	0x00FF00
#define Blue  	0x0000FF
#define Yellow 	0xFFFF00
#define Cyan	0x00FFFF
#define Magenta 0xFF00FF

class BasicRenderer{
    public:
        BasicRenderer(Framebuffer* targetFramebuffer, PSF1_FONT* psf1_Font);
        Point CursorPosition;
        Framebuffer* TargetFramebuffer;
        PSF1_FONT* PSF1_Font;
        unsigned int* pixPtr = (unsigned int*)TargetFramebuffer->BaseAddress;
        unsigned int Colour;
        unsigned int ClearColour;
        uint32_t mouseColor = White;
        Point lastCursorPosition;
        uint32_t lastCursorBackground[16 * 16];
        void PutPixel(uint32_t x, uint32_t y, uint32_t color);
        uint32_t GetPixel(uint32_t x, uint32_t y);
        void Print(const char* str);
        void PutChar(char chr, unsigned int xOff, unsigned int yOff);
        void PutChar(char chr);
        void ClearChar();
        void Clear();
        void Next();
        void Next(uint64_t times);
        void DrawCursor(uint8_t* cursorBitmap, Point position);
        void DelteCursor(uint8_t* cursorBitmap);
        void Println(const char* str);
        void PrintColoring(const char* str, uint32_t color);
        void showPointer();
        void hidePointer();
        bool firstCursor = true;
};

extern BasicRenderer* GlobalRenderer;