#include "PieceList.h"

PieceList::PieceList(int maxPieceCount)
{
	occupiedSquares = new int[maxPieceCount];
	map = new int[64];
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
