/** 
 * \file io.h
 * \author AYOUB Pierre
 * \date 29 janvier 2017
 *
 * \brief Entrées/sorties.
 * \details Module de gestion des entrées/sorties sur le disque.
 */

#include <stdlib.h>
#include "io.h"

/* Fonctions publiques ====================================================== */

compress_file_s init_cmp_file(const char *s_filepath_in,
                              const char *s_filepath_out)
{
    compress_file_s cf;
    cf.p_stream = NULL;
    if (!(cf.fp_in = fopen(s_filepath_in, "rb")) ||
        !(cf.fp_out = fopen(s_filepath_out, "wb"))) {
        perror("fopen in init_cmp_file");
        exit(EXIT_FAILURE);
    }
    return cf;
}
