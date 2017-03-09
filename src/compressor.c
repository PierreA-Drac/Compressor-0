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
#include "errors.h"
#include "init.h"
#include "io.h"
#include "stats.h"
#include "algo_rle.h"

/* Point d'entrée =========================================================== */

int main(int argc, char *argv[])
{
    /* Initialisation du programme (si une fonction échoue, le programme est
     * terminé automatiquement par ces fonctions). */

    /* Récupérations des paramètres. */
    const prog_info_s pi = init_prog(argc, argv);
    /* Initialisation de la génération des statistiques. */
    if (pi.stat)
        stat_init();
    /* Ouverture des flux. */
    cmp_file_s cf = cmpf_open(pi.s_input_file, pi.s_output_file);

    /* Partie compression. */

    int status = 0;
    if (pi.mode == MODE_COMPRESS) {
        switch (pi.algo) {
            case ALGO_NONE:    /* Avoid gcc warning. */
                break;
            case ALGO_RLE:
                status = rle_compress(&cf);
                break;
        }
        if (status)
            return err_print(ERR_COMPRESSION_FAILED);
    } else {
        switch (pi.algo) {
            case ALGO_NONE:
                break;
            case ALGO_RLE:
                break;
        }
        if (status)
            return err_print(ERR_DECOMPRESSION_FAILED);
    }

    /* Fin du programme. */

    /* Fermeture des flux. */
    if (cmpf_close(&cf))
        return err_print(ERR_IO_FCLOSE);
    /* Génération des statistiques si demandé. */
    if (pi.stat && stat_print(pi.s_input_file, pi.s_output_file))
        err_print(ERR_STAT_PRINT);
    return 0;
}
