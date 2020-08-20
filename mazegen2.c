/*
 *Carlos Ramos Project 1 Phase 2
 * Cs241 Section 1.
 * Nobember 8 2019
*/
#include <stdio.h>
#include <time.h>
//#include "mazegen.h"
#include "mazegen.h"
#include <stdlib.h>
#include <string.h>
static unsigned char **maze;
int startPosition;
int endPosition;
int gheight;
int gwidth;
int gwayPointX, gwayPointY, gwayPointAlleyLength, gprintAlgorithmSteps;
int galleyWayX[] = {0, 0, 0, 0};
int galleyWayY[] = {0, 0, 0, 0};
int galleyWayNorthX, galleyWayNorthY, galleyWayEastX, galleyWayEastY;
int galleyWaySouthX, galleyWaySouthY, galleyWayWestX, galleyWayWestY;
double gwayPointDirectionPercent;
double gmaxCellsToCarve;
int alleyWayNorth, alleyWayEast, alleyWaySouth, alleyWayWest;
int TOTALELEMENTS;
int freeCheck;
int possibleIndexes;
int isSolving;
int hitEntranceCheck = FALSE;
int possiblePaths[3];
int alleyCarveCheck = FALSE;
int solveMazeCheck = FALSE;
int foundExit = FALSE;
/*
 * Gives you an index to the opposite side direction of DirectionList
*/
int reverseDir(int x)
{
    int north = 0;
    int east = 1;
    int south = 2;
    int west = 3;

    if(x == 0)
    {
        return south;
    }
    else if( x == 1)
    {
        return west;
    }
    else if( x == 2)
    {
        return north;
    }
    else return east;
}
/*
 * Checks the bounds of the given index
 * If it works then returns true.
*/
int boundTest(int x, int y)
{
if (((x) < gwidth && (x >= 0)) && ((y ) < gheight && (y >= 0)))
{
    return TRUE;
}
else return FALSE;
}
/*
 * Gives you Possible directions to take
 * Returns a random number from the list of possible
 * directions to take.Supposed to give you a index 
 * to look at the arrays with.
 */
int dirFinder(int x, int y)
{
    int indPos[TOTAL_DIRECTIONS];
    int count = 0;
    for(int i = 0; i < TOTAL_DIRECTIONS; i++)
    {
	   
        if( ((x + DIRECTION_DX[i]) < gwidth && (x + DIRECTION_DX[i] >= 0)) &&
          ((y + DIRECTION_DY[i]) < gheight && (y + DIRECTION_DY[i] >= 0)))
	{
            if(maze[x + DIRECTION_DX[i]][y+DIRECTION_DY[i]] ==  0 && isSolving == FALSE)
            {
                indPos[count] =i;// DIRECTION_LIST[i];
                count++;
            }
            else if(  isSolving == TRUE &&
                   (maze[x][y] & DIRECTION_LIST[i]) > 0 &&
                   (maze[x+DIRECTION_DX[i]][y+DIRECTION_DY[i]] & VISITED) == 0)
            {
                indPos[count] = i;
                count++;
            }
        }
    }
    if( count == 0 )
    {
        return 4;
    }
    int randomValue = rand()%count;
    return indPos[randomValue];
}
//========================================================================
//This function copies 4 bytes from in int to an unsigned char array where
//  the least significant byte of the int is placed in the first element
//  of the array.
//========================================================================
void copyIntToAddress(int n, unsigned char bytes[])
{
  bytes[0] = n & 0xFF;
  bytes[1] = (n >>  8) & 0xFF;
  bytes[2] = (n >> 16) & 0xFF;
  bytes[3] = (n >> 24) & 0xFF;
}
//========================================================================
//Sets the RGB value of a single pixel at coordinates (x,y) in the
//   character array, data in bitmap format with three bytes per pixel.
//
//Bitmap format stores rows from bottom to top. Therefore, this function
//   needs the pixelHeight to calculate the offset into data.
//
//Bitmap format requires that each row is divisible by 4. Therefore,
//   rowSize may need to be padded up to 3 bytes past the end of the data.
//========================================================================
void setRGB(unsigned char data[], int x, int y, int rowSize,
            int pixelHeight,
            unsigned char r, unsigned char g, unsigned char b)
{
  y = (pixelHeight - y) - 1;
  int offset = (x * 3) + (y * rowSize);
  data[offset] = b;
  data[offset+1] = g;
  data[offset+2] = r;
}
/*
 * Prints out the maze.
 * Has two cases. One if the maze has just been carved
 * and the other if the maze solveMazeCheck is true.
*/
void mazePrint()
{	
    printf("\n\n");
    printf("========================\n");
    for(int i =0; i < gheight; i++)
    {
        for( int j = 0; j < gwidth; j++)
        {
            if(solveMazeCheck)
            {
                if(maze[j][i] & GOAL && gprintAlgorithmSteps == TRUE)
                {
                    textcolor(TEXTCOLOR_GREEN);
                    maze[j][i] = maze[j][i] & 15;  
                }
                else if(j == startPosition && i == 0 || maze[j][i] > SPECIAL )
                {
                    maze[j][i] = maze[j][i] & 15;
                    textcolor(TEXTCOLOR_RED);
                }
                else
                {
                    textcolor(TEXTCOLOR_WHITE);
                    maze[j][i] = maze[j][i] & 15;
                }
                printf("%c", pipeList[maze[j][i]]);
                textcolor(TEXTCOLOR_WHITE);
            }
            else
            {
                if( maze[j][i] > SPECIAL)
                {
                    textcolor(TEXTCOLOR_RED); 
                    printf("%c", pipeList[maze[j][i] & 15]);
                   // printf("%d",maze[j][i] & 15);
                }	       
                else if(maze[j][i] >= GOAL)
                {
                    textcolor(TEXTCOLOR_RED); 
                    printf("%c", pipeList[maze[j][i] - GOAL]);
                }
                else if (maze[j][i] < GOAL)
                {
                    textcolor(TEXTCOLOR_WHITE);
                    printf("%c",pipeList[maze[j][i]]);//], maze[j][i]); //maze[i][j]);
                }
            }
        }
        printf("\n");
    }
    textcolor(TEXTCOLOR_WHITE);
    printf("========================\n");
    isSolving = FALSE;

    //sets sizeof each pipe in the maze
    int pixelWidth = gwidth*8;
    int pixelHeight = gheight*8;
    int rowSize = pixelWidth*3;
    int rowPadding = 0;
    rowPadding = (4 - (rowSize % 4)) % 4;
    rowSize += rowPadding;
    int pixelDataSize = rowSize*pixelHeight;
    int fileSize = 54 + pixelDataSize;
    //indexes for for loops
    int i, j, k, m, n, r;
    //Temporary storage for locations
    int tempX = 0;
    int tempY = 0;
    int tempX2 = 0;
    int tempY2 = 0;

    unsigned char header[54] = 
    {
      'B','M',  // magic number
      0,0,0,0,  // size in bytes (set below)
      0,0,0,0,  // reserved
      54,0,0,0, // offset to start of pixel data
      40,0,0,0, // info hd size
      0,0,0,0,  // image width (set below)
      0,0,0,0,  // image heigth (set below)
      1,0,      // number color planes
      24,0,     // bits per pixel
      0,0,0,0,  // compression is none
      0,0,0,0,  // image bits size
      0x13,0x0B,0,0, // horz resoluition in pixel / m
      0x13,0x0B,0,0, // vert resolutions (0x03C3 = 96 dpi, 0x0B13 = 72 dpi)
      0,0,0,0,  // #colors in pallete
      0,0,0,0,  // #important colors
    };

    copyIntToAddress(fileSize, &header[2]);
    copyIntToAddress(pixelWidth, &header[18]);
    copyIntToAddress(pixelHeight, &header[22]);
    copyIntToAddress(pixelDataSize, &header[34]);

    unsigned char img[pixelDataSize];

    memset(img,0xFF,sizeof(img));

    // Sets background.
    for(i = 0; i < gheight*8; i++)
    {
        for(j = 0; j < gwidth*8; j++)
        {
            setRGB(img, j, i, rowSize, pixelHeight, 255, 255, 255);
        }
    }
    int red = 210;
    int green = 100;
    int blue = 0;
    // Creates the bitmap for the maze
    for(i = 0; i < gheight; i++)
    {
        for(j = 0; j < gwidth; j++)
        {
            tempX = j * 8;
            tempY = i * 8;
     
            //  1
            if(maze[j][i] == 1)
            { 
                tempX += 4;
                for(k = 0; k < 4; k++)
                {
                    setRGB(img, tempX, tempY, rowSize, pixelHeight, red,green, blue);
                    setRGB(img, tempX - 1, tempY, rowSize, pixelHeight, red,green, blue);
                    tempY += 1;
                 }
            setRGB(img, tempX, tempY, rowSize, pixelHeight, red,green, blue);
            setRGB(img, tempX - 1, tempY, rowSize, pixelHeight, red,green, blue);
            setRGB(img, tempX + 1, tempY, rowSize, pixelHeight, red,green, blue);
            setRGB(img, tempX - 2, tempY, rowSize, pixelHeight, red,green, blue);
            }
     
      //  2
      else if(maze[j][i] == 2)
      {
        tempY += 4;
        setRGB(img, tempX, tempY + 1, rowSize, pixelHeight, red,green, blue);
        setRGB(img, tempX, tempY - 2, rowSize, pixelHeight, red,green, blue);
        for(k = 0; k < 4; k++)
        {
          setRGB(img, tempX, tempY, rowSize, pixelHeight, red,green, blue);
          setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, red,green, blue);
          tempX += 1;
        }
        setRGB(img, tempX, tempY, rowSize, pixelHeight, red,green, blue);
        setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, red,green, blue);
        tempX += 1;
        setRGB(img, tempX, tempY, rowSize, pixelHeight, red,green, blue);
        setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, red,green, blue);
        tempX += 1;
        setRGB(img, tempX, tempY, rowSize, pixelHeight, red,green, blue);
        setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, red,green, blue);
      }
     
      // 3 and 6
      else if(maze[j][i] == 3 || maze[j][i] == 6)
      { 
        tempY2 = tempY + 4;
        tempX += 7;
        tempY += 3;
        for(k = 0; k < 4; k++)
        {
            setRGB(img, tempX, tempY, rowSize, pixelHeight, red,green, blue);
            setRGB(img, tempX, tempY2, rowSize, pixelHeight, red,green, blue);
          tempX -= 1;
        }
        for(k = 0; k < 4; k++)
        {
            setRGB(img, tempX, tempY, rowSize, pixelHeight, red,green, blue);
            setRGB(img, tempX + 1, tempY, rowSize, pixelHeight, red,green, blue);
          if(maze[j][i] == 3)
          {
            tempY -= 1;
          }
          else
          {
            tempY += 1;
          }
        }
          setRGB(img, tempX, tempY, rowSize, pixelHeight, red,green, blue);
          setRGB(img, tempX + 1, tempY, rowSize, pixelHeight, red,green, blue);
      }
     
      // 4, 5, 7 and 13
      else if(maze[j][i] == 4 || maze[j][i] == 5 || maze[j][i] == 7 || maze[j][i] == 13)
      { 
        tempX2 = tempX + 3;
        tempX += 4;
        if(maze[j][i] == 4)
        {
          setRGB(img, tempX2 - 1, tempY, rowSize, pixelHeight, red,green, blue);
          setRGB(img, tempX + 1, tempY, rowSize, pixelHeight, red,green, blue);
        }
        for(k = 0; k < 8; k++)
        {
            setRGB(img, tempX, tempY, rowSize, pixelHeight, red,green, blue);
            setRGB(img, tempX2, tempY, rowSize, pixelHeight, red,green, blue);
            tempY += 1;
        }
        if(maze[j][i] == 7)
        {
          tempY2 = tempY - 5;
          tempY -= 4;
          for(k = 0; k < 4; k++)
          {
              setRGB(img, tempX, tempY2, rowSize, pixelHeight, red,green, blue);
              setRGB(img, tempX, tempY, rowSize, pixelHeight, red,green, blue);
              tempX += 1;
          }
        }
        if(maze[j][i] == 13)
        {
          tempY2 = tempY - 5;
          tempY -= 4;
          for(k = 0; k < 4; k++)
          {
              setRGB(img, tempX, tempY2, rowSize, pixelHeight, red,green, blue);
              setRGB(img, tempX, tempY, rowSize, pixelHeight, red,green, blue);
              tempX -= 1;
          }
          for(r = 0; r < 3; r++)
      	  {
	      setRGB(img, tempX, tempY2, rowSize, pixelHeight, red,green, blue);
              setRGB(img, tempX, tempY, rowSize, pixelHeight, red,green, blue);
              tempX -= 1;
	   }
        }  
      }
     
      //  8 and 10
      else if(maze[j][i] == 8 || maze[j][i] == 10)
      {
        if(maze[j][i] == 10)
        {
          tempY2 = tempY + 4;
        }
        else if(maze[j][i] == 8)
        {
          tempY2 = tempY + 4;
        }
        tempX += 7;
        tempY += 3;
        if(maze[j][i] == 8)
        {
          setRGB(img, tempX, tempY2 + 1, rowSize, pixelHeight, red,green, blue);
          setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, red,green, blue);
        }
        for(k = 0; k < 8; k++)
        {
          setRGB(img, tempX, tempY, rowSize, pixelHeight, red,green, blue);
          setRGB(img, tempX, tempY2, rowSize, pixelHeight, red,green, blue);
          tempX -= 1;
        }
        if(maze[j][i] == 10)
        {
          setRGB(img, tempX, tempY, rowSize, pixelHeight, red,green, blue);
          setRGB(img, tempX, tempY2, rowSize, pixelHeight, red,green, blue);
        }  
      }
     
      //  9, 11, 14 and 15
      else if(maze[j][i] == 9 || maze[j][i] == 11 || maze[j][i] == 14 || maze[j][i] == 15)
      {
        tempX2 = tempX + 3;
        tempX += 4;
        if(maze[j][i] == 14 || maze[j][i] == 15)
        {
          tempY += 8;
        }
        for(k = 0; k < 4; k++)
        {
            setRGB(img, tempX, tempY, rowSize, pixelHeight, red,green, blue);
            setRGB(img, tempX2, tempY, rowSize, pixelHeight, red,green, blue);
          if(maze[j][i] == 9 || maze[j][i] == 11)
          {
            tempY += 1;
          }
          else if(maze[j][i] == 14 || maze[j][i] == 15)
          {
            tempY -= 1;
          }
        }
        for(k = 0; k < 4; k++)
        {
            setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, red,green, blue);
            setRGB(img, tempX, tempY, rowSize, pixelHeight, red,green, blue);
          if(maze[j][i] == 9)
          {
            tempX -= 1;
          }
          else
          {
            tempX += 1;
          }
        }
        if(maze[j][i] != 9)
        {
            setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, red,green, blue);
            setRGB(img, tempX, tempY, rowSize, pixelHeight, red,green, blue);
          for(k = 0; k < 8; k++)
          {
              setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, red,green, blue);
              setRGB(img, tempX, tempY, rowSize, pixelHeight, red,green, blue);
            tempX -= 1;
          }
        }
        for(r = 0; r < 2; r++)
        {
            setRGB(img, tempX, tempY - 1, rowSize, pixelHeight, red,green, blue);
            setRGB(img, tempX, tempY, rowSize, pixelHeight, red,green, blue);
          tempX -= 1;
        }
        if(maze[j][i] == 15)
        {
          for(k = 0; k < 5; k++)
          {
              setRGB(img, tempX + 6, tempY, rowSize, pixelHeight, red,green, blue);
              setRGB(img, tempX + 5, tempY, rowSize, pixelHeight, red,green, blue);
            tempY -= 1;
          }
        }
      }
      // Pipe 12
      else if(maze[j][i] == 12)
      {
        tempY += 4;
        for(k = 0; k < 4; k++)
        {
          setRGB(img, tempX -1, tempY - 1, rowSize, pixelHeight, red,green, blue);
          setRGB(img, tempX -1, tempY, rowSize, pixelHeight, red,green, blue);
          tempX += 1;
        }
        setRGB(img, tempX -1, tempY - 1, rowSize, pixelHeight, red,green, blue);
        setRGB(img, tempX -1, tempY, rowSize, pixelHeight, red,green, blue);
        for(k = 0; k < 3; k++)
        {
          setRGB(img, tempX - 1, tempY, rowSize, pixelHeight, red,green, blue);
          setRGB(img, tempX, tempY, rowSize, pixelHeight, red,green, blue);
          tempY += 1;
        }
        setRGB(img, tempX - 1, tempY, rowSize, pixelHeight, red,green, blue);
        setRGB(img, tempX, tempY, rowSize, pixelHeight, red,green, blue);
      }
    }
  }
  // Makes BMP file 
  FILE* myFile = fopen("geretatedMaze.bmp", "wb");
  fwrite(header, 1, sizeof(header), myFile);
  fwrite(img, 1, sizeof(img), myFile);
  fclose(myFile); 
 /*   
    int pixelWidth = 16;
    int pixelHeight = 16;
    
    FILE *pipe1Pt = fopen("Pipe-1.bmp","wr");
    FILE *pipe2Pt = fopen("Pipe-2.bmp","wr");
    
    FILE* myFilePt = fopen("myPic.bmp", "wb");
    
    int finalTotalData = gheight*gwidth*pixelHeight*pixelWidth*3 + 54;
    int totalData = pixelWidth*pixelHeight*3;
    
    unsigned char img[finalTotalData];
    unsigned char pipe1[totalData];
    
    //fread is for binnary
    fseek(pipe1Pt,52,SEEK_SET);
    fread(pipe1, totalData, 1, pipe1Pt);
    fwrite(header, 1,sizeof(header), myFilePt);

    for(int i = 0; i < totalData -1; i ++ )
    {
        img[i] = pipe1[i];
    }
    fwrite(img, 1, sizeof(img), myFilePt);

    fclose(pipe1Pt);
    fclose(pipe2Pt);
    fclose(myFilePt);
   */  
}
/*
 * Carves the maze my looking at random directions available from
 * dirFinder. if there is a spot the it will head that way and
 * and values at the spot. also calls reverseDirection
 * */
int carvemaze(int x, int y, int dir)
{ 
    mazePrint();
    int randDirection;   
    while(TOTALELEMENTS > 0)
    {  
        if ( y == 0 && hitEntranceCheck == FALSE)
        {   
            startPosition = x;
            maze[x][y] += DIRECTION_LIST[dir];
            maze[x][y] |= 1;
            hitEntranceCheck = TRUE;
            x = galleyWaySouthX;
            y = galleyWaySouthY;
        }
        else
        {  
            randDirection = dirFinder(x,y);
            if( randDirection != 4)
            {
                int newDir = reverseDir(randDirection);
                if(maze[x][y] == 0)
                {
                  //  if(gwayPointAlleyLength != 0)
                  //  {
                    maze[x][y] = DIRECTION_LIST[dir] + DIRECTION_LIST[randDirection];
                  //  }
                }
                else if(maze[x][y] != 0)
                {
                maze[x][y] += DIRECTION_LIST[randDirection];
                }
                x = x + DIRECTION_DX[randDirection];
                y = y + DIRECTION_DY[randDirection];
////            newDir = reverseDir(randDirection); 
                TOTALELEMENTS--;
                int randomWayPointX = rand()%4;
                int randomWayPointY = rand()%4;
               // x = randomWayPointX;
                carvemaze(x,y,newDir);
            }
            else
            {
                if(maze[x][y] == 0)
                {
                    maze[x][y] = DIRECTION_LIST[dir];
                }
                return 0;
            }
        }
    }
maze[gwayPointX][gwayPointY] |= 15;
}
/*
 *Solves the maze by checking the possible directions to go to
 if there is a place to head to the method calls itseeld.
*/
int mazeSolveIt(int x, int y, int dir)
{
    int randomDirection;     
    isSolving = TRUE;
    int mask = GOAL + VISITED;
    while(isSolving)
    { 
        maze[x][y] |= mask;
        randomDirection = dirFinder(x,y);
        if ((x == endPosition) && ( y == (gheight -1)))
        {
            foundExit = TRUE;
            isSolving = FALSE;
            break;
          //  return 0;
        }
        if( randomDirection == 4)
        {
            maze[x][y] -= GOAL;
            return 0;
        }
        int x2 = x + DIRECTION_DX[randomDirection];
        int y2 = y + DIRECTION_DY[randomDirection];
        mazeSolveIt(x2, y2,reverseDir(randomDirection));
        if(foundExit == FALSE && (maze[x][y] & GOAL))
        {
            maze[x][y] -= GOAL;
        }
    }
}
/*
 * Solves the maze by calling a helper method.
 * */
void mazeSolve()
{
    solveMazeCheck = TRUE;
    mazeSolveIt(startPosition, 0, NORTH);
}
/*
 * carves the alleys for the maze.
 * in this function I create a for loop for each direction
 * I check if it passes the bounds test.
 * if it does I add the prooper pipe into that position.
 * if it doesnt I go back one step and add an endPipe towards the
 * opposite side it was carving and exit.
*/
void carveAlleyLength(int x, int y)
{
    if (gwayPointAlleyLength != 0)
    {   
    int north,east,south,west;
    //CARVING UP
    for(north = 1; north < gwayPointAlleyLength + 1; north++)
    {
        if( boundTest(x, y - north))
        {
            maze[x][y - north] |= SPECIAL;	
            maze[x][y - north] |= NORTH | SOUTH;
            galleyWayX[0] = x;
            galleyWayY[0] = y - north;
            galleyWayNorthX = x;
            galleyWayNorthY = y - north;
        }
        else 
        {
            maze[x][y - north + 1] |= SPECIAL;
            maze[x][y - north + 1] += SOUTH;
            galleyWayX[0] = x;
            galleyWayY[0] = y - north;
            galleyWayNorthX = x;
            galleyWayNorthY = y - north + 1;
            alleyWayNorth = y - north + 1;
            break;
        }
        alleyWayNorth = y - north + 1;
    }
    maze[x][y - north + 1] |= SPECIAL;
    maze[x][y - north + 1] = SOUTH;
    //CARVING DOWN
    for( south = 1; south < gwayPointAlleyLength + 1; south++)
    {
        if(boundTest(x, y+south))
        {
            maze[x][y+south] |= SPECIAL;
            maze[x][y+south] += NORTH | SOUTH;
        }
        else 
        {
            maze[x][y + south - 1] |= SPECIAL;
            maze[x][y + south - 1] += NORTH;
            galleyWaySouthX = x;
            galleyWaySouthY = y + south - 1;
            alleyWaySouth = y + south - 1;
            break;
        }
        galleyWaySouthX = x;
        galleyWaySouthY = y + south - 1;
        alleyWaySouth = y + south - 1;
    }
    maze[x][y + south - 1] |= SPECIAL;
    maze[x][y + south - 1] = NORTH;
    // CARVING WEST
    for(west = 1; west < gwayPointAlleyLength + 1; west ++)
    {
        if(boundTest(x - east, y))
        {
            maze[x - west ][y] |= SPECIAL;
            maze[x - west ][y] += WEST | EAST;
        }
        else 
        {
            maze[x - west + 1][y] |= SPECIAL;
            maze[x - west +1][y] += EAST;
            galleyWayWestX = x;
            galleyWayWestY = y - west + 1;
            alleyWayWest = x - west + 1;
            break;
        }
        galleyWayWestX = x;
        galleyWayWestY = y - west + 1;
        alleyWayWest = x - west + 1;
    }
    maze[x - west + 1 ][y] |= SPECIAL;
    maze[x - west + 1][y] = EAST;
    //CARVING EAST
    for( east =1; east < gwayPointAlleyLength + 1; east ++)
    {
        if(boundTest(x+ east, y))
        {
            maze[x + east][y] |= SPECIAL;
            maze[x + east][y] += WEST | EAST;
        }
        else 
        {
            maze[x + east - 1][y] |= SPECIAL;
            maze[x+east -1][y] += WEST;
            galleyWayEastX = x;
            galleyWayEastY = y + east - 1;
            alleyWayEast = x + east - 1;
            break;
        }
        galleyWayEastX = x;
        galleyWayEastY = y + east - 1;
        alleyWayEast = x + east -1;
    }
    maze[x + east - 1][y] |= SPECIAL;
    maze[x + east - 1][y] = WEST;
    }
    else 
    {
        galleyWayNorthX = x;
        galleyWayNorthY = y;
        galleyWayEastX = x;
        galleyWayEastY = y;
        galleyWaySouthX = x;
        galleyWaySouthY = y;
        galleyWayWestX = x;
        galleyWayWestY = y;
        maze[x][y] |= SPECIAL;
    }
}
/*
 *method frees up the memory used my the maze each time a new maze
 is created. If its called twice in a row then it does nothing.
*/
void mazeFree()
{
    if(freeCheck == 1)
    {
        return;
    }
    else 
    {
        for(int x = 0; x < gheight; x++)
        {
            free(maze[x]);
        }
        free(maze);
        freeCheck = 0;
    }
}
/*
 * Generates the maze. 
 * Fress up memory then it checks the values of the width and the height.
 * it will then allocate memory for the maze.
 * Sets global varies used in other methods.
 * Calls CarveMaze.
*/
int mazeGenerate(int width, int height, int wayPointX, int wayPointY,
    int wayPointAlleyLength,
    double wayPointDirectionPercent, 
    double straightProbability, int printAlgorithmSteps)
{

    mazeFree();
    freeCheck = 1;
    /* Validate the length of the maze*/
    if ( width < 3 || height < 3 ) 
    {
        fprintf(stdout, "Error: width or Height is less than 3\n");
        return 1;
    }
    /* Allocate the maze array*/
    // original width *
    maze = malloc(width * sizeof(*maze));
    int x;
    for(x = 0; x < width; x++)
    {
        // original height *
        maze[x] = malloc(height * sizeof(int));
    }
    if(maze == NULL)
    {
        fprintf(stdout, "Error: not enough memory\n");
        return 1;
    }  
    for(int x =0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            maze[x][y] =  0;
        }
    }
    if( wayPointX - 1 <= 0 || wayPointX - 1 > width -1  ||
        wayPointY - 1 <= 0 || wayPointY - 1> height-1)
    {
        fprintf(stdout, "Error: way point is out of bounds\n");
    }
    
    hitEntranceCheck = FALSE;
    TOTALELEMENTS = width * height;
    gheight = height;
    gwidth = width;
    gprintAlgorithmSteps = printAlgorithmSteps;
    gwayPointX = wayPointX - 1;
    gwayPointY = wayPointY - 1;    
    gwayPointAlleyLength = wayPointAlleyLength;
    gwayPointDirectionPercent = wayPointDirectionPercent; 
    gmaxCellsToCarve = width * height * gwayPointDirectionPercent;
    maze[gwayPointX][gwayPointY] |= 15;
    maze[gwayPointX][gwayPointY] |= SPECIAL;
    carveAlleyLength(gwayPointX, gwayPointY); 
    startPosition = rand()%width;
   // randomAlley = rand()%4;
    // if recursion based on the alleyways is the case then TRUE
    alleyCarveCheck = TRUE;
 //   if (alleyCarveCheck == TRUE) TOTALELEMENTS = gmaxCellsToCarve;
  //  printf("%f = \n", gmaxCellsToCarve);
   // printf("(%d,%d)\n", galleyWayNorthX, galleyWayNorthY);
    int alleyCarveNorth = carvemaze(galleyWayNorthX,galleyWayNorthY ,0);
 //   int complete = carvemaze(startPosition,0,0);
    endPosition = rand()%width;
    hitEntranceCheck = FALSE; 
    maze[endPosition][height -1] += 4;    
    return 0;
}

