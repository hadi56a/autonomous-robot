/**
 * @file utils.h
 * @brief Fonctions utilitaires
 * @details Fournit des fonctions d'aide diverses, notamment pour la gestion des entrées/sorties
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Lit l'entrée standard de manière dynamique
 * @details Cette fonction lit tout le contenu de stdin jusqu'à EOF et le stocke dans un buffer alloué dynamiquement
 * @return Un pointeur vers la chaîne de caractères lue (doit être libérée avec free()), ou NULL en cas d'erreur
 */
char* readStdinDynamic();

#endif
