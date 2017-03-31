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

void err_print(const err_code_e err)
{
    static const char *err_desc[] = {
        "aucune erreur",
        "l'adresse spécifiée est invalide",
        "une option requise est manquante pour le fonctionnement du programme",
        "récupération des statistiques impossible",
        "lecture du fichier impossible",
        "lecture du fichier impossible car la fin à été atteinte",
        "écriture du fichier impossible",
        "fermeture du fichier impossible",
        "compression du fichier impossible",
        "décompression du fichier impossible"
    };
    (unsigned int)err <= ERR_DECOMPRESSION_FAILED ?
        fprintf(stderr, "Erreur %d : %s.\n", err, err_desc[err]) :
        fprintf(stderr, "Erreur inconnu.\n");
}

void help_print(FILE * const p_stream, const int exit_code, const char *s_name)
{
    fprintf(p_stream,
            "Affichage de l'aide :\n\n"
            "Synopsis :\n"
            "\t%s -c|-d -i INPUT FILE [-o OUTPUT FILE]"
            "[ALGORITHM FLAG] [-s] [-h]\n\n"
            "Options :\n"
            "\t-h, --help\n"
            "\t\tAffiche l'aide sur la sortie standard.\n\n"
            "\t-c, --compress\n"
            "\t\tMode de compression du fichier entrant.\n\n"
            "\t-d, --decompress\n"
            "\t\tMode de décompression du fichier entrant.\n"
            "\t\tImplique de préciser un algorthime.\n\n"
            "\t-s, --statistics\n"
            "\t\tAffiche les statistiques de la compression ou de la\n"
            "\t\tdécompression effectuée sur la sortie standard.\n\n"
            "\t-i INPUT FILE, --input=INPUT FILE\n"
            "\t\tChemin vers le fichier entrant à traiter.\n\n"
            "\t-o OUTPUT FILE, --output=OUTPUT FILE\n"
            "\t\tChemin vers le fichier sortant résultant. Si non spécifié,\n"
            "\t\tle fichier sortant gardera le nom du fichier source et\n"
            "\t\tsera écrit dans le répertoire \"out/\" situé dans le\n"
            "\t\trépertoire de l'exécutable.\n\n"
            "Algorithmes :\n"
            "\t--RLE\n"
            "\t\tCompresse le fichier en utilisant l'algorithme RLE\n"
            "\t\t(Run-Lenght Encoding). L'algorithme nécéssite\n"
            "\t\tobligatoirement un fichier encodé en ASCII pour fonctionner.\n\n"
            "Exemples :\n"
            "\t%s -c -i env/corpus/text.txt -o text.cmp --RLE -s\n\n"
            "\t%s --decompress --input=\"text.cmp\" "
            "--output=\"text.txt\"\n\n", s_name, s_name, s_name);
    exit(exit_code);
}
