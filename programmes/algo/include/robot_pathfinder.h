#ifndef __ROBOT_PATHFINDER__
#define __ROBOT_PATHFINDER__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include "graphe.h"
#include "copieLiberationComparaisonTypesSimples.h"
#include "labyrinthe.h"
#include "robot.h"
#include "BFS_Structs.h"
#include "file.h"

#include "ListeChaineeListe.h"
#include "TableHachageDictionnaire.h"
#include "TableHachageEnsemble.h"

/**
 * @brief Calcule un indice lineaire pour adresser un etat dans un tableau.
 * @param e etat du robot (position, orientation, masque, derniere action).
 * @param nb_cases nombre total de cases du labyrinthe.
 * @param nb_masques nombre de combinaisons possibles de masques.
 * @return indice correspondant ou -1 si l'etat est invalide.
 */
int indexEtat(etat_robot_courant_t e, int nb_cases, int nb_masques);

/**
 * @brief Construit un labyrinthe et un robot depuis un fichier texte.
 * @param fichier flux deja ouvert en lecture.
 * @param labyrinthe pointeur de sortie vers le labyrinthe cree.
 * @param robot pointeur de sortie vers le robot cree.
 * @return true si la lecture et la creation reussissent.
 */
bool construire_labyrinthe(FILE *fichier, labyrinthe_t** labyrinthe, robot_t** robot);

/**
 * @brief Genere les successeurs possibles pour un etat en BFS.
 * @param labyrinthe labyrinthe de reference.
 * @param etat etat courant du robot.
 * @param points_transit tableau des points de transit.
 * @param nb_points_transit taille du tableau points_transit.
 * @return liste allouee dynamiquement contenant les successeurs.
 */
liste_successeurs_robot_t successeurs_BFS(labyrinthe_t* labyrinthe,
                                        etat_robot_courant_t etat,
                                        const int* points_transit,
                                        int nb_points_transit);

/**
 * @brief Effectue une recherche BFS pour traverser tous les points de transit.
 * @param labyrinthe labyrinthe dans lequel evoluer.
 * @param robot robot de depart (position et orientation initiales).
 * @return resultat contenant la liste d'actions et un indicateur de succes.
 */
resultat_bfs_t recherche_BFS(labyrinthe_t* labyrinthe, robot_t* robot);

/**
 * @brief Affiche une liste d'actions ligne par ligne dans un flux.
 * @param actions liste d'actions renvoyee par la recherche.
 * @param sortie flux cible (stdout si NULL).
 */
void afficher_actions(LCL_Liste actions, FILE* sortie);
#endif
