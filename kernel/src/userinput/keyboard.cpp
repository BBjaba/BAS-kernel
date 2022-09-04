#include "keyboard.h"
#include "../../../desktop/handlers/keyboard.h"

    bool isLeftShiftPressed;
    bool isRightShiftPressed;
    
char HandleScancode(uint8_t scancode)
{
    char keyPressed;
    switch (scancode){
        case LeftShift:
            isLeftShiftPressed = true;
            return 0;
        case LeftShift + 0x80:
            isLeftShiftPressed = false;
            return 0;
        case RightShift:
            isRightShiftPressed = true;
            return 0;
        case RightShift + 0x80:
            isRightShiftPressed = false;
            return 0;
        case Enter:
            keyPressed = '\n';
            return '\n';
        case Spacebar:
            keyPressed = ' ';
            return ' ';
        case BackSpace:
            keyPressed = '\b';
            return '\b';
    }
    keyPressed = QWERTYKeyboard::Translate(scancode, isLeftShiftPressed | isRightShiftPressed);
    return keyPressed;
}

void HandleKeyboard(uint8_t scancode)
{
    KeyboardEvent(HandleScancode(scancode));
}