/*******************************************************************
*    Couche 3 :                                                    *
*    Gestion des utilisateurs                                      *
*    Projet S4                                                     *
*                                                                  *
*    P.Alibert - Univ. Toulouse III Paul-Sabatier   2021-2022      *
*                                                                  *
*******************************************************************/

#include "couche1.h"
#include "couche2.h"

extern virtual_disk_t virtual_disk_sos;
extern session_t session;

/**
 * @brief Recupere les infos de la table utilisateur et les mets dans la structure table_users
 * 
 */
void read_users_table(){
    block_t *block;
    block = &(virtual_disk_sos.users_table);
    int j = 0;
    while(j < virtual_disk_sos.super_block.number_of_files && strcmp(virtual_disk_sos.inodes[j].filename, "passwd")){
        j++;
    }
    for(int i = 0; i < compute_nblock(sizeof(users_table_t)); i++){
        read_block(block, virtual_disk_sos.inodes[j].first_byte+i*BLOCK_SIZE);
        block++;
    }
}

/**
 * @brief Recupere les infos dans la structure table_users et les ecrits sur le systeme
 * 
 */
void write_users_table(){
    block_t *block;
    block = &(virtual_disk_sos.users_table);
    int j = 0;
    while(j < virtual_disk_sos.super_block.number_of_files && strcmp(virtual_disk_sos.inodes[j].filename, "passwd")){
        j++;
    }
    for(int i = 0; i < compute_nblock(sizeof(users_table_t)); i++){
        write_block(*block, virtual_disk_sos.inodes[j].first_byte+i*BLOCK_SIZE);
        block++;
    }
}

/**
 * @brief Initialise l'utilisateur 0 (root) dans la structure table_users
 * 
 * @param login 
 * @param password 
 */
void create_root(char login[], char password[]){
    for(int i = 0; i < NB_USERS; i++){
        strcpy(virtual_disk_sos.users_table[i].login, "0");
    }
    strcpy(virtual_disk_sos.users_table[0].login, login);
    strcpy(virtual_disk_sos.users_table[0].passwd, password);
}