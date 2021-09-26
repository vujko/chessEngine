#include "Board.h"
#include <Windows.h>
#include <cstdio>
#include <fcntl.h>
#include <io.h>
#include "Move.h"
#include "Perft.h"

int search(Board* b, int depth) {

	std::vector<Move> moves = b->getAllMoves();
	if (depth == 1) {
		return moves.size();

	}

	int localNodes = 0;
	for (int i = 0; i < moves.size(); i++) {
		Board copy(*b);
		copy.makeMove(moves[i]);
		int nodesFromNewPos = search(&copy, depth - 1);
		localNodes += nodesFromNewPos;
	}
	return localNodes;
}

void test() {
	Board* b = new Board();
	b->loadPosition("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N3Qp/PPPBBPPP/R3K2R b KQkq - 0 1");
	Move m = Move(0, 0, 0, 1);
	for (int i = 0; i < 10000000; i++) {
		b->makeMove(m);
		b->makeMove(m);
	}
}

int main(int argc, char** argv) {
	_setmode(_fileno(stdout), _O_U16TEXT);
	//Board* b = new Board();
	//b->initStartPosition();
	//b->loadPosition("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
	//std::wcout << search(b,4) << std::endl;
	//std::wcout << *b << std::endl;
	//runTest();
	test();
}