#pragma once
#include <cstddef>
class PieceList
{

public:

	int* occupiedSquares;
	int* map;
	int numPieces;

	PieceList(int maxPieceCount = 16);
	void addPieceAtSquare(int square);
	void removePieceAtSquare(int square);
	void movePiece(int startSquare, int targetSquare);
	int& operator[](std::size_t idx);

};

