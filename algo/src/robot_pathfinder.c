#include "robot_pathfinder.h"

bool construire_labyrinthe(FILE *fichier, labyrinthe_t** labyrinthe, robot_t** robot) {
    if (fichier == NULL || labyrinthe == NULL || robot == NULL) {
        return false;
    }

    char ligne[256];
    int largeur;
    unsigned int id_depart;
    direction_t orientation_depart;

    if (fgets(ligne, sizeof(ligne), fichier) == NULL || sscanf(ligne, "%d", &largeur) != 1) {
        fprintf(stderr, "Erreur: Lecture de la largeur.\n");
        return false;
    }

    *labyrinthe = creerLabyrinthe(largeur);

    char orientation_char;
    if (fgets(ligne, sizeof(ligne), fichier) == NULL || sscanf(ligne, "%u%c", &id_depart, &orientation_char) != 2) {
        fprintf(stderr, "Erreur: Lecture du depart et orientation.\n");
        return false;
    }

    switch (orientation_char) {
        case 'N': orientation_depart = NORD; break;
        case 'E': orientation_depart = EST; break;
        case 'S': orientation_depart = SUD; break;
        case 'O': orientation_depart = OUEST; break;
        default: orientation_depart = NORD;
    }

    *robot = creerRobot(id_depart, orientation_depart);

    while (fgets(ligne, sizeof(ligne), fichier) != NULL && ligne[0] != '.') {
        if (strchr(ligne, '-') != NULL) {
            unsigned int u, v;
            if (sscanf(ligne, "%u-%u", &u, &v) == 2) {
                ajouterPassage(*labyrinthe, u, v);
                ajouterPassage(*labyrinthe, v, u);
            }
        } else {
            unsigned int point;
            if (sscanf(ligne, "%u", &point) == 1) {
                ajouterPointTransit(*labyrinthe, point);
            }
        }
    }

    return true;
}

static int indice_transit(const int* points, int nb_points, int element) {
    for (int i = 0; i < nb_points; i++) {
        if (points[i] == element) {
            return i;
        }
    }
    return -1;
}

static void* copier_etat_robot(void* valeur){
    robot_state_t* etat = (robot_state_t*) valeur;
    robot_state_t* copie = malloc(sizeof(robot_state_t));
    if (copie != NULL) {
        *copie = *etat;
    }
    return copie;
}

static void liberer_etat_robot(void* valeur){
    free(valeur);
}

liste_successeurs_robot_t successeurs_BFS(labyrinthe_t* labyrinthe, etat_robot_courant_t etat, const int* points_transit, int nb_points_transit){
    liste_successeurs_robot_t successeurs;
    successeurs.nb_successeurs = 0;
    successeurs.successeurs = NULL;

    robot_t* robot_etat = creerRobot(etat.case_lab, etat.orientation);
    int masque = etat.masque;
    robot_t* robot_avant = creerRobot(etat.case_lab, etat.orientation);
    int* positions_corridor = NULL;
    int nb_positions = avancerJusqueVirage(robot_avant, labyrinthe, &positions_corridor);

    /* capacité maximale : TG + TD + un seul AV (segment) */
    int capacite = 3;
    successeurs.successeurs = malloc(capacite * sizeof(successeur_robot_t));
    if (successeurs.successeurs == NULL) {
        free(positions_corridor);
        libererRobot(robot_avant);
        libererRobot(robot_etat);
        return successeurs;
    }
    if (nb_positions > 0) {
        int pos_finale = positions_corridor[nb_positions - 1];
        int masque_final = masque;
        for (int i = 0; i < nb_positions; i++) {
            int indice = indice_transit(points_transit, nb_points_transit, positions_corridor[i]);
            if (indice != -1) {
                masque_final |= (1 << indice);
            }
        }

        etat_robot_courant_t av_state = {
            .case_lab = pos_finale,
            .orientation = obtenirDirection(robot_avant),
            .masque = masque_final,
            .derniere_action = AV
        };
        successeurs.successeurs[successeurs.nb_successeurs++] = (successeur_robot_t){AV, av_state};
    }
    if (etat.derniere_action != TG) {
        etat_robot_courant_t tg_state = {
            .case_lab = etat.case_lab,
            .orientation = directionGauche(robot_etat),
            .masque = etat.masque,
            .derniere_action = TG
        };
        successeurs.successeurs[successeurs.nb_successeurs++] = (successeur_robot_t){TG, tg_state};
    }
    if (etat.derniere_action != TD) {
        etat_robot_courant_t td_state = {
            .case_lab = etat.case_lab,
            .orientation = directionDroite(robot_etat),
            .masque = etat.masque,
            .derniere_action = TD
        };
        successeurs.successeurs[successeurs.nb_successeurs++] = (successeur_robot_t){TD, td_state};
    }

    free(positions_corridor);
    libererRobot(robot_avant);
    libererRobot(robot_etat);
    successeurs.successeurs = realloc(successeurs.successeurs, successeurs.nb_successeurs * sizeof(successeur_robot_t));
    return successeurs;
}

int indexEtat(etat_robot_courant_t e, int nb_cases, int nb_masques){
    if (e.case_lab < 1 || e.case_lab > nb_cases) {
        return -1;
    }
    int pos = e.case_lab - 1;
    return ((pos * 4 + e.orientation) * nb_masques + e.masque) * 4 + e.derniere_action;
}

resultat_bfs_t recherche_BFS(labyrinthe_t* labyrinthe, robot_t* robot){
    THE_Ensemble* passages = obtenirPointsTransit(labyrinthe);
    LCL_Liste liste = THE_elements(*passages);
    int nb_passages = LCL_longueur(liste);

    int* points_transit = NULL;
    if (nb_passages > 0) {
            points_transit = malloc(sizeof(int) * nb_passages);
            if (points_transit == NULL) {
                LCL_vider(&liste);
                resultat_bfs_t res = {LCL_liste(copier_etat_robot, liberer_etat_robot), false};
                return res;
            }
        for (int i = 0; i < nb_passages; i++) {
            int* val = (int*) LCL_element(liste, i);
            points_transit[i] = *val;
            free(val);
        }
    }
    LCL_vider(&liste);

    File_t* file_r = malloc(sizeof(File_t));
    if (file_r == NULL) {
        free(points_transit);
        resultat_bfs_t res = {LCL_liste(copier_etat_robot, liberer_etat_robot), false};
        return res;
    }
    initialiserFile(file_r);

    int nb_cases_lab = obtenirTaille(labyrinthe) * obtenirTaille(labyrinthe);
    int nb_masques = 1 << nb_passages;
    int indice_max = nb_cases_lab * nb_masques * 16;
    info_visite_t *visites = malloc(sizeof(info_visite_t) * indice_max);
    if (visites == NULL) {
        free(points_transit);
        libererFile(file_r);
        resultat_bfs_t res = {LCL_liste(copier_etat_robot, liberer_etat_robot), false};
        return res;
    }

    for (int i = 0; i < indice_max; i++) {
        visites[i].distance = -1;   // -1 = non visite
        visites[i].indice_parent = -1;
        visites[i].action_parent = RIEN;
    }

    LCL_Liste actions = LCL_liste(copier_etat_robot, liberer_etat_robot);

    int pos_dep = obtenirPosition(robot);
    int masque = 0;

    int indice_transit_dep = indice_transit(points_transit, nb_passages, pos_dep);
    if (indice_transit_dep != -1) {
        masque |= (1 << indice_transit_dep);
    }

    etat_robot_courant_t etat_initial = {
        .case_lab = pos_dep,
        .orientation = obtenirDirection(robot),
        .masque = masque,
        .derniere_action = RIEN
    };

    enfiler(file_r, etat_initial);
    int indice_initial = indexEtat(etat_initial, nb_cases_lab, nb_masques);
    if (indice_initial < 0 || indice_initial >= indice_max) {
        free(points_transit);
        free(visites);
        libererFile(file_r);
        resultat_bfs_t res = {actions, false};
        return res;
    }
    visites[indice_initial].distance = 0;
    visites[indice_initial].indice_parent = -1;
    visites[indice_initial].action_parent = RIEN;

    int masque_complet = (1 << nb_passages) - 1;
    bool trouve = false;
    int indice_final = -1;

    while(!fileEstVide(file_r)){
        etat_robot_courant_t etat = defiler(file_r);
        int indice_etat = indexEtat(etat, nb_cases_lab, nb_masques);
        if (indice_etat < 0 || indice_etat >= indice_max) {
            continue;
        }
        if(etat.case_lab == pos_dep && etat.masque == masque_complet){
            trouve = true;
            indice_final = indice_etat;
            break;
        }

        liste_successeurs_robot_t successeurs = successeurs_BFS(labyrinthe, etat, points_transit, nb_passages);
        for(int i = 0; i < successeurs.nb_successeurs; i++){
            etat_robot_courant_t etat_suivant = successeurs.successeurs[i].etat_courant;
            int indice_suivant = indexEtat(etat_suivant, nb_cases_lab, nb_masques);
            if (indice_suivant < 0) {
                continue;
            }
            if(visites[indice_suivant].distance == -1){
                visites[indice_suivant].distance = visites[indice_etat].distance + 1;
                visites[indice_suivant].indice_parent = indice_etat;
                visites[indice_suivant].action_parent = successeurs.successeurs[i].action;
                enfiler(file_r, etat_suivant);
            }
        }
        free(successeurs.successeurs);
    }

    if (trouve) {
        int indice_courant = indice_final;
        int etapes = visites[indice_courant].distance;
        int capacite_pile = etapes;
        if (capacite_pile <= 0) {
            capacite_pile = 1;
        }
        robot_state_t* pile = malloc(sizeof(robot_state_t) * capacite_pile);
        int nb_actions = 0;

        while (indice_courant != -1 && visites[indice_courant].action_parent != RIEN) {
            pile[nb_actions++] = visites[indice_courant].action_parent;
            indice_courant = visites[indice_courant].indice_parent;
        }

        for (int i = nb_actions - 1; i >= 0; i--) {
            robot_state_t action = pile[i];
            LCL_inserer(&actions, &action, LCL_longueur(actions));
        }
        free(pile);
    }

    free(points_transit);
    free(visites);
    libererFile(file_r);

    resultat_bfs_t res = {actions, trouve};
    return res;
}

static const char* action_vers_chaine(robot_state_t action) {
    switch (action) {
        case AV: return "AV";
        case TG: return "TG";
        case TD: return "TD";
        case RIEN: default: return "RIEN";
    }
}

void afficher_actions(LCL_Liste actions, FILE* sortie) {
    if (sortie == NULL) {
        sortie = stdout;
    }

    for (int i = 0; i < LCL_longueur(actions); i++) {
        robot_state_t* act = (robot_state_t*) LCL_element(actions, i);
        if (act != NULL) {
            fprintf(sortie, "%s\n", action_vers_chaine(*act));
            free(act);
        }
    }
    fprintf(sortie, ".\n");
}
