/*******************************************************************
*    Couche 1 : les blocs                                          *
*    Gestion de l'écriture d'un bloc sur le disque                 *
*    Projet S4                                                     *
*                                                                  *
*    K.Mohamed - Univ. Toulouse III Paul-Sabatier   2021-2022      *
*                                                                  *
*******************************************************************/

#include "couche1.h"
#include "couche2.h"
#include "couche3.h"

extern virtual_disk_t virtual_disk_sos;

/**
 * @brief Initialise la structure global virtual_disk_sos
 * 
 * @param nom_repertoire 
 */
void init_disk_sos(char nom_repertoire[])
{
    char file_emplacement[50] = "";
    strcat(file_emplacement,nom_repertoire);
    strcat(file_emplacement,"/d0");
    virtual_disk_sos.storage = fopen(file_emplacement,"rb+");
    read_super_block(virtual_disk_sos.storage, &(virtual_disk_sos.super_block));
	read_inodes_table(virtual_disk_sos.storage, virtual_disk_sos.inodes);
    read_users_table();
}

/**
 * @brief Eteint le systeme en prenant soin de sauvegarder toute les modifications effectuées et renvoie un code retour
 * 
 * @param exit_sys 
 */
void shut_system(int exit_sys, int cr)
{
	write_super_block(virtual_disk_sos.storage, virtual_disk_sos.super_block);
	write_inodes_table(virtual_disk_sos.storage, virtual_disk_sos.inodes);
	write_users_table();
	fclose(virtual_disk_sos.storage);
    if(exit_sys) exit(cr);
}

uint compute_nblock(uint n_octets){
	return (n_octets+BLOCK_SIZE-1)/BLOCK_SIZE;
}

/**
 * @brief Ecrit le bloc block a la position pos sur le fichier
 * 
 * @param block 
 * @param pos 
 */
void write_block(block_t block, long pos){
	
	/**
	 *  On commence par positioner le curseur à la position donnée en paramètre
	 * 	-> On considère que le fichier du virtual_disk_sos est ouvert dans init_disk_sos(char* nom_repertoire) 
	 */
	fseek(virtual_disk_sos.storage, pos, SEEK_SET);
	fwrite(&block,BLOCK_SIZE, 1, virtual_disk_sos.storage);
}

/**
 * @brief Lit un bloc à la position pos sur le fichier
 * 
 * @param block 
 * @param pos 
 */
void read_block(block_t *block, long pos){
	
	/**
	 *  On commence par positioner le curseur à la psotion 'pos'
	 *  -> On considère que le fichier du virtual_disk_sos est ouvert dans init_disk_sos(char* nom_repertoire) 
	 */
	fseek(virtual_disk_sos.storage,pos,SEEK_SET);
	fread(block,BLOCK_SIZE,1,virtual_disk_sos.storage );
	if (ferror(virtual_disk_sos.storage)) {
		fprintf(stderr, "Lecture du bloc impossible dans le fichier\n");
		shut_system(1, 1);
		exit(EXIT_FAILURE);
	}
}

