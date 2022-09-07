#include "mouse.h"

uint8_t CursorBitmap[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 
    1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 
    1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 
    1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 
    1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 
    1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 
};

void MouseWait()
{
    uint64_t timeout = 100000;
    while (timeout--)
    {
        if ((getBit(inb(0x64), 2)) == 0)
        {
            return;
        }
    } 
}

void MouseWaiInput()
{
    uint64_t timeout = 100000;
    while (timeout--)
    {
        if (getBit(inb(0x64), 1))
        {
            return;
        }
    }
}

void MouseWrite(uint8_t value)
{
    MouseWait();
    outb(0x64, 0xD4);
    MouseWait();
    outb(0x60, value);
}

uint8_t MouseRead()
{
    MouseWaiInput();
    return inb(0x60);
}

void PS2MouseInitialization()
{
    outb(0x64, 0xA8);
    MouseWait();
    outb(0x64, 0x20);
    MouseWaiInput();
    uint8_t mouseData = inb(0x60);
    mouseData |= 0b10;
    MouseWait();
    outb(0x64, 0x60);
    MouseWait();
    outb(0x64, mouseData);

    MouseWrite(0xF6);
    MouseRead();
    MouseWrite(0xF4);
    MouseRead();
}

uint8_t MouseDataBuffer[3];
bool MouseBufferReady = false;
uint64_t mouseDataTime = 0;
void HandleMouseData(uint8_t data)
{
    MouseDataAction();
    static bool skip = true;
    if (skip) {skip = false; return;}

    switch (mouseDataTime)
    {
        case 0:
            if ((data & 0b00001000) == 0) break;
            MouseDataBuffer[0] = data;
            MouseBufferReady = false;
            mouseDataTime++;
            break;
        case 1:
            MouseDataBuffer[1] = data;
            mouseDataTime++;
            break;
        case 2:
            MouseDataBuffer[2] = data;
            mouseDataTime = 0;
            MouseBufferReady = true;
            break;
    }
}

Point MousePointerPosition;

void MouseDataAction()
{
    if (!MouseBufferReady) return;

    bool X_Negative, Y_Negative, X_Overflow, Y_Overflow, MiddleButton, RightButton, LeftButton;

    if (MouseDataBuffer[0] & PS2XSign){
        X_Negative = true;
    } else X_Negative = false;

    if (MouseDataBuffer[0] & PS2YSign){
        Y_Negative = true;
    } else Y_Negative = false;

    if (MouseDataBuffer[0] & PS2XOverflow){
        X_Overflow = true;
    } else X_Overflow = false;

    if (MouseDataBuffer[0] & PS2YOverflow){
        Y_Overflow = true;
    } else Y_Overflow = false;

    if (!X_Negative)
    {
        MousePointerPosition.X += MouseDataBuffer[1];
        if (X_Overflow) MousePointerPosition.X += 255;
    } else
    {
        MouseDataBuffer[1] = 256 - MouseDataBuffer[1];
        MousePointerPosition.X -= MouseDataBuffer[1];
        if (X_Overflow) MousePointerPosition.X -= 255;
    } 

    if (!Y_Negative)
    {
        MousePointerPosition.Y -= MouseDataBuffer[2];
        if (Y_Overflow) MousePointerPosition.Y -= 255;
    } else
    {
        MouseDataBuffer[2] = 256 - MouseDataBuffer[2];
        MousePointerPosition.Y += MouseDataBuffer[2];
        if (Y_Overflow) MousePointerPosition.Y += 255;
    }
    if (MousePointerPosition.X < 0) MousePointerPosition.X = 0;
    if (MousePointerPosition.X > GlobalRenderer->TargetFramebuffer->Width - 8) MousePointerPosition.X = GlobalRenderer->TargetFramebuffer->Width - 8;
    
    if (MousePointerPosition.Y < 0) MousePointerPosition.Y = 0;
    if (MousePointerPosition.Y > GlobalRenderer->TargetFramebuffer->Height - 16) MousePointerPosition.Y = GlobalRenderer->TargetFramebuffer->Height - 16;

    if (MouseDataBuffer[0] & PS2Leftbutton)
    {
        LeftButton = true;
        //Left button pressed
    }
    if (MouseDataBuffer[0] & PS2Middlebutton)
    {
        MiddleButton = true;
        //Middle button pressed
    }
    if (MouseDataBuffer[0] & PS2Rightbutton)
    {
        RightButton = true;
        //Right button pressed
    }

    GlobalRenderer->DelteCursor(CursorBitmap);
    GlobalRenderer->DrawCursor(CursorBitmap, MousePointerPosition);
    
    MouseBufferReady = false;
    mosueEvent(MousePointerPosition, MiddleButton, LeftButton, RightButton);
}