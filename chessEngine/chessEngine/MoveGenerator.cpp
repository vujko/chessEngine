#include "MoveGenerator.h"
#include "Board.h"
#include "PrecomputedMoveData.h"
#include "BitBoardUtility.h"
#include "Move.h"

std::vector<Move> MoveGenerator::generateMoves(Board& b)
{
	initialize();
	calculateAttackData();
	generateKingMoves();

	//Only king moves are valid in double check
	if (inDoubleCheck)
		return moves;

	generateSlidingMoves();
	generateKnightMoves();
	generatePawnMoves();

	return moves;
}

void MoveGenerator::initialize()
{
	moves = std::vector<Move>(64);
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

	for (int knightIndex = 0; knightIndex < opponentKnights.numPieces; knightIndex++) {
		int startSquare = opponentKnights[knightIndex];
		opponentKnightAttacks |= PrecomputedMoveData::knightAttackBitBoards[startSquare];

		if (!isKnightCheck && BitBoardUtility::containsSquare(opponentKnightAttacks, friendlyKingSquare)) {
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

		if (!isPawnCheck && BitBoardUtility::containsSquare(pawnAttacks, friendlyKingSquare)) {
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

void MoveGenerator::generateKingMoves()
{
	for (int i = 0; i < PrecomputedMoveData::kingMoves[friendlyKingSquare].size(); i++) {
		int targetSquare = (int) PrecomputedMoveData::kingMoves[friendlyKingSquare][i];
		int pieceOnTargetSquare = board.squares[targetSquare];

		// Skip squares occupied by friendly pieces
		if (Piece::isColor(pieceOnTargetSquare, friendlyColour)) {
			continue;
		}

		bool isCapture = Piece::isColor(pieceOnTargetSquare, opponentColour);
		if (!isCapture) {
			// King can't move to square marked as under enemy control, unless he is capturing that piece
			// Also skip if not generating quiet moves
			if (squareIsInCheckRay(targetSquare)) { //!genQuiets || 
				continue;
			}
		}

		// Safe for king to move to this square
		if (!squareIsAttacked(targetSquare)) {
			moves.push_back(Move(friendlyKingSquare, targetSquare));

			// Castling:
			if (!inCheck && !isCapture) {
				// Castle kingside
				if ((targetSquare == 5 || targetSquare == 61) && hasKingsideCastleRight()) {
					int castleKingsideSquare = targetSquare + 1;
					if (board.squares[castleKingsideSquare] == Piece::none) {
						if (!squareIsAttacked(castleKingsideSquare)) {
							moves.push_back(Move(friendlyKingSquare, castleKingsideSquare, MoveFlag::castling));
						}
					}
				}
				// Castle queenside
				else if ((targetSquare == 3 || targetSquare == 59) && hasQueensideCastleRight()) {
					int castleQueensideSquare = targetSquare - 1;
					if (board.squares[castleQueensideSquare] == Piece::none && board.squares[castleQueensideSquare - 1] == Piece::none) {
						if (!squareIsAttacked(castleQueensideSquare)) {
							moves.push_back(Move(friendlyKingSquare, castleQueensideSquare, MoveFlag::castling));
						}
					}
				}
			}
		}
	}
}

void MoveGenerator::generateSlidingMoves()
{
	PieceList rooks = board.rooks[friendlyColourIndex];
	for (int i = 0; i < rooks.numPieces; i++) {
		generateSlidingPieceMoves(rooks[i], 0, 4);
	}

	PieceList bishops = board.bishops[friendlyColourIndex];
	for (int i = 0; i < bishops.numPieces; i++) {
		generateSlidingPieceMoves(bishops[i], 4, 8);
	}

	PieceList queens = board.queens[friendlyColourIndex];
	for (int i = 0; i < queens.numPieces; i++) {
		generateSlidingPieceMoves(queens[i], 0, 8);
	}
}

void MoveGenerator::generateSlidingPieceMoves(int startSquare, int startDirIndex, int endDirIndex)
{
	bool squareIsPinned = isPinned(startSquare);

	// If this piece is pinned, and the king is in check, this piece cannot move
	if (inCheck && squareIsPinned) {
		return;
	}

	for (int directionIndex = startDirIndex; directionIndex < endDirIndex; directionIndex++) {
		int currentDirOffset = PrecomputedMoveData::directionOffsets[directionIndex];

		// If pinned, this piece can only move along the ray towards/away from the friendly king, so skip other directions
		if (squareIsPinned && !isMovingAlongRay(currentDirOffset, friendlyKingSquare, startSquare)) {
			continue;
		}

		for (int n = 0; n < PrecomputedMoveData::numSquaresToEdge[startSquare][directionIndex]; n++) {
			int targetSquare = startSquare + currentDirOffset * (n + 1);
			int targetSquarePiece = board.squares[targetSquare];

			// Blocked by friendly piece, so stop looking in this direction
			if (Piece::isColor(targetSquarePiece, friendlyColour)) {
				break;
			}
			bool isCapture = targetSquarePiece != Piece::none;

			bool movePreventsCheck = squareIsInCheckRay(targetSquare);
			if (movePreventsCheck || !inCheck) {
				if (isCapture) { //genQuiets || 
					moves.push_back(Move(startSquare, targetSquare));
				}
			}
			// If square not empty, can't move any further in this direction
			// Also, if this move blocked a check, further moves won't block the check
			if (isCapture || movePreventsCheck) {
				break;
			}
		}
	}
}

void MoveGenerator::generateKnightMoves()
{
	PieceList myKnights = board.knights[friendlyColourIndex];

	for (int i = 0; i < myKnights.numPieces; i++) {
		int startSquare = myKnights[i];

		// Knight cannot move if it is pinned
		if (isPinned(startSquare)) {
			continue;
		}

		for (int knightMoveIndex = 0; knightMoveIndex < PrecomputedMoveData::knightMoves[startSquare].size(); knightMoveIndex++) {
			int targetSquare = (int)PrecomputedMoveData::knightMoves[startSquare][knightMoveIndex];
			int targetSquarePiece = board.squares[targetSquare];
			bool isCapture = Piece::isColor(targetSquarePiece, opponentColour);
			if (isCapture) { //genQuiets || 
				// Skip if square contains friendly piece, or if in check and knight is not interposing/capturing checking piece
				if (Piece::isColor(targetSquarePiece, friendlyColour) || (inCheck && !squareIsInCheckRay(targetSquare))) {
					continue;
				}
				moves.push_back(Move(startSquare, targetSquare));
			}
		}
	}
}

void MoveGenerator::generatePawnMoves()
{
	PieceList myPawns = board.pawns[friendlyColourIndex];
	int pawnOffset = (friendlyColour == Piece::white) ? 8 : -8;
	int startRank = (board.whiteToMove) ? 1 : 6;
	int finalRankBeforePromotion = (board.whiteToMove) ? 6 : 1;

	int enPassantFile = ((int)(board.currentGameState >> 4) & 15) - 1;
	int enPassantSquare = -1;
	if (enPassantFile != -1) {
		enPassantSquare = 8 * ((board.whiteToMove) ? 5 : 2) + enPassantFile;
	}

	for (int i = 0; i < myPawns.numPieces; i++) {
		int startSquare = myPawns[i];
		int rank = startSquare >> 3;
		bool oneStepFromPromotion = rank == finalRankBeforePromotion;

		//if (genQuiets) {

		int squareOneForward = startSquare + pawnOffset;

		// Square ahead of pawn is empty: forward moves
		if (board.squares[squareOneForward] == Piece::none) {
			// Pawn not pinned, or is moving along line of pin
			if (!isPinned(startSquare) || isMovingAlongRay(pawnOffset, startSquare, friendlyKingSquare)) {
				// Not in check, or pawn is interposing checking piece
				if (!inCheck || squareIsInCheckRay(squareOneForward)) {
					if (oneStepFromPromotion) {
						makePromotionMoves(startSquare, squareOneForward);
					}
					else {
						moves.push_back(Move(startSquare, squareOneForward));
					}
				}

				// Is on starting square (so can move two forward if not blocked)
				if (rank == startRank) {
					int squareTwoForward = squareOneForward + pawnOffset;
					if (board.squares[squareTwoForward] == Piece::none) {
						// Not in check, or pawn is interposing checking piece
						if (!inCheck || squareIsInCheckRay(squareTwoForward)) {
							moves.push_back(Move(startSquare, squareTwoForward, MoveFlag::pawnTwoForward));
						}
					}
				}
			}
		}
		

		// Pawn captures.
		for (int j = 0; j < 2; j++) {
			// Check if square exists diagonal to pawn
			if (PrecomputedMoveData::numSquaresToEdge[startSquare][(int)PrecomputedMoveData::pawnAttackDirections[friendlyColourIndex][j]] > 0) {
				// move in direction friendly pawns attack to get square from which enemy pawn would attack
				int pawnCaptureDir = (int)PrecomputedMoveData::directionOffsets[(int)PrecomputedMoveData::pawnAttackDirections[friendlyColourIndex][j]];
				int targetSquare = startSquare + pawnCaptureDir;
				int targetPiece = board.squares[targetSquare];

				// If piece is pinned, and the square it wants to move to is not on same line as the pin, then skip this direction
				if (isPinned(startSquare) && !isMovingAlongRay(pawnCaptureDir, friendlyKingSquare, startSquare)) {
					continue;
				}

				// Regular capture
				if (Piece::isColor(targetPiece, opponentColour)) {
					// If in check, and piece is not capturing/interposing the checking piece, then skip to next square
					if (inCheck && !squareIsInCheckRay(targetSquare)) {
						continue;
					}
					if (oneStepFromPromotion) {
						makePromotionMoves(startSquare, targetSquare);
					}
					else {
						moves.push_back(Move(startSquare, targetSquare));
					}
				}

				// Capture en-passant
				if (targetSquare == enPassantSquare) {
					int epCapturedPawnSquare = targetSquare + ((board.whiteToMove) ? -8 : 8);
					if (!inCheckAfterEnPassant(startSquare, targetSquare, epCapturedPawnSquare)) {
						moves.push_back(Move(startSquare, targetSquare, MoveFlag::enPassantCapture));
					}
				}
			}
		}
	}
}

void MoveGenerator::makePromotionMoves(int fromSquare, int toSquare)
{
	moves.push_back(Move(fromSquare, toSquare, MoveFlag::promoteToQueen));
	moves.push_back(Move(fromSquare, toSquare, MoveFlag::promoteToKnight));
	moves.push_back(Move(fromSquare, toSquare, MoveFlag::promoteToRook));
	moves.push_back(Move(fromSquare, toSquare, MoveFlag::promoteToBishop));
}

bool MoveGenerator::hasKingsideCastleRight()
{
	int mask = (board.whiteToMove) ? 1 : 4;
	return (board.currentGameState & mask) != 0;
}

bool MoveGenerator::hasQueensideCastleRight()
{
	int mask = (board.whiteToMove) ? 2 : 8;
	return (board.currentGameState & mask) != 0;
}

bool MoveGenerator::squareIsAttacked(int square)
{
	return BitBoardUtility::containsSquare(opponentAttackMap, square);
}

bool MoveGenerator::squareIsInCheckRay(int square)
{
	return inCheck && ((checkRayBitmask >> square) & 1) != 0;
}

bool MoveGenerator::isMovingAlongRay(int rayDir, int startSquare, int targetSquare)
{
	int moveDir = PrecomputedMoveData::directionLookup[targetSquare - startSquare + 63];
	return (rayDir == moveDir || -rayDir == moveDir);
}

bool MoveGenerator::isPinned(int square)
{
	return pinsExistInPosition && ((pinRayBitmask >> square) & 1) != 0;
}

bool MoveGenerator::inCheckAfterEnPassant(int startSquare, int targetSquare, int epCapturedPawnSquare)
{
	// Update board to reflect en-passant capture
	board.squares[targetSquare] = board.squares[startSquare];
	board.squares[startSquare] = Piece::none;
	board.squares[epCapturedPawnSquare] = Piece::none;

	bool inCheckAfterEpCapture = false;
	if (squareAttackedAfterEPCapture(epCapturedPawnSquare, startSquare)) {
		inCheckAfterEpCapture = true;
	}

	// Undo change to board
	board.squares[targetSquare] = Piece::none;
	board.squares[startSquare] = Piece::pawn | friendlyColour;
	board.squares[epCapturedPawnSquare] = Piece::pawn | opponentColour;
	return inCheckAfterEpCapture;
}

bool MoveGenerator::squareAttackedAfterEPCapture(int epCaptureSquare, int capturingPawnStartSquare)
{
	if (BitBoardUtility::containsSquare(opponentAttackMapNoPawns, friendlyKingSquare)) {
		return true;
	}

	// Loop through the horizontal direction towards ep capture to see if any enemy piece now attacks king
	int dirIndex = (epCaptureSquare < friendlyKingSquare) ? 2 : 3;
	for (int i = 0; i < PrecomputedMoveData::numSquaresToEdge[friendlyKingSquare][dirIndex]; i++) {
		int squareIndex = friendlyKingSquare + PrecomputedMoveData::directionOffsets[dirIndex] * (i + 1);
		int piece = board.squares[squareIndex];
		if (piece != Piece::none) {
			// Friendly piece is blocking view of this square from the enemy.
			if (Piece::isColor(piece, friendlyColour)) {
				break;
			}
			// This square contains an enemy piece
			else {
				if (Piece::isRookOrQueen(piece)) {
					return true;
				}
				else {
					// This piece is not able to move in the current direction, and is therefore blocking any checks along this line
					break;
				}
			}
		}
	}

	// check if enemy pawn is controlling this square (can't use pawn attack bitboard, because pawn has been captured)
	for (int i = 0; i < 2; i++) {
		// Check if square exists diagonal to friendly king from which enemy pawn could be attacking it
		if (PrecomputedMoveData::numSquaresToEdge[friendlyKingSquare][(int)PrecomputedMoveData::pawnAttackDirections[friendlyColourIndex][i]] > 0) {
			// move in direction friendly pawns attack to get square from which enemy pawn would attack
			int piece = board.squares[friendlyKingSquare + PrecomputedMoveData::directionOffsets[(int)PrecomputedMoveData::pawnAttackDirections[friendlyColourIndex][i]]];
			if (piece == (Piece::pawn | opponentColour)) // is enemy pawn
			{
				return true;
			}
		}
	}

	return false;

}
