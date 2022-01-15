#include "ChessBoard.h"

//Builds a chess Board from a given fenstring
ChessBoard::ChessBoard(std::string fenString, StateInfo& si) {
    initBoard(si);
    fenToBoard(fenString);
}

void ChessBoard::makeMove(Move move, StateInfo& si) {
    FLAGS flag = getFlag(move);
    int from = getFrom(move);
    int to = getTo(move);
    
    Colour us = colToMove;
    Colour them = ~colToMove;
    Piece moved = pieceOn(from);
    Piece capt = flag == ENPASSENT? makePiece(them, PAWN) : pieceOn(to);

    memcpy(&si, st, offsetof(StateInfo, captured));
    si.captured = capt;
    if (capt) {
        int capsq = to;
        if (flag == ENPASSENT) {
            capsq = us == WHITE ? to - 8 : to + 8;
        }
        removePiece(capsq);
    }
    if (flag == PROMOTION) {
        removePiece(from);
        putPiece(makePiece(us, getProm(move)), from);
    }
    if (flag == CASTLE) {
        if (to > from) {
            movePiece(to + 1, to - 1);
        } else {
            movePiece(to - 2, to + 1);
        }
        if (us == WHITE) {
                si.castlingRights ^= WHITE_CASTLING;
            } else {
                si.castlingRights ^= BLACK_CASTLING;
        }
    }
    si.epSquare = -1;
    if (typeOf(moved) == PAWN) {
        if ((to ^ from) == 16) {
            si.epSquare = us == WHITE ? from + 8 : from - 8;
        }
    }
    movePiece(from, to);
    si.previous = st;
    st = &si;
    colToMove = ~colToMove;
}

void ChessBoard::popBit(bitBoard& bb, int index) {
    bb = bb ^ ((bitBoard)1 << index);
}

void ChessBoard::setBit(bitBoard& bb, int index) {
    bb = bb | ((bitBoard)1 << index);
}

void ChessBoard::printBoard(bitBoard bb) {
    std::bitset<64> bits = std::bitset<64>(bb);
    int file;
    int rank;
    for (rank = 7; rank >= 0; rank--) {
        for (file = 0; file < 8; file++) {
            std::cout << bits[rank * 8 + file] << " ";
        }
        std::cout << std::endl;
    }
}

int ChessBoard::countBits(bitBoard bb) {
    int count = 0;

    while (bb) {
        count++;
        bb &= bb-1;
    }

    return count;
}

void ChessBoard::fenToBoard(std::string fenString) {
    int boardIndex = 56;
    int stringIndex = 0;
    char curr = fenString[0];
    int num;
    while (curr != *" ") {
        if (curr == *"/") {
            stringIndex++;
            boardIndex -= 16;
            curr = fenString[stringIndex];
            continue;
        }
        char* endPtr = &curr;
        strtol(&curr, &endPtr, 10);
        if (endPtr != &curr) {
            num = curr - 48;
            if (num >= 1) {
                boardIndex += num;
            }
        } else { 
            switch(curr) {
                case(*"P"): putPiece(WHITEPAWN, boardIndex);
                    break;
                case(*"N"): putPiece(WHITEKNIGHT, boardIndex);
                    break;
                case(*"R"): putPiece(WHITEROOK, boardIndex);
                    break;
                case(*"B"): putPiece(WHITEBISHOP, boardIndex);
                    break;
                case(*"Q"): putPiece(WHITEQUEEN, boardIndex);
                    break;
                case(*"K"): putPiece(WHITEKING, boardIndex);
                    break;
                case(*"p"): putPiece(BLACKPAWN, boardIndex);
                    break;
                case(*"n"): putPiece(BLACKKNIGHT, boardIndex);
                    break;
                case(*"r"): putPiece(BLACKROOK, boardIndex);
                    break;
                case(*"b"): putPiece(BLACKBISHOP, boardIndex);
                    break;
                case(*"q"): putPiece(BLACKQUEEN, boardIndex);
                    break;
                case(*"k"): putPiece(BLACKKING, boardIndex);
                    break;
            }
            boardIndex++;
        }
        stringIndex++;
        curr = fenString[stringIndex];
    }

    // Parse next Move
    stringIndex++;
    curr = fenString[stringIndex];
    if (curr == *"w") {
        colToMove = WHITE;
    } else {
        colToMove = BLACK;
    }

    // Parse Castling Rights
    stringIndex += 2;
    curr = fenString[stringIndex];
    if (curr != *"-") {
        while (curr != *" ") {
            if (curr == *"K") {
                st->castlingRights |= WHITE_OO;
            } else if (curr == *"Q") {
                st->castlingRights |= WHITE_OOO;
            } else if (curr == *"k") {
                st->castlingRights |= BLACK_OO;
            } else if (curr == *"q") {
                st->castlingRights |= BLACK_OOO;
            }
            stringIndex++;
            curr = fenString[stringIndex];
        }
    }

    stringIndex++;
    curr = fenString[stringIndex];

    // parse En Passent square
    if (curr != *"-") {
        if (curr == *"a") {
            st->epSquare = 0;
        } else if (curr == *"b") {
            st->epSquare = 1;
        } else if (curr == *"c") {
            st->epSquare = 2;
        } else if (curr == *"d") {
            st->epSquare = 3;
        } else if (curr == *"e") {
            st->epSquare = 4;
        } else if (curr == *"f") {
            st->epSquare = 5;
        } else if (curr == *"g") {
            st->epSquare = 6;
        } else if (curr == *"h") {
            st->epSquare = 7;
        }
        stringIndex++;
        curr = fenString[stringIndex];
        num = strtol(&curr, NULL, 10);
        st->epSquare += (8 - num) * 8;
    } else {
        st->epSquare = -1;
    }
    
}

// Initializes all bitboards to empty and all fields
void ChessBoard::initBoard(StateInfo& si) {
    for (int i = 0; i < PIECENB; i++) {
        piecesByType[i] = (bitBoard)0;
    }
    piecesByColour[WHITE] = (bitBoard)0;
    piecesByColour[BLACK] = (bitBoard)0;
    allPieces = (bitBoard)0;

    st = &si;

    st->castlingRights = NO_CASTLING;
    st->epSquare = -1;
    st->previous = NULL;

    colToMove = WHITE;
}

