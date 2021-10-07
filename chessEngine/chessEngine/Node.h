#pragma once
#include <vector>

class Node
{
public:

	Node(class Board& board);
	~Node();

	class Move* move;
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
	Move getUntriedMove();
	Node* addChild(Move& move, Board& board);

private:

	Node(Board& board, Move& move, Node* parent);
	double uctScore;
};

