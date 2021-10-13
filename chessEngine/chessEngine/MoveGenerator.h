#pragma once
#include <vector>
class MoveGenerator
{
public:
	std::vector<class Move&> moves;
	bool isWhiteToMove;
	int friendlyColour;
	int opponentColour;
	int friendlyKingSquare;
	int friendlyColourIndex;
	int opponentColourIndex;

	bool inCheck;
	bool inDoubleCheck;
	bool pinsExistInPosition;
	unsigned long checkRayBitmask;
	unsigned long pinRayBitmask;
	unsigned long opponentKnightAttacks;
	unsigned long opponentAttackMapNoPawns;
	unsigned long opponentAttackMap;
	unsigned long opponentPawnAttackMap;
	unsigned long opponentSlidingAttackMap;

	class Board& board;

	std::vector<Move&> generateMoves(Board& b);
	void initialize();
	void calculateAttackData();
	void generateSlidingAttackMap();
	void updateSlidingAttackPiece(int startSquare, int startDirIndex, int endDirIndex);
};

