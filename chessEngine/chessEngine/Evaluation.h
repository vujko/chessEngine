#pragma once
#include <vector>
static class Evaluation
{
	//favorable positions tables
	static const std::vector<int> pawnsPos;

	static const std::vector<int> knightsPos;

	static const std::vector<int> bishopsPos;

	static const std::vector<int> rooksPos;

	static const std::vector<int> queensPos;

	static const std::vector<int> kingEarlyPos;

	static const std::vector<int> kingEndPos;

	static class Board* board;
	static const int pawnValue = 100;
	static const int knightValue = 300;
	static const int bishopValue = 320;
	static const int rookValue = 500;
	static const int queenValue = 900;
	static const int endGameMaterialStart = rookValue * 2 + bishopValue + knightValue;


	static int whiteEval;
	static int blackEval;

	static int countMaterial(int colorIndex);
	static float getEndGamePhaseWeight(const int materialWithoutPawns);
	static int evaluatePieceSquareTables(int colorIndex, float endgamePhaseWeight);
	static int evaluatePieceSquareTable(std::vector<int> table, class PieceList* pieceList, bool isWhite);
	static int readTable(std::vector<int> table, int square, bool isWhite);

public:

	static int evaluate(Board* b);
	
};

