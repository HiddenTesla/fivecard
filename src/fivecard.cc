#include "fivecard.hh"

#include <map>
#include <cstdio>
#include <cstring>
#include <algorithm>

bool cardCmp(const Card&, const Card&);
bool intCmp(int, int);
int findMostFreqRank(const std::map<int, int>&);

bool isFlush(const Card*);
bool isStraight(const Card*);
int getStraightTopRank(const Card*);


int rem(int d, int q)
{
    int r = d % q;
    if (r == 0)
        r = q;
    return r;
}

int sign(int n)
{
    if (n > 0) {
        return 1;
    }
    else if (n < 0) {
        return -1;
    }
    else {
        return 0;
    }
}

void Card::assign(int n)
{
    n = rem(n, SIZE_OF_DECK);
    suite = (Suite)(n / NUM_OF_NOMINALS);
    rank = rem(n, NUM_OF_NOMINALS);
    if (rank == NUM_OF_NOMINALS)
        suite = (Suite)((int)suite - 1);
    isSet = true;
}

void Card::print() const
{
    if (!isSet) {
        return;
    }
    switch (suite) {
    case CLUB:
        printf("Club    ");
        break;
    case DIAMOND:
        printf("Diamond ");
        break;
    case HEART:
        printf("Heart   ");
        break;
    case SPADE:
        printf("Spade   ");
        break;
    }

    switch (rank) {
    case 10:
        printf("J");
        break;
    case 11:
        printf("Q");
        break;
    case 12:
        printf("K");
        break;
    case 13:
        printf("A");
        break;
    default:
        printf("%d", rank + 1);
        break;
    }
    printf("\n");
}

bool Card::operator >(const Card& ref) const
{
    if (this->rank > ref.rank)
        return true;
    else if (this->rank < ref.rank)
        return false;
    else
        return (this->suite > ref.suite);
}

HandInfo::HandInfo()
{
    memset(this, 0, sizeof(*this));
}

void HandInfo::printRank() const
{
    printf("%s\n", getHandRankName(handRank));
}

enum HandRank HandInfo::read(const Card* handCards)
{
    // Have to make a copy, as quicksort needs to be performed on the array
    static Card copy[SIZE_OF_FULLHAND];
    static const size_t SIZE = sizeof(Card);
    memcpy(copy, handCards, SIZE * SIZE_OF_FULLHAND);
    std::sort(copy, copy + SIZE_OF_FULLHAND, cardCmp);

    typedef std::map<int, int> Counter;
    Counter rankStats;
    int cardRank;
    for (int i = 0; i < SIZE_OF_FULLHAND; ++i) {
        cardRank = copy[i].rank;
        if (rankStats.count(cardRank) <= 0) {
            rankStats[cardRank] = 1;
        }
        else {
            ++rankStats[cardRank];
        }
    }

    int mostFreqRank = findMostFreqRank(rankStats);
    switch (rankStats[mostFreqRank]) {
    case 4:
        this->handRank = FOUR_OF_A_KIND;
        rankStats.erase(mostFreqRank);
        this->four.fourRank = mostFreqRank;
        this->four.singleRank = rankStats.cbegin()->first;
        break;
    case 3: {
        rankStats.erase(mostFreqRank);
        int secondMFR = findMostFreqRank(rankStats);
        if (rankStats[secondMFR] == 2) {
            this->handRank = FULL_HOUSE;
            this->fullHouse.threeRank = mostFreqRank;
            this->fullHouse.pairRank = secondMFR;
        }
        else {
            this->handRank = THREE_OF_A_KIND;
            this->three.threeRank = mostFreqRank;
            Counter::const_iterator iter = rankStats.cbegin();
            int single1 = iter->first;
            ++iter;
            int single2 = iter->first;
            this->three.bigSingleRank = std::max(single1, single2);
            this->three.smallSingleRank = std::min(single1, single2);
        }
        break;
    }
    case 2: {
        rankStats.erase(mostFreqRank);
        int secondMFR = findMostFreqRank(rankStats);
        if (rankStats[secondMFR] == 2) {
            this->handRank = TWO_PAIR;
            this->twoPair.bigPairRank = std::max(mostFreqRank, secondMFR);
            this->twoPair.smallPairRank = std::min(mostFreqRank, secondMFR);
            this->twoPair.singleRank = rankStats.cbegin()->first;
        }
        else {
            this->handRank = ONE_PAIR;
            this->onePair.pairRank = mostFreqRank;
            Counter::const_iterator iter = rankStats.cbegin();
            int* const singles = onePair.singles;
            for (int i = 0; i < 3; ++i) {
                singles[i] = iter->first;
                ++iter;
            }
            std::sort(singles, singles + 3, intCmp);
            for (int i = 0; i < 3; ++i) {
            }
        }
        break;
    }
    case 1:
        break;
    default:
        printf("Impossible\n");
        return UNDEFINED_HANDRANK;
    }

    if (rankStats[mostFreqRank] == 1) {
        bool flush = isFlush(copy);
        bool straight = isStraight(copy);
        if (flush && straight) {
            if (copy[0].rank == RANK_10) {
                this->handRank = ROYAL_FLUSH;
            }
            else {
                this->handRank = STRAIGHT_FLUSH;
            }
            this->straightFlush.topRank = getStraightTopRank(copy);
        }
        else if (flush) {
            this->handRank = FLUSH;
            for (int i = 0; i < SIZE_OF_FULLHAND; ++i) {
                this->flush.ranks[i] = copy[i].rank;
            }
        }
        else if (straight) {
            this->handRank = STRAIGHT;
            this->straight.topRank = getStraightTopRank(copy);
        }
        else {
            this->handRank = NO_PAIR;
            for (int i = 0; i < SIZE_OF_FULLHAND; ++i) {
                this->noPair.ranks[i] = copy[i].rank;
            }
        }
    }

    return handRank;;
}

const char* getHandRankName(HandRank handRank)
{
    switch (handRank) {
    case NO_PAIR:
        return "No pair";
    case ONE_PAIR:
        return "One pair";
    case TWO_PAIR:
        return "Two pair";
    case THREE_OF_A_KIND:
        return "Three of a kind";
    case STRAIGHT:
        return "Staight";
    case FLUSH:
        return "Flush";
    case FULL_HOUSE:
        return "Full house";
    case FOUR_OF_A_KIND:
        return "Four of a kind";
    case STRAIGHT_FLUSH:
        return "Straight flush";
    case ROYAL_FLUSH:
        return "Royal flush";
    default:
        return "Undefined";
    }
}

int findMostFreqRank(const std::map<int, int>& map)
{
    std::map<int, int>::const_iterator max = map.cbegin();
    for (std::map<int, int>::const_iterator iter = map.cbegin();
        iter != map.cend(); ++iter)
    {
        //printf("iter %d vs max %d\n", iter->second, max->second);
        if (iter->second > max->second) {
            max = iter;
        }
    }
    return max->first;
}

bool cardCmp(const Card& c1, const Card& c2)
{
    return !(c1 > c2);
}

bool intCmp(int a, int b)
{
    return a < b;
}

bool isFlush(const Card* cards)
{
    for (int i = 1; i < SIZE_OF_FULLHAND; ++i) {
        if (cards[i].suite != cards[0].suite) {
            return false;
        }
    }
    return true;
}

bool isStraight(const Card* orderedCards)
{
    bool straight = true;
    for (int i = 1; i < SIZE_OF_FULLHAND; ++i) {
        if (orderedCards[i].rank != orderedCards[i-1].rank + 1) {
            straight = false;
            break;
        }
    }
    if (straight) {
        return true;
    }
    if (orderedCards[SIZE_OF_FULLHAND - 1].rank != RANK_A) {
        return false;
    }
    // Check if the ranks are (internal) 1, 2, 3, 4, 5
    for (int i = 0; i < SIZE_OF_FULLHAND - 1; ++i) {
        if (orderedCards[i].rank != i+1) {
            return false;
        }
    }
    return true;
}

// A, 2, 3, 4, 5: top is 5 not A
int getStraightTopRank(const Card* straightCards)
{
    if (straightCards[SIZE_OF_FULLHAND -1].rank == RANK_A) {
        if (straightCards[0].rank == RANK_2) {
            return RANK_5;
        }
        else {
            return RANK_A;
        }
    }
    else {
        return straightCards[SIZE_OF_FULLHAND - 1].rank;
    }
}

int handCompare(const HandInfo& p1, const HandInfo& p2)
{
    if (p1.handRank > p2.handRank) {
        return P1_WIN;
    }
    else if (p1.handRank < p2.handRank) {
        return P2_WIN;
    }

    switch (p1.handRank) {
    case NO_PAIR:
    case FLUSH: {
        const int* p1Ranks;
        const int* p2Ranks;
        if (p1.handRank == NO_PAIR) {
            p1Ranks = p1.noPair.ranks;
            p2Ranks = p2.noPair.ranks;
        }
        else {
            p1Ranks = p1.flush.ranks;
            p2Ranks = p2.flush.ranks;
        }
        for (int i = SIZE_OF_FULLHAND; i >= 0; --i) {
            if (p1Ranks[i] > p2Ranks[i]) {
                return P1_WIN;
            }
            else if (p1Ranks[i] < p2Ranks[i]) {
                return P2_WIN;
            }
        }
        return DRAW;
    }
    case ONE_PAIR: {
        if (p1.onePair.pairRank > p2.onePair.pairRank) {
            return P1_WIN;
        }
        if (p1.onePair.pairRank < p2.onePair.pairRank) {
            return P2_WIN;
        }
        return DRAW;
    }
    case TWO_PAIR:
        if (p1.twoPair.bigPairRank > p2.twoPair.bigPairRank) {
            return P1_WIN;
        }
        if (p1.twoPair.bigPairRank < p2.twoPair.bigPairRank) {
            return P2_WIN;
        }
        if (p1.twoPair.smallPairRank > p2.twoPair.smallPairRank) {
            return P1_WIN;
        }
        if (p1.twoPair.smallPairRank < p2.twoPair.smallPairRank) {
            return P2_WIN;
        }
        return DRAW;
    case THREE_OF_A_KIND:
        return sign(p2.three.threeRank - p1.three.threeRank);
    case STRAIGHT:
        return sign(p2.straight.topRank - p1.straight.topRank);
    case FULL_HOUSE: 
        return sign(p2.fullHouse.threeRank - p1.fullHouse.threeRank);
    case FOUR_OF_A_KIND:
        return sign(p2.four.fourRank - p1.four.fourRank);
    case STRAIGHT_FLUSH:
    case ROYAL_FLUSH:
        return sign(p2.straightFlush.topRank - p1.straightFlush.topRank);
    default:
        break;
    }
    return 0;
}
