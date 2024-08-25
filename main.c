#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#include <intrin.h>
#include <zmmintrin.h>

UINT32 Squares = 0;
UINT32 Bmp = 0x1FF;
BOOL EngineTurn;

// 1 = Win
// 2 = draw
static inline BOOL Win(UINT32 _bmp, UINT32 sq) {
    // Search variations
    _bmp = ~_bmp;
    // Horizontal
    
    if((_bmp & 7) == (7) && ((sq & 7) == 7 || (sq & 7) == 0)) return 1;
    if((_bmp & (7 << 3)) == (7 << 3) && ((sq & (7 << 3)) == (7 << 3) || (sq & (7 << 3)) == 0)) return 1;
    if((_bmp & (7 << 6)) == (7 << 6) && ((sq & (7 << 6)) == (7 << 6) || (sq & (7 << 6)) == 0)) return 1;
    // Vertical
    
    if((_bmp & 0x49) == 0x49 && ((sq & 0x49) == 0x49 || (sq & 0x49) == 0)) return 1;
    if((_bmp & (0x49 << 1)) == (0x49 << 1) && ((sq & (0x49 << 1)) == (0x49 << 1) || (sq & (0x49 << 1)) == 0)) return 1;
    if((_bmp & (0x49 << 2)) == (0x49 << 2) && ((sq & (0x49 << 2)) == (0x49 << 2) || (sq & (0x49 << 2)) == 0)) return 1;
    // Diagonal
    if((_bmp & 0x111) == 0x111 && ((sq & 0x111) == 0x111 || (sq & 0x111) == 0)) return 1;
    if((_bmp & 0x54) == 0x54 && ((sq & 0x54) == 0x54 || (sq & 0x54) == 0)) return 1;

    return 0;
}

static inline void PrintGame() {
    for(UINT y = 0;y<3;y++) {
        for(UINT x = 0;x<3;x++) {
            if(x) {
                printf(" | ");
            }
            if(!_bittest(&Bmp, x + y * 3)) {
                if(_bittest(&Squares, x + y * 3)) {
                    // O
                    printf("O ");
                } else {
                    // X
                    printf("X ");
                }
            } else {
                printf("  ");
            }
        }
        printf("\n");
        if(y != 2){
            printf("------------");
            printf("\n");
        }
    }
}

static float minimax(UINT32 Present, UINT32 SquareType, BOOL Turn) {
    float ev = Turn ? 1 : -1;
    UINT32 sm = Present;
    UINT32 Index;

    while(_BitScanForward(&Index, sm)) {
        _bittestandreset(&sm, Index);
        _bittestandreset(&Present, Index);
        SquareType |= (Turn << Index);
        if(Win(Present, SquareType)) {
            return Turn ? -1 : 1;
        } 
        if(!Present) return 0;
        float v = minimax(Present, SquareType, Turn ^ 1);
        if(Turn) {
            if(v < ev) {
                ev = v;
            }
        } else if(v > ev) {
            ev = v;
        }
        _bittestandset(&Present, Index);
        _bittestandreset(&SquareType, Index);
    }
finish:
    return ev;
}

float eval;


// Present = (each bit set to 0 means that the square is present)
int Engine() {
    eval = EngineTurn ? 1 : -1;
    UINT32 Present = Bmp, SquareType = Squares;
    UINT32 SearchMask = Present;

    UINT32 Index = 0;

    int bestmove = 0;




    while(_BitScanForward(&Index, SearchMask)) {
        _bittestandreset(&SearchMask, Index);
        _bittestandreset(&Present, Index);
        SquareType |= (EngineTurn << Index);
        if(Win(Present, SquareType)) {
            eval = EngineTurn ? -1 : 1;
            return Index;
        } 
        if(!Present) {
            eval = 0; // Draw
            return Index;
        }
        // Try the move
        float v = minimax(Present, SquareType, EngineTurn ^ 1);
        if(EngineTurn) {
            if(v < eval) {
                eval = v;
                bestmove = Index;
            }
        } else if(v > eval) {
            eval = v;
            bestmove = Index;
        }
        _bittestandset(&Present, Index);
        _bittestandreset(&SquareType, Index);
    }
finish:
    return bestmove;
}

int main() {

    printf("Tic Tac Toe AI (I dare you to beat me !) \n");

    printf("Choose X(0) or O(1) : "); 
    int Turn = 0;
    scanf("%d", &Turn);
    EngineTurn = Turn ^ 1;
    int PlayedMove;

    Turn = 0;
    for(;;) {
        if(Turn == EngineTurn) {
            PlayedMove = Engine();

            printf("Engine played : %d, eval : %f\n", PlayedMove, eval);
            _bittestandreset(&Bmp, PlayedMove);
            if(Turn) {
                _bittestandset(&Squares, PlayedMove);
            }
            PrintGame();
            if(Win(Bmp, Squares)) {
                printf("Engine won !\n");
                return 0;
            }
            else {
                if(!Bmp) {
                    printf("The game is a draw.\n");
                    return 0;
                }
            }
        } else {
            printf("Your turn : ");
            scanf("%d", &PlayedMove);
            _bittestandreset(&Bmp, PlayedMove);
            if(Turn) {
                _bittestandset(&Squares, PlayedMove);
            }
            PrintGame();
            if(Win(Bmp, Squares)) {
                printf("Player won ! (It's actually impossible to print this message btw)\n");
                return 0;
            }
            else {
                if(!Bmp) {
                    printf("The game is a draw.\n");
                    return 0;
                }
            }
        }
        Turn ^= 1;
    }
    return 0;
}