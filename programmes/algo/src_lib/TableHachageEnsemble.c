#include <stdbool.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

#include "TableHachageEnsemble.h"

THE_Ensemble THE_ensemble(CLC_FonctionCopier copier_element,
                        CLC_FonctionLiberer liberer_element,
                        CLC_FonctionComparer comparer_element,
                        size_t taille_element) {
  THE_Ensemble ensemble;

  /* clé == valeur == élément */
  ensemble = TH_tableHachage(/* clé */
			     copier_element, liberer_element, comparer_element, taille_element,
			     /* valeur */
			     copier_element, liberer_element);
  return ensemble;
}

bool THE_estVide(THE_Ensemble ensemble) {
  return TH_estVide(ensemble);
}

unsigned int THE_cardinalite(THE_Ensemble ensemble) {
  return TH_taille(ensemble);
}

LCL_Liste THE_elements(THE_Ensemble ensemble) {
  void** tableCles = TH_cles(ensemble);
  LCL_Liste res = LCL_liste(ensemble.copierCle,
			    ensemble.libererCle);
  unsigned int i;
  if (tableCles != NULL) {
    for (i = 0U; i < ensemble.nbElements; ++i) {
      LCL_inserer(&res, tableCles[i], i);
      ensemble.libererCle(tableCles[i]);
    }
    free(tableCles);
  }
  return res;
}

/* ============================ Modifications ============================= */

void THE_ajouter(THE_Ensemble* pensemble, void* p_element) {
  if (!pensemble) { errno = EINVAL; return; }

  /* clé == valeur == élément */
  TH_inserer(pensemble, p_element, p_element);
}

void THE_supprimer(THE_Ensemble* pensemble, void* p_element) {
  if (!pensemble) { errno = EINVAL; return; }

  TH_supprimer(pensemble, p_element);
}

void THE_vider(THE_Ensemble* pensemble) {
  if (!pensemble) { errno = EINVAL; return; }

  TH_vider(pensemble);
}

/* ============================== Requêtes ================================ */

bool THE_estPresent(THE_Ensemble ensemble, void* p_element) {
  return TH_contient(ensemble, p_element);
}

/* ======================= Opérations ensemblistes ======================== */

/*
 * Pour construire un résultat indépendant (pas d'aliasing), on recrée un
 * dictionnaire « vierge » avec la **même signature** que l'opérande `a`,
 * grâce aux champs publics du type TH_Dictionnaire.
 */
static THE_Ensemble THE_creerComme(THE_Ensemble a) {
  THE_Ensemble nouvel_ensemble;

  nouvel_ensemble = THE_ensemble(a.copierCle,
                                a.libererCle,
                                a.comparerCle,
                                a.tailleCle);
  return nouvel_ensemble;
}

/*
 * Facteur commun : parcourir tous les éléments d'un ensemble source et n'ajouter
 * à `p_resultat` que ceux qui vérifient une fonction de test.
 */
typedef bool (*THE_FonctionTestElement)(THE_Ensemble ensemble_source,
                                       THE_Ensemble ensemble_autre,
                                       void* p_element);

static void THE_ajouter_selon(THE_Ensemble* p_resultat,
                             THE_Ensemble ensemble_source,
                             THE_Ensemble ensemble_autre,
                             THE_FonctionTestElement fonction_test) {
  void** cles;
  unsigned int i;
  void* p_element;

  cles = TH_cles(ensemble_source);
  for (i = 0; i < THE_cardinalite(ensemble_source); ++i) {
    p_element = cles[i];
    if (fonction_test(ensemble_source, ensemble_autre, p_element)) {
      THE_ajouter(p_resultat, p_element);
    }
    ensemble_source.libererCle(cles[i]);
  }
  free(cles);
}

/* Prédicats concrets (privés) */
static bool THE_test_toujours(THE_Ensemble ensemble_source,
                             THE_Ensemble ensemble_autre,
                             void* p_element) {
  bool ok;
  (void)ensemble_source; (void)ensemble_autre; (void)p_element;
  ok = true;
  return ok;
}

static bool THE_test_present_dans_autre(THE_Ensemble ensemble_source,
                                       THE_Ensemble ensemble_autre,
                                       void* p_element) {
  bool ok;
  (void)ensemble_source;
  ok = THE_estPresent(ensemble_autre, p_element);
  return ok;
}

static bool THE_test_absent_de_autre(THE_Ensemble ensemble_source,
                                    THE_Ensemble ensemble_autre,
                                    void* p_element) {
  bool ok;
  (void)ensemble_source;
  ok = !THE_estPresent(ensemble_autre, p_element);
  return ok;
}

THE_Ensemble THE_union(THE_Ensemble a, THE_Ensemble b) {
  THE_Ensemble resultat;

  resultat = THE_creerComme(a);
  THE_ajouter_selon(&resultat, a, b, THE_test_toujours);
  THE_ajouter_selon(&resultat, b, a, THE_test_toujours);

  return resultat;
}

THE_Ensemble THE_intersection(THE_Ensemble a, THE_Ensemble b) {
  THE_Ensemble resultat;

  resultat = THE_creerComme(a);
  THE_ajouter_selon(&resultat, a, b, THE_test_present_dans_autre);

  return resultat;
}

THE_Ensemble THE_soustraction(THE_Ensemble a, THE_Ensemble b) {
  THE_Ensemble resultat;

  resultat = THE_creerComme(a);
  THE_ajouter_selon(&resultat, a, b, THE_test_absent_de_autre);

  return resultat;
}
