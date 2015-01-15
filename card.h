#ifndef CARD_H
#define CARD_H

using namespace std;

class card
{
    int cardSuit;
    int cardNum;

    public:
    card();
    card(int suit, int number);
    int getSuit() {return cardSuit;}
	int getCardNum() {return cardNum;}
	void setSuit(int a){ cardSuit = a;}
	void setCardNum(int a){cardNum = a;}

};

#endif // CARD_H
