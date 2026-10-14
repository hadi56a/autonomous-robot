#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include "ListeChaineeListe.h" // Assurez-vous que ce fichier existe
#include <stdio.h>
#include <stdbool.h>

#include "graphe.h"
#include "TableHachageEnsemble.h"

/**
 * @brief Structure de donnees pour un labyrinthe carre.
 *
 * Contient un graphe orientant les arcs, un ensemble de points de transit et la taille du carre.
 */
typedef struct labyrinthe{
    G_Graphe *graphe;
    THE_Ensemble *ensemble;
    int taille;
} labyrinthe_t;

/**
 * @brief Construit un labyrinthe carre de taille taille x taille.
 * @param taille nombre de cases par cote.
 * @return un pointeur vers le labyrinthe alloue.
 */
labyrinthe_t* creerLabyrinthe(int taille);

/**
 * @brief Retourne la taille (nombre de cases par cote) du labyrinthe.
 * @param labyrinthe labyrinthe cible.
 */
int obtenirTaille(labyrinthe_t* labyrinthe);

/**
 * @brief Ajoute un chemin entre deux cases.
 * @param labyrinthe labyrinthe cible.
 * @param source identifiant de la case de depart.
 * @param destination identifiant de la case d'arrivee.
 */
void ajouterPassage(labyrinthe_t* labyrinthe, int source, int destination);

/**
 * @brief Supprime un chemin existant entre deux cases.
 * @param labyrinthe labyrinthe cible.
 * @param source identifiant de la case de depart.
 * @param destination identifiant de la case d'arrivee.
 */
void supprimerPassage(labyrinthe_t* labyrinthe, int source, int destination);

/**
 * @brief Marque une case comme point de transit.
 * @param labyrinthe labyrinthe cible.
 * @param source identifiant de la case a marquer.
 */
void ajouterPointTransit(labyrinthe_t* labyrinthe, int source);

/**
 * @brief Retire une case des points de transit.
 * @param labyrinthe labyrinthe cible.
 * @param source identifiant de la case a retirer.
 */
void supprimerPointTransit(labyrinthe_t* labyrinthe, int source);

/**
 * @brief Donne l'ensemble des points de transit.
 * @param labyrinthe labyrinthe cible.
 * @return pointeur vers l'ensemble interne a ne pas liberer.
 */
THE_Ensemble* obtenirPointsTransit(labyrinthe_t* labyrinthe);

/**
 * @brief Calcule les voisins adjacents d'une case.
 * @param labyrinthe labyrinthe cible.
 * @param source identifiant de la case de reference.
 * @return un ensemble nouvellement cree contenant les voisins.
 */
THE_Ensemble obtenirVoisins(labyrinthe_t* labyrinthe, int source);

/**
 * @brief Donne le degre (nombre de voisins) d'une case.
 * @param labyrinthe labyrinthe cible.
 * @param cell identifiant de la case.
 * @return nombre de voisins adjacents.
 */
int obtenirDegre(labyrinthe_t* labyrinthe, int cell);

/**
 * @brief Indique si un chemin direct existe entre deux cases.
 * @param labyrinthe labyrinthe cible.
 * @param source identifiant de la case de depart.
 * @param destination identifiant de la case d'arrivee.
 * @return true si le chemin existe, false sinon.
 */
bool estPassage(labyrinthe_t* labyrinthe, int source, int destination);

/**
 * @brief Verifie si une sequence de cases suit des chemins valides.
 * @param labyrinthe labyrinthe cible.
 * @param trajet liste chainee de cases representant le trajet.
 * @return true si chaque etape est reliee, false sinon.
 */
bool estTrajet(labyrinthe_t* labyrinthe, LCL_Liste* trajet);

/**
 * @brief Indique si une case est un point de transit.
 * @param labyrinthe labyrinthe cible.
 * @param source identifiant de la case.
 * @return true si la case est presente dans l'ensemble, false sinon.
 */
bool estPointTransit(labyrinthe_t* labyrinthe, int source);

/**
 * @brief Affiche le labyrinthe en representation textuelle.
 * @param labyrinthe labyrinthe cible.
 */
void afficherLabyrinthe(labyrinthe_t* labyrinthe);

/**
 * @brief Libere toutes les ressources associees a un labyrinthe.
 * @param labyrinthe labyrinthe a detruire.
 */
void libererLabyrinthe(labyrinthe_t* labyrinthe);


#endif
