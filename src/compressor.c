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
    cmp_file_s *cf = cmpf_open(pi.s_input_file, pi.s_output_file);

    /* Partie compression. */

#pragma GCC diagnostic ignored "-Wswitch"
    if (pi.mode == MODE_COMPRESS) {
        switch (pi.algo) {
            case ALGO_RLE:
                rle_compress(cf);
                break;
        }
        if (CMP_err == ERR_COMPRESSION_FAILED)
            return err_print(CMP_err), -1;
    } else {
        switch (pi.algo) {
            case ALGO_RLE:
                rle_decompress(cf);
                break;
        }
        if (CMP_err == ERR_DECOMPRESSION_FAILED)
            return err_print(CMP_err), -1;
    }
#pragma GCC diagnostic pop

    /* Fin du programme. */

    /* Fermeture des flux. */
    if (cmpf_close(cf))
        return err_print(CMP_err), -1;
    /* Génération des statistiques si demandé. */
    if (pi.stat && stat_print(pi.s_input_file, pi.s_output_file))
        err_print(ERR_STAT);
    return 0;
}
