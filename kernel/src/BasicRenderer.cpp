#include "BasicRenderer.h"

BasicRenderer* GlobalRenderer;
uint32_t mouseColor = White;

char pointerBackground [2][16];
Point pastPoint;

BasicRenderer::BasicRenderer(Framebuffer* targetFramebuffer, PSF1_FONT* psf1_Font)
{
    TargetFramebuffer = targetFramebuffer;
    PSF1_Font = psf1_Font;
    Colour = 0xffffffff;
    CursorPosition = {0, 0};
}

void BasicRenderer::PutPixel(uint32_t x, uint32_t y, uint32_t colour){
    *(uint32_t*)((uint64_t)TargetFramebuffer->BaseAddress + (x*4) + (y * TargetFramebuffer->PixelsPerScanLine * 4)) = colour;
}

uint32_t BasicRenderer::GetPixel(uint32_t x, uint32_t y){
    return *(uint32_t*)((uint64_t)TargetFramebuffer->BaseAddress + (x*4) + (y * TargetFramebuffer->PixelsPerScanLine * 4));
}

void BasicRenderer::Clear(){
    uint64_t fbBase = (uint64_t)TargetFramebuffer->BaseAddress;
    uint64_t bytesPerScanline = TargetFramebuffer->PixelsPerScanLine * 4;
    uint64_t fbHeight = TargetFramebuffer->Height;
    uint64_t fbSize = TargetFramebuffer->BufferSize;

    for (int verticalScanline = 0; verticalScanline < fbHeight; verticalScanline ++)
    {
        uint64_t pixPtrBase = fbBase + (bytesPerScanline * verticalScanline);
        for (uint32_t* pixPtr = (uint32_t*)pixPtrBase; pixPtr < (uint32_t*)(pixPtrBase + bytesPerScanline); pixPtr ++)
        {
            *pixPtr = ClearColour;
        }
    }
}

void BasicRenderer::ClearChar()
{
    showPointer();
    hidePointer();
    if (CursorPosition.X == 0){
        CursorPosition.X = TargetFramebuffer->Width;
        CursorPosition.Y -= 16;
        if (CursorPosition.Y < 0) CursorPosition.Y = 0;
    }

    unsigned int xOff = CursorPosition.X;
    unsigned int yOff = CursorPosition.Y;

    unsigned int* pixPtr = (unsigned int*)TargetFramebuffer->BaseAddress;
    for (unsigned long y = yOff; y < yOff + 16; y++)
    {
        for (unsigned long x = xOff - 8; x < xOff; x++)
        {
                    *(unsigned int*)(pixPtr + x + (y * TargetFramebuffer->PixelsPerScanLine)) = ClearColour;
        }
    }

    CursorPosition.X -= 8;

    if (CursorPosition.X < 0)
    {
        CursorPosition.X = TargetFramebuffer->Width;
        CursorPosition.Y -= 16;
        if (CursorPosition.Y < 0) CursorPosition.Y = 0;
    }
    showPointer();

}

void BasicRenderer::Next(){
    showPointer();
    hidePointer();
    CursorPosition.X = 0;
    CursorPosition.Y += 16;
    showPointer();
    hidePointer();
}

void BasicRenderer::Next(uint64_t times){
    for (int i = 0; i < times; i++)
    {
        Next();
    }
}

void BasicRenderer::Print(const char* str)
{
    
    char* chr = (char*)str;
    while(*chr != 0)
    {
        PutChar(*chr, CursorPosition.X, CursorPosition.Y);
        CursorPosition.X+=8;
        if(CursorPosition.X + 8 > TargetFramebuffer->Width)
        {
            CursorPosition.X = 0;
            CursorPosition.Y += 16;
        }
        chr++;
    }
}

void BasicRenderer::PutChar(char chr, unsigned int xOff, unsigned int yOff)
{
    unsigned int* pixPtr = (unsigned int*)TargetFramebuffer->BaseAddress;
    char* fontPtr = (char*)PSF1_Font->glyphBuffer + (chr * PSF1_Font->psf1_Header->charsize);
    for (unsigned long y = yOff; y < yOff + 16; y++)
    {
        for (unsigned long x = xOff; x < xOff+8; x++)
        {
            if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0)
            {
                *(unsigned int*)(pixPtr + x + (y * TargetFramebuffer->PixelsPerScanLine)) = Colour;
            }

        }
        fontPtr++;
    }
}

void BasicRenderer::PutChar(char chr)
{
    showPointer();
    hidePointer();
    PutChar(chr, CursorPosition.X, CursorPosition.Y);
    CursorPosition.X += 8;
    if (CursorPosition.X + 8 > TargetFramebuffer->Width){
        CursorPosition.X = 0; 
        CursorPosition.Y += 16;
    }
    showPointer();
    hidePointer();
}

void BasicRenderer::DrawCursor(uint8_t* cursorBitmap, Point position)
{
    lastCursorPosition = position;

    int xMax = 16;
    int yMax = 16;
    int differenceX = TargetFramebuffer->Width - position.X;
    int differenceY = TargetFramebuffer->Height - position.Y;

    if (differenceX < 16) xMax = differenceX;
    if (differenceY < 16) yMax = differenceY;

    for (int y = 0; y < yMax; y++){
        for (int x = 0; x < xMax; x++){
            int bit = y * 16 + x;
            if (cursorBitmap[bit] == 1)
            {
                lastCursorBackground[x + y * 16] = GetPixel(position.X + x, position.Y + y);
                PutPixel(position.X + x, position.Y + y, mouseColor);

            }
        }
    }

    firstCursor = false;
}

void BasicRenderer::DelteCursor(uint8_t* cursorBitmap)
{
    if (firstCursor) return;
    
    int xMax = 16;
    int yMax = 16;
    int differenceX = TargetFramebuffer->Width - lastCursorPosition.X;
    int differenceY = TargetFramebuffer->Height - lastCursorPosition.Y;

    if (differenceX < 16) xMax = differenceX;
    if (differenceY < 16) yMax = differenceY;

    for (int y = 0; y < yMax; y++){
        for (int x = 0; x < xMax; x++){
            int bit = y * 16 + x;
            if (cursorBitmap[bit] == 1)
            {
                PutPixel(lastCursorPosition.X + x, lastCursorPosition.Y + y, lastCursorBackground[x + y * 16]);

            }
        }
    }
}

void BasicRenderer::Println(const char* str)
{
    GlobalRenderer->Print(str);
    GlobalRenderer->Next();
}

void BasicRenderer::PrintColoring(const char* str, uint32_t color)
{
    uint32_t pastColor = Colour;
    Colour = color;
    Print(str);
    Colour = pastColor;
}

void BasicRenderer::showPointer()
{
    for (int i = CursorPosition.X + 0; i < CursorPosition.X + 2; i++)
    {
        for (int j = CursorPosition.Y; j < CursorPosition.Y + 16; j++)
        {
            pastPoint = CursorPosition;
            pointerBackground[i - CursorPosition.X - 5][j - CursorPosition.Y] = GetPixel(i, j);
            PutPixel(i, j, Colour);
        }
    }
}

void BasicRenderer::hidePointer()
{
    for (int i = CursorPosition.X + 0; i < CursorPosition.X + 2; i++)
    {
        for (int j = CursorPosition.Y; j < CursorPosition.Y + 16; j++)
        {
            PutPixel(i, j, pointerBackground[i - CursorPosition.X - 5][j - CursorPosition.Y]);
        }
    }
}