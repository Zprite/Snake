#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#define WIDTH 15
#define HEIGHT 15
#define SnakeMark 254 // ASCII square
#define ESC 27
#define FruitMark 'O';
enum directions {STOP=0, UP, LEFT , DOWN, RIGHT};
directions dir;
enum difficulty {EASY=0, HARD, INSANE};
using namespace std;

char board[HEIGHT][WIDTH];
bool game_over;
bool exit_game;
int speed;
int score;
int xPos;
int yPos;
int tailX[(HEIGHT - 1)*(WIDTH - 1)]; // Tail need not be longer than the area of the map.
int tailY[(HEIGHT - 1)*(WIDTH - 1)];
int tail_length;
int fruitX;
int fruitY;

void hide_cursor();
int select_difficulty();
void print_board();
void set_walls();
void spawn_fruit();
void get_input();
void updatePos();
void collision();
void update_tailPos();
void score_screen();

int main() 
{	
	hide_cursor();
	exit_game = false;
	do {
		int difficulty=select_difficulty(); // Start program with difficulty select screen
		if (exit_game == true)
		{
			cout << "\n Exiting . . .\n";
			Sleep(1000);
			return 0;
		}
		// Dificulty adjusts the speed of the game. The speed value adjusts delay after each frame (ms).
		if (difficulty == EASY) { speed = 150; } 
		else if (difficulty == HARD) { speed = 100; }
		else if (difficulty == INSANE) { speed = 50; }
		Sleep(50);
		score = 0;
		tail_length = 0;
		game_over = false;
		srand(time(NULL));
		xPos = WIDTH / 2;
		yPos = HEIGHT / 2;
		dir = STOP;
		set_walls();
		spawn_fruit();
		print_board();
		do {
			get_input(); // Get keyboard input
			updatePos(); // Update x and y coordinates for head
			collision(); // Detect collision
			update_tailPos(); // Need to update tail position AFTER eating apple.
			print_board();
			Sleep(speed); 
			// Exit game if ESC key is pressed
			if (exit_game == true)
			{
				cout << "\n Exiting . . .\n";
				Sleep(1000);
				return 0;
			}
		} while (!game_over);
		score_screen(); // Display score screen after a game over.
	} while (exit_game==false); 
}

int select_difficulty()
{
	char cursor[3];
	char cursorPos = 0;
	do {
		// Option list
		for (int i = 0; i < 3; i++)
		{
			cursor[i] = ' ';
		}
		cursor[cursorPos]='>';
		system("CLS");
		cout << " --------SNAKE--------\n\n"
			<< " Select dificulty: \n\n"
			<< cursor[0] << "Easy \n"
			<< cursor[1] << "Hard \n"
			<< cursor[2] << "Insane \n\n\n"
			<< " ------CONTROLS------ \n"
			<< " Movement: WASD\n"
			<< " Select: J\n"
			<< " Exit game: ESC\n ";
		int input;
		input = _getch();
		if (input == 'W' || input == 'w')
		{
			cursorPos -= 1;
			if (cursorPos < 0){cursorPos=2;} // Loop around to bottom element if pos = 0
		} 
		else if (input == 'S' || input == 's') 
		{
			cursorPos += 1;
			cursorPos = cursorPos % 3; // Loop around to top element if pos = max.
		}
		else if (input == 'J' || input == 'j') {
			return cursorPos; // If j is pressed, return the position of cursor. This will determine difficulty.
		} 
		else if (input == ESC) { exit_game = true; return 0; }
	} while (true);
}

void spawn_fruit()
{
	bool validSpawn = false;
	do {
		fruitX = rand() % WIDTH;
		fruitY = rand() % HEIGHT;
		// Fruit will only be spawned in empty spaces (' ').
		if (board[fruitY][fruitX] == ' ') {
			validSpawn = true;
		}
	} while (validSpawn== false);
}

void get_input()
{	
	if (_kbhit()) // If keyboard key is struck
	{
		switch (_getch())
		{
		case'W':
		case'w':
			if (dir != DOWN) {dir = UP;} // Moving in opposite direction is not allowed
			break;
		case'A':
		case'a':
			if (dir != RIGHT) { dir = LEFT; }
			break;
		case'S':
		case's':
			if (dir != UP) { dir = DOWN; }
			break;
		case'D':
		case'd':
			if (dir != LEFT) { dir = RIGHT; }
			break;
		case ESC:
			exit_game = true;
			break;
		default: 
			break;
		}
	}
}

void update_tailPos()
{
	//Tail positions are stored in the tailX and tailY arrays. 
	for (int i = tail_length; i > 0; i--)
	{
		/*Set tail X and Y position equal to the tail element before it.
		In void updatePos(), tailX[0] and tailY[0] is set to the previous head position before updating it. 
		Therefore, the first tail element will move to previous head postion.*/
		tailX[i] = tailX[i - 1]; 
		tailY[i] = tailY[i - 1];
	}
}

void updatePos()
{
	tailX[0] = xPos; // Set previous X and y values for head as start point for 1st tail element.
	tailY[0] = yPos;
	if (dir == UP) {yPos--;}
	else if (dir == LEFT) { xPos--; }
	else if (dir == DOWN) { yPos++; }
	else if (dir == RIGHT) { xPos++; }
}
void collision()
{
	if (xPos <= 0 || xPos >= WIDTH- 1 || yPos <= 0 || yPos >= HEIGHT-1) // If head position = border of map.
	{
		game_over = true;
	}
	else if (xPos == fruitX && yPos == fruitY) // When colliding with fruit.
	{
		score += 20;
		spawn_fruit();
		tail_length++;
	}
	else {
		for (int i = tail_length; i > 0; i--)  // When colliding with tail
		{
			if (xPos == tailX[i] && yPos == tailY[i])
			{
				game_over = true;
			}
		}
	}
}
void set_walls()  // Set map borders to be walls ('#'). Fill in the rest with empty spaces (' ').
{
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH -1)
			{
				board[i][j] = '#';
			}
			// Fill inn board with empty space
			else {
				board[i][j] = ' ';
			}
		}
	}

}

void print_board()
{
	// Instead of using system("CLS"), change cursor pos to top and redraw.
	COORD newpos = { 0,0 }; //position to 0, 0
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), newpos);

	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			if (board[i][j] != '#') // Fill in board with empty spaces to reset visuals of prevoius snake positions.
			{
				board[i][j] = ' '; 
			}
		}
	}
	board[yPos][xPos] = SnakeMark; // Print snake head.
	for (int i = tail_length; i > 0; i--) // Print snake tail.
	{
		board[tailY[i]][tailX[i]] = SnakeMark;
		
	}
	board[fruitY][fruitX] = FruitMark; // Print fruit.
	for (int i = 0; i < HEIGHT; i++) // Print empty spaces.
	{
		for (int j = 0; j < WIDTH; j++)
		{	
			cout << board[i][j] << " ";
		}
		cout << "\n";
	}
	// Print score display.
	cout << "\n Score: " << score << "  Tail length: " << tail_length << "\n";
}

void score_screen()
{
	system("CLS");
	cout << "\n **************GAME  OVER***************\n\n\n";
	cout << " SCORE: " << score << "\n\n Press any key to return to title screen.";
	_getch();
}

void hide_cursor() {
	HANDLE hOut;
	CONSOLE_CURSOR_INFO ConCurInf;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	ConCurInf.dwSize = 10;
	ConCurInf.bVisible = FALSE;
	SetConsoleCursorInfo(hOut, &ConCurInf);

}