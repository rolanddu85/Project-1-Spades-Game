#include <iostream>
#include <sys/ioctl.h>
#include <cstdio>
#include <unistd.h>
#include <locale.h>
#include <ncursesw/ncurses.h>

#include "card.h"

card::card()
{
    cardSuit = 0;
    cardNum = 0;
}

card::card(int suit, int number)
{
    cardSuit = suit;
    cardNum = number;
}
