#include <stdio.h>
#include <stdlib.h>

void printBoard(int[25]);
int canMove(int, int);
int repeats(int, int[25], int);
int board[5][5];

void tour(int i, int j, int spaces, int spacesVisited[25]){
    ++spaces;
    if(spaces == 25){
        printBoard(spacesVisited);
        return;
    }
    if(canMove(i + 1, j + 2) == 0 && repeats(spaces, spacesVisited, board[i+1][j+2]) == 0){
        spacesVisited[spaces] = board[i+1][j+2];
        tour(i+1, j+2, spaces, spacesVisited);
    }
    if(canMove(i+1, j-2) == 0 && repeats(spaces, spacesVisited, board[i+1][j-2]) == 0){
        spacesVisited[spaces] = board[i+1][j-2];
        tour(i+1, j-2, spaces, spacesVisited);
    }
    if(canMove(i-1, j+2) == 0 && repeats(spaces, spacesVisited, board[i-1][j+2]) == 0){
        spacesVisited[spaces] = board[i-1][j+2];
        tour(i-1, j+2, spaces, spacesVisited);
    }
    if(canMove(i-1, j-2) == 0 && repeats(spaces, spacesVisited, board[i-1][j-2]) == 0){
        spacesVisited[spaces] = board[i-1][j-2];
        tour(i-1, j-2, spaces, spacesVisited);
    }
    if(canMove(i+2, j+1) == 0 && repeats(spaces, spacesVisited, board[i+2][j+1]) == 0){
        spacesVisited[spaces] = board[i+2][j+1];
        tour(i+2, j+1, spaces, spacesVisited);
    }
    if(canMove(i+2, j-1) == 0 && repeats(spaces, spacesVisited, board[i+2][j-1]) == 0){
        spacesVisited[spaces] = board[i+2][j-1];
        tour(i+2, j-1, spaces, spacesVisited);
    }
    if(canMove(i-2, j+1) == 0 && repeats(spaces, spacesVisited, board[i-2][j+1]) == 0){
        spacesVisited[spaces] = board[i-2][j+1];
        tour(i-2, j+1, spaces, spacesVisited);
    }
    if(canMove(i-2, j-1) == 0 && repeats(spaces, spacesVisited, board[i-2][j-1]) == 0){
        spacesVisited[spaces] = board[i-2][j-1];
        tour(i-2, j-1, spaces, spacesVisited);
    }
}

void printBoard(int spacesVisited[25]){
    int i;
    for(i = 0; i < 25; ++i){
        if(i == 24){
            printf("%d\n", spacesVisited[i]);
        }else{
            printf("%d, ", spacesVisited[i]);
        }
    }
}

int canMove(int i, int j){
    if(i < 5 && i >= 0 && j >= 0 && j < 5){
        return 0;
    }else{
        return 1;
    }
}

int repeats(int spaces, int spacesVisited[25], int val){
    int i;
    for(i = 0; i < spaces; ++i){
        if(val == spacesVisited[i]){
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]){
    int spacesVisited[25];
    int i, j;
    int spaces = 1;
    for(i = 0; i < 5; ++i){
        for(j = 0; j < 5; ++j){
            board[i][j] = spaces;
            ++spaces;
        }
    }
    spaces = 0;
    for(i = 0; i < 5; ++i){
        for(j = 0; j < 5; ++j){
            spacesVisited[spaces] = board[i][j];
            tour(i, j, spaces, spacesVisited);
            spaces = 0;
        }
    }
}
