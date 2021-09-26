#pragma once
#include "Move.h"
#include <string>

void runTest();
int searchDivide(class Board* b, int startDepth, int currentDepth);
void comparePerftDivideResults(std::string fen);
std::string moveName(Move& m);