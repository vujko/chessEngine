#include "FenUtility.h"
#include "Piece.h"
#include <ctype.h>
#include <string>
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
	int rank = 0;

	res.whiteKingSquare = 0;
	res.blackKingSquare = 0;
	for (char symbol : sections[0])	{
		if (symbol == '/') {
			file = 0;
			rank++;
		}
		else {
			int value = isdigit(symbol);
			if (value) file += symbol - '0';
			else {
				int pieceColor = isupper(symbol) ? Piece::white : Piece::black;
				int pieceType = pieceTypeFromSymbol[tolower(symbol)];
				res.squares[rank][file] = pieceType | pieceColor;
				if (pieceType == Piece::king) {
					if (pieceColor == Piece::white) res.whiteKingSquare = rank * 8 + file;
					else res.blackKingSquare = rank * 8 + file;
				}
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
		res.playCount = stoi(sections[5]);

	return res;
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
