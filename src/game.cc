#include "game.hh"

#include <string>
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
    mBalance -= AI_BET;
    mRoundBet += AI_BET;
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
        //if (bet > mBalance) {
        // Allow bet exceed your balance
        if (0) {
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
    mBalance -= bet;
    mRoundBet += bet;
    return bet;
}

bool AIPlayer::askForFollow(int amount)
{
    printf("Computer follows your bet\n");
    mBalance -= amount;
    mRoundBet += amount;
    return true;
}

bool HumanPlayer::askForFollow(int amount)
{
    printf("Computer bets $%d. Follow (Y/N)? ", amount);
    // Never use scanf("%c\n". ch)... which will lead to strange \n issues
    static const int MAX_BUFFER_SIZE = 1024;
    static char choice[MAX_BUFFER_SIZE];
    scanf("%s", choice);
    char ch = choice[0];
    if (ch == 'y' || ch == 'Y') {
        mBalance -= amount;
        mRoundBet += amount;
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
    printf("Computer's cards:\n");
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
    printf("Your cards: \n");
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
    mPlayer.mRoundBet = 0;
    mComputer.mRoundBet = 0;
    deal();
    deal();
    this->printPlayerCards();
    Player* roundWinner = NULL;
    Player* roundLoser = NULL;

    while (mPlayer.mNumCards <= SIZE_OF_FULLHAND) {
        // Todo: penalty if folds at the first bet or follow
        int bet = mpBig->askForBet();
        if (bet <= 0) {
            printf("Fold\n");
            roundWinner = mpSmall;
            roundLoser = mpBig;
            break;
        }
        if (!mpSmall->askForFollow(bet)) {
            printf("Fold\n");
            roundLoser = mpSmall;
            roundWinner = mpBig;
            break;
        }

        if (mPlayer.mNumCards == SIZE_OF_FULLHAND) {
            break; // Already drawn 5 cards. Should ask for bet, but no more draw
        }
        deal();

    } // while

    this->printPlayerCards();
    printf("%d cards left in deck\n", (int)mDeck.size());

    if (roundWinner != NULL) { // Folds before all cards are revealed
        roundWinner->mBalance += (roundWinner->mRoundBet + roundLoser->mRoundBet);
        if (roundWinner == &mPlayer) {
            printf("You win $%d\n", mComputer.mRoundBet);
        }
        else {
            printf("You lose $%d\n", mPlayer.mRoundBet);
        }
        printBalance();
    }

    // No one folds. Todo: compare 5 cards
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

inline void Game::printBalance() const
{
    printf("You have      $%d\n", mPlayer.mBalance);
    printf("Computer have $%d\n", mComputer.mBalance);
}

int randomBetween(int lower, int upper)
{
    srand(time(NULL));
    int r = rand() % (upper - lower) + lower;
    return r;
}
