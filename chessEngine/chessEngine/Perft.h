#pragma once
#include "Move.h"
#include <string>

void runTest1();
void runTest2();
int search(int depth);
int search_copy(class Board* b, int depth);
int searchDivide(int startDepth, int currentDepth);
void comparePerftDivideResults(std::string fen);
std::string moveName(Move& m);