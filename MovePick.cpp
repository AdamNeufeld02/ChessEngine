#include "MovePick.h"

MovePick::MovePick(ScoredMove* allMoves, int initLength, ChessBoard& cb) {
    moves = allMoves;
    length = initLength;
    score(cb);
    buildHeap();
}

Move MovePick::getNext() {
    Move ret = moves[0].move;
    swap(0, --length);
    heapifyDown(0);
    return ret;
}

void MovePick::score(ChessBoard& cb) {
    for (int i = 0; i < length; i++) {
        Move move = moves[i].move;
        PieceType capt = typeOf(cb.pieceOn(getTo(move)));
        PieceType pc = typeOf(cb.pieceOn(getFrom(move)));
        moves[i].score = mgVals[capt] + (mgVals[pc]/100);
    }
}

void MovePick::buildHeap() {
    for (int i = (length / 2) - 1; i >= 0; i--) {
        heapifyDown(i);
    }
}

void MovePick::heapifyDown(int start) {
    int largest = start;
    int leftChild = getLeftChild(start);
    int rightChild = getRightChild(start);
    if (leftChild < length && moves[largest] < moves[leftChild]) {
        largest = leftChild;
    }
    if (rightChild < length && moves[largest] < moves[rightChild]) {
        largest = rightChild;
    }
    if (largest != start) {
        swap(start, largest);
        heapifyDown(largest);
    }
}

inline int MovePick::getLeftChild(int parent) {
    return 2 * parent + 1;
}

inline int MovePick::getRightChild(int parent) {
    return 2 * parent + 2;
}

void MovePick::swap(int ind1, int ind2) {
    ScoredMove temp = moves[ind1];
    moves[ind1] = moves[ind2];
    moves[ind2] = temp;
}