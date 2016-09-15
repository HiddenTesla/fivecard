#include "fivecard.hh"

#include <cstdio>

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
