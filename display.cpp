/* Title: Project 1 ECE373 Display Class C++ File
 * Author: John Shield
 * Description: Defines the functionality of the user interface.
 *
 * NOTES: 
 *		* Requires the terminal (Putty) to be set to UTF-8.
 *		* Does not function when running a screen session.
 */

#include <iostream>
#include <sys/ioctl.h>
#include <cstdio>
#include <unistd.h>
#include <locale.h>
#include <ncursesw/ncurses.h>

#include "display.h"

using namespace std;

/* Function: This is the constructor.
 * Description: It is called whenever an object of class display is initialized
 */
display::display(void) {
	/* Initilization of Terminal */
	// required to get card suits displaying, combined with UTF-8 set in terminal
	setlocale(LC_ALL,"en_US.utf-8");

	// initialize ncurses screen
	initscr();
	// allow for color settings
	start_color();
	// clear the screen
	clear();
	// turn off the keyboard echo (reqiured while drawing)
	noecho();
	// Change to character mode (so individual characters are being read at a 
	// time rather than waiting for a carriage return).
	cbreak(); 
	// Allows for function keys to be used (also nessacary for getting the mouse 
	// movement working).	
	keypad(stdscr, TRUE);
	// set which mouse events are captured 
	mousemask(ALL_MOUSE_EVENTS, NULL); 
	// Setting the timeout for the capture input values are in 1/10ths of a second.	
	halfdelay(5); 

	// setup the screen size settings.
	cols = 80;
    lines = 24;
	// this updates the locally stored col and line variables
	handleResize(0);
	// set a no card draw offset of 1 so the bottom banner is not overwritten
	lineBoundaryOffset = 1;

	// Settings for card colors (these can be set outside of the display class)
	init_pair(1, COLOR_CYAN, COLOR_BLACK); // for card outline
	init_pair(2, COLOR_BLUE, COLOR_BLACK); // for spades and clubs
	init_pair(3, COLOR_RED, COLOR_BLACK);  // for hearts and diamonds
	init_pair(4, COLOR_GREEN, COLOR_BLACK); // for turned over card
	init_pair(5, COLOR_GREEN, COLOR_BLACK); // for box drawing
	init_pair(6, COLOR_GREEN, COLOR_BLACK); // for banner display
}

/* Function: This is the destructor.
 * Description: This is called just before an object is deleted.
 */
display::~display() {
	// this is turns off all the special settings and returns the terminal to normal
	endwin(); 
	// insert deletion of dynamically created objects here too
}

/*
 * Function: This captures all the userinput.
 * Description: It captures mouse and keyboard events.
 * 		Returns "Positive Number" 
 *			- for user keypress
 *			- this is a character code typed
 * 		Returns "0" - for no user input
 *			- this is when nothing is typed for half a second
 *			- allows for other timed operations to occur
 *		Returns "-1" - for mouse event
 *			- details of the mouse event must be fetched from this class
 *			- use getMouseEventX, getMouseEventY and getMouseEventButton
 */
int display::captureInput(void) {	
	// obtain one mouse event or keypress
	int ch=getch(); 
    // this is a switch statement for the result of getch
	switch (ch) {
    case KEY_MOUSE: // this occurs when an mouse event occurs
      {
        MEVENT mevent; // this is a variable declared of type MEVENT
        getmouse(&mevent); // this gets the mouse event from ncurses (library)
		mouseEventX = mevent.x; // get the column location of the event
    	mouseEventY = mevent.y; // get the row location of the event
		mouseEventButton = mevent.bstate; // get the button state of the mouse
		return -1; // -1 is for a mouse event
      }
      break;
	case ERR: // this occurs when there is a timeout
	  {
		return 0; // 0 is when nothing occurs
	  }
	  break;
    default: // this occurs when a key is pressed
	  return ch; // a character is when the user types something
    }

	return 0; // this is never called, but is used to stop compiler complaints
}

/*
 * Function: Updates all the information in the display class on window resize
 * Description: This function just updates information, it requires the call
 *		from a static singal handler. Signal handlers service interrupts and require
 *		a static function to be called, because they are not part of the main
 * 		program control flow. The signal handler should be declared in the main
 *		class, because your game should redraw the display on a resize.
 */
void display::handleResize(int sig) {
#ifdef TIOCGSIZE // operating system dependant differences
    struct ttysize ts;
    ioctl(STDIN_FILENO, TIOCGSIZE, &ts); // get the information of the terminal
    cols = ts.ts_cols;
    lines = ts.ts_lines;
#elif defined(TIOCGWINSZ)
    struct winsize ts;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &ts); // get the information of the terminal
    cols = ts.ws_col;
    lines = ts.ws_row;
#endif /* TIOCGSIZE */
	resizeterm(lines, cols); // sets the ncurses window size correctly
}

/*
 * Function: Displays various cards on the game screen
 * Description: This function displays various playing cards on the screen.
 *		The first two arguments are the x and y coordinates of the top left corner
 * 		of the card. 
 *			The suit values are: 1=spades, 2=hearts, 3=clubs, 4=diamonds
 * 			The numbers are: 1=Ace, 2-10=2-10, 11=Jack, 12=Queen, 13=King, 14=Joker
 *		Any suit and number that do not match the valid numberrs generates a face down
 *		card.
 *			The printAtt allows for one or more any of the following display settings:
 *				A_NORMAL        Normal display (no highlight)
 *				A_STANDOUT      Best highlighting mode of the terminal.
 *				A_UNDERLINE     Underlining
 *				A_REVERSE       Reverse video
 *				A_BLINK         Blinking
 *				A_DIM           Half bright
 *				A_BOLD          Extra bright or bold
 *				A_PROTECT       Protected mode
 *				A_INVIS         Invisible or blank mode
 *				A_ALTCHARSET    Alternate character set
 *				A_CHARTEXT      Bit-mask to extract a character
 *				COLOR_PAIR(n)   Color-pair number n 
 */
void display::displayCard(int x, int y, int suit, int number, int printAtt) {

	// Ncurses drawing settings
	attron(COLOR_PAIR(1) | printAtt);
	// prevent draw if it off the screen
	if (x>=0 && y>=0 && x<cols-6 && y<lines-lineBoundaryOffset) {
		// print the top lines of the card
		mvprintw(y,x,"\u250c\u2500\u2500\u2500\u2500\u2510");
		// the next 4 if statements prevent draw if it is over the bottom of the screen
		if (y<lines-1-lineBoundaryOffset) {
			move(y+1,x); // move command
			printFace(suit,number,0, printAtt); // call function to print card face
		}
		if (y<lines-2-lineBoundaryOffset) {
			move(y+2,x); // move command
			printFace(suit,number,1, printAtt); // call function to print card face
		}
		if (y<lines-3-lineBoundaryOffset) {
			move(y+3,x); // move command
			printFace(suit,number,2, printAtt); // call function to print card face
		}
		if (y<lines-4-lineBoundaryOffset) { 
			// prints the bottom lines of the card
			mvprintw(y+4,x,"\u2514\u2500\u2500\u2500\u2500\u2518");
		}
	}
	// Ncurses turn off the drawing settings
	attroff(COLOR_PAIR(1) | printAtt);
}

/*
 * Function: Print a single line of what is written on the card.
 * Description: This copies suit, number and printAtt from the calling function.
 *		Also includes what line of the card face is being drawn.
 */
void display::printFace(int suit, int number, int line, int printAtt) {	
	// draw left edge of the card
	printw("\u2502");

	if (suit==2 || suit==4) { // Red for Hearts and Diamonds
		attron(COLOR_PAIR(3) | printAtt);
	} else { // Black for Spades and Clover
		attron(COLOR_PAIR(2) | printAtt);
	}

	// this the display of the joker
	if (number==14) {
        if (line==0)
            printw("J%s  ", joker);
        if (line==1)
            printw("oker");
        if (line==2)
            printw("  J%s", joker);
	// this is the display for the cards with suits and numbers
	} else if (suit>=1 && suit <=4 && number>=1 && number<=13) {
		if (line==0) {
			printSuit(suit); // function to draw suit
			printNumber(number); // function to draw number
			if (number!=10)
				printw(" ");
			printw(" ");	
		} else if (line==2) {
			if (number!=10)
                printw(" ");
            printw(" ");
			printNumber(number); // function to draw number
			printSuit(suit);	// function to draw suit
		} else {
			printw("    ");
		}
	// this is for a face down card
	} else {
		// the face down cards have a special color
		attron(COLOR_PAIR(4) | printAtt);
		if (line==0)
			printw("%s  %s", spades, hearts);
		if (line==1)
            printw("Play");
		if (line==2)
			printw("%s  %s", diamonds, clubs);
		attroff(COLOR_PAIR(1) | printAtt);
	}

	// turn on the card edge color settings
    attron(COLOR_PAIR(1) | printAtt);
	// print the right edge of the card
	printw("\u2502");
}

/*
 * Function: Print the suit of the card
 * Description: This is just a look up table. 
 */
void display::printSuit(int suit) {
	switch (suit) {
		case 1:
            printw("%s",spades);
            break;
		case 2:
            printw("%s",hearts);
            break;
		case 3:
            printw("%s",clubs);
            break;
		case 4:
            printw("%s",diamonds);
            break;
		default:
			printw(" ");
			break;
	}
}

/*
 * Function: Prints the number on the card
 * Description: This is just a look up table.
 */
void display::printNumber(int number) {
    switch (number) {
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
		case 9:
		case 10:
            printw("%i",number);
            break;
        case 1:
            printw("%c",'A');
            break;
        case 11:
            printw("%c",'J');
            break;
        case 12:
            printw("%c",'Q');
            break;
        case 13:
            printw("%c",'K');
            break;
        default:
            printw(" ");
            break;
    }
}

/*
 * Function: Erases a rectangle on the screen
 * Description: x,y is for the top left corner, sizeX and sizeY set
 * 			how big the square is.
 */
void display::eraseBox(int x, int y, int sizeX, int sizeY) {
	string strDraw;
	int yCount;
	int maxSizeX;

	// this limits the column size of the draw when it is over the edge
	// of the drawing area
    if (sizeX+x > cols)
        maxSizeX=cols-x;
    else
        maxSizeX=sizeX;

	// for the number of rows that need to be drawn
	for (yCount=0; yCount<sizeY;yCount++) {
		// if the box goes over the edge of the drawable screen
		// stop drawing by breaking the loop
		if (yCount+y > lines-lineBoundaryOffset || y < 0)
			break;
		// reset the line to be drawn
		strDraw = "";
		// check that x is not off the screen
		if (x<=cols && x >= 0) {
			// make a string needed for box width	
			strDraw.append(maxSizeX,' ');
			// print the line of the box
			mvprintw(y+yCount,x,"%s",strDraw.c_str());
		}
	}
}

/*
 * Function: Draws a box on the screen
 * Description: x,y is for the top left corner, sizeX and sizeY set
 *          how big the square is. printAtt allows for changes in the
 *			display settings.
 */
void display::drawBox(int x, int y, int sizeX, int sizeY, int printAtt) {
	string strDraw;
    int ii;
    int yCount;

	// set the box setting colors on
	attron(COLOR_PAIR(5) | printAtt);    

	// for the box height being drawn loop
    for (yCount=0; yCount<sizeY;yCount++) {
		// break loop if the drawing is offscreen
        if (yCount+y > lines-lineBoundaryOffset || y < 0)
            break;
		// if x is on the screen
        if (x<=cols) {
			strDraw = "";
			// for the box width loop
            for (ii=0;ii<sizeX;ii++){
				// stop drawing if the x is offscreen
                if (ii+x > cols || x < 0)
                    break;
				// first line
                if (yCount==0) {
                    if (ii==0) {
						strDraw.append("\u250c"); // left
                    } else if (ii==sizeX-1) {
						strDraw.append("\u2510"); // right
                    } else {
						strDraw.append("\u2500"); // middle
                    }
				// last line
                } else if (yCount==sizeY-1) {
                    if (ii==0) {
                        strDraw.append("\u2514"); // left
                    } else if (ii==sizeX-1) {
                        strDraw.append("\u2518"); // right
                    } else {
                        strDraw.append("\u2500"); // middle
                    }
				// other lines
                } else {
                    if (ii==0) {
                        strDraw.append("\u2502"); // left
                    } else if (ii==sizeX-1) {
                        strDraw.append("\u2502"); // right
                    } else {
                        strDraw.append(" "); // middle
                    }
                }
			}
			// print the line that was created
			mvprintw(y+yCount,x,"%s",strDraw.c_str());
        }
    }
	// turn off the attribute colors
	attroff(COLOR_PAIR(5) | printAtt);
}

/*
 * Function: Draws a banner of text at the bottom right of the screen
 * Description: Inverts the color and draws the banner at the bottom
 *		of the screen. Does not handle carriage returns on the string.
 */
void display::bannerBottom(string bannerText) {
	// change to the banner draw settings
	attron(COLOR_PAIR(6) | A_REVERSE | A_BOLD);
	// checks if the banner string size is smaller than the width of the screen
    if((unsigned)cols > bannerText.size()) {
		// moves the cursor to the bottom of the screen
		move(lines-1,0);
		// fill in extra space to the banner text is right adjusted
        hline(' ',cols - bannerText.size());
		// prints out the banner text
		mvprintw(lines-1,cols-bannerText.size(),"%s", bannerText.c_str());
	// if banner string size is larger than width of screen
	} else {
		// clip the banner text so it doesn't wrap over to the next line
		mvprintw(lines-1,0,"%s", (bannerText.substr(0,cols)).c_str());
	}
	// turn off the draw colors
	attroff(COLOR_PAIR(6) | A_REVERSE | A_BOLD);
}

/*
 * Function: Draws a banner of text at the top left of the screen
 * Description: Inverts the color and draws the banner at the top
 *      of the screen. Does not handle carriage returns on the string.
 */
void display::bannerTop(string bannerText) {
	// change to the banner draw settings
	attron(COLOR_PAIR(6) | A_REVERSE | A_BOLD);
	// checks if the banner string size is smaller than the width of the screen
    if((unsigned)cols > bannerText.size()) {
		// moves the cursor to the bottom of the screen
		move(0,0);
		// prints out the banner text
		printw("%s", bannerText.c_str());
		// fill in extra space after the banner text
        hline(' ',cols - bannerText.size());
	// if banner string size is larger than width of screen
	} else {
		// clip the banner text so it doesn't wrap over to the next line
		mvprintw(0,0,"%s", (bannerText.substr(0,cols)).c_str());
	}
	// turn off the draw colors
	attroff(COLOR_PAIR(6) | A_REVERSE | A_BOLD);
}

