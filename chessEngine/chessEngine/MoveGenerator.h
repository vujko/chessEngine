#pragma once
#include <vector>
static class MoveGenerator
{
public:
	static std::vector<class Move> moves;
	static bool isWhiteToMove;
	static int friendlyColour;
	static int opponentColour;
	static int friendlyKingSquare;
	static int friendlyColourIndex;
	static int opponentColourIndex;

	static bool inCheck;
	static bool inDoubleCheck;
	static bool pinsExistInPosition;
	static unsigned long long checkRayBitmask;
	static unsigned long long pinRayBitmask;
	static unsigned long long opponentKnightAttacks;
	static unsigned long long opponentAttackMapNoPawns;
	static unsigned long long opponentAttackMap;
	static unsigned long long opponentPawnAttackMap;
	static unsigned long long opponentSlidingAttackMap;

	static class Board* board;

	friend class constructor;

	struct constructor {
		constructor() {
			//initialize();
		}
	};

	static constructor cons;

	static std::vector<Move> generateMoves(Board& b);
	static void initialize();
	static void calculateAttackData();
	static void generateSlidingAttackMap();
	static void updateSlidingAttackPiece(int startSquare, int startDirIndex, int endDirIndex);
	static void generateKingMoves();
	static void generateSlidingMoves();
	static void generateSlidingPieceMoves(int startSquare, int startDirIndex, int endDirIndex);
	static void generateKnightMoves();
	static void generatePawnMoves();
	static void makePromotionMoves(int fromSquare, int toSquare);

private:

	static bool hasKingsideCastleRight();
	static bool hasQueensideCastleRight();
	static bool squareIsAttacked(int square);
	static bool squareIsInCheckRay(int square);
	static bool isMovingAlongRay(int rayDir, int startSquare, int targetSquare);
	static bool isPinned(int square);
	static bool inCheckAfterEnPassant(int startSquare, int targetSquare, int epCapturedPawnSquare);
	static bool squareAttackedAfterEPCapture(int epCaptureSquare, int capturingPawnStartSquare);
};

