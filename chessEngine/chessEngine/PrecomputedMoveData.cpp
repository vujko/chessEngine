#include "PrecomputedMoveData.h"

const int* PrecomputedMoveData::directionOffsets = new int[8]{ 8, -8, -1, 1, 7, -7, 9, -9 };

const std::array<std::array<std::byte, 2>, 2> PrecomputedMoveData::pawnAttackDirections = std::array<std::array<std::byte, 2>, 2>{
	 std::array{
		 std::byte{4}, std::byte{6}
	 }, 
	 std::array{
		 std::byte{7},std::byte{5}
	 } 
 };


// Stores number of moves available in each of the 8 directions for every square on the board
// Order of directions is: N, S, W, E, NW, SE, NE, SW
// So for example, if availableSquares[0][1] == 7...
// that means that there are 7 squares to the north of b1 (the square with index 1 in board array)
std::vector<std::vector<int>> PrecomputedMoveData::numSquaresToEdge;

// Stores array of indices for each square a knight can land on from any square on the board
// So for example, knightMoves[0] is equal to {10, 17}, meaning a knight on a1 can jump to c2 and b3
std::vector<std::vector<std::byte>> PrecomputedMoveData::knightMoves;
std::vector<std::vector<std::byte>> PrecomputedMoveData::kingMoves;


std::vector<std::vector<int>> PrecomputedMoveData::pawnAttacksWhite;
std::vector<std::vector<int>> PrecomputedMoveData::pawnAttacksBlack;
std::vector<int> PrecomputedMoveData::directionLookup;

std::vector<unsigned long long> PrecomputedMoveData::kingAttackBitBoards;
std::vector<unsigned long long> PrecomputedMoveData::knightAttackBitBoards;
std::vector<std::vector<unsigned long long>> PrecomputedMoveData::pawnAttackBitBoards;

std::vector<unsigned long long> PrecomputedMoveData::rookMoves;
std::vector<unsigned long long> PrecomputedMoveData::bishopMoves;
std::vector<unsigned long long> PrecomputedMoveData::queenMoves;

std::vector<std::vector<int>> PrecomputedMoveData::orthogonalDistance;
std::vector<std::vector<int>> PrecomputedMoveData::kingDistance;
std::vector<int> PrecomputedMoveData::centreManhattanDistance;

PrecomputedMoveData::constructor PrecomputedMoveData::cons;
