//============================================================================
// Name        : cookie-crush.cpp
// Author      : Sibt ul Hussain
// Version     :
// Copyright   : (c) Reserved
// Description : Basic 2D game of Cookie  Crush...
//============================================================================
#ifndef WORD_SHOOTER_CPP
#define WORD_SHOOTER_CPP

// #include <GL/gl.h>
// #include <GL/glut.h>
#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include "util.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
using namespace std;
#define MAX(A, B) ((A) > (B) ? (A) : (B)) // defining single line functions....
#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define ABS(A) ((A) < (0) ? -(A) : (A))
#define FPS 60

string *dictionary;
int dictionarysize = 369646;
#define KEY_ESC 27 // A

// 20,30,30
const int bradius = 30; // ball radius in pixels...
int width = 930, height = 660;
int currentChar = 0, nextChar = 0;
int remainingTime = 120, framesDrawn = 0;
int posX = (width / 2), posY = 10, cellPosX = 0, cellPosY = 0, checkPosX = 0, checkPosY = 0, checkCellX = 0, checkCellY = 0;
int clickPosX = 0, clickPosY = 0;
int byoffset = bradius;
bool leftClicked = false, gameOver = false;
int nxcells = (width - bradius) / (2 * bradius);
int nycells = (height - byoffset /*- bradius*/) / (2 * bradius); 
int xCell = 0, yCell = 0;
int nfrows = 2; // initially number of full rows //
int filledRows = nfrows;
float score = 0;
int **board; // 2D-arrays for holding the data...
int bwidth = 130;
int bheight = 10;
int bsx, bsy;
const int nalphabets = 26;
enum alphabets
{
	AL_A,
	AL_B,
	AL_C,
	AL_D,
	AL_E,
	AL_F,
	AL_G,
	AL_H,
	AL_I,
	AL_J,
	AL_K,
	AL_L,
	AL_M,
	AL_N,
	AL_O,
	AL_P,
	AL_Q,
	AL_R,
	AL_S,
	AL_T,
	AL_U,
	AL_W,
	AL_X,
	AL_y,
	AL_Z
};
GLuint texture[nalphabets];
GLuint tid[nalphabets];
string tnames[] = {"a.bmp", "b.bmp", "c.bmp", "d.bmp", "e.bmp", "f.bmp", "g.bmp", "h.bmp", "i.bmp", "j.bmp",
				   "k.bmp", "l.bmp", "m.bmp", "n.bmp", "o.bmp", "p.bmp", "q.bmp", "r.bmp", "s.bmp", "t.bmp", "u.bmp", "v.bmp", "w.bmp",
				   "x.bmp", "y.bmp", "z.bmp"};
GLuint mtid[nalphabets];
int awidth = 60, aheight = 60; // 60x60 pixels cookies...

ALuint source;
ALuint buffer;

// USED THIS CODE FOR WRITING THE IMAGES TO .bin FILE
void RegisterTextures_Write()
// Function is used to load the textures from the
//  files and display
{
	// allocate a texture name
	glGenTextures(nalphabets, tid);
	vector<unsigned char> data;
	ofstream ofile("image-data.bin", ios::binary | ios::out);
	// now load each cookies data...

	for (int i = 0; i < nalphabets; ++i)
	{

		// Read current cookie

		ReadImage(tnames[i], data);
		if (i == 0)
		{
			int length = data.size();
			ofile.write((char *)&length, sizeof(int));
		}
		ofile.write((char *)&data[0], sizeof(char) * data.size());

		mtid[i] = tid[i];
		// select our current texture
		glBindTexture(GL_TEXTURE_2D, tid[i]);

		// select modulate to mix texture with color for shading
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// when texture area is small, bilinear filter the closest MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
						GL_LINEAR_MIPMAP_NEAREST);
		// when texture area is large, bilinear filter the first MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if wrap is true, the texture wraps over at the edges (repeat)
		//       ... false, the texture ends at the edges (clamp)
		bool wrap = true;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
						wrap ? GL_REPEAT : GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
						wrap ? GL_REPEAT : GL_CLAMP);

		// build our texture MIP maps
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, awidth, aheight, GL_RGB,
						  GL_UNSIGNED_BYTE, &data[0]);
	}
	ofile.close();
}
void RegisterTextures()
/*Function is used to load the textures from the
 * files and display*/
{
	// allocate a texture name
	glGenTextures(nalphabets, tid);

	vector<unsigned char> data;
	ifstream ifile("image-data.bin", ios::binary | ios::in);

	if (!ifile)
	{
		cout << " Couldn't Read the Image Data file ";
		// exit(-1);
	}
	// now load each cookies data...
	int length;
	ifile.read((char *)&length, sizeof(int));
	data.resize(length, 0);
	for (int i = 0; i < nalphabets; ++i)
	{
		// Read current cookie
		// ReadImage(tnames[i], data);
		/*if (i == 0) {
		int length = data.size();
		ofile.write((char*) &length, sizeof(int));
		}*/
		ifile.read((char *)&data[0], sizeof(char) * length);

		mtid[i] = tid[i];
		// select our current texture
		glBindTexture(GL_TEXTURE_2D, tid[i]);

		// select modulate to mix texture with color for shading
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// when texture area is small, bilinear filter the closest MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
						GL_LINEAR_MIPMAP_NEAREST);
		// when texture area is large, bilinear filter the first MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if wrap is true, the texture wraps over at the edges (repeat)
		//       ... false, the texture ends at the edges (clamp)
		bool wrap = true;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
						wrap ? GL_REPEAT : GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
						wrap ? GL_REPEAT : GL_CLAMP);

		// build our texture MIP maps
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, awidth, aheight, GL_RGB,
						  GL_UNSIGNED_BYTE, &data[0]);
	}
	ifile.close();
}
void DrawAlphabet(const alphabets &cname, int sx, int sy, int cwidth = 60,
				  int cheight = 60)
/*Draws a specfic cookie at given position coordinate
 * sx = position of x-axis from left-bottom
 * sy = position of y-axis from left-bottom
 * cwidth= width of displayed cookie in pixels
 * cheight= height of displayed cookiei pixels.
 * */
{	
	float fwidth = (float)cwidth / width * 2, fheight = (float)cheight / height * 2;
	float fx = (float)sx / width * 2 - 1, fy = (float)sy / height * 2 - 1;

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mtid[cname]);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(fx, fy);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(fx + fwidth, fy);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(fx + fwidth, fy + fheight);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(fx, fy + fheight);
	glEnd();

	glColor4f(1, 1, 1, 1);

	//	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	// glutSwapBuffers();
}
int GetAlphabet()
{
	return GetRandInRange(1, 26);
}

void Pixels2Cell(int px, int py, int & cx, int &cy) {
	cx = (px / 60);
	cy = ((height - py - 1 - 20) / 60) + 1;
}

void Cell2Pixels(int cx, int cy, int & px, int &py){
	px = (cx * awidth);
	py = (cy * aheight);
}
void DrawShooter(int sx, int sy, int cwidth = 60, int cheight = 60)

{
	float fwidth = (float)cwidth / width * 2, fheight = (float)cheight / height * 2;
	float fx = (float)sx / width * 2 - 1, fy = (float)sy / height * 2 - 1;

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, -1);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(fx, fy);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(fx + fwidth, fy);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(fx + fwidth, fy + fheight);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(fx, fy + fheight);
	glEnd();

	glColor4f(1, 1, 1, 1);

	//	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	// glutSwapBuffers();
}
/*
 * Main Canvas drawing function.
 * */

void DisplayFunction()
{
	// set the background color using function glClearColor.
	// to change the background play with the red, green and blue values below.
	// Note that r, g and b values must be in the range [0,1] where 0 means dim red and 1 means pure red and so on.
	// #if 0
	glClearColor(1 /*Red Component*/, 1.0 /*Green Component*/,
				 1.0 /*Blue Component*/, 0 /*Alpha component*/); // Red==Green==Blue==1 --> White Colour
	glClear(GL_COLOR_BUFFER_BIT);								 // Update the colors

	// write your drawing commands here or call your drawing functions...
	for (int i = 0; i < nycells; i++)
	{
		for (int j = 0; j < nxcells; j++)
		{
			if (board[i][j] != -1)
				DrawAlphabet((alphabets)board[i][j], (10 + (j * awidth)), (570 - (i * aheight)), awidth, aheight);
		}
	}

	DrawAlphabet((alphabets)currentChar, posX, posY, awidth, aheight);
	DrawAlphabet((alphabets)nextChar, (width - (width / 8)), 10 , awidth, aheight);


	if (leftClicked == true)
	{
		Pixels2Cell(posX, posY, checkCellX, checkCellY);

		if (board[checkCellY - 1][checkCellX] != -1)
		{
			leftClicked = false;
			Pixels2Cell(posX, posY, xCell, yCell);
			cout << "xCell: " << xCell << " yCell: " << yCell << endl;
			board[yCell][xCell] = currentChar;
			currentChar = nextChar;
			nextChar = GetAlphabet();
			posX = (width / 2);
			posY = 10;
		}
		else
		{
			cout << "ClickPosX: " << clickPosX << " ClickPosY: " << clickPosY << " posX: " << posX << " PosY: " << posY << endl;

			//dividing by FPS makes it so that the change in position is split up frame-by-frame to keep it smooth
			posX = (posX + (clickPosX/FPS));
			posY = (posY + (clickPosY/FPS));

			//boundary checks, negating it makes it so that the ball bounces off the wall as the value being "added" changes direction (subtracting the change moves to left, adding the change moves to right, behavior must change at boundaries)
			if ((posX <= 0) || (posX >= (width - 1 - 60)))
				clickPosX = -(clickPosX);
		}


	}

	DrawString(5, height - 20, width, height + 5, "Score " + Num2Str(score), colors[BLUE_VIOLET]);
	DrawString((width / 2) - 85, height - 25, width, height, "Time Left:" + Num2Str(remainingTime) + " secs", colors[RED]);
	DrawString(width - 225, height - 20, width, height + 5, "Aarab Malik 24i-2552", colors[BLUE_VIOLET]);
			   

	// #----------------- Write your code till here ----------------------------#
	// DO NOT MODIFY THESE LINES
	DrawShooter((width / 2) - 35, 0, bwidth, bheight);
	glutSwapBuffers();
	// DO NOT MODIFY THESE LINES..
}

/* Function sets canvas size (drawing area) in pixels...
 *  that is what dimensions (x and y) your game will have
 *  Note that the bottom-left coordinate has value (0,0) and top-right coordinate has value (width-1,height-1)
 * */
void SetCanvasSize(int width, int height)
{
	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1); // set the screen size to given width and height.*/
}

/*This function is called (automatically) whenever any non-printable key (such as up-arrow, down-arraw)
 * is pressed from the keyboard
 *
 * You will have to add the necessary code here when the arrow keys are pressed or any other key is pressed...
 *
 * This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
 * program coordinates of mouse pointer when key was pressed.
 *
 * */

void NonPrintableKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT /*GLUT_KEY_LEFT is constant and contains ASCII for left arrow key*/)
	{
		// what to do when left key is pressed...
	}
	else if (key == GLUT_KEY_RIGHT /*GLUT_KEY_RIGHT is constant and contains ASCII for right arrow key*/)
	{
	}
	else if (key == GLUT_KEY_UP /*GLUT_KEY_UP is constant and contains ASCII for up arrow key*/)
	{
	}
	else if (key == GLUT_KEY_DOWN /*GLUT_KEY_DOWN is constant and contains ASCII for down arrow key*/)
	{
	}

	/* This function calls the Display function to redo the drawing. Whenever you need to redraw just call
	 * this function*/
	/*
	glutPostRedisplay();
	*/
}
/*This function is called (automatically) whenever your mouse moves witin inside the game window
 *
 * You will have to add the necessary code here for finding the direction of shooting
 *
 * This function has two arguments: x & y that tells the coordinate of current position of move mouse
 *
 * */

void MouseMoved(int x, int y)
{
	// If mouse pressed then check than swap the balls and if after swaping balls dont brust then reswap the balls
}

/*This function is called (automatically) whenever your mouse button is clicked witin inside the game window
 *
 * You will have to add the necessary code here for shooting, etc.
 *
 * This function has four arguments: button (Left, Middle or Right), state (button is pressed or released),
 * x & y that tells the coordinate of current position of move mouse
 *
 * */

void MouseClicked(int button, int state, int x, int y)
{

	if (button == GLUT_LEFT_BUTTON) // dealing only with left button
	{
		//x and y are calculated w.r.t the top left of the canvas, so these need to be converted to be w.r.t bottom left to keep them in line with the sketch canvas pixel coordinates
		if ((state == GLUT_UP) && (leftClicked != true))
		{
			leftClicked = true;
			//(subtracting (with / 2) so that clickPosX is w.r.t the ball in the middle of the screen, -30 to compensate for the ball radius)
			clickPosX = (x - (width / 2) - 30);
			//subtracted height to make the clickPosY w.r.t the bottom of the screen, -30 to compenste for the ball radius
			clickPosY = (height - y - 30);
		}
	}
	else if (button == GLUT_RIGHT_BUTTON) // dealing with right button
	{
	}
	glutPostRedisplay();
}
/*This function is called (automatically) whenever any printable key (such as x,b, enter, etc.)
 * is pressed from the keyboard
 * This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
 * program coordinates of mouse pointer when key was pressed.
 * */
void PrintableKeys(unsigned char key, int x, int y)
{
	if (key == KEY_ESC /* Escape key ASCII*/)
	{
		exit(1); // exit the program when escape key is pressed.
	}
}

/*
 * This function is called after every 1000.0/FPS milliseconds
 * (FPS is defined on in the beginning).
 * You can use this function to animate objects and control the
 * speed of different moving objects by varying the constant FPS.
 *
 * */
void Timer(int m)
{
	if (gameOver == false)
	{
		framesDrawn++;
		if ((framesDrawn % FPS) == 0)
		{	
			remainingTime--;
			if (remainingTime == 0)
				gameOver = true;
		}
	}

	glutPostRedisplay();

	glutTimerFunc(1000.0 / FPS, Timer, 0);
}

void DefineBoard(int ** &board) {
    
    board = new int *[14];
    for (int i = 0; i < 14; i++) {
        
        board[i] = new int[15];
    }

	for (int i = 0; i < nfrows; i++)
	{
        for (int j = 0; j < 15; j++) {
            board[i][j] = GetAlphabet();
        }
	}

	for (int i = nfrows; i < 14; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			board[i][j] = -1;
		}
	}

	currentChar = GetAlphabet();
	nextChar = GetAlphabet();
}

void DeleteBoard(int ** &board) {
	for (int i = 0; i < nfrows; i++) {
		delete board[i];
	}
	delete[] board;
}

void InitMusic() {
    alutInit(0, NULL);
    
    buffer = alutCreateBufferFromFile("backgroundMusic.wav");
    
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);
    alSourcei(source, AL_LOOPING, AL_TRUE);
    
    alSourcePlay(source);
}

/*
 * our gateway main function
 * */
int main(int argc, char *argv[])
{
	InitRandomizer(); // seed the random number generator...

	// Dictionary for matching the words. It contains the 370099 words.
	dictionary = new string[dictionarysize];
	ReadWords("words_alpha.txt", dictionary); // dictionary is an array of strings
	// print first 5 words from the dictionary
	for (int i = 0; i < 5; ++i)
		cout << " word " << i << " =" << dictionary[i] << endl;

	cout << "nx cells: " << nxcells;


	// Write your code here for filling the canvas with different Alphabets. You can use the Getalphabet function for getting the random alphabets
	DefineBoard(board);
	InitMusic();

	glutInit(&argc, argv);						  // initialize the graphics library...
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // we will be using color display mode
	glutInitWindowPosition(50, 50);				  // set the initial position of our window
	glutInitWindowSize(width, height);			  // set the size of our window
	glutCreateWindow("ITCs Word Shooter");		  // set the title of our game window
	// SetCanvasSize(width, height); // set the number of pixels...

	// Register your functions to the library,
	// you are telling the library names of function to call for different tasks.
	RegisterTextures();
	glutDisplayFunc(DisplayFunction);  // tell library which function to call for drawing Canvas.
	glutSpecialFunc(NonPrintableKeys); // tell library which function to call for non-printable ASCII characters
	glutKeyboardFunc(PrintableKeys);   // tell library which function to call for printable ASCII characters
	glutMouseFunc(MouseClicked);
	glutPassiveMotionFunc(MouseMoved); // Mouse

	//// This function tells the library to call our Timer function after 1000.0/FPS milliseconds...
	glutTimerFunc(1000.0 / FPS, Timer, 0);

	//// now handle the control to library and it will call our registered functions when
	//// it deems necessary...

	glutMainLoop();

	// Add cleanup before return
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    alutExit();
	DeleteBoard(board);
	return 1;
}
#endif /* */
