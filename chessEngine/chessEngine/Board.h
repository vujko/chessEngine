#pragma once
#include <iostream>
#include "Piece.h"
#include <unordered_map>
#include <stack>
enum Direction {
	
	NORTH, 
	WEST,
	EAST,
	SOUTH,
	NORTH_WEST,
	NORTH_EAST,
	SOUTH_WEST,
	SOUTH_EAST,
	NONE
};

class Board
{
public:

	int squares[8][8] = { 0 };
	bool whiteToMove;
	int colorToMove;
	int opponentColor;
	int playCount;
	int halfMoveCount;
	bool whiteCastleKingside;
	bool whiteCastleQueenside;
	bool blackCastleKingside;
	bool blackCastleQueenside;
	int epFile;

	bool whiteKingCheck;
	bool blackKingCheck;

	// Bits 0-3 store white and black kingside/queenside castling legality
	// Bits 4-7 store file of ep square (starting at 1, so 0 = no ep square)
	// Bits 8-13 captured piece type
	// Bits 14-15 whiteKingCheck, blackKingCheck
	// Bits 16-... fifty mover counter
	unsigned int currentGameState;
	std::stack<unsigned int> gameStateHistory;

	const unsigned int whiteCastleKingsideMask = 0b1111111111111110;
	const unsigned int whiteCastleQueensideMask = 0b1111111111111101;
	const unsigned int blackCastleKingsideMask = 0b1111111111111011;
	const unsigned int blackCastleQueensideMask = 0b1111111111110111;

	//bit 0 - 5: white king square(0 to 63)
	//bit 6 - 11 : black king square(0 to 63)
	unsigned short kingPositions;

	const unsigned short whiteKingPosMask = 0b000000111111;
	const unsigned short blackKingPosMask = 0b111111000000;

	const std::unordered_map<int, std::wstring> pieceSymbols{
		{Piece::pawn | Piece::white, L"\x2659"},
		{Piece::knight | Piece::white, L"\x2658"},
		{Piece::bishop | Piece::white, L"\x2657"},
		{Piece::rook | Piece::white, L"\x2656"},
		{Piece::queen | Piece::white, L"\x2655"},
		{Piece::king | Piece::white, L"\x2654"},
		{Piece::pawn | Piece::black, L"\x265F"},
		{Piece::knight | Piece::black, L"\x265E"},
		{Piece::bishop | Piece::black, L"\x265D"},
		{Piece::rook | Piece::black, L"\x265C"},
		{Piece::queen | Piece::black, L"\x265B"},
		{Piece::king | Piece::black, L"\x265A"},
		{0, L" "}
	};

	Board();
	void initStartPosition();
	void loadPosition(std::string fen);
	void makeMove(class Move& move);
	void unmakeMove(Move& move);
	void setCurrentGameState(unsigned int gameState);
	int getWhiteKingPosI();
	int getWhiteKingPosJ();
	int getBlackKingPosI();
	int getBlackKingPosJ();
	void setWhiteKingPos(int i, int j);
	void setBlackKingPos(int i, int j);
	bool isKingInCheck(Move& move, bool filter, int kingColor);
	friend std::wostream& operator<<(std::wostream& out, Board& b);
	Direction findDirection(int kingI, int kingJ, int startPosI, int startPosJ);
	bool isDiscoveryCheck(Move& move, int kingI, int kingJ, int kingColor);
	void handleCastling(int targetI, int targetJ, int color);
	void handleEnPassantCapture(int targetI, int targetJ, int color);
	std::vector<Move> getAllMoves();
	bool isSquareAttacked(int kingI, int kingJ, int kingColor);

};

