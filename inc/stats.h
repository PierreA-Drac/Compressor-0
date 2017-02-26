/**
 * \file stats.c
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
