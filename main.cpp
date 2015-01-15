/* Title: Project 1 ECE373 Example driver artifact for Display class
 * Author: John Shield
 * Description: Demonstrates Unix Signal capturing and the Display class
 *		functionality.
 *
 * NOTES:
 *      * Requires the terminal (Putty) to be set to UTF-8.
 *      * Does not function when running a screen session.
 */

//#include "display.h"
#include <signal.h>
#include <ncurses.h>
#include <cstdlib>
#include <sstream>
#include "gamePlay.h"

using namespace std;

/* No Header file for this example driver artifact
 * function declaration here instead.
 */
// Signal Subroutine for Window Resize
static void detectResize (int sig);
// stub artifact for what the game does when the screen resizes
void stub_PrintResize(void);

// The gameDisplay object is global, because the static signal handler object
// needs to access the dynamic object.
display gameDisplay;

/*
 * This is the main function that starts the driver artifact.
 * This function demonstrates some of the abilities of the Display class
 */

 int main(int argc, char* argv[])
{
	gamePlay spades;
	spades.runGame();
}


/*int main(int argc, char* argv[])
{
	// using a stringstream rather than a string to make making the banner easier
	stringstream messageString;

	// various variable declarations
	char key;
	int cardX = 0;
	int cardY = 0;
	int suit = 0;
	int number = 0;

	int dragX = 0;
	int dragY = 0;

	// enable a interrupt triggered on a window resize
	signal(SIGWINCH, detectResize); // enable the window resize signal

///* You can uncomment and change the colors for various cards here/
//    init_pair(1, COLOR_CYAN, COLOR_BLACK); // for card outline
//    init_pair(2, COLOR_BLUE, COLOR_BLACK); // for spades and clubs
//    init_pair(3, COLOR_RED, COLOR_BLACK);  // for hearts and diamonds
//    init_pair(4, COLOR_GREEN, COLOR_BLACK); // for turned over card
//    init_pair(5, COLOR_GREEN, COLOR_BLACK); // for box drawing
//    init_pair(6, COLOR_GREEN, COLOR_BLACK); // for banner display


	// infinite loop for the main program, you can press ctrl-c to quit
	for (;;) {
		// calls the game display to capture some input
    	key = gameDisplay.captureInput();
		// if a mouse event occurred
		if (key == -1) {
			// make a banner message
			messageString.str("");
			messageString << "A mouse event occurred x=" \
				<< gameDisplay.getMouseEventX() << ", y=" \
				<< gameDisplay.getMouseEventY() << ", bstate=" \
				<< gameDisplay.getMouseEventButton();
			// display a banner message
			gameDisplay.bannerTop(messageString.str());
			// record the location of the mouse event
			cardX = gameDisplay.getMouseEventX();
			cardY = gameDisplay.getMouseEventY();
			// Some of the mouse click values are defined in display.h
			// check if it was a left click
			if (gameDisplay.getMouseEventButton()&LEFT_CLICK) {
				// draw a random card at the click location
				suit = rand()%5;
				number = rand()%15;
				gameDisplay.displayCard(cardX,cardY,suit,number, A_BOLD);
			// check if it was a right click
			} else if (gameDisplay.getMouseEventButton()&RIGHT_CLICK) {
				// erase a portion of the screen in the shape of a card
				gameDisplay.eraseBox(cardX,cardY,6,5);
			// check for the start of a drag click
			} else if (gameDisplay.getMouseEventButton()&LEFT_DOWN) {
				// record start of the drag
				dragX = cardX;
				dragY = cardY;
			// when the mouse is released
			} else if (gameDisplay.getMouseEventButton()&LEFT_UP) {
				// calculate size of the drag
				int sizeX = abs(dragX-cardX);
				int sizeY = abs(dragY-cardY);
				// get to the top left corner of the drag area
				if (dragX > cardX)
					dragX = cardX;
                if (dragY > cardY)
                    dragY = cardY;
				// draw a box around the drag area
				gameDisplay.drawBox(dragX, dragY, sizeX, sizeY, 0);
			}
		// if a key was pressed
		} else if(key > 0) {
			// make bottom a banner message saying that a key was pressed
			messageString.str("");
			messageString << "Key " << key << " pressed";
			gameDisplay.bannerBottom(messageString.str());
		}
	}

	return 0;
}
*/



/*
 * This is the interrupt service routine called when the resize screen
 * signal is captured.
 */
void detectResize(int sig) {
	// update the display class information with the new window size
    gameDisplay.handleResize(sig);
	// re-enable the interrupt for a window resize
    signal(SIGWINCH, detectResize);
	/*INSERT YOUR OWN SCREEN UPDATE CODE instead of stub_PrintResize*/
	stub_PrintResize();
}

/*
 * This is a simple stub that should be replaced with what the game does
 * when the screen resizes.
 */
void stub_PrintResize(void) {
	// gets the new screen size
	int cols = gameDisplay.getCols();
	int lines = gameDisplay.getLines();
	// setups a message stream
	stringstream messageString;
	messageString << "Terminal is " << cols << "x" << lines;
	// prints out the information of the new screen size in a top banner
	gameDisplay.bannerTop(messageString.str());
}

