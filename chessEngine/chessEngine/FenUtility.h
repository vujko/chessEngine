#pragma once
#include <unordered_map>

struct LoadedPositionInfo
{
	int squares[8][8] = { 0 };
	bool whiteCastleKingside;
	bool whiteCastleQueenside;
	bool blackCastleKingside;
	bool blackCastleQueenside;
	int epFile;
	bool whiteToMove;
	int playCount;
	int halfMoveCount;
	int whiteKingSquare;
	int blackKingSquare;
};

static class FenUtility
{
public:
	
	static std::unordered_map<char, int> pieceTypeFromSymbol;
	static std::string startPosition;

	static LoadedPositionInfo getPositionFromFen(std::string fen);

	static std::vector<std::string> splitString(std::string line, std::string delimeter);

};

