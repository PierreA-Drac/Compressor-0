/** 
 * \file errors.c
 * \author AYOUB Pierre
 * \date 29 janvier 2017
 *
 * \brief Erreurs.
 * \details Module de gestion des erreurs. Contient les fonctions de gestion des
 * erreurs ainsi que les définitions des codes d'erreurs.
 */

#include <stdio.h>
#include <stdlib.h>
#include "errors.h"

/* Fonctions publiques ====================================================== */

err_code_e p_error(const err_code_e err)
{
    fprintf(stderr, "Erreur : ");
    switch (err) {
        case ERR_NONE:
            break;
        case ERR_BAD_ADRESS:
            fprintf(stderr, "L'adresse spécifiée est invalide (ou nulle).\n");
            break;
        case ERR_MISSING_OPTIONS:
            fprintf(stderr, "Une option requise est manquante pour le "
                    "fonctionnement du programme.\n");
            break;
        case ERR_PRINT_STAT:
            fprintf(stderr, "Récupération des statistiques impossible.\n");
            break;
        case ERR_FREAD:
            fprintf(stderr, "Lecture du fichier impossible.\n");
            break;
        case ERR_FREAD_EOF:
            fprintf(stderr, "Lecture du fichier impossible car la fin à été"
                    "atteinte.\n");
            break;
        case ERR_FWRITE:
            fprintf(stderr, "Écriture du fichier impossible.\n");
            break;
        case ERR_FCLOSE:
            fprintf(stderr, "Fermeture du fichier impossible.\n");
            break;
    }
    return err;
}

void print_help(FILE * const stream, const int exit_code)
{
    fprintf(stream,
            "Affichage de l'aide :\n\n"
            "Synopsis :\n"
            "\tcompressor-0 -c|-d -i INPUT FILE [-o OUTPUT FILE]"
            "[ALGORITHM FLAG] [-s] [-h]\n\n"
            "Options :\n"
            "\t-h, --help\n"
            "\t\tAffiche l'aide sur la sortie standard.\n\n"
            "\t-c, --compress\n"
            "\t\tMode de compression du fichier entrant.\n\n"
            "\t-d, --decompress\n"
            "\t\tMode de décompression du fichier entrant. Implique de préciser"
            " un algorthime.\n\n"
            "\t-s, --statistics\n"
            "\t\tAffiche les statistiques de la compression ou de la "
            "décompression effectuée sur la sortie standard.\n\n"
            "\t-i INPUT FILE, --input=INPUT FILE\n"
            "\t\tChemin vers le fichier entrant à traiter.\n\n"
            "\t-o OUTPUT FILE, --output=OUTPUT FILE\n"
            "\t\tChemin vers le fichier sortant résultant. Si non spécifié, le "
            "fichier sortant gardera le nom du fichier source et sera écrit\n "
            "\t\tdans le répertoire \"out/\" situé dans le répertoire de "
            "l'exécutable.\n\n"
            "Algorithmes :\n"
            "\t--LZMA\n"
            "\t\tCompresse le fichier en utilisant l'algorithme LZMA. (Ici à "
            "titre d'exemple, algorithme non implémenté).\n\n"
            "Exemples :\n"
            "\tcompressor -c -i env/corpus/text.txt -o text.cmp --LZMA -s\n\n"
            "\tcompressor --decompress --input=\"text.cmp\""
            "--output=\"text.txt\"\n\n");
    exit(exit_code);
}
