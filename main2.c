#include "couche2.h"
#include "couche1.h"
#include "couche4.h"
#include "couche5.h"
#include "sos_defines.h"


virtual_disk_t virtual_disk_sos;
session_t session;


/* main de test à compiler avec timestamp.c */
int main(int argc, char *argv[]){
   
    int cr;
    pid_t pid;

    switch (pid = fork())
    {
    case -1:
        fprintf(stderr, "echec fork\n");
        exit(1);
        break;
    case 0:
        execl("cmd_format", "./cmd_format", argv[1], "500", NULL);
        fprintf(stderr, "echec disk\n");
        exit(2);
        break;
        
    default:
        wait(&cr);
        break;
    }
    
    // Initialise le super block, la table d'inodes et la session
    super_block_t sup_b = {0, 1, 0, INODES_START+INODE_TABLE_SIZE*INODE_SIZE*BLOCK_SIZE};
    session.userid = 0;
    init_disk_sos("DiskDir");
    virtual_disk_sos.super_block = sup_b;
    for(int i = 0; i < NB_USERS; i++){
        strcpy(virtual_disk_sos.users_table[i].login, "0");
    }
    strcpy(virtual_disk_sos.users_table[0].login, "Pierre");
    virtual_disk_sos.super_block.number_of_users = 1;
    // Ecrit le super block et la table d'inodes
    write_super_block(virtual_disk_sos.storage, virtual_disk_sos.super_block);
    init_inode("test", 25, 46);
    init_inode("test2", 35, 67);

    write_inodes_table(virtual_disk_sos.storage, virtual_disk_sos.inodes);
    // initialise 2 inodes
    
    // Affiche la table
    cmd_dump_inode();
    // Retire la première inode
    delete_inode(0);
    // Affiche la table
    cmd_dump_inode();
    load_file_from_host("test.bin");
    /*file_t file;
    read_file("test.bin", &file); 
    printf("%s\n", file.data);*/
    store_file_to_host("test.bin");
    cmd_dump_inode();
    super_block_t sup;
    read_super_block(virtual_disk_sos.storage, &sup);
    printf("%d, %d, %d, %d\n", sup.number_of_files, sup.number_of_users, sup.nb_blocks_used, sup.first_free_byte);
    printf("\n");
    command_interpreter();
    write_super_block(virtual_disk_sos.storage, virtual_disk_sos.super_block);
    write_inodes_table(virtual_disk_sos.storage, virtual_disk_sos.inodes);
    fclose(virtual_disk_sos.storage);
    return 0;
}