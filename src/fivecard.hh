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

enum CardRank
{
    RANK_2 = 1, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_9,
    RANK_10, RANK_J, RANK_Q, RANK_K, RANK_A,
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

enum HandRank
{
    UNDEFINED_HANDRANK = -1,
    NO_PAIR = 0,
    ONE_PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    STRAIGHT_FLUSH,
    ROYAL_FLUSH,
};

const char* getHandRankName(HandRank);

HandRank getHandRank(const Card* handCards);

std::ostream& operator <<(std::ostream&, const Card&);

struct HandInfo
{  
    // These nested structs and unions are more like Linux C kernel style
    // Poor readability
    union { 
        struct {
            int ranks[SIZE_OF_FULLHAND];
        } noPair;

        struct {
            int pairRank;
            int singles[SIZE_OF_FULLHAND - 2];
        } onePair;

        struct {
            int bigPairRank;
            int smallPairRank;
            int singleRank;
        } twoPair;

        struct {
            int threeRank;
            int bigSingleRank;
            int smallSingleRank;
        } three;

        struct {
            int topRank;
        } straight;

        struct {
            int ranks[SIZE_OF_FULLHAND];
        } flush;
        
        struct {
            int threeRank;
            int pairRank;
        } fullHouse;

        struct {
            int fourRank;
            int singleRank;
        } four;

        struct {
            int topRank;
            enum Suite suite;
        } straightFlush;
    };
    enum HandRank handRank;

    // Fixme: Member functions in a C style struct is ugly
    HandInfo();
    enum HandRank read(const Card*);
    void printRank() const;
};

enum
{
    P1_WIN = -1,
    DRAW   = 0,
    P2_WIN = 1,
};

int handCompare(const HandInfo&, const HandInfo&);

#endif
