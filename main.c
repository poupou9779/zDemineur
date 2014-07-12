#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "prototypes.h"

int colors_tab[] = {FOREGROUND_BLUE|FOREGROUND_INTENSITY,
                    FOREGROUND_GREEN|FOREGROUND_INTENSITY,
                    FOREGROUND_RED|FOREGROUND_INTENSITY,
                    FOREGROUND_BLUE|FOREGROUND_GREEN,
                    FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY,
                    FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY,
                    FOREGROUND_RED,
                    FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY};
const int vx[] = {-1, -1, -1, 0, 0, +1, +1, +1},
          vy[] = {-1, 0, +1, -1, +1, -1, 0, +1};

int main(void) {
    struct map map;
    int i;

    srand((unsigned int)time(NULL));
    printf("** zDemineur **\n\n");
    if(!load(&map))
        fill_map(&map);

    map_gen(&map);

    while(!iswon(&map)) {
#if defined (_WIN32) || defined (_WIN64)
        system("cls");
#else
        printf("________________________________________________________\n");
#endif
        display_map(&map);
        action(&map);
    }
    printf("\nYou won ! Here is the completely visible map :\n");
    display_map_debug(&map);

    for(i = 0; i < map.rows; ++i)
        free(map.Map[i]);

    free(map.Map);
    return 0;
}

bool alloc(struct map *map) {
    int i;
    map->Map = malloc(map->rows * sizeof(*map->Map));
    if(map == NULL)
        exit(1);

    for(i = 0; i < map->rows; ++i) {
        map->Map[i] = malloc(map->columns * sizeof(**map->Map));
        if(map->Map[i] == NULL)
            return false;
    }
    return true;
}

void map_gen(struct map *map) {
    int i, j,
         x, y;
    for(i = 0; i < map->rows; ++i)
        for(j = 0; j < map->columns; ++j)
            map->Map[i][j].value = EMPTY;

    for(i = 0; i < map->mines; ++i) {
        do {
            x = (int)rand() % map->rows;
            y = (int)rand() % map->columns;
        } while(map->Map[x][y].value == MINE);

        add_mine(map, x, y);
    }
    for(i = 0; i < map->rows; ++i)
        for(j = 0; j < map->columns; ++j)
            if(map->Map[i][j].value != MINE && map->Map[i][j].value != ' ')
                map->Map[i][j].value += '0' - EMPTY;
}

void display_map(const struct map *map) {
    int i, j, z;
    printf("\n   ");
    for(i = 0; i < map->columns; ++i)
        printf(" %2d ", i);

    printf("\n   +");

    for(i = 0; i < map->columns; ++i)
        printf("---+");
    putchar('\n');

    for(i = 0; i < map->rows; ++i) {
        printf("%2d |", i);
        for(j = 0; j < map->columns; ++j) {
#if defined WIN
            if(!map->Map[i][j].flagged && !map->Map[i][j].visible) {
                color(FOREGROUND_INTENSITY|FOREGROUND_GREEN|BACKGROUND_BLUE);
                printf(" ? ");
                reset_color();
                putchar('|');
            } else {
                color(colors_tab[map->Map[i][j].value - '1']);
                printf(" %c ", map->Map[i][j].value);
                reset_color();
                putchar('|');
            }
#else
            if(map->Map[i][j].value == MINE)
                printf(" ? |");
            else
                printf(" %c |", map->Map[i][j].value);
#endif
            /*}*/
        }

        putchar('\n');
        printf("   +");

        for(z = 0; z < map->columns; ++z)
            printf("---+");
        putchar('\n');
    }
    printf("\n\n");
}

void fill_map(struct map *map) {
    int i, j;
    printf("parameters :\n\trows : ");
    scanf("%d", &map->rows);
    printf("\tcolumns : ");
    scanf("%d", &map->columns);
    printf("\tmines : ");
    scanf("%d", &map->mines);

    if(!alloc(map)) {
        printf("Error : Unable to generate the map\n");
        getchar();
        exit(2);
    }

    for(i = 0; i < map->rows; ++i)
        for(j = 0; j < map->columns; ++j)
            map->Map[i][j].visible = map->Map[i][j].flagged = false;
    map->flags = 0;
}

void action(struct map *map) {
    int tmp;
    printf("What do you want to do ?\n"
               "\t1) Abandon\n"
               "\t2) Set a flag\n"
               "\t3) Erase a flag\n"
               "\t4) Explore a cell\n"
               "\t5) Quit\n\t");
    scanf("%d", &tmp);
    if(tmp == 1)
        abandon();
    else if(tmp == 2)
        flag(map);
    else if(tmp == 3)
        unflag(map);
    else if(tmp == 4)
        explore(map);
    else if(tmp == 5)
        quit(map);
#if defined CHEAT_ALLOWED
    else if(tmp == 16012006)
        display_map_debug(map);
#endif
}

void abandon(void) {
    printf("Thanks for playing.");
    getchar();
    exit(EXIT_SUCCESS);
}

void explore(struct map *map) {
    struct coord coord;
    get_coord(&coord, map);

    if(map->Map[coord.x][coord.y].flagged == true)
        printf("This cell has a flag...\n");
    else if(map->Map[coord.x][coord.y].value == MINE) {
            printf("\n\tThis cell is mined... Too bad...\n\n");
            display_map_debug(map);
            getchar();
            exit(EXIT_SUCCESS);
    } else
        explore_cell(map, coord.x, coord.y);
}

void explore_cell(struct map *map, int x, int y) {
    int i;
    if(!map->Map[x][y].flagged && !map->Map[x][y].visible) {
        map->Map[x][y].visible = true;
        if(map->Map[x][y].value == EMPTY)
            for(i = 0; i < 8; ++i)
                if((x+vx[i] >= 0 && x+vx[i] < map->rows)
                && (y+vy[i] >= 0 && y+vy[i] < map->columns))
                    explore_cell(map, x+vx[i], y+vy[i]);
    }
}

void flag(struct map *map) {
    struct coord coord;
    get_coord(&coord, map);
    if(!map->Map[coord.x][coord.y].visible)
        map->Map[coord.x][coord.y].flagged = true;
    if(map->Map[coord.x][coord.y].value == MINE)
        ++map->flags;

}

void unflag(struct map *map) {
    struct coord coord;
    get_coord(&coord, map);

    map->Map[coord.x][coord.y].flagged = false;
    if(map->Map[coord.x][coord.y].value == MINE)
        --map->flags;
}

void get_coord(struct coord *coord, const struct map *map) {
    printf("Enter x : ");
    scanf("%d", &coord->y);
    printf("Enter y : ");
    scanf("%d", &coord->x);
    if(coord->x > map->rows - 1)
        get_coord(coord, map);
    if(coord->y > map->columns - 1)
        get_coord(coord, map);
}

void display_map_debug(const struct map *map) {
    int i, j, z;
    putchar('+');
    for(i = 0; i < map->columns; ++i)
        printf("---+");
    putchar('\n');
    for(i = 0; i < map->rows; ++i) {
        putchar('|');
        for(j = 0; j < map->columns; ++j) {
#if defined WIN
            if(map->Map[i][j].value == MINE) {
                color(BACKGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY);
                printf(" X ");
                reset_color();
                putchar('|');
            } else {
                color(colors_tab[map->Map[i][j].value - '1']);
                printf(" %c ", map->Map[i][j].value);
                reset_color();
                putchar('|');
            }
#else
            if(map->Map[i][j].value == MINE)
                printf(" X |");
            else
                printf(" %c |", map->Map[i][j].value);
#endif
        }
        putchar('\n');
        putchar('+');
        for(z = 0; z < map->columns; ++z)
            printf("---+");
        putchar('\n');
    }
    purge_buffer();
    printf("\n");
}

#if defined WIN

void color(int flags) {
    HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(H, (WORD)flags);
}

void reset_color(void) { color(FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED); }

#endif

bool iswon(const struct map *map) {
    bool ret = false;
    int i, j;
    if(map->flags == map->mines) {
        ret = true;
        for(i = 0 ; i < map->rows; ++i)
            for(j = 0; j < map-> columns; ++j)
                if(map->Map[i][j].value == MINE && !map->Map[i][j].flagged)
                    ret = false;
    }
    else if(nb_unvisible(map) == map->mines)
        ret = true;
    return ret;
}

void add_mine(struct map *map, int x, int y) {
    int i = 0;

    map->Map[x][y].value = MINE;
    for(i = 0; i < 8; ++i)
        if((x+vx[i] >= 0 && x+vx[i] < map->rows)
        && (y+vy[i] >= 0 && y+vy[i] < map->columns)
        && map->Map[x+vx[i]][y+vy[i]].value != MINE)
            ++map->Map[x+vx[i]][y+vy[i]].value;
}

void quit(const struct map *map) {
    int bool_save;
    printf("\n\tDo you want to save ? (1/0)\n\t");
    scanf("%d", &bool_save);
    if(bool_save == 1)
        save(map);
    printf("\n\t\t\tGoodbye");
    getchar();
    exit(EXIT_SUCCESS);
}

void save(const struct map *map) {
    int i, j;
    FILE *f = fopen("save.sav", "w+");
    if(f == NULL)
        exit(0x20);
    fprintf(f, "%2d%2d%2d\n", map->rows, map->columns, map->mines);

    for(i = 0; i < map->rows; ++i) {
        for(j = 0; j < map->columns; ++j) {
            if(map->Map[i][j].value == MINE && map->Map[i][j].flagged == true)
                fprintf(f, "-1");
            else if(map->Map[i][j].value == MINE && !map->Map[i][j].flagged)
                fprintf(f, "-2");
            else if(map->Map[i][j].value != MINE && map->Map[i][j].flagged == true)
                fprintf(f, "9%c", map->Map[i][j].value);
            else
                fprintf(f, "%2d", map->Map[i][j].value);
        }
        fputc('\n', f);
    }

    for(i = 0; i < map->rows; ++i) {
        for(j = 0; j < map->columns; ++j)
            fprintf(f, "%d", (map->Map[i][j].visible));
        fputc('\n', f);
    }
    fclose(f);
}

bool load(struct map *map) {
    int i, j;
    FILE *f = fopen("save.sav", "r");
    if(f == NULL)
        return false;
    fscanf(f, "%2d%2d%2d\n", &map->rows, &map->columns, &map->mines);
    alloc(map);

    for(i = 0; i < map->rows; ++i) {
        for(j = 0; j < map->columns; ++j) {
            int tmp;
            fscanf(f, "%2d", &tmp);
            if(tmp == -1) {
                map->Map[i][j].value = MINE;
                map->Map[i][j].flagged = true;
            } else if(tmp == -2) {
                map->Map[i][j].value = MINE;
                map->Map[i][j].flagged = false;
            } else if(tmp / 10 == 9) {
                map->Map[i][j].value = (tmp % 10) + '0';
                map->Map[i][j].flagged = true;
            } else {
                map->Map[i][j].value = tmp;
                map->Map[i][j].flagged = false;
            }
        }
        fgetc(f);
    }
    fscanf(f, "\n");
    for(i = 0; i < map->rows; ++i) {
        for(j = 0; j < map->columns; ++j) {
            int tmp;
            fscanf(f, "%d", &tmp);
            map->Map[i][j].visible = (bool)tmp;
        }
        fgetc(f);
    }
    fclose(f);
    remove("save.sav");
    return true;
}

int nb_unvisible(const struct map *map) {
    int n = 0,
         i, j;
    for(i = 0; i < map->rows; ++i)
        for(j = 0; j < map->columns; ++j)
            if(!map->Map[i][j].visible)
                ++n;
    return n;
}

void purge_buffer(void) { while(getchar()!='\n'); }

