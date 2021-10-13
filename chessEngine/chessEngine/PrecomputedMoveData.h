#pragma once
#include <cstddef>
#include <array>
#include <vector>
#include "Board.h"
static class PrecomputedMoveData
{
public:

	// First 4 are orthogonal, last 4 are diagonals (N, S, W, E, NW, SE, NE, SW)
	static const int* directionOffsets;

	// Stores number of moves available in each of the 8 directions for every square on the board
	// Order of directions is: N, S, W, E, NW, SE, NE, SW
	// So for example, if availableSquares[0][1] == 7...
	// that means that there are 7 squares to the north of b1 (the square with index 1 in board array)
	static std::vector<std::vector<int>> numSquaresToEdge;

	// Stores array of indices for each square a knight can land on from any square on the board
	// So for example, knightMoves[0] is equal to {10, 17}, meaning a knight on a1 can jump to c2 and b3
	static std::vector<std::vector<std::byte>> knightMoves;
	static std::vector<std::vector<std::byte>> kingMoves;

	// Pawn attack directions for white and black (NW, NE; SW SE)
	static const std::array<std::array<std::byte, 2>, 2> pawnAttackDirections;

	static std::vector<std::vector<int>> pawnAttacksWhite;
	static std::vector<std::vector<int>> pawnAttacksBlack;
	static const std::vector<int> directionLookup;

	static std::vector<unsigned long> kingAttackBitBoards;
	static std::vector<unsigned long> knightAttackBitBoards;
	static std::vector<std::vector<unsigned long>> pawnAttackBitBoards;

	static std::vector<unsigned long> rookMoves;
	static std::vector<unsigned long> bishopMoves;
	static std::vector<unsigned long> queenMoves;

	static std::vector<std::vector<int>> orthogonalDistance;
	static std::vector<std::vector<int>> kingDistance;
	static std::vector<int> centreManhattanDistance;

	friend class constructor;

	struct constructor {
		constructor() {
			pawnAttacksWhite = std::vector<std::vector<int>>();
			pawnAttacksBlack = std::vector<std::vector<int>>();
			//numSquaresToEdge = new int* [8];

			knightMoves = std::vector<std::vector<std::byte>>();
			kingMoves = std::vector<std::vector<std::byte>>();
			numSquaresToEdge = std::vector<std::vector<int>>();

			rookMoves = std::vector<unsigned long>();
			bishopMoves = std::vector<unsigned long>();
			queenMoves = std::vector<unsigned long>();
			// Calculate knight jumps and available squares for each square on the board.
			int allKnightJumps[8] = { 15, 17, -17, -15, 10, -6, 6, -10 };
			knightAttackBitBoards = std::vector<unsigned long>();
			kingAttackBitBoards = std::vector<unsigned long>();
			pawnAttackBitBoards = std::vector<std::vector<unsigned long>>();

			for (int squareIndex = 0; squareIndex < 64; squareIndex++) {
				int y = squareIndex / 8;
				int x = squareIndex - y * 8;

				int north = 7 - y;
				int south = y;
				int west = x;
				int east = 7 - x;

				numSquaresToEdge[squareIndex] = std::vector<int>{
					north,
					south,
					west,
					east,
					std::min(north,west),
					std::min(south,east),
					std::min(north,east),
					std::min(south,west) 
				};

				//calculate all squares knight can jump to from current square
				std::vector<std::byte> legalKnightJumps;
				unsigned long knightBitboard = 0;
				for (int knightJumpDelta = 0; knightJumpDelta < 8; knightJumpDelta++) {
					int knightJumpSquare = squareIndex + allKnightJumps[knightJumpDelta];
					if (knightJumpSquare >= 0 && knightJumpSquare < 64) {
						int knightSquareY = knightJumpSquare / 8;
						int knightSquareX = knightJumpSquare - knightSquareY * 8;
						// Ensure knight has moved max of 2 squares on x/y axis (to reject indices that have wrapped around side of board)
						int maxCoordMoveDst = std::max(std::abs(x - knightSquareX), std::abs(y - knightSquareY));
						if (maxCoordMoveDst == 2) {
							legalKnightJumps.push_back((std::byte)knightJumpSquare);
							knightBitboard |= 1ul << knightJumpSquare;
						}
					}
				}
				//mozda ovde dodje do greske
				knightMoves[squareIndex] = legalKnightJumps;
				knightAttackBitBoards[squareIndex] = knightBitboard;

				// Calculate all squares king can move to from current square (not including castling)
				std::vector<std::byte> legalKingMoves;
				for (int kingMoveDelta = 0; kingMoveDelta < 8; kingMoveDelta++) {
					int kingMoveSquare = squareIndex + directionOffsets[kingMoveDelta];
					if (kingMoveSquare >= 0 && kingMoveSquare < 64) {
						int kingSquareY = kingMoveSquare / 8;
						int kingSquareX = kingMoveSquare - kingSquareY * 8;
						// Ensure king has moved max of 1 square on x/y axis (to reject indices that have wrapped around side of board)
						int maxCoordDst = std::max(std::abs(x - kingSquareX), std::abs(y - kingSquareY));
						if (maxCoordDst == 1) {
							legalKingMoves.push_back((std::byte)kingMoveSquare);
							kingAttackBitBoards[squareIndex] |= 1ul << kingMoveSquare;
						}
					}
				}
				kingMoves[squareIndex] = legalKingMoves;

				//calculate legal pawn captures for white and black
				std::vector<int> pawnCapturesWhite;
				std::vector<int> pawnCapturesBlack;
				pawnAttackBitBoards[squareIndex] = std::vector<unsigned long>();
				if (x > 0) {
					if (y < 7) {
						pawnCapturesWhite.push_back(squareIndex + 7);
						pawnAttackBitBoards[squareIndex][Board::whiteIndex] |= 1ul << (squareIndex + 7);
					}
					if (y > 0) {
						pawnCapturesBlack.push_back(squareIndex - 9);
						pawnAttackBitBoards[squareIndex][Board::blackIndex] |= 1ul << (squareIndex - 9);
					}
				}
				if (x < 7) {
					if (y < 7) {
						pawnCapturesWhite.push_back(squareIndex + 9);
						pawnAttackBitBoards[squareIndex][Board::whiteIndex] |= 1ul << (squareIndex + 9);
					}
					if (y > 0) {
						pawnCapturesBlack.push_back(squareIndex - 7);
						pawnAttackBitBoards[squareIndex][Board::blackIndex] |= 1ul << (squareIndex - 7);
					}
				}
				pawnAttacksWhite[squareIndex] = pawnCapturesWhite;
				pawnAttacksBlack[squareIndex] = pawnCapturesBlack;

				//Rook moves
				for (int directionIndex = 0; directionIndex < 4; directionIndex++) {
					int currentDirOffset = directionOffsets[directionIndex];
					for (int n = 0; n < numSquaresToEdge[squareIndex][directionIndex]; n++) {
						int targetSquare = squareIndex + currentDirOffset * (n + 1);
						rookMoves[squareIndex] |= 1ul << targetSquare;
					}
				}
				// Bishop moves
				for (int directionIndex = 4; directionIndex < 8; directionIndex++) {
					int currentDirOffset = directionOffsets[directionIndex];
					for (int n = 0; n < numSquaresToEdge[squareIndex][directionIndex]; n++) {
						int targetSquare = squareIndex + currentDirOffset * (n + 1);
						bishopMoves[squareIndex] |= 1ul << targetSquare;
					}
				}
				queenMoves[squareIndex] = rookMoves[squareIndex] | bishopMoves[squareIndex];
			}

			std::vector<int> directionLookup;
			for (int i = 0; i < 127; i++) {
				int offset = i - 63;
				int absOffset = std::abs(offset);
				int absDir = 1;
				if (absOffset % 9 == 0) {
					absDir = 9;
				}
				else if (absOffset % 8 == 0) {
					absDir = 8;
				}
				else if (absOffset % 7 == 0) {
					absDir = 7;
				}
				int multiplier = (offset < 0) ? -1 : +1;
				multiplier = (offset == 0) ? 0 : multiplier;
				directionLookup[i] = absDir * multiplier;
			}

			orthogonalDistance = std::vector<std::vector<int>>();
			kingDistance = std::vector<std::vector<int>>();
			centreManhattanDistance = std::vector<int>();
			for (int squareA = 0; squareA < 64; squareA++) {

				int AFileIndex = squareA & 0b000111;
				int ARankIndex = squareA >> 3;
				int fileDstFromCentre = std::max(3 - AFileIndex, AFileIndex - 4);
				int rankDstFromCentre = std::max(3 - ARankIndex, ARankIndex - 4);
				centreManhattanDistance[squareA] = fileDstFromCentre + rankDstFromCentre;

				for (int squareB = 0; squareB < 64; squareB++) {

					int BFileIndex = squareB & 0b000111;
					int BRankIndex = squareB >> 3;
					int rankDistance = std::abs(ARankIndex - BRankIndex);
					int fileDistance = std::abs(AFileIndex - BFileIndex);
					orthogonalDistance[squareA][squareB] = fileDistance + rankDistance;
					kingDistance[squareA][squareB] = std::max(fileDistance, rankDistance);
				}
			}
		}
	};


};

