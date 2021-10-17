#include "UCI.h"
#include "Board.h"
#include "MoveGenerator.h"
#include <iostream>
#include "Search.h"
#include "Move.h"
#include <vector>
#include "FenUtility.h"
#include "Game.h"
#include <omp.h>
#pragma warning(disable : 4996)

#define INPUTBUFFER 400 * 6
#define NOMOVE Move(0)

// go depth 6 wtime 180000 btime 100000 binc 1000 winc 1000 movetime 1000 movestogo 40
void UCI::parseGo(char* line, ComputeOptions* info, Board* pos)
{
    int depth = -1, movestogo = 30, movetime = -1;
    int time = -1, inc = 0;
    bool timeset = false;
    char* ptr = NULL;
    info->max_time = -1;

    if ((ptr = strstr(line, "infinite"))) {
        ;
    }

    if ((ptr = strstr(line, "binc")) && !pos->whiteToMove) {
        inc = atoi(ptr + 5);
    }

    if ((ptr = strstr(line, "winc")) && pos->whiteToMove) {
        inc = atoi(ptr + 5);
    }

    if ((ptr = strstr(line, "wtime")) && pos->whiteToMove) {
        time = atoi(ptr + 6);
    }

    if ((ptr = strstr(line, "btime")) && !pos->whiteToMove) {
        time = atoi(ptr + 6);
    }

    if ((ptr = strstr(line, "movestogo"))) {
        movestogo = atoi(ptr + 10);
    }

    if ((ptr = strstr(line, "movetime"))) {
        movetime = atoi(ptr + 9);
    }

    if ((ptr = strstr(line, "depth"))) {
        depth = atoi(ptr + 6);
    }

    if (movetime != -1) {
        time = movetime;
        movestogo = 1;
    }

    info->startTime = ::omp_get_wtime();
    info->depth = depth;

    if (time != -1) {
        timeset = true;
        time /= movestogo;
        time -= 50;
        info->max_time = (time + inc) / 1000;
    }

    if (depth == -1) {
        info->depth = 1000;
    }

    std::cout << "time:" << time << " start:" << info->startTime << " stop:" << info->max_time << " depth: " << info->depth << " timeset : " << timeset << std::endl;
        //time, info->startTime, info->max_time, info->depth, timeset);
    Search s;
    Game game = Game(pos);
    s.computeMove(game, *info);
    //SearchPosition(pos, info);
}

// position fen fenstr
// position startpos
// ... moves e2e4 e7e5 b7b8q
void UCI::parsePosition(char* lineIn, Board* pos)
{
    lineIn += 9;
    char* ptrChar = lineIn;

    if (strncmp(lineIn, "startpos", 8) == 0) {
        pos->initStartPosition();
    }
    else {
        ptrChar = strstr(lineIn, "fen");
        if (ptrChar == NULL) {
            pos->initStartPosition();
        }
        else {
            ptrChar += 4;
            pos->loadPosition(ptrChar);
        }
    }

    ptrChar = strstr(lineIn, "moves");
    Move move;
    if (ptrChar != NULL) {
        ptrChar += 6;
        while (*ptrChar) {
            move = parseMove(ptrChar, pos);
            if (move == NOMOVE) break;
            pos->makeMove(move);
            pos->plyCount = 0;
            while (*ptrChar && *ptrChar != ' ') ptrChar++;
            ptrChar++;
        }
    }
    std::cout << FenUtility::currentFen(pos) << std::endl;
}

Move UCI::parseMove(char* ptrChar, Board* pos)
{
    if (ptrChar[1] > '8' || ptrChar[1] < '1') return NOMOVE;
    if (ptrChar[3] > '8' || ptrChar[3] < '1') return NOMOVE;
    if (ptrChar[0] > 'h' || ptrChar[0] < 'a') return NOMOVE;
    if (ptrChar[2] > 'h' || ptrChar[2] < 'a') return NOMOVE;

    int from = ptrChar[0] - 'a' + (ptrChar[1] - '1') * 8;
    int to = ptrChar[2] - 'a' + (ptrChar[3] - '1') * 8;


    std::vector<Move> list = MoveGenerator::generateMoves(*pos);
    int moveNum = 0;
    Move move;

    for (moveNum = 0; moveNum < list.size(); ++moveNum) {
        move = list[moveNum];
        int moveFrom = move.getStartSquare();
        int moveTo = move.getTargetSquare();
        if (moveFrom == from && moveTo == to) {
            //PromPce = PROMOTED(move);
            if (move.isPromotion()) {
                int moveFlag = move.getMoveFlag();
                if (moveFlag == MoveFlag::promoteToRook && ptrChar[4] == 'r') {
                    return move;
                }
                else if (moveFlag == MoveFlag::promoteToBishop && ptrChar[4] == 'b') {
                    return move;
                }
                else if (moveFlag == MoveFlag::promoteToQueen && ptrChar[4] == 'q') {
                    return move;
                }
                else if (moveFlag == MoveFlag::promoteToKnight && ptrChar[4] == 'n') {
                    return move;
                }
                continue;
            }
            return move;
        }
    }

    return NOMOVE;
}

void UCI::UCILoop()
{
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    char line[INPUTBUFFER];
    printf("id name VJ\n");
    printf("id author Vukasin\n");
    printf("uciok\n");


    Board* board = new Board();
    ComputeOptions* info = new ComputeOptions();


    while (true) {
        memset(&line[0], 0, sizeof(line));
        fflush(stdout);
        if (!fgets(line, INPUTBUFFER, stdin))
            continue;

        if (line[0] == '\n')
            continue;

        if (!strncmp(line, "isready", 7)) {
            printf("readyok\n");
            continue;
        }
        else if (!strncmp(line, "position", 8)) {
            parsePosition(line, board);
        }
        else if (!strncmp(line, "ucinewgame", 10)) {
            parsePosition((char*)"position startpos\n", board);
        }
        else if (!strncmp(line, "go", 2)) {
            parseGo(line, info, board);
        }
        else if (!strncmp(line, "quit", 4)) {
            //info->quit = TRUE;
            break;
        }
        else if (!strncmp(line, "uci", 3)) {
            printf("id name VJ\n");
            printf("id author Vukasin\n");
            printf("uciok\n");
        }
        //if (info->quit) break;
    }
    //free(pos->PvTable->pTable);
}
