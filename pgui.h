#include <stdio.h>
#include <stdlib.h>
#define SWidth 79
#define SHeight 24
#define InfoPanelWidth 30


void FillRect(char c, int x, int y, int w, int h);
void DrawRect(int bordertype, int x, int y, int w, int h);
extern void ClearScreen();
extern void DrawScreen();
extern void DrawLabel(char* text, int x, int y, int w);
extern void DrawRightLabel(char* text, int x, int y, int w);

extern void MMDown();
extern void MMUp();
extern void MMAct();
extern void PMDown();
extern void PMUp();
extern void PMAct();

extern void Refresh();

extern char InputC;
