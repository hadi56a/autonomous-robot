#include "labyrinthe.h"
#include "ListeChaineeListe.h"
#include "TableHachageEnsemble.h"
#include "copieLiberationComparaisonTypesSimples.h" // CLCTS_*
#include "graphe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// --- Fonctions de base du Labyrinthe ---

labyrinthe_t* creerLabyrinthe(int taille){
    labyrinthe_t* laby = malloc(sizeof(labyrinthe_t));
    if(laby == NULL){
        fprintf(stderr, "Erreur lors de l'allocation memoire pour le labyrinthe\n");
        exit(EXIT_FAILURE);
    }
    laby->taille = taille;
    
    laby->graphe = malloc(sizeof(G_Graphe));
    laby->ensemble = malloc(sizeof(THE_Ensemble));

    G_Graphe graphe = G_graphe(false, CLCTS_copierInt, CLCTS_libererInt, CLCTS_comparerInt, CLCTS_copierInt, CLCTS_libererInt, CLCTS_comparerInt);
    THE_Ensemble ensemble = THE_ensemble(CLCTS_copierInt, CLCTS_libererInt, CLCTS_comparerInt, sizeof(int));

    *(laby->graphe) = graphe;
    *(laby->ensemble) = ensemble;

    for (int i = 1; i <= taille*taille; i++) {
        int val = i;
        int resultat = G_ajouterSommet(laby->graphe, i, &val); 
        if (resultat == -1) {  
            fprintf(stderr, "Erreur lors de l'ajout du sommet %d au graphe (errno = %d)\n", i, errno);
            exit(EXIT_FAILURE);
        }
    }
    return laby;
}

int obtenirTaille(labyrinthe_t *labyrinthe){
    return labyrinthe->taille;
}

void ajouterPassage(labyrinthe_t *labyrinthe, int source, int destination){
    int resultat = G_ajouterArc(labyrinthe->graphe, source, destination, NULL);
    if(resultat != 0){
        fprintf(stderr, "Erreur lors de l'ajout de l'arc (%d, %d) au graphe\n", source, destination);
        exit(EXIT_FAILURE);
    }
}

void supprimerPassage(labyrinthe_t *labyrinthe, int source, int destination){
    if(!G_arcPresent(*(labyrinthe->graphe), source, destination)){
        fprintf(stderr, "Erreur lors de la suppression de l'arc (%d, %d) du graphe du labyrinthe, l'arc n'est pas present\n", source, destination);
        exit(EXIT_FAILURE);
    }
    int resultat = G_supprimerArc(labyrinthe->graphe, source, destination);
    if(resultat != 0){
        fprintf(stderr, "Erreur lors de la suppression de l'arc (%d, %d) du graphe\n", source, destination);
        exit(EXIT_FAILURE);
    }
}

void ajouterPointTransit(labyrinthe_t *labyrinthe, int source){
    int l_source = source; 
    THE_ajouter(labyrinthe->ensemble, &l_source);
}

void supprimerPointTransit(labyrinthe_t *labyrinthe, int source){
    int l_source = source;
    if(!THE_estPresent(*(labyrinthe->ensemble), &l_source)){
        fprintf(stderr, "Erreur lors de la suppression du point de transit (%d) de l'ensemble du labyrinthe, le point n'est pas present\n", source);
        exit(EXIT_FAILURE);
    }
    THE_supprimer(labyrinthe->ensemble, &l_source);
}

THE_Ensemble* obtenirPointsTransit(labyrinthe_t *labyrinthe){
    return labyrinthe->ensemble;
}

THE_Ensemble obtenirVoisins(labyrinthe_t *labyrinthe, int source){
    LCL_Liste liste = G_obtenirSommetsAdjacents(*(labyrinthe->graphe), source);
    THE_Ensemble voisins = THE_ensemble(CLCTS_copierInt, CLCTS_libererInt, CLCTS_comparerInt, sizeof(int));
    while(!LCL_estVide(liste)){
        int* voisin = (int*) LCL_element(liste, 0);
        THE_ajouter(&voisins, voisin);
        free(voisin);
        LCL_supprimer(&liste, 0); 
    }
    LCL_vider(&liste);
    return voisins;
}

int obtenirDegre(labyrinthe_t* labyrinthe, int cell){
    THE_Ensemble voisins = obtenirVoisins(labyrinthe, cell);
    int deg = THE_cardinalite(voisins);
    THE_vider(&voisins);
    return deg;
}

bool estPassage(labyrinthe_t *labyrinthe, int source, int destination){
    return G_arcPresent(*(labyrinthe->graphe), source, destination);
}

bool estTrajet(labyrinthe_t *labyrinthe, LCL_Liste* trajet){
    if(LCL_longueur(*trajet) == 0){
        return false;
    }
    for(int i = 0; i < LCL_longueur(*trajet) - 1; i++){
        int* courant = (int*) LCL_element(*trajet, i);
        int* suivant = (int*) LCL_element(*trajet, i + 1);

        bool chemin_existe = estPassage(labyrinthe, *courant, *suivant);

        free(courant);
        free(suivant);

        if(!chemin_existe){
            return false;
        }
    }
    return true;
}

bool estPointTransit(labyrinthe_t *labyrinthe, int source){
    return THE_estPresent(*(labyrinthe->ensemble), &source);
}

void afficherLabyrinthe(labyrinthe_t *labyrinthe){
    for(int i = 0; i < (2*obtenirTaille(labyrinthe))-1; i++){
        for(int j = 1; j <= obtenirTaille(labyrinthe); j++){
            int case_a = (i/2)*obtenirTaille(labyrinthe)+j;
            if(i % 2 == 0){
                if(estPointTransit(labyrinthe, case_a)){
                    printf("*%03d*", case_a);
                } else {
                    printf(" %03d ", case_a);
                }
                if(j < obtenirTaille(labyrinthe)){
                    if(estPassage(labyrinthe, case_a, case_a+1)){
                        printf("-");
                    } else {
                        printf(" ");
                    }
                }
            } else {
                if(estPassage(labyrinthe, case_a, case_a+obtenirTaille(labyrinthe))){
                    printf("  |  ");
                } else {
                    printf("     ");
                }
                if(j < obtenirTaille(labyrinthe)){
                    printf(" ");
                }
            }
        }
        printf("\n");
    }
}

void libererLabyrinthe(labyrinthe_t *labyrinthe){
    THE_vider(labyrinthe->ensemble);
    G_vider(labyrinthe->graphe);
    free(labyrinthe->ensemble);
    free(labyrinthe->graphe);
    free(labyrinthe);
}

// --- Fonction de Lecture Corrigée ---

labyrinthe_t* lireLabyrinthe(const char* filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Erreur ouverture fichier labyrinthe");
        return NULL;
    }

    char buffer[256];

    /* ---- lecture de la taille ---- */
    if (!fgets(buffer, sizeof(buffer), f)) {
        fprintf(stderr, "Erreur lecture taille\n");
        fclose(f);
        return NULL;
    }

    int taille = atoi(buffer);
    labyrinthe_t *lab = creerLabyrinthe(taille);
    if (!lab) {
        fclose(f);
        return NULL;
    }

    /* ---- lecture position initiale du robot (case + orientation) ---- */
    if (!fgets(buffer, sizeof(buffer), f)) {
        fprintf(stderr, "Erreur lecture position initiale\n");
        libererLabyrinthe(lab);
        fclose(f);
        return NULL;
    }

    /* Extraction des données de départ (ignorées dans cette version) */
    (void)buffer; /* position/direction initiales gérées ailleurs */


    /* ---- lecture des passages et points de transit ---- */
    while (fgets(buffer, sizeof(buffer), f)) {
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strcmp(buffer, ".") == 0) {
            break;
        }

        if (strchr(buffer, '-') != NULL) {
            int src, dest;
            if (sscanf(buffer, "%d-%d", &src, &dest) == 2) {
                ajouterPassage(lab, src, dest);
                ajouterPassage(lab, dest, src); 
            }
        }
        else {
            int transit = atoi(buffer);
            if (transit > 0) {
                ajouterPointTransit(lab, transit);
            }
        }
    }

    fclose(f);
    return lab;
}
