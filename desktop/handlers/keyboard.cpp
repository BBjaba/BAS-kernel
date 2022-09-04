#include "keyboard.h"
#include "kernelUtil.h"
#include "userinput/mouse.h"

char* inputedString;
uint8_t i = 0;

void shellInitialize()
{
    GlobalRenderer->Next();
    GlobalRenderer->Colour = Green;
    GlobalRenderer->Print("Shell >> $ ");
    GlobalRenderer->Colour = Yellow;
    GlobalRenderer->showPointer();
    i = 0;
    inputedString[0] = '\0';
}

void shellFinishedString(char* string)
{
    GlobalRenderer->Next(2);
    uint32_t pastColor = GlobalRenderer->Colour;
    GlobalRenderer->Colour = White;
    
    BSLstr stringPrepare {string};
    BSLstr finishedString {stringPrepare.toLowercase()};

    for (int i = finishedString.length() - 1; i >= 0; i--)
    {
        if (finishedString[i] == ' ') finishedString[i] = '\0';
        else break;
    }

    if (finishedString == "help")
    { 
        GlobalRenderer->Print("1. echo <message>");
        GlobalRenderer->PrintColoring("         Prints sth on screen", Cyan);
        GlobalRenderer->Next();

        GlobalRenderer->Print("2. help");
        GlobalRenderer->PrintColoring("                   Prints helpful commands", Cyan);
        GlobalRenderer->Next();
        
        GlobalRenderer->Print("3. clear");
        GlobalRenderer->PrintColoring("                  Cleans the screen", Cyan);
        GlobalRenderer->Next();
        
        GlobalRenderer->Print("4. shutdown");
        GlobalRenderer->PrintColoring("               Powers the computer off", Cyan);
        GlobalRenderer->Next();
        
        /*GlobalRenderer->Print("5. reboot");
        GlobalRenderer->PrintColoring("                 Restarts the computer", Cyan);
        GlobalRenderer->Next();*/
        
        GlobalRenderer->Print("5. mcolor <color>");
        GlobalRenderer->PrintColoring("         Changes the mouse color", Cyan);
        GlobalRenderer->Next();

        GlobalRenderer->Next();
    } else if (finishedString.startswith("echo"))
    {
        GlobalRenderer->Println(finishedString.substr(5, finishedString.length()));
    } else if (finishedString == "clear")
    {
        GlobalRenderer->Clear();
        GlobalRenderer->CursorPosition = {0, 0};
    } else if (finishedString == "shutdown")
    {
        shutdown();    
    } else if (finishedString.startswith("mcolor"))
    {
        BSLstr substracted {finishedString.substr(7, finishedString.length())};
        if (substracted == "yellow")
        {
            GlobalRenderer->mouseColor = Yellow;
            GlobalRenderer->Println("Mouse color changed to yellow");
        } else if (substracted == "blue")
        {
            GlobalRenderer->mouseColor = Blue;
            GlobalRenderer->Println("Mouse color changed to yellow");
        } else if (substracted == "black")
        {
            GlobalRenderer->mouseColor = Black;
            GlobalRenderer->Println("Mouse color changed to black");
        } else if (substracted == "white")
        {
            GlobalRenderer->mouseColor = White;
            GlobalRenderer->Println("Mouse color changed to white");
        } else if (substracted == "red")
        {
            GlobalRenderer->mouseColor = Red;
            GlobalRenderer->Println("Mouse color changed to red");
        } else if (substracted == "green")
        {
            GlobalRenderer->mouseColor = Green;
            GlobalRenderer->Println("Mouse color changed to green");
        } else if (substracted == "cyan")
        {
            GlobalRenderer->mouseColor = Cyan;
            GlobalRenderer->Println("Mouse color changed to cyan");
        } else if (substracted == "magenta")
        {
            GlobalRenderer->mouseColor = Magenta;
            GlobalRenderer->Println("Mouse color changed to magenta");
        } else {
            GlobalRenderer->PrintColoring("Unknown color!", Red);
            GlobalRenderer->Next();
        }

        GlobalRenderer->DelteCursor(CursorBitmap);
        GlobalRenderer->DrawCursor(CursorBitmap, MousePointerPosition);
    } else {
        GlobalRenderer->PrintColoring("Unknown command!", Red);
        GlobalRenderer->Next();
    }

    GlobalRenderer->Colour = pastColor;
    shellInitialize();
}

void KeyboardEvent (char keyPressed)
{
    if(keyPressed != '\n') 
    {
        if(keyPressed == '\b') 
        {
            if (i > 0) 
            {
                GlobalRenderer->ClearChar();
                i--;
            }
        } else {
            if (keyPressed != 0)
            {
                GlobalRenderer->PutChar(keyPressed);
                inputedString[i] = keyPressed;
                i++;
            }
        }
    } else {
        inputedString[i] = '\0';
        shellFinishedString(inputedString);
    }
}

