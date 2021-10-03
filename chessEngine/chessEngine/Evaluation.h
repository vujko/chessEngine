#pragma once
class Evaluation
{
	class Board* board;
	const int pawnValue = 100;
	const int knightValue = 300;
	const int bishopValue = 320;
	const int rookValue = 500;
	const int queenValue = 900;

	void countMaterial(int& outWhiteMat, int& outBlackMat);
	int getPieceValue(int pieceType);

public:

	int evaluate(Board* b);
	
};

