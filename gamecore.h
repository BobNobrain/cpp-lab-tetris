extern int GStatus;
extern void OnKeyPressed(char key);
extern void OnFrame();
extern void NewGame();
void GameOver();
extern int MoveCurrentDown();
extern void MoveCurrentLeft();
extern void MoveCurrentRight();
extern void RotateCurrent();

struct Figure;

extern int RedrawInterval, Score, FiguresDropped, inpIndex;

void RemoveFromField(Figure f);
void PlaceToField(Figure f);
extern int RemoveFullRows();
int IsFullRow(int row);
bool CollisionTest(Figure f);
bool CheckInside(Figure f);

void OnGotUserName();

void NextFigure();

extern int HighScores[10];
extern char Names[10][6];

extern char* dmsg;
extern char UserName[6];

extern char NextImage[2][4];
void CreateNextImage();

void CreateShadow();

extern void LoadScores();
extern void SaveScores();

#define STATE_PLAYING 1
#define STATE_PAUSED 2
#define STATE_GAMEOVER 3
#define STATE_MENU 0
#define STATE_SCORES 4
#define STATE_NEWREC 5
#define STATE_EXIT 6

#define TF_WIDTH 12
#define TF_HEIGHT 18

#define TILE_EMPTY 32
#define TILE_LGREY 176
#define TILE_GREY 177
#define TILE_DGREY 178
#define TILE_BLACK 219
#define TILE_SHADOW 250

extern char GameField[TF_HEIGHT][TF_WIDTH];

extern int frame;
