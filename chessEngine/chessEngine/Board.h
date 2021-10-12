#pragma once
#include <iostream>
#include "Piece.h"
#include <unordered_map>
#include <stack>
#include <random>
#include "PieceList.h"
class Board
{
public:

	const int whiteIndex = 0;
	const int blackIndex = 1;

	int* squares;
	bool whiteToMove;
	int colorToMove;
	int opponentColor;
	int colorToMoveIndex;

	// Bits 0-3 store white and black kingside/queenside castling legality
	// Bits 4-7 store file of ep square (starting at 1, so 0 = no ep square)
	// Bits 8-13 captured piece
	// Bits 14-... fifty mover counter
	unsigned int currentGameState;
	std::stack<unsigned int> gameStateHistory;

	int plyCount;
	//num ply since last pawn move or capture
	int fiftyMoveCounter;

	int* kingSquare;
	PieceList* rooks;
	PieceList* bishops;
	PieceList* queens;
	PieceList* knights;
	PieceList* pawns;

	PieceList* allPieceLists;

	const unsigned int whiteCastleKingsideMask = 0b1111111111111110;
	const unsigned int whiteCastleQueensideMask = 0b1111111111111101;
	const unsigned int blackCastleKingsideMask = 0b1111111111111011;
	const unsigned int blackCastleQueensideMask = 0b1111111111110111;

	const unsigned int whiteCastleMask = whiteCastleKingsideMask & whiteCastleQueensideMask;
	const unsigned int blackCastleMask = blackCastleKingsideMask & blackCastleQueensideMask;

	PieceList getPieceList(int pieceType, int colorIndex);
	void makeMove(class Move& m);
	void unmakeMove(Move& m);
	void loadPosiiton(std::string fen);
	void initialize();
};

