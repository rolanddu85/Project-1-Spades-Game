#include <iostream> // Stream declarations
#include <vector> //Vectors used to store deck and players hands
#include <string> //String declarations
#include <algorithm> //Shuffle Method
#include <sys/ioctl.h>
#include <cstdio>
#include <unistd.h>
#include <locale.h>
#include <ncursesw/ncurses.h>
#include "player.h"

using namespace std;


player::player() {
	score =0; //total score
	bid = 0; //bid for that round
	tricksTaken = 0; //score for thast round
	sandBag = 0; //the number of points you win, more than what you bid, every 10th bag = -100
	doubleNil = false;
	for(int i=0; i<13; i++)
        hand.push_back(card());
}

void player::addCard(card b){
    for (int i=0; i<hand.size(); i++){
		//compare card being played to the ones in your hand to search and determine which one to erase
		if((hand.at(i).getCardNum() == 0) &&
			(hand.at(i).getSuit() == 0))
		{
			hand.at(i).setCardNum(b.getCardNum());
			hand.at(i).setSuit(b.getSuit());
			return;
		}
	}
}

void player::removeCard(card a) {
	for (int i=0; i<hand.size(); i++){
		//compare card being played to the ones in your hand to search and determine which one to erase
		if((hand.at(i).getCardNum() == a.getCardNum()) &&
			(hand.at(i).getSuit() == a.getSuit()))
		{
			hand.at(i).setCardNum(0);
			hand.at(i).setSuit(0);
			return;
		}
	}
}
