#pragma once
#include "../IO.h"
#include "../math.h"
#include "../BasicRenderer.h"
#include "../../../desktop/handlers/mouse.h"

extern uint8_t CursorBitmap[];

#define PS2Leftbutton 0b00000001
#define PS2Middlebutton 0b00000100
#define PS2Rightbutton 0b00000010
#define PS2XSign 0b00010000
#define PS2YSign 0b00100000
#define PS2XOverflow 0b01000000
#define PS2YOverflow 0b10000000

void PS2MouseInitialization();
void HandleMouseData(uint8_t data);
void MouseDataAction();

extern Point MousePointerPosition;