#pragma once
#include <memory>
struct ComputeOptions
{
	int number_of_threads;
	int max_iterations;
	double max_time;
	bool verbose;

	ComputeOptions() :
		number_of_threads(8),
		max_iterations(10000),
		max_time(-1.0), // default is no time limit.
		verbose(false)
	{ }
};

class Node;

class Search
{
private:
	std::unique_ptr<Node> computeTree(class Board& rootBoard, const ComputeOptions& options);

public:
	class Move computeMove(Board& rootBoard, const ComputeOptions options);
};

