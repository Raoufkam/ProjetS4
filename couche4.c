/********************************************************************
 *    Couche 4 :                                                    *
 *    Gestion des fichiers et des droits                            *
 *    Projet S4                                                     *
 *                                                                  *
 *    K.Mohamed - Univ. Toulouse III Paul-Sabatier   2021-2022      *
 *                                                                  *
 ********************************************************************/
 
#include "couche2.h"
#include "couche1.h"
#include "couche4.h"

extern virtual_disk_t virtual_disk_sos;
extern session_t session;

/**
 * @brief Ecrit le fichier, dont le nom est dans la chaine filename, sur le systeme
 * 
 * @param filename 
 * @param fichier 
 * @return uint 
 */
uint write_file(char filename[], file_t fichier){
	
    uint position;
	int found = 0;
	for(int i = 0; i < virtual_disk_sos.super_block.number_of_files; i++)
	{
		if(!strcmp(virtual_disk_sos.inodes[i].filename, filename)){
			found = 1;
			/*
			 * Dans le cas ou le fichier est de taille inférieur ou égale
			 * --> Mettre à jour la table du inode
			 */
			if (virtual_disk_sos.inodes[i].size >= fichier.size)
			{
				virtual_disk_sos.inodes[i].size = fichier.size;
				strcpy(virtual_disk_sos.inodes[i].mtimestamp, timestamp());
				virtual_disk_sos.inodes[i].nblock = compute_nblock(fichier.size);
				position = virtual_disk_sos.inodes[i].first_byte;
			}
			
			/*
			 * Dans le cas ou le fichier est de taille supérieur 
			 * --> Suppression du inode et création d'un nouveau à la fin du disque
			 */
			 
			else if (virtual_disk_sos.inodes[i].size < fichier.size)
			{
				delete_inode(i);
				fseek(virtual_disk_sos.storage,0,SEEK_END);
				position = ftell(virtual_disk_sos.storage);
				init_inode(filename, fichier.size, position);
			}
		}
		
	}

	if(!found){
		if(virtual_disk_sos.super_block.number_of_files >= INODE_TABLE_SIZE)
			return 0;
		fseek(virtual_disk_sos.storage,0,SEEK_END);
		position = ftell(virtual_disk_sos.storage);
		init_inode(filename, fichier.size, position);
	}

	/*
	* Ecriture des Blocks 
	*/
	block_t *block = &(fichier.data[0]);
	for(int j = 0; j < compute_nblock(fichier.size); j++)
	{
		write_block(*block, position+j*BLOCK_SIZE);
        block++;
	}
	return 1;
}

/**
 * @brief Recupere un fichier dans le systeme dont le nom correspond a la chaine filename
 * 
 * @param filename 
 * @param fichier 
 * @return uint 
 */
uint read_file(char filename[], file_t *fichier){	
	
	for(int i = 0; i<virtual_disk_sos.super_block.number_of_files; i++)
	{
		/*
		 * Dans le cas où le 'fichier' est sur le disque | Lecture des blocks | Retour --> 1
		 */
		 
		if(!strcmp(virtual_disk_sos.inodes[i].filename, filename))
		{
			
			fichier->size = virtual_disk_sos.inodes[i].size;
			block_t *block;
			block = &(fichier->data[0]);
			for(int j=0; j< virtual_disk_sos.inodes[i].nblock ;j++)
			{
				read_block(block, virtual_disk_sos.inodes[i].first_byte+j*BLOCK_SIZE);
				block++;
			}
			return 1; 
		}
	}	
	return 0;
}
	
/**
 * @brief Efface un fichier du systeme dont le nom correspond a la chaine filename
 * 
 * @param filename 
 * @return uint 
 */
uint delete_file(char filename[]){

	for(int i=0 ; i < virtual_disk_sos.super_block.number_of_files ; i++)
	{
		if(!strcmp(virtual_disk_sos.inodes[i].filename, filename))
		{
			delete_inode(i);
			return 1;
		}
	}
	update_first_free_byte();

	return 0;
}

/**
 * @brief Ecrit un fichier de l'ordinateur sur le systeme virtuel
 * 
 * @param filename 
 */
void load_file_from_host(char filename[]){
	FILE *host = fopen(filename, "rb");
	file_t file;
	int i = 0;
	while(!feof(host)){
		fread(&(file.data[i]), BLOCK_SIZE, 1, host);
		i+=4;
	}
	fseek(host, 0, SEEK_END);
	file.size = ftell(host);
	write_file(filename, file);
	fclose(host);
}

/**
 * @brief Ecrit sur l'ordinateur un fichier du systeme virtuel
 * 
 * @param filename 
 */
void store_file_to_host(char filename[]){
	FILE *host = fopen(filename, "wb");
	
	for(int i = 0; i < virtual_disk_sos.super_block.number_of_files; i++){
		if(!strcmp(virtual_disk_sos.inodes[i].filename, filename)){
			file_t file;
			read_file(filename, &file);
			fwrite(file.data, file.size, 1, host);
		}
	}
	fclose(host);
}