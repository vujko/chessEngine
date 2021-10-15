#include "Evaluation.h"
#include "Board.h"

//void Evaluation::countMaterial(int& outWhiteMat, int& outBlackMat, int& outWhitePawnMaterial, int& outBlackPawnMaterial)
//{
//    for (int i = 0; i < 8; i++) {
//        for (int j = 0; j < 8; j++) {
//            //square occupied
//            if (int piece = board->squares[i][j]) {
//                int pieceType = Piece::getPieceType(piece);
//                bool isPawn = pieceType == Piece::pawn;
//                int pieceVal = getPieceValue(pieceType);
//                if (Piece::getPieceColor(piece) == Piece::white) {
//                    outWhiteMat += pieceVal;
//                    if (isPawn) outWhitePawnMaterial += pieceVal;
//                    whiteEval += evaluatePiecesPositions(pieceType, Piece::white, i, j);
//                }
//                else {
//                    outBlackMat += pieceVal;
//                    if (isPawn) outBlackPawnMaterial += pieceVal;
//                    blackEval += evaluatePiecesPositions(pieceType, Piece::black, i, j);
//                }
//                
//
//            }
//        }
//    }
//}
//
//int Evaluation::getPieceValue(const int pieceType)
//{
//    switch (pieceType) {
//    case Piece::bishop:
//        return bishopValue;
//    case Piece::knight:
//        return knightValue;
//    case Piece::pawn:
//        return pawnValue;
//    case Piece::queen:
//        return queenValue;
//    case Piece::rook:
//        return rookValue;
//    }
//    return 0;
//}
//
//float Evaluation::getEndGamePhaseWeight(const int materialWithoutPawns)
//{
//    int endGameMaterialStart = rookValue * 2 + bishopValue + knightValue;
//    float multiplier = 1 / endGameMaterialStart;
//    return 1 - std::min(1.f, materialWithoutPawns * multiplier);
//
//}
//
//int Evaluation::evaluatePiecesPositions(int pieceType, int pieceColor, int i, int j)
//{
//    if (pieceColor == Piece::black) {
//        i = 7 - i;
//    }
//    switch (pieceType) {
//    case Piece::bishop:
//        return bishopsPos[i][j];
//    case Piece::knight:
//        return knightsPos[i][j];
//    case Piece::pawn:
//        return pawnsPos[i][j];
//    case Piece::queen:
//        return queensPos[i][j];
//    case Piece::rook:
//        return rooksPos[i][j];
//    }
//    //king position handled later depending on stage of the game
//    return 0;
//}
//
//int Evaluation::evaluateKingPosition(float endGameWeight, bool isWhite)
//{
//    int kingEarlyPhase;
//    if (isWhite) {
//        int i = board->getWhiteKingPosI();
//        int j = board->getWhiteKingPosJ();
//
//        kingEarlyPhase = kingEarlyPos[i][j];
//    }
//    else {
//        int i = 7 - board->getBlackKingPosI();
//        int j = board->getBlackKingPosJ();
//
//        kingEarlyPhase = kingEarlyPos[i][j];
//    }
//    return kingEarlyPhase * (1 - endGameWeight);
//}
//
//int Evaluation::evaluate(Board* b)
//{
//    board = b;
//    whiteEval = 0, blackEval = 0;
//    int whiteMat = 0, blackMat = 0, whitePawnMat = 0, blackPawnMat = 0;
//    countMaterial(whiteMat, blackMat, whitePawnMat, blackPawnMat);
//
//    whiteEval += whiteMat;
//    blackEval += blackMat;
//
//    float whiteEndGameWeight = getEndGamePhaseWeight(whiteMat - whitePawnMat);
//    float blackEndGameWeight = getEndGamePhaseWeight(blackMat - blackPawnMat);
//
//    whiteEval += evaluateKingPosition(whiteEndGameWeight, true);
//    blackEval += evaluateKingPosition(blackEndGameWeight, false);
//
//    int eval = whiteEval - blackEval;
//
//    //mozda treba obrnuto zavisi kad se poziva funkcija
//    int perspective = (board->whiteToMove) ? 1 : -1;
//    return eval * perspective;
//}
