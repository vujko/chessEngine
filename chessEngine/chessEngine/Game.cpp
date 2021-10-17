#include "Game.h"
#include "MoveGenerator.h"
#include "Move.h"
#include "Perft.h"
#include "Board.h"
Game::Game() : gameResult(Result::playing)
{
	board = new Board();
	board->initStartPosition();
}

Game::Game(const Game& other)
{
	gameResult = other.gameResult;
	board = new Board();
	*board = *(other.board);
}

Game::Game(Board* board): board(board), gameResult(Result::playing)
{
	
}

void Game::doMove(Move& m)
{
	board->makeMove(m);
}

void Game::doRandomMove(std::mt19937_64* engine)
{
	std::uniform_int_distribution<std::size_t> moves_distribution(0, board->moves.size() - 1);
	doMove(board->moves[moves_distribution(*engine)]);
}

bool Game::isEndState()
{
	Result res = getGameState();
	return res != Result::playing;
}

//should be called only after end state is reached
double Game::getResult(bool whitePlayer)
{
	Result res = getGameState();
	if (res == Result::draw)
		return 0.5;
	bool hasWon = (whitePlayer) ? (res == Result::blackIsMated) : (res == Result::whiteIsMated);
	return (hasWon);
}

Game::Result Game::getGameState()
{
	std::vector<Move> moves = MoveGenerator::generateMoves(*board);
	if (moves.size() == 0) {
		if (MoveGenerator::inCheck) {
			return (board->whiteToMove) ? Result::whiteIsMated : Result::blackIsMated;
		}
		return Result::draw;
	}
	//fifty move?
	//threefold repetition?
	// Look for insufficient material (not all cases implemented yet)
	int numPawns = board->pawns[Board::whiteIndex]->numPieces + board->pawns[Board::blackIndex]->numPieces;
	int numRooks = board->rooks[Board::whiteIndex]->numPieces + board->rooks[Board::blackIndex]->numPieces;
	int numQueens = board->queens[Board::whiteIndex]->numPieces + board->queens[Board::blackIndex]->numPieces;
	int numKnights = board->knights[Board::whiteIndex]->numPieces + board->knights[Board::blackIndex]->numPieces;
	int numBishops = board->bishops[Board::whiteIndex]->numPieces + board->bishops[Board::blackIndex]->numPieces;

	if (numPawns + numRooks + numQueens == 0) {
		if (numKnights + numBishops <= 2) {
			return Result::draw;
		}
	}

	return Result::playing;
}
