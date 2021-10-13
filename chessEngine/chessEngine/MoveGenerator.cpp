#include "MoveGenerator.h"
#include "Board.h"
#include "PrecomputedMoveData.h"
#include "BitBoardUtility.h"

std::vector<Move&> MoveGenerator::generateMoves(Board& b)
{
	initialize();

}

void MoveGenerator::initialize()
{
	moves = std::vector<Move&>(64);
	inCheck = false;
	inDoubleCheck = false;
	pinsExistInPosition = false;
	checkRayBitmask = 0;
	pinRayBitmask = 0;

	isWhiteToMove = board.colorToMove == Piece::white;
	friendlyColour = board.colorToMove;
	opponentColour = board.opponentColor;
	friendlyKingSquare = board.kingSquare[board.colorToMoveIndex];
	friendlyColourIndex = (board.whiteToMove) ? Board::whiteIndex : Board::blackIndex;
	opponentColourIndex = 1 - friendlyColourIndex;
}

void MoveGenerator::calculateAttackData()
{
	generateSlidingAttackMap();
	// Search squares in all directions around friendly king for checks/pins by enemy sliding pieces (queen, rook, bishop)
	int startDirIndex = 0;
	int endDirIndex = 8;

	if (board.queens[opponentColourIndex].numPieces == 0) {
		startDirIndex = (board.rooks[opponentColourIndex].numPieces > 0) ? 0 : 4;
		endDirIndex = (board.bishops[opponentColourIndex].numPieces > 0) ? 8 : 4;
	}

	for (int dir = startDirIndex; dir < endDirIndex; dir++) {
		bool isDiagonal = dir > 3;

		int n = PrecomputedMoveData::numSquaresToEdge[friendlyKingSquare][dir];
		int directionOffset = PrecomputedMoveData::directionOffsets[dir];
		bool isFriendlyPieceAlongRay = false;
		unsigned long rayMask = 0;

		for (int i = 0; i < n; i++) {
			int squareIndex = friendlyKingSquare + directionOffset * (i + 1);
			rayMask |= 1ul << squareIndex;
			int piece = board.squares[squareIndex];

			// This square contains a piece
			if (piece != Piece::none) {
				if (Piece::isColor(piece, friendlyColour)) {
					// First friendly piece we have come across in this direction, so it might be pinned
					if (!isFriendlyPieceAlongRay) {
						isFriendlyPieceAlongRay = true;
					}
					// This is the second friendly piece we've found in this direction, therefore pin is not possible
					else {
						break;
					}
				}
				// This square contains an enemy piece
				else {
					int pieceType = Piece::getPieceType(piece);

					// Check if piece is in bitmask of pieces able to move in current direction
					if (isDiagonal && Piece::isBishopOrQueen(pieceType) || !isDiagonal && Piece::isRookOrQueen(pieceType)) {
						// Friendly piece blocks the check, so this is a pin
						if (isFriendlyPieceAlongRay) {
							pinsExistInPosition = true;
							pinRayBitmask |= rayMask;
						}
						// No friendly piece blocking the attack, so this is a check
						else {
							checkRayBitmask |= rayMask;
							inDoubleCheck = inCheck; // if already in check, then this is double check
							inCheck = true;
						}
						break;
					}
					else {
						// This enemy piece is not able to move in the current direction, and so is blocking any checks/pins
						break;
					}
				}
			}
		}
		// Stop searching for pins if in double check, as the king is the only piece able to move in that case anyway
		if (inDoubleCheck) {
			break;
		}
	}

	// Knight attacks
	PieceList opponentKnights = board.knights[opponentColourIndex];
	opponentKnightAttacks = 0;
	bool isKnightCheck = false;

	for (int knightIndex = 0; knightIndex < opponentKnights.Count; knightIndex++) {
		int startSquare = opponentKnights[knightIndex];
		opponentKnightAttacks |= knightAttackBitboards[startSquare];

		if (!isKnightCheck && BitBoardUtility.ContainsSquare(opponentKnightAttacks, friendlyKingSquare)) {
			isKnightCheck = true;
			inDoubleCheck = inCheck; // if already in check, then this is double check
			inCheck = true;
			checkRayBitmask |= 1ul << startSquare;
		}
	}

	// Pawn attacks
	PieceList opponentPawns = board.pawns[opponentColourIndex];
	opponentPawnAttackMap = 0;
	bool isPawnCheck = false;

	for (int pawnIndex = 0; pawnIndex < opponentPawns.numPieces; pawnIndex++) {
		int pawnSquare = opponentPawns[pawnIndex];
		unsigned long pawnAttacks = PrecomputedMoveData::pawnAttackBitBoards[pawnSquare][opponentColourIndex];
		opponentPawnAttackMap |= pawnAttacks;

		if (!isPawnCheck && BitBoardUtility::ContainsSquare(pawnAttacks, friendlyKingSquare)) {
			isPawnCheck = true;
			inDoubleCheck = inCheck; // if already in check, then this is double check
			inCheck = true;
			checkRayBitmask |= 1ul << pawnSquare;
		}
	}

	int enemyKingSquare = board.kingSquare[opponentColourIndex];

	opponentAttackMapNoPawns = opponentSlidingAttackMap | opponentKnightAttacks | PrecomputedMoveData::kingAttackBitBoards[enemyKingSquare];
	opponentAttackMap = opponentAttackMapNoPawns | opponentPawnAttackMap;
}

void MoveGenerator::generateSlidingAttackMap()
{
	opponentSlidingAttackMap = 0;

	PieceList enemyRooks = board.rooks[opponentColourIndex];
	for (int i = 0; i < enemyRooks.numPieces; i++) {
		updateSlidingAttackPiece(enemyRooks[i], 0, 4);
	}

	PieceList enemyQueens = board.queens[opponentColourIndex];
	for (int i = 0; i < enemyRooks.numPieces; i++) {
		updateSlidingAttackPiece(enemyQueens[i], 0, 4);
	}

	PieceList enemyBishops = board.bishops[opponentColourIndex];
	for (int i = 0; i < enemyRooks.numPieces; i++) {
		updateSlidingAttackPiece(enemyBishops[i], 0, 4);
	}
}

void MoveGenerator::updateSlidingAttackPiece(int startSquare, int startDirIndex, int endDirIndex)
{
	for (int directionIndex = startDirIndex; directionIndex < endDirIndex; directionIndex++) {
		int currentDirOffset = PrecomputedMoveData::directionOffsets[directionIndex];
		for (int n = 0; n < PrecomputedMoveData::numSquaresToEdge[startSquare][directionIndex]; n++) {
			int targetSquare = startSquare + currentDirOffset * (n + 1);
			int targetSquarePiece = board.squares[targetSquare];
			opponentSlidingAttackMap |= 1ul << targetSquare;
			if (targetSquare != friendlyKingSquare) {
				if (targetSquarePiece != Piece::none) {
					break;
				}
			}
		}
	}
}
