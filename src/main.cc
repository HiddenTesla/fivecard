#include "fivecard.hh"
#include "game.hh"

#include <cstdio>
#include <fstream>

int main()
{
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
    }
    infile.close();
    HandInfo handInfo;
    handInfo.read(p1Cards);
    return 0;
    Game game;
    game.play();
    return 0;
}
