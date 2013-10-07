#ifndef PROTOTYPES_H_INCLUED
#define PROTOTYPES_H_INCLUED
    #if defined (_WIN32) || defined (_WIN64)
        #include <windows.h>
    #endif
    #define CHEAT_ALLOWED
    #define VIDE    32
    #define MINE    9

    typedef unsigned int uint;

    typedef struct {
        bool    visible,  //if(!visible) print '?'
                flagged;
        char    valeur; //nombre de mines aux alentours ( 9 pour MINE)
    } Case_S;

    typedef struct {
        uint    lines,
                col,
                mines,
                drapeaux;
        Case_S  **Map;
    } map_S;

    typedef struct {
        uint    x,
                y;
    } Coordonnees_S;


    /*Alloue la mémoire de la grille*/
    bool alloc(map_S *map);

    /*Génère les cases (VIDE || MINE)*/
    void mapGen(map_S *map);

    /*Pose la mine, et ajoute 1 à la valeur des cases adjacentes*/
    void ajoutermine(map_S *map, uint x, uint y);

    /*Affiche la map*/
    void affiche_map(const map_S *map);

    /*retourne le nombre de mines adjacentes*/
    char numb_adj_mines(const map_S *map, uint i, uint j);

    /*remplit la structure map*/
    void fillmap(map_S *map);

    /*demande à l'utilisateur l'action à faire*/
    void action(map_S *map);

    /*Dit au revoir et quitte*/
    void Abandon(void);

    /*Pose un drapeau*/
    void Flag(map_S *map);

    /*Enlève un drapeau*/
    void Unflag(map_S *map);

    /*demande la case à explorer et l'envoie à Explore2();*/
    void Explore(map_S *map);

    /*Explore une case pour voir s'il y a une bombe*/
    void Explore2(map_S *map, uint x, uint y);

    /*Renvoie une valeur de x && de y*/
    void getCoord(Coordonnees_S *coord, const map_S *map);

    /*Affiche la carte entièrement résolue*/
    void affiche_map_Debug(const map_S *map);

#if defined _WIN32 || defined _WIN64
    /*Change la couleur du texte à afficher dans la console*/
    void Color(int flags);

    /*Remet la couleur d'écriture en blanc*/
    void ResetColor(void);
#endif

    /*renvoie true si toutes les mines ont un drapeau*/
    bool gagne(const map_S *map);

    /*Demande si l'utilisateur veut sauvegarder et quitte*/
    void Quit(const map_S *map);

    /*Sauvegarde la partie dans un fichier*/
    void Sauvegarder(const map_S *map);

    /*charge la sauvegarde save.sav*/
    bool charger(map_S *map);

    /*nettoie le buffer*/
    void clean(void);

    /*retroune le nombre de case cachées sur la carte*/
    uint nb_unvisible(const map_S *map);

#endif //PROTOTYPES_H_INCLUED
