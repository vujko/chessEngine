#pragma once
enum MoveFlag {
	none = 0,
	enPassantCapture = 1,
	castling = 2,
	promoteToQueen = 3,
	promoteToKnight = 4,
	promoteToRook = 5,
	promoteToBishop = 6,
	pawnTwoForward = 7
};
/*
To preserve memory, moves are stored as 16 bit numbers.
The format is as follows:

bit 0-5: from square (0 to 63)
bit 6-11: to square (0 to 63)
bit 12-15: flag
*/
class Move
{
private:

	const unsigned short startSquareMask = 0b0000000000111111;
	const unsigned short targetSquareMask = 0b0000111111000000;
	const unsigned short flagMask = 0b1111000000000000;

public:

	unsigned short moveValue;
	Move();
	Move(unsigned short moveValue);
	Move(int startI, int startJ, int targetI, int targetJ);
	Move(int startI, int startJ, int targetI, int targetJ, int moveFlag);
	int getStartSquare();
	int getTargetSquare();
	int getStartI();
	int getStartJ();
	int getTargetI();
	int getTargetJ();
	int getMoveFlag();
	bool isPromotion();
	friend bool operator==(const Move& lhs, const Move& rhs);
	friend bool operator!=(const Move& lhs, const Move& rhs);
	Move& operator=(const Move& other);
};

