#include "fivecard.hh"

#include <map>
#include <cstdio>
#include <cstring>
#include <algorithm>

bool cardCmp(const Card&, const Card&);
int findMostFreqRank(const std::map<int, int>&);

int rem(int d, int q)
{
    int r = d % q;
    if (r == 0)
        r = q;
    return r;
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

/*
    for (Counter::const_iterator iter = rankStats.cbegin();
        iter != rankStats.cend(); ++iter)
    {
        printf("Card (internal) rank %d appers %d times\n",
            iter->first, iter->second);
    }
    */
    int mostFreqRank = findMostFreqRank(rankStats);
    switch (rankStats[mostFreqRank]) {
    case 4:
        this->handRank = FOUR_OF_A_KIND;
        rankStats.erase(mostFreqRank);
        this->four.fourRank = mostFreqRank;
        this->four.singleRank = rankStats.cbegin()->first;
        printf("Four %d with one %d\n", four.fourRank, four.singleRank);
        break;
    case 3: {
        rankStats.erase(mostFreqRank);
        int secondMFR = findMostFreqRank(rankStats);
        if (rankStats[secondMFR] == 2) {
            this->handRank = FULL_HOUSE;
            this->fullHouse.threeRank = mostFreqRank;
            this->fullHouse.pairRank = secondMFR;
            printf("Three %d with pair of %d\n", fullHouse.threeRank, fullHouse.pairRank);
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
            printf("Three %d with big single %d small single %d\n",
                three.threeRank, three.bigSingleRank, three.smallSingleRank);
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
            printf("Two pairs. Big %d small %d single %d\n",
                twoPair.bigPairRank, twoPair.smallPairRank, twoPair.singleRank);
        }
        else {
            this->handRank = ONE_PAIR;
            this->onePair.pairRank = mostFreqRank;
            // Todo: sort the other 3
            printf("One pair %d\n", onePair.pairRank);
        }
        break;
    }
    default:
        printf("Impossible\n");
        return UNDEFINED_HANDRANK;
    }

    printRank();
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


HandRank getHandRank(const Card* handCards)
{
    // Have to make a copy, as quicksort needs to be performed on the array
    static Card copy[SIZE_OF_FULLHAND];
    static const size_t SIZE = sizeof(Card);
    memcpy(copy, handCards, SIZE * SIZE_OF_FULLHAND);
    printf("Unsorted cards:\n");
    for (int i = 0; i < SIZE_OF_FULLHAND; ++i) {
        copy[i].print();
    }
    std::sort(copy, copy + SIZE_OF_FULLHAND, cardCmp);

    printf("Sorted cards:\n");
    for (int i = 0; i < SIZE_OF_FULLHAND; ++i) {
        copy[i].print();
    }

    return UNDEFINED_HANDRANK;
}
