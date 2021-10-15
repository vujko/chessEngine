#pragma once
#include <memory>
#include <random>
struct ComputeOptions
{
	int number_of_threads;
	int max_iterations;
	double max_time;
	bool verbose;

	ComputeOptions() :
		number_of_threads(1),
		max_iterations(100),
		max_time(-1.0), // default is no time limit.
		verbose(true)
	{ }
};

class Node;

class Search
{
private:
	std::unique_ptr<Node> computeTree(class Game& rootGame, const ComputeOptions& options, std::mt19937_64::result_type initial_seed);

public:
	class Move computeMove(Game& rootGame, const ComputeOptions options);
};

