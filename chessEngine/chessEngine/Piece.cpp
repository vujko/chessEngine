#include "Piece.h"
#include "Board.h"
#include "Move.h"

bool Piece::isColor(int piece, int color)
{
    return (piece & colorMask) == color;
}

int Piece::getPieceColor(int piece)
{
    return piece & colorMask;
}

int Piece::getPieceType(int piece)
{
    return piece & typeMask;
}

bool Piece::isSlidingPiece(int piece)
{
    return (piece & 0b100) != 0;
}

std::vector<Move> Piece::getMoves(Board* b, int i, int j)
{
    int piece = b->squares[i][j];
    int pieceType = Piece::getPieceType(piece);
    switch (pieceType)
    {
    case Piece::pawn:
        return Piece::getPawnMoves(b, i, j);

    case Piece::bishop:
        return Piece::getSlidingPieceMoves(b, i, j, Piece::bishop);

    case Piece::knight:
        return Piece::getKnightMoves(b, i, j);

    case Piece::rook:
        return Piece::getSlidingPieceMoves(b, i, j, Piece::rook);

    case Piece::queen:
        return Piece::getSlidingPieceMoves(b, i, j, Piece::queen);

    case Piece::king:
        return Piece::getKingMoves(b, i, j);
    }
}

std::vector<Move> Piece::getPawnMoves(Board* b, int i, int j)
{
    std::vector<Move> moveList;
    int piece = b->squares[i][j];
    int pieceColor = Piece::getPieceColor(piece);
    int startRow = (pieceColor == Piece::white) ? 6 : 1;
    int epSquareX = (pieceColor == Piece::white) ? 2 : 5;
    int epSquareY = b->epFile;
    int finalX = (pieceColor == Piece::white) ? 0 : 7;

    //offset is needed, white goes NORTH = -1, black goes SOUTH = 1;
    int offset = (pieceColor == Piece::white) ? -1 : 1;
    //4 possible moves
    for (int moveType = 0; moveType < 4; moveType++) {
        //moving 2 spaces is only possible if the pawn hasn't moved yet
        if (moveType == 1 && i != startRow) continue;
        int x = i, y = j;

        switch (moveType) {
        case 0: x += offset; break;         //one space
        case 1: x += 2 * offset; break;     //two spaces
        case 2: x += offset; y++; break;    //right diagonal
        case 3: x += offset; y--; break;    //left diagonal
        }

        if (x >= 0 && x < 8 && y >= 0 && y < 8) {
            //cannot capture going forward
            if ((moveType == 0 || moveType == 1) && b->squares[x][y]) continue;

            //cannot jump over pieces
            if (moveType == 1 && b->squares[x - offset][y]) continue;

            //enPassant capture
            if (epSquareX == x && epSquareY == y) {
                //TODO check if check after ep
                moveList.push_back(Move(i, j, x, y, MoveFlag::enPassantCapture));
            }

            //if square is occupied
            if ((moveType == 2 || moveType == 3) && b->squares[x][y]) {
                int targetPiece = b->squares[x][y];
                if (Piece::getPieceColor(targetPiece) != pieceColor) {
                    if (x == finalX) {
                        moveList.push_back(Move(i, j, x, y, MoveFlag::promoteToQueen));
                        moveList.push_back(Move(i, j, x, y, MoveFlag::promoteToBishop));
                        moveList.push_back(Move(i, j, x, y, MoveFlag::promoteToKnight));
                        moveList.push_back(Move(i, j, x, y, MoveFlag::promoteToRook));
                    }
                    else
                        moveList.push_back(Move(i, j, x, y));
                }
            }
            //if square is not occupied
            else if (!(moveType == 2 || moveType == 3)){
                //two step forward
                if (moveType == 1)
                    moveList.push_back(Move(i, j, x, y, MoveFlag::pawnTwoForward));
                //pawn promotion
                else if (finalX == x) {
                    //promoting only to queen for now
                    moveList.push_back(Move(i, j, x, y, MoveFlag::promoteToQueen));
                    moveList.push_back(Move(i, j, x, y, MoveFlag::promoteToBishop));
                    moveList.push_back(Move(i, j, x, y, MoveFlag::promoteToKnight));
                    moveList.push_back(Move(i, j, x, y, MoveFlag::promoteToRook));
                }
                //regular step forward
                else
                    moveList.push_back(Move(i, j, x, y));
            }
        }
    }
    return moveList;
}

std::vector<Move> Piece::getKnightMoves(Board* b, int i, int j)
{
    std::vector<Move> moveList;
    int piece = b->squares[i][j];
    int pieceColor = Piece::getPieceColor(piece);

    for (int dir = 0; dir < 8; dir++) {
        int x = i, y = j;
        switch (dir) {
        case 0: x += 1; y += 2; break;
        case 1: x -= 1; y += 2; break;
        case 2: x += 2; y += 1; break;
        case 3: x += 2; y -= 1; break;
        case 4: x -= 2; y += 1; break;
        case 5: x -= 2; y -= 1; break;
        case 6: x += 1; y -= 2; break;
        case 7: x -= 1; y -= 2; break;
        }

        //on board
        if (x >= 0 && x < 8 && y >= 0 && y < 8) {
            //square occupied
            if (b->squares[x][y]) {
                //by opponent piece
                if (Piece::getPieceColor(b->squares[x][y]) != pieceColor)
                    moveList.push_back(Move(i, j, x, y));

            }
            //square not occupied
            else
                moveList.push_back(Move(i, j, x, y));
        }
    }
    return moveList;

}

std::vector<Move> Piece::getKingMoves(Board* b, int i, int j)
{
    std::vector<Move> moveList;
    int piece = b->squares[i][j];
    int pieceColor = Piece::getPieceColor(piece);

    for (int dir = 0; dir < 8; dir++) {
        int x = i, y = j;
        switch (dir) {
        case 0: y++; break; // north
        case 1: x++; break; // east
        case 2: y--; break; // south
        case 3: x--; break; // west
        case 4: x ++; y ++; break; //south-east
        case 5: x ++; y --; break; //south-west
        case 6: x --; y --; break; //north-west
        case 7: x --; y ++; break; //north-east
        }

        //on board
        if (x >= 0 && x < 8 && y >= 0 && y < 8) {
            //square occupied
            if (b->squares[x][y]) {
                //by opponent piece
                if (Piece::getPieceColor(b->squares[x][y]) != pieceColor)
                    moveList.push_back(Move(i, j, x, y));

            }
            //square not occupied
            else
                moveList.push_back(Move(i, j, x, y));
        }
    }
     
    //castling
    int pos = (pieceColor == Piece::white) ? 7 : 0;
    bool kingCheck = (pieceColor == Piece::white) ? b->whiteKingCheck : b->blackKingCheck;
    bool castleKingSide = (pieceColor == Piece::white) ? b->whiteCastleKingside : b->blackCastleKingside;
    bool castleQueenSide = (pieceColor == Piece::white) ? b->whiteCastleQueenside : b->blackCastleQueenside;
    if (!kingCheck){
        //rook hasn't moved or king hasn't moved
        if (castleKingSide) {
            //spaces are empty 
            if (!b->squares[pos][5] && !b->squares[pos][6]) {
                int possibleRook = b->squares[pos][7];
                if (Piece::getPieceType(possibleRook) == Piece::rook && Piece::getPieceColor(possibleRook) == pieceColor) {
                    if(!b->isSquareAttacked(pos, 5, pieceColor))
                        moveList.push_back(Move(i, j, pos, 6, MoveFlag::castling));
                }
            }
        }
        if (castleQueenSide) {
            //spaces are empty
            if (!b->squares[pos][1] && !b->squares[pos][2] && !b->squares[pos][3]) {
                int possibleRook = b->squares[pos][0];
                if (Piece::getPieceType(possibleRook) == Piece::rook && Piece::getPieceColor(possibleRook) == pieceColor) {
                    if(!b->isSquareAttacked(pos, 3, pieceColor))
                        moveList.push_back(Move(i, j, pos, 2, MoveFlag::castling));
                }
            }
        }
    }
    return moveList;
}

std::vector<Move> Piece::getSlidingPieceMoves(Board* b, int i, int j, int pieceType)
{
    std::vector<Move> moveList;
    int piece = b->squares[i][j];
    int pieceColor = Piece::getPieceColor(piece);
    int startDir = 0, endDir = 0;

    switch (pieceType) {
    case Piece::rook: startDir = 0; endDir = 4; break;
    case Piece::bishop: startDir = 4; endDir = 8; break;
    case Piece::queen: startDir = 0; endDir = 8; break;
    }

    for (int dir = startDir; dir < endDir; dir++) {
        int x = i, y = j;
        for (int step = 1; step < 8; step++, x = i, y = j) {
            switch (dir) {
            case 0: y += step; break; // north
            case 1: x += step; break; // east
            case 2: y -= step; break; // south
            case 3: x -= step; break; // west
            case 4: x += step; y += step; break; //south-east
            case 5: x += step; y -= step; break; //south-west
            case 6: x -= step; y -= step; break; //north-west
            case 7: x -= step; y += step; break; //north-east
            }

            //on board
            if (x >= 0 && x < 8 && y >= 0 && y < 8) {
                //square occupied
                if (b->squares[x][y]) {
                    //by opponent piece
                    if (Piece::getPieceColor(b->squares[x][y]) != pieceColor)
                        moveList.push_back(Move(i, j, x, y));
                    //cannot skip over pieces
                    break;
                }
                //square not occupied
                else
                    moveList.push_back(Move(i, j, x, y));
            }
            else
                //any other move in the same direction is also not on board
                break;

        }
    }
    return moveList;

}
