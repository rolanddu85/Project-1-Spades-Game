#include <iostream> // Stream declarations
#include <vector> //Vectors used to store deck and players hands
#include <string> //String declarations
#include <algorithm> //Shuffle Method
#include "card.h"

#ifndef PLAYER_H
#define PLAYER_H

using namespace std;

class player {


	int score; //total score
	int bid; //bid for that round
	int tricksTaken; //score for thast round
	int sandBag; //the number of points you win, more than what you bid, every 10th bag = -100
	bool doubleNil;
	//vector <card> hand;

public:
	player();
	vector <card> hand;

	int getSandBag(){return sandBag;}
	bool getDoubleNil() {return doubleNil;}
	int getScore() {return score;}
	int getBid() {return bid;}
	int getTricksTaken() {return tricksTaken;}
	vector<card> getHand() {return hand;}

	void setSandBag(int i) {sandBag = i;}
	void setBid(int i) {bid = i;}
	int setTricksTaken(int i) {tricksTaken=i;}
	void setScore(int i) {score = i;}
	void setDoubleNil(bool i) {doubleNil = i;}

	//add card to end of vector
	void addCard(card b);
	void removeCard(card a);

};

#endif
