#pragma once
#include <stdint.h>
#include "kbScancodeTranslation.h"
#include "../BasicRenderer.h"

void HandleKeyboard(uint8_t scancode);
char OutputResult(Point minumumIndex, uint32_t color);