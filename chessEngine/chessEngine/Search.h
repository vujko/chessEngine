#pragma once
#include <memory>
#include <random>
struct ComputeOptions
{
	int number_of_threads;
	int max_iterations;
	double max_time;
	bool verbose;
	double startTime;
	int depth; //needed for uci protocol not used

	ComputeOptions() :
		number_of_threads(1),
		max_iterations(10000),
		max_time(-1.0), // default is no time limit.
		verbose(false),
		startTime(0.0),
		depth(0)
	{ }
};

class Node;

class Search
{
private:
	std::unique_ptr<Node> computeTree(class Game& rootGame, ComputeOptions& options, std::mt19937_64::result_type initial_seed);

public:
	class Move computeMove(Game& rootGame, ComputeOptions options);
};

