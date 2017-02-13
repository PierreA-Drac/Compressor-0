/** 
 * \file io.h
 * \author AYOUB Pierre
 * \date 29 janvier 2017
 *
 * \brief Entrées/sorties.
 * \details Module de gestion des entrées/sorties, sur le disque ou sur les
 * entrée/sortie standards (hormis les erreurs).
 */

#ifndef __IO_H
#define __IO_H

#include <stdio.h>
#include <stdint.h>

/* Structures publiques ===================================================== */

typedef struct compress_file compress_file_s;

/** Correspond à un fichier en cours de traitement. */
struct compress_file {
    FILE *fd_in;                /*!< Fichier entrant. */
    FILE *fd_out;               /*!< Fichier sortant. */
    uint64_t *p_stream;         /*!< Flux contenant les données à compresser. */
};

/* Fonctions publiques ====================================================== */

/**
 * Initialise les flux vers les fichiers entrant et sortant, et assure les
 * routines de détection d'erreurs. Quitte le programme avec EXIT_FAILURE si une
 * erreur survient.
 * \param filepath_in Chemin vers le fichier entrant.
 * \param filepath_out Chemin vers le fichier sortant.
 * \return compressed_file_s Structure contenant les flux des fichiers.
 */
compress_file_s init_cmp_file(const char *s_filepath_in,
                              const char *s_filepath_out);

/**
 * Initialise les variables nécéssaire à la génération des statistiques du
 * programme.
 */
void init_stat();

/**
 * Affiche les statistiques sur le programme et les fichiers traités sur la
 * sortie standard. La fonction init_stat doit être appellée avant print_stat.
 * \param filepath_in Chemin vers le fichier entrant.
 * \param filepath_out Chemin vers le fichier sortant.
 * \return Si succès 0 est retourné, si erreur -1 est retourné.
 */
int print_stat(const char *s_filepath_in, const char *s_filepath_out);

#endif
