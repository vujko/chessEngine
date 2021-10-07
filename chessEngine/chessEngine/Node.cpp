#include "Node.h"
#include "Move.h"
#include "Board.h"
#include <algorithm>

Node::Node(Board& board) : 
	parent(nullptr),
	move(nullptr),
	wins(0),
	visits(0),
	uctScore(0),
	whiteToMove(board.whiteToMove)
{
	moves = board.generateMoves();
}

Node* Node::bestChild()
{
	return *std::max_element(children.begin(), children.end(),
		[](Node* a, Node* b) { return a->visits < b->visits; });
}

Node* Node::selectChildUct()
{
	for (Node* child : children) {
		child->uctScore = double(child->wins) / double(child->visits) +
			std::sqrt(2.0 * std::log(double(this->visits)) / child->visits);
	}
	return *std::max_element(children.begin(), children.end(),
		[](Node* a, Node* b) {return a->uctScore < b->uctScore; });
}

void Node::update(double result)
{
	visits++;
	wins += result;
}

Node* Node::addChild(Move& move, Board& board)
{
	Node* node = new Node(board, move, this);
	children.push_back(node);

	//remove move from list
	auto itr = moves.begin();
	for (; itr != moves.end() && *itr != move; ++itr);
	std::iter_swap(itr, moves.end() - 1);
	moves.resize(moves.size() - 1);
	return node;
}

Move Node::getUntriedMove()
{
	return moves[rand() % moves.size()];
}


Node::Node(Board& board, Move& move, Node* parent) :
	move(&move),
	parent(parent),
	wins(0),
	visits(0),
	uctScore(0),
	whiteToMove(board.whiteToMove)
{
	moves = board.generateMoves();
}

Node::~Node()
{
	for (Node* child : children)
		delete child;
}

bool Node::hasUntriedMoves() {
	return !moves.empty();
}

bool Node::hasChildren(){
	return children.empty();
}
