/**
 * ProgC - Projet Automne 25-26 : Gestion de systèmes de fichiers
 * VERSION 2
 * Fichier : gerer_sf_v2.c
 * Programme de test pour le module SF (gestion d'un seul bloc par fichier)
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sf.h"
#include "inode.h"
#include "bloc.h"

#define NOM_DISQUE "DISQUE_DE_TEST_V2"

int main(void) {
    tSF monSF = NULL;
    long tailleEcrite;

    // 1. Création du système de fichiers
    printf("--- Création du système de fichiers '%s'...\n", NOM_DISQUE);
    monSF = CreerSF(NOM_DISQUE);
    if (monSF == NULL) {
        fprintf(stderr, "Erreur : Impossible de créer le système de fichiers.\n");
        return EXIT_FAILURE;
    }
    printf("Super-bloc créé avec succès.\n");

    // 2. Écriture des fichiers
    printf("Ajout du fichier 'f1' (type ORDINAIRE) au SF...\n");
    const char *contenu1 = "Une Grenouille vit un boeuf\nQui lui sembla de belle taille";
    tailleEcrite = Ecrire1BlocFichierSF(monSF, (char *)contenu1, ORDINAIRE);
    if (tailleEcrite < 0) {
        fprintf(stderr, "Erreur : Impossible d'écrire le fichier 'f1'.\n");
        return EXIT_FAILURE;
    }
    printf("%ld octets écrits dans le SF.\n", tailleEcrite);

    printf("Ajout du fichier 'f2' (type ORDINAIRE) au SF...\n");
    const char *contenu2 = "Bonjour, Monde !";
    tailleEcrite = Ecrire1BlocFichierSF(monSF, (char *)contenu2, ORDINAIRE);
    if (tailleEcrite < 0) {
        fprintf(stderr, "Erreur : Impossible d'écrire le fichier 'f2'.\n");
        return EXIT_FAILURE;
    }
    printf("%ld octets écrits dans le SF.\n", tailleEcrite);

    // 3. Affichage du système de fichiers
    printf("\n--- Affichage du système de fichiers '%s' :\n", NOM_DISQUE);
    AfficherSF(monSF);

    // 4. Destruction du système de fichiers
    printf("\n--- Destruction du système de fichiers '%s'...\n", NOM_DISQUE);
    DetruireSF(&monSF);
    if (monSF == NULL) {
        printf("Système de fichiers détruit avec succès.\n");
    } else {
        fprintf(stderr, "ERREUR: Échec de la destruction du SF.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
