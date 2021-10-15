#include "PieceList.h"

PieceList::PieceList()
{
	occupiedSquares = std::vector<int>(0);
	map = std::vector<int>(64);
	numPieces = 0;
}

PieceList::PieceList(int maxPieceCount)
{
	occupiedSquares = std::vector<int>(maxPieceCount);
	map = std::vector<int>(64);
	numPieces = 0;
}

void PieceList::addPieceAtSquare(int square)
{

	occupiedSquares[numPieces] = square;
	map[square] = numPieces;
	numPieces++;
}

void PieceList::removePieceAtSquare(int square)
{
	
	int pieceIndex = map[square];
	occupiedSquares[pieceIndex] = occupiedSquares[numPieces - 1];
	map[occupiedSquares[pieceIndex]] = pieceIndex;
	numPieces--;
}

void PieceList::movePiece(int startSquare, int targetSquare)
{
	int pieceIndex = map[startSquare];
	occupiedSquares[pieceIndex] = targetSquare;
	map[targetSquare] = pieceIndex;

}

int& PieceList::operator[](std::size_t idx)
{
	return occupiedSquares[idx];
}
