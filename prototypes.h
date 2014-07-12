#ifndef PROTOTYPES_H_INCLUED
#define PROTOTYPES_H_INCLUED
# if defined (_WIN32) || defined (_WIN64)
#  define WIN
#  include <windows.h>
# endif
# define CHEAT_ALLOWED
# define EMPTY    ' '
# define MINE    9

# define true 1
# define false 0

    typedef int bool;

    struct cell {
        bool visible,   /*if(!visible) print '?'*/
             flagged;
        char value;     /*contains the number of adjacent mines (= 9 if cell is a mine)*/
    };

    struct map {
        int rows,
            columns,
            mines,
            flags;
        struct cell  **Map;
    };

    struct coord {
        int x, y;
    };


    /*Allocates memory for the grid*/
    bool alloc(struct map *map);

    /*generates the cells (MINE or EMPTY)*/
    void map_gen(struct map *map);

    /*Sets the mine and adds 1 to each adjacent cell*/
    void add_mine(struct map *map, int x, int y);

	/*explicit*/
    void display_map(const struct map *map);

    /*explicit*/
    void fill_map(struct map *map);

    /*asks and handles what action to do*/
    void action(struct map *map);

    /*explicit*/
    void abandon(void) __attribute__((noreturn));

    /*explicit*/
    void flag(struct map *map);

    /*explicit*/
    void unflag(struct map *map);

    /*asks the cell to explore with explore_cell*/
    void explore(struct map *map);

    /*explores a cell and its adjacents to check for bombs and EMPTY-cells*/
    void explore_cell(struct map *map, int x, int y);

    /*asks a (X,Y) value*/
    void get_coord(struct coord *coord, const struct map *map);

    /*explicit*/
    void display_map_debug(const struct map *map);

# if defined WIN
    /*sets a color for console text writing*/
    void color(int flags);

    /*explicit*/
    void reset_color(void);
# endif

    /*returns true if every mine is discovered*/
    bool iswon(const struct map *map);

    /*ask if user wants to save and then leaves*/
    void quit(const struct map *map) __attribute__((noreturn));

    /*explicit*/
    void save(const struct map *map);

    /*explicit*/
    bool load(struct map *map);

    /*explicit*/
    void purge_buffer(void);

    /*explicit*/
    int nb_unvisible(const struct map *map);

#endif
