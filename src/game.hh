#ifndef _GAME_HH_
#define _GAME_HH_

#include "fivecard.hh"

#include <vector>

class Game;

class Player
{
    friend class Game;
protected:
    static const int INITIAL_BALANCE = 200;
    
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
    void dropAllCards();
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
private:
    AIPlayer mComputer;
    HumanPlayer mPlayer;

    std::vector<int> mDeck;

    Player* mpBig;
    Player* mpSmall;

    int mRounds;

    void deal();
    void resetDeck();
    void resetRound();

public:
    Game();

    void round();
    void play();

    void printPlayerCards() const {mPlayer.print(); mComputer.print();}
    void printBalance() const;
};


#endif
