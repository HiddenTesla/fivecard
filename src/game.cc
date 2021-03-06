#include "game.hh"

#include <string>
#include <cstring>
#include <cstdio>
#include <sys/time.h>

int randomBetween(int lower, int upper);

Player::Player():
    mRoundBet(0),
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

void Player::dropAllCards()
{
    mNumCards = 0;
    for (int i = 0; i < SIZE_OF_FULLHAND; ++i) {
        mCards[i].isSet = false;
    }
}

int AIPlayer::askForBet()
{
    static const int AI_BET = 10;
    mBalance -= AI_BET;
    mRoundBet += AI_BET;
    return AI_BET;
}

bool isValidInteger(const char*);

int HumanPlayer::askForBet()
{
    static const int MAX_BET = 100;
    static const int MAX_BUFFER_SIZE = 1024;
    static char input[MAX_BUFFER_SIZE];
    int bet = 0;
    bool validBet = false;
    while (!validBet) {
        printf("How much do you want to bet (0 or negative to fold)? ");
        scanf("%s", input);

        // Seems wordy, but...
        // If you enter something other than a number, e.g. "ssss"
        // You will have wierd output!
        if (!isValidInteger(input)) {
            printf("Cannot convert '%s' into an integer. Try again\n", input);
            continue;
        }
        bet = atoi(input);

        // Allow bet exceed your balance (code removed)

        if (bet > MAX_BET) {
            printf("Bet at most $%d\n", MAX_BET);
            continue;
        }
        validBet = true;
    }
    // Bug fixed:
    // If you enter a negative number, though considered to fold,
    // your balance will still be "deducted" the negative bet (and thus increase)
    if (bet > 0) {
        printf("You bet $%d\n", bet);
        mBalance -= bet;
        mRoundBet += bet;
    }
    else {
        printf("You fold\n");
    }
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
    // Never use scanf("%c\n". ch)... which will lead to strange \n issues
    static const int MAX_BUFFER_SIZE = 1024;
    static char choice[MAX_BUFFER_SIZE];

    // In practice, easily to enter a number and judeged as fold
    // Hence change to:
    // Must enter 'y', 'Y', 'n', 'N'
    do {
        printf("Computer bets $%d. Follow (Y/N)? ", amount);
        scanf("%s", choice);
        char ch = choice[0];
        switch (ch) {
        case 'y': case 'Y':
            mBalance -= amount;
            mRoundBet += amount;
            return true;
        case 'n': case 'N':
            return false;
        default:
            printf("Must enter 'y' or 'n'\n");
            break;
        }
    }
    while (true);
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
    mDeck(SIZE_OF_DECK),
    mpBig(&mPlayer),
    mpSmall(&mComputer),
    mRounds(0)
{
    for (int i = 0; i < SIZE_OF_DECK; ++i) {
        mDeck[i] = i + 1;
    }
}

void Game::resetDeck()
{
    mDeck.resize(SIZE_OF_DECK);
    for (int i = 0; i < SIZE_OF_DECK; ++i) {
        mDeck[i] = i + 1;
    }
}

void Game::play()
{
    bool gameover = false;
    const Player* winner = NULL;
    // Fixme: do we really need the 'gameover' flag?
    while (!gameover) {
        round();
        if (mPlayer.mBalance <= 0) {
            printf("You've lost all your money\n");
            gameover = true;
            winner = &mComputer;
        }
        if (mComputer.mBalance <= 0) {
            printf("Computer's lost all its money\n");
            gameover = true;
            winner = &mPlayer;
        }
    }
    if (winner == &mPlayer) {
        printf("You win the game\n");
    }
    else {
        printf("You lose the game\n");
    }
}

void Game::round()
{
    ++mRounds;
    deal();
    deal();
    Player* roundWinner = NULL;
    Player* roundLoser = NULL;

    while (mPlayer.mNumCards <= SIZE_OF_FULLHAND) {
        // Todo: penalty if folds at the first bet or follow
        int bet = mpBig->askForBet();
        if (bet <= 0) {
            roundWinner = mpSmall;
            roundLoser = mpBig;
            break;
        }
        if (!mpSmall->askForFollow(bet)) {
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

    if (roundWinner != NULL) { // Folds before all cards are revealed
        roundWinner->mBalance += (roundWinner->mRoundBet + roundLoser->mRoundBet);
        //printf("In round %d, you bet %d, computer bet %d\n",
            //mRounds, mPlayer.mRoundBet, mComputer.mRoundBet);
        if (roundWinner == &mPlayer) {
            printf("You win $%d\n", mComputer.mRoundBet);
        }
        else {
            printf("You lose $%d\n", mPlayer.mRoundBet);
        }
        printBalance();
        mpBig = roundWinner;
        mpSmall = roundLoser;
    }
    else {
        struct HandInfo playerHandInfo, computerHandInfo;
        playerHandInfo.read(mPlayer.mCards);
        computerHandInfo.read(mComputer.mCards);
        printf("You are: ");
        playerHandInfo.printRank();
        printf("Computer is: ");
        computerHandInfo.printRank();
        switch (handCompare(playerHandInfo, computerHandInfo)) {
        case P1_WIN:
            roundWinner = &mPlayer;
            roundLoser = &mComputer;
            roundWinner->mBalance += roundWinner->mRoundBet * 2;
            printf("You win %d\n", roundWinner->mRoundBet);
            break;
        case P2_WIN:
            roundWinner = &mComputer;
            roundLoser = &mPlayer;
            roundWinner->mBalance += roundWinner->mRoundBet * 2;
            printf("You lose %d\n", roundWinner->mRoundBet);
            break;
        case DRAW:
            mPlayer.mBalance += mPlayer.mRoundBet;
            mComputer.mBalance += mComputer.mRoundBet;
        default:
            break;
        }
        printBalance();
    }
    resetRound();

    printf("\n\e[32;1m----------- ROUND %2d END -----------------\e[0m\n", mRounds);
}

void Game::deal()
{
    mpBig->drawCard(mDeck);
    mpSmall->drawCard(mDeck);
    printf("You draw:        \e[34;1m");
    mPlayer.lastCard()->print();
    printf("\e[0mComputer draw:           \e[31;1m");
    if (mComputer.mNumCards <= 1) {
        printf("First card hidden\n");
    }
    else {
        mComputer.lastCard()->print();
    }
    printf("\e[0m");
    if (*mPlayer.lastCard() > *mComputer.lastCard()) {
        mpBig = &mPlayer;
        mpSmall = &mComputer;
    }
    else {
        mpBig = &mComputer;
        mpSmall = &mPlayer;
    }
}

void Game::resetRound()
{
    resetDeck();
    mPlayer.dropAllCards();
    mComputer.dropAllCards();
    mPlayer.mRoundBet = 0;
    mComputer.mRoundBet = 0;
}

inline void Game::printBalance() const
{
    printf("You have      \e[33;1m$%d\n\e[0m", mPlayer.mBalance);
    printf("Computer have \e[33;1m$%d\n\e[0m", mComputer.mBalance);
}

int randomBetween(int lower, int upper)
{
    srand(time(NULL));
    int r = rand() % (upper - lower) + lower;
    return r;
}

bool isValidInteger(const char* input)
{
    if (input[0] == '-' || input[0] == '+') {
        return isValidInteger(input + 1);
    }

    int length = strlen(input);
    for (int i = 0; i < length; ++i) {
        if (input[i] > '9' || input[i] < '0') {
            return false;
        }
    }
    return true;
}
