#include "file.h"



void initialiserFile(File_t* file){
    file->liste = LCL_liste(copier_RCS, liberer_RCS);
}

void enfiler(File_t* file, etat_robot_courant_t valeur){
    LCL_inserer(&(file->liste), &valeur, 0);
}

etat_robot_courant_t defiler(File_t* file){
    if(LCL_estVide(file->liste)){
        return (etat_robot_courant_t){0, 0, 0, RIEN};
    }
    etat_robot_courant_t* valeur = (etat_robot_courant_t*) LCL_element(file->liste, LCL_longueur(file->liste)-1);
    LCL_supprimer(&(file->liste), LCL_longueur(file->liste)-1);
    etat_robot_courant_t resultat = *valeur;
    free(valeur);
    return resultat;
}

bool fileEstVide(File_t* file){
    return LCL_estVide(file->liste);
}

void libererFile(File_t* file){
    LCL_vider(&(file->liste));
    free(file);
}

void* copier_RCS(void* valeur){
    void* copie = malloc(sizeof(etat_robot_courant_t));
    if(copie == NULL){
        return NULL;
    }
    memcpy(copie, valeur, sizeof(etat_robot_courant_t));
    return copie;
}

void liberer_RCS(void* valeur){
    free(valeur);
}
