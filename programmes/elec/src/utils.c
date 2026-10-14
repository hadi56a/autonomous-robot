/**
 * @file utils.c
 * @brief Implémentation des fonctions utilitaires
 */

#include "utils.h"

char* readStdinDynamic() {
    size_t capacite = 128;
    size_t longueur = 0;
    char *buffer = malloc(capacite * sizeof(char));

    if (!buffer) return NULL;

    int ch;
    while ((ch = getchar()) != EOF) {
        if (longueur + 1 >= capacite) {
            capacite *= 2;
            char *temp = realloc(buffer, capacite);
            if (!temp) {
                free(buffer);
                return NULL;
            }
            buffer = temp;
        }
        buffer[longueur++] = ch;
    }
    buffer[longueur] = '\0'; // Toujours terminer la string
    return buffer;
}
