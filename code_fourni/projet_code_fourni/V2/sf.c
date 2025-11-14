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
    tSuperBloc sb = (tSuperBloc) malloc(sizeof(struct sSuperBloc));
    if (sb == NULL) {
        fprintf(stderr, "Erreur: Problème d'allocation mémoire pour le SuperBloc.\n");
        return NULL;
    }

    // Copie du nom du disque, tronqué si nécessaire
    strncpy(sb->nomDisque, nomDisque, TAILLE_NOM_DISQUE);
    sb->nomDisque[TAILLE_NOM_DISQUE] = '\0'; // Assure la terminaison par null

    // Mise à jour de la date
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
* Fonction non publique (static)
*/
static void AfficherSuperBloc(tSuperBloc superBloc) {
    if (superBloc == NULL) {
        printf("--- Super-bloc : vide ---\n");
        return;
    }
    printf("--- Super-bloc ---\n");
    printf("Nom du disque : %s\n", superBloc->nomDisque);
    printf("Date dernière modification : %s", ctime(&(superBloc->dateDerModif)));
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

    // 1. Initialisation du SuperBloc
    sf->superBloc = CreerSuperBloc(nomDisque);
    if (sf->superBloc == NULL) {
        free(sf);
        return NULL;
    }

    // 2. Initialisation de la Liste d'Inodes
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

        // 1. Destruction de la liste chaînée des inodes
        struct sListeInodesElement *courant = sf->listeInodes.premier;
        struct sListeInodesElement *suivant = NULL;

        while (courant != NULL) {
            suivant = courant->suivant;

            // Détruire l'inode contenu
            DetruireInode(&(courant->inode));

            // Libérer la mémoire de l'élément de la liste
            free(courant);
            courant = suivant;
        }

        // 2. Destruction du SuperBloc
        DetruireSuperBloc(&(sf->superBloc));

        // 3. Destruction du SF lui-même
        free(*pSF);
        *pSF = NULL;
    }
}

/* V2
 * Affiche les informations relative à un système de fichiers i.e;
 * le contenu du super-bloc et celui des différents inodes du SF
 */
void AfficherSF (tSF sf){
    if (sf == NULL) {
        printf("Système de Fichiers vide\n");
        return;
    }

    printf("\n");
    printf("------------- Système de Fichiers -------------\n");
    printf("\n");

    // 1. Affichage du Super-bloc
    AfficherSuperBloc(sf->superBloc);

    printf("\n--- Liste des Inodes (%d) ---\n", sf->listeInodes.nbInodes);

    // 2. Affichage des Inodes
    struct sListeInodesElement *courant = sf->listeInodes.premier;
    int i = 1;

    while (courant != NULL) {
        printf("Élément de la liste #%d :\n", i++);
        AfficherInode(courant->inode);
        courant = courant->suivant;
    }
}

/* V2
 * Ecrit un fichier d'un seul bloc dans le système de fichiers.
 */
long Ecrire1BlocFichierSF(tSF sf, char nomFichier[], natureFichier type) {
    long tailleCopiee = -1;

    if (sf == NULL || nomFichier == NULL) {
        return -1;
    }

    // 1. Déterminer le numéro du nouvel inode (commence à 1)
    unsigned int nouveauNumInode = sf->listeInodes.nbInodes + 1;

    // 2. Créer un nouvel inode
    tInode nouvelInode = CreerInode(nouveauNumInode, type);
    if (nouvelInode == NULL) {
        return -1;
    }

    // 3. Écrire le contenu (nomFichier) dans l'inode (max 1 bloc)
    tailleCopiee = EcrireDonneesInode1bloc(
        nouvelInode,
        (unsigned char *)nomFichier,
        (long)strlen(nomFichier)
    );

    // Si l'écriture échoue ou ne copie rien (hors 0 octet si contenu vide)
    if (tailleCopiee == -1) {
        DetruireInode(&nouvelInode);
        return -1;
    }

    // 4. Créer l'élément de liste pour le nouvel inode
    struct sListeInodesElement *nouvelElement =
        (struct sListeInodesElement *) malloc(sizeof(struct sListeInodesElement));

    if (nouvelElement == NULL) {
        fprintf(stderr, "Erreur: Problème d'allocation mémoire pour l'élément de liste.\n");
        DetruireInode(&nouvelInode);
        return -1;
    }

    nouvelElement->inode = nouvelInode;
    nouvelElement->suivant = NULL;

    // 5. Ajouter l'élément à la fin de la liste chaînée
    if (sf->listeInodes.premier == NULL) {
        sf->listeInodes.premier = nouvelElement;
        sf->listeInodes.dernier = nouvelElement;
    } else {
        sf->listeInodes.dernier->suivant = nouvelElement;
        sf->listeInodes.dernier = nouvelElement;
    }

    sf->listeInodes.nbInodes++;

    // 6. Mettre à jour le super-bloc
    // Attention: sf->superBloc est un pointeur.
    sf->superBloc->dateDerModif = time(NULL);

    return tailleCopiee;
}
