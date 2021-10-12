#pragma once
#include <vector>
#include <string>
#include "Move.h"
#include <random>
class Node
{
public:

	Node(class Board& board);
	~Node();

	class Move move;
	Node* parent;

	double wins;
	int visits;

	bool whiteToMove;

	std::vector<Move> moves;
	std::vector<Node*> children;

	Node* bestChild();
	Node* selectChildUct();
	void update(double result);
	bool hasUntriedMoves();
	bool hasChildren();
	Move getUntriedMove(std::mt19937_64* randomEngine);
	Node* addChild(Move& move, Board& board);

	std::string to_string();
	std::string tree_to_string(int max_depth, int indent);
	std::string indent_string(int indent);


private:

	Node(Board& board, Move& move, Node* parent);
	double uctScore;
};

