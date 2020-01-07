#include <curses.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define CLK 100000

#define DIR_UP 0
#define DIR_RIGHT 1
#define DIR_DOWN 2
#define DIR_LEFT 3

#define K_UP 65
#define K_DOWN 66
#define K_RIGHT 67
#define K_LEFT 68

struct Snake {
	unsigned y;
	unsigned x;
	unsigned dir;
	struct Snake* prev;
	struct Snake* next;
};

struct Pellet {
	unsigned y;
	unsigned x;
};

int main () {

	// curses init
	initscr();
	noecho();
	cbreak();

	// getScreenSize
	int yMax, xMax;
	getmaxyx(stdscr, yMax, xMax); //make this dynamic (maybe)

	// creating window
	WINDOW * win = newwin(yMax-2,xMax-3, 1, 2);
	clear();
	curs_set(0);
	box(stdscr, 0, 0);
	nodelay(win, true);
	nodelay(stdscr, true);
	refresh();
	wrefresh(win);

	keypad(win, true);	
	

	//variable to store input
	int input_buffer = ERR;
	int input = K_RIGHT;

	//variables for time between turns
	time_t last_time, curr_time;
       	last_time = clock();
	
	//declare snake with 3 segments
	struct Snake* head = NULL;
	struct Snake* second = NULL;
	struct Snake* third = NULL;

	//allocate space
	head = (struct Snake*) malloc (sizeof(struct Snake));
	second = (struct Snake*) malloc (sizeof(struct Snake));
	third = (struct Snake*) malloc (sizeof(struct Snake));

	//initialize snake values
	head->y = (yMax - 2)/2;
	head->x = (xMax - 3)/2;
	head->dir = DIR_RIGHT;
	head->prev = NULL;
	head->next = second;

	second->y = head->y;
	second->x = (head->x) - 2;
	second->prev = head;
	second->next = third;
	
	third->y = head->y;
	third->x = (second->x) - 4;
	third->prev = second;
	third->next = NULL;

	struct Snake* last = third;
	
	//creating pellet
	struct Pellet pellet;
	srand(clock());
	pellet.y = rand() % (yMax - 2);
	srand(clock());
	pellet.x = rand() % (xMax -4);

	
	while(1) 
	{
		curr_time = clock();		
		
		//get_input
		input_buffer = getch();
		if ((input_buffer != ERR)) {
			input = input_buffer;
		}
		

		if ((curr_time - last_time) > CLK)
		{
			//update clock
			last_time = curr_time;

		
			//clean screen for next frame
			werase(win);


			//eat, respawn pellet, and grow snake
			if (head->y == pellet.y){
				if ((head->x == pellet.x) || (head->x == (pellet.x + 1))){

					srand(clock());
					pellet.y = rand() % (yMax - 2);
					srand(clock());
					pellet.x = rand() % (xMax -4);

					struct Snake* new = (struct Snake*) malloc (sizeof(struct Snake));
					new->prev = last;
					last = new;
 
				}
			}
			
			//print pellet
			mvwprintw(win, pellet.y,pellet.x, "<>");

			struct Snake* curr = last;

			//update position
			while ((curr->prev) != NULL) {

				curr->x = (curr->prev)->x;
				curr->y = (curr->prev)->y;
				mvwprintw(win, curr->y, curr->x, "@");
				curr = curr->prev;

			}

			//update head dir
			switch (input) {
				case K_UP:
					if ((head->dir) != DIR_DOWN) {
						head->dir = DIR_UP;
						head->y -= 1;	
					}
					else
						head->y += 1;
					break;
				case K_RIGHT: 
					if ((head->dir) != DIR_LEFT) {
						head->dir = DIR_RIGHT;
						head->x += 2;
					}
					else
						head->x -= 2;
					break;
				case K_LEFT:
					if ((head->dir) != DIR_RIGHT) {
						head->dir = DIR_LEFT;
						head->x -= 2;
					}
					else 
						head->x += 2;
					break;
				case K_DOWN: 
					if ((head->dir) != DIR_UP) {
						head->dir = DIR_DOWN;
						head->y += 1;
					}
					else
						head->y -= 1;
					break;
				default:
					break;
			}

			//check for collision 
			curr = last;
			
			while (curr->prev != NULL){
				if ((curr->x == head->x) && (curr->y == head->y)) {
					endwin();
					return 0;
				}

				curr = curr->prev;
			}


			//logic for appearing on other side after wall
			if (head->x > xMax-3)
				head->x = 1;
			else if (head->x < 2)
				if (head->dir == DIR_LEFT)
					head->x = xMax-5;
			
			if(head->y > yMax-2)
				head->y = 0;
			else if (head->y == 0)
				if (head->dir == DIR_UP)
					head->y = yMax-2;
			
			//print head
			mvwprintw(win, head->y, head->x, "@");
			

			refresh();
			wrefresh(win);

		}

	}
	
	endwin();
	return 0;

}
