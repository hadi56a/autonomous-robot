#include "robot.h"
#include "labyrinthe.h"
#include <stdio.h>

robot_t* creerRobot(int position, direction_t direction){
    robot_t* robot = malloc(sizeof(robot_t));
    robot->position = position;
    robot->direction = direction;
    return robot;
}

int obtenirPosition(robot_t *robot){
    return robot->position;
}

void definirPosition(robot_t *robot, int position){
    robot->position = position;
}

direction_t obtenirDirection(robot_t *robot){
    return robot->direction;
}

void tournerGauche(robot_t *robot){
    robot->direction = (direction_t)((robot->direction + 3) % 4);
}

void tournerDroite(robot_t *robot){
    robot->direction = (direction_t)((robot->direction + 1) % 4);
}

bool caseDevantExiste(robot_t* robot, labyrinthe_t* laby){
    bool estValide = (obtenirDirection(robot) == NORD && obtenirPosition(robot) > obtenirTaille(laby)) ||
                     (obtenirDirection(robot) == EST && obtenirPosition(robot) % obtenirTaille(laby) != 0) ||
                     (obtenirDirection(robot) == SUD && obtenirPosition(robot) <= obtenirTaille(laby)*(obtenirTaille(laby)-1)) ||
                     (obtenirDirection(robot) == OUEST && obtenirPosition(robot) % obtenirTaille(laby) != 1);
    return estValide;
}

int caseSuivante(robot_t *robot, labyrinthe_t *laby){
    int prochainePosition = obtenirPosition(robot);
    switch(obtenirDirection(robot)){
        case NORD:
            prochainePosition -= obtenirTaille(laby);
            break;
        case EST:
            prochainePosition += 1;
            break;
        case SUD:
            prochainePosition += obtenirTaille(laby);
            break;
        case OUEST:
            prochainePosition -= 1;
            break;
    }
    return prochainePosition;
}

bool peutAvancer(robot_t *robot, labyrinthe_t *laby){
    return caseDevantExiste(robot, laby) && estPassage(laby, obtenirPosition(robot), caseSuivante(robot, laby));
}

bool avancer(robot_t *robot, labyrinthe_t *laby){
    int prochainePosition = caseSuivante(robot, laby);
    if(peutAvancer(robot, laby)){
        robot->position = prochainePosition;
        return true;
    }else{
        fprintf(stderr, "Le robot est bloque, mouvement illegal : %d -> %d\n", obtenirPosition(robot), prochainePosition);
        return false;
    }
}

bool estSurPointTransit(robot_t *robot, labyrinthe_t *laby){
    return estPointTransit(laby, obtenirPosition(robot));
}

bool peutAllerGauche(robot_t *robot, labyrinthe_t *laby){
    tournerGauche(robot);
    bool possible = peutAvancer(robot, laby);
    tournerDroite(robot);
    return possible;
}

bool peutAllerDroite(robot_t *robot, labyrinthe_t *laby){
    tournerDroite(robot);
    bool possible = peutAvancer(robot, laby);
    tournerGauche(robot);
    return possible;
}

direction_t directionGauche(robot_t* robot){
    tournerGauche(robot);
    direction_t dirGauche = obtenirDirection(robot);
    tournerDroite(robot);
    return dirGauche;
}

direction_t directionDroite(robot_t* robot){
    tournerDroite(robot);
    direction_t dirDroite = obtenirDirection(robot);
    tournerGauche(robot);
    return dirDroite;
}

int avancerJusqueVirage(robot_t* robot, labyrinthe_t* labyrinthe, int** positions){
    int capacite = 4;
    int nb_pas = 0;
    int* chemin = malloc(capacite * sizeof(int));
    if(chemin == NULL){
        *positions = NULL;
        return 0;
    }

    while (peutAvancer(robot, labyrinthe)) {
        avancer(robot, labyrinthe);

        if (nb_pas >= capacite) {
            capacite *= 2;
            int* temp = realloc(chemin, capacite * sizeof(int));
            if (temp == NULL) {
                free(chemin);
                *positions = NULL;
                return 0;
            }
            chemin = temp;
        }

        chemin[nb_pas++] = obtenirPosition(robot);

        if (obtenirDegre(labyrinthe, obtenirPosition(robot)) != 2) {
            break;
        }
    }

    *positions = chemin;
    return nb_pas;
}

void libererRobot(robot_t *robot){
    free(robot);
}
