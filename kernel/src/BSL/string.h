#pragma once

#include <stdint.h>
#include "Heap.h"
#include "dynamics.h"

#define nullptr 0

class BSLstr {
    char* value;

    public:
        BSLstr();
        BSLstr(const char* val);
        BSLstr(const BSLstr& val);
        operator char*();
        BSLstr operator=(BSLstr& val);
        BSLstr operator+(BSLstr& val);
        bool operator==(BSLstr& val);
        bool operator==(char* val);
        BSLstr operator*(int val);
        BSLstr operator+=(BSLstr& val);
        BSLstr substr(uint32_t start, uint32_t end);
        bool startswith(BSLstr& val);
        bool startswith(char* val);
        char* deleteLastChar();
        BSLstr toUppercase();
        BSLstr toLowercase();
        int length();
        BSLstr reverse();
};
void strrev(char *str1);
char* appendChar(char* array, char a);
char* strcpy(char* destination, char* source);
uint32_t strlen (const char* str);
char* deleteChar(char* array);
bool isUppercase(char ch);
bool isLowercase(char ch);