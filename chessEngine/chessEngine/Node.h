#pragma once
#include <vector>
#include <string>
#include "Move.h"
#include <random>
class Node
{
public:

	Node(class Game& game);
	~Node();

	class Move move;
	Node* parent;

	double wins;
	int visits;


	bool whiteToMove;

	std::vector<Move> moves;
	std::vector<Node*> children;

	Node* bestChild();
	Node* selectRandomChild(std::mt19937_64* randomEngine);
	Node* selectChildUct();
	void update(double result);
	bool hasUntriedMoves();
	bool hasChildren();
	Move getUntriedMove(std::mt19937_64* randomEngine);
	Node* addChild(Move& move, Game& game);

	std::string to_string();
	std::string tree_to_string(int max_depth, int indent);
	std::string indent_string(int indent);


private:

	Node(Game& game, Move& move, Node* parent);
	double uctScore;
	int heuristicScore;

};

