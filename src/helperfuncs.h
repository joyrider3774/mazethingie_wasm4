#ifndef helperfuncs_h
#define helperfuncs_h


#include "wasm4.h"

//added these for convienence
#define Color uint32_t
#define ColorIndex uint8_t

typedef struct Palette Palette;
struct Palette 
{
	Color Col1;
	Color Col2;
	Color Col3;
	Color Col4;
};

char* citoa(int num, char* str, int base);
void setPalette(Palette pal);
void setPaletteColors(Color col1, Color col2, Color col3, Color col4);
void clear(ColorIndex Index);
void setBackForeGroundColor(ColorIndex BackgroundIndex, ColorIndex ForGroundIndex);
void setBackGroundColor(ColorIndex Index);
void getDrawColor(ColorIndex *Index1, ColorIndex *Index2, ColorIndex *Index3, ColorIndex *Index4);
void setDrawColor(ColorIndex Index1, ColorIndex Index2, ColorIndex Index3, ColorIndex Index4);
bool buttonReleased(int Button);
bool mouseButtonReleased(int Button);
bool mouseMoved();
void updatePrevInputs();
bool mouseMovedAtleastOnce();
bool mouseInGameBounds();
bool anyButtonReleased();
void resetPrevMousePos();
#endif