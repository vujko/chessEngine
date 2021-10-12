﻿#include "Board.h"
#include <Windows.h>
#include <cstdio>
#include <fcntl.h>
#include <io.h>
#include "Move.h"
#include "Perft.h"
#include "Search.h"
#include <thread>

void test() {
	Board* b = new Board();
	b->loadPosition("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N3Qp/PPPBBPPP/R3K2R b KQkq - 0 1");
	Move m = Move(0, 0, 0, 1);
	for (int i = 0; i < 100000; i++) {
		b->makeMove(m);
		b->makeMove(m);
	}
}

int main(int argc, char** argv) {
	_setmode(_fileno(stdout), _O_U16TEXT);
	//runTest2();
	//runTest2();
	Board* b = new Board();
	b->initStartPosition();
	Search* s = new Search();
	Move m = s->computeMove(*b, *(new ComputeOptions));
	std::wcout << moveName(m).c_str() << std::endl;
}