#include "FenUtility.h"
#include "Piece.h"
#include <ctype.h>
#include "Board.h"
#include <string>
#include <cctype>
std::unordered_map<char, int> FenUtility::pieceTypeFromSymbol = {
	{'k', Piece::king},
	{'p', Piece::pawn},
	{'n', Piece::knight},
	{'b', Piece::bishop},
	{'r', Piece::rook},
	{'q', Piece::queen}
};

std::string FenUtility::startPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

LoadedPositionInfo FenUtility::getPositionFromFen(std::string fen)
{
	LoadedPositionInfo res;
	std::vector<std::string> sections = splitString(fen, " ");

	int file = 0;
	int rank = 7;

	for (char symbol : sections[0])	{
		if (symbol == '/') {
			file = 0;
			rank--;
		}
		else {
			int value = isdigit(symbol);
			if (value) file += symbol - '0';
			else {
				int pieceColor = isupper(symbol) ? Piece::white : Piece::black;
				int pieceType = pieceTypeFromSymbol[tolower(symbol)];
				res.squares[rank * 8 + file] = pieceType | pieceColor;
				file++;
				
			}
		}
	}

	res.whiteToMove = (sections[1] == "w");
	std::string castlingRights = (sections.size() > 2) ? sections[2] : "KQkq";
	res.whiteCastleKingside = castlingRights.find("K") != std::string::npos;
	res.whiteCastleQueenside = castlingRights.find("Q") != std::string::npos;
	res.blackCastleKingside = castlingRights.find("k") != std::string::npos;
	res.blackCastleQueenside = castlingRights.find("q") != std::string::npos;

	size_t sectionLength = sections.size(); 
	if (sectionLength > 3) {
		char enPassantFileName = sections[3][0];
		if ('a' >= enPassantFileName && enPassantFileName >= 'h')
			res.epFile = enPassantFileName - 96;
		else
			res.epFile = -1;
	}
	if (sectionLength > 4)
		res.halfMoveCount = stoi(sections[4]);
	if (sectionLength > 5)
		res.plyCount = stoi(sections[5]);

	return res;
}

std::string FenUtility::currentFen(Board* board)
{
	std::string fen = "";
	for (int rank = 7; rank >= 0; rank--) {
		int numEmptyFiles = 0;
		for (int file = 0; file < 8; file++) {
			int i = rank * 8 + file;
			int piece = board->squares[i];
			if (piece != 0) {
				if (numEmptyFiles != 0) {
					fen += std::to_string(numEmptyFiles);
					numEmptyFiles = 0;
				}
				bool isBlack = Piece::isColor(piece, Piece::black);
				int pieceType = Piece::getPieceType(piece);
				char pieceChar = ' ';
				switch (pieceType) {
				case Piece::rook:
					pieceChar = 'R';
					break;
				case Piece::knight:
					pieceChar = 'N';
					break;
				case Piece::bishop:
					pieceChar = 'B';
					break;
				case Piece::queen:
					pieceChar = 'Q';
					break;
				case Piece::king:
					pieceChar = 'K';
					break;
				case Piece::pawn:
					pieceChar = 'P';
					break;
				}
				fen += (isBlack) ? tolower(pieceChar) : pieceChar;
			}
			else {
				numEmptyFiles++;
			}

		}
		if (numEmptyFiles != 0) {
			fen += std::to_string(numEmptyFiles);
		}
		if (rank != 0) {
			fen += '/';
		}
	}

	// Side to move
	fen += ' ';
	fen += (board->whiteToMove) ? 'w' : 'b';

	// Castling
	bool whiteKingside = (board->currentGameState & 1) == 1;
	bool whiteQueenside = (board->currentGameState >> 1 & 1) == 1;
	bool blackKingside = (board->currentGameState >> 2 & 1) == 1;
	bool blackQueenside = (board->currentGameState >> 3 & 1) == 1;
	fen += ' ';
	fen += (whiteKingside) ? "K" : "";
	fen += (whiteQueenside) ? "Q" : "";
	fen += (blackKingside) ? "k" : "";
	fen += (blackQueenside) ? "q" : "";
	fen += ((board->currentGameState & 15) == 0) ? "-" : "";

	// En-passant
	fen += ' ';
	int epFile = (int)(board->currentGameState >> 4) & 15;
	if (epFile == 0) {
		fen += '-';
	}
	else {
		std::string fileNames = "abcdefgh";
		std::string fileName = "" + fileNames[epFile - 1];
		int epRank = (board->whiteToMove) ? 6 : 3;
		fen += fileName + std::to_string(epRank);
	}

	// 50 move counter
	fen += ' ';
	fen += std::to_string(board->fiftyMoveCounter);

	// Full-move count (should be one at start, and increase after each move by black)
	fen += ' ';
	fen += std::to_string((board->plyCount / 2) + 1);

	return fen;
}

std::vector<std::string> FenUtility::splitString(std::string s, std::string delimeter = " ")
{
	std::vector<std::string> res;
	size_t last = 0;
	size_t next = 0;
	while ((next = s.find(delimeter, last)) != std::string::npos) { 
		res.push_back(s.substr(last, next - last));   
		last = next + 1; 
	}
	res.push_back(s.substr(last));
	return res;
}
