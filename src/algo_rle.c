/**
 * \file algo_rle.c
 * \author AYOUB Pierre
 * \date 02 mars 2017
 *
 * \brief RLE.
 * \details Module de l'algorithme RLE (Run-Lenght Encoding).
 */

/* Fonctionnement détaillé de l'algorithme : On lit les caractères du fichier
 * entrant sur 8 octets. On remarque que les caractères de la table ASCII
 * sont < 128, donc que les octets des caractères ont leurs bit de poids fort à
 * 0. Si on détecte plusieurs caractères identiques à la suite, on ne va noter
 * qu'un seul de ces caractères précédé par un bit à 1 puis d'un code de
 * répétition sur 2 bits indiquant le nombre d'apparition du caractère. Ce
 * premier bit à 1 permettra d'identifier un code de répétition par rapport à un
 * caractère codé sur 8 bits. */

#include <stdio.h>
#include <limits.h>
#include "errors.h"
#include "io.h"
#include "common.h"

/* Macro-constantes privées ================================================= */

/* Valeur maximal du code de répétition. */
#define REP_CODE_MAX ((0b1 << REP_CODE_LENGHT)-1)
/* Nombre de bit du code de répétition (sans compter le premier bit ID.) */
#define REP_CODE_LENGHT 3       /* Valeur optimale déterminée empiriquement. */

/* Fonctions privées ======================================================== */

/* Fonctions de manipulation de bloc par RLE. Ces fonctions sont spécifiques à
 * cet algorithme car RLE utilise un traitement de bloc particulier (écrit dans
 * le sens inverse de la lecture, et gère ses indices de position différemment). 
 * Les prototypes sont rajouté car obligatoire pour l'inlining (sinon, gcc
 * annonce qu'il manque des références sur ces fonctions). */

/* Vide le bloc "blck" dans la structure "cf". Réinitialise le bloc à 0 et la
 * position "pos" à la longueur du bloc. Renvoie 0 sur un succès, -1 sur une
 * erreur. */
static int rle_blck_flush(cmp_file_s * cf, block_t * blck, int *pos);
inline static int rle_blck_flush(cmp_file_s * cf, block_t * blck, int *pos)
{
    assert(cf && blck && pos);
    if (cmpf_put_block(cf, *blck))
        return -1;
    *blck = 0;
    *pos = BLOCK_LENGHT;
    return 0;
}

/* Ajoute un bit à la position "pos" au bloc "blck". Le bloc sera vidé dans "cf"
 * s'il n'y a pas assez de place pour rajouter le bit. La position sera modifiée
 * automatiquement pour l'écriture du bit. Renvoie 0 sur un succès, -1 sur une
 * erreur. */
static int rle_blck_put_bit(cmp_file_s * cf, block_t * blck,
                            const int bit, int *pos);
inline static int rle_blck_put_bit(cmp_file_s * cf, block_t * blck,
                                   const int bit, int *pos)
{
    if (*pos == 0) {            /* Cas où le bloc est plein. */
        if (rle_blck_flush(cf, blck, pos))
            return -1;
    }
    (*pos)--;
    PUT_BIT(*blck, bit, *pos);
    return 0;
}

/* Ajoute le mot "byte" de longueur "w_len" au block "blck" à la position "pos"
 * (bit de poids faible). Écris le mot d'un seul coup s'il est d'une longueur de
 * "CHAR_BIT" (meilleur performance), ou bit par bit s'il est plus petit. Le
 * bloc sera vidé automatiquement dans "cf" s'il n'y a plus de place pour le
 * mot. La position sera modifiée automatiquement pour l'écriture du mot.
 * Renvoie 0 sur un succès, -1 sur une erreur. */
static int rle_blck_put_word(cmp_file_s * cf, block_t * blck, int *pos,
                             const byte_t byte, const int w_len);
inline static int rle_blck_put_word(cmp_file_s * cf, block_t * blck, int *pos,
                                    const byte_t byte, const int w_len)
{
    /* Cas où il n'y a pas la place pour le mot, ou qu'il est plus petit que 8
     * bits et qu'il faut l'écrire bit par bit (code de répétition). */
    if (w_len < CHAR_BIT || *pos < w_len) {
        for (char i = w_len - 1; i >= 0; i--) {
            byte_t bit;
            GET_BIT(byte, bit, i);
            if (rle_blck_put_bit(cf, blck, bit, pos))
                return -1;
        }
    } else {
        *pos -= CHAR_BIT;
        *blck = blck_put_byte(*blck, byte, *pos);
    }
    return 0;
}

/* Fonctions publiques ====================================================== */

int rle_compress(cmp_file_s * cf)
{
    /* Test usuel. */
    if (!cf)
        return err_print(ERR_BAD_ADRESS);

    block_t blck_in = 0, blck_out = 0;  /* Blocs de données. */
    byte_t byte_1 = 0, byte_2 = 0;      /* Octets temporaires pour comparaisons. */
    int count = 1, ind_out = BLOCK_LENGHT;      /* Compteur et indice. */

    /* N.B. : L'indice commence à BLOCK_LENGHT car on écris les caractères dans
     * le sens inverse duquel ont les à lu, pour pouvoir à la décompression
     * détecter les bits de poids fort à 0 d'un caractère et le bit à 1 d'un
     * code de répétition. */

    /* Tant que l'on n'a pas atteint la fin du fichier, on récupère les blocs. */
    while (!(cmpf_get_block(cf, &blck_in))) {
        byte_2 = blck_get_byte(blck_in, 0);
        /* Parsing d'un bloc de données entrant. */
        for (int ind_in = CHAR_BIT; ind_in < BLOCK_LENGHT; ind_in += CHAR_BIT) {
#pragma GCC diagnostic ignored "-Wsequence-point"
            count += (byte_1 = byte_2) == (byte_2 =
                                           blck_get_byte(blck_in, ind_in));
#pragma GCC diagnostic pop
            if (count == 1) {   /* Cas sans répétition, écriture du caractère. */
                if (rle_blck_put_word
                    (cf, &blck_out, &ind_out, byte_1, CHAR_BIT))
                    return ERR_COMPRESSION_FAILED;
            } else if (byte_1 != byte_2 || count == REP_CODE_MAX) {     /* Cas avec répétition. */
                /* Si la répétition est terminée ou qu'on dépasse la taille du
                 * code de répétition. */
                /* Éciture de l'ID d'un code : 1 bit à 1. */
                PUT_BIT(count, 1, REP_CODE_LENGHT);
                /* Écriture du code de répétition. */
                if (rle_blck_put_word(cf, &blck_out, &ind_out, count,
                                      REP_CODE_LENGHT + 1)) {
                    return ERR_COMPRESSION_FAILED;
                }
                count = 1;      /* Réinit. */
                /* Caractère à répeter. */
                if (rle_blck_put_word
                    (cf, &blck_out, &ind_out, byte_1, CHAR_BIT))
                    return ERR_COMPRESSION_FAILED;
            }
            /* Sinon, on est dans une répétition non terminée et count s'est
             * juste incrémenté de 1. */
        }
        byte_1 = byte_2;        /* Switch avant la lecture du premier caractère. */
    }
    return 0;
}
