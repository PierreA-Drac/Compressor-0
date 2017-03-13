/**
 * \file algo_rle.h
 * \author AYOUB Pierre
 * \date 02 mars 2017
 *
 * \brief RLE.
 * \details Module de l'algorithme RLE (Run-Lenght Encoding).
 */

/* Principe de l'algorithme : le codage par répétition consiste à ce que lorsque
 * que le programme rencontre une répétition consécutive de plusieurs
 * caractères, on note un code qui indique ce nombre de répétition, puis un seul
 * de ces caractères. */

/* Fonctions publiques ====================================================== */

/**
 * Lance la compression RLE sur un fichier entrant et l'inscris sur un fichier
 * sortant.
 * \param cf Pointeur vers une structure de couple fichier entrant/sortant à
 * compresser.
 * \return 0 sur succès, ERR_BAD_ADRESS (affiche message) si le pointeur est
 * nulle ou invalide, ERR_COMPRESSION_FAILED si une erreur survient lors de la
 * compression. */
int rle_compress(cmp_file_s * cf);
