#include<stdio.h>
#include<CUnit/Basic.h>
#include "labyrinthe.h"
#include "copieLiberationComparaisonTypesSimples.h"

int init_suite_success(void) {
  return 0;
}

int clean_suite_success(void) {
  return 0;
}

void test_creer_labyrinthe(void) {
  labyrinthe_t* laby = creerLabyrinthe(4);
  CU_ASSERT_PTR_NOT_NULL_FATAL(laby);
  CU_ASSERT_EQUAL(obtenirTaille(laby), 4);
  CU_ASSERT_TRUE(THE_estVide(*(obtenirPointsTransit(laby))));
  libererLabyrinthe(laby);
}

void test_ajouter_et_verifier_passage(void) {
  labyrinthe_t* laby = creerLabyrinthe(2);
  ajouterPassage(laby, 1, 2);
  CU_ASSERT_TRUE(estPassage(laby, 1, 2));
  CU_ASSERT_TRUE(estPassage(laby, 2, 1));
  libererLabyrinthe(laby);
}

void test_supprimer_passage(void) {
  labyrinthe_t* laby = creerLabyrinthe(2);
  ajouterPassage(laby, 1, 2);
  supprimerPassage(laby, 1, 2);
  CU_ASSERT_FALSE(estPassage(laby, 1, 2));
  CU_ASSERT_FALSE(estPassage(laby, 2, 1));
  libererLabyrinthe(laby);
}

void test_transit_points(void) {
  labyrinthe_t* laby = creerLabyrinthe(3);
  ajouterPointTransit(laby, 3);
  ajouterPointTransit(laby, 5);
  CU_ASSERT_TRUE(estPointTransit(laby, 3));
  CU_ASSERT_TRUE(estPointTransit(laby, 5));
  CU_ASSERT_EQUAL(THE_cardinalite(*(obtenirPointsTransit(laby))), 2);
  supprimerPointTransit(laby, 3);
  CU_ASSERT_FALSE(estPointTransit(laby, 3));
  CU_ASSERT_EQUAL(THE_cardinalite(*(obtenirPointsTransit(laby))), 1);
  libererLabyrinthe(laby);
}

void test_obtenir_voisins(void) {
  labyrinthe_t* laby = creerLabyrinthe(3);
  ajouterPassage(laby, 1, 2);
  ajouterPassage(laby, 1, 4);
  THE_Ensemble voisins = obtenirVoisins(laby, 1);
  int voisinA = 2;
  int voisinB = 4;
  int nonVoisin = 5;
  CU_ASSERT_EQUAL(THE_cardinalite(voisins), 2);
  CU_ASSERT_TRUE(THE_estPresent(voisins, &voisinA));
  CU_ASSERT_TRUE(THE_estPresent(voisins, &voisinB));
  CU_ASSERT_FALSE(THE_estPresent(voisins, &nonVoisin));
  THE_vider(&voisins);
  libererLabyrinthe(laby);
}

void test_est_trajet(void) {
  labyrinthe_t* laby = creerLabyrinthe(3);
  ajouterPassage(laby, 1, 2);
  ajouterPassage(laby, 2, 3);

  LCL_Liste valide = LCL_liste(CLCTS_copierInt, CLCTS_libererInt);
  int n1 = 1, n2 = 2, n3 = 3;
  LCL_inserer(&valide, &n1, 0);
  LCL_inserer(&valide, &n2, 1);
  LCL_inserer(&valide, &n3, 2);
  CU_ASSERT_TRUE(estTrajet(laby, &valide));

  LCL_Liste invalide = LCL_liste(CLCTS_copierInt, CLCTS_libererInt);
  int n4 = 4;
  LCL_inserer(&invalide, &n1, 0);
  LCL_inserer(&invalide, &n4, 1);
  CU_ASSERT_FALSE(estTrajet(laby, &invalide));

  LCL_Liste vide = LCL_liste(CLCTS_copierInt, CLCTS_libererInt);
  CU_ASSERT_FALSE(estTrajet(laby, &vide));

  LCL_vider(&valide);
  LCL_vider(&invalide);
  LCL_vider(&vide);
  libererLabyrinthe(laby);
}



int main(int argc, char** argv){
  CU_pSuite pSuite = NULL;

  /* initialisation du registre de tests */
  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  /* ajout d'une suite de test */
  pSuite = CU_add_suite("Tests boite noire : labyrinthe", init_suite_success, clean_suite_success);
  if (NULL == pSuite) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* Ajout des tests à la suite de tests boite noire */
  if ((NULL == CU_add_test(pSuite, "creerLabyrinthe initialise taille et structures", test_creer_labyrinthe))
      || (NULL == CU_add_test(pSuite, "ajouterPassage / estPassage expose un lien non oriente", test_ajouter_et_verifier_passage))
      || (NULL == CU_add_test(pSuite, "supprimerPassage retire le lien", test_supprimer_passage))
      || (NULL == CU_add_test(pSuite, "Points de transit ajout/suppression et acces", test_transit_points))
      || (NULL == CU_add_test(pSuite, "Voisins refletent les cases adjacentes", test_obtenir_voisins))
      || (NULL == CU_add_test(pSuite, "estTrajet valide la continuite du chemin", test_est_trajet))
      )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  /* Lancement des tests */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  printf("\n");
  CU_basic_show_failures(CU_get_failure_list());
  printf("\n\n");

  /* Nettoyage du registre */
  CU_cleanup_registry();
  return CU_get_error();
}
