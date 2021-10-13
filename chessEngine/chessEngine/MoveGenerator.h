#pragma once
#include <vector>
class MoveGenerator
{
public:
	std::vector<class Move> moves;
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

	std::vector<Move> generateMoves(Board& b);
	void initialize();
	void calculateAttackData();
	void generateSlidingAttackMap();
	void updateSlidingAttackPiece(int startSquare, int startDirIndex, int endDirIndex);
	void generateKingMoves();
	void generateSlidingMoves();
	void generateSlidingPieceMoves(int startSquare, int startDirIndex, int endDirIndex);
	void generateKnightMoves();
	void generatePawnMoves();
	void makePromotionMoves(int fromSquare, int toSquare);

private:

	bool hasKingsideCastleRight();
	bool hasQueensideCastleRight();
	bool squareIsAttacked(int square);
	bool squareIsInCheckRay(int square);
	bool isMovingAlongRay(int rayDir, int startSquare, int targetSquare);
	bool isPinned(int square);
	bool inCheckAfterEnPassant(int startSquare, int targetSquare, int epCapturedPawnSquare);
	bool squareAttackedAfterEPCapture(int epCaptureSquare, int capturingPawnStartSquare);
};

