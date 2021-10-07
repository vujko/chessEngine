#include "Perft.h"
#include "Board.h"
#include "Move.h"
#include "FenUtility.h"
#include <unordered_map>
#include <string>
#include <chrono>

Board* b;
std::unordered_map<std::string, int>* perftDivideResults;
//starting pos
//std::string expectedResults = "a2a3: 8457\nb2b3: 9345\nc2c3: 9272\nd2d3: 11959\ne2e3: 13134\nf2f3: 8457\ng2g3: 9345\nh2h3: 8457\na2a4: 9329\nb2b4: 9332\nc2c4: 9744\nd2d4: 12435\ne2e4: 13160\nf2f4: 8929\ng2g4: 9328\nh2h4: 9329\nb1a3: 8885\nb1c3: 9755\ng1f3: 9748\ng1h3: 8881";

////pos 2 for depth 2
//std::string expectedResults = "a2a3: 44\nb2b3: 42\ng2g3: 42\nd5d6: 41\na2a4: 44\ng2g4: 42\ng2h3: 43\nd5e6: 46\nc3b1: 42\nc3d1: 42\nc3a4: 42\nc3b5: 39\ne5d3: 43\ne5c4: 42\ne5g4: 44\ne5c6: 41\ne5g6: 42\ne5d7: 45\ne5f7: 44\nd2c1: 43\nd2e3: 43\nd2f4: 43\nd2g5: 42\nd2h6: 41\ne2d1: 44\ne2f1: 44\ne2d3: 42\ne2c4: 41\ne2b5: 39\ne2a6: 36\na1b1: 43\na1c1: 43\na1d1: 43\nh1f1: 43\nh1g1: 43\nf3d3: 42\nf3e3: 43\nf3g3: 43\nf3h3: 43\nf3f4: 43\nf3g4: 43\nf3f5: 45\nf3h5: 43\nf3f6: 39\ne1d1: 43\ne1f1: 43\ne1g1: 43\ne1c1: 43";
//std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
////pos 2 for depth 1
//std::string expectedResults = "b4b3: 1\ne6e5: 1\ng6g5: 1\nd7d6: 1\nh3g2: 1\ne6d5: 1\nd7c6: 1\nb4c3: 1\nb6a4: 1\nb6c4: 1\nb6d5: 1\nb6c8: 1\nf6e4: 1\nf6g4: 1\nf6d5: 1\nf6h5: 1\nf6h7: 1\nf6g8: 1\na6e2: 1\na6d3: 1\na6c4: 1\na6b5: 1\na6b7: 1\na6c8: 1\ng7h6: 1\ng7f8: 1\na8b8: 1\na8c8: 1\na8d8: 1\nh8h4: 1\nh8h5: 1\nh8h6: 1\nh8h7: 1\nh8f8: 1\nh8g8: 1\ne7c5: 1\ne7d6: 1\ne7d8: 1\ne7f8: 1\ne8g8: 1\ne8f8: 1";
//std::string fen = "r3k2r/p1ppqpb1/bnN1pnp1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1";

////pos 2 for depth 1
//std::string expectedResults = "b4b3: 1\ne6e5: 1\ng6g5: 1\nd7d6: 1\nh3g2: 1\ne6d5: 1\nd7c6: 1\nb4c3: 1\nb6a4: 1\nb6c4: 1\nb6d5: 1\nb6c8: 1\nf6e4: 1\nf6g4: 1\nf6d5: 1\nf6h5: 1\nf6h7: 1\nf6g8: 1\na6e2: 1\na6d3: 1\na6c4: 1\na6b5: 1\na6b7: 1\na6c8: 1\ng7h6: 1\ng7f8: 1\na8b8: 1\na8c8: 1\na8d8: 1\nh8h4: 1\nh8h5: 1\nh8h6: 1\nh8h7: 1\nh8f8: 1\nh8g8: 1\ne7c5: 1\ne7d6: 1\ne7d8: 1\ne7f8: 1\ne8g8: 1\ne8f8: 1";
//std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N3Qp/PPPBBPPP/R3K2R b KQkq - 0 1";

//pos 2 for depth 3
//int depth = 3;
//std::string expectedResults = "a2a3: 2186\nb2b3: 1964\ng2g3: 1882\nd5d6: 1991\na2a4: 2149\ng2g4: 1843\ng2h3: 1970\nd5e6: 2241\nc3b1: 2038\nc3d1: 2040\nc3a4: 2203\nc3b5: 2138\ne5d3: 1803\ne5c4: 1880\ne5g4: 1878\ne5c6: 2027\ne5g6: 1997\ne5d7: 2124\ne5f7: 2080\nd2c1: 1963\nd2e3: 2136\nd2f4: 2000\nd2g5: 2134\nd2h6: 2019\ne2d1: 1733\ne2f1: 2060\ne2d3: 2050\ne2c4: 2082\ne2b5: 2057\ne2a6: 1907\na1b1: 1969\na1c1: 1968\na1d1: 1885\nh1f1: 1929\nh1g1: 2013\nf3d3: 2005\nf3e3: 2174\nf3g3: 2214\nf3h3: 2360\nf3f4: 2132\nf3g4: 2169\nf3f5: 2396\nf3h5: 2267\nf3f6: 2111\ne1d1: 1894\ne1f1: 1855\ne1g1: 2059\ne1c1: 1887";
//std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";

//
int depth = 2;
std::string expectedResults = "b4b3: 46\ng6g5: 44\nc7c6: 46\nd7d6: 44\nc7c5: 46\ne6d5: 45\nb4c3: 45\nb6a4: 44\nb6c4: 43\nb6d5: 45\nb6c8: 45\nf6e4: 48\nf6g4: 44\nf6d5: 46\nf6h5: 46\nf6h7: 46\nf6g8: 46\na6e2: 38\na6d3: 43\na6c4: 43\na6b5: 44\na6b7: 45\na6c8: 45\ng7h6: 45\ng7f8: 45\na8b8: 45\na8c8: 45\na8d8: 45\nh8h4: 46\nh8h5: 45\nh8h6: 45\nh8h7: 45\nh8f8: 45\nh8g8: 45\ne7c5: 45\ne7d6: 44\ne7d8: 45\ne7f8: 45\ne8d8: 45\ne8f8: 45\ne8g8: 45\ne8c8: 45";
std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2QPp/PPPBBP1P/R3K2R b KQkq - 0 1";

//int depth = 1;
//std::string fen = "r3k2r/p1ppqpb1/bn2pn2/3PN1p1/1p2P3/2N2QPp/PPPBBP1P/R3K2R w KQkq - 0 1";

//int depth = 1;
//std::string fen = "r3k2r/p1ppqpb1/1n2pnp1/3PN3/1p2P3/2N2QPp/PPPBbP1P/R3K2R w KQkq - 0 1";
void runTest1() {
	b = new Board();
	perftDivideResults = new std::unordered_map<std::string, int>();
	//int depth = 3;
	b->loadPosition(fen);
	int numNodes = searchDivide(depth, depth);
	comparePerftDivideResults(fen);
	
}

void runTest2() {
	b = new Board();
	b->initStartPosition();
	b->loadPosition("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
	//b->loadPosition("8/p7/8/1P6/K1k3p1/6P1/7P/8 w - -");
	auto start = std::chrono::high_resolution_clock::now();
	//std::wcout << search_copy(b, 4) << std::endl;
	auto end = std::chrono::high_resolution_clock::now();
	std::wcout << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " seconds" << std::endl;

	//b = new Board();
	b->initStartPosition();
	b->loadPosition("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
	//b->loadPosition("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
	start = std::chrono::high_resolution_clock::now();
	std::wcout << search(4) << std::endl;
	end = std::chrono::high_resolution_clock::now();
	std::wcout << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " seconds" << std::endl;
}

int search(int depth) {

	std::vector<Move> moves = b->generateMoves();
	if (depth == 1) {
		return moves.size();

	}
	int localNodes = 0;
	for (int i = 0; i < moves.size(); i++) {
		b->makeMove(moves[i]);
		int nodesFromNewPos = search(depth - 1);
		localNodes += nodesFromNewPos;
		b->unmakeMove(moves[i]);
	}
	return localNodes;
}

int search_copy(Board* b, int depth) {

	std::vector<Move> moves = b->getAllMovesCopy();
	if (depth == 1) {
		return moves.size();

	}
	int localNodes = 0;
	for (int i = 0; i < moves.size(); i++) {
		Board copy(*b);
		copy.makeMove(moves[i]);
		int nodesFromNewPos = search_copy(&copy, depth - 1);
		localNodes += nodesFromNewPos;
	}
	return localNodes;
}

int searchDivide(int startDepth, int currentDepth) {
	std::vector<Move> moves = b->generateMoves();
	if (currentDepth == 1) {
		/*for (Move m : moves) {
			std::wcout << moveName(m).c_str() << std::endl;
		}*/
		
		//std::wcout << moves.size() << std::endl;
		return moves.size();
	}
	int numLocalNodes = 0;
	for (Move m : moves) {
		b->makeMove(m);
		//Board copy(*b);
		//copy.makeMove(m);
		int numMovesForThisNode = searchDivide(startDepth, currentDepth - 1);
		numLocalNodes += numMovesForThisNode;
		b->unmakeMove(m);

		if (currentDepth == startDepth) {
			(*perftDivideResults)[moveName(m)] = numMovesForThisNode;
		}
	}
	return numLocalNodes;
}

void comparePerftDivideResults(std::string fen) {
	std::vector<std::string> expected = FenUtility::splitString(expectedResults, "\n");
	std::unordered_map<std::string, int> expectedPerftResults;
	for (std::string line : expected) {
		std::string moveName = FenUtility::splitString(line, ":")[0];
		std::string nodeCount = FenUtility::splitString(line, ":")[1];

		expectedPerftResults[moveName] = stoi(nodeCount);
	}

	for (auto pair : expectedPerftResults) {
		std::string move = pair.first;
		if (perftDivideResults->find(move) != perftDivideResults->end()) {
			int expectedValue = expectedPerftResults[move];
			int actualValue = (*perftDivideResults)[move];

			if (expectedValue != actualValue) {
				b->loadPosition(fen);
				std::vector<Move> movesFromPos = b->generateMoves();
				for (Move m : movesFromPos) {
					if (moveName(m) == move) {
						b->makeMove(m);
						break;
					}
				}

				std::wcout << move.c_str() << ": Expected " << expectedValue << " but had " << actualValue << std::endl;
				std::wcout << *b << std::endl;
			}
		}
		else {
			std::wcout << "Expected " << move.c_str() << " but was not found" << std::endl;
		}
	}
}

std::string moveName(Move& m) {
	std::string fileNames = "abcdefgh";
	std::string rankNames = "87654321";

	std::string from = "";
	from += fileNames[m.getStartJ()];
	from += rankNames[m.getStartI()];

	std::string to = "";
	to += fileNames[m.getTargetJ()];
	to += rankNames[m.getTargetI()];
	std::string promotion = "";

	switch (m.getMoveFlag()) {
	case MoveFlag::promoteToBishop:
		promotion += "b";
		break;
	case MoveFlag::promoteToKnight:
		promotion += "n";
		break;

	case MoveFlag::promoteToQueen:
		promotion += "q";
		break;

	case MoveFlag::promoteToRook:
		promotion += "r";
		break;
	}
	return from + to + promotion;
}