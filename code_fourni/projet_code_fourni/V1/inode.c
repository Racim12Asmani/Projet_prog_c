/**
 * ProgC - Projet Automne 25-26 : Gestion de systèmes de fichiers
 * VERSION 1
 * Fichier : inode.c
 * Module de gestion des inodes.
 **/
#include "inode.h"
#include "bloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Nombre maximal de blocs dans un inode
#define NB_BLOCS_DIRECTS 10

// Définition d'un inode
struct sInode
{
  // Numéro de l'inode
  unsigned int numero;
  // Le type du fichier : ordinaire, répertoire ou autre
  natureFichier type;
  // La taille en octets du fichier
  long taille;
  // Les adresses directes vers les blocs (NB_BLOCS_DIRECTS au maximum)
  tBloc blocDonnees[NB_BLOCS_DIRECTS];
  // Les dates : dernier accès à l'inode, dernière modification du fichier et de l'inode
  time_t dateDerAcces, dateDerModif, dateDerModifInode;
};

/* V1
 * Crée et retourne un inode.
 * Entrées : numéro de l'inode et le type de fichier qui y est associé
 * Retour : l'inode créé ou NULL en cas de problème
 */
tInode CreerInode(int numInode, natureFichier type) {
  tInode inode = (tInode)malloc(sizeof(struct sInode));
  if (inode == NULL) {
    fprintf(stderr, "CreerInode : probleme creation\n");
    return NULL;
  }

  inode->numero = numInode;
  inode->type = type;
  inode->taille = 0;

  time_t maintenant = time(NULL);
  inode->dateDerAcces = maintenant;
  inode->dateDerModif = maintenant;
  inode->dateDerModifInode = maintenant;

  for (int i = 0; i < NB_BLOCS_DIRECTS; i++) {
    inode->blocDonnees[i] = NULL;
  }

  return inode;
}

/* V1
 * Détruit un inode.
 * Entrée : l'inode à détruire
 * Retour : aucun
 */
void DetruireInode(tInode *pInode) {
  if (*pInode != NULL) {
    for (int i = 0; i < NB_BLOCS_DIRECTS; i++) {
      if ((*pInode)->blocDonnees[i] != NULL) {
        DetruireBloc(&((*pInode)->blocDonnees[i]));
      }
    }
    free(*pInode);
    *pInode = NULL;
  }
}

/* V1
 * Récupère la date de dernier accès à un inode.
 * Entrée : l'inode pour lequel on souhaite connaître la date de dernier accès
 * Retour : la date de dernier accès à l'inode
 */
time_t DateDerAcces(tInode inode) {
  if (inode == NULL) {
    return (time_t)-1;
  }
  return inode->dateDerAcces;
}

/* V1
 * Récupère la date de dernière modification d'un inode.
 * Entrée : l'inode pour lequel on souhaite connaître la date de dernière modification
 * Retour : la date de dernière modification de l'inode
 */
time_t DateDerModif(tInode inode) {
  if (inode == NULL) {
    return (time_t)-1;
  }
  return inode->dateDerModif;
}

/* V1
 * Récupère la date de dernière modification d'un fichier associé à un inode.
 * Entrée : l'inode pour lequel on souhaite connaître la date de dernière modification du fichier associé
 * Retour : la date de dernière modification du fichier associé à l'inode
 */
time_t DateDerModifFichier(tInode inode) {
  if (inode == NULL) {
    return (time_t)-1;
  }
  return inode->dateDerModif;
}

/* V1
 * Récupère le numéro d'un inode.
 * Entrée : l'inode pour lequel on souhaite connaître le numéro
 * Retour : le numéro de l'inode
 */
unsigned int Numero(tInode inode) {
  if (inode == NULL) {
    return 0;
  }
  return inode->numero;
}

/* V1
 * Récupère la taille en octets du fichier associé à un inode.
 * Entrée : l'inode pour lequel on souhaite connaître la taille
 * Retour : la taille en octets du fichier associé à l'inode
 */
long Taille(tInode inode) {
  if (inode == NULL) {
    return -1;
  }
  return inode->taille;
}

/* V1
 * Récupère le type du fichier associé à un inode.
 * Entrée : l'inode pour lequel on souhaite connaître le type de fichier associé
 * Retour : le type du fichier associé à l'inode
 */
natureFichier Type(tInode inode) {
  if (inode == NULL) {
    return AUTRE;
  }
  return inode->type;
}

/* V1
 * Affiche les informations d'un inode
 * Entrée : l'inode dont on souhaite afficher les informations
 * Retour : aucun
 */

void AfficherInode(tInode inode) {
  if (inode == NULL) {
    printf("vide\n");
    return;
  }

  printf("--------Inode----[%u] :\n", inode->numero);
  printf("type : ");
  switch (inode->type) {
    case ORDINAIRE: printf("Ordinaire\n"); break;
    case REPERTOIRE: printf("Repertoire\n"); break;
    default: printf("Autre\n");
  }
  printf("taille : %ld octets\n", inode->taille);
  printf("date dernier accès : %s", ctime(&(inode->dateDerAcces)));
  printf("date dernière modification : %s", ctime(&(inode->dateDerModif)));
  printf("date dernière modification inode : %s", ctime(&(inode->dateDerModifInode)));

  printf("Données :\n");
  if (inode->blocDonnees[0] != NULL) {
    unsigned char buffer[TAILLE_BLOC + 1];
    long tailleLue = LireContenuBloc(inode->blocDonnees[0], buffer, inode->taille);
    buffer[tailleLue] = '\0';
    printf("%s\n", buffer);
  } else {
    printf("\n");
  }
}


/* V1
 * Copie à l'adresse contenu les taille octets stockés dans un inode.
 * Si taille est supérieure à la taille d’un bloc, seuls les TAILLE_BLOC premiers octets doivent être copiés.
 * Entrées : l'inode, l'adresse de la zone où recopier et la taille en octets de l'inode
 * Retour : le nombre d'octets effectivement lus dans l'inode ou -1 en cas d'erreur
 */
long LireDonneesInode1bloc(tInode inode, unsigned char *contenu, long taille) {
  if (inode == NULL || contenu == NULL || taille <= 0) {
    return -1;
  }

  if (inode->blocDonnees[0] == NULL) {
    return 0;
  }

  long tailleLue = LireContenuBloc(inode->blocDonnees[0], contenu, taille);
  inode->dateDerAcces = time(NULL);

  return tailleLue;
}



/* V1
 * Copie dans un inode les taille octets situés à l’adresse contenu.
 * Si taille est supérieure à la taille d’un bloc, seuls les TAILLE_BLOC premiers octets doivent être copiés.
 * Entrées : l'inode, l'adresse de la zone à recopier et sa taille en octets
 * Retour : le nombre d'octets effectivement écrits dans l'inode ou -1 en cas d'erreur
 */
long EcrireDonneesInode1bloc(tInode inode, unsigned char *contenu, long taille) {
  if (inode == NULL || contenu == NULL || taille <= 0) {
    return -1;
  }

  if (inode->blocDonnees[0] == NULL) {
    inode->blocDonnees[0] = CreerBloc();
    if (inode->blocDonnees[0] == NULL) {
      return -1;
    }
  }

  long tailleCopiee = EcrireContenuBloc(inode->blocDonnees[0], contenu, taille);
  inode->taille = tailleCopiee;
  inode->dateDerModif = time(NULL);
  inode->dateDerModifInode = time(NULL);

  return tailleCopiee;
}
