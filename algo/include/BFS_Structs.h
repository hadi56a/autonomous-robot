#ifndef BFS_STRUCTS_H
#define BFS_STRUCTS_H

#include "robot.h"

/**
 * @brief Etat minimal du robot pendant la recherche BFS.
 *
 * Encode la case courante, l'orientation, le masque des points de transit visites et
 * la derniere action effectuee. Ce format compact facilite l'indexation dans les
 * tableaux de visite.
 */
typedef struct {
    int case_lab;
    direction_t orientation;
    int masque;
    robot_state_t derniere_action;
} etat_robot_courant_t;

/**
 * @brief Transition generee depuis un etat courant.
 *
 * Associe une action elementaire au nouvel etat obtenu apres l'avoir appliquee.
 */
typedef struct{
    robot_state_t action;
    etat_robot_courant_t etat_courant;
} successeur_robot_t;

/**
 * @brief Liste dynamique de successeurs d'un etat.
 *
 * Contient un tableau alloue dynamiquement et le nombre d'elements valides.
 */
typedef struct{
    successeur_robot_t* successeurs;
    int nb_successeurs;
} liste_successeurs_robot_t;

/**
 * @brief Informations de visite pour reconstruire le chemin BFS.
 *
 * Stocke la distance depuis la racine, l'indice du parent et l'action empruntee.
 */
typedef struct {
    int distance;
    int indice_parent;
    robot_state_t action_parent;
} info_visite_t;

/**
 * @brief Resultat d'une recherche BFS sur le labyrinthe.
 *
 * Fournit la liste ordonnee des actions et un indicateur de succes.
 */
typedef struct {
    LCL_Liste actions;
    bool trouve;
} resultat_bfs_t;

#endif
