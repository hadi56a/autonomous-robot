/**
 * \file listeChainee.h
 * \brief Implantation du TAD ListeChainee génériques
 * \author N. Delestre
 * \version 2.0
 * \date 23/10/2012
 *
 */


#ifndef __LISTE_CHAINEE__
#define __LISTE_CHAINEE__
#include <errno.h>
#include "copieLiberationComparaison.h"

/* Partie privee */


typedef struct LC_Noeud* LC_ListeChainee;
typedef struct LC_Noeud {
  void* lElement;
  LC_ListeChainee listeSuivante;
}  LC_Noeud;

/* Partie publique */

/**
 * \def LC_ERREUR_MEMOIRE
 * \brief valeur que prend errno lorsqu'il n'y a plus de mémoire système
 */
#define LC_ERREUR_MEMOIRE 1

/**
 * \fn LC_ListeChainee LC_listeChainee()
 * \brief Fonction de création d'une liste chaînée vide
 *
 * \return LC_ListeChainee
 */
LC_ListeChainee LC_listeChainee();

/**
 * \fn int LC_estVide(LC_ListeChainee)
 * \brief Fonction qui permet de savoir si une liste est vide
 *
 * \param LC_ListeChainee l, la liste chaînée
 * \return int
 */
int LC_estVide(LC_ListeChainee);

/**
 * \fn LC_ajouter(LC_ListeChainee*,void*,CLC_FonctionCopier)
 * \brief Fonction d'ajout d'un élément en tête de liste
 *        errno contient LC_ERREUR_MEMOIRE s'il n'y a plus assez de mémoire système
 * \attention la donnée ajoutée à la collection est dupliquée
 * \post errno=LC_ERREUR_MEMOIRE si pas assez de mémoire
 *
 * \param LC_ListeChainee* pl, la liste chaînée
 * \param void* element, un pointeur vers l'émélemnt à ajouter
 * \param CLC_FonctionCopier copierElement, la fonction qui permet de dupliquer l'élément
 */
void LC_ajouter(LC_ListeChainee*,void*,CLC_FonctionCopier);

/**
 * \fn LC_obtenirElement(LC_ListeChainee)
 * \brief Fonction qui retourne l'élément qui se trouve en tête de liste
 * \pre la liste doit être non vide 
 *
 * \param LC_ListeChainee l
 * \return void*
 */
void* LC_obtenirElement(LC_ListeChainee); 

/**
 * \fn LC_obtenirListeSuivante(LC_ListeChainee)
 * \brief Fonction qui retourne l'élémént qui se trouve en tete de liste
 * \pre la liste doit être non vide 
 *
 * \param LC_ListeChainee l
 * \return LC_ListeChainee
 */
LC_ListeChainee LC_obtenirListeSuivante(LC_ListeChainee); 

/**
 * \fn LC_fixerListeSuivante(LC_ListeChainee*, LC_ListeChainee)
 * \brief Fonction qui permet de modifier la liste suivante
 * \pre la liste doit être non vide 
 *
 * \param LC_ListeChainee* pl
 * \param LC_ListeChainee suivant
 */
void LC_fixerListeSuivante(LC_ListeChainee*, LC_ListeChainee); 

/**
 * \fn LC_fixerElement(LC_ListeChainee*,void*,CLC_FonctionCopier,CLC_FonctionLiberer)
 * \brief Fonction qui permet de remplacer l'élément qui se trouve en tête de liste
 * \attention la donnée ajoutée à remplacer est supprimée de la mémoire et la donnée de remplacement est dupliquée
 * \pre la liste doit être non vide 
 *
 * \param LC_ListeChainee* pl
 * \param CLC_FonctionCopier copierElement
 * \param CLC_FonctionLiberer libererElement
 */
void LC_fixerElement(LC_ListeChainee*,void*,CLC_FonctionCopier,CLC_FonctionLiberer);

/**
 * \fn LC_supprimerTete(LC_ListeChainee*,CLC_FonctionLiberer)
 * \brief Fonction qui permet de supprimer la tête de liste
 * \pre la liste doit être non vide 
 * \attention la donnée qui se trouvait en tête de liste est supprimée
 *
 * \param LC_ListeChainee* pl
 * \param CLC_FonctionLiberer libererElement
 */
void LC_supprimerTete(LC_ListeChainee*, CLC_FonctionLiberer);

/**
 * \fn LC_supprimer(LC_ListeChainee*,CLC_FonctionLiberer)
 * \brief Fonction qui permet de supprimer une liste en profondeur, c'est-à-dire que tous les éléments de la collection sont supprimés
 *
 * \param LC_ListeChainee* pl
 * \param CLC_FonctionLiberer libererElement
 */
void LC_supprimer(LC_ListeChainee*, CLC_FonctionLiberer); 

/**
 * \fn LC_copier(LC_ListeChainee,CLC_FonctionCopier)
 * \brief Fonction qui permet de copier  une liste en profondeur, c'est-à-dire que tous les éléments de la collection sont dupliqués
 *
 * \param LC_ListeChainee l
 * \param CLC_FonctionCopier copierElement
 */
LC_ListeChainee LC_copier(LC_ListeChainee,CLC_FonctionCopier);

/**
 * \fn LC_egales(LC_ListeChainee,LC_ListeChainee,CLC_FonctionComparer)
 * \brief Fonction qui permet de savoir si deux listes sont égales
 * (pas obligatoirement identiques)
 *
 * \param LC_ListeChainee l1
 * \param LC_ListeChainee l2
 * \param CLC_FonctionComparer 
 * \return int
 */
int LC_egales(LC_ListeChainee,LC_ListeChainee,CLC_FonctionComparer);
#endif 
