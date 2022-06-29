/********************************************************************
 *    Couche 1 : les blocs                                          *
 *    Gestion de l'écriture d'un bloc sur le disque                 *
 *    Projet S4                                                     *
 *                                                                  *
 *    K.Mohamed - Univ. Toulouse III Paul-Sabatier  2021-2022       *
 *                                                                  *
 ********************************************************************/

#ifndef couche1_h
#define couche1_h

#include "sos_defines.h"

/**
 * @brief Initialise la structure global virtual_disk_sos
 * 
 * @param nom_repertoire 
 */
void init_disk_sos(char nom_repertoire[]);

/**
 * @brief Eteint le systeme en prenant soin de sauvegarder toute les modifications effectuées et renvoie un code retour
 * 
 * @param exit_sys 
 */
void shut_system(int exit_sys, int cr);

uint compute_nblock(uint n_octets);

/**
 * @brief Ecrit le bloc block a la position pos sur le fichier
 * 
 * @param block 
 * @param pos 
 */
void write_block(block_t block, long pos);

/**
 * @brief Lit un bloc à la position pos sur le fichier
 * 
 * @param block 
 * @param pos 
 */
void read_block(block_t *block, long pos);

#endif

