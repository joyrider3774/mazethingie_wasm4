#include <stdlib.h>
#include <cstring>
#include <stdint.h>
#include "wasm4.h"
#include "helperfuncs.h"

using namespace std;


enum GameStates {GSGame,GSTitleScreen,GSWinner};
const int BoxWidth=3, BoxHeight=3;
const int MaxMazeWidth = (SCREEN_SIZE / BoxWidth) -2, MaxMazeHeight = (SCREEN_SIZE / BoxHeight) -2;
const int MaxMazeSize = MaxMazeWidth * MaxMazeHeight;
GameStates GameState = GSTitleScreen;
uint8_t MazeWidth=MaxMazeWidth, MazeHeight=MaxMazeHeight, PlayerPosX=32, PlayerPosY=32, Selection = 1, xoffset = 0, yoffset = 0;
uint8_t Maze[MaxMazeSize];
unsigned int frames_drawn=0;

typedef struct SaveData SaveData;
struct SaveData {
  uint8_t w,h;
};

void LoadSettings()
{
	SaveData save;
	save.w = MaxMazeWidth;
	save.h = MaxMazeHeight;
	diskr(&save, sizeof(save));
	MazeWidth = save.w;
	MazeHeight = save.h;
	if (MazeWidth > MaxMazeWidth)
		MazeWidth = MaxMazeWidth;
	if(MazeHeight > MaxMazeHeight)
		MazeHeight = MaxMazeHeight;
}

void SaveSettings()
{
	SaveData save;
	save.w = MazeWidth;
	save.h = MazeHeight;
	diskw(&save, sizeof(save));
}

void DrawMaze()
{
	
    int X,Y,I;
    setDrawColor(0,0,1,1);
	rect(0, 0, SCREEN_SIZE, SCREEN_SIZE);
	setDrawColor(0,0,1,2);
    for (Y= 0;Y< MazeHeight;Y++)
        for(X = 0;X < MazeWidth;X++)
        {
			I = (Y*MazeWidth) + X;
            //north wall
			if (Maze[I] & 1)
            {
                line(xoffset + (X+1) * BoxWidth,yoffset + (Y+1) * BoxHeight,xoffset + (X+2)*BoxWidth,yoffset + (Y+1)*BoxHeight);
            }

			//east wall
			if(Maze[I] & 2)
            {
                line(xoffset + (X+2) * BoxWidth,yoffset + (Y+1) * BoxHeight,xoffset + (X+2)*BoxWidth,yoffset + (Y+2)*BoxHeight);
            }

			//south wall
            if(Maze[I] & 4)
            {
                line(xoffset + (X+1) * BoxWidth,yoffset + (Y+2) * BoxHeight,xoffset + (X+2)*BoxWidth,yoffset + (Y+2)*BoxHeight);
            }

			//west wall
            if(Maze[I] & 8)
            {
                line(xoffset + (X+1) * BoxWidth,yoffset + (Y+1) * BoxHeight,xoffset + (X+1)*BoxWidth,yoffset + (Y+2)*BoxHeight);
            }
        }
}

void GenerateMaze()
{
	srand(frames_drawn + MazeWidth + MazeHeight);
    uint16_t neighbours[4];
    uint16_t cellStack[MaxMazeSize];
    uint16_t cc = 0;
    uint16_t currentPoint = 0;
    uint16_t visitedRooms = 1;
    uint16_t tmp2;
    uint16_t selectedNeighbour;
    uint16_t rnd;
    
    //intial all walls value in every room we will remove bits of this value to remove walls
    memset(Maze, 0xfu, (size_t)(MaxMazeSize));

	while (visitedRooms != MazeHeight*MazeWidth)
    {
        uint16_t neighboursFound = 0;
        uint16_t lookUpX = currentPoint % MazeWidth;
        uint16_t lookUpY = currentPoint / MazeWidth;
        uint16_t tmp  = currentPoint+1; 
        //tile has neighbour to the right which we did not handle yet
        if (( lookUpX + 1 < MazeWidth) && (Maze[tmp] == 0xfu))
            neighbours[neighboursFound++] = tmp;
    
        tmp = currentPoint-1; 
        //tile has neighbour to the left which we did not handle yet
        if ((lookUpX > 0) && (Maze[tmp] == 0xfu))
            neighbours[neighboursFound++] = tmp;

        tmp = currentPoint - MazeWidth; 
        //tile has neighbour the north which we did not handle yet
        if ((lookUpY > 0) && (Maze[tmp] == 0xfu))
            neighbours[neighboursFound++] = tmp;

        tmp = currentPoint + MazeWidth; 
        //tile has neighbour the south which we did not handle yet
        if ((lookUpY + 1 < MazeHeight) && (Maze[tmp] == 0xfu))
            neighbours[neighboursFound++] = tmp;

        switch (neighboursFound)
        {
            case 0:
                currentPoint = cellStack[--cc];
                continue;
                break;
            default:
                rnd = (uint16_t)(rand() % neighboursFound);
                break;
        }
        selectedNeighbour = neighbours[rnd];      
        tmp = (selectedNeighbour % MazeWidth);
        //tile has neighbour to the east
        if(tmp > lookUpX)
        {
            //remove west wall neighbour
            Maze[selectedNeighbour] &= ~(8);
            //remove east wall tile
            Maze[currentPoint] &= ~(2);
        }
        else // tile has neighbour to the west
        {
            if(tmp < lookUpX)
            {
                //remove east wall neighbour
                Maze[selectedNeighbour] &= ~(2);
                //remove west wall tile
                Maze[currentPoint] &= ~(8);
            }
            else // tile has neighbour to the north
            {
                tmp2 = selectedNeighbour / MazeWidth;
                if(tmp2 < lookUpY)
                {
                    //remove south wall neighbour
                    Maze[selectedNeighbour] &= ~(4);
                    //remove north wall tile
                    Maze[currentPoint] &= ~(1);
                }
                else // tile has neighbour to the south
                {
                    if(tmp2 > lookUpY)
                    {
                        //remove north wall neighbour
                        Maze[selectedNeighbour] &= ~(1);
                        //remove south wall tile
                        Maze[currentPoint] &= ~(4);
                    }
                }
            }
        }
        
        //add tile to the cellstack
        if(neighboursFound > 1)
        {
            cellStack[cc++] = currentPoint;
        } 
        //set tile to the neighbour   
        currentPoint = selectedNeighbour;
        visitedRooms++;
    }
	Maze[0] &= ~(1);
    Maze[(MazeWidth)*(MazeHeight)-1] &= ~(4);
    PlayerPosX = MazeWidth -1;
    PlayerPosY = MazeHeight - 1;
	xoffset = (((SCREEN_SIZE/BoxWidth) - (MazeWidth+2)) * BoxWidth) >> 1;
	yoffset = (((SCREEN_SIZE/BoxHeight) - (MazeHeight+2)) * BoxHeight) >> 1;
}

void TitleScreen()
{
   	// SDL_Color Color1={255,255,255,255},Color2={100,100,255,255};
	if(buttonReleased(BUTTON_UP))
   		if (Selection > 1)
    	    Selection--;
        
	if(buttonReleased(BUTTON_DOWN))
		if (Selection < 3)
        	Selection++;

	if(buttonReleased(BUTTON_1))
	{
		SaveSettings();
		GenerateMaze();
	    GameState = GSGame;
	}

	if(Selection == 2)
    {
	    if (buttonReleased(BUTTON_RIGHT))
    	    if (MazeWidth < MaxMazeWidth)
            	MazeWidth++;

        if (buttonReleased(BUTTON_LEFT))
	        if(MazeWidth > 10 )
    	        MazeWidth--;
        
	}

    if(Selection == 3)
    {
		if (buttonReleased(BUTTON_RIGHT))
			if (MazeHeight < MaxMazeHeight)
				MazeHeight++;
		if (buttonReleased(BUTTON_LEFT))
			if(MazeHeight > 10 )
				MazeHeight--;
	}

	setDrawColor(0,0,1,1);
    rect(0, 0, SCREEN_SIZE,SCREEN_SIZE);
	setDrawColor(0,0,1,2);
    text("    MAZE THINGIE", 0,20);
    if (Selection==1)
	{
		setDrawColor(0,0,1,3);
        text("Play",30,50);
	}
    else
	{
		setDrawColor(0,0,1,2);
        text("Play",30,50);
	}

    char Text[50];
    citoa(MazeWidth, Text, 10);
    if(Selection == 2)
	{
		setDrawColor(0,0,1,3);
	    text("Maze Width:",30,70);
		text(Text,30+11*8,70);
	}
    else
    {
		setDrawColor(0,0,1,2);
	    text("Maze Width:",30,70);
		text(Text,30+11*8,70);
	}

	citoa(MazeHeight, Text, 10);
    if(Selection == 3)
	{
		setDrawColor(0,0,1,3);
	    text("Maze Height:",30,90);
		text(Text,30+12*8,90);
	}
    else
    {
		setDrawColor(0,0,1,2);
	    text("Maze Height:",30,90);
		text(Text,30+12*8,90);
	}

	setDrawColor(0,0,1,2);
    text("     Created by\nWillems Davy - 2024",4,130);
}

void DrawPlayer()
{
	setDrawColor(0,0,3,3);
    rect((int32_t)(xoffset + ((PlayerPosX+1) * BoxWidth)+1), (int32_t)(yoffset + ((PlayerPosY + 1) * BoxHeight)+1), BoxWidth-1, BoxHeight-1);
}

void Game()
{
    bool GameWon = false;
    if(buttonReleased(BUTTON_2))
		GameState = GSTitleScreen;
	if(buttonReleased(BUTTON_1))
		GenerateMaze();

    if (buttonReleased(BUTTON_RIGHT))
    	if (!(Maze[(PlayerPosY*MazeWidth) + PlayerPosX] & 2))
            PlayerPosX++;
    if (buttonReleased(BUTTON_LEFT))
        if (!(Maze[(PlayerPosY*MazeWidth) + PlayerPosX] & 8))
            PlayerPosX--;
    if (buttonReleased(BUTTON_UP))
	{    
        if((PlayerPosX == 0) && (PlayerPosY == 0))
            GameWon = true;
        else
            if (!(Maze[(PlayerPosY*MazeWidth) + PlayerPosX] & 1))
                PlayerPosY--;
	}

    if (buttonReleased(BUTTON_DOWN))
    	if((PlayerPosX != MazeWidth-1) || (PlayerPosY !=MazeHeight -1))
        	if (!(Maze[(PlayerPosY*MazeWidth) + PlayerPosX] & 4))
                PlayerPosY++;
    DrawMaze();
    DrawPlayer();

    if (GameWon)
	    GameState = GSWinner;
}

void Winner()
{
	setDrawColor(0,0,1,1);
	rect(0,0,SCREEN_SIZE, SCREEN_SIZE);
    DrawMaze();
    DrawPlayer();
	setDrawColor(0,0,1,3);
	//spaces are added to form rectangle around text
	text("Congratulations!!!     \nYou solved the maze,   \nlet's try another      \none ...               ", 0, 60);
	
	if(buttonReleased(BUTTON_1))
	{
	    GenerateMaze();
		GameState = GSGame;
	}
}

void update()
{
	setDrawColor(1,2,3,4);
	switch(GameState)
	{
		case GSGame :
			Game();
			break;
		case GSTitleScreen:
			TitleScreen();
			break;
		case GSWinner:
			Winner();
			break;
	}
	updatePrevInputs();
	frames_drawn++;
}

void start()
{
	setPaletteColors(0x000000,0xffffff,0xff0000,0x0000ff);
	updatePrevInputs();
	LoadSettings();
}
