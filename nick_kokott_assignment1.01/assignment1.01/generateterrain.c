#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int topExitLocation;
int leftExitLocation;
int rightExitLocation;
int bottomExitLocation;
void helperPlaceTerrain(char[21][80], int, int, int, int, char);

void printMap(char map[21][80]){
    int i, j;
    for(i = 0; i < 21; ++i){
        for(j = 0; j < 80; ++j){
            printf("%c", map[i][j]);
        }
        printf("\n");
    }
}

void generateBorder(char map[21][80]){
    int i;
    time_t t;
    for(i = 0; i < 21; ++i){
        map[i][0] = '%';
        map[i][79] = '%';
    }
    for(i = 0; i < 80; ++i){
        map[0][i] = '%';
        map[20][i] = '%';
    }
    srand((unsigned) time(&t));
    topExitLocation = rand() % 80;
    if(topExitLocation == 0){
        ++topExitLocation;
    }if(topExitLocation == 79){
        --topExitLocation;
    }
    map[0][topExitLocation] = '#';
    bottomExitLocation = rand() % 80;
    if(bottomExitLocation == 0){
        ++bottomExitLocation;
    }if(bottomExitLocation == 79){
        --bottomExitLocation;
    }
    map[20][bottomExitLocation] = '#';
    leftExitLocation = rand() % 21;
    if(leftExitLocation == 0){
        ++leftExitLocation;
    }if(leftExitLocation == 20){
        --leftExitLocation;
    }
    map[leftExitLocation][0] = '#';
    rightExitLocation = rand() % 21;
    if(rightExitLocation == 0){
        ++rightExitLocation;
    }if(rightExitLocation == 20){
        --rightExitLocation;
    }
    map[rightExitLocation][79] = '#';
}

void generateRoads(char map[21][80]){
    srand(time(NULL));
    int goRight = rand() % 60 + 1;
    srand(time(NULL));
    int goUp = rand() % 15 + 1;
    int i, j;
    for(i = 0; i < 80; ++i){
        if(i == goRight){
            int distApart = rightExitLocation - leftExitLocation;
            if(distApart < 0){
                for(j = 0; j > distApart; --j){
                    map[leftExitLocation + j][i] = '#';
                }
            }else{
                for(j = 0; j < distApart; ++j){
                    map[leftExitLocation + j][i] = '#';
                }
            }
        }if (i < goRight){
            map[leftExitLocation][i] = '#';
        }else{
            map[rightExitLocation][i] = '#';
        }
    }
    for(i = 20; i >= 0; --i){
        if(i == goUp){
            int distApart = topExitLocation - bottomExitLocation;
            if(distApart < 0){
                for(j = 0; j >= distApart; --j){
                    map[i][bottomExitLocation + j] = '#';
                }
            }else{
                for(j = 0; j <= distApart; ++j){
                    map[i][bottomExitLocation + j] = '#';
                }
            }
        }if(i < goUp){
            map[i][topExitLocation] = '#';
        }else{
            map[i][bottomExitLocation] = '#';
        }
    }
}

void placeWorldTerrain(char map[21][80]){
    char tallGrass = ':';
    char water = '~';
    char tree = '^';
    char mtn = '%';
    time_t t;
    srand((unsigned) time(&t));
    int i;
    for(i = 0; i < 6; ++i){
        int mapPlaceX = rand() % 75;
        int mapPlaceY = rand() % 15;
        int xStop = (rand() % 20 + 2) + mapPlaceX;
        int yStop = (rand() % 20 + 2) + mapPlaceY;
        if(i == 2 || i == 3 || i == 4){
            if(map[mapPlaceY][mapPlaceX] == ':' && mapPlaceX > 40){
                mapPlaceX = mapPlaceX - 17;
            }if(map[mapPlaceY][mapPlaceX] == ':' && mapPlaceX < 40){
                mapPlaceX = mapPlaceX + 17;
            }
            helperPlaceTerrain(map, mapPlaceX, mapPlaceY, xStop, yStop, tallGrass);
        }if(i == 5){
            helperPlaceTerrain(map, mapPlaceX, mapPlaceY, xStop, yStop, water);
        }if(i == 0){
            helperPlaceTerrain(map, mapPlaceX, mapPlaceY, xStop, yStop, tree);
        }if(i == 1){
            helperPlaceTerrain(map, mapPlaceX, mapPlaceY, xStop, yStop, mtn);
        }
    }
}

void helperPlaceTerrain(char map[21][80], int mapPlaceX, int mapPlaceY, int xStop, int yStop, char toPlace){
    int i, j;
    for(i = mapPlaceY; i < yStop; ++i){
        for(j = mapPlaceX; j < xStop; ++j){
            if(i < 21 && j < 80 && map[i][j] != '%' && map[i][j] != '#'){
                map[i][j] = toPlace;
            }
        }
    }
}

void placeMartAndCenter(char map[21][80]){
    int i;
    for(i = 0; i < 80; ++i){
        if(map[10][i] == '#' && map[10][i + 1] != '#' && i + 1 != 79){
            map[10][i + 1] = 'C';
            break;
        }if(map[10][i] == '#' && map[10][i - 1] != '#' && i - 1 != 0){
            map[10][i - 1] = 'C';
            break;
        }
    }
    for(i = 0; i < 21; ++i){
        if(map[i][40] == '#' && map[i + 1][40] != '#' && i + 1 != 20){
            map[i + 1][40] = 'M';
            break;
        }if(map[i][40] == '#' && map[i - 1][40] != '#' && i - 1 != 0){
            map[i - 1][40] = 'M';
            break;
        }
    }
}

void fillShortGrass(char map[21][80]){
    int i, j;
    for(i = 1; i < 21; ++i){
        for(j = 1; j < 80; ++j){
            map[i][j] = '.';
        }
    }
}

int main(int argc, char *argv[]){
    char map[21][80];
    int i, j;
    for(i = 0; i < 21; ++i){
        for(j = 0; j < 80; ++j){
            map[i][j] = ' ';
        }
    }
    fillShortGrass(map);
    generateBorder(map);
    placeWorldTerrain(map);
    generateRoads(map);
    placeMartAndCenter(map);
    printMap(map);
}