#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "ListeChaineeListe.h"
#include "BFS_Structs.h"
#include "copieLiberationComparaisonTypesSimples.h"
#include "ListeChaineeListe.h"
#include "copieLiberationComparaisonTypesSimples.h"

/**
 * @brief File FIFO basee sur une liste.
 */
typedef struct{
    LCL_Liste liste;
} File_t;

/**
 * @brief Initialise une file vide.
 * @param file pointeur vers la file a initialiser.
 */
void initialiserFile(File_t* file);

/**
 * @brief Ajoute un etat en fin de file.
 * @param file file cible.
 * @param valeur etat a enfiler (copie stockee).
 */
void enfiler(File_t* file, etat_robot_courant_t valeur);

/**
 * @brief Retire et retourne l'element en tete de file.
 * @param file file cible.
 * @return l'etat precedemment en tete (copie).
 */
etat_robot_courant_t defiler(File_t* file);

/**
 * @brief Indique si la file est vide.
 * @param file file cible.
 * @return true si aucun element n'est stocke.
 */
bool fileEstVide(File_t* file);

/**
 * @brief Libere les ressources associees a la file.
 * @param file file cible.
 */
void libererFile(File_t* file);

/**
 * @brief Copie d'un etat robot courant pour usage dans les listes chainees.
 * @param valeur pointeur vers l'etat source.
 * @return pointeur vers une copie allouee dynamiquement.
 */
void* copier_RCS(void* valeur);

/**
 * @brief Libere un etat robot courant precedemment copie.
 * @param valeur pointeur a liberer.
 */
void liberer_RCS(void* valeur);

#endif
