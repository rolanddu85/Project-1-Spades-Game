#include <iostream>
#include <sys/ioctl.h>
#include <cstdio>
#include <unistd.h>
#include <locale.h>
#include <ncursesw/ncurses.h>

#include "gamePlay.h"
#include <cstdlib>
#include <sstream>
#include <ctime>

vector<player> players(4, player());
vector <card> deck(52,card());
display monitor;
card center[4];

vector <card> gamePlay::getDeck(){return deck;}
vector <player> gamePlay::getplayers(){return players;}

//sorts the cards in the players hand into diamonds, clubs, hearts, spades
void gamePlay::handSort(){
	int spades[13];
	int hearts[13];
	int clubs[13];
	int diamonds[13];
	int index;
	int i;
	for(i=0; i<13; i++){ //determines the card number and places them into corresponding indexes
		index = (players.at(0).hand.at(i).getCardNum()+11)%13; //cause the cards to be placed based on their number with 2 being index 0 and 1(ace) being index 12
		switch(players.at(0).hand.at(i).getSuit())
		{
		case 1: spades[index] = 1;
		break;
		case 2: hearts[index] = 1;
		break;
		case 3: clubs[index] = 1;
		break;
		case 4: diamonds[index] = 1;
		break;
		default: mvprintw(3,2,"!!!!!!!we have a problem!!!!!!!!!!");
		break;
		}
	}
	i = 0;
	while(i<13){ //being placing the ordered cards back into the players hand
		int j = 0;
		while(j < 13){
			if(diamonds[j] == 1){ //if a card has been placed in this index for the diamonds only array
				if(j+2 == 14) //if the card is an ace
					players.at(0).hand.at(i).setCardNum(1);
				else
					players.at(0).hand.at(i).setCardNum(j+2); //add 2 to each index to get the card number
				players.at(0).hand.at(i).setSuit(4);
				i++;
			}
			j++;
		}
		j = 0;
		while(j < 13){
			if(clubs[j] == 1){
				if(j+2 == 14)
					players.at(0).hand.at(i).setCardNum(1);
				else
					players.at(0).hand.at(i).setCardNum(j+2);
				players.at(0).hand.at(i).setSuit(3);
				i++;
			}
			j++;
		}
		j = 0;
		while(j < 13){
			if(hearts[j] == 1){
				if(j+2 == 14)
					players.at(0).hand.at(i).setCardNum(1);
				else
					players.at(0).hand.at(i).setCardNum(j+2);
				players.at(0).hand.at(i).setSuit(2);
				i++;
			}
			j++;
		}
		j = 0;
		while(j < 13){
			if(spades[j] == 1){
				if(j+2 == 14)
					players.at(0).hand.at(i).setCardNum(1);
				else
					players.at(0).hand.at(i).setCardNum(j+2);
				players.at(0).hand.at(i).setSuit(1);
				i++;
			}
			j++;
		}
	}
}

//compares the center pile of 4 played cards to determine who wins the trick
int gamePlay::compareCenter(int leadplayer){
	int highest = center[leadplayer].getCardNum();
	if(center[leadplayer].getCardNum() == 1)
		highest = 14;
	int suit = center[leadplayer].getSuit();
	int player = leadplayer;

	for(int i = leadplayer+1; i < leadplayer+4; i++)
	{
		if(center[i%4].getSuit() == 1)
			setSpadesBroken(true);
		if((suit != 1) && (center[i%4].getSuit() == 1))
		{
			player = i%4;
			suit = 1;
			highest = center[i%4].getCardNum();
		}
		if(suit == center[i%4].getSuit()){
			if(center[i%4].getCardNum() == 1){
				player = i % 4;
				highest = 14;
			}
			if(highest < center[i%4].getCardNum())
			{
				player = i%4;
				highest = center[i%4].getCardNum();
			}
		}
	}
	players.at(player).setTricksTaken(players.at(player).getTricksTaken()+1); //increments the trick count of the winning player
	return player; //return the player who won to determine who goes first next turn
}

//Create the deck of 52 cards by setting the suit and number of each card to a nonzero integer
void gamePlay::createDeck() {
	for(int j = 0; j < 52; j++)
	{
		deck.at(j).setCardNum((j%13)+1);
		deck.at(j).setSuit((j/13)+1);
	}
	random_shuffle(deck.begin(), deck.end());
}

//deal out 13 cards to each player by setting the
void gamePlay::deal(vector <card> &newdeck, vector <player> &newplayers){
	for(int i = 0; i<52; i++){
		newplayers.at(i/13).addCard(newdeck.at(i));
		newdeck.at(i).setSuit(0);
		newdeck.at(i).setCardNum(0);
	}
}

//determines if the player still has a card of the same suit in their hand as the leading card played
bool gamePlay::containSuit(card lead, player players){
	bool suit = false;
	for(int i = 0; i < players.getHand().size(); i++){
		if(lead.getSuit() == players.getHand().at(i).getSuit())
			suit = true;
	}
	return suit;
}

//determines if the player has only spades cards left in their hand
bool gamePlay::onlySpade(player play){
	for(int i = 0; i<play.getHand().size(); i++){
		if(play.getHand().at(i).getSuit()!=1)
			return false;
	}
	return true;
}

//determines if the position the player is clicking on the screen actually points to a playable card 
//and then returns the position of that card based on the player's hand vector of type <card>
int gamePlay::handCheck(int xevent, int yevent, vector <player> players, int trickStart){
	int i = xevent/6;
	//first check to find the card on the display
	if(i>=0 && i<players.at(0).getHand().size() && yevent>17 && yevent<23 &&
			players.at(0).getHand().at(i).getSuit() != 0 &&
			players.at(0).getHand().at(i).getCardNum() != 0)
	{
		card playedCard = players.at(0).getHand().at(i);
		//check to see if leading the current round or not and if spades are "broken"
		if(trickStart==0 && !getSpadesBroken()){
			if(onlySpade(players.at(0)))
				return i;
			else if(playedCard.getSuit() != 1)
				return i;
			else
				return (-1);
		}
		if(trickStart == 0 && getSpadesBroken())
			return i;
		//if not leading, then call the containsuit method to see if your hand contains one of similar suit
		if(trickStart > 0 && containSuit(center[trickStart],players.at(0))){
			if(playedCard.getSuit()==center[trickStart].getSuit())
				return i;
		}
		if(trickStart > 0 && !containSuit(center[trickStart],players.at(0)))
			return i;
		else
			return (-1);
	}
	mvprintw(4,3,"invalid card");
	return (-1);
}

//draws the cards in the player's hand if it contains a valid card, erase the card otherwise if invalid 
void gamePlay::displayHand(){
	int offset = 0;
	for(int i =0; i<players.at(0).getHand().size(); i++){
		if(players.at(0).getHand().at(i).getSuit() != 0)
			monitor.displayCard(offset, 18, players.at(0).getHand().at(i).getSuit(), players.at(0).getHand().at(i).getCardNum(), 0);
		else
			monitor.eraseBox(offset, 18, 6, 5);
		offset+=6;
	}
}

//determins the position of a mouse click and sends that to handcheck(), and then sends the card to the center pile array to be scored
void gamePlay::humanPlay(int trickStart){
	int xevent, yevent;
	for(;;){
		mvprintw(3,2,"Please choose a card to play.");
		int key = monitor.captureInput();
		// if a mouse click occurred
		if (key == -1) {
			xevent = monitor.getMouseEventX();
			yevent = monitor.getMouseEventY();
			int handCh = handCheck(xevent, yevent, players, trickStart);
			if(handCh != (-1)){ //if the card is valid
				card played = players.at(0).getHand().at(handCh);
				players.at(0).hand.at(handCh).setCardNum(0);
				players.at(0).hand.at(handCh).setSuit(0);
				center[0]= played;
				monitor.displayCard(39, 12, center[0].getSuit(), center[0].getCardNum(), 0);
				displayHand();
				break;
			}
			else
				mvprintw(4,3,"invalid card");
		}
	}
}

//loops through a computer players hand and checks to see if the random card is playable within the rules of the game
void gamePlay::CPUplay(int trickStart, int CPU){
	bool goodCard = false;
	card playedCard =players.at(CPU).getHand().at(0);
	int i;
	for(i = 0; i < players.at(CPU).getHand().size(); i++){
		if(players.at(CPU).getHand().at(i).getSuit() != 0 &&
				players.at(CPU).getHand().at(i).getCardNum() != 0){
			playedCard = players.at(CPU).getHand().at(i);
			//check to see if leading or not
			if(trickStart==CPU && !getSpadesBroken()){
				if(onlySpade(players.at(CPU)))
					break;
				if(playedCard.getSuit()!=1)
					break;
			}
			if(trickStart == CPU && getSpadesBroken())
				break;
			//if not leading use contains suit function
			if(trickStart != CPU && containSuit(center[trickStart], players.at(CPU)))
				if(playedCard.getSuit()==center[trickStart].getSuit())
					break;
			if(trickStart != CPU && !containSuit(center[trickStart], players.at(CPU)))
				break;
		}
	}
	players.at(CPU).hand.at(i).setCardNum(0);
	players.at(CPU).hand.at(i).setSuit(0);
	center[CPU]= playedCard;
	if(CPU==1)
		monitor.displayCard(29, 7, center[CPU].getSuit(), center[CPU].getCardNum(), 0);
	if(CPU==2)
		monitor.displayCard(39, 2, center[CPU].getSuit(), center[CPU].getCardNum(), 0);
	if(CPU==3)
		monitor.displayCard(49, 7, center[CPU].getSuit(), center[CPU].getCardNum(), 0);
}

//scores each team with the player being teamed with CPU 2 and the other team being CPU 1 and CPU 3
void gamePlay::score(player &play, player &play2){
	int trickOver = play.getTricksTaken()-play.getBid(); // Calculate the difference between bid and actually taken.
	if(play.getBid() == NULL)
		trickOver = 0;
	int trickOver2 = play2.getTricksTaken()-play2.getBid(); // Calculate the difference between bid and actually taken.
	int totalBid = play.getBid()+play2.getBid();
	int totalTrick = trickOver+trickOver2;
	//Bidding Double Nil (if gets it 200 points other wise -200 points)
	if(play.getDoubleNil()){
		if(play.getTricksTaken()==0) //player did get Double Nil successfully
			play.setScore(play.getScore()+200); // add 200 points
		else
			play.setScore(play.getScore()-200);
	}
	else if(play.getBid()==0){ //Bidding Nil (if gets it 100 points other wise -100 points)
		if(play.getTricksTaken()==0) //player did get Nil successfully
			play.setScore(play.getScore()+100); //add 100 points
		else //player didnt get Nil
			play.setScore(play.getScore()-100); //take away 100 points
	}
	if(totalTrick < 0){ //player bids more than number of tricks won
		play.setScore(play.getScore()+(totalBid*(-10)));  //decrease score by 10 poitns for every overtrick
	}
	else if(totalTrick >= 0){ //player bids less then number of tricks won
		play.setSandBag(play.getSandBag() + totalTrick); //increase sandbag by 1
		play.setScore(play.getScore()+totalTrick+(10*totalBid)); //increase 10 points per trick bid on and 1 point per trick over
	}
	if(play.getSandBag()>10){ //check for sandbagging
		play.setScore(play.getScore()-100);
		play.setSandBag(play.getSandBag()-10);
	}
	play.setBid(NULL); //reset players bid to NULL
	play2.setBid(NULL); //reset players bid to NULL
	play.setTricksTaken(0);
	play2.setTricksTaken(0);
	play.setDoubleNil(false); //Player has not yet bid double NILL.
}

//loops infinitely until a mouse click or key input is detected, then sets bid accordingly
void gamePlay::storeBid(stringstream &msg){
	int xevent;
	int yevent;
	for(;;){
		mvprintw(3,2,"Click the box bid Double Nil or type out your bid now. \n  Use the keys 1-9, a for 10, b for 11, c for 12, and d for 13.");
		int key = monitor.captureInput();
		monitor.drawBox(2, 5, 3, 2, 0);
		xevent = monitor.getMouseEventX();
	    yevent = monitor.getMouseEventY();
		if (key == -1)
			if((xevent>=2 && xevent<4)&&(yevent>=5 && yevent <7)){
				msg.str("");
				msg << "Your bid: double nil";
				monitor.bannerTop(msg.str());
				players.at(0).setDoubleNil(true);
				break;
			}
			else{
				msg.str("");
				msg << "Your bid is horrible. Bid again!!!!";
				monitor.bannerTop(msg.str());
			}
		if ((key-48) >= 0 && (key-48) <= 9){
			msg.str("");
			msg << "Your bid: " << (key-48);
			monitor.bannerTop(msg.str());
			players.at(0).setBid(key-48);
			break;
		}
		else if(key>=97 && key<=100){
			msg.str("");
			msg << "Your bid: " << (key-87);
			monitor.bannerTop(msg.str());
			players.at(0).setBid(key-87);
			break;
		}
		else if(key!=0 && key!=-1){
			msg.str("");
			msg << "Your bid is horrible. Bid again!!!!";
			monitor.bannerTop(msg.str());
		}
	}
	monitor.eraseBox(1, 1, 62, 16);
}

//main function that runs the entire game
void gamePlay::runGame(){

	stringstream messageString;
	srand(time(NULL));

	int count;
	int handStart = 0;
	int trickStart;
	while(players.at(0).getScore()<500 && players.at(1).getScore()<500)
	{
		createDeck();

		deal(deck, players); //deal out 13 cards to each player
		handSort(); // sort the human players hand
		displayHand(); 

		storeBid(messageString); //asks for bid
		
		//sets the bid randomly between 2-4 for each computer player
		players.at(1).setBid(rand()%3+2);
		players.at(2).setBid(rand()%3+2);
		players.at(3).setBid(rand()%3+2);
		//displays the current bid and scores of each team
		messageString << "  Partner: " << (players.at(2).getBid())
		<< "  CPU 1: " << players.at(1).getBid()
		<< "  CPU 3: " << players.at(3).getBid()
		<< "    Team Score = " << players.at(0).getScore()
		<< "  Enemy Score = " << players.at(1).getScore();
		monitor.bannerTop(messageString.str());

		trickStart = handStart;
		count = 0;
		setSpadesBroken(false);
		while(count<13){ // simulates the different cases where the human player plays 1st, 2nd, 3rd, last
			bool success = true;
			switch (trickStart)
			{
			case 0: humanPlay(trickStart);
			CPUplay(trickStart,1);
			CPUplay(trickStart,2);
			CPUplay(trickStart,3);
			break;
			case 3: CPUplay(trickStart,3);
			humanPlay(trickStart);
			CPUplay(trickStart,1);
			CPUplay(trickStart,2);
			break;
			case 2: CPUplay(trickStart,2);
			CPUplay(trickStart,3);
			humanPlay(trickStart);
			CPUplay(trickStart,1);
			break;
			case 1: CPUplay(trickStart,1);
			CPUplay(trickStart,2);
			CPUplay(trickStart,3);
			humanPlay(trickStart);
			break;
			default: mvprintw(3,2,"!!!!we have a problem!!!!");
			success = false;
			break;
			}
			monitor.eraseBox(3, 2, 20, 2);
			
			for(;;){//pause to wait for an acknowledgment mouse click
				int key = monitor.captureInput();
				monitor.eraseBox(3, 2, 30, 4);
				mvprintw(3,2,"click anywhere to continue");
				if(key == -1){
					break;
				}
			}
			//if no errors appear, display the tricks taken
			if(success){
				monitor.eraseBox(1, 1, 62, 16);
				trickStart = compareCenter(trickStart);
				messageString.str("");
				messageString << "Your Tricks= " << players.at(0).getTricksTaken()
				<< "   Partner= " << players.at(2).getTricksTaken()
				<< "   CPU 1= " << players.at(1).getTricksTaken()
				<< "   CPU 3= " << players.at(3).getTricksTaken();
				monitor.bannerBottom(messageString.str());
				count++;
			}
		}

		//score the round and tally up for each team
		for(int n =0; n<2;n++)
			score(players.at(n),players.at(n+2));
		messageString.str("");
		monitor.bannerTop(messageString.str());
		messageString << "Your Tricks = " << players.at(0).getTricksTaken()
		<< "   Partner = " << players.at(2).getTricksTaken()
		<< "   CPU 1 = " << players.at(1).getTricksTaken()
		<< "   CPU 3 = " << players.at(3).getTricksTaken();
		handStart = (handStart +1) % 4;
	}
}





