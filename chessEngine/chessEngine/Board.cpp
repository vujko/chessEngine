#include "Board.h"
#include "Move.h"

PieceList Board::getPieceList(int pieceType, int colorIndex)
{
	return allPieceLists[colorIndex * 8 + pieceType];
}

void Board::makeMove(Move& m)
{
	unsigned int oldEnPassantFile = (currentGameState >> 4) & 15;
	unsigned int originalCastleState = currentGameState & 15;
	unsigned int newCastleState = originalCastleState;

	currentGameState = 0;
	int opponentColorIndex = 1 - colorToMoveIndex;
	int moveFrom = m.getStartSquare();
	int moveTo = m.getTargetSquare();

	int capturedPieceType = Piece::getPieceType(squares[moveTo]);
	int movePiece = squares[moveFrom];
	int movePieceType = Piece::getPieceType(movePiece);

	int moveFlag = m.getMoveFlag();
	bool isPromotion = m.isPromotion();
	bool isEnpassant = moveFlag == MoveFlag::enPassantCapture;

	//Handle captures
	currentGameState |= (capturedPieceType << 8);
	if (capturedPieceType != 0 && !isEnpassant) {
		getPieceList(capturedPieceType, opponentColorIndex).removePieceAtSquare(moveTo);
	}

	if (movePiece == Piece::king) {
		kingSquare[colorToMoveIndex] = moveTo;
		newCastleState &= (whiteToMove) ? whiteCastleMask : blackCastleMask;
	}
	else {
		getPieceList(movePieceType, colorToMoveIndex).movePiece(moveFrom, moveTo);
	}

	int pieceOnTargetSquare = movePiece;

	if (isPromotion) {
		int promoteType = 0;
		switch (moveFlag) {
		case MoveFlag::promoteToQueen:
			promoteType = Piece::queen;
			queens[colorToMoveIndex].addPieceAtSquare(moveTo);
			break;
		case MoveFlag::promoteToRook:
			promoteType = Piece::rook;
			rooks[colorToMoveIndex].addPieceAtSquare(moveTo);
			break;
		case MoveFlag::promoteToBishop:
			promoteType = Piece::bishop;
			bishops[colorToMoveIndex].addPieceAtSquare(moveTo);
			break;
		case MoveFlag::promoteToKnight:
			promoteType = Piece::knight;
			knights[colorToMoveIndex].addPieceAtSquare(moveTo);
			break;
		}
		pieceOnTargetSquare = promoteType | colorToMove;
		pawns[colorToMoveIndex].removePieceAtSquare(moveTo);
	}
	else {
		//handle castling and enPassant
		switch (moveFlag) {
		case MoveFlag::enPassantCapture:
			int epPawnSquare = moveTo + ((colorToMove == Piece::white) ? -8 : 8);
			//add pawn as captured type
			currentGameState |= (squares[epPawnSquare] << 8);
			squares[epPawnSquare] = Piece::none;
			pawns[opponentColorIndex].removePieceAtSquare(epPawnSquare);
			break;
		case MoveFlag::castling:
			bool kingSide = moveTo == 6 || moveTo == 62;
			int castlingRookFromIndex = (kingSide) ? moveTo + 1 : moveTo - 2;
			int castlingRookToIndex = (kingSide) ? moveTo - 1 : moveTo + 1;

			squares[castlingRookFromIndex] = Piece::none;
			squares[castlingRookToIndex] = Piece::rook | colorToMove;

			rooks[colorToMoveIndex].movePiece(castlingRookFromIndex, castlingRookToIndex);
			break;
		}
	}

	squares[moveTo] = pieceOnTargetSquare;
	squares[moveFrom] = Piece::none;

	if (moveFlag == MoveFlag::pawnTwoForward) {
		int file = (moveFrom & 0b000111) + 1;
		currentGameState |= (file << 4);
	}

	//Remove castle rights if a piece moves to or from rooks initial squares
	if (originalCastleState != 0) {
		if (moveTo == 7 || moveFrom == 7) 
			newCastleState &= whiteCastleKingsideMask;
		else if (moveTo == 0 || moveFrom == 0)
			newCastleState &= whiteCastleQueensideMask;

		if (moveTo == 63 || moveFrom == 63)
			newCastleState &= blackCastleKingsideMask;
		else if (moveTo == 56 || moveFrom == 56)
			newCastleState &= blackCastleQueensideMask;
	}

	currentGameState |= newCastleState;
	currentGameState |= (fiftyMoveCounter << 14);
	gameStateHistory.push(currentGameState);

	whiteToMove = !whiteToMove;
	colorToMove = (whiteToMove) ? Piece::white : Piece::black;
	opponentColor = (whiteToMove) ? Piece::black : Piece::white;
	colorToMoveIndex = 1 - colorToMoveIndex;
	plyCount++;
	fiftyMoveCounter++;


}

void Board::unmakeMove(Move& m)
{
	int opponentColorIndex = colorToMoveIndex;
	bool undoingWhiteMove = opponentColor == Piece::white;
	colorToMove = opponentColor;
	opponentColor = (undoingWhiteMove) ? Piece::black : Piece::white;
	colorToMoveIndex = 1 - colorToMoveIndex;
	whiteToMove = !whiteToMove;

	unsigned int originalCastleState = currentGameState & 0b1111;

	int capturedPieceType = (currentGameState >> 8) & 0b111111;
	int capturedPiece = (capturedPieceType == Piece::none) ? 0 : capturedPieceType | opponentColor;

	int movedFrom = m.getStartSquare();
	int movedTo = m.getTargetSquare();
	int moveFlag = m.getMoveFlag();
	bool isPromotion = m.isPromotion();
	bool isEnPassant = moveFlag == MoveFlag::enPassantCapture;

	int toSquarePieceType = Piece::getPieceType(squares[movedTo]);
	int movedPieceType = (isPromotion) ? Piece::pawn : toSquarePieceType;

	unsigned int oldEnPassantFile = (currentGameState >> 4) & 0b1111;

	//ignore enPasant captures handled later
	if (capturedPieceType != Piece::none && !isEnPassant) {
		getPieceList(capturedPieceType, opponentColorIndex).addPieceAtSquare(movedTo);
	}

	//update king index
	if (movedPieceType == Piece::king)
		kingSquare[colorToMoveIndex] = movedFrom;
	else if (!isPromotion)
		getPieceList(movedPieceType, colorToMoveIndex).movePiece(movedTo, movedFrom);

	squares[movedFrom] = movedPieceType | colorToMove;
	squares[movedTo] = capturedPiece;

	if (isPromotion) {
		pawns[colorToMoveIndex].addPieceAtSquare(movedFrom);
		switch (moveFlag) {
		case MoveFlag::promoteToQueen:
			queens[colorToMoveIndex].removePieceAtSquare(movedTo);
			break;
		case MoveFlag::promoteToKnight:
			knights[colorToMoveIndex].removePieceAtSquare(movedTo);
			break;
		case MoveFlag::promoteToRook:
			rooks[colorToMoveIndex].removePieceAtSquare(movedTo);
			break;
		case MoveFlag::promoteToBishop:
			bishops[colorToMoveIndex].removePieceAtSquare(movedTo);
			break;
		}
	}
	else if (isEnPassant) {
		//put captured pawn on right square
		int epIndex = movedTo + ((colorToMove == Piece::white) ? -8 : 8);
		squares[movedTo] = Piece::none;
		squares[movedFrom] = capturedPiece;
		pawns[opponentColorIndex].addPieceAtSquare(epIndex);
	}
	else if (moveFlag == MoveFlag::castling) {

	}
}
