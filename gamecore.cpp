#include <fstream>
#include <iostream>
#include "pgui.h"
#include "gamecore.h"

using namespace std;

char* dmsg="";
char UserName[6]={'n','o','n','a','m','e'};

char NextImage[2][4]=
{
    {32,32,32,32},
    {32,32,32,32}
};

struct Figure
{
    int xc, yc, x1, y1, x2, y2, x3, y3;
    char fillType;
    int type;

    Figure()
    {
        xc=TF_WIDTH/2;
        yc=0;
        int rnd=rand()%7;
        type=rnd;
        switch(rnd)
        {
            //creating figure shape
            case 0: // I
                x1=xc-1; x2=xc+1; x3=xc+2;
                y1=yc; y2=yc; y3=yc;
                fillType=TILE_GREY;
                break;
            case 1: // L
                x1=xc-1; x2=xc-1; x3=xc+1;
                y1=yc+1; y2=yc; y3=yc;
                fillType=TILE_LGREY;
                break;
            case 2: // Z
                x1=xc-1; x2=xc; x3=xc+1;
                y1=yc; y2=yc+1; y3=yc+1;
                fillType=TILE_DGREY;
                break;
            case 3: // Ã
                x1=xc-1; x2=xc+1; x3=xc+1;
                y1=yc; y2=yc; y3=yc+1;
                fillType=TILE_LGREY;
                break;
            case 4: // T
                x1=xc-1; x2=xc; x3=xc+1;
                y1=yc; y2=yc+1; y3=yc;
                fillType=TILE_BLACK;
                break;
            case 5: // S
                x1=xc-1; x2=xc; x3=xc+1;
                y1=yc+1; y2=yc+1; y3=yc;
                fillType=TILE_DGREY;
                break;
            case 6: // [ ]
                x1=xc+1; x2=xc; x3=xc+1;
                y1=yc; y2=yc+1; y3=yc+1;
                fillType=TILE_BLACK;
                break;
        }
    }

    void Move(int dx, int dy)
    {
        xc+=dx; x1+=dx; x2+=dx; x3+=dx;
        yc+=dy; y1+=dy; y2+=dy; y3+=dy;
    }
    void MoveDown() { Move(0, 1); }
    void MoveRight() { Move(1, 0); }
    void MoveLeft() { Move(-1, 0); }
    void MoveUp() { Move(0, -1); }

    void MoveTo(int x, int y)
    {
        Move(x-xc, y-yc);
    }

    void Rotate()
    {
        int nx1, nx2, nx3, ny1, ny2, ny3;
        nx1=xc-y1+yc; ny1=yc-xc+x1;
        nx2=xc-y2+yc; ny2=yc-xc+x2;
        nx3=xc-y3+yc; ny3=yc-xc+x3;
        x1=nx1; x2=nx2; x3=nx3;
        y1=ny1; y2=ny2; y3=ny3;
    }
    void RotateBack() { Rotate(); Rotate(); Rotate(); }
};

const char* ScoresFileName="scores.sav";

char GameField[TF_HEIGHT][TF_WIDTH];

char Names[10][6];
int HighScores[10];

int Score=0, FiguresDropped, frame=0, maxframe=5, inpIndex=0, insIndex=10;

Figure curFig;
Figure nextFig;
Figure shadowFig;

int RedrawInterval=30;
int GStatus=STATE_MENU;

//
// Functions
//

//starts a new game
void NewGame()
{
    GStatus=STATE_PLAYING;
    Score=0;
    FiguresDropped=0;
    frame=0;

    curFig =Figure();
    nextFig=Figure();
    CreateNextImage();

    for(int row=0; row<TF_HEIGHT; ++row)
    {
        for(int col=0; col<TF_WIDTH; ++col)
        {
            GameField[row][col]=TILE_EMPTY;
        }
    }

    CreateShadow();

    PlaceToField(shadowFig);
    PlaceToField(curFig);
}
void GameOver()
{
    GStatus=STATE_GAMEOVER;

    insIndex=10;
    for(int i=9; i>=0; --i)
    {
        if(Score>HighScores[i]) --insIndex;
        else break;
    }
    if(insIndex==10)
    {
        return;
    }
    for(int i=8; i>=insIndex; --i)
    {
        HighScores[i+1]=HighScores[i];
        for(int j=0; j<6; ++j)
        {
            Names[i+1][j]=Names[i][j];
        }
    }
    HighScores[insIndex]=Score;
    GStatus=STATE_NEWREC;
}

void OnGotUserName()
{
    for(int i=0; i<6; ++i)
    {
        Names[insIndex][i]=UserName[i];
    }
    GStatus=STATE_SCORES;

    SaveScores();
}

int MoveCurrentDown()
{
    RemoveFromField(curFig);
    RemoveFromField(shadowFig);
    curFig.MoveDown();
    if(!CheckInside(curFig) || CollisionTest(curFig))
    {
        //reached the floor
        curFig.MoveUp();
        PlaceToField(curFig);
        NextFigure();
        return 1;
    }
    PlaceToField(shadowFig);
    PlaceToField(curFig);
    return 0;
}
void MoveCurrentLeft()
{
    RemoveFromField(curFig);
    RemoveFromField(shadowFig);
    curFig.MoveLeft();
    if(!CheckInside(curFig) || CollisionTest(curFig))
    {
        curFig.MoveRight();
    }
    CreateShadow();
    PlaceToField(shadowFig);
    PlaceToField(curFig);
}
void MoveCurrentRight()
{
    RemoveFromField(curFig);
    RemoveFromField(shadowFig);
    curFig.MoveRight();
    if(!CheckInside(curFig) || CollisionTest(curFig))
    {
        curFig.MoveLeft();
    }
    CreateShadow();
    PlaceToField(shadowFig);
    PlaceToField(curFig);
}
void RotateCurrent()
{
    RemoveFromField(curFig);
    RemoveFromField(shadowFig);
    int xc=curFig.xc, yc=curFig.yc;
    curFig.Rotate();
    if(CheckInside(curFig) && !CollisionTest(curFig))
    {
        CreateShadow();
        PlaceToField(shadowFig);
        PlaceToField(curFig);
        return;
    }
    // failed, trying to place it lower
    curFig.MoveDown();
    if(CheckInside(curFig) && !CollisionTest(curFig))
    {
        CreateShadow();
        PlaceToField(shadowFig);
        PlaceToField(curFig);
        return;
    }
    // failed, trying to place it at left
    curFig.MoveUp();
    curFig.MoveLeft();
    if(CheckInside(curFig) && !CollisionTest(curFig))
    {
        CreateShadow();
        PlaceToField(shadowFig);
        PlaceToField(curFig);
        return;
    }
    // failed, trying to place it ar right
    curFig.MoveRight();
    curFig.MoveRight();
    if(CheckInside(curFig) && !CollisionTest(curFig))
    {
        CreateShadow();
        PlaceToField(shadowFig);
        PlaceToField(curFig);
        return;
    }
    // epic fail, cannot rotate
    curFig.MoveTo(xc, yc);
    curFig.RotateBack();
    PlaceToField(curFig);
}
void DropCurrent()
{
    while(!MoveCurrentDown());
    Score+=5;
}
void NextFigure()
{
    Score+=RemoveFullRows();
    curFig=nextFig;
    nextFig=Figure();
    CreateNextImage();
    if(CollisionTest(nextFig))
    {
        GameOver();
    }
    CreateShadow();
    PlaceToField(shadowFig);
    PlaceToField(curFig);
}

int RemoveFullRows()
{
    int done=0;
    int points=0;
    for(int row=0; row<TF_HEIGHT; ++row)
    {
        if(IsFullRow(row)) //all cells are full
        {
            ++done;
            for(int r=row; r>0; --r)
            {
                int cellsfull=0;
                //[r-1]->[r]
                for(int c=0; c<TF_WIDTH; c++)
                {
                    GameField[r][c]=GameField[r-1][c];
                    if(GameField[r-1][c]!=TILE_EMPTY)
                    {
                        ++cellsfull;
                    }
                }
                if(cellsfull==0) //was the row that we copied an empty one?
                {
                    //yeah, break
                    break;
                }
                //clear last row
                for(int c=0; c<TF_WIDTH; c++)
                {
                    GameField[0][c]=GameField[0][c];
                }
            }
        }
    }
    //bonuses for multiply clearings
    if(done>4) done*=2;
    if(done==4) done=6;
    if(done==3) done=4;
    if(done==2) done=3;
    return done*TF_WIDTH;
}
int IsFullRow(int row)
{
    int rowfull=0;
    for(int col=0; col<TF_WIDTH; ++col)
    {
        if(GameField[row][col]==TILE_EMPTY) break; //this row is not full, i'm sure
        ++rowfull;
    }
    return rowfull==TF_WIDTH;
}

void CreateNextImage()
{
    switch(nextFig.type)
    {
        case 0: // I
            NextImage[0][0]=TILE_GREY; NextImage[0][1]=TILE_GREY; NextImage[0][2]=TILE_GREY; NextImage[0][3]=TILE_GREY;
            NextImage[1][0]=TILE_EMPTY;NextImage[1][1]=TILE_EMPTY;NextImage[1][2]=TILE_EMPTY;NextImage[1][3]=TILE_EMPTY;
            break;
        case 1: // L
            NextImage[0][0]=TILE_EMPTY;NextImage[0][1]=TILE_EMPTY;NextImage[0][2]=TILE_LGREY;NextImage[0][3]=TILE_EMPTY;
            NextImage[1][0]=TILE_LGREY;NextImage[1][1]=TILE_LGREY;NextImage[1][2]=TILE_LGREY;NextImage[1][3]=TILE_EMPTY;
            break;
        case 2: // Z
            NextImage[0][0]=TILE_DGREY;NextImage[0][1]=TILE_DGREY;NextImage[0][2]=TILE_EMPTY;NextImage[0][3]=TILE_EMPTY;
            NextImage[1][0]=TILE_EMPTY;NextImage[1][1]=TILE_DGREY;NextImage[1][2]=TILE_DGREY;NextImage[1][3]=TILE_EMPTY;
            break;
        case 3: // Ã
            NextImage[0][0]=TILE_LGREY;NextImage[0][1]=TILE_EMPTY;NextImage[0][2]=TILE_EMPTY;NextImage[0][3]=TILE_EMPTY;
            NextImage[1][0]=TILE_LGREY;NextImage[1][1]=TILE_LGREY;NextImage[1][2]=TILE_LGREY;NextImage[1][3]=TILE_EMPTY;
            break;
        case 4: // T
            NextImage[0][0]=TILE_BLACK;NextImage[0][1]=TILE_BLACK;NextImage[0][2]=TILE_BLACK;NextImage[0][3]=TILE_EMPTY;
            NextImage[1][0]=TILE_EMPTY;NextImage[1][1]=TILE_BLACK;NextImage[1][2]=TILE_EMPTY;NextImage[1][3]=TILE_EMPTY;
            break;
        case 5: // S
            NextImage[0][0]=TILE_EMPTY;NextImage[0][1]=TILE_DGREY;NextImage[0][2]=TILE_DGREY;NextImage[0][3]=TILE_EMPTY;
            NextImage[1][0]=TILE_DGREY;NextImage[1][1]=TILE_DGREY;NextImage[1][2]=TILE_EMPTY;NextImage[1][3]=TILE_EMPTY;
            break;
        case 6: // [ ]
            NextImage[0][0]=TILE_EMPTY;NextImage[0][1]=TILE_BLACK;NextImage[0][2]=TILE_BLACK;NextImage[0][3]=TILE_EMPTY;
            NextImage[1][0]=TILE_EMPTY;NextImage[1][1]=TILE_BLACK;NextImage[1][2]=TILE_BLACK;NextImage[1][3]=TILE_EMPTY;
            break;
    }
}

void CreateShadow()
{
    shadowFig=curFig;
    shadowFig.fillType=TILE_SHADOW;
    while(!CollisionTest(shadowFig) && CheckInside(shadowFig))
    {
        shadowFig.MoveDown();
    }
    shadowFig.MoveUp();
}

//
// Service
//
void PlaceToField(Figure f)
{
    GameField[f.yc][f.xc]=GameField[f.y1][f.x1]=GameField[f.y2][f.x2]=GameField[f.y3][f.x3]=f.fillType;
}
void RemoveFromField(Figure f)
{
    GameField[f.yc][f.xc]=GameField[f.y1][f.x1]=GameField[f.y2][f.x2]=GameField[f.y3][f.x3]=TILE_EMPTY;
}
bool CheckInside(Figure f)
{
    bool res=true;
    res=res && f.xc<TF_WIDTH && f.x1<TF_WIDTH && f.x2<TF_WIDTH && f.x3<TF_WIDTH;
    res=res && f.yc<TF_HEIGHT && f.y1<TF_HEIGHT && f.y2<TF_HEIGHT && f.y3<TF_HEIGHT;
    res=res && f.xc>=0 && f.x1>=0 && f.x2>=0 && f.x3>=0;
    res=res && f.yc>=0 && f.y1>=0 && f.y2>=0 && f.y3>=0;
    return res;
}
bool CollisionTest(Figure f)
{
    bool res=true;
    res=res && GameField[f.yc][f.xc]==TILE_EMPTY;
    res=res && GameField[f.y1][f.x1]==TILE_EMPTY;
    res=res && GameField[f.y2][f.x2]==TILE_EMPTY;
    res=res && GameField[f.y3][f.x3]==TILE_EMPTY;
    return !res;
}

//
// Game loop
//
void OnFrame()
{
    ++frame;
    if(frame>=maxframe)
    {
         frame=0;
    }
    switch(GStatus)
    {
    case STATE_MENU:
        //doing nothing
        break;
    case STATE_PLAYING:
        if(frame==0)
            MoveCurrentDown();
        break;
    case STATE_PAUSED:
        break;
    case STATE_GAMEOVER:
        if(frame==0)
            GStatus=STATE_MENU;
        break;
    case STATE_SCORES:
        break;
    }
}

//
// Key controls
//
void OnDownPressed()
{
    switch(GStatus)
    {
    case STATE_MENU:
        MMDown();
        break;
    case STATE_PLAYING:
        MoveCurrentDown();
        break;
    case STATE_PAUSED:
        PMDown();
        break;
    case STATE_GAMEOVER:
        break;
    case STATE_SCORES:
        break;
    }
}
void OnUpPressed()
{
    switch(GStatus)
    {
    case STATE_MENU:
        MMUp();
        break;
    case STATE_PLAYING:
        DropCurrent();
        break;
    case STATE_PAUSED:
        PMUp();
        break;
    case STATE_GAMEOVER:
        break;
    case STATE_SCORES:
        break;
    }
}
void OnRightPressed()
{
    switch(GStatus)
    {
    case STATE_MENU:
        MMAct();
        break;
    case STATE_PLAYING:
        MoveCurrentRight();
        break;
    case STATE_PAUSED:
        PMAct();
        break;
    case STATE_GAMEOVER:
        break;
    case STATE_SCORES:
        break;
    }
}
void OnLeftPressed()
{
    switch(GStatus)
    {
    case STATE_MENU:
        break;
    case STATE_PLAYING:
        MoveCurrentLeft();
        break;
    case STATE_PAUSED:
        break;
    case STATE_GAMEOVER:
        break;
    case STATE_SCORES:
        break;
    }
}
void OnSpacePressed()
{
    switch(GStatus)
    {
    case STATE_MENU:
        MMAct();
        break;
    case STATE_PLAYING:
        RotateCurrent();
        break;
    case STATE_PAUSED:
        PMAct();
        break;
    case STATE_GAMEOVER:
        break;
    case STATE_SCORES:
        GStatus=STATE_MENU;
        break;
    }
}
void OnEscPressed()
{
    if(GStatus==STATE_PLAYING)
    {
        GStatus=STATE_PAUSED;
        return;
    }
    if(GStatus==STATE_PAUSED)
    {
        GStatus=STATE_PLAYING;
        return;
    }
    exit(0);
}

void OnKeyPressed(char key)
{
    switch(key)
    {
        case 27: OnEscPressed(); break;
        case 80: case 'S': case 's': OnDownPressed(); break;
        case 72: case 'W': case 'w': OnUpPressed(); break;
        case 77: case 'D': case 'd': OnRightPressed(); break;
        case 75: case 'A': case 'a': OnLeftPressed(); break;
        case 13: case ' ': OnSpacePressed(); break;
    }
    if(GStatus==STATE_NEWREC)
    {
        if(key>='A' && key<='z' || key==' ')
        {
            UserName[inpIndex]=key;
            if(++inpIndex>5)
                inpIndex=0;
        }
        if(key==13)
        {
            OnGotUserName();
        }
    }
}

//
// SAVING
//

void LoadScores()
{
    // OBSOLETE
    /*ifstream svs;
    svs.open("scores.sav");
    char inc;
    int points;
    for(int i=0; i<10; ++i)
    {
        for(int j=0; j<6; ++j)
        {
            svs>>inc;
            if(!svs.good())
            {
                svs.close();
                return -1;
            }
            Names[i][j]=inc;
        }
        svs>>points;
        HighScores[i]=points;
    }
    svs.close();
    return 0;*/
    FILE * pFile;
    pFile=fopen("scores.sav", "r");
    if (pFile!=NULL)
    {
        for(int i=0; i<10; ++i)
        {
            for(int j=0; j<6; ++j)
            {
                fscanf(pFile, "%c", &Names[i][j]);
            }
            fscanf(pFile, "%d", &HighScores[i]);
        }
        fclose (pFile);
    }
}

void SaveScores()
{
    FILE * pFile;
    pFile = fopen("scores.sav", "w");
    if (pFile!=NULL)
    {
        for(int i=0; i<10; ++i)
        {
            for(int j=0; j<6; ++j)
            {
                fprintf(pFile, "%c", Names[i][j]);
                ;
            }
            fprintf(pFile, "%d", HighScores[i]);
        }
        fclose (pFile);
    }
}
