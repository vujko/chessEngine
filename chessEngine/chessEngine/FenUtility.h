#pragma once
#include <unordered_map>

struct LoadedPositionInfo
{
	std::vector<int> squares = std::vector<int>(64);
	bool whiteCastleKingside;
	bool whiteCastleQueenside;
	bool blackCastleKingside;
	bool blackCastleQueenside;
	int epFile;
	bool whiteToMove;
	int plyCount;
	int halfMoveCount;
};

static class FenUtility
{
public:
	
	static std::unordered_map<char, int> pieceTypeFromSymbol;
	static std::string startPosition;

	static LoadedPositionInfo getPositionFromFen(std::string fen);
	static std::string currentFen(class Board* board);

	static std::vector<std::string> splitString(std::string line, std::string delimeter);

};

