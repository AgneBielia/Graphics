/* CMP-5010B Graphics 1 
   Design and Implement a Two Dimensional Interactive Platform Game using C++ and OpenGL. 
   May 2019
 */

#include <windows.h>		// Header File For Windows
// for texture
#include "Image_Loading/nvImage.h"

#include <iostream>
#include <stdio.h>
#include <string> 
using namespace std;
#include <math.h>			// Header File For Maths Functions
#include <time.h>			// Header File For Clock Functions and Variables
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library

#include "glfont2.h"
using namespace glfont;

int jump = 0;

// seconds and minutes
int secs = 0;
int mins = 0;


int score = 0;			// score (counts the number of items player collects)
int lives = 8;			// Player health (9 lives)

// Handles to the textures
GLuint cat = 0; GLuint cat1 = 0; GLuint cat2 = 0; GLuint cat3 = 0; GLuint cat4 = 0;		// walking textures
GLuint happy = 0; GLuint angry = 0; // wasn't used 
GLuint jump1 = 0; GLuint jump2 = 0;														// jump textures
GLuint platform = 0; GLuint mPlatform = 0; GLuint background = 0;						//fixed platform, moving platform and background texture 
GLuint bird0 = 0; GLuint bird1 = 0;														// item to collect - bird, textures
GLuint enemy = 0;																		// enemy - white cate
GLuint heartTex = 0; GLuint blueHeart = 0;												// hearts textures for health and start screen

//glFont
GLFont myfont;

/* frames for handling player walk animation */
int frame = 0;

int	mouse_x = 0, mouse_y = 0;
// starts the simulation is variable is true
bool startSimulation = false;
// starts the fall if true
bool startFall = false;
// true when player touches the top of the platform
bool touchingPlatform = false;

// total of items for player to collect in the game env.
int totalColl = 7;

// OpenGL Window size
int screenWidth = 900, screenHeight = 600;

// Game environment size
int gameWidth = 60;
int gameHeight = 20;

// OpenGL Window Position
int xPos = 70, yPos = 50;

// keyboard keys
bool keys[256];
// keys pressed

bool done = false;

// true when the enemy object is touched, used to handle player reaction to the collision
// and changing player health
bool touchedEnemy = false;

//is player moving left
bool pLeft = false;

// Stores game environment array (game1) row and column numbers
int gamePlatformX = 0;
int gamePlatformY = 0;

// size of the side of the square
float tile = screenWidth / screenHeight;
// size of half of the side of the square or radius of the circle
float side = tile / 2;

/***** User controlled object  ****/
float Xtri = screenWidth / 2;                           //x coords of the user controlled object
float playerYpos = screenHeight / 2 + tile;				// y coords of the user control object

const float PI = 3.14159265359;
// s1 Jump, s2 and v2 is for both   
static float s1, s2, v2;

// Simulation settings
GLint choice = 0;

bool LeftPressed = false;

const float HDist = -side;

// Timer 
// convert clock ticks to time increment suitable for simulation constants
double timerFrequencyRecip = 0.000003;  // Only needs to be changed to change speed of simulation but is platform independent
										// Smaller values will slow down the simulation, larger values will speed it up

__int64 prevTime = 0;				// Previous count


//OPENGL FUNCTION PROTOTYPES
void display();				//called in winmain to draw everything to the screen
void reshape();				//called when the window is resized
void init();				//called in winmain when the program starts.
void processKeys();         //called in winmain to process keyboard input
void update();				//called in winmain to update variables
float getDistance(float x1, float y1, float x2, float y2); // calculates the distance between two objects
void collision(float x, float y); // handles collisions between objects and the player
void timer();                     // timer
void loadSquare(int i, int j);    // template for drawing objects(squares) with texture in game environment 
void reverseLoadSquare(int i, int j); //template for drawing objects (squares) with texture flipped vertically
void checkSpace(float x, float y);  // for handling player collision with empty space (0) in the game environment
void displayScore();				// displays timer (does not work correctly), and score (items collected)
void menu();						// start screen

/********************************************** Game environment blueprint **************************************************/
int game1[20][60] = {
	{ 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2 },
	{ 2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,2,2,3,3,3,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,3,3,3,0,0,0,0,0,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0 },
	{ 2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 2,2,2,2,2,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,2,2,2,2,2,2,2 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,2,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,4,0,0,0,0,0 },
	{ 0,0,0,0,0,6,6,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,2,2,2,2,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,0,0,0,0,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2 },
	{ 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2 }
};

// player health
int heart[9] = { 1,1,1,1,1,1,1,1,1 };
/*************    START OF OPENGL FUNCTIONS   ****************/

/* Player */
void drawPlayer(float side, float cx, float cy, int ngons)
{

	float x, y;
	float frac = 2 * PI / ngons;
	// jump animation
	if (jump == 1) {
		glBindTexture(GL_TEXTURE_2D, jump1);
	}
	if (jump == 2||startFall) {
		glBindTexture(GL_TEXTURE_2D, jump2);
	}
	// walk animation
	if (!startFall && !startSimulation) {
		if (frame == 0) {
			glBindTexture(GL_TEXTURE_2D, cat);
		}
		else if (frame == 1) {
			glBindTexture(GL_TEXTURE_2D, cat1);
		}
		else if (frame == 2) {
			glBindTexture(GL_TEXTURE_2D, cat2);
		}
		else if (frame == 3) {
			glBindTexture(GL_TEXTURE_2D, cat3);
		}
		else if (frame == 4) {
			glBindTexture(GL_TEXTURE_2D, cat4);
		}
	}
	
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// blends the background of the character
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// small single character
	if (pLeft) {
		glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0); glVertex2f(cx - side, cy - side);
		glTexCoord2f(0.0, 1.0); glVertex2f(cx - side, cy + side);
		glTexCoord2f(1.0, 1.0); glVertex2f(cx + side, cy + side);
		glTexCoord2f(1.0, 0.0); glVertex2f(cx + side, cy - side);
		glEnd();
	}
	else if(!pLeft){
		glBegin(GL_POLYGON);
		glTexCoord2f(1.0, 0.0); glVertex2f(cx - side, cy - side);
		glTexCoord2f(1.0, 1.0); glVertex2f(cx - side, cy + side);
		glTexCoord2f(0.0, 1.0);  glVertex2f(cx + side, cy + side);
		glTexCoord2f(0.0, 0.0);  glVertex2f(cx + side, cy - side);
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

}

/* Game environment */
void drawScene() {

	glBindTexture(GL_TEXTURE_2D, background);

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	//// blends the background of the character
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex2f(0, 0);
	glTexCoord2f(0.0, 1.0); glVertex2f(0, screenHeight);
	glTexCoord2f(1.0, 1.0); glVertex2f(screenWidth, screenHeight);
	glTexCoord2f(1.0, 0.0); glVertex2f(screenWidth, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	int gameWidth = 60;
	int gameHeight = 20;
	// tile height and width
	tile = screenHeight / gameHeight;

	for (int i = 0; i < gameHeight; i++) {

		for (int j = 0; j < gameWidth; j++) {

			/* x and y coords of the center of the current platform sqaure */
			float  platformPosx = (j + 0.5)*tile;
			float platformPosy = screenHeight - (i + 0.5)*tile;
			float d = getDistance(Xtri, s2, platformPosx, platformPosy);
			glPushMatrix();

			// when the right side screen limit is reached
			if (Xtri > gameWidth*tile - screenWidth / 2) {
				glTranslatef(-(gameWidth*tile - screenWidth), 0.0, 0.0);

			}
			// when the left side screen limit is reached
			else if (Xtri <= screenWidth / 2) {
				glTranslatef(0.0, 0.0, 0.0);

			}
			// middle, move the whole scene instead
			else {
				glTranslatef(-Xtri + screenWidth / 2, 0.0, 0.0);
			}

			// for performing fall when player goes over the platform
			if (game1[i][j] == 0) {
				checkSpace(platformPosx, platformPosy);
				gamePlatformX = j;
				gamePlatformY = i;
			}

			// fixed platform
			if (game1[i][j] == 2) {
				glPushMatrix();
				glBindTexture(GL_TEXTURE_2D, platform);
				collision(platformPosx, platformPosy);

				loadSquare(i, j);
				glPopMatrix();
			}
			// moving platform
			if (game1[i][j] == 3) {
				timer();
				// to change movement of the platform, 5 represents the number of tiles it's going to move

				/* Platform moving obliquely */
				/*glTranslatef(secs%5*tile, secs % 5 * tile, 0.0);
				collision(platformPosx+ secs % 5 * tile, platformPosy+ secs % 5 * tile);*/

				/* platform moving horizontally */

				glTranslatef(secs % 5 * tile, 0.0, 0.0);
				collision(platformPosx + secs % 5 * tile, platformPosy);

				glBindTexture(GL_TEXTURE_2D, mPlatform);
				loadSquare(i, j);

			}
			/* Vertically moving platform */
			if (game1[i][j] == 6) {
				
				glTranslatef(0.0, secs % 6 * tile, 0.0);
				collision(platformPosx, platformPosy + secs % 6 * tile);

				glBindTexture(GL_TEXTURE_2D, mPlatform);
				loadSquare(i, j);
			}

			/* objects for player to collect */
			if (game1[i][j] == 4) {
				
				if (d <= tile) {
					game1[i][j] = 0;
				}
				if (game1[i][j] == 0) {
					score += 1;
				}
				timer();

				if (secs % 1 == 0) {
					glBindTexture(GL_TEXTURE_2D, bird0);
				}
				if (secs % 2 == 0) {
					glBindTexture(GL_TEXTURE_2D, bird1);
				}

				loadSquare(i, j);

			}
			/* Enemy/objects for player to avoid */
			if (game1[i][j] == 5) {

				gamePlatformX = j;
				gamePlatformY = i;

				// removes some health after colliding
				if (d <= tile) {
					heart[lives] = 0;
					touchedEnemy = true;
				}
				if (heart[lives] == 0 && lives != 0) {
					lives = lives - 1;

				}
				
				collision(platformPosx + secs % 2 * tile, platformPosy);
				// moves on the same 2 tiles
				glTranslatef(secs % 2 * tile, 0.0, 0.0);
				glBindTexture(GL_TEXTURE_2D, enemy);
				// flips texture with each movement
				if (secs % 2 == 1) {
					reverseLoadSquare(i, j);
				}
				else {
					loadSquare(i, j);
				}
			}
			glPopMatrix();
		}
	}
}


/* Start screen */
void menu() {

	glPushMatrix();

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// blends the background of the character
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// mouse cursor collision with the start button
	if (mouse_x <= screenWidth / 2 + 200 * tile&&mouse_x >= screenWidth / 2 - 200 * tile&&mouse_y <= screenHeight / 2 + 100 * tile&&mouse_y >= screenHeight / 2 - 100 * tile) {
		glBindTexture(GL_TEXTURE_2D, blueHeart);
		// if start is pressed, start the game.
		if (LeftPressed) {
			choice = 2;
		}
	}
	else
		glBindTexture(GL_TEXTURE_2D, heartTex);

	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex2f(screenWidth / 2 - 200 * tile, screenHeight / 2 - 200 * tile);
	glTexCoord2f(0.0, 1.0); glVertex2f(screenWidth / 2 - 200 * tile, screenHeight / 2 + 200 * tile);
	glTexCoord2f(1.0, 1.0); glVertex2f(screenWidth / 2 + 200 * tile, screenHeight / 2 + 200 * tile);
	glTexCoord2f(1.0, 0.0); glVertex2f(screenWidth / 2 + 200 * tile, screenHeight / 2 - 200 * tile);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	myfont.Begin();

	glTranslatef(tile * 6, 0.0, 0.0);
	glColor3f(1.0,1.0,1.0);
	myfont.DrawString("Begin", 0.8f, screenWidth / 2 -100*tile, screenHeight / 2+50*tile );
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

/* Timer (doesn'work) and score display */
void displayScore() {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	glPushMatrix();

	myfont.Begin();

	glPushMatrix();

	glPopMatrix();
	glPushMatrix();
	timer();
	std::string s = std::to_string(60 - secs);
	std::string m = std::to_string(mins);
	std::string time = m + " : " + s;
	glTranslatef(tile * 2, 0.0, 0.0);
	myfont.DrawString(time, 0.3f, 0.0, screenHeight);
	glPopMatrix();

	std::string s1 = std::to_string(score);
	std::string s3 = std::to_string(totalColl);
	std::string s2 = "SCORE: " + s1 + "/" + s3;
	glTranslatef(tile * 6, 0.0, 0.0);
	myfont.DrawString(s2, 0.3f, 0.0, screenHeight);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

/* Player health (Hearts) */
void drawLives() {
	for (int i = 0; i < 9; i++) {
		if (heart[i] == 1) {
			glBindTexture(GL_TEXTURE_2D, heartTex);
			glEnable(GL_TEXTURE_2D);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

			//// blends the background of the character
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glBegin(GL_POLYGON);
			glTexCoord2f(0.0, 0.0); glVertex2f(screenWidth - (i + 1)*tile, screenHeight - 2 * tile);
			glTexCoord2f(0.0, 1.0); glVertex2f(screenWidth - (i + 1)*tile, screenHeight - tile);
			glTexCoord2f(1.0, 1.0); glVertex2f(screenWidth - (i)*tile, screenHeight - tile);
			glTexCoord2f(1.0, 0.0); glVertex2f(screenWidth - (i)*tile, screenHeight - 2 * tile);
			glEnd();

			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
		}
	}
}

void loadSquare(int i, int j) {
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	//// blends the background of the character
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex2f((j)*tile, screenHeight - (i + 1)*tile);
	glTexCoord2f(0.0, 1.0); glVertex2f(j*tile, screenHeight - i*tile);
	glTexCoord2f(1.0, 1.0); glVertex2f((j + 1)*tile, screenHeight - i*tile);
	glTexCoord2f(1.0, 0.0); glVertex2f((j + 1)*tile, screenHeight - (i + 1)*tile);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void reverseLoadSquare(int i, int j) {

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	//// blends the background of the character
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_POLYGON);
	glTexCoord2f(1.0, 0.0); glVertex2f((j)*tile, screenHeight - (i + 1)*tile);
	glTexCoord2f(1.0, 1.0); glVertex2f(j*tile, screenHeight - i*tile);
	glTexCoord2f(0.0, 1.0); glVertex2f((j + 1)*tile, screenHeight - i*tile);
	glTexCoord2f(0.0, 0.0); glVertex2f((j + 1)*tile, screenHeight - (i + 1)*tile);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void Reset(void)
{
	cout << "Press up arrow to jump " << endl;
	cout << "Press <- to move right and -> to move left" << endl;
}

//ref.: http://www.cse.unt.edu/~donr/courses/4410/NOTES/timeExamples.html
void timer() {
	time_t timer = time(NULL);
	struct tm timeinfo = *localtime(&timer);
	secs = timeinfo.tm_sec;
	mins = timeinfo.tm_min;
}

GLuint loadPNG(char* name)
{
	// Texture loading object
	nv::Image img;

	GLuint myTextureID;

	// Return true on success
	if (img.loadImageFromFile(name))
	{
		glGenTextures(1, &myTextureID);
		glBindTexture(GL_TEXTURE_2D, myTextureID);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexImage2D(GL_TEXTURE_2D, 0, img.getInternalFormat(), img.getWidth(), img.getHeight(), 0, img.getFormat(), img.getType(), img.getLevel(0));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
	}

	else
		MessageBox(NULL, "Failed to load texture", "End of the world", MB_OK | MB_ICONINFORMATION);

	return myTextureID;
}


void init(void)
{
	Reset();
	s2 = playerYpos; v2 = 126.269;
	choice = 1;
	glClearColor(0.0, 0.0, 0.0, 0.0);

	// font
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (!myfont.Create("Arial72.glf", 1))
		exit(0);

// texture mapping
/**********************************************************************************************/

	// Player walking and standing texture, 5 frames
	cat = loadPNG("kitty2/cat0.png");
	cat1 = loadPNG("kitty2/cat1.png");
	cat2 = loadPNG("kitty2/cat2.png");
	cat3 = loadPNG("kitty2/cat3.png");
	cat4 = loadPNG("kitty2/cat4.png");

	// Player jump texture, 3 frames
	jump1 = loadPNG("kitty2/jump1.png");
	jump2 = loadPNG("kitty2/jump3.png");

	happy = loadPNG("kitty2/happy.png");

	// fixed platform texture
	platform = loadPNG("kitty2/wall.png");
	// moving platform texture
	mPlatform = loadPNG("kitty2/cloud.png");
	// background texture
	background = loadPNG("kitty2/back1.png");

	// Object for player to collect texture, 2 frames (Bird)
	bird0 = loadPNG("kitty2/bird0.png");
	bird1 = loadPNG("kitty2/bird1.png");

	// Object for player to avoid texture
	enemy = loadPNG("kitty2/enemy.png");

	// Player health/hearts texture
	//Pink
	heartTex = loadPNG("kitty2/heart.png");
	//Blue
	blueHeart = loadPNG("kitty2/blueHeart.png");

	// angry reaction (wasn't used)
	angry = loadPNG("kitty2/angry.png");

	/**********************************************************************************************/

	glShadeModel(GL_FLAT);
}


// Parts of code for fall and jump methods have been taken from bouncing ball lab example:
// Simple example of integration using Improved Euler for a bouncing ball
// By: RJAL, Date: 04/2017

void fall(double dt)
{
	// so player couldn't jump while falling
	startSimulation = false;

	float g = -9.81; float frac = 0.95;
	float v_old, s_old, fc;
	double ndt;

	v_old = v2;
	s_old = s2;
	v2 = v_old + g*dt;
	s2 = s_old + ((v_old + v2) / 2.0)*dt; // Use improved Euler Integration

	if (touchingPlatform) { // Player has collided
		startFall = false;
		
	}
}

float Jump(double dt, float y) 
{
	float g = 40.0; float frac = 0.22; 
	float v_old, s_old, fc;
	
	double ndt;

	v_old = v2;
	s_old = s2;
	
	v2 = v_old - g*dt;
	
	s2 = s_old + ((v_old + v2) / 2.0)*dt; // Use improved Euler Integration
	// character has collided
	if (s2 <= y) {
		s2 = y; v2 = 100.0 + screenHeight*0.05;
		startSimulation = false;
		s_old = y;
	}
	// if fall is started, stop the jump simulation
	else if (startFall) {
		startSimulation = false;
	}

	if (v2 > 0.0 && v2 < 100.0 + screenHeight*0.05) {
		jump = 1;
	}
	else if (v2 < 0.0) {
		jump = 2;
	}
	//cout << s2 << "  " << s_old << " " << v2 << endl;
	return s_old;
}


void Simulate(double cdt) // cdt is a clock based time increment and is as such platform independent
{
	switch (choice) {
	case 1:
		
		break;
	case 2:
		s1 = Jump(cdt, playerYpos);
		break;
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();
	switch (choice) {
	case 1:
		glPushMatrix();
		glLoadIdentity();
		menu();					// Start screen
		glPopMatrix();
		glFlush();
		break;

	case 2:


		drawScene();			// whole game environment 
		glPushMatrix();

		glTranslatef(-side, side, 0.0);
		drawLives();			// player health (9 hearts)

		glPopMatrix();

		displayScore(); 		// timer and score


		glPushMatrix();

		tile = screenHeight / gameHeight;

		// when the right side of the game environment is reached

		if (Xtri > gameWidth*tile - screenWidth / 2) {
			//Xtri = gameWidth*tile - screenWidth / 2;
			glTranslatef(Xtri - screenWidth, s2, 0.0);
			if (Xtri + side >= gameWidth*tile) {
				Xtri = gameWidth*tile - side;
			}
		}
		// when the left side of the game environment is reached
		else if (Xtri <= screenWidth / 2) {
			glTranslatef(Xtri, s2, 0.0);
			if (Xtri <= 0) {
				Xtri = side;
			}
		}
		// middle, don't move the character on the x axis
		else {
			glTranslatef(screenWidth / 2, s2, 0.0);
		}

		side = tile / 2;
		drawPlayer(side, 0.0, 0.0, 24);
		
		glPopMatrix();
		glFlush();
		break;

	}
	glPopMatrix();

	glFlush();

}

/* calculates the distance between two objects based on their x and y coords */
float getDistance(float x1, float y1, float x2, float y2) {
	float xDistance = x2 - x1;
	float yDistance = y2 - y1;

	return sqrtf(pow(xDistance, 2) + pow(yDistance, 2));
}

/* for handling player collision with empty space (0) in the game environment game1[][] */
void checkSpace(float x, float y) {
	float distance2 = getDistance(Xtri, s2 + side, x, y);
	if (distance2 <= side) {
		if (game1[gamePlatformY][gamePlatformX] == 0 && !startFall && !startSimulation) {
			touchingPlatform = false;
			v2 = 0;
			startFall = true;
		}
	}
}

void collision(float x, float y) {
	// calculates the distance between the platforms
	float distance = getDistance(Xtri, s2, x, y);
	float bottom = getDistance(Xtri + side, s2, x, y);

	// if player and platform collide
	if (distance <= side * 2) {
		playerYpos = y + side;

		// player colliding with the bottom of the platform
		if (s2 + side >= y - side&&s2 + side < y) {
			// set limit so it doesn't pass through
			s2 = y - tile;
			// if player is jumping, stop the jump and start the fall
			startSimulation = false;
			v2 = 0;
			touchingPlatform = false;
			startFall = true;
		}
		// player colliding with the top of the platform
		else if (s2 - side > y&&s2 - side <= y + side) {
			// limiting the player movement
			s2 = y + tile;
			touchingPlatform = true;
			playerYpos = y + tile;
			// if collided with an enemy object, eliminate the object
			if (touchedEnemy) {
				/*s2 = y + 2 * tile;*/
				game1[gamePlatformY][gamePlatformX] = 0;
				touchedEnemy = false;
				
			}
		}
		// player colliding with right side of the platform limit
		else if (Xtri + side >= x - side&& Xtri + side<x) {
			Xtri = x - tile;
			// if collided with an enemy object, bounce off
			if (touchedEnemy) {
				Xtri = x - 3 * tile;
				touchedEnemy = false;
			}
		}
		// player colliding with left side of the paltform limit
		else if (Xtri - side <= x + side&& Xtri - side>x) {
			Xtri = x + tile;
			// if collided with an enemy object, bounce off
			if (touchedEnemy) {
				Xtri = x + 3 * tile;
				touchedEnemy = false;
			}
		}
	}
}

void reshape(int width, int height)		// Resize the OpenGL window
{
	screenWidth = width; screenHeight = height; 
														  
	glViewport(0, 0, width, height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	gluOrtho2D(0, width, 0, height);           // set the coordinate system for the window

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

void processKeys()
{
	if (keys[VK_LEFT])
	{
		Xtri -= (float)screenHeight*0.002;
		pLeft = true;
		frame = (int)Xtri % 4;
	}
	if (keys[VK_RIGHT])
	{
		Xtri += (float)screenHeight*0.002;
		pLeft = false;
		frame = (int)Xtri % 4;
	}
	if (keys[VK_UP]) {
		touchingPlatform = false;
		startSimulation = true;
	}
}

// Implemented from bouncing ball lab example.
void TimeSimulation()
{
	// Get the current time
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
	__int64 currentTime = t.QuadPart;

	__int64 ticksElapsed = currentTime - prevTime;					// Ticks elapsed since the previous time step
	double deltaT = double(ticksElapsed) * timerFrequencyRecip;		// Convert to second
																	//cout << ticksElapsed << " " << deltaT << endl;
	if (startFall) {
		fall(deltaT);
	}
	
	if (startSimulation) {
		Simulate(deltaT);	   // Update variables when left mouse button is pressed
	}

	// Advance timer
	prevTime = currentTime;					// use the current time as the previous time in the next step
}


/**************** END OPENGL FUNCTIONS *************************/

//WIN32 function declarations
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc
void KillGLWindow();									// releases and destroys the window
bool CreateGLWindow(char* title, int width, int height); //creates the window
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);  // Win32 main function

													   //WIN32 global variabless
HDC			hDC = NULL;		// Private GDI Device Context
HGLRC		hRC = NULL;		// Permanent Rendering Context
HWND		hWnd = NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application


							/******************* WIN32 FUNCTIONS ***************************/
int WINAPI WinMain(HINSTANCE	hInstance,			// Instance
	HINSTANCE	hPrevInstance,		// Previous Instance
	LPSTR		lpCmdLine,			// Command Line Parameters
	int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	bool	done = false;								// Bool Variable To Exit Loop


	AllocConsole();
	FILE *stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);

	// Create Our OpenGL Window
	if (!CreateGLWindow("OpenGL Simulation 1", screenWidth, screenHeight))
	{
		return 0;									// Quit If Window Was Not Created
	}

	while (!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message == WM_QUIT)				// Have We Received A Quit Message?
			{
				done = true;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			if (keys[VK_ESCAPE])
				done = true;

			processKeys();			//process keyboard
			display();				// Draw The Scene
			TimeSimulation();		// Time the simulation
			SwapBuffers(hDC);		// Swap Buffers (Double Buffering)
		}
	}
	// Shutdown
	KillGLWindow();									// Kill The Window
	return (int)(msg.wParam);						// Exit The Program
}

//WIN32 Processes function - useful for responding to user inputs or other events.
LRESULT CALLBACK WndProc(HWND	hWnd,			// Handle For This Window
	UINT	uMsg,			// Message For This Window
	WPARAM	wParam,			// Additional Message Information
	LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)							     // Check For Windows Messages
	{
	case WM_CLOSE:								 // Did We Receive A Close Message?
	{
		PostQuitMessage(0);						 // Send A Quit Message
		return 0;								 // Jump Back
	}
	break;

	case WM_SIZE:								  // Resize The OpenGL Window
	{
		reshape(LOWORD(lParam), HIWORD(lParam));  // LoWord=Width, HiWord=Height
		return 0;								  // Jump Back
	}
	break;

	case WM_LBUTTONDOWN:
	{
		mouse_x = LOWORD(lParam);
		mouse_y = screenHeight - HIWORD(lParam);
		startSimulation = true;  // jump
		LeftPressed = true;
	}
	break;

	case WM_RBUTTONDOWN:
	{
		startSimulation = false;
	}
	break;

	case WM_LBUTTONUP:
	{
		LeftPressed = false;
	}
	break;

	case WM_MOUSEMOVE:
	{
		mouse_x = LOWORD(lParam);
		mouse_y = screenHeight - HIWORD(lParam);
	}
	break;
	case WM_KEYDOWN:							// Is A Key Being Held Down?
	{
		keys[wParam] = true;					// If So, Mark It As TRUE
		return 0;								// Jump Back
	}
	break;
	case WM_KEYUP:								// Has A Key Been Released?
	{
		keys[wParam] = false;					// If So, Mark It As FALSE
		return 0;								// Jump Back
	}
	break;
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void KillGLWindow()								// Properly Kill The Window
{
	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL, NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd, hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hDC = NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL", hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;									// Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
*	title			- Title To Appear At The Top Of The Window				*
*	width			- Width Of The GL Window Or Fullscreen Mode				*
*	height			- Height Of The GL Window Or Fullscreen Mode			*/

bool CreateGLWindow(char* title, int width, int height)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left = (long)0;			// Set Left Value To 0
	WindowRect.right = (long)width;		// Set Right Value To Requested Width
	WindowRect.top = (long)0;				// Set Top Value To 0
	WindowRect.bottom = (long)height;		// Set Bottom Value To Requested Height

	hInstance = GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc = (WNDPROC)WndProc;					// WndProc Handles Messages
	wc.cbClsExtra = 0;									// No Extra Window Data
	wc.cbWndExtra = 0;									// No Extra Window Data
	wc.hInstance = hInstance;							// Set The Instance
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground = NULL;									// No Background Required For GL
	wc.lpszMenuName = NULL;									// We Don't Want A Menu
	wc.lpszClassName = "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;											// Return FALSE
	}

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
	dwStyle = WS_OVERLAPPEDWINDOW;							// Windows Style

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

																	// Create The Window
	if (!(hWnd = CreateWindowEx(dwExStyle,							// Extended Style For The Window
		"OpenGL",					// Class Name
		title,						// Window Title
		dwStyle |							// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		xPos, yPos,							// Window Position
		WindowRect.right - WindowRect.left,	// Calculate Window Width
		WindowRect.bottom - WindowRect.top,	// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		hInstance,							// Instance
		NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		24,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		24,											// 24Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if (!(hDC = GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if (!(hRC = wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if (!wglMakeCurrent(hDC, hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	ShowWindow(hWnd, SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	reshape(width, height);					// Set Up Our Perspective GL Screen
	init();

	return true;									// Success
}





