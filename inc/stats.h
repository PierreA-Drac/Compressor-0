/**
 * \file stats.h
 * \author AYOUB Pierre
 * \date 20 février 2017
 *
 * \brief Statistiques.
 * \details Module de génération et d'affichage des statistiques sur le
 * processus et les fichiers.
 */

#ifndef __STATS_H
#define __STATS_H

/* Fonctions publiques ====================================================== */

/**
 * Initialise les variables nécéssaire à la génération des statistiques du
 * programme.
 */
void stat_init();

/**
 * Affiche les statistiques sur le programme et les fichiers traités sur la
 * sortie standard. La fonction stat_init doit être appellée avant stat_print.
 * \param filepath_in Chemin vers le fichier entrant.
 * \param filepath_out Chemin vers le fichier sortant.
 * \return 0 sur un succès, ou -1 sur une erreur.
 * \error ERR_STAT si une erreur est survenu.
 */
int stat_print(const char *s_filepath_in, const char *s_filepath_out);

#endif
