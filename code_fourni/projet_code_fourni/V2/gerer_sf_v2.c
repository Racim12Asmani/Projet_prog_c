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
#include "bloc.h" // Pour utiliser TAILLE_BLOC dans les tests

#define NOM_DISQUE "DISQUE_DE_TEST_V2"

int main(void) {
    tSF monSF = NULL;
    long tailleEcrite;

    // --- Initialisation ---
    // 1. Création du Système de Fichiers (SF)
    printf("\n--- 1. CRÉATION DU SF : '%s' ---\n", NOM_DISQUE);
    monSF = CreerSF(NOM_DISQUE);
    if (monSF == NULL) {
        fprintf(stderr, "ERREUR FATALE: Échec de la création du SF.\n");
        return EXIT_FAILURE;
    }
    printf("Système de fichiers créé avec succès.\n");

    // 2. Écriture de fichiers (max 1 bloc)
    printf("\n--- 2. ÉCRITURE DE FICHIERS ---\n");

    // Fichier 1 : Contenu court (Ordinaire)
    const char *contenu1 = "Contenu test tres court.";
    tailleEcrite = Ecrire1BlocFichierSF(monSF, (char *)contenu1, ORDINAIRE);
    printf("-> Fichier 1 ('%s') écrit, taille: %ld octets.\n", contenu1, tailleEcrite);

    // Fichier 2 : Répertoire (Le contenu est juste le nom)
    const char *contenu2 = "dossier_racine";
    tailleEcrite = Ecrire1BlocFichierSF(monSF, (char *)contenu2, REPERTOIRE);
    printf("-> Fichier 2 (Répertoire) écrit, taille: %ld octets.\n", tailleEcrite);

    // Fichier 3 : Contenu plus long que TAILLE_BLOC pour tester la troncature
    char contenu3[TAILLE_BLOC + 50];
    // LIGNE 44 : Début de la chaîne de 'X'
    memset(contenu3, 'X', TAILLE_BLOC + 49);
    contenu3[TAILLE_BLOC + 49] = '\0'; // Assure la terminaison

    tailleEcrite = Ecrire1BlocFichierSF(monSF, contenu3, ORDINAIRE);
    printf("-> Fichier 3 (Tronqué) écrit, taille: %ld octets (attend: %d).\n", tailleEcrite, TAILLE_BLOC);

    // Fichier 4 : Contenu pour afficher "Bonjour Monde !" (remplace les A)
    // LIGNES 53 à 56 : Ancien code pour les 'A' (Maintenant commenté)
    /*
    char contenu4[TAILLE_BLOC + 1];
    // LIGNE 54 : Début de la chaîne de 'A' (ANCIEN)
    memset(contenu4, 'A', TAILLE_BLOC);
    contenu4[TAILLE_BLOC] = '\0';
    tailleEcrite = Ecrire1BlocFichierSF(monSF, contenu4, ORDINAIRE);
    printf("-> Fichier 4 (Taille exacte) écrit, taille: %ld octets (attend: %d).\n", tailleEcrite, TAILLE_BLOC);
    */

    // NOUVEAU CODE : Écriture de "Bonjour Monde !"
    const char *message_bonjour = "Bonjour Monde !";
    tailleEcrite = Ecrire1BlocFichierSF(monSF, (char *)message_bonjour, ORDINAIRE);
    printf("-> Fichier 4 (Bonjour Monde !) écrit, taille: %ld octets.\n", tailleEcrite);


    // 3. Affichage du SF
    printf("\n--- 3. AFFICHAGE DU SF ---\n");
    printf("\n");
    AfficherSF(monSF);

    // 4. Destruction du SF
    printf("\n--- 4. DESTRUCTION DU SF ---\n");
    DetruireSF(&monSF);
    if (monSF == NULL) {
        printf("Système de fichiers détruit avec succès.\n");
    } else {
        fprintf(stderr, "ERREUR: Échec de la destruction du SF.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
