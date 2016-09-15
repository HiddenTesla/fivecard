#ifndef _GAME_HH_
#define _GAME_HH_

#include "fivecard.hh"

#include <vector>

class Game;

class Player
{
    friend class Game;
protected:
    static const int INITIAL_BALANCE = 1000;
    
    int mRoundBet;
    int mBalance;
    int mNumCards;
    Card mCards[SIZE_OF_FULLHAND]; 

public:
    Player();
    virtual ~Player() = default;

    virtual int askForBet() = 0;
    virtual bool askForFollow(int amount) = 0;
    virtual void print() const = 0;
    
    int drawCard(std::vector<int>&);
    const Card* lastCard() const;
};

class AIPlayer: public Player
{
public:
    int askForBet();
    bool askForFollow(int amount);
    void print() const;
};

class HumanPlayer: public Player
{
public:
    int askForBet();
    bool askForFollow(int amount);
    void print() const;
};


class Game
{
    friend int main2();
    friend int main2(int, char**);
private:
    AIPlayer mComputer;
    HumanPlayer mPlayer;

    Player* mpBig;
    Player* mpSmall;

    std::vector<int> mDeck;

    void deal();

public:
    Game();

    void round();
    void printPlayerCards() const {mPlayer.print(); mComputer.print();}
    void printBalance() const;
};


#endif
