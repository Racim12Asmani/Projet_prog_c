/**
 * ProgC - Projet Automne 25-26 : Gestion de systèmes de fichiers
 * VERSION 2
 * Fichier : sf.c
 * Module de gestion d'un systèmes de fichiers (simulé)
 **/

#include "sf.h"
#include "bloc.h"
#include "inode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Taille maximale du nom du SF (ou nom du disque)
#define TAILLE_NOM_DISQUE 24

// Définition du super-bloc
struct sSuperBloc
{
  // Le nom du disque (ou du système de fichiers) (avec le '\0')
  char nomDisque[TAILLE_NOM_DISQUE+1];
  // La date de dernière modification du système de fichiers
  time_t dateDerModif;
};

// Type représentant le super-bloc (déjà défini par l'utilisateur comme pointeur)
// typedef struct sSuperBloc *tSuperBloc;

// Définition de la liste chaînée des inodes
struct sListeInodes
{
  // Références vers le premier et le dernier élément de la liste chaînée
  struct sListeInodesElement *premier, *dernier;
  // Nombre d'inodes dans la liste
  int nbInodes;
};

// Définition d'un élement de la liste chaînée des inodes dans le SF
struct sListeInodesElement
{
  // L'inode proprement dit
  tInode inode;
  // L'élément suivant dans la liste chaînée
  struct sListeInodesElement *suivant;
};

// Définition d'un système de fichiers (simplifié)
struct sSF
{
  // Le super-bloc (Pointeur, selon votre définition)
  tSuperBloc superBloc;
  // La liste chaînée des inodes (Valeur, selon votre définition)
  struct sListeInodes listeInodes;
};

/* ------------- FONCTIONS NON PUBLIQUES (STATIC) ------------- */

/* V2
*  Crée et retourne un super-bloc.
* Fonction non publique (static)
* Sortie : le super-bloc, ou NULL en cas de problème
*/
static tSuperBloc CreerSuperBloc(char nomDisque[]) {
    tSuperBloc sb = (tSuperBloc)malloc(sizeof(struct sSuperBloc));
    if (sb == NULL) {
        fprintf(stderr, "CreerSuperBloc : probleme creation\n");
        return NULL;
    }
    strncpy(sb->nomDisque, nomDisque, TAILLE_NOM_DISQUE);
    sb->nomDisque[TAILLE_NOM_DISQUE] = '\0';
    sb->dateDerModif = time(NULL);
    return sb;
}

/* V2
*  Détruit un super-bloc.
* Fonction non publique (static)
*/
static void DetruireSuperBloc(tSuperBloc *pSuperBloc) {
    if (pSuperBloc != NULL && *pSuperBloc != NULL) {
        free(*pSuperBloc);
        *pSuperBloc = NULL;
    }
}


/* V2
*  Affiche le contenu d'un super-bloc.
*  Fonction non publique (static)
*/
static void AfficherSuperBloc(tSuperBloc superBloc) {
    if (superBloc == NULL) {
        printf("Super-bloc vide.\n");
        return;
    }
    printf("Sf de nom %s, super bloc :\n", superBloc->nomDisque);
    printf("taille bloc = %d, date der modif = %s", TAILLE_BLOC, ctime(&superBloc->dateDerModif));
}




/*------------- FONCTIONS PUBLIQUES -------------*/


/* V2
 * Crée un nouveau système de fichiers.
 */
tSF CreerSF (char nomDisque[]){
    tSF sf = (tSF) malloc(sizeof(struct sSF));

    if (sf == NULL) {
        fprintf(stderr, "Erreur: Problème d'allocation mémoire pour le SF.\n");
        return NULL;
    }

    // Initialisation du SuperBloc
    sf->superBloc = CreerSuperBloc(nomDisque);
    if (sf->superBloc == NULL) {
        free(sf);
        return NULL;
    }

    // Initialisation de la Liste d'Inodes
    sf->listeInodes.premier = NULL;
    sf->listeInodes.dernier = NULL;
    sf->listeInodes.nbInodes = 0;

    return sf;
}

/* V2
 * Détruit un système de fichiers et libère la mémoire associée.
 */
void DetruireSF(tSF *pSF) {
    if (pSF != NULL && *pSF != NULL) {
        tSF sf = *pSF;
        struct sListeInodesElement *courant = sf->listeInodes.premier;
        struct sListeInodesElement *suivant = NULL;
        while (courant != NULL) {
            suivant = courant->suivant;
            DetruireInode(&(courant->inode));
            free(courant);
            courant = suivant;
        }
        DetruireSuperBloc(&(sf->superBloc));
        free(*pSF);
        *pSF = NULL;
    }
}


/* V2
 * Affiche les informations relative à un système de fichiers i.e;
 * le contenu du super-bloc et celui des différents inodes du SF
 */
void AfficherSF(tSF sf) {
    if (sf == NULL || sf->superBloc == NULL) {
        printf("Système de fichiers vide.\n");
        return;
    }
    AfficherSuperBloc(sf->superBloc);
    printf("Inodes :\n");
    struct sListeInodesElement *courant = sf->listeInodes.premier;
    while (courant != NULL) {
        AfficherInode(courant->inode);
        courant = courant->suivant;
    }
}

/* V2
 * Ecrit un fichier d'un seul bloc dans le système de fichiers.
 */
long Ecrire1BlocFichierSF(tSF sf, char nomFichier[], natureFichier type) {
    if (sf == NULL || nomFichier == NULL) {
        return -1;
    }
    unsigned int nouveauNumInode = sf->listeInodes.nbInodes + 1;
    tInode nouvelInode = CreerInode(nouveauNumInode, type);
    if (nouvelInode == NULL) {
        return -1;
    }
    long tailleCopiee = EcrireDonneesInode1bloc(nouvelInode, (unsigned char *)nomFichier, (long)strlen(nomFichier));
    if (tailleCopiee == -1) {
        DetruireInode(&nouvelInode);
        return -1;
    }
    struct sListeInodesElement *nouvelElement = malloc(sizeof(struct sListeInodesElement));
    if (nouvelElement == NULL) {
        fprintf(stderr, "Erreur: Problème d'allocation mémoire pour l'élément de liste.\n");
        DetruireInode(&nouvelInode);
        return -1;
    }
    nouvelElement->inode = nouvelInode;
    nouvelElement->suivant = NULL;
    if (sf->listeInodes.premier == NULL) {
        sf->listeInodes.premier = nouvelElement;
        sf->listeInodes.dernier = nouvelElement;
    } else {
        sf->listeInodes.dernier->suivant = nouvelElement;
        sf->listeInodes.dernier = nouvelElement;
    }
    sf->listeInodes.nbInodes++;
    sf->superBloc->dateDerModif = time(NULL);
    return tailleCopiee;
}
