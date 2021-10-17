#include "Board.h"
#include "Move.h"
#include "FenUtility.h"
#include "Move.h"
#include "Perft.h"
PieceList* Board::getPieceList(int pieceType, int colorIndex)
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
		getPieceList(capturedPieceType, opponentColorIndex)->removePieceAtSquare(moveTo);
	}

	if (movePieceType == Piece::king) {
		kingSquare[colorToMoveIndex] = moveTo;
		newCastleState &= (whiteToMove) ? whiteCastleMask : blackCastleMask;
	}
	else {
		if (getPieceList(movePieceType, colorToMoveIndex)->numPieces == 0) {
			int x = 123;
			std::wcout << moveName(m).c_str() << std::endl;
			std::wcout << FenUtility::currentFen(this).c_str() << std::endl;
		}
		getPieceList(movePieceType, colorToMoveIndex)->movePiece(moveFrom, moveTo);
	}

	int pieceOnTargetSquare = movePiece;

	if (isPromotion) {
		int promoteType = 0;
		switch (moveFlag) {
		case MoveFlag::promoteToQueen:
			promoteType = Piece::queen;
			queens[colorToMoveIndex]->addPieceAtSquare(moveTo);
			break;
		case MoveFlag::promoteToRook:
			promoteType = Piece::rook;
			rooks[colorToMoveIndex]->addPieceAtSquare(moveTo);
			break;
		case MoveFlag::promoteToBishop:
			promoteType = Piece::bishop;
			bishops[colorToMoveIndex]->addPieceAtSquare(moveTo);
			break;
		case MoveFlag::promoteToKnight:
			promoteType = Piece::knight;
			knights[colorToMoveIndex]->addPieceAtSquare(moveTo);
			break;
		}
		pieceOnTargetSquare = promoteType | colorToMove;
		pawns[colorToMoveIndex]->removePieceAtSquare(moveTo);
	}
	else {
		//handle castling and enPassant
		switch (moveFlag) {
		case MoveFlag::enPassantCapture:
			handleEnPassant(moveTo, opponentColorIndex);
			break;
		case MoveFlag::castling:
			handleCastling(moveTo);
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
		getPieceList(capturedPieceType, opponentColorIndex)->addPieceAtSquare(movedTo);
	}

	//update king index
	if (movedPieceType == Piece::king)
		kingSquare[colorToMoveIndex] = movedFrom;
	else if (!isPromotion)
		getPieceList(movedPieceType, colorToMoveIndex)->movePiece(movedTo, movedFrom);

	squares[movedFrom] = movedPieceType | colorToMove;
	squares[movedTo] = capturedPiece;

	if (isPromotion) {
		pawns[colorToMoveIndex]->addPieceAtSquare(movedFrom);
		switch (moveFlag) {
		case MoveFlag::promoteToQueen:
			queens[colorToMoveIndex]->removePieceAtSquare(movedTo);
			break;
		case MoveFlag::promoteToKnight:
			knights[colorToMoveIndex]->removePieceAtSquare(movedTo);
			break;
		case MoveFlag::promoteToRook:
			rooks[colorToMoveIndex]->removePieceAtSquare(movedTo);
			break;
		case MoveFlag::promoteToBishop:
			bishops[colorToMoveIndex]->removePieceAtSquare(movedTo);
			break;
		}
	}
	else if (isEnPassant) {
		//put captured pawn on right square
		int epIndex = movedTo + ((colorToMove == Piece::white) ? -8 : 8);
		squares[movedTo] = Piece::none;
		squares[epIndex] = capturedPiece;
		pawns[opponentColorIndex]->addPieceAtSquare(epIndex);
	}
	else if (moveFlag == MoveFlag::castling) {
		bool kingSide = movedTo == 6 || movedTo == 62;
		int castlingRookFromIndex = (kingSide) ? movedTo + 1 : movedTo - 2;
		int castlingRookToIndex = (kingSide) ? movedTo - 1 : movedTo + 1;

		squares[castlingRookToIndex] = Piece::none;
		squares[castlingRookFromIndex] = Piece::rook | colorToMove;

		rooks[colorToMoveIndex]->movePiece(castlingRookToIndex, castlingRookFromIndex);
	}

	gameStateHistory.pop();
	currentGameState = gameStateHistory.top();

	fiftyMoveCounter = (currentGameState & 4294950912) >> 14;
	//int newEnPassantFile = (currentGameState >> 4) & 15;
	plyCount--;

}

Board::Board()
{
	initialize();
}

//Board::Board(const Board& other)
//{
//	squares = other.squares;
//	whiteToMove = other.whiteToMove;
//	colorToMove = other.colorToMove;
//	opponentColor = other.opponentColor;
//	colorToMoveIndex = other.colorToMoveIndex;
//
//	currentGameState = other.currentGameState;
//	gameStateHistory = other.gameStateHistory;
//
//	plyCount = other.plyCount;
//	fiftyMoveCounter = other.fiftyMoveCounter;
//
//	kingSquare = other.kingSquare;
//
//	rooks = std::vector<PieceList*>();
//	for (int i = 0; i < other.rooks.size(); i++) {
//		rooks.push_back(new PieceList());
//		*(rooks[i]) = *(other.rooks[i]);
//	}
//	bishops = std::vector<PieceList*>();
//	for (int i = 0; i < other.bishops.size(); i++) {
//		bishops.push_back(new PieceList());
//		*(bishops[i]) = *(other.bishops[i]);
//	};
//	queens = std::vector<PieceList*>();
//	for (int i = 0; i < other.queens.size(); i++) {
//		queens.push_back(new PieceList());
//		*(queens[i]) = *(other.queens[i]);
//	};
//	knights = std::vector<PieceList*>();
//	for (int i = 0; i < other.knights.size(); i++) {
//		knights.push_back(new PieceList());
//		*(knights[i]) = *(other.knights[i]);
//	};
//	pawns = std::vector<PieceList*>();
//	for (int i = 0; i < other.pawns.size(); i++) {
//		pawns.push_back(new PieceList());
//		*(pawns[i]) = *(other.pawns[i]);
//	};
//
//	allPieceLists = std::vector<PieceList*>();
//	for (int i = 0; i < other.allPieceLists.size(); i++) {
//		allPieceLists.push_back(new PieceList());
//		*(allPieceLists[i]) = *(other.allPieceLists[i]);
//	};
//}

void Board::loadPosition(std::string fen)
{
	initialize();
	LoadedPositionInfo loadedPosition = FenUtility::getPositionFromFen(fen);
	for (int squareIndex = 0; squareIndex < 64; squareIndex++) {
		int piece = loadedPosition.squares[squareIndex];
		squares[squareIndex] = piece;

		if (piece != Piece::none) {
			int pieceType = Piece::getPieceType(piece);
			int pieceColorIndex = (Piece::isColor(piece, Piece::white)) ? whiteIndex : blackIndex;
			if (Piece::isSlidingPiece(piece)) {
				if (pieceType == Piece::queen)
					queens[pieceColorIndex]->addPieceAtSquare(squareIndex);
				else if (pieceType == Piece::rook)
					rooks[pieceColorIndex]->addPieceAtSquare(squareIndex);
				else if (pieceType == Piece::bishop)
					bishops[pieceColorIndex]->addPieceAtSquare(squareIndex);
			}
			else if (pieceType == Piece::knight)
				knights[pieceColorIndex]->addPieceAtSquare(squareIndex);
			else if (pieceType == Piece::pawn)
				pawns[pieceColorIndex]->addPieceAtSquare(squareIndex);
			else if (pieceType == Piece::king)
				kingSquare[pieceColorIndex] = squareIndex;
		}
	}

	whiteToMove = loadedPosition.whiteToMove;
	colorToMove = (whiteToMove) ? Piece::white : Piece::black;
	opponentColor = (whiteToMove) ? Piece::black : Piece::white;
	colorToMoveIndex = (whiteToMove) ? 0 : 1;

	//create gamestate
	int whiteCastle = ((loadedPosition.whiteCastleKingside) ? 1 << 0 : 0) | ((loadedPosition.whiteCastleQueenside) ? 1 << 1 : 0);
	int blackCastle = ((loadedPosition.blackCastleKingside) ? 1 << 2 : 0) | ((loadedPosition.blackCastleQueenside) ? 1 << 3 : 0);
	int epState = loadedPosition.epFile << 4;
	unsigned short initialGameState = (whiteCastle | blackCastle | epState);
	gameStateHistory.push(initialGameState);
	currentGameState = initialGameState;
	plyCount = loadedPosition.plyCount;
}

void Board::initialize()
{
	squares = std::vector<int>(64);
	kingSquare = std::vector<int>(2);
	currentGameState = 0;
	plyCount = 0;
	fiftyMoveCounter = 0;

	knights = std::vector<PieceList*>{ new PieceList(10), new PieceList(10) };
	pawns = std::vector<PieceList*>{ new PieceList(8), new PieceList(8) };
	rooks = std::vector<PieceList*>{ new PieceList(10), new PieceList(10) };
	bishops = std::vector<PieceList*>{ new PieceList(10), new PieceList(10) };
	queens = std::vector<PieceList*>{ new PieceList(9), new PieceList(9) };
	PieceList* emptyList = new PieceList(0);

	allPieceLists = std::vector<PieceList*>();
	allPieceLists.push_back(emptyList);
	allPieceLists.push_back(emptyList);
	allPieceLists.push_back(pawns[whiteIndex]);
	allPieceLists.push_back(knights[whiteIndex]);
	allPieceLists.push_back(emptyList);
	allPieceLists.push_back(bishops[whiteIndex]);
	allPieceLists.push_back(rooks[whiteIndex]);
	allPieceLists.push_back(queens[whiteIndex]);
	allPieceLists.push_back(emptyList);
	allPieceLists.push_back(emptyList);
	allPieceLists.push_back(pawns[blackIndex]);
	allPieceLists.push_back(knights[blackIndex]);
	allPieceLists.push_back(emptyList);
	allPieceLists.push_back(bishops[blackIndex]);
	allPieceLists.push_back(rooks[blackIndex]);
	allPieceLists.push_back(queens[blackIndex]);

	/*{
		emptyList,
		emptyList,
		pawns[whiteIndex],
		knights[whiteIndex],
		emptyList,
		bishops[whiteIndex],
		rooks[whiteIndex],
		queens[whiteIndex],
		emptyList,
		emptyList,
		pawns[blackIndex],
		knights[blackIndex],
		emptyList,
		bishops[blackIndex],
		rooks[blackIndex],
		queens[blackIndex],
	});*/
}

void Board::initStartPosition()
{
	loadPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

void Board::handleEnPassant(int moveTo, int opponentColorIndex)
{
	int epPawnSquare = moveTo + ((colorToMove == Piece::white) ? -8 : 8);
	//add pawn as captured type
	currentGameState |= (squares[epPawnSquare] << 8);
	squares[epPawnSquare] = Piece::none;
	pawns[opponentColorIndex]->removePieceAtSquare(epPawnSquare);
}

void Board::handleCastling(int moveTo)
{
	bool kingSide = moveTo == 6 || moveTo == 62;
	int castlingRookFromIndex = (kingSide) ? moveTo + 1 : moveTo - 2;
	int castlingRookToIndex = (kingSide) ? moveTo - 1 : moveTo + 1;

	squares[castlingRookFromIndex] = Piece::none;
	squares[castlingRookToIndex] = Piece::rook | colorToMove;

	rooks[colorToMoveIndex]->movePiece(castlingRookFromIndex, castlingRookToIndex);
}

Board& Board::operator=(const Board& other)
{
	squares = std::vector<int>(64);
	kingSquare = std::vector<int>(2);

	squares = other.squares;
	whiteToMove = other.whiteToMove;
	colorToMove = other.colorToMove;
	opponentColor = other.opponentColor;
	colorToMoveIndex = other.colorToMoveIndex;

	currentGameState = other.currentGameState;
	gameStateHistory = other.gameStateHistory;

	plyCount = other.plyCount;
	fiftyMoveCounter = other.fiftyMoveCounter;

	kingSquare = other.kingSquare;

	rooks = std::vector<PieceList*>();
	for (int i = 0; i < other.rooks.size(); i++) {
		rooks.push_back(new PieceList());
		*(rooks[i]) = *(other.rooks[i]);
	}
	bishops = std::vector<PieceList*>();
	for (int i = 0; i < other.bishops.size(); i++) {
		bishops.push_back(new PieceList());
		*(bishops[i]) = *(other.bishops[i]);
	};
	queens = std::vector<PieceList*>();
	for (int i = 0; i < other.queens.size(); i++) {
		queens.push_back(new PieceList());
		*(queens[i]) = *(other.queens[i]);
	};
	knights = std::vector<PieceList*>();
	for (int i = 0; i < other.knights.size(); i++) {
		knights.push_back(new PieceList());
		*(knights[i]) = *(other.knights[i]);
	};
	pawns = std::vector<PieceList*>();
	for (int i = 0; i < other.pawns.size(); i++) {
		pawns.push_back(new PieceList());
		*(pawns[i]) = *(other.pawns[i]);
	};

	allPieceLists = std::vector<PieceList*>();
	PieceList* emptyList = new PieceList(0);

	allPieceLists.push_back(emptyList);
	allPieceLists.push_back(emptyList);
	allPieceLists.push_back(pawns[whiteIndex]);
	allPieceLists.push_back(knights[whiteIndex]);
	allPieceLists.push_back(emptyList);
	allPieceLists.push_back(bishops[whiteIndex]);
	allPieceLists.push_back(rooks[whiteIndex]);
	allPieceLists.push_back(queens[whiteIndex]);
	allPieceLists.push_back(emptyList);
	allPieceLists.push_back(emptyList);
	allPieceLists.push_back(pawns[blackIndex]);
	allPieceLists.push_back(knights[blackIndex]);
	allPieceLists.push_back(emptyList);
	allPieceLists.push_back(bishops[blackIndex]);
	allPieceLists.push_back(rooks[blackIndex]);
	allPieceLists.push_back(queens[blackIndex]);

	return *this;
}