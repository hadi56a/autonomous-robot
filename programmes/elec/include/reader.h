/**
 * @file reader.h
 * @brief Lecture et parsing des commandes de robot
 * @details Ce module permet d'analyser une chaîne de caractères contenant des instructions (AV, TG, TD) et de générer une liste de commandes
 */

#ifndef __READER_H__
#define __READER_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Énumération des commandes possibles
 */
typedef enum {
    FORWARD, /**< Avancer (AV) */
    LEFT,    /**< Tourner à gauche (TG) */
    RIGHT    /**< Tourner à droite (TD) */
} Command;

/**
 * @brief Structure représentant une liste de commandes
 */
typedef struct {
    Command* list; /**< Tableau dynamique des commandes */
    int size;      /**< Nombre de commandes dans la liste */
} CommandList;

/**
 * @brief Parse une chaîne de caractères pour extraire les commandes
 * @details Lit la chaîne input ligne par ligne. Chaque ligne doit contenir une commande ("AV", "TG", "TD").
 * Le parsing s'arrête si une ligne contient "." ou à la fin de la chaîne
 * @param input La chaîne de caractères contenant les commandes
 * @return Un pointeur vers la structure CommandList allouée dynamiquement
 */
CommandList* read_command(char* input);

/**
 * @brief Affiche le contenu d'une liste de commandes sur la sortie standard
 * @param command La liste des commandes à afficher
 */
void printCommand(CommandList* command);

/**
 * @brief Libère la mémoire allouée pour une liste de commandes
 * @param commandList La liste à libérer
 */
void freeCommandList(CommandList* commandList);

#endif
