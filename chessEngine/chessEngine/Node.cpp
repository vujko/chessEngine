#include "Node.h"
#include "Move.h"
#include "Board.h"
#include <algorithm>
#include <sstream>
#include "Perft.h"
#include <random>

Node::Node(Board& board) : 
	parent(nullptr),
	wins(0),
	visits(0),
	uctScore(0),
	whiteToMove(board.whiteToMove),
	move(0)
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

Move Node::getUntriedMove(std::mt19937_64* randomEngine)
{
	std::uniform_int_distribution<std::size_t> moves_distribution(0, moves.size() - 1);
	return moves[moves_distribution(*randomEngine)];
}


Node::Node(Board& board, Move& move, Node* parent) :
	move(move),
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
	return !children.empty();
}

std::string Node::to_string()
{
	std::stringstream sout;
	sout << "["
		<< "P" << 2 - whiteToMove << " "
		<< "M:" << moveName(move) << " "
		<< "W/V: " << wins << "/" << visits << " "
		<< "U: " << moves.size() << "]\n";
	return sout.str();
}

std::string Node::tree_to_string(int max_depth, int indent)
{
	if (indent >= max_depth) {
		return "";
	}

	std::string s = indent_string(indent) + to_string();
	for (auto child : children) {
		s += child->tree_to_string(max_depth, indent + 1);
	}
	return s;
}

std::string Node::indent_string(int indent)
{
	std::string s = "";
	for (int i = 1; i <= indent; ++i) {
		s += "| ";
	}
	return s;
}