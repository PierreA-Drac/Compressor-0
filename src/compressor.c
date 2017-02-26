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
    /* Initialisation du programme (si une fonction échoue, le programme est
     * terminé automatiquement par ces fonctions). */

    /* Récupérations des paramètres. */
    const prog_info_s pi = init_prog(argc, argv);
    /* Initialisation de la génération des statistiques. */
    if (pi.stat)
        init_stat();
    /* Ouverture des flux. */
    cmp_file_s cmp_f = cmpf_open(pi.s_input_file, pi.s_output_file);

    /* Partie compression. */

    /* Fin du programme. */

    /* Fermeture des flux. */
    if (cmpf_close(&cmp_f))
        return p_error(ERR_FCLOSE);
    /* Génération des statistiques si demandé. */
    if (pi.stat && print_stat(pi.s_input_file, pi.s_output_file))
        p_error(ERR_PRINT_STAT);
    return 0;
}
