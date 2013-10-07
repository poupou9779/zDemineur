#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "prototypes.h"


int main(void){
    srand((unsigned int)time(NULL));
    map_S   map;

    printf("** zDemineur (niveau 1) **\n\n");
    if(charger(&map) == false)
        fillmap(&map);
    mapGen(&map);
    while(gagne(&map) == false) {
#if defined (_WIN32) || defined (_WIN64)
        system("cls");
#else
        printf("________________________________________________________________________________\n");
#endif
        affiche_map(&map);
        action(&map);
    }
    printf("\nVous avez gagn%c ! Voici la carte enti%crement d%ccouverte :\n",130,138,130);
    affiche_map_Debug(&map);

    for(uint k = 0;k < map.lines;++k)
        free(map.Map[k]);

    free(map.Map);
    return 0;
}

bool alloc(map_S *map){
    map->Map = malloc(map->lines * sizeof(uint *));
    if(map == NULL)
        exit(1);

    for(uint k = 0;k < map->lines;++k) {
        map->Map[k]=malloc(map->col * sizeof(uint));
        if(map->Map[k] == NULL)
            return false;
    }
    return true;
}

void mapGen(map_S *map) {
    for(uint i = 0; i < map->lines; ++i)
        for(uint j = 0; j < map->col; ++j)
            map->Map[i][j].valeur = VIDE;

    for(uint i = 0; i < map->mines; ++i) {
        uint    x,
                y;
        do {
            x = (uint)rand() % map->lines;
            y = (uint)rand() % map->col;
        } while(map->Map[x][y].valeur == MINE);

        /*map->Map[x][y].valeur = MINE;*/
        ajoutermine(map,x,y);
    }
    for(uint i = 0; i < map->lines; ++i)
        for(uint j = 0; j < map->col; ++j)
            if(map->Map[i][j].valeur != MINE && map->Map[i][j].valeur != ' ')
                map->Map[i][j].valeur += 16;
}

void affiche_map(const map_S *map) {
    printf("\n   ");
    for(uint i = 0; i < map->col; ++i)
        if(i < 10)
            printf("  %d ",i);
        else
            printf(" %d ",i);

    printf("\n   +");

    for(uint i = 0; i < map->col; ++i)
        printf("---+");
    putchar('\n');

    for(uint i = 0; i < map->lines; ++i) {
        printf("%2d |",i);

        for(uint j = 0; j < map->col; ++j) {
            if(map->Map[i][j].flagged == false) {

                if(map->Map[i][j].visible == false) {
#if defined _WIN32 || defined _WIN64
                    Color(FOREGROUND_INTENSITY|FOREGROUND_GREEN|BACKGROUND_BLUE);
                    printf(" ? ");
                    ResetColor();
                    putchar('|');
#else
                    printf(" ? |");
#endif
                } else {
#if defined _WIN32 || defined _WIN64
                    if(map->Map[i][j].valeur == '1')
                        Color(FOREGROUND_BLUE|FOREGROUND_INTENSITY);
                    else if(map->Map[i][j].valeur == '2')
                        Color(FOREGROUND_GREEN|FOREGROUND_INTENSITY);
                    else if(map->Map[i][j].valeur == '3')
                        Color(FOREGROUND_RED|FOREGROUND_INTENSITY);
                    else if(map->Map[i][j].valeur == '4')
                        Color(FOREGROUND_BLUE|FOREGROUND_GREEN);
                    else if(map->Map[i][j].valeur == '5')
                        Color(FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY);
                    else if(map->Map[i][j].valeur == '6')
                        Color(FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY);
                    else if(map->Map[i][j].valeur == '7')
                        Color(FOREGROUND_RED);
                    else if(map->Map[i][j].valeur == '8')
                        Color(FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
                    printf(" %c ",map->Map[i][j].valeur);
                    ResetColor();
                    putchar('|');
#else
                    printf(" %c |",map->Map[i][j].valeur);
#endif
                }

            }
            else {
#if defined _WIN32 || defined _WIN64
                Color(FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
                printf(" F ");
                ResetColor();
                putchar('|');
#else
                printf(" F |");
#endif
            }
        }

        putchar('\n');
        printf("   +");

        for(uint z = 0; z < map->col; ++z)
            printf("---+");
        putchar('\n');
    }
    printf("\n\n");
}

char numb_adj_mines(const map_S *map, uint i, uint j) {
    uint     number=0;

    if(i > 0 && j > 0 && map->Map[i-1][j-1].valeur == MINE)
        number++;
    if(i > 0 && map->Map[i-1][j].valeur == MINE)
        number++;
    if(i > 0 && j < map->col-1 && map->Map[i-1][j+1].valeur == MINE)
        number++;
    if(j > 0 && map->Map[i][j-1].valeur == MINE)
        number++;
    if(j < map->col-1 && map->Map[i][j+1].valeur == MINE)
        number++;
    if(i < map->lines-1 && j > 0 && map->Map[i+1][j-1].valeur == MINE)
        number++;
    if(i < map->lines-1 && map->Map[i+1][j].valeur == MINE)
        number++;
    if(i < map->lines-1 && j < map->col-1 && map->Map[i+1][j+1].valeur == MINE)
        number++;


    if(number)
        return (char)('0' + number);
    else
        return ' ';
}

void fillmap(map_S *map) {
    printf("param%ctres :\n\tlignes : ",138);
    scanf("%u",&map->lines);
    printf("\tcolonnes : ");
    scanf("%u",&map->col);
    printf("\tmines : ");
    scanf("%u",&map->mines);

    if(alloc(map) == false) {
        printf("Erreur : La carte n'a pas pu %ctre gener%ce !",136,130);
        getchar();
        exit(2);
    }

    for(uint i = 0; i < map->lines; ++i) {
        for(uint j = 0; j < map->col; ++j) {
            map->Map[i][j].visible = false;
            map->Map[i][j].flagged = false;
        }
    }
    map->drapeaux = 0;
}

void action(map_S *map) {
    int tmp;
    printf("Que voulez-vous faire ?\n\t1) Abandonner\n\t2) Poser un drapeau\n\t3) Enlever un drapeau\n\t4) Explorer une case\n\t5) Quitter\n\t");
    scanf("%d",&tmp);
    if(tmp == 1)
        Abandon();
    else if(tmp == 2)
        Flag(map);
    else if(tmp == 3)
        Unflag(map);
    else if(tmp == 4)
        Explore(map);
    else if(tmp == 5)
        Quit(map);
#if defined CHEAT_ALLOWED
    else if(tmp == 16012006)
        affiche_map_Debug(map);
#endif
}

void Abandon(void) {
    printf("Merci d'avoir jou%c. Dommage d'avoir abandonn%c...",130,130);
    getchar();
    exit(EXIT_SUCCESS);
}

void Explore(map_S *map) {
    Coordonnees_S coord;
    getCoord(&coord,map);

    if(map->Map[coord.x][coord.y].flagged == true)
        printf("Cette case a un drapeau...\n");
    else
        if(map->Map[coord.x][coord.y].valeur == MINE) {
            printf("\n\tIl y a une mine ici... Dommage...\n\n");
            affiche_map_Debug(map);
            getchar();
            exit(EXIT_SUCCESS);
        }
    else
        Explore2(map,coord.x,coord.y);
}

void Explore2(map_S *map, uint x, uint y) {
    if(map->Map[x][y].flagged == false) {
        if(map->Map[x][y].visible == false) {
            map->Map[x][y].visible = true;
            if(map->Map[x][y].valeur == VIDE) {
                if(x > 0 && y > 0)
                    if(map->Map[x-1][y-1].valeur != VIDE)
                        Explore2(map,x-1,y-1);
                if(x > 0)
                    Explore2(map,x-1,y);
                if(x > 0 && y < map->col - 1)
                    if(map->Map[x-1][y+1].valeur != VIDE)
                        Explore2(map,x-1,y+1);
                if(y > 0)
                    Explore2(map,x,y-1);
                if(y < map->col - 1)
                    Explore2(map,x,y+1);
                if(x < map->lines - 1 && y > 0)
                    if(map->Map[x+1][y-1].valeur != VIDE)
                        Explore2(map,x+1,y-1);
                if(x < map->lines - 1)
                    Explore2(map,x+1,y);
                if(x < map->lines - 1 && y < map->col - 1)
                    if(map->Map[x+1][y+1].valeur != VIDE)
                        Explore2(map,x+1,y+1);
            }
        }
    }
}

void Flag(map_S *map) {
    Coordonnees_S coord;
    getCoord(&coord,map);
    if(map->Map[coord.x][coord.y].visible == false)
        map->Map[coord.x][coord.y].flagged = true;
    if(map->Map[coord.x][coord.y].valeur == MINE)
        map->drapeaux++;

}

void Unflag(map_S *map) {
    Coordonnees_S coord;
    getCoord(&coord,map);

    map->Map[coord.x][coord.y].flagged = false;
    if(map->Map[coord.x][coord.y].valeur == MINE)
        map->drapeaux++;
}

void getCoord(Coordonnees_S *coord, const map_S *map) {
    printf("Entrez x : ");
    scanf("%d",&coord->y);
    printf("Entrez y : ");
    scanf("%d",&coord->x);
    if(coord->x > map->lines - 1)
        getCoord(coord,map);
    if(coord->y > map->col - 1)
        getCoord(coord,map);
}

void affiche_map_Debug(const map_S *map) {
    putchar('+');
    for(uint8_t i = 0; i < map->col; ++i)
        printf("---+");
    putchar('\n');
    for(uint8_t i = 0; i < map->lines; ++i) {
        putchar('|');
        for(uint8_t j = 0; j < map->col; ++j) {
            if(map->Map[i][j].valeur == MINE) {
#if defined _WIN32 ||defined _WIN64
                Color(BACKGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY);
                printf(" X ");
                ResetColor();
                putchar('|');
#else
                printf(" X |");
#endif
            }
            else {
#if defined _WIN32 || defined _WIN64
                if(map->Map[i][j].valeur == '1')
                    Color(FOREGROUND_BLUE |FOREGROUND_INTENSITY);
                else if(map->Map[i][j].valeur == '2')
                    Color(FOREGROUND_GREEN|FOREGROUND_INTENSITY);
                else if(map->Map[i][j].valeur == '3')
                    Color(FOREGROUND_RED|FOREGROUND_INTENSITY);
                else if(map->Map[i][j].valeur == '4')
                    Color(FOREGROUND_BLUE|FOREGROUND_GREEN);
                else if(map->Map[i][j].valeur == '5')
                    Color(FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY);
                else if(map->Map[i][j].valeur == '6')
                    Color(FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY);
                else if(map->Map[i][j].valeur == '7')
                    Color(FOREGROUND_RED);
                else if(map->Map[i][j].valeur == '8')
                    Color(FOREGROUND_BLUE|FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
                printf(" %c ",map->Map[i][j].valeur);
                ResetColor();
                putchar('|');
#else
                    printf(" %c |"n,map->Map[i][j].valeur);
#endif
            }
        }
        putchar('\n');
        putchar('+');
        for(uint8_t z = 0; z < map->col; ++z)
            printf("---+");
        putchar('\n');
    }
    clean();
    printf("\n");
}
#if defined _WIN32 || defined _WIN64
void Color(int flags) {
    HANDLE H=GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(H,(WORD)flags);
}

void ResetColor(void) {
    Color(FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED);
}
#endif

bool gagne(const map_S *map) {
    bool ret = false;
    if(map->drapeaux == map->mines) {
        ret = true;
        for(uint i = 0 ; i < map->lines; ++i)
            for(uint j = 0; j < map-> col; ++j)
                if(map->Map[i][j].valeur == MINE)
                    if(map->Map[i][j].flagged == false)
                        ret = false;
    }
    else if(nb_unvisible(map) == map->mines)
        ret = true;
    return ret;
}

void ajoutermine(map_S *map, uint x, uint y) {
    map->Map[x][y].valeur = MINE;
    if(x > 0 && y > 0 && map->Map[x-1][y-1].valeur != MINE)
        map->Map[x-1][y-1].valeur++;
    if(x > 0 && map->Map[x-1][y].valeur != MINE)
        map->Map[x-1][y].valeur++;
    if(x > 0 && y < map->col - 1 && map->Map[x-1][y+1].valeur != MINE)
        map->Map[x-1][y+1].valeur++;
    if(y > 0 && map->Map[x][y-1].valeur != MINE)
        map->Map[x][y-1].valeur++;
    if(y < map->col - 1 && map->Map[x][y+1].valeur != MINE)
        map->Map[x][y+1].valeur++;
    if(x < map->lines - 1 && y > 0 && map->Map[x+1][y-1].valeur != MINE)
        map->Map[x+1][y-1].valeur++;
    if(x < map->lines - 1 && map->Map[x+1][y].valeur != MINE)
        map->Map[x+1][y].valeur++;
    if(x < map->lines - 1 && y < map->col - 1 && map->Map[x+1][y+1].valeur != MINE)
        map->Map[x+1][y+1].valeur++;
}

void Quit(const map_S *map) {
    int save;
    printf("\n\tVoulez-vous sauvegarder ? (1/0)\n\t");
    scanf("%d",&save);
    if(save == 1)
        Sauvegarder(map);
    printf("\n\t\t\tAu revoir");
    getchar();
    exit(EXIT_SUCCESS);
}

void Sauvegarder(const map_S *map) {
    FILE *f = fopen("save.sav","w+");
    if(f == NULL)
        exit(0x20);
    fprintf(f,"%2d%2d%2d\n",map->lines,map->col,map->mines);

    for(uint i = 0; i < map->lines; ++i) {
        for(uint j = 0; j < map->col; ++j) {
            if(map->Map[i][j].valeur == MINE && map->Map[i][j].flagged == true)
                fprintf(f,"-1");
            else if(map->Map[i][j].valeur == MINE && map->Map[i][j].flagged == false)
                fprintf(f,"-2");
            else if(map->Map[i][j].valeur != MINE && map->Map[i][j].flagged == true)
                fprintf(f,"9%1d", map->Map[i][j].valeur - '0');
            else
                fprintf(f,"%2d",map->Map[i][j].valeur);
        }
        fputc('\n',f);
    }

    for(uint i = 0; i < map->lines; ++i) {
        for(uint j = 0; j < map->col; ++j)
            fprintf(f,"%d",(map->Map[i][j].visible));
        fputc('\n',f);
    }
    fclose(f);
}

bool charger(map_S *map) {
    FILE *f = fopen("save.sav","r");
    if(f == NULL)
        return false;
    fscanf(f,"%2d%2d%2d\n",&map->lines, &map->col,&map->mines);
    alloc(map);

    for(uint i = 0; i < map->lines; ++i) {
        for(uint j = 0; j < map->col; ++j) {
            int tmp;
            fscanf(f,"%2d",&tmp);
            if(tmp == -1) {
                map->Map[i][j].valeur = MINE;
                map->Map[i][j].flagged = true;
            } else if(tmp == -2) {
                map->Map[i][j].valeur = MINE;
                map->Map[i][j].flagged = false;
            } else if(tmp / 10 == 9) {
                map->Map[i][j].valeur = (tmp % 10) + '0';
                map->Map[i][j].flagged = true;
            } else {
                map->Map[i][j].valeur = tmp;
                map->Map[i][j].flagged = false;
            }
        }
        fgetc(f);
    }
    fscanf(f,"\n");
    for(uint i = 0; i < map->lines; ++i) {
        for(uint j = 0; j < map->col; ++j) {
            int tmp;
            fscanf(f,"%d",&tmp);
            map->Map[i][j].visible = (bool)tmp;
        }
        fgetc(f);
    }
    fclose(f);
    remove("save.sav");
    return true;
}

uint nb_unvisible(const map_S *map) {
    uint n = 0;
    for(uint i = 0; i < map->lines; ++i)
        for(uint j = 0; j < map->col; ++j)
            if(map->Map[i][j].visible == false)
                ++n;
    return n;
}

void clean(void) {
    while(getchar()!='\n');
}

