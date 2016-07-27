#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <ctime>

#include "pgui.h"
#include "gamecore.h"

using namespace std;

int main()
{
    srand(time(NULL));
    /*if(LoadScores()!=0)
    {
        for(int i=0; i<10; ++i)
        {
            for(int j=0; j<6; ++j)
            {
                Names[i][j]=32;
            }
            HighScores[i]=0;
        }
        SaveScores();
    }*/
    LoadScores();

    for(int row=0; row<TF_HEIGHT; ++row)
    {
        for(int col=0; col<TF_WIDTH; ++col)
        {
            switch(col*row%5)
            {
                case 0: GameField[row][col]=TILE_EMPTY; break;
                case 1: GameField[row][col]=TILE_LGREY; break;
                case 2: GameField[row][col]= TILE_GREY; break;
                case 3: GameField[row][col]=TILE_DGREY; break;
                case 4: GameField[row][col]=TILE_BLACK; break;
            }
        }
    }

    while(1) //game cycle
    {
        if(kbhit() == true)
        {
            char c=getch();
            OnKeyPressed(c);
            //InputC=c;
        }

        if(GStatus==STATE_EXIT)
            return 0;

        OnFrame();

        Refresh();

        Sleep(RedrawInterval);
    }
}
