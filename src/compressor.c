/** 
 * \file compressor.c
 * \author AYOUB Pierre
 * \date 18 janvier 2017
 *
 * \brief Main.
 * \details Organise le main programme, et contient éventuellement certaines
 * définitions de fonctions du main pour regrouper les grandes étapes.
 */

#include <stdio.h>
#include <stdlib.h>
#include "init.h"
#include "io.h"
#include "errors.h"

/* Point d'entrée =========================================================== */

int main(int argc, char *argv[])
{
    /* Initialisation du programme. */

    /* Récupérations des paramètres. */
    const prog_info_s prog_info = init_prog(argc, argv);
    /* Initialisation de la génération des statistiques. */
    if (prog_info.stat)
        init_stat();
    /* Ouverture des flux. */
    compress_file_s cmp_f = init_cmp_file(prog_info.s_input_file,
                                          prog_info.s_output_file);

    /* Partie compression. */

    /* Fin du programme. */

    /* Génération des statistiques si demandé. */
    if (prog_info.stat && print_stat(prog_info.s_input_file,
                                     prog_info.s_output_file))
        p_error(ERR_PRINT_STAT);
    /* Libération de la mémoire. */
    fclose(cmp_f.fp_in);
    fclose(cmp_f.fp_out);
    return 0;
}
