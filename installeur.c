/*******************************************************************
*    Installeur :                                                  *
*                                                                  *
*    Projet S4                                                     *
*                                                                  *
*    P.Alibert - Univ. Toulouse III Paul-Sabatier   2021-2022      *
*                                                                  *
*******************************************************************/

#include "couche1.h"
#include "couche2.h"
#include "couche3.h"
#include "couche5.h"
#include "installeur.h"
#include "sos_defines.h"

extern virtual_disk_t virtual_disk_sos;
extern session_t session;

/**
 * @brief Cree le systeme virtuel et l'initialise avec l'utilisateur 0 (root)
 * 
 * @param rep 
 * @return int 
 */
int install_OS(char rep[]){

    int cr;
    pid_t pid;

    switch (pid = fork())
    {
    case -1:
        fprintf(stderr, "echec fork\n");
        exit(1);
        break;
    case 0:
        execl("cmd_format", "./cmd_format", rep, "500", NULL);
        fprintf(stderr, "echec disk\n");
        exit(2);
        break;
        
    default:
        wait(&cr);
        break;
    }

    char file_emplacement[50] = "";
    strcat(file_emplacement, rep);
    strcat(file_emplacement,"/d0");
    virtual_disk_sos.storage = fopen(file_emplacement,"rb+");

    virtual_disk_sos.super_block.number_of_files = 0;
    virtual_disk_sos.super_block.number_of_users = 0;
    virtual_disk_sos.super_block.nb_blocks_used = 0;
    virtual_disk_sos.super_block.first_free_byte = 0;
    update_first_free_byte();
    char login[MAX_WORD];
    do{ 
        printf("login(not 0): ");
        scanf("%s", login);
        while(getchar() != '\n'){}

    }while(!strcmp(login, "0"));

    char password[MAX_WORD];
    char hashRes[SHA256_BLOCK_SIZE*2 + 1];
    printf("password: ");
    scanf("%s", password);
    sha256ofString((BYTE *)password,hashRes);
    create_root(login, hashRes);
    virtual_disk_sos.super_block.number_of_users++;
    char *passwordfile = "passwd";
    init_inode(passwordfile, sizeof(users_table_t), virtual_disk_sos.super_block.first_free_byte);
    virtual_disk_sos.inodes[0].uright = 0;
    
    shut_system(0, 0);

    return 0;
}