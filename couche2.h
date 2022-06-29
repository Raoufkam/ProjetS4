/*******************************************************************
*    Couche 2 :                                                    *
*    Gestion du catalogue, du super bloc, et de la table d'inodes  *
*    Projet S4                                                     *
*                                                                  *
*    P.Alibert - Univ. Toulouse III Paul-Sabatier   2021-2022      *
*                                                                  *
*******************************************************************/

#ifndef couche2_h
#define couche2_h

#include "sos_defines.h"
#include "timestamp.h"


/**
 * @brief Ecrit les infos de la structure super_block sur l'emplacement fichier reserve
 * 
 * @param storage 
 * @param super_block 
 */
void write_super_block(FILE *storage, super_block_t super_block);

/**
 * @brief Recupere les infos du super bloc du fichier et les ecrits sur la structure super_block
 * 
 * @param storage 
 * @param super_block 
 */
void read_super_block(FILE *storage, super_block_t *super_block);

/**
 * @brief Met a jour le champ first_free_byte de la structure super_block
 * 
 */
void update_first_free_byte();

/**
 * @brief Recupere les infos du super bloc du fichier et les ecrits sur la structure super_block
 * 
 * @param storage 
 * @param table_inodes 
 */
void read_inodes_table(FILE *storage, inode_table_t table_inodes);

/**
 * @brief Recupere les infos du super bloc du fichier et les ecrits sur la structure super_block
 * 
 * @param storage 
 * @param table_inodes 
 */
void write_inodes_table(FILE *storage, inode_table_t table_inodes);

/**
 * @brief Retire un inode  de la table d'inodes et met la table a jour
 * 
 * @param indice 
 */
void delete_inode(int indice);

/*  */
/**
 * @brief Cherche l'indice du premier inode disponible de la table
 * 
 * @return int 
 */
int get_unused_inode();

/**
 * @brief Initialise un inode grâce au nom du fichier, sa taille et sa position
 * 
 * @param nom_fichier 
 * @param taille 
 * @param pos 
 */
void init_inode(char nom_fichier[FILENAME_MAX_SIZE], uint taille, uint pos);

/**
 * @brief Affiche sur la sortie les infos de chaque inode de la table
 * 
 */
void cmd_dump_inode();

#endif