#pragma once
#include <vector>
#include <random>
class Game
{
public:
	enum Result { playing, whiteIsMated, blackIsMated, draw };

	Result gameResult;
	class Board* board;
	//std::vector<class Move> possibleMoves;
	//bool whiteToMove;

	Game();
	Game(const Game& other);
	Game(Board* board);
	void doMove(class Move& m);
	void doRandomMove(std::mt19937_64* engine);
	bool isEndState();
	double getResult(bool whitePlayer);
	Result getGameState();

};

