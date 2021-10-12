#pragma once
#include <vector>
static class Piece
{
public:
	static const int none = 0;
	static const int king = 1;
	static const int pawn = 2;
	static const int knight = 3;
	static const int bishop = 5;
	static const int rook = 6;
	static const int queen = 7;

	static const int white = 8;
	static const int black = 16;

	static const int typeMask = 0b00111;
	static const int colorMask = 0b11000;

	static bool isColor(int piece, int color);
	static int getPieceColor(int piece);
	static int getPieceType(int piece);
	static bool isSlidingPiece(int piece);

	static std::vector<class Move> getMoves(class Board* b, int i, int j);
	static std::vector<Move> getPawnMoves(Board* b, int i, int j);
	static std::vector<Move> getKnightMoves(Board* b, int i, int j);
	static std::vector<Move> getKingMoves(Board* b, int i, int j);
	static std::vector<Move> getSlidingPieceMoves(Board* b, int i, int j, int pieceType);
};

