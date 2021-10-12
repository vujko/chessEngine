#include "Search.h"
#include "Node.h"
#include "Board.h"
#include "Move.h"
#include <map>
#include "Evaluation.h"
#include <omp.h>
#include <thread>
#include <vector>
#include <future>
#include "Perft.h"
//using this struct for making maps where Move is the key value
struct MoveCompare
{
	bool operator() (const Move& lhs, const Move& rhs) const
	{
		return lhs.moveValue < rhs.moveValue;
	}
};

std::unique_ptr<Node> Search::computeTree(Board& rootBoard, const ComputeOptions& options, std::mt19937_64::result_type initialSeed)
{
	std::random_device randDev;
	std::mt19937_64 randomEngine(randDev());

	std::unique_ptr<Node> root = std::unique_ptr<Node>(new Node(rootBoard));

	double start_time = ::omp_get_wtime();
	double print_time = start_time;
	//Board board;
	for (int iter = 1; iter <= options.max_iterations || options.max_iterations < 0; ++iter) {
		Node* node = root.get();
		Board board = rootBoard;

		//Select a path through the tree to a leaf node
		while (!node->hasUntriedMoves() && node->hasChildren()) {
			node = node->selectChildUct();
			board.makeMove((node->move));
		}

		//if we are not at a final state, expand the tree
		//with a new node and move there
		if (node->hasUntriedMoves()) {
			Move move = node->getUntriedMove(&randomEngine);
			board.makeMove(move);
			node = node->addChild(move, board);
		}

		//we now play randomly until the game ends
		std::vector<Move> moves = board.moves;
		do {
			board.doRandomMove(&randomEngine);
			moves = board.generateMoves();
			//std::wcout << board << std::endl;
		} while (!board.isEndState());

		//we have reached a final state. Backpropagate the result
		//up the tree 
		while (node != nullptr) {
			node->update(board.getResult(node->whiteToMove));
			node = node->parent;
		}
		if (options.verbose || options.max_time >= 0) {
			double time = ::omp_get_wtime();
			if (options.verbose && (time - print_time >= 1.0 || iter == options.max_iterations)) {
				std::cerr << iter << " games played (" << double(iter) / (time - start_time) << " / second)." << std::endl;
				print_time = time;
			}

			if (time - start_time >= options.max_time) {
				break;
			}
		}
	}
	return root;
}

Move Search::computeMove(Board& rootBoard, const ComputeOptions options)
{
	std::vector<Move> moves = rootBoard.generateMoves();
	if (moves.size() == 1) 
		return moves[0];

	double start_time = ::omp_get_wtime();

	// Start all jobs to compute trees.
	std::vector<std::future<std::unique_ptr<Node>>> root_futures;
	ComputeOptions job_options = options;
	job_options.verbose = false;
	for (int t = 0; t < options.number_of_threads; ++t) {
		auto func = [this, t, &rootBoard, &job_options]() -> std::unique_ptr<Node>
		{
			return computeTree(rootBoard, job_options, 1012411 * t + 12515);
		};

		root_futures.push_back(std::async(std::launch::async, func));
	}

	// Collect the results.
	std::vector<std::unique_ptr<Node>> roots;
	for (int t = 0; t < options.number_of_threads; ++t) {
		roots.push_back(std::move(root_futures[t].get()));
	}

	//std::unique_ptr<Node> root = computeTree(rootBoard, options, 12515);
	//std::wcout << root->tree_to_string(100, 2).c_str() << std::endl;
	std::map<Move, int, MoveCompare> visits;
	std::map<Move, double, MoveCompare> wins;
	long long gamesPlayed = 0;
	for (int t = 0; t < options.number_of_threads; ++t) {
		auto root = roots[t].get();
		gamesPlayed += root->visits;
		for (auto child = root->children.cbegin(); child != root->children.cend(); ++child) {
			Move m = ((*child)->move);

			visits[m] += (*child)->visits;
			wins[m] += (*child)->wins;
		}
	}
	//find node with highest score
	double bestScore = -1;
	Move bestMove = Move(0);
	for (const auto& itr : visits) {
		Move move = itr.first;
		double v = itr.second;
		double w = wins[move];
		if (v > bestScore) {
			bestMove = move;
			bestScore = v;
		}
		if (options.verbose) {
			std::cerr << "Move: " << moveName(move).c_str()
				<< " (" << int(100.0 * v / double(gamesPlayed) + 0.5) << "% visits)"
				<< " (" << int(100.0 * w / v + 0.5) << "% wins)" << std::endl;
		}
	}

	if (options.verbose) {
		auto best_wins = wins[bestMove];
		auto best_visits = visits[bestMove];
		std::cerr << "----" << std::endl;
		std::cerr << "Best: " << moveName(bestMove)
		     << " (" << 100.0 * best_visits / double(gamesPlayed) << "% visits)"
		     << " (" << 100.0 * best_wins / best_visits << "% wins)" << std::endl;
	}

	if (options.verbose) {
		double time = ::omp_get_wtime();
		std::cerr << gamesPlayed << " games played in " << double(time - start_time) << " s. "
		          << "(" << double(gamesPlayed) / (time - start_time) << " / second, "
		          << options.number_of_threads << " parallel jobs)." << std::endl;
	}


	return bestMove;
}


