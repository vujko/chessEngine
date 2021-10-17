#include "Evaluation.h"
#include "Board.h"
#include "PieceList.h"


const std::vector<int> Evaluation::pawnsPos = {
			0,  0,  0,  0,  0,  0,  0,  0,
			50, 50, 50, 50, 50, 50, 50, 50,
			10, 10, 20, 30, 30, 20, 10, 10,
			5,  5, 10, 25, 25, 10,  5,  5,
			0,  0,  0, 20, 20,  0,  0,  0,
			5, -5,-10,  0,  0,-10, -5,  5,
			5, 10, 10,-20,-20, 10, 10,  5,
			0,  0,  0,  0,  0,  0,  0,  0
};

const std::vector<int> Evaluation::knightsPos = {
			-50,-40,-30,-30,-30,-30,-40,-50,
			-40,-20,  0,  0,  0,  0,-20,-40,
			-30,  0, 10, 15, 15, 10,  0,-30,
			-30,  5, 15, 20, 20, 15,  5,-30,
			-30,  0, 15, 20, 20, 15,  0,-30,
			-30,  5, 10, 15, 15, 10,  5,-30,
			-40,-20,  0,  5,  5,  0,-20,-40,
			-50,-40,-30,-30,-30,-30,-40,-50,
};

const std::vector<int> Evaluation::bishopsPos = {
			-20,-10,-10,-10,-10,-10,-10,-20,
			-10,  0,  0,  0,  0,  0,  0,-10,
			-10,  0,  5, 10, 10,  5,  0,-10,
			-10,  5,  5, 10, 10,  5,  5,-10,
			-10,  0, 10, 10, 10, 10,  0,-10,
			-10, 10, 10, 10, 10, 10, 10,-10,
			-10,  5,  0,  0,  0,  0,  5,-10,
			-20,-10,-10,-10,-10,-10,-10,-20,
};

const std::vector<int> Evaluation::rooksPos = {
			0,  0,  0,  0,  0,  0,  0,  0,
			5, 10, 10, 10, 10, 10, 10,  5,
			-5,  0,  0,  0,  0,  0,  0, -5,
			-5,  0,  0,  0,  0,  0,  0, -5,
			-5,  0,  0,  0,  0,  0,  0, -5,
			-5,  0,  0,  0,  0,  0,  0, -5,
			-5,  0,  0,  0,  0,  0,  0, -5,
			0,  0,  0,  5,  5,  0,  0,  0
};

const std::vector<int> Evaluation::queensPos = {
			-20,-10,-10, -5, -5,-10,-10,-20,
			-10,  0,  0,  0,  0,  0,  0,-10,
			-10,  0,  5,  5,  5,  5,  0,-10,
			-5,  0,  5,  5,  5,  5,  0, -5,
			0,  0,  5,  5,  5,  5,  0, -5,
			-10,  5,  5,  5,  5,  5,  0,-10,
			-10,  0,  5,  0,  0,  0,  0,-10,
			-20,-10,-10, -5, -5,-10,-10,-20
};

const std::vector<int> Evaluation::kingEarlyPos = {
			-30,-40,-40,-50,-50,-40,-40,-30,
			-30,-40,-40,-50,-50,-40,-40,-30,
			-30,-40,-40,-50,-50,-40,-40,-30,
			-30,-40,-40,-50,-50,-40,-40,-30,
			-20,-30,-30,-40,-40,-30,-30,-20,
			-10,-20,-20,-20,-20,-20,-20,-10,
			20, 20,  0,  0,  0,  0, 20, 20,
			20, 30, 10,  0,  0, 10, 30, 20
};

const std::vector<int> Evaluation::kingEndPos = {
			-50,-40,-30,-20,-20,-30,-40,-50,
			-30,-20,-10,  0,  0,-10,-20,-30,
			-30,-10, 20, 30, 30, 20,-10,-30,
			-30,-10, 30, 40, 40, 30,-10,-30,
			-30,-10, 30, 40, 40, 30,-10,-30,
			-30,-10, 20, 30, 30, 20,-10,-30,
			-30,-30,  0,  0,  0,  0,-30,-30,
			-50,-30,-30,-30,-30,-30,-30,-50
};

Board* Evaluation::board = new Board();

int Evaluation::whiteEval = 0;
int Evaluation::blackEval = 0;

int Evaluation::countMaterial(int colorIndex)
{
	int material = 0;
	material += board->pawns[colorIndex]->numPieces * pawnValue;
	material += board->knights[colorIndex]->numPieces * knightValue;
	material += board->bishops[colorIndex]->numPieces * bishopValue;
	material += board->rooks[colorIndex]->numPieces * rookValue;
	material += board->queens[colorIndex]->numPieces * queenValue;

	return material;
}


float Evaluation::getEndGamePhaseWeight(const int materialWithoutPawns)
{
	float multiplier = 1 / endGameMaterialStart;
	return 1 - std::min(1.f, materialWithoutPawns * multiplier);

}

int Evaluation::evaluatePieceSquareTables(int colorIndex, float endgamePhaseWeight)
{
	int value = 0;
	bool isWhite = colorIndex == Board::whiteIndex;
	value += evaluatePieceSquareTable(pawnsPos, board->pawns[colorIndex], isWhite);
	value += evaluatePieceSquareTable(rooksPos, board->rooks[colorIndex], isWhite);
	value += evaluatePieceSquareTable(knightsPos, board->knights[colorIndex], isWhite);
	value += evaluatePieceSquareTable(bishopsPos, board->bishops[colorIndex], isWhite);
	value += evaluatePieceSquareTable(queensPos, board->queens[colorIndex], isWhite);
	int kingEarlyPhase = readTable(kingEarlyPos, board->kingSquare[colorIndex], isWhite);

	value += (int)(kingEarlyPhase * (1 - endgamePhaseWeight));
	//value += PieceSquareTable.Read (PieceSquareTable.kingMiddle, board.KingSquare[colourIndex], isWhite);

	return value;
}

int Evaluation::evaluatePieceSquareTable(std::vector<int> table, PieceList* pieceList, bool isWhite)
{
	int value = 0;
	for (int i = 0; i < pieceList->numPieces; i++) {
		value += readTable(table, (*pieceList)[i], isWhite);
	}
	return value;
}

int Evaluation::readTable(std::vector<int> table, int square, bool isWhite)
{
	if (isWhite) {
		int file = square & 0b000111;
		int rank = square >> 3;
		rank = 7 - rank;
		square = rank * 8 + file;
	}
	return table[square];
}


int Evaluation::evaluate(Board* b)
{
	board = b;
	whiteEval = 0, blackEval = 0;

	int whiteMaterial = countMaterial(Board::whiteIndex);
	int blackMaterial = countMaterial(Board::blackIndex);

	int whiteMaterialWithoutPawns = whiteMaterial - board->pawns[Board::whiteIndex]->numPieces * pawnValue;
	int blackMaterialWithoutPawns = blackMaterial - board->pawns[Board::blackIndex]->numPieces * pawnValue;
	float whiteEndgamePhaseWeight = getEndGamePhaseWeight(whiteMaterialWithoutPawns);
	float blackEndgamePhaseWeight = getEndGamePhaseWeight(blackMaterialWithoutPawns);

	whiteEval += whiteMaterial;
	blackEval += blackMaterial;
	//whiteEval += MopUpEval(Board::whiteIndex, Board::blackIndex, whiteMaterial, blackMaterial, blackEndgamePhaseWeight);
	//blackEval += MopUpEval(Board::blackIndex, Board::whiteIndex, blackMaterial, whiteMaterial, whiteEndgamePhaseWeight);

	whiteEval += evaluatePieceSquareTables(Board::whiteIndex, blackEndgamePhaseWeight);
	blackEval += evaluatePieceSquareTables(Board::blackIndex, whiteEndgamePhaseWeight);

	int eval = whiteEval - blackEval;

	//mozda treba obrnuto zavisi kada se poziva funk
	int perspective = (board->whiteToMove) ? 1 : -1;
	return eval * perspective;
}
