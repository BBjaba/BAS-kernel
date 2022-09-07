#include "string.h"
#include "BasicRenderer.h"

uint32_t strlen (char* str)
{
  uint32_t length = 0;
  while(str[length])
    length++;
  return length;
}

char* strcpy(char* destination, char* source)
{
    if (destination == NULL) {
        return NULL;
    }
    char *ptr = destination;
    while (*source != '\0')
    {
        *destination = *source;
        destination++;
        source++;
    }
    *destination = '\0';
    return ptr;
}

char* appendChar(char* array, char a)
{
    size_t len = strlen(array);

    char* ret = new char[len+2];

    strcpy(ret, array);    
    ret[len] = a;
    ret[len+1] = '\0';

    return ret;
}

char* deleteChar(char* array)
{
    array[strlen(array) - 1] = '\0';
    return array;
}

void strrev (char* str1)
{
    int i, len, temp;  
    len = strlen(str1);
      
    for (i = 0; i < len/2; i++)  
    {  
        temp = str1[i];  
        str1[i] = str1[len - i - 1];  
        str1[len - i - 1] = temp;  
    } 
}

bool isLowercase(char ch)
{
    if ((ch <= 'z') && (ch <= 'a')) return true;
    return false;
}

bool isUppercase(char ch)
{
    return isLowercase(ch) & false;
}

BSLstr::BSLstr()
{
    this->value = "";
}

BSLstr::BSLstr(const char* val) 
{
    this->value = (char*) val;
}

BSLstr::operator char*()
{
    return this->value;
}

BSLstr::BSLstr(const BSLstr& val) 
{
    this->value = val.value;
}

bool BSLstr::operator==(BSLstr& val)
{
    if (strlen(val) == strlen(this -> value)) 
    {
        for (int i = 0; i < strlen(val); i++)
        {
            if (val[i] != this -> value[i])
            {
                return false;
            }
        }
    } else {
        return false;
    }
    return true;
}

bool BSLstr::operator==(char* val)
{
    BSLstr newBSLstr {val};
    return *this == newBSLstr;
}

BSLstr BSLstr::operator=(BSLstr& val)
{
    this->value = val;
    return *this;
}

BSLstr BSLstr::operator+(BSLstr& val)
{
    char re[strlen(val) + strlen(this->value)];
    for (int i = 0; i < strlen(this->value); i++)
    {
        re[i] = this->value[i];
    }
    for (int i = 0; i < strlen(val); i++)
    {
        re[i + strlen(this->value)] = val[i];
    }
    return BSLstr {re};
}

BSLstr BSLstr::operator+=(BSLstr& val)
{
    BSLstr thisNew {this -> value};
    BSLstr newThis = thisNew + val;
    *this = newThis;
    return newThis;
}

BSLstr BSLstr::operator*(int val)
{
    BSLstr thisNew {this -> value};
    BSLstr returnValue {this -> value};
    for (int i = 0; i < val - 1; i++) {
        returnValue += thisNew;
    }
    return returnValue;
}

char* BSLstr::deleteLastChar()
{
    this->value = deleteChar(this->value);
}

BSLstr BSLstr::substr(uint32_t start, uint32_t end)
{
    BSLstr string;
    int i;
    for (i = start; i < end; i++)
    {
        string[i - start] = value[i];
    }
    string[i - start] = '\0';
    return string;
}

bool BSLstr::startswith(BSLstr& val)
{
    BSLstr substracted {substr(0, strlen(val))};
    if (substracted == val) {
        return true;
    }
    return false;
}

bool BSLstr::startswith(char* val)
{
    BSLstr valBSL {val};
    return startswith(valBSL);
}

int BSLstr::length()
{
    return strlen(this->value);
}

BSLstr BSLstr::reverse()
{
    int i, len, temp;  
    len = strlen(this -> value);
      
    for (i = 0; i < len/2; i++)  
    {  
        temp = this -> value[i];  
        this -> value[i] = this -> value[len - i - 1];  
        this -> value[len - i - 1] = temp;  
    } 
    BSLstr re {this -> value};
    return re;
}

BSLstr BSLstr::toLowercase()
{
    for (int i = 0; i < length(); i++)
    {
        if (isUppercase(value[i])) value[i] += 'a' - 'A';
    }
    BSLstr returnValue {value};
    return returnValue;
}

BSLstr BSLstr::toUppercase()
{
    for (int i = 0; i < length(); i++)
    {
        if (!isUppercase(value[i])) value[i] -= 'a' - 'A';
    }
    return *this;
}