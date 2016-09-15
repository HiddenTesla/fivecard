#ifndef _FIVECARD_HH_
#define _FIVECARD_HH_

#include <iostream>

const int SIZE_OF_FULLHAND = 5;
const int NUM_OF_SUITES    = 4; 
const int NUM_OF_NOMINALS  = 13;
const int SIZE_OF_DECK     = NUM_OF_SUITES * NUM_OF_NOMINALS;

enum Suite
{
    CLUB,
    DIAMOND,
    HEART,
    SPADE,
};

struct Card
{
    Suite suite;
    int rank;
    bool isSet; // Use this flag to indicate whether this card has been set
                // Avoid use of pointers
    Card(): isSet(false) {}
    Card(int n) {assign(n);}
    Card& operator =(int n) {assign(n); return *this;}
    void assign(int n);
    
    void print() const; // Maybe std::ostream is better?

    bool operator >(const Card&) const;
};

std::ostream& operator <<(std::ostream&, const Card&);

#endif
