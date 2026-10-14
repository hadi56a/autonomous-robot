#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

#include "robot_pathfinder.h"


int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s <fichier_entree> [fichier_sortie_commandes]\n", argv[0]);
        return 1;
    }

    FILE *f_entree = fopen(argv[1], "r");
    if (f_entree == NULL) {
        perror("Erreur lors de l'ouverture du fichier d'entrée");
        return 1;
    }

    FILE *f_sortie = NULL;
    if (argc == 3) {
        f_sortie = fopen(argv[2], "w");
        if (f_sortie == NULL) {
            perror("Erreur lors de l'ouverture du fichier de sortie");
            fclose(f_entree);
            return 1;
        }
    }

    labyrinthe_t* labyrinthe = NULL;
    robot_t* robot = NULL;

    if (!construire_labyrinthe(f_entree, &labyrinthe, &robot)) {
        fprintf(stderr, "Erreur lors de la construction du labyrinthe.\n");
        fclose(f_entree);
        fclose(f_sortie);
        return 1;
    }

    resultat_bfs_t resultat = recherche_BFS(labyrinthe, robot);
    if (!resultat.trouve) {
        fprintf(stderr, "Aucun chemin trouvé.\n");
    } else {
        afficher_actions(resultat.actions, stdout);
        if (f_sortie != NULL) {
            afficher_actions(resultat.actions, f_sortie);
        }
    }

    LCL_vider(&resultat.actions);
    libererRobot(robot);
    libererLabyrinthe(labyrinthe);
    fclose(f_entree);
    if (f_sortie != NULL) {
        fclose(f_sortie);
    }
    return 0;
}
