#pragma once
#include <cstddef>
#include <vector>
class PieceList
{

public:

	std::vector<int> occupiedSquares;
	std::vector<int> map;
	int numPieces;

	PieceList();
	PieceList(int maxPieceCount);
	void addPieceAtSquare(int square);
	void removePieceAtSquare(int square);
	void movePiece(int startSquare, int targetSquare);
	int& operator[](std::size_t idx);

};

