#include "keyboard.h"
#include "kernelUtil.h"
#include "userinput/mouse.h"

char* inputedString;
uint8_t i = 0;

char* currentPath = "/";

AHCI::device disk;

void setDisk(AHCI::device diskPass)
{
    disk = diskPass;
}

void shellInitialize()
{
    GlobalRenderer->Next();
    GlobalRenderer->Colour = Green;
    GlobalRenderer->Print(currentPath); GlobalRenderer->Print(" >> $ ");
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

    if (finishedString == "");

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
        
        GlobalRenderer->Print("5. mcolor <color>");
        GlobalRenderer->PrintColoring("         Changes the mouse color", Cyan);
        GlobalRenderer->Next();

        GlobalRenderer->Print("6. ls"); 
        GlobalRenderer->PrintColoring("                     Lists all files and directories in current path", Cyan);
        GlobalRenderer->Next();
        
        GlobalRenderer->Print("7. cd <directory>"); 
        GlobalRenderer->PrintColoring("         Changes the current path to <directory>. <directory> must be absolute.", Cyan);
        GlobalRenderer->Next();

        GlobalRenderer->Print("8. read <file>"); 
        GlobalRenderer->PrintColoring("            Prints the content of <file> as characters. <file> must be relative.", Cyan);
        GlobalRenderer->Next();

        GlobalRenderer->Next();
    } else if (finishedString.startswith("echo "))
    {
        if (strlen(finishedString) > 5)
        {
            GlobalRenderer->Println(finishedString.substr(5, finishedString.length()));
        }
    } else if (finishedString == "clear")
    {
        GlobalRenderer->Clear();
        GlobalRenderer->CursorPosition = {0, 0};
    } else if (finishedString == "shutdown")
    {
        shutdown();    
    } else if (finishedString.startswith("mcolor "))
    {
        if (strlen(finishedString) > 7)
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
            GlobalRenderer->PrintColoring("Unknown color!", Red);
            GlobalRenderer->Next();
        }
    } else if (finishedString == "ls")
    {
        FAT_File* fd = FAT_Open(disk, currentPath);
        FAT_DirectoryEntry entry;
        int i = 0;
        while (FAT_ReadEntry(disk, fd, &entry) && i++ < 14)
        {
            bool good = false;
            uint32_t pastColor = GlobalRenderer->Colour;
            if (FAT_isDirectory(entry)) GlobalRenderer->Colour = Cyan;
            for (int i = 0; i < 11; i++) if (entry.Name[i] != 0x00) good = true;
            if (good)
            {
                GlobalRenderer->Print("  ");
                for (int i = 0; i < 11; i++)
                    GlobalRenderer->PutChar(entry.Name[i]);
                GlobalRenderer->Next();
            }
            GlobalRenderer->Colour = pastColor;
        }
        FAT_Close(fd);

    } else if (finishedString.startswith("cd "))
    {
        char* result;
        if (strlen(finishedString) == 3) result = "/";
        else result = substr(finishedString, 3, finishedString.length());
        if (result[strlen(result) - 1] != '/') result = appendChar(result, '/');
        char* path = new char[strlen(result)];
        strcpy(path, result);
        FAT_File* fd = FAT_Open(disk, result);
        if (fd != NULL && fd->IsDirectory) currentPath = path;
        else {GlobalRenderer->PrintColoring("Failed changing directory", Red); GlobalRenderer->Next(); FAT_Initialize(disk);}
        FAT_Close(fd);
        
    } else if (finishedString.startswith("read "))
    {
        if (strlen(finishedString) > 5)
        {
            char* file = finishedString.substr(5, finishedString.length());

            char buffer[100];
            char* path = new char[strlen(currentPath)];
            strcpy(path, currentPath);
            for (int i = 0; i < strlen(file); i++) path = appendChar(path, file[i]);
            path[strlen(currentPath) + strlen(file)] = '\0';
            uint32_t read;
            FAT_File* fd = FAT_Open(disk, path);
            if (fd != NULL && !fd->IsDirectory)
            {
                while ((read = FAT_Read(disk, fd, sizeof(buffer), buffer)))
                {
                    for (uint32_t i = 0; i < read; i++)
                    {
                        if (buffer[i] == '\n') GlobalRenderer->Next(); 
                        else GlobalRenderer->PutChar(buffer[i]);
                    }
                }
            } else {
                GlobalRenderer->PrintColoring("Failed reading from file", Red);
            }
            FAT_Close(fd);
            GlobalRenderer->Next();
        }
    }
    else {
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

