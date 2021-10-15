#include "BitBoardUtility.h"

bool BitBoardUtility::containsSquare(unsigned long long bitBoard, int square)
{
    return ((bitBoard >> square) & 1) != 0;
}
