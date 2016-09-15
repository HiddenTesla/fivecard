#include "game.hh"

#include <cstdio>
#include <sys/time.h>

int randomBetween(int lower, int upper);

Player::Player():
    mBalance(INITIAL_BALANCE),
    mNumCards(0)
    {
    }

int Player::drawCard(std::vector<int>& deck)
{
    if (mNumCards >= SIZE_OF_FULLHAND) {
        printf("%s: Already has %d handcards\n", __func__, mNumCards);
        return -1;
    }
    if (deck.size() <= 0) {
        printf("%s: Deck is empty\n", __func__);
        return -1;
    }
    // Always draw card from the end of the deck
    // Todo: randomly draw a card
    std::vector<int>::const_iterator i = deck.cbegin();
    i += randomBetween(0, deck.size());
    int cardNo = *i;
    deck.erase(i);
    mCards[mNumCards] = cardNo;
    ++mNumCards;
    return cardNo;
};

const Card* Player::lastCard() const
{
    if (mNumCards <= 0) {
        return NULL;
    }
    return &mCards[mNumCards - 1];
}

int AIPlayer::askForBet()
{
    static const int AI_BET = 10;
    mBalance -= 10;
    return AI_BET;
}

int HumanPlayer::askForBet()
{
    static const int MAX_BET = 100;
    int bet;
    bool validBet = false;
    while (!validBet) {
        printf("How much do you want to bet (0 or negative to fold)? ");
        scanf("%d", &bet);
        if (bet > mBalance) {
            printf("Bet $%d greater than your balance $%d. Try again\n",
                bet, mBalance);
            continue;
        }
        if (bet > MAX_BET) {
            printf("Bet at most $%d\n", MAX_BET);
            continue;
        }
        validBet = true;
    }
    printf("You bet $%d\n", bet);
    mBalance -= 10;
    return bet;
}

bool AIPlayer::askForFollow(int amount)
{
    printf("Computer follows your bet\n");
    mBalance -= amount;
    return true;
}

bool HumanPlayer::askForFollow(int amount)
{
    printf("Computer bets $%d. Follow (Y/N)? ", amount);
    char ch;
    scanf("%c", &ch);
    if (ch == 'y' || ch == 'Y') {
        mBalance -= amount;
        return true;
    }
    else {
        return false;
    }
}


void AIPlayer::print() const
{
    if (mNumCards < 2) {
        return;
    }
    printf("Computer has:\n");
    // Opponent's first card is hidden
    printf("Hidden\n");
    for (int i = 0; i < mNumCards; ++i) {
        mCards[i].print();
    }
}

void HumanPlayer::print() const
{
    if (mNumCards < 2) {
        return;
    }
    printf("You have: \n");
    for (int i = 0; i < mNumCards; ++i) {
        mCards[i].print();
    }
}

Game::Game():
    mpBig(&mPlayer),
    mpSmall(&mComputer),
    mDeck(SIZE_OF_DECK)
{
    for (int i = 0; i < SIZE_OF_DECK; ++i) {
        mDeck[i] = i + 1;
    }
}

void Game::round()
{
    deal();
    deal();
    this->print();

    while (mPlayer.mNumCards <= SIZE_OF_FULLHAND) {
        int bet = mpBig->askForBet();
        if (bet <= 0) {
            printf("Fold\n");
            break;
        }
        if (!mpSmall->askForFollow(bet)) {
            printf("Fold\n");
            break;
        }

        if (mPlayer.mNumCards == SIZE_OF_FULLHAND) {
            break; // Already drawn 5 cards. Should ask for bet, but no more draw
        }
        deal();

    } // while

    this->print();
    printf("%d cards left in deck\n", (int)mDeck.size());
}

void Game::deal()
{
    mpBig->drawCard(mDeck);
    mpSmall->drawCard(mDeck);
    printf("You draw:        ");
    mPlayer.lastCard()->print();
    printf("Computer draw:           ");
    if (mComputer.mNumCards <= 1) {
        printf("First card hidden\n");
    }
    else {
        mComputer.lastCard()->print();
    }
    if (*mPlayer.lastCard() > *mComputer.lastCard()) {
        mpBig = &mPlayer;
        mpSmall = &mComputer;
    }
    else {
        mpBig = &mComputer;
        mpSmall = &mPlayer;
    }
}

int randomBetween(int lower, int upper)
{
    srand(time(NULL));
    int r = rand() % (upper - lower) + lower;
    return r;
}
