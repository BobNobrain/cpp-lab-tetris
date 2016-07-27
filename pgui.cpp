#include <string.h>
#include "pgui.h"
#include "gamecore.h"

using namespace std;

//single border
//corners
const char CLT1=218, CRT1=191, CLB1=192, CRB1=217;
//borders
const char VERT1=179, HOR1=196;

char Screen[SHeight][SWidth+1];

char* StatusText="";
char* temp1, temp2;

struct Menu
{
    char **Items;
    char *Title;
    int Selected, cnt;

    Menu()
    {
        Selected=-1;
        Title="";
        cnt=0;
    }
    Menu(char *caption, int itemsCount, char menuitems[3][32])
    {
        Items=new char*[itemsCount];
        for(int i=0; i<itemsCount; i++)
        {
            Items[i]=menuitems[i];
        }
        Selected=0;
        cnt=itemsCount;
        Title=caption;
    }

    void MoveUp()
    {
        if(--Selected<0) Selected=cnt-1;
    }
    void MoveDown()
    {
        if(++Selected>cnt-1) Selected=0;
    }
    void Draw(int x, int y)
    {
        DrawLabel(Title, x, y, SWidth);
        for(int i=0; i<cnt; ++i)
        {
            if(i==Selected)
            {
                DrawLabel(Items[i], x+3, y+i+2, SWidth);
            }
            else
            {
                DrawLabel(Items[i], x+2, y+i+2, SWidth);
            }
        }
        Screen[y+Selected+2][x]=16;
    }
};


char _mainMenuItems[3][32]={"Play", "High scores", "Exit"};
Menu MainMenu("MAIN MENU", 3, _mainMenuItems);
char _pausedMenuItems[4][32]={"Resume", "Restart", "Main menu", "Exit"};
Menu PausedMenu("PAUSED", 4, _pausedMenuItems);

void MMDown(){MainMenu.MoveDown();}
void MMUp(){MainMenu.MoveUp();}
void MMAct()
{
    switch(MainMenu.Selected)
    {
    case 0:
        NewGame();
        break;
    case 1:
        GStatus=STATE_SCORES;
        break;
    case 2:
        GStatus=STATE_EXIT;
        break;
    }
}

void PMDown(){PausedMenu.MoveDown();}
void PMUp(){PausedMenu.MoveUp();}
void PMAct()
{
    switch(PausedMenu.Selected)
    {
    case 0:
        GStatus=STATE_PLAYING;
        break;
    case 1:
        NewGame();
        break;
    case 2:
        GStatus=STATE_MENU;
        break;
    case 3:
        exit(0);
        break;
    }
}

//double border
//corners
const char CLT2=201, CRT2=187, CLB2=200, CRB2=188;
//borders
const char VERT2=186, HOR2=205;


int str_len(char* str)
{
    int r=0;
    while(str[r]!='\0')
        ++r;
    return r;
}

void FillRect(char c, int x, int y, int w, int h)
{
    int row, col;
    for(row=0; row<h; ++row)
        for(col=0; col<w; ++col)
            Screen[row+y][col+x]=c;
}

void DrawRect(int bordertype, int x, int y, int w, int h)
{
    int row, col;
    char c1, c2, c3, c4, vt, hz;
    if(bordertype==2)
    {
        c1=CLT2;
        c2=CRT2;
        c3=CLB2;
        c4=CRB2;
        vt=VERT2;
        hz=HOR2;
    }
    else
    {
        c1=CLT1;
        c2=CRT1;
        c3=CLB1;
        c4=CRB1;
        vt=VERT1;
        hz=HOR1;
    }
    Screen[y][x]=c1;
    Screen[y][x+w-1]=c2;
    Screen[y+h-1][x]=c3;
    Screen[y+h-1][x+w-1]=c4;
    FillRect(hz, x+1, y, w-2, 1);
    FillRect(hz, x+1, y+h-1, w-2, 1);
    FillRect(vt, x, y+1, 1, h-2);
    FillRect(vt, x+w-1, y+1, 1, h-2);
}

void DrawGroupBox(char* title, int x, int y, int w, int h)
{
    DrawRect(1, x, y, w, h);
    DrawLabel(title, x+1, y, w-2);
}

void DrawLabel(char* text, int x, int y, int w)
{
    int i;

    for(i=0; i<w && text[i]!='\0'; ++i)
    {
        Screen[y][x+i]=text[i];
    }
}
void DrawLabelWithBg(char* text, char background, int x, int y, int w)
{
    int i;

    for(i=x; i<x+w; ++i)
        Screen[y][i]=background;

    for(i=0; x+i<w && text[i]!='\0'; ++i)
    {
        Screen[y][x+i]=text[i];
    }
}
void DrawRightLabel(char* text, int x, int y, int w)
{
    int i, l=str_len(text);

    for(i=l-1; i>=0; --i)
    {
        if(0<=w+i-l)
        Screen[y][x+w+i-l]=text[i];
    }
}
void DrawRightLabelWithBg(char* text, char background, int x, int y, int w)
{
     int i, l=str_len(text);

    for(i=x; i<x+w; ++i)
        Screen[y][i]=background;

    for(i=l-1; i>=0; --i)
    {
        if(0<=w+i-l)
            Screen[y][x+w+i-l]=text[i];
    }
}

void DrawTetrisFieldBorder(int x, int y)
{
    int fw=12, fh=18; //field width and height
    Screen[y+fh+1][x+fw+1]=Screen[y+fh+1][x]=Screen[y][x+fw+1]=Screen[y][x]=254;
    FillRect(223, x+1, y, fw, 1);
    FillRect(220, x+1, y+fh+1, fw, 1);
    FillRect(221, x, y+1, 1, fh);
    FillRect(222, x+fw+1, y+1, 1, fh);
}
void DrawHighScores(int x, int y)
{
    //TODO: implement
}
void DrawGameField(int x, int y)
{
    for(int row=0; row<TF_HEIGHT; ++row)
    {
        for(int col=0; col<TF_WIDTH; ++col)
        {
            if(GStatus!=STATE_GAMEOVER)
                Screen[y+row][x+col]=GameField[row][col];
            else
                Screen[y+row][x+col]=TILE_DGREY;
        }
    }
}

void ClearScreen()
{
    system("color F0");

    int row, col;
    for(row=0; row<SHeight; row++)
        for(col=0; col<SWidth; col++)
            Screen[row][col]=' ';

    //smth for status strip
    int st_x=InfoPanelWidth+TF_WIDTH+5;
    DrawRect(1, st_x, SHeight-3, SWidth-st_x, 3);

    //border
    DrawRect(2, 0, 0, SWidth, SHeight);

    //infopanel border
    FillRect(VERT2, InfoPanelWidth, 1, 1, SHeight-2);
    Screen[0][InfoPanelWidth]=203;
    Screen[SHeight-1][InfoPanelWidth]=202;

    //corners and intersections
    FillRect(196, 1, 4, InfoPanelWidth-1, 1);
    Screen[4][0]=199;
    Screen[4][InfoPanelWidth]=182;

    //INFOPANEL

    //logo
    char logo[3][27]=
    {
        {177,32,178,219,219,32,32,178,178,32,32,32,176,176,32,32,176,32,32,32,32,32,32,32,176,32,32},
        {177,178,178,219,219,176,178,178,32,'T','E','T','R','I','S',32,176,32,219,178,178,32,219,219,176,32,32},
        {177,178,32,176,176,176,32,32,32,32,32,177,177,177,177,176,176,219,219,219,178,178,219,219,176,176,32}
    };
    int logox=2, logoy=1;
    for(row=0; row<3; ++row)
    {
        for(col=0; col<26; ++col)
        {
            Screen[row+logoy][col+logox]=logo[row][col];
        }
    }

    //game characteristics
    DrawRightLabel("SCORE:", 2, 5, 12);
    char temp[8];
    itoa(Score, temp, 10);
    DrawRightLabelWithBg(temp, 176, 16, 5, 7);

    DrawRightLabel("FIGURES:", 2, 6, 12);
    itoa(FiguresDropped, temp, 10);
    DrawRightLabelWithBg(temp, 176, 16, 6, 7);

    //next figure
    DrawGroupBox("NEXT", 2, 9, 27, 4);

    //help text
    DrawGroupBox("CONTROLS", 2, 13, 27, 7);

    DrawLabel("A,S,D  - MOVE", 4, 15, 23);
    DrawLabel("Space  - ROTATE", 4, 16, 23);
    DrawLabel("W      - DROP", 4, 17, 23);
    if(GStatus==STATE_PLAYING) DrawLabel("ESC    - PAUSE", 4, 18, 23);
    else if(GStatus==STATE_PAUSED) DrawLabel("ESC    - CONTINUE", 4, 18, 23);
    else DrawLabel("ESC    - EXIT", 4, 18, 23);

    DrawLabel("ver. 0.99 (16.12.13) [++]", 2, 22, 25);

    //game field
    DrawTetrisFieldBorder(32, 1);
    DrawGameField(33, 2);

    //endof INFOPANEL


    //status strip
    Screen[SHeight-2][st_x]=180;
    Screen[SHeight-1][st_x]=207;
    Screen[SHeight-3][SWidth-1]=182;
    Screen[SHeight-2][InfoPanelWidth]=199;
    FillRect(196, InfoPanelWidth+1, SHeight-2, TF_WIDTH+4, 1);

    //status text
    if(StatusText!="")
        DrawLabel(StatusText, 60, 22, 9);
    else
    {   //default
        switch(GStatus)
        {
        case STATE_MENU:
            DrawLabel("  Ready", 49, 22, 20);
            break;

        case STATE_PLAYING:
        {
            char tmp[]={16,32,'P','l','a','y','i','n','g', '\0'};
            DrawLabel(tmp, 49, 22, 20);
            //rendering gamefield
            break;
        }
        case STATE_PAUSED:
        {
            char tmp[]={222,222,32,'P','a','u','s','e','d', '\0'};
            DrawLabel(tmp, 49, 22, 20);
            break;
        }
        case STATE_GAMEOVER:
        {
            char tmp[]={254,32,'G','A','M','E','O','V','E','R', '\0'};
            DrawLabel(tmp, 49, 22, 20);
            break;
        }
        case STATE_SCORES:
            DrawLabel("  SPACE to exit", 49, 22, 20);
            break;
        }
    }
    DrawLabel(dmsg, 60, 22, 9);
}
void DrawScreen()
{
    system("cls");
    for(int row=0; row<SHeight; row++)
    {
        puts(Screen[row]);
    }
}

void Refresh()
{
    ClearScreen();
    switch(GStatus)
    {
    case STATE_MENU:
        MainMenu.Draw(49, 1);
        break;

    case STATE_PLAYING:
        for(int row=0; row<2; ++row)
        {
            for(int col=0; col<4; ++col)
            {
                Screen[row+10][col+4]=NextImage[row][col];
            }
        }
        break;

    case STATE_PAUSED:
        PausedMenu.Draw(49, 1);
        break;

    case STATE_GAMEOVER:
        //drawing gameover msg
        break;

    case STATE_SCORES:
        DrawLabel("HIGH SCORES", 49, 1, 11);
        for(int i=0; i<10; ++i)
        {
            char scoreStr[8];
            itoa(HighScores[i], scoreStr, 10);
            DrawRightLabelWithBg(scoreStr, i%2==0?TILE_LGREY:TILE_GREY, 49, 3+i, 15);
            DrawLabel(Names[i], 50, 3+i, 6);
        }
        break;

    case STATE_NEWREC:
        DrawLabel("New record! Enter your name:", 49, 1, 30);
        DrawLabel(UserName, 50, 3, 6);
        Screen[4][50+inpIndex]='^';
        DrawLabel("Press ENTER to continue", 49, 5, 30);
        break;
    }
    DrawScreen();
}
