#include "player.h"
#include "display.h"

#ifndef gamePlay_H
#define gamePlay_H

using namespace std;

class gamePlay{

    bool spadesBroken;

public:
    vector <card> getDeck();
	vector <player>getplayers();
	bool getSpadesBroken() {return spadesBroken;}
	void setSpadesBroken(bool b){spadesBroken = b;}

	int compareCenter(int leadplayer);
	void createDeck();
   	void deal(vector <card> &deck, vector <player> &players);
	void handSort();
	bool containSuit(card lead, player players);
	bool onlySpade(player play);

	int handCheck(int xevent, int yevent, vector <player> players, int trickStart);

	void displayHand();
	void humanPlay(int trickStart);
	void CPUplay(int trickStart, int CPU);
	void score(player &play, player &play2);
	void storeBid(stringstream &msg);
	void runGame();

};

#endif
