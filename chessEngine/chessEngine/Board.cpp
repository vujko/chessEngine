                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         #include "Board.h"
#include "FenUtility.h"
#include "Move.h"

Board::Board()
{
}


std::wostream& operator<<(std::wostream& out, Board& b)
{
	out << "\n     ";
	for (unsigned int i = 0; i < 8; i++) {
		out << (char)(97 + i) << "   ";
	}
	out << "\n   ";
	for (unsigned int i = 0; i < 8; i++) {
		out << "+---";
	}
	out << "+ \n";
	for (unsigned int i = 0; i < 8; i++) {
		out << " " << i << " | ";
		for (unsigned int j = 0; j < 8; j++) {
			out << b.pieceSymbols.at(b.squares[i][j]) << " | ";
		}
		out << i << "\n   ";
		for (unsigned int j = 0; j < 8; j++) {
			out << "+---";
		}
		out << "+ \n";
	}
	out << "     ";
	for (unsigned int i = 0; i < 8; i++) {
		out << (char)(97 + i) << "   ";
	}
	out << "\n";
	return out;
}



void Board::initStartPosition()
{
	loadPosition(FenUtility::startPosition);
}

void Board::loadPosition(std::string fen)
{
	LoadedPositionInfo pos = FenUtility::getPositionFromFen(fen);
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			int piece = pos.squares[i][j];
			squares[i][j] = piece;
		}
	}
	whiteCastleKingside = pos.whiteCastleKingside;
	whiteCastleQueenside = pos.whiteCastleQueenside;
	blackCastleKingside = pos.blackCastleKingside;
	blackCastleQueenside = pos.blackCastleQueenside;
	playCount = pos.playCount;
	halfMoveCount = pos.halfMoveCount;
	epFile = pos.epFile;

	whiteToMove = pos.whiteToMove;
	colorToMove = (whiteToMove) ? Piece::white : Piece::black;
	opponentColor = (whiteToMove) ? Piece::black : Piece::white;

	int whiteKingPosI = pos.whiteKingSquare / 8;
	int whiteKingPosJ = pos.whiteKingSquare % 8;
	setWhiteKingPos(whiteKingPosI, whiteKingPosJ);
	int blackKingPosI = pos.blackKingSquare / 8;
	int blackKingPosJ = pos.blackKingSquare % 8;
	setBlackKingPos(blackKingPosI, blackKingPosJ);
	
	whiteKingCheck = isSquareAttacked(whiteKingPosI, whiteKingPosJ, Piece::white);
	blackKingCheck = isSquareAttacked(blackKingPosI, blackKingPosJ, Piece::black);

	currentGameState = 0;
	currentGameState |= ((epFile + 1) << 4);
	currentGameState |= (whiteCastleKingside);
	currentGameState |= (whiteCastleQueenside << 1);
	currentGameState |= (blackCastleKingside << 2);
	currentGameState |= (blackCastleQueenside << 3);
	currentGameState |= (whiteKingCheck << 14);
	currentGameState |= (blackKingCheck << 15);
	currentGameState |= (halfMoveCount << 16);

	gameStateHistory.push(currentGameState);

}

void Board::makeMove(Move& move)
{
	int startI = move.getStartI();
	int startJ = move.getStartJ();
	int targetI = move.getTargetI();
	int targetJ = move.getTargetJ();

	int piece = squares[startI][startJ];
	int pieceType = Piece::getPieceType(piece);
	int color = Piece::getPieceColor(piece);

	int capturedPieceType = Piece::getPieceType(squares[targetI][targetJ]);
	currentGameState = 0;
	currentGameState |= (capturedPieceType << 8);

	squares[targetI][targetJ] = piece;
	squares[startI][startJ] = Piece::none;

	//update castle flags if rook moved
	if (pieceType == Piece::rook) {
		bool kingSide = (startJ) == 7;
		if (color == Piece::white) {
			if (kingSide) whiteCastleKingside = false;
			else whiteCastleQueenside = false;
		}
		else {
			if (kingSide) blackCastleKingside = false;
			else blackCastleQueenside = false;
		}
	}

	//update castle flags and position if king has moved
	if (pieceType == Piece::king) {
		if (color == Piece::white && (whiteCastleKingside || whiteCastleQueenside)) {
			whiteCastleKingside = false;
			whiteCastleQueenside = false;
		}
		else if (color == Piece::black && (blackCastleKingside || blackCastleQueenside)) {
			blackCastleKingside = false;
			blackCastleQueenside = false;
		}

		if (color == Piece::white) setWhiteKingPos(targetI, targetJ);
		else setBlackKingPos(targetI, targetJ);

	}

	//reset it every move, set it only when a pawn moves two spaces forward
	epFile = -1;
	switch (move.getMoveFlag()) {
	case MoveFlag::castling:
		handleCastling(targetI, targetJ, color);
		break;

	case MoveFlag::promoteToQueen:
		squares[targetI][targetJ] = Piece::queen | color;
		break;

	case MoveFlag::promoteToBishop:
		squares[targetI][targetJ] = Piece::bishop | color;
		break;

	case MoveFlag::promoteToKnight:
		squares[targetI][targetJ] = Piece::knight | color;
		break;

	case MoveFlag::promoteToRook:
		squares[targetI][targetJ] = Piece::rook | color;
		break;

	case MoveFlag::enPassantCapture:
		handleEnPassantCapture(targetI, targetJ, color);
		break;

	case MoveFlag::pawnTwoForward:
		epFile = targetJ;

		break;
	}
	//incrementing by 1 so the range will be 1-8
	currentGameState |= ((epFile + 1) << 4);
	whiteToMove = !whiteToMove;
	colorToMove = (whiteToMove) ? Piece::white : Piece::black;
	opponentColor = (whiteToMove) ? Piece::black : Piece::white;
	playCount++;
	halfMoveCount++;

	if (pieceType == Piece::pawn || capturedPieceType != Piece::none) {
		halfMoveCount = 0;
	}

	whiteKingCheck = isKingInCheck(move, false, Piece::white);
	blackKingCheck = isKingInCheck(move, false, Piece::black);

	currentGameState |= (whiteCastleKingside);
	currentGameState |= (whiteCastleQueenside << 1);
	currentGameState |= (blackCastleKingside << 2);
	currentGameState |= (blackCastleQueenside << 3);
	currentGameState |= (whiteKingCheck << 14);
	currentGameState |= (blackKingCheck << 15);
	currentGameState |= (halfMoveCount << 16);
	gameStateHistory.push(currentGameState);
}

void Board::unmakeMove(Move& move)
{
	int startI = move.getStartI();
	int startJ = move.getStartJ();
	int targetI = move.getTargetI();
	int targetJ = move.getTargetJ();

	whiteToMove = !whiteToMove;
	colorToMove = (whiteToMove) ? Piece::white : Piece::black;
	opponentColor = (whiteToMove) ? Piece::black : Piece::white;

	bool isPromotion = move.isPromotion();
	bool isEnPassant = move.getMoveFlag() == enPassantCapture;
	if (isEnPassant) {
		int brea = 4;
	}

	int capturedPieceType = (currentGameState >> 8) & 0b111111;
	int capturedPiece = (capturedPieceType == 0) ? 0 : capturedPieceType | opponentColor;

	int targetSquarePieceType = Piece::getPieceType(squares[targetI][targetJ]);
	int movedPieceType = (isPromotion) ? Piece::pawn : targetSquarePieceType;

	if (movedPieceType == Piece::king) {
		if (colorToMove == Piece::white) setWhiteKingPos(startI, startJ);
		else setBlackKingPos(startI, startJ);
	}

	squares[startI][startJ] = movedPieceType | colorToMove;
	squares[targetI][targetJ] = capturedPiece;

	if (isEnPassant) {
		int offset = (colorToMove == Piece::white) ? 1 : -1;
		squares[targetI + offset][targetJ] = Piece::pawn | opponentColor;
	}
	else if (move.getMoveFlag() == castling) {
		bool kingSide = targetJ == 6;
		int rookStartJ = (kingSide) ? 7 : 0;
		int rookTargetJ = (kingSide) ? 5 : 3;
		squares[targetI][rookStartJ] = Piece::rook | colorToMove;
		squares[targetI][rookTargetJ] = Piece::none;
	}

	gameStateHistory.pop();
	currentGameState = gameStateHistory.top();

	halfMoveCount = (currentGameState & 0b11111111111111110000000000000000) >> 16;
	epFile = ((currentGameState >> 4) & 0b1111) - 1;
	whiteCastleKingside = currentGameState & 0b1;
	whiteCastleQueenside = (currentGameState >> 1) & 0b1;
	blackCastleKingside = (currentGameState >> 2) & 0b1;
	blackCastleQueenside = (currentGameState >> 3) & 0b1;
	whiteKingCheck = (currentGameState >> 14) & 0b1;
	blackKingCheck = (currentGameState >> 15) & 0b1;


	playCount--;
}

void Board::setCurrentGameState(unsigned int gameState)
{
}

//remove a pawn behind the pawn which made the capture
void Board::handleEnPassantCapture(int targetI, int targetJ, int color) 
{
	int offset = (color == Piece::white) ? 1 : -1;
	//currentGameState |= (Piece::pawn << 8);
	squares[targetI + offset][targetJ] = Piece::none;
}

void Board::handleCastling(int kingTargetI, int kingTargetJ, int color)
{
	bool kingSide = kingTargetJ == 6;
	int rookStartJ = (kingSide) ? 7 : 0;
	int rookTargetJ = (kingSide) ? 5 : 3;
	squares[kingTargetI][rookStartJ] = Piece::none;
	squares[kingTargetI][rookTargetJ] = Piece::rook | color;
}

int Board::getWhiteKingPosI()
{
	int position = kingPositions & whiteKingPosMask;
	return position / 8;
}

int Board::getWhiteKingPosJ()
{
	int position = kingPositions & whiteKingPosMask;
	return position % 8;
}

int Board::getBlackKingPosI()
{
	int position = (kingPositions & blackKingPosMask) >> 6;
	return position / 8;
}

int Board::getBlackKingPosJ()
{
	int position = (kingPositions & blackKingPosMask) >> 6;
	return position % 8;
}

void Board::setWhiteKingPos(int i, int j)
{
	int whiteKingPos = i * 8 + j;
	kingPositions = (kingPositions & blackKingPosMask) | whiteKingPos;
}

void Board::setBlackKingPos(int i, int j)
{
	int blackKingPos = i * 8 + j;
	kingPositions = (kingPositions & whiteKingPosMask) | blackKingPos << 6;
}
//filter - wheter or not this function is used for filtering pseudo legal moves
//move - last move made on board
bool Board::isKingInCheck(Move& move, bool filter, int kingColor)
{
	//assuming the move is already made
	//is there a straight line or diagonal
	int pieceI = move.getTargetI();
	int pieceJ = move.getTargetJ();
	int piece = squares[pieceI][pieceJ];
	int pieceColor = Piece::getPieceColor(piece);

	if (Piece::getPieceType(piece) == Piece::king && pieceColor == kingColor)
		return isSquareAttacked(pieceI, pieceJ, pieceColor);

	//int kingColor = (Piece::white == pieceColor) ? Piece::black : Piece::white;
	//get opposing king position
	int kingI = (kingColor == Piece::white) ? getWhiteKingPosI() : getBlackKingPosI();
	int kingJ = (kingColor == Piece::white) ? getWhiteKingPosJ() : getBlackKingPosJ();

	bool kingAlreadyInCheck = (kingColor == Piece::white) ? whiteKingCheck : blackKingCheck;
	if (kingAlreadyInCheck)
		if(isSquareAttacked(kingI, kingJ, pieceColor)) return true;
	

	//when filter is true theres no need to check if the last moved piece can check its own king
	if (pieceColor != kingColor) {
		//check if the piece can give check
		std::vector<Move> moveList = Piece::getMoves(this, pieceI, pieceJ);
		for (Move m : moveList)
			if (m.getTargetI() == kingI && m.getTargetJ() == kingJ)
				return true;
	}
	
	//check if discovery check	
	return isDiscoveryCheck(move, kingI, kingJ, kingColor);
}

//find a line from king to the moved piece destination if exists
//check if along the line is a piece that can give that check (rook, bishop, queen)
bool Board::isDiscoveryCheck(Move& move, int kingI, int kingJ, int kingColor) {
	int startPosI = move.getStartI();
	int startPosJ = move.getStartJ();
	//find the direction if exists out of 8 possible
	//from king to the startPos
	Direction dir = findDirection(kingI, kingJ, startPosI, startPosJ);
	if (dir == Direction::NONE) return false;

	int x = kingI, y = kingJ;
	for (int step = 1; step < 8; step++, x = kingI, y = kingJ) {
		switch (dir) {
		case Direction::WEST:	y -= step; break; // west
		case Direction::SOUTH:	x += step; break; // south
		case Direction::EAST:	y += step; break; // east
		case Direction::NORTH:	x -= step; break; // north
		case Direction::SOUTH_EAST: x += step; y += step; break; //south-east
		case Direction::SOUTH_WEST: x += step; y -= step; break; //south-west
		case Direction::NORTH_WEST: x -= step; y -= step; break; //north-west
		case Direction::NORTH_EAST: x -= step; y += step; break; //north-east
		}

		//on board
		if (x >= 0 && x < 8 && y >= 0 && y < 8) {
			//square occupied
			int piece = squares[x][y];
			if (piece) {
				//by opponent piece
				if (Piece::getPieceColor(piece) != kingColor) {
					int pieceType = Piece::getPieceType(piece);

					if (pieceType == Piece::queen) return true;

					if (dir >= 0 && dir <= 3 && pieceType == Piece::rook) return true;

					if (dir >= 4 && dir <= 7 && pieceType == Piece::bishop) return true;
				}
				//cannot skip over pieces
				break;
			}
		}
		else
			//any other move in the same direction is also not on board
			break;
	}
	return false;
}



Direction Board::findDirection(int kingI, int kingJ, int startPosI, int startPosJ) {
	int iDifference = kingI - startPosI;
	int jDifference = kingJ - startPosJ;
	if		(kingI > startPosI && iDifference == jDifference)		return Direction::NORTH_WEST;
	else if (kingI > startPosI && kingJ == startPosJ)				return Direction::NORTH;
	else if (iDifference > 0 && iDifference == jDifference * -1)	return Direction::NORTH_EAST;
	else if (kingI == startPosI && kingJ > startPosJ)				return Direction::WEST;
	else if (kingI == startPosI && kingJ < startPosJ)				return Direction::EAST;
	else if (iDifference < 0 && iDifference == jDifference * -1)	return Direction::SOUTH_WEST;
	else if (kingI < startPosI && kingJ == startPosJ)				return Direction::SOUTH;
	else if (kingI < startPosI && iDifference == jDifference) 		return Direction::SOUTH_EAST;
	return Direction::NONE;
	
}

std::vector<Move> Board::getAllMoves()
{
	std::vector<Move> allMoves;
	int captures = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (!squares[i][j] || Piece::getPieceColor(squares[i][j]) != colorToMove)
				continue;

			std::vector<Move> moves = Piece::getMoves(this, i, j);
			/*std::wcout << "(" << i << " , " << j << " ) : " << moves.size() << std::endl;*/
			for (Move m : moves) {
				//Board copy(*this);
				makeMove(m);
				bool kingInCheck = (colorToMove == Piece::black) ? whiteKingCheck : blackKingCheck;
				if (!kingInCheck)
					allMoves.push_back(m);
				unmakeMove(m);
			}
		}
	}
	//std::wcout << "castles: " << captures << std::endl;
	return allMoves;
}

std::vector<Move> Board::getAllMovesCopy()
{
	std::vector<Move> allMoves;
	int captures = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (!squares[i][j] || Piece::getPieceColor(squares[i][j]) != colorToMove)
				continue;

			std::vector<Move> moves = Piece::getMoves(this, i, j);

			/*std::wcout << "(" << i << " , " << j << " ) : " << moves.size() << std::endl;*/
			for (Move m : moves) {

				Board copy(*this);
				copy.makeMove(m);
				bool kingInCheck = (colorToMove == Piece::white) ? copy.whiteKingCheck : copy.blackKingCheck;
				if (!kingInCheck)
					allMoves.push_back(m);
			}
		}
	}
	//std::wcout << "castles: " << captures << std::endl;
	return allMoves;
}

bool Board::isSquareAttacked(int kingI, int kingJ, int kingColor)
{
	for (int dir = 0; dir < 8; dir++) {
		int x = kingI, y = kingJ;
		for (int step = 1; step < 8; step++, x = kingI, y = kingJ) {
			switch (dir) {
			case 0:	y -= step; break; // west
			case 1:	x += step; break; // south
			case 2:	y += step; break; // east
			case 3:	x -= step; break; // north
			case 4: x += step; y += step; break; //south-east
			case 5: x += step; y -= step; break; //south-west
			case 6: x -= step; y -= step; break; //north-west
			case 7: x -= step; y += step; break; //north-east
			}

			//on board
			if (x >= 0 && x < 8 && y >= 0 && y < 8) {
				//square occupied
				int piece = squares[x][y];
				if (piece) {
					//by opponent piece
					if (Piece::getPieceColor(piece) != kingColor) {

						int pieceType = Piece::getPieceType(piece);

						if (pieceType == Piece::queen) return true;

						if (dir >= 0 && dir <= 3 && pieceType == Piece::rook) return true;

						if (dir >= 4 && dir <= 7 && pieceType == Piece::bishop) return true;

						if (pieceType == Piece::pawn && step == 1) {
							bool pawnAttackingCurrentSquare = (kingColor == Piece::white) ? (dir == 6 || dir == 7) : (dir == 4 || dir == 5);
							if (pawnAttackingCurrentSquare) return true;
						}

					}
					//cannot skip over pieces
					break;
				}
			}
			else
				//any other move in the same direction is also not on board
				break;
		}

		//check for knight attacks
		x = kingI, y = kingJ;
		switch (dir) {
		case 0: x += 1; y += 2; break;
		case 1: x -= 1; y += 2; break;
		case 2: x += 2; y += 1; break;
		case 3: x += 2; y -= 1; break;
		case 4: x -= 2; y += 1; break;
		case 5: x -= 2; y -= 1; break;
		case 6: x += 1; y -= 2; break;
		case 7: x -= 1; y -= 2; break;
		}

		//on board
		if (x >= 0 && x < 8 && y >= 0 && y < 8) {
			//square occupied
			int piece = squares[x][y];
			if (piece) {
				//by opponent piece
				if (Piece::getPieceColor(piece) != kingColor) {

					int pieceType = Piece::getPieceType(piece);

					if (pieceType == Piece::knight) return true;
				}
			}
		}
		
	}
	return false;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           