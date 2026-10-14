/**
 * @file reader.c
 * @brief Implémentation du parsing des commandes/instructions
 */

#include "reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

CommandList* read_command(char* input){
    CommandList* command_list = malloc(sizeof(CommandList));
    command_list->list = malloc(10*sizeof(Command));
    command_list->size = 0;

    if (!command_list) {
        fprintf(stderr, "Erreur lors de l'allocation de mémoire pour la commande.\n");
        exit(EXIT_FAILURE);
    }

    int ind = 1;

    const char * curLine = input;
    while(curLine)
    {
        const char * nextLine = strchr(curLine, '\n');
        int curLineLen = nextLine ? (nextLine-curLine) : strlen(curLine);
        if (curLineLen == 0) {
            curLine = nextLine ? (nextLine+1) : NULL;
            continue; // On passe directement à la ligne suivante
        }
        char * tempStr = (char *) malloc(curLineLen+1);
        if (tempStr){
            memcpy(tempStr, curLine, curLineLen);
            tempStr[curLineLen] = '\0';
            if(ind%10==0){
                command_list->list = realloc(command_list->list, (ind+10)*sizeof(Command));
            }
            if(strcmp(tempStr, "AV") == 0){
                command_list->list[ind-1] = FORWARD;
            }
            else if(strcmp(tempStr, "TG") == 0){
                command_list->list[ind-1] = LEFT;
            }
            else if(strcmp(tempStr, "TD") == 0){
                command_list->list[ind-1] = RIGHT;
            }
            else if(strcmp(tempStr, ".") == 0){
                command_list->size = ind-1;
                command_list->list = realloc(command_list->list, ind*sizeof(Command));
                free(tempStr);
                break;
            }
            else {
                fprintf(stderr, "Commande invalide : %s\n", tempStr);
            }
            ind++;
            free(tempStr);
        }
        else printf("malloc() failed!?\n");
        curLine = nextLine ? (nextLine+1) : NULL;
    }
    return command_list;
}


void printCommand(CommandList* command){
    if(command){
        for(int i = 0; i < command->size; i++){
            switch(command->list[i]){
                case FORWARD:
                    printf("FORWARD\n");
                    break;
                case LEFT:
                    printf("LEFT\n");
                    break;
                case RIGHT:
                    printf("RIGHT\n");
                    break;
                default:
                    printf("UNKNOWN\n");
                    break;
            }
        }
        printf("\n");
    }
}

void freeCommandList(CommandList* command){
    if(command){
        free(command->list);
        free(command);
    }
}
