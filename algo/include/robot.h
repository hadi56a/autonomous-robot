#ifndef ROBOT_H
#define ROBOT_H

#include <stdbool.h>

#include "ListeChaineeListe.h"

#include "labyrinthe.h"

/**
 * @brief Orientations cardinales possibles du robot.
 */
typedef enum {
    NORD, /**< Vers le haut. */
    EST,  /**< Vers la droite. */
    SUD,  /**< Vers le bas. */
    OUEST /**< Vers la gauche. */
} direction_t;

/**
 * @brief Actions elementaires qu'un robot peut effectuer.
 */
typedef enum{
    RIEN, /**< Aucun mouvement (etat initial). */
    AV,   /**< Avancer tout droit. */
    TG,   /**< Tourner a gauche. */
    TD    /**< Tourner a droite. */
}robot_state_t;

/**
 * @brief Representation d'un robot avec position et orientation.
 */
typedef struct robot{
    int position;
    direction_t direction;
} robot_t;

/**
 * @brief Cree un robot a une position et orientation initiales.
 * @param position identifiant de case du labyrinthe.
 * @param direction orientation initiale du robot.
 * @return pointeur vers le robot alloue.
 */
robot_t* creerRobot(int position, direction_t direction);

/**
 * @brief Donne la position courante du robot.
 * @param robot robot concerne.
 * @return identifiant de case du labyrinthe.
 */
int obtenirPosition(robot_t* robot);

/**
 * @brief Met a jour la position du robot.
 * @param robot robot concerne.
 * @param position nouvelle case dans le labyrinthe.
 */
void definirPosition(robot_t* robot, int position);

/**
 * @brief Donne la direction courante du robot.
 * @param robot robot concerne.
 * @return orientation cardinale actuelle.
 */
direction_t obtenirDirection(robot_t* robot);

/**
 * @brief Tourne le robot de 90 degres vers la gauche.
 * @param robot robot concerne.
 */
void tournerGauche(robot_t* robot);

/**
 * @brief Tourne le robot de 90 degres vers la droite.
 * @param robot robot concerne.
 */
void tournerDroite(robot_t* robot);

/**
 * @brief Verifie si la case devant le robot existe dans la grille.
 * @param robot robot concerne.
 * @param laby labyrinthe de reference.
 * @return true si la case est dans les bornes, false sinon.
 */
bool caseDevantExiste(robot_t* robot, labyrinthe_t* laby);

/**
 * @brief Calcule l'identifiant de la case devant le robot.
 * @param robot robot concerne.
 * @param laby labyrinthe de reference.
 * @return identifiant de la case en face selon la direction.
 */
int caseSuivante(robot_t* robot, labyrinthe_t* laby);

/**
 * @brief Indique si un mouvement en avant est possible.
 * @param robot robot concerne.
 * @param laby labyrinthe de reference.
 * @return true si la case suivante est valide et reliee par un chemin.
 */
bool peutAvancer(robot_t* robot, labyrinthe_t* laby);

/**
 * @brief Indique si le robot est actuellement sur un point de transit.
 * @param robot robot concerne.
 * @param laby labyrinthe de reference.
 * @return true si la case actuelle est marquee comme point de transit.
 */
bool estSurPointTransit(robot_t* robot, labyrinthe_t* laby);

/**
 * @brief Fait avancer le robot si le chemin est libre.
 * @param robot robot concerne.
 * @param laby labyrinthe de reference.
 * @return true si le mouvement a ete effectue, false sinon.
 */
bool avancer(robot_t* robot, labyrinthe_t* laby);

/**
 * @brief Indique si un virage a gauche est possible depuis la position courante.
 * @param robot robot concerne.
 * @param laby labyrinthe de reference.
 * @return true si la case a gauche est accessible.
 */
bool peutAllerGauche(robot_t* robot, labyrinthe_t* laby);

/**
 * @brief Indique si un virage a droite est possible depuis la position courante.
 * @param robot robot concerne.
 * @param laby labyrinthe de reference.
 * @return true si la case a droite est accessible.
 */
bool peutAllerDroite(robot_t* robot, labyrinthe_t* laby);

/**
 * @brief Calcule l'orientation apres un virage a gauche sans modifier le robot.
 * @param robot robot concerne.
 * @return orientation resultante.
 */
direction_t directionGauche(robot_t* robot);

/**
 * @brief Calcule l'orientation apres un virage a droite sans modifier le robot.
 * @param robot robot concerne.
 * @return orientation resultante.
 */
direction_t directionDroite(robot_t* robot);


/**
 * @brief Avance tout droit jusqu'a atteindre un virage ou une impasse.
 * @param robot robot a deplacer.
 * @param labyrinthe labyrinthe de reference.
 * @param positions tableau alloue contenant les positions visitees a chaque pas (a liberer par l'appelant).
 * @return nombre de pas effectues.
 */
int avancerJusqueVirage(robot_t* robot, labyrinthe_t* labyrinthe, int** positions);

/**
 * @brief Libere les ressources du robot.
 * @param robot robot a detruire.
 */
void libererRobot(robot_t* robot);

#endif
