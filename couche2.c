/*******************************************************************
*    Couche 2 :                                                    *
*    Gestion du catalogue, du super bloc, et de la table d'inodes  *
*    Projet S4                                                     *
*                                                                  *
*    P.Alibert - Univ. Toulouse III Paul-Sabatier   2021-2022      *
*                                                                  *
*******************************************************************/

#include "couche2.h"
#include "couche1.h"

extern virtual_disk_t virtual_disk_sos;
extern session_t session;

/**
 * @brief Ecrit les infos de la structure super_block sur l'emplacement systeme reserve
 * 
 * @param storage 
 * @param super_block 
 */
void write_super_block(FILE *storage, super_block_t super_block){
    block_t *block = &super_block;
    for(int i = 0; i < SUPER_BLOCK_SIZE; i++){
        write_block(*block, i*BLOCK_SIZE);
        block++;
    }
}

/**
 * @brief Recupere les infos du super bloc du systeme et les ecrits sur la structure super_block
 * 
 * @param storage 
 * @param super_block 
 */
void read_super_block(FILE *storage, super_block_t *super_block){
    block_t *block = super_block;
    for(int i = 0; i < SUPER_BLOCK_SIZE; i++){
        read_block(block, i*BLOCK_SIZE);
        block++;
    }
}

/**
 * @brief Met a jour le champ first_free_byte de la structure super_block
 * 
 */
void update_first_free_byte(){
    int i = INODES_START+INODE_TABLE_SIZE*INODE_SIZE*BLOCK_SIZE;
    block_t read_b;
    read_block(&read_b, i);
    while(read_b.data[0] != 0 && read_b.data[1] != 0 && read_b.data[2] != 0 && read_b.data[3] != 0){
        i += BLOCK_SIZE;
        read_block(&read_b, i);
    }
    if(read_b.data[0] == 0);
    else if(read_b.data[1] == 0) i++;
    else if(read_b.data[2] == 0) i+=2;
    else if(read_b.data[3] == 0) i+=3;
    virtual_disk_sos.super_block.first_free_byte = i;
}

/**
 * @brief Recupere les infos du super bloc du systeme et les ecrits sur la structure super_block
 * 
 * @param storage 
 * @param table_inodes 
 */
void read_inodes_table(FILE *storage, inode_table_t table_inodes){
    fseek(storage, INODES_START, SEEK_SET);
    for(int i = 0; i<virtual_disk_sos.super_block.number_of_files; i++){
        fread(&table_inodes[i], BLOCK_SIZE, INODE_SIZE, storage);
    }
}

/**
 * @brief Recupere les infos du super bloc du systeme et les ecrits sur la structure super_block
 * 
 * @param storage 
 * @param table_inodes 
 */
void write_inodes_table(FILE *storage, inode_table_t table_inodes){
    fseek(storage, INODES_START, SEEK_SET);
   for(int i = 0; i<virtual_disk_sos.super_block.number_of_files; i++){
       fwrite(&table_inodes[i], BLOCK_SIZE, INODE_SIZE, storage);
   }
}

/**
 * @brief Retire un inode de la table d'inodes et met la table a jour
 * 
 * @param indice 
 */
void delete_inode(int indice){
    virtual_disk_sos.super_block.nb_blocks_used -= virtual_disk_sos.inodes[indice].nblock;
    for(int i = indice; i+1<virtual_disk_sos.super_block.number_of_files; i++){
      virtual_disk_sos.inodes[i] = virtual_disk_sos.inodes[i+1];
    }
    virtual_disk_sos.super_block.number_of_files--;
}

/*  */
/**
 * @brief Cherche l'indice du premier inode disponible de la table
 * 
 * @return int 
 */
int get_unused_inode(){
    return (int) virtual_disk_sos.super_block.number_of_files;
}

/**
 * @brief Initialise un inode grâce au nom du fichier, sa taille et sa position
 * 
 * @param nom_fichier 
 * @param taille 
 * @param pos 
 */
void init_inode(char nom_fichier[FILENAME_MAX_SIZE], uint taille, uint pos){
    inode_t inode;
    char *date;
    strcpy(inode.filename, nom_fichier);
    inode.size = taille;
    inode.first_byte = pos;
    inode.uid = session.userid;
    inode.uright = RW;
    inode.oright = rw;
    inode.nblock = (inode.size+BLOCK_SIZE-1)/BLOCK_SIZE;
    date = timestamp();
    strcpy(inode.ctimestamp, date);
    strcpy(inode.mtimestamp, date);
    virtual_disk_sos.inodes[get_unused_inode()] = inode;
    virtual_disk_sos.super_block.number_of_files++;
    virtual_disk_sos.super_block.nb_blocks_used += inode.nblock;
}

/**
 * @brief Affiche sur la sortie les infos de chaque inode de la table
 * 
 */
void cmd_dump_inode(){
    for(int i = 0; i<virtual_disk_sos.super_block.number_of_files; i++){
        inode_t inode = virtual_disk_sos.inodes[i];
        printf("Inode %d: \n", i);
        printf("    - Nom: %s\n", inode.filename);
        printf("    - Taille: %d\n", inode.size);
        printf("    - User ID: %d\n", inode.uid);
        printf("    - User Rights: %d\n", inode.uright);
        printf("    - Others Rights: %d\n", inode.oright);
        printf("    - Date Création: %s\n", inode.ctimestamp);
        printf("    - Date Modification: %s\n", inode.mtimestamp);
        printf("    - Nombre Blocks: %d\n", inode.nblock);
        printf("    - Premier Octet: %d\n\n", inode.first_byte);
    }
}