#!/bin/bash

# Vérification du bon nombre d'arguments.
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <input_file>"
    exit 1
fi

INPUT_FILE=$1

# Vérifie que l'entrée existe et n'est pas un répertoire.
if [ ! -f "$INPUT_FILE" ]; then
    echo "Error: Input file not found at '$INPUT_FILE'"
    exit 1
fi

# Définition de nos variables de programmes.
MAIN_ROBOT_EXEC="./elec/bin/main"
MAIN_EXEC="./algo/bin/laby"

# Vérifie que les 2 programmes sont accessibles et éxécutables.
if [ ! -x "$MAIN_EXEC" ]; then
    echo "Error: '$MAIN_EXEC' not found or is not executable. Please compile it first."
    exit 1
fi

if [ ! -x "$MAIN_ROBOT_EXEC" ]; then
    echo "Error: '$MAIN_ROBOT_EXEC' not found or is not executable. Please compile it first."
    exit 1
fi

(time "$MAIN_EXEC" "$INPUT_FILE") | (time "$MAIN_ROBOT_EXEC")