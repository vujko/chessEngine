#include "Evaluation.h"
#include "Board.h"

void Evaluation::countMaterial(int& outWhiteMat, int& outBlackMat)
{
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            //square occupied
            if (int piece = board->squares[i][j]) {
                int pieceType = Piece::getPieceType(piece);
                int pieceVal = getPieceValue(pieceType);
                if (Piece::getPieceColor(piece) == Piece::white)
                    outWhiteMat += pieceVal;
                else
                    outBlackMat += pieceVal;
                

            }
        }
    }
}

int Evaluation::getPieceValue(int pieceType)
{
    switch (pieceType) {
    case Piece::bishop:
        return bishopValue;
    case Piece::knight:
        return knightValue;
    case Piece::pawn:
        return pawnValue;
    case Piece::queen:
        return queenValue;
    case Piece::rook:
        return rookValue;
    }
    return 0;
}

int Evaluation::evaluate(Board* b)
{
    board = b;
    int whiteEval = 0, blackEval = 0;
    countMaterial(whiteEval, blackEval);
}
