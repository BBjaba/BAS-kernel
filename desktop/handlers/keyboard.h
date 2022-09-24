#pragma once
#include <stdint.h>
#include "BasicRenderer.h"
#include "userinput/keyboard.h"
#include "BSL/string.h"
#include "memory.h"
#include "filesystem/fat.h"

void KeyboardEvent (char scancode);
void shellInitialize();
void setDisk(AHCI::device diskPass);
