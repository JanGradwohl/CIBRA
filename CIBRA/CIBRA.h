#include "stdio.h"
#include "stdlib.h"

#ifndef CIBRA_H
#define CIBRA_H

void SendKeys(const char* keys);
void SetMousePosition(int x, int y);
char* os(const char* command);
void Erase(const char* filePath, const char* algorithm);
char* startKeylogger();

#endif
