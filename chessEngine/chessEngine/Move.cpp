#include "Move.h"

Move::Move()
{
}

Move::Move(unsigned short moveValue) : moveValue(moveValue)
{
}

Move::Move(int startI, int startJ, int targetI, int targetJ)
{
	int startSquare = startI * 8 + startJ;
	int targetSquare = targetI * 8 + targetJ;
	moveValue = (unsigned short)(startSquare | targetSquare << 6);
}

Move::Move(int startI, int startJ, int targetI, int targetJ, int moveFlag)
{
	int startSquare = startI * 8 + startJ;
	int targetSquare = targetI * 8 + targetJ;
	moveValue = (unsigned short)(startSquare | targetSquare << 6 | moveFlag << 12);
}

Move::Move(int startSquare, int targetSquare)
{
	moveValue = (startSquare | targetSquare << 6);
}

Move::Move(int startSquare, int targetSquare, int flag)
{
	moveValue = (startSquare | targetSquare << 6 | flag << 12);
}

int Move::getStartSquare()
{
	return moveValue & startSquareMask;
}

int Move::getTargetSquare()
{
	return (moveValue & targetSquareMask) >> 6;
}

int Move::getStartI() 
{
	int startSquare = moveValue & startSquareMask;
	return startSquare / 8;
}

int Move::getStartJ()
{
	int startSquare = moveValue & startSquareMask;
	return startSquare % 8;
}

int Move::getTargetI()
{
	int targetSquare = (moveValue & targetSquareMask) >> 6;
	return targetSquare / 8;
}

int Move::getTargetJ()
{
	int targetSquare = (moveValue & targetSquareMask) >> 6;
	return targetSquare % 8;

}

int Move::getMoveFlag()
{
	return moveValue >> 12;
}

bool Move::isPromotion()
{
	int flag = getMoveFlag();
	return (flag == promoteToBishop || flag == promoteToKnight || flag == promoteToQueen || flag == promoteToRook);
}

Move& Move::operator=(const Move& other)
{
	moveValue = other.moveValue;
	return *this;
}

bool operator==(const Move& lhs, const Move& rhs)
{
	return lhs.moveValue == rhs.moveValue;
}

bool operator!=(const Move& lhs, const Move& rhs)
{
	return !(lhs == rhs);
}
