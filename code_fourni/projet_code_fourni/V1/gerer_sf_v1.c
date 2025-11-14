/**
 * ProgC - Projet Automne 25-26 : Gestion de systèmes de fichiers
 * VERSION 1
 * Fichier : gerer_sf_v1.c
 * Programme principal pour tester les fonctions de gestion des inodes et des blocs.
 **/
#include <stdio.h>
#include <string.h>
#include "inode.h"
#define TAILLE_BUFFER 100

int main(void) {
    tInode inode;

    // Créer un inode (vide) de numéro 0 et dont le type de fichier est ordinaire
    inode = CreerInode(0, ORDINAIRE);
    printf("** Inode cree\n");
    AfficherInode(inode);

    // Écrire/stocker dans cet inode (dans le seul bloc 0 pour cette V1) un "fichier"
    char unContenu[TAILLE_BUFFER];
    strcpy(unContenu, "Bonjour, Monde !");
    long tailleOctetsContenu = strlen(unContenu) + 1; // Inclut le caractère de fin de chaîne '\0'
    long nb_ecrits = EcrireDonneesInode1bloc(inode, (unsigned char *)unContenu, tailleOctetsContenu);
    printf("\nNombre d'octets ecrits %ld\n", nb_ecrits);
    printf("\n*** Inode rempli\n");
    AfficherInode(inode);

    // Lire le contenu du fichier stocké dans l'inode dans buffer
    char buffer[TAILLE_BUFFER];
    long nb_lus = LireDonneesInode1bloc(inode, (unsigned char *)buffer, tailleOctetsContenu);
    buffer[nb_lus] = '\0';
    printf("\n*** Lecture du contenu de l'inode :\n");
    printf("Nombre d'octets lus : %ld\n", nb_lus);
    printf("Contenu lu : %s\n", buffer);

    // Détruire l'inode
    DetruireInode(&inode);
    printf("\n*** Inode detruit : tests OK!\n");

    return 0;
}
