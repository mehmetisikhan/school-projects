/**********************************************************************
*
* Pong - Computer Graphics and Game Programming
*
* Things you must do:
* * set initial values based on command line arguments
* * add basic collision detection and ball trajectory handling
* * add code that prevents paddles from moving outside the court
* * add logic to determine when the game is over
* * create a README file named 'README' describing your work
*
* Things you may do:
* * keep score (and display it!)
* * move ball a distance proportional to how much time has
* elapsed since the last ball move rather than a fixed distance
* * allow ball_delay to be changed at runtime as a "difficulty" level
* * extra visual goodies (we're easy to impress)
* 
**********************************************************************/
#include <glut.h>
#include <stdio.h>
#include <time.h> 
/* ascii codes for various special keys */
#define ESCAPE 27
/**********************************************************************
* Configuration
**********************************************************************/
#define INITIAL_X_POS 50
#define INITIAL_Y_POS 50
#define INITIAL_WIDTH 640
#define INITIAL_HEIGHT 480
#define MARGIN_SIZE 10
#define BORDER_SIZE 18
#define PADDLE_LENGTH 50
#define BALL_SIZE 15
#define BALL_STEP BALL_SIZE /* how far ball travels per time step */
#define WINDOW_NAME "91220001906_assignment_2"
/**********************************************************************
* Globals
**********************************************************************/
GLsizei window_width;
GLsizei window_height;
GLfloat initial_paddle_y_pos;
GLfloat initial_ball_x_pos;
GLfloat initial_ball_y_pos;
GLfloat initial_ball_trajectory;
GLfloat paddle_y_pos = INITIAL_HEIGHT / 2;
GLfloat ball_x_pos = INITIAL_WIDTH / 2;
GLfloat ball_y_pos = INITIAL_HEIGHT / 2;
GLfloat ball_trajectory;
GLuint ball_delay = 30; /* time between ball position updates in msecs */
int game_over = 1;
int current_game = 0;
int left_button_state = GLUT_UP;
int left_button_lasty;

//MENU//
char errorMessage[50];
int menu_selection = 1;
bool isMenuActive = true;
int gameMode = 1;

bool ballDirectionX = true;
int ballDirectionY = 0;
float ball_route[2][2];
int game_over_counter = 0;
int game_chance = 2;

float slope = 0;
float applySlope = 1;

bool fourthBoxBug = false;
bool resetButtonCheck = true;


//TIME ELEMENTS//
clock_t start, end, pause_end, pause_start, pause;
double execution_time;

//BOX GAME//
bool boxCheck[100];
int removedBox = 0;
bool successTextCheck = false;

void stroke_output(GLfloat x, GLfloat y, int scale, const char *format,...){
	va_list args;
	char buffer[200], *p;
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(0.1*scale, 0.1*scale, 0.1*scale);
	for (p = buffer; *p; p++)
	glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
	glPopMatrix();
}

/*
void getArgumentsOnConsole(){
	while(true){
		printf("Please enter necessary arguments (ball_x_pos ball_y_pos ball_trajectory paddle_y_pos) :");
		scanf("%f %f %f %f", &ball_x_pos, &ball_y_pos, &ball_trajectory, &paddle_y_pos);
		if(ball_x_pos < 37 || ball_x_pos > (window_width-74) ){
			printf(ball_x_pos is out of range !");
		}else if(ball_y_pos < 37 || ball_y_pos > (window_height-38) ){
			printf("ball_y_pos is out of range !");
		}else if(paddle_y_pos < 80 || paddle_y_pos > (window_height-80) ){
			printf("paddle_y_pos is out of range !");
		}else{
			initial_ball_x_pos = ball_x_pos;
			initial_ball_y_pos = ball_y_pos ;
			initial_paddle_y_pos = paddle_y_pos;
			initial_ball_trajectory = ball_trajectory;
			break;
		}
	}
}
*/


/*Main Menu*/
void mainMenu(){
	
	glColor3d(1,1,1); //Reset the color matrix to white.
	
	/* Ball X */
	if(menu_selection == 1) //Check the menu item selection
		glColor3d(1,0,0); //If the main item selection is 1, change the color matrix to red.
	stroke_output((window_width/2)-200, (window_height/2)+100, 2, "ball_x_pos : ");
	stroke_output((window_width/2)+200, (window_height/2)+100, 2, "%.1f",ball_x_pos);
	glColor3d(1,1,1); //Reset the color matrix to white.
	
	/* Ball Y */
	if(menu_selection == 2)
		glColor3d(1,0,0); //If the main item selection is 2, change the color matrix to red.
	stroke_output((window_width/2)-200, (window_height/2)+50, 2, "ball_y_pos : ");
	stroke_output((window_width/2)+200, (window_height/2)+50, 2, "%.1f",ball_y_pos);
	glColor3d(1,1,1); //Reset the color matrix to white.
	
	/* Ball Trajectory */
	if(menu_selection == 3)
		glColor3d(1,0,0); //If the main item selection is 3, change the color matrix to red.
	stroke_output((window_width/2)-200, (window_height/2), 2, "ball_trajectory : ");
	stroke_output((window_width/2)+200, (window_height/2), 2, "%.1f",ball_trajectory);
	glColor3d(1,1,1); //Reset the color matrix to white.
	
	/* Paddle Y */
	if(menu_selection == 4)
		glColor3d(1,0,0); //If the main item selection is 4, change the color matrix to red.
	stroke_output((window_width/2)-200, (window_height/2)-50, 2, "paddle_y_pos : ");
	stroke_output((window_width/2)+200, (window_height/2)-50, 2, "%.1f",paddle_y_pos);
	glColor3d(1,1,1); //Reset the color matrix to white.
	
	/*Game Mode*/
	if(menu_selection == 5)
		glColor3d(1,0,0); //If the main item selection is 5, change the color matrix to red.
	stroke_output((window_width/2)-200, (window_height/2)-100, 2, "Game Mode : ");
	
	if(gameMode == 0)
		stroke_output((window_width/2)+162, (window_height/2)-100, 2, "TRAINING");
	if(gameMode == 1)
		stroke_output((window_width/2)+128, (window_height/2)-100, 2, "BOX GAME");
	glColor3d(1,1,1);
	
	stroke_output((window_width/2)-200, (window_height/2)-150, 1, "Use 'DOWNWARD', 'UPWARD', 'LEFT' AND 'RIGHT' Keys");
	stroke_output((window_width/2)-200, (window_height/2)-165, 1, "Press 'F1' save and start.");
	
	//https://stackoverflow.com/questions/1793867/best-way-to-check-if-a-character-array-is-empty
	if(strlen(errorMessage) != 0){
		glColor3d(1,0,0);
		stroke_output((window_width/2)-200, (window_height/2)-135, 1, "%s",errorMessage);
		glColor3d(1,1,1);
	}
	
		
    glLoadIdentity();
    glutWireTeapot(1);
}

/*Box Game*/
void boxGame(){
	int boxRowCounter = (window_height / 150); //defining the row counter for the boxes
	int spaceBtwBoxes = (boxRowCounter-1)*20; //the space value between the boxes
	int borderSpace = (window_height - (spaceBtwBoxes + (boxRowCounter * 100)))/2; //calculation of the border space
	int btwSpace = 20; //the space value between the boxes
	int boxColumnCounter = (window_width / 200) - 1; //defining the column counter for the boxes
	int upborderSpace = borderSpace+100; //first box upper coordinate
	int boxCounter = boxRowCounter * boxColumnCounter; //the box counter
	float boxRightSides[boxCounter][4];
	float boxLeftSides[boxCounter][4];
	float boxUp[boxCounter][3];
	float boxDown[boxCounter][3];
	

	boxCounter = 0;
	for(int a = 0 ; a < boxRowCounter ; a++){
		btwSpace = 20;
		for(int i = 0 ; i < boxColumnCounter ; i++){
			if(boxCheck[boxCounter]){
				glColor3d(1,0,0);
				glBegin(GL_QUADS);
					glVertex3f(50+btwSpace,borderSpace,0);
					glVertex3f(110+btwSpace,borderSpace,0);
					glVertex3f(110+btwSpace,upborderSpace,0);
					glVertex3f(50+btwSpace,upborderSpace,0);
				glEnd();
				//for right sides//
				boxRightSides[boxCounter][0] = 110+btwSpace;
				boxRightSides[boxCounter][1] = borderSpace;
				boxRightSides[boxCounter][2] = upborderSpace;
				boxRightSides[boxCounter][3] = 50+btwSpace;
				//for left sides//
				boxLeftSides[boxCounter][0] = 50+btwSpace;
				boxLeftSides[boxCounter][1] = borderSpace;
				boxLeftSides[boxCounter][2] = upborderSpace;
				boxLeftSides[boxCounter][3] = 110+btwSpace;
				//for up//
				boxUp[boxCounter][0] = upborderSpace;
				boxUp[boxCounter][1] = 55+btwSpace;
				boxUp[boxCounter][2] = 115+btwSpace;
				//for down//
				boxDown[boxCounter][0] = borderSpace;
				boxDown[boxCounter][1] = 55+btwSpace;
				boxDown[boxCounter][2] = 115+btwSpace;
				glColor3d(1,1,1);
			}
			boxCounter++;
			btwSpace +=90;

		}
		borderSpace = upborderSpace + 20;
		upborderSpace = borderSpace+100;
	}
	
	for(int i = 0 ; i < boxCounter ; i++){
		if( (boxUp[i][0]+5 > ball_y_pos) && (boxUp[i][0]-5 < ball_y_pos) && (boxUp[i][1] < ball_x_pos) && (boxUp[i][2] > ball_x_pos)){
			printf("touch to (up) %d \n",i);
			slope = 0.0;
			ballDirectionY = 1;
			boxCheck[i] = false;
			removedBox++;
		}
		else if( (boxDown[i][0]+5 > ball_y_pos) && (boxDown[i][0]-5 < ball_y_pos) && (boxDown[i][1] < ball_x_pos) && (boxDown[i][2] > ball_x_pos)){
			printf("touch to (down) %d \n",i);
			slope = 0.0;
			ballDirectionY = 2;
			boxCheck[i] = false;
			removedBox++;
		}
		else if( (boxLeftSides[i][0] < ball_x_pos) && (boxLeftSides[i][3] > ball_x_pos) && (boxLeftSides[i][1] < ball_y_pos) && (boxLeftSides[i][2] > ball_y_pos)){
			printf("touch to (left) %d \n",i);
			slope = 0.0;
			ballDirectionX = !ballDirectionX;
			boxCheck[i] = false;
			removedBox++;
		}
		else if( (boxRightSides[i][0] > ball_x_pos) && (boxRightSides[i][3] < ball_x_pos) && (boxRightSides[i][1] < ball_y_pos) && (boxRightSides[i][2] > ball_y_pos)){
			if(fourthBoxBug || i== 4) //index 4. box bug fix
				continue;
			printf("touch to (right) %d \n",i);
			slope = 0.0;
			ballDirectionX = !ballDirectionX;
			boxCheck[i] = false;
			removedBox++;
			if(i == 4) //index 4. box bug fix
				fourthBoxBug = true;
		}else{
			
		}
	}
	
		if(removedBox == boxCounter){
			successTextCheck = true;
			if(end == 0){
				end = clock();
				execution_time = (((double)(end - start))/CLOCKS_PER_SEC) - pause;
			}
			glColor3d(0,1,0);
			stroke_output((window_width/2)-150, (window_height/2), 5, "SUCCESS!!");
			stroke_output((window_width/2)-60, (window_height/2)-50, 1, "Your score %fsn",execution_time);
			glColor3d(1,1,1);
			stroke_output((window_width/2)-60, (window_height/2)-75, 1, "Press 'ESC' to exit.");
		}
	

}


/**********************************************************************
* Set the new size of the window
**********************************************************************/
void resize_scene(GLsizei width, GLsizei height){
	if(isMenuActive){
		paddle_y_pos = height / 2;
		ball_x_pos = width / 2;
		ball_y_pos = height / 2;
	}
	//printf("width %d , height %d",width,height);
 	glViewport(0, 0, width, height); /* reset the current viewport and 
	* perspective transformation */
	window_width = width;
	window_height = height;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	/* for the purposes of this assignment, we are making the world
	* coordinate system have a 1-1 correspondence with screen space
	* (in pixels). The origin maps to the lower-left corner of the screen.*/
	glOrtho(0, width, 0, height, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	
}




/**********************************************************************
* The main drawing functions. 
**********************************************************************/
void draw_border(void){
	
	const GLfloat border = MARGIN_SIZE + BORDER_SIZE;
	glBegin(GL_QUADS);
		/* top bar */
		glVertex3f(MARGIN_SIZE,(window_height-MARGIN_SIZE),0);
		glVertex3f(MARGIN_SIZE,(window_height-border),0);
		glVertex3f((window_width-MARGIN_SIZE),(window_height-border),0);
		glVertex3f((window_width-MARGIN_SIZE),(window_height-MARGIN_SIZE),0);
		/* side bar */
		glVertex3f(MARGIN_SIZE,(window_height-border),0);
		glVertex3f(MARGIN_SIZE,border,0);
		glVertex3f(border,border,0);
		glVertex3f(border,(window_height-border),0);
		/* bottom bar */
		glVertex3f(MARGIN_SIZE,border,0);
		glVertex3f(MARGIN_SIZE,MARGIN_SIZE,0);
		glVertex3f((window_width-MARGIN_SIZE),MARGIN_SIZE,0);
		glVertex3f((window_width-MARGIN_SIZE),border,0);
	glEnd();
	
}

void draw_UI(){
	glColor3d(1,0,0);
	stroke_output((window_width)-200, (window_height)-25, 1, "Remains chance : %d",game_chance-game_over_counter); //printing the remains chance 
	glColor3d(1,1,1);
	if(game_over_counter == game_chance && (!successTextCheck)){
		if(end == 0){
			end = clock();
			execution_time = (((double)(end - start))/CLOCKS_PER_SEC) - pause; //score calculating
		}
		glColor3d(1,0,0);
		stroke_output((window_width/2)-180, (window_height/2), 5, "GAME OVER!"); //printing the game over message
		stroke_output((window_width/2)-60, (window_height/2)-50, 1, "Your score %fsn",execution_time); //printing the player's score
		glColor3d(1,1,1);
		stroke_output((window_width/2)-60, (window_height/2)-75, 1, "Press 'ESC' to exit.");
	}
	
	if(game_over && game_over_counter != 0 && game_over_counter != game_chance && (!successTextCheck)){
		glColor3d(1,1,1);
		stroke_output((window_width/2)-100, (window_height/2), 3, "You Failed"); //printing the failed messaga
		stroke_output((window_width/2)-80, (window_height/2)-50, 1, "Your Remain chance %d",game_chance-game_over_counter); //printing the remains chance 
		stroke_output((window_width/2)-80, (window_height/2)-75, 1, "Press 'R' to restart again."); //resetting the game information
		stroke_output((window_width/2)-80, (window_height/2)-100, 1, "Then press 'S' for the start");
		glColor3d(1,1,1);
	}
}


void draw_paddle(void){
	
	const GLfloat xpos = window_width - (MARGIN_SIZE+3*BORDER_SIZE);
	const GLfloat yoff = PADDLE_LENGTH;
	/* the paddle is always drawn at a fixed location within its own
	* coordinate space. its actual world-space position is determined
	* by the modelview matrix (as affected by any glTranslatef(), 
	glRotatef(),
	* or other function calls) */
	
	//The developer paddle
	/*
	//RED AREA//
	glColor3d(1,0,0);
	glBegin(GL_QUADS);
		glVertex3f(xpos,-yoff+10,0);
		glVertex3f(xpos,-yoff,0);
		glVertex3f(xpos+BORDER_SIZE,-yoff+10,0);
		glVertex3f(xpos+BORDER_SIZE,-yoff,0);
	glEnd();
	glBegin(GL_QUADS);
		glVertex3f(xpos,yoff-10,0);
		glVertex3f(xpos,yoff,0);
		glVertex3f(xpos+BORDER_SIZE,yoff-10,0);
		glVertex3f(xpos+BORDER_SIZE,yoff,0);
	glEnd();
	glBegin(GL_QUADS);
		glVertex3f(xpos,-yoff+30,0);
		glVertex3f(xpos,-yoff+35,0);
		glVertex3f(xpos+BORDER_SIZE,-yoff+30,0);
		glVertex3f(xpos+BORDER_SIZE,-yoff+35,0);
	glEnd();
	glBegin(GL_QUADS);
		glVertex3f(xpos,yoff-30,0);
		glVertex3f(xpos,yoff-35,0);
		glVertex3f(xpos+BORDER_SIZE,yoff-30,0);
		glVertex3f(xpos+BORDER_SIZE,yoff-35,0);
	glEnd();
	
	//BLUE AREA//
	glColor3d(0,0,1);
	glBegin(GL_QUADS);
		glVertex3f(xpos,-yoff+10,0);
		glVertex3f(xpos,-yoff+30,0);
		glVertex3f(xpos+BORDER_SIZE,-yoff+10,0);
		glVertex3f(xpos+BORDER_SIZE,-yoff+30,0);
	glEnd();
	glBegin(GL_QUADS);
		glVertex3f(xpos,yoff-10,0);
		glVertex3f(xpos,yoff-30,0);
		glVertex3f(xpos+BORDER_SIZE,yoff-10,0);
		glVertex3f(xpos+BORDER_SIZE,yoff-30,0);
	glEnd();
	
	//WHITE AREA//
	glColor3d(1,1,1);
	glBegin(GL_QUADS);
		glVertex3f(xpos,-yoff+35,0);
		glVertex3f(xpos,yoff-35,0);
		glVertex3f(xpos+BORDER_SIZE,-yoff+35,0);
		glVertex3f(xpos+BORDER_SIZE,yoff-35,0);
	glEnd();
	*/
	
	
	
	glBegin(GL_QUADS);
		glVertex3f(xpos,-yoff,0);
		glVertex3f(xpos,yoff,0);
		glVertex3f(xpos+BORDER_SIZE,yoff,0);
		glVertex3f(xpos+BORDER_SIZE,-yoff,0);
	glEnd();
	
}


void draw_ball(void){
	
	/* the ball is always drawn at the origin of its own coordinate
	* system. its real position in world-space is determined by the
	* modelview matrix, just as with the paddle */
	glBegin(GL_POINTS);
	glVertex3f(0,0,0);
	glEnd();
	
}

//Draw Scenes//
void draw_scene(void){
	
		
	/* clear the screen and the depth buffer */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//https://gist.github.com/insaneyilin/9320e8263b29e3c172c4f5963b8db693
	if(isMenuActive){
		mainMenu();
	}else if(gameMode == 1){ //Game mode checking
		/* reset modelview matrix */
		glLoadIdentity();
		/* draw the border */
		draw_border();
		/* draw boxes*/
		boxGame();
		/* draw the paddle */
		glPushMatrix();
			glTranslatef(0, paddle_y_pos, 0);
			draw_paddle();
			glPopMatrix();
			/* draw the ball */
			glPushMatrix();
			glTranslatef(ball_x_pos, ball_y_pos, 0);
			draw_ball();
		glPopMatrix();
		draw_UI();
		/* since this is double buffered, swap the
		* buffers to display what just got drawn */
	}else{
		/* reset modelview matrix */
		glLoadIdentity();
		/* draw the border */
		draw_border();
		/* draw the paddle */
		glPushMatrix();
			glTranslatef(0, paddle_y_pos, 0);
			draw_paddle();
			glPopMatrix();
			/* draw the ball */
			glPushMatrix();
			glTranslatef(ball_x_pos, ball_y_pos, 0);
			draw_ball();
		glPopMatrix();
		draw_UI();
		/* since this is double buffered, swap the
		* buffers to display what just got drawn */
	}
	
	
	
	glutSwapBuffers();
	
}

//Ball and paddle movement//
void move_ball(int game){

	/* this allows a game reset in the middle of a game, since
	* once a ball move timer function is registered on the old
	* game, it cannot be cancelled */
	
	if (game != current_game)
		return;
		
	/* FIXME: you must make the ball move in the direction indicated
	* by ball_trajectory and account for collisions/bounces. you
	* must also determine when the game is over and set game_over
	* appropriately. */

	if(ball_y_pos <= 40){ //bottom border
		ballDirectionY = 1;
	}
	if(ball_y_pos > (window_height-38)){ //up border
		ballDirectionY = 2;
	}
	if(ball_x_pos < 37){ //left side border
		ball_route[0][0] = (float)ball_x_pos;
		ball_route[0][1] = (float)ball_y_pos;
		ballDirectionX = false;
		slope = ((ball_route[0][1]-ball_route[1][1]) / (ball_route[0][0] - ball_route[1][0]));
		printf("y1 : %.1f - x1 : %.1f, y2 : %.1f - x2 : %.1f -- slope is  %f\n" ,ball_route[0][1],ball_route[0][0], ball_route[1][1],ball_route[1][0], slope);
	}
	//if(ball_x_pos > (window_width-74)){ //cheat code
	//paddle
	if( (ball_x_pos > (window_width-74)) && (!(ball_x_pos > (window_width-50))) & ( ball_y_pos < paddle_y_pos+55 && ball_y_pos > paddle_y_pos-55)){
		//ball_delay -= 1;
		
		if(ball_y_pos > paddle_y_pos+45){ //RED AREA-1//
			ballDirectionY = 1;
			applySlope = 1.1;
		}else if(ball_y_pos < paddle_y_pos-45){ //RED AREA-2//
			ballDirectionY = 2;
			applySlope = 1.1;
		}else if(ball_y_pos == paddle_y_pos){ //MIDDLE POINT//
			if(slope != 0.0 || slope != -0.0){
				ballDirectionY = 0;
			}else{
				//https://stackoverflow.com/questions/822323/how-to-generate-a-random-int-in-c
				ballDirectionY = rand() % 2 + 1;
			}
		}else if(ball_y_pos < paddle_y_pos+45 && ball_y_pos > paddle_y_pos+25){ //BLUE AREA-1//
			if(slope >= 1){
				
			}if(slope == 0 || slope == -0){
				ballDirectionY = 1;
				applySlope = ((float)rand()/(float)(RAND_MAX));
				printf("radnom slope is : %.1f\n",applySlope);
			}else{
				if((rand() % 4) == 1)
					applySlope = abs(slope)*10;
			}
		}else if(ball_y_pos > paddle_y_pos-45 && ball_y_pos < paddle_y_pos-25){ //BLUE AREA-2//
			if(slope >= 1){
				
			}else if(slope == 0.0 || slope == -0.0){
				ballDirectionY = 2;
				applySlope = ((float)rand()/(float)(RAND_MAX));
				printf("radnom slope is : %.1f\n",applySlope);
			}else{
				if((rand() % 4) == 1)
					applySlope = abs(slope)*10;
			}
		}else if(ball_y_pos < paddle_y_pos+25 && ball_y_pos > paddle_y_pos+20){ //RED AREA-3//
			if(slope == 0.0 || slope == -0.0){
				//applySlope = ((float)rand()/(float)(RAND_MAX));
				ballDirectionY = rand() % 2 + 1;
			}
			applySlope = 1.1;
		}else if(ball_y_pos > paddle_y_pos-25 && ball_y_pos < paddle_y_pos-20){ //RED AREA-4//
			if(slope == 0.0 || slope == -0.0){
				//applySlope = ((float)rand()/(float)(RAND_MAX));
				ballDirectionY = rand() % 2 + 1;
			}
			applySlope = 1.1;
			}else{ //WHITE AREA//
				if(slope == 0.0 || slope == -0.0){
					applySlope = ((float)rand()/(float)(RAND_MAX));
					ballDirectionY = rand() % 2 + 1;
				}else{
					if((rand() % 8) == 1)
						applySlope = abs(slope)*10;
				}
			}
		ballDirectionX = true;
		ball_route[1][0] = (float)ball_x_pos;
		ball_route[1][1] = (float)ball_y_pos;
		slope = ((ball_route[1][1]-ball_route[0][1]) / (ball_route[1][0] - ball_route[0][0]));
		printf("y1 : %.1f - x1 : %.1f, y2 : %.1f - x2 : %.1f -- slope is  %f\n" ,ball_route[1][1],ball_route[1][0], ball_route[0][1],ball_route[0][0],  slope);
	}else if(ball_x_pos > (window_width)){
		pause_start = clock();
		//printf("Game Over!");
		game_over_counter++;
		game_over = 1;
		resetButtonCheck = false;
	}
	
	if(ballDirectionX){
		ball_x_pos += -BALL_STEP;
	}else{
		ball_x_pos += BALL_STEP;
	}
	
	switch(ballDirectionY){
		case 0:
			break;
		case 1:
			ball_y_pos += BALL_STEP*applySlope;
			break;
		case 2:
			ball_y_pos += -BALL_STEP*applySlope;
			break;
			
	}
	
	
	if (!game_over) {
		/* tell glut when next to update the ball's position */
		glutTimerFunc(ball_delay, move_ball, current_game);
	}
	
	glutPostRedisplay();
}


/**********************************************************************
* BEWARE: in the following functions, y=0 is at the TOP of the screen,
* which is the exact opposite of our world which has y=0 at the
* bottom of the screen. It shouldn't matter much for the purposes of
* this assignment, but you have been warned.
**********************************************************************/
/**********************************************************************
* the function called whenever a normal key is pressed. 
**********************************************************************/
void key_press(unsigned char key, int x, int y){
	if(!isMenuActive)
		switch (key) {
			case 's': /* start game */
			case 'S':
				if(start == 0)
					start = clock();
				if(pause_start != 0){
					pause_end = clock();
					pause += ((double)(pause_end - pause_start))/CLOCKS_PER_SEC;
				}
				if (game_over && (game_chance != game_over_counter) && resetButtonCheck) {
					game_over = 0;
					move_ball(current_game);
				}
				break;
			case 'r': /* reset game */
			case 'R':
				if (game_over && (game_chance != game_over_counter)) {
					resetButtonCheck = true;
					ballDirectionY = 0;
					applySlope = 1;
					ballDirectionX = true;
					current_game++;
					ball_x_pos = initial_ball_x_pos;
					ball_y_pos = initial_ball_y_pos;
					paddle_y_pos = initial_paddle_y_pos;
					ball_trajectory = initial_ball_trajectory;
					game_over = 1;
					glutPostRedisplay();
				}
				break;
			case ESCAPE: /* exit the program...normal termination. */
				exit(0);
		}
}

//https://stackoverflow.com/questions/4249133/how-do-i-specify-a-keyboards-directional-keys-in-glut
//Main Menu Keys//
void mainMenuKeys(int key, int x, int y){
	if(isMenuActive) //Checking of the main menu activity
		switch(key){
			case GLUT_KEY_LEFT: //left arrow key
				switch(menu_selection){
					case 1:
						ball_x_pos -=1;
						//printf("%d",ball_x_pos);
						glutPostRedisplay();
						break;
					case 2:
						ball_y_pos -=1;
						//printf("%d",ball_y_pos);
						glutPostRedisplay();
						break;
					case 3:
						ball_trajectory -=0.1;
						glutPostRedisplay();
						break;
					case 4:
						paddle_y_pos -=1;
						glutPostRedisplay();
						break;
					case 5:
						if(gameMode == 1){
							gameMode = 0;
						}else{
							gameMode = 1;
						}
						glutPostRedisplay();
						break;
				}
				break;
			case GLUT_KEY_RIGHT: //right arrow key
				switch(menu_selection){
					case 1:
						ball_x_pos +=1;
						//printf("%d",ball_x_pos);
						glutPostRedisplay();
						break;
					case 2:
						ball_y_pos +=1;
						//printf("%d",ball_y_pos);
						glutPostRedisplay();
						break;
					case 3:
						ball_trajectory +=0.1;
						glutPostRedisplay();
						break;
					case 4:
						paddle_y_pos +=1;
						glutPostRedisplay();
						break;
					case 5:
						if(gameMode == 1){
							gameMode = 0;
						}else{
							gameMode = 1;
						}
						glutPostRedisplay();
						break;
				}
				break;
			case GLUT_KEY_DOWN: //down arrow key
				if(menu_selection==5){
					
				}else{
					menu_selection += 1;
				}
				glutPostRedisplay();
				break;
			case GLUT_KEY_UP: //up arrow key
				if(menu_selection==1){
					
				}else{
					menu_selection -= 1;
				}
				glutPostRedisplay();
				break;
			case GLUT_KEY_F1: //F1 key
				if(ball_x_pos < 37 || ball_x_pos > (window_width-74) ){
					strcpy(errorMessage,"ball_x_pos is out of range !");
					glutPostRedisplay();
				}else if(ball_y_pos < 37 || ball_y_pos > (window_height-38) ){
					strcpy(errorMessage,"ball_y_pos is out of range !");
					glutPostRedisplay();
				}else if(paddle_y_pos < 80 || paddle_y_pos > (window_height-80) ){
					//https://www.scaler.com/topics/c/c-string-declaration/
					strcpy(errorMessage,"paddle_y_pos is out of range !");
					glutPostRedisplay();
				}else{
					//printf(" ball_x_pos : %f, ball_y_pos: %f",ball_x_pos,ball_y_pos);
					initial_ball_x_pos = ball_x_pos;
					initial_ball_y_pos = ball_y_pos ;
					initial_paddle_y_pos = paddle_y_pos;
					initial_ball_trajectory = ball_trajectory;
					isMenuActive = false;
					glutPostRedisplay();
				}
				break;
		}
}



/**********************************************************************
* this function is called whenever a mouse button is pressed and moved
**********************************************************************/
void handle_mouse_motion(int x, int y){
	if (left_button_state == GLUT_DOWN && (!isMenuActive)) {
		if(paddle_y_pos < 80 ){
			paddle_y_pos = 80;
		}else if(paddle_y_pos > (window_height-80) ){
			paddle_y_pos = (window_height-80);
		}else{
			paddle_y_pos += left_button_lasty - y;
			left_button_lasty = y;
			glutPostRedisplay();
		}
		//printf("ball : %.1f , paddle : %.1f, paddle : %.1f\n",ball_y_pos, paddle_y_pos,paddle_y_pos+1);
 	}
 	
}



/**********************************************************************
* this function is called whenever a mouse button is pressed or released
**********************************************************************/
void handle_mouse_click(int btn, int state, int x, int y){
	switch (btn) {
		case GLUT_LEFT_BUTTON:
		left_button_state = state;
		left_button_lasty = y;
		break;
	}
}



int main(int argc, char **argv){
	//getArgumentsOnConsole(); 
	/*  This line about getting inputs for paddle_y_pos, ball_x_pos, ball_y_pos and ball_trajectory 
	 *	but thanks to main menu scene these setting can be taken on the screen.
	 */
	/* Initialize GLUT */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(INITIAL_WIDTH, INITIAL_HEIGHT);
	glutInitWindowPosition(INITIAL_X_POS, INITIAL_Y_POS);
	glutCreateWindow(WINDOW_NAME);
	
	//Assigning all boxCheck elements to true at the beginning of the game.
	for(int i = 0 ; i < 100 ; i ++){
		boxCheck[i] = true;
	}
	
	/* Register callback functions */
	glutDisplayFunc(draw_scene);
	glutReshapeFunc(resize_scene);
	
	//The menu keys
	glutSpecialFunc(mainMenuKeys);
	glutKeyboardFunc(key_press);
	glutMouseFunc(handle_mouse_click);
	glutMotionFunc(handle_mouse_motion);
	srand(time(NULL)); //https://stackoverflow.com/questions/822323/how-to-generate-a-random-int-in-c
	
	/* Initialize GL */
	glPointSize(BALL_SIZE);
	/* FIXME: set ball_trajectory, ball position, and paddle
	* position based on command line arguments */
	//paddle_y_pos = initial_paddle_y_pos = INITIAL_HEIGHT / 2;
	//ball_x_pos = initial_ball_x_pos = INITIAL_WIDTH / 2;
	//ball_y_pos = initial_ball_y_pos = INITIAL_HEIGHT / 2;
	//ball_trajectory = initial_ball_trajectory = 180.0f;
	/* Enter event processing loop */
	glutMainLoop(); 
	return 1;
}
