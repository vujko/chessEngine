#pragma once
#include <iostream>
#include "Piece.h"
#include <unordered_map>
#include <stack>
#include "PieceList.h"
#include <vector>
class Board
{
public:

	const static int whiteIndex = 0;
	const static int blackIndex = 1;

	std::vector<int> squares;
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

	std::vector<int> kingSquare;
	std::vector<PieceList*> rooks;
	std::vector<PieceList*> bishops;
	std::vector<PieceList*> queens;
	std::vector<PieceList*> knights;
	std::vector<PieceList*> pawns;

	std::vector<PieceList*> allPieceLists;

	const unsigned int whiteCastleKingsideMask = 0b1111111111111110;
	const unsigned int whiteCastleQueensideMask = 0b1111111111111101;
	const unsigned int blackCastleKingsideMask = 0b1111111111111011;
	const unsigned int blackCastleQueensideMask = 0b1111111111110111;

	const unsigned int whiteCastleMask = whiteCastleKingsideMask & whiteCastleQueensideMask;
	const unsigned int blackCastleMask = blackCastleKingsideMask & blackCastleQueensideMask;

	std::vector<class Move> moves;

	Board();
	//Board(const Board& other);
	PieceList* getPieceList(int pieceType, int colorIndex);
	void makeMove(class Move& m);
	void unmakeMove(Move& m);
	void loadPosition(std::string fen);
	void initialize();
	void initStartPosition();
	void handleEnPassant(int moveTo, int opponentColorIndex);
	void handleCastling(int moveTo);
	Board& operator=(const Board& other);
};

