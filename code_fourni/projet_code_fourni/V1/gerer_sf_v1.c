#include <stdio.h>
#include <string.h>
#include "inode.h"
#include "bloc.h"

#define TAILLE_BUFFER 100

int main(void) {
    tInode inode;

    // Créer un inode (vide) de numéro 0 et dont le type de fichier est ordinaire
    inode = CreerInode(0, ORDINAIRE);
    printf("** Inode cree\n");
    AfficherInode(inode);

    // Ecrire/stocker dans cet inode un "fichier"
    char unContenu[] = "Bonjour, Monde !";  // plus besoin de strcpy
    long tailleOctetsContenu = sizeof(char) * (strlen(unContenu) + 1);

    long nb_ecrits = EcrireDonneesInode1bloc(
        inode,
        (unsigned char *)unContenu,
        tailleOctetsContenu
    );

    printf("\nNombre d'octets écrits %ld\n", nb_ecrits);
    printf("\n*** Inode rempli\n");
    AfficherInode(inode);

    // Lire le contenu du fichier stocké dans l'inode
    long taille_fichier = Taille(inode);         // taille réelle du fichier
    char buffer[TAILLE_BLOC + 1];                // un bloc max + '\0'

    long nb_lus = LireDonneesInode1bloc(
        inode,
        (unsigned char *)buffer,
        taille_fichier
    );

    buffer[nb_lus] = '\0';  // terminer la chaîne
    printf("\n*** Lecture du contenu de l'inode :\n");
    printf("Nombre d'octets lus : %ld\n", nb_lus);
    printf("Contenu lu : %s\n", buffer);

    // Détruire l'inode
    DetruireInode(&inode);
    printf("\n*** Inode detruit : tests OK!\n");

    return 0;
}
