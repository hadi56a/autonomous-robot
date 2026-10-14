#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>
#include "robot.h"
#include "labyrinthe.h"

int init_suite_robot(void) {
    return 0;
}

int clean_suite_robot(void) {
    return 0;
}

void test_creation_et_getters(void) {
    robot_t* robot = creerRobot(5, NORD);
    CU_ASSERT_PTR_NOT_NULL_FATAL(robot);
    CU_ASSERT_EQUAL(obtenirPosition(robot), 5);
    CU_ASSERT_EQUAL(obtenirDirection(robot), NORD);

    definirPosition(robot, 10);
    CU_ASSERT_EQUAL(obtenirPosition(robot), 10);

    libererRobot(robot);
}

void test_rotations(void) {
    robot_t* robot = creerRobot(1, NORD);

    // Test rotation droite : N -> E -> S -> O -> N
    tournerDroite(robot);
    CU_ASSERT_EQUAL(obtenirDirection(robot), EST);
    tournerDroite(robot);
    CU_ASSERT_EQUAL(obtenirDirection(robot), SUD);
    tournerDroite(robot);
    CU_ASSERT_EQUAL(obtenirDirection(robot), OUEST);
    tournerDroite(robot);
    CU_ASSERT_EQUAL(obtenirDirection(robot), NORD);

    // Test rotation gauche : N -> O -> S -> E -> N
    tournerGauche(robot);
    CU_ASSERT_EQUAL(obtenirDirection(robot), OUEST);
    
    // Vérification des fonctions de direction
    CU_ASSERT_EQUAL(directionGauche(robot), SUD);
    CU_ASSERT_EQUAL(directionDroite(robot), NORD);

    libererRobot(robot);
}

void test_limites_grille(void) {
    labyrinthe_t* laby = creerLabyrinthe(3);
    robot_t* robot = creerRobot(2, NORD);

    // Au bord Nord (case 2), peut-on regarder au Nord ?
    CU_ASSERT_FALSE(caseDevantExiste(robot, laby));

    // Au bord Ouest (case 4), peut-on regarder à l'Ouest ?
    definirPosition(robot, 4);
    tournerGauche(robot); // Direction OUEST
    CU_ASSERT_FALSE(caseDevantExiste(robot, laby));

    // Au milieu (case 5), la case devant existe
    definirPosition(robot, 5);
    CU_ASSERT_TRUE(caseDevantExiste(robot, laby));
    CU_ASSERT_EQUAL(caseSuivante(robot, laby), 4); // 5 - 1 (OUEST) = 4

    libererLabyrinthe(laby);
    libererRobot(robot);
}

void test_mouvement_et_passages(void) {
    labyrinthe_t* laby = creerLabyrinthe(3);
    robot_t* robot = creerRobot(1, EST);

    // Pas de passage entre 1 et 2 au début
    CU_ASSERT_FALSE(peutAvancer(robot, laby));
    CU_ASSERT_FALSE(avancer(robot, laby)); 
    CU_ASSERT_EQUAL(obtenirPosition(robot), 1);

    // Ajout d'un passage
    ajouterPassage(laby, 1, 2);
    CU_ASSERT_TRUE(peutAvancer(robot, laby));
    CU_ASSERT_TRUE(avancer(robot, laby));
    CU_ASSERT_EQUAL(obtenirPosition(robot), 2);

    libererLabyrinthe(laby);
    libererRobot(robot);
}

void test_points_transit_robot(void) {
    labyrinthe_t* laby = creerLabyrinthe(3);
    robot_t* robot = creerRobot(5, NORD);

    CU_ASSERT_FALSE(estSurPointTransit(robot, laby));

    ajouterPointTransit(laby, 5);
    CU_ASSERT_TRUE(estSurPointTransit(robot, laby));

    libererLabyrinthe(laby);
    libererRobot(robot);
}

void test_regarder_cote(void) {
    labyrinthe_t* laby = creerLabyrinthe(3);
    robot_t* robot = creerRobot(5, NORD);

    // Création d'un couloir vers la gauche (case 4)
    ajouterPassage(laby, 5, 4);
    
    CU_ASSERT_TRUE(peutAllerGauche(robot, laby));
    CU_ASSERT_FALSE(peutAllerDroite(robot, laby));
    
    // On vérifie que le robot n'a pas bougé de sa position/direction initiale
    CU_ASSERT_EQUAL(obtenirPosition(robot), 5);
    CU_ASSERT_EQUAL(obtenirDirection(robot), NORD);

    libererLabyrinthe(laby);
    libererRobot(robot);
}

void test_avancer_jusque_virage(void) {
    labyrinthe_t* laby = creerLabyrinthe(5);
    robot_t* robot = creerRobot(1, EST);
    int* chemin = NULL;

    // Création d'un couloir 1-2-3-4
    ajouterPassage(laby, 1, 2);
    ajouterPassage(laby, 2, 3);
    ajouterPassage(laby, 3, 4);
    // Le sommet 4 est un cul-de-sac (degré 1) -> donc un "virage/arrêt"
    // Le sommet 3 a un degré 2 (connecté à 2 et 4) -> le robot continue

    int nb_pas = avancerJusqueVirage(robot, laby, &chemin);

    CU_ASSERT_EQUAL(nb_pas, 3); // 1->2, 2->3, 3->4
    CU_ASSERT_PTR_NOT_NULL(chemin);
    if(chemin != NULL) {
        CU_ASSERT_EQUAL(chemin[0], 2);
        CU_ASSERT_EQUAL(chemin[1], 3);
        CU_ASSERT_EQUAL(chemin[2], 4);
        free(chemin);
    }
    CU_ASSERT_EQUAL(obtenirPosition(robot), 4);

    libererLabyrinthe(laby);
    libererRobot(robot);
}


int main() {
    CU_pSuite pSuite = NULL;

    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    pSuite = CU_add_suite("Tests Robot", init_suite_robot, clean_suite_robot);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ((NULL == CU_add_test(pSuite, "Initialisation et Getters", test_creation_et_getters)) ||
        (NULL == CU_add_test(pSuite, "Rotations et directions relatives", test_rotations)) ||
        (NULL == CU_add_test(pSuite, "Limites de la grille et cases adjacentes", test_limites_grille)) ||
        (NULL == CU_add_test(pSuite, "Déplacements et collisions", test_mouvement_et_passages)) ||
        (NULL == CU_add_test(pSuite, "Détection des points de transit", test_points_transit_robot)) ||
        (NULL == CU_add_test(pSuite, "Anticipation des mouvements latéraux", test_regarder_cote)) ||
        (NULL == CU_add_test(pSuite, "Parcours automatique jusqu'au virage", test_avancer_jusque_virage))
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    printf("\n");
    CU_basic_show_failures(CU_get_failure_list());
    printf("\n\n");

    CU_cleanup_registry();
    return CU_get_error();
}