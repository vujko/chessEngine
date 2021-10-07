#include "Search.h"
#include "Node.h"
#include "Board.h"
#include "Move.h"
#include <map>

std::unique_ptr<Node> Search::computeTree(Board& rootBoard, const ComputeOptions& options)
{
	std::unique_ptr<Node> root = std::unique_ptr<Node>(new Node(rootBoard));

	//Board board;
	for (int iter = 1; iter <= options.max_iterations || options.max_iterations < 0; ++iter) {
		Node* node = root.get();
		Board board = rootBoard;

		//Select a path through the tree to a leaf node
		while (!node->hasUntriedMoves() && node->hasChildren()) {
			node = node->selectChildUct();
			board.makeMove(*(node->move));
		}

		//if we are not at a final state, expand the tree
		//with a new node and move there
		if (node->hasUntriedMoves()) {
			Move move = node->getUntriedMove();
			board.makeMove(move);
			node = node->addChild(move, board);
		}

		//we now play randomly until the game ends
		std::vector<Move> moves = board.generateMoves();
		do {
			board.makeMove(moves[rand() % moves.size()]);
			moves = board.generateMoves();
		} while (!moves.empty());

		//we have reached a final state. Backpropagate the result
		//up the tree 
		while (node != nullptr) {
			node->update(board.getResult(node->whiteToMove));
			node = node->parent;
		}
	}
	return root;
}

Move Search::computeMove(Board& rootBoard, const ComputeOptions options)
{
	std::vector<Move> moves = rootBoard.generateMoves();
	if (moves.size() == 1) 
		return moves[0];

	std::unique_ptr<Node> root = computeTree(rootBoard, options);

	std::map<Move, int> visits;
	std::map<Move, double> wins;
	long long gamesPlayer = root->visits;
	for (auto child = root->children.cbegin(); child != root->children.cend(); ++child) {
		Move* m = ((*child)->move);
		visits[*m] += (*child)->visits;
		wins[*m] += (*child)->wins;
	}

	//find node with highest score
	double bestScore = -1;
	Move bestMove = Move(0);
	for (auto itr : visits) {
		Move move = itr.first;
		double v = itr.second;
		double w = wins[move];

	}


}
