/**
 * \file TableHachageEnsemble.h
 * \brief Ensemble générique fondé sur un dictionnaire (clé == valeur).
 * \author Nicolas Delestre et ChatGPT
 * \version 1.0
 * \date 2025
 *
 * \section CONCEPTION
 *  - Un ensemble est modélisé par un dictionnaire `HD_Dictionnaire` dont
 *    la clé et la valeur sont le même élément (copié à l’insertion).
 *  - L’égalité des éléments est définie par `comparerElement(a,b)==0`.
 *  - Les fonctions `copierElement` et `libererElement` gèrent la politique
 *    mémoire (copie profonde / libération).
 *  - La fonction de hachage interne du dictionnaire utilise la taille en
 *    octets fournie à la création pour sommer les octets de la clé (l’élément).
 *
 * \note Les opérations d’ensemble (`DE_union`, `DE_intersection`, `DE_soustraction`)
 *       supposent que les ensembles manipulés sont **compatibles** : même
 *       `tailleElement` et une sémantique de `comparerElement` cohérente.
 */

#ifndef __TABLEHACHAGEENSEMBLE_H__
#define __TABLEHACHAGEENSEMBLE_H__

#include <stdbool.h>
#include <stddef.h> /* size_t */
#include "tableHachage.h"
#include "ListeChaineeListe.h"
#include "copieLiberationComparaison.h"

/**
 * \struct THE_Ensemble
 * \brief Ensemble générique encapsulant un `HD_Dictionnaire`.
 */
typedef TH_TableHachage THE_Ensemble;

/**
 * \brief Crée un ensemble \b vide et prêt à l’emploi.
 *
 * \param copierElement  Fonction de \b copie profonde d’un élément (doit allouer et retourner un duplicata).
 * \param libererElement Fonction de \b libération d’un élément précédemment copié/interné.
 * \param comparerElement Fonction de \b comparaison d’éléments : <0 si a<b, 0 si a==b, >0 sinon.
 * \param tailleElement  Taille \b en octets des éléments (utilisée par le hachage interne).
 * \return Un ensemble initialisé et vide.
 *
 * \pre Tous les pointeurs de fonction doivent être non-NULL ; \c tailleElement > 0.
 * \post L’ensemble est vide (même sémantique que `HD_dictionnaire` pour l’initialisation).
 */
THE_Ensemble THE_ensemble(CLC_FonctionCopier copierElement,
                        CLC_FonctionLiberer libererElement,
                        CLC_FonctionComparer comparerElement,
                        size_t tailleElement);

/**
 * \brief Indique si l’ensemble est vide.
 *
 * \param ensemble L’ensemble examiné (passage par valeur).
 * \return \c true si l’ensemble est vide, \c false sinon.
 * \complexite O(1).
 */
bool THE_estVide(THE_Ensemble ensemble);

/**
 * \brief Renvoie le nombre d’éléments de l’ensemble.
 *
 * \param ensemble L’ensemble consulté.
 * \return Taille de l’ensemble (entier non signé).
 * \complexite O(1) (délégué au dictionnaire interne).
 */
unsigned int THE_cardinalite(THE_Ensemble ensemble);


/**
 * \brief Renvoie une liste des éléments de l’ensemble.
 *
 * \param ensemble L’ensemble consulté.
 * \return Une liste contenant des pointeurs vers les éléments de l’ensemble.
 *       Chaque élément est copié via la fonction `copierElement` de l’ensemble.
 **/ 
LCL_Liste THE_elements(THE_Ensemble ensemble);

/**
 * \brief Ajoute un élément à l’ensemble (s’il n’y est pas déjà).
 *
 * \param pensemble Pointeur vers l’ensemble à modifier.
 * \param pelement  Pointeur vers l’élément à ajouter (copié via \c copierElement).
 *
 * \post
 *  - Si l’élément n’était pas présent : insertion, la taille augmente, \c errno=0.
 *  - S’il était déjà présent : l’ensemble est inchangé (remplacement équivalent),
 *    \c errno peut être positionné à \c HD_CLE_DEJA_PRESENTE_INFO par le dictionnaire.
 * \complexite Amortie O(1), O(n) au pire sur la liste chaînée correspondante.
 */
void THE_ajouter(THE_Ensemble* pensemble, void* pelement);

/**
 * \brief Supprime un élément de l’ensemble (s’il est présent).
 *
 * \param pensemble Pointeur vers l’ensemble à modifier.
 * \param pelement  Pointeur vers l’élément à supprimer.
 *
 * \post
 *  - Si l’élément existait : suppression, la taille diminue, \c errno=0.
 *  - Sinon : ensemble inchangé, \c errno=HD_CLE_ABSENTE_ERREUR (renvoyé par le dictionnaire).
 * \complexite Amortie O(1), O(n) au pire sur la liste chaînée correspondante.
 */
void THE_supprimer(THE_Ensemble* pensemble, void* pelement);

/**
 * \brief Teste la présence d’un élément dans l’ensemble.
 *
 * \param ensemble L’ensemble consulté (passage par valeur).
 * \param pelement Pointeur vers l’élément à rechercher (jamais modifié).
 * \return \c true si l’élément appartient à l’ensemble, \c false sinon.
 * \note Ne modifie ni l’ensemble ni \c errno.
 * \complexite Amortie O(1), O(n) au pire sur la liste chaînée correspondante.
 */
bool THE_estPresent(THE_Ensemble ensemble, void* pelement);

/**
 * \brief Renvoie l’\b union de deux ensembles (nouvel ensemble).
 *
 * \param a Premier ensemble (source).
 * \param b Deuxième ensemble (source).
 * \return Un \b nouvel ensemble contenant tous les éléments de \c a ∪ \c b.
 *
 * \pre Les ensembles doivent être \b compatibles (même \c tailleElement et
 *      une sémantique de comparaison cohérente). L’implémentation s’aligne sur
 *      la politique mémoire (copie/libération) de \c a.
 * \post L’appelant est responsable de libérer le résultat via \c THE_vider (si fourni) ou
 *       en laissant la durée de vie naturelle si gérée en pile.
 * \complexite O(|a| + |b|) amortie.
 */
THE_Ensemble THE_union(THE_Ensemble a, THE_Ensemble b);

/**
 * \brief Renvoie l’\b intersection de deux ensembles (nouvel ensemble).
 *
 * \param a Premier ensemble (source).
 * \param b Deuxième ensemble (source).
 * \return Un \b nouvel ensemble contenant les éléments communs à \c a et \c b.
 *
 * \pre Compatibilité comme pour \c THE_union.
 * \complexite O(|a| + |b|) amortie.
 */
THE_Ensemble THE_intersection(THE_Ensemble a, THE_Ensemble b);

/**
 * \brief Renvoie la \b soustraction (différence ensembliste) \c a \ \\ \c b.
 *
 * \param a Premier ensemble (source).
 * \param b Deuxième ensemble (source).
 * \return Un \b nouvel ensemble contenant les éléments de \c a qui ne sont pas dans \c b.
 *
 * \pre Compatibilité comme pour \c THE_union.
 * \complexite O(|a| + |b|) amortie.
 */
THE_Ensemble THE_soustraction(THE_Ensemble a, THE_Ensemble b);

/**
 * \brief Vide complètement l’ensemble.
 *
 * \param pensemble Pointeur vers l’ensemble à vider.
 * \post Tous les éléments sont supprimés, la taille devient 0, \c errno=0.
 * \complexite O(N) où N est le nombre d’éléments.
 */
void THE_vider(THE_Ensemble* pensemble);



#endif /* __DICTIONNAIREENSEMBLE_H__ */
