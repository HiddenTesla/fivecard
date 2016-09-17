#include "fivecard.hh"
#include "game.hh"

#include <cstdio>
#include <fstream>

int main()
{
    #if 0
    Card p1Cards[SIZE_OF_FULLHAND];
    Card p2Cards[SIZE_OF_FULLHAND];
    std::ifstream infile("sample");
    if (infile.fail()) {
        return 1;
    }
    int cardNo;
    for (int i = 0; i < SIZE_OF_FULLHAND; ++i) {
        infile >> cardNo;
        p1Cards[i] = cardNo;
        p1Cards[i].print();
    }
    printf("-------------\n");
    for (int i = 0; i < SIZE_OF_FULLHAND; ++i) {
        infile >> cardNo;
        p2Cards[i] = cardNo;
        p2Cards[i].print();
    }
    infile.close();
    HandInfo p1HandInfo, p2HandInfo;
    p1HandInfo.read(p1Cards);
    p2HandInfo.read(p2Cards);
    switch (handCompare(p1HandInfo, p2HandInfo)) {
    case P1_WIN:
        printf("P1 wins\n");
        break;
    case P2_WIN:
        printf("P2 wins\n");
        break;
    default:
        printf("Draw\n");
        break;
    }
    return 0;
#endif
    Game game;
    game.play();
    return 0;
}
