/**
 * @file capteur_color.h
 * @brief Gestion du capteur de couleur TCS34725
 * @details Ce module permet d'initialiser, de lire les couleurs (rouge/vert) et d'obtenir les valeurs brutes RGB
 */

#ifndef CAPTEUR_COLOR_H
#define CAPTEUR_COLOR_H

/**
 * @brief Initialise le capteur de couleur
 * @return 1 en cas d'erreur
 */
int color_initialisation();

/**
 * @brief Lit la couleur détectée par le capteur
 * @param file Le descripteur de fichier du capteur
 * @return 
 * - 1 : Vert
 * - 2 : Rouge
 * - 0 : Indéterminé ou erreur
 */
int lire_couleur(int file);

/**
 * @brief Lit les valeurs brutes RGB et Clear du capteur
 * @param file Le descripteur de fichier du capteur
 * @param r Pointeur pour stocker la valeur rouge
 * @param g Pointeur pour stocker la valeur vert
 * @param b Pointeur pour stocker la valeur bleu
 * @param c Pointeur pour stocker la valeur clear
 */
void lire_valeurs_brutes(int file, int *r, int *g, int *b, int *c);

#endif