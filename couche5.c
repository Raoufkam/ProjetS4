/*******************************************************************
*    Couche 5 :                                                    *
*    Interprete de commande                                        *
*    Projet S4                                                     *
*                                                                  *
*    P.Alibert - Univ. Toulouse III Paul-Sabatier   2021-2022      *
*                                                                  *
*******************************************************************/

#include "couche1.h"
#include "couche2.h"
#include "couche4.h"
#include "couche5.h"



extern virtual_disk_t virtual_disk_sos;
extern session_t session;

/**
 * @brief Verifie que identifiant et mot de passe en parametre correspondent a un utilisateur existant et soit correct
 * 
 * @param login 
 * @param password 
 * @return true 
 * @return false 
 */
bool check_user(char login[], char password[]){
    int i = 0;
    while(i < NB_USERS){
        if(!strcmp(virtual_disk_sos.users_table[i].login, login) && !strcmp(virtual_disk_sos.users_table[i].passwd, password)){
            return true;
        }
        i++;
    }
    return false;
}

/**
 * @brief Gere l'entree dans l'interprete en verifiant que l'utilisateur a acces au systeme
 * 
 */
void login_step(){
    int tentative = 0;
    bool user_OK = false;
    while(tentative < 3 && !user_OK){
        printf("login: ");
        char login[MAX_WORD];
        scanf("%s", login);
        printf("password: ");
        char password[MAX_WORD];
        char hashRes[SHA256_BLOCK_SIZE*2 + 1];
        scanf("%s", password);
        sha256ofString((BYTE *)password,hashRes);
        user_OK = check_user(login, hashRes);
    }
    if(tentative >= 3){
        printf("Too much attempt\n");
        shut_system(1, 2);
    }
    while(getchar() != '\n'){}
}

/**
 * @brief Remplie la structure command avec la chaine line pour faciliter la recuperation des parametres par la suite
 * 
 * @param command 
 * @param line 
 */
void fill_command(cmd_t *command, char line[]){
    char * separators = " \n";
    int i = 0;
    char *tok = strtok(line, separators);
    while(tok != NULL && i < MAX_WORDS_COMMAND){
        strcpy(command->tabArgs[i], tok);
        i++;
        tok = strtok(NULL, separators);
    }
    command->nbArgs = i;
}

/**
 * @brief Donne des infos sur les commandes utilisables
 * 
 */
void com_help(){
    printf("Defined commands:\n");
    printf("ls [-l] : Liste le contenu du catalogue\n");
    printf("cat <nom de fichier>                                : Affiche le contenu du fichier\n");
    printf("rm <nom de fichier>                                 : Efface le fichier\n");
    printf("cr <nom de fichier>                                 : Cree le fichier\n");
    printf("edit <nom de fichier>                               : Modifie le contenu du fichier\n");
    printf("load <nom de fichier>                               : Copie le fichier de l'ordinateur sur le systeme virtuel\n");
    printf("store <nom de fichier>                              : Copie le fichier du systeme virtuel sur l'ordinateur\n");
    printf("chown <nom de fichier> <login autre utlisateur>     : Change le proprietaire du fichier\n");
    printf("chmod <nom de fichier> <login autre utlisateur>     : Change les droits du fichier pour les autres utilisateurs\n");
    printf("lisusers                                            : Liste les utlisateurs\n");
    printf("quit                                                : Quitte l'interprete et sauvegarde le systeme\n");
}

/**
 * @brief Affiche la totalite des fichiers sur le systeme avec plus ou moins d'infos selon la demande
 * 
 * @param arg 
 */
void com_ls(int arg){
    for(int i = 0; i<virtual_disk_sos.super_block.number_of_files; i++){
        inode_t inode = virtual_disk_sos.inodes[i];
        printf("Fichier: %s\n", inode.filename);
        printf("    - Taille: %d\n", inode.size);
        if(arg){
            printf("    - User ID: %d\n", inode.uid);
            printf("    - User Rights: %d\n", inode.uright);
            printf("    - Others Rights: %d\n", inode.oright);
            printf("    - Date Création: %s\n", inode.ctimestamp);
            printf("    - Date Modification: %s\n", inode.mtimestamp);
            printf("    - Nombre Blocks: %d\n", inode.nblock);
            printf("    - Premier Octet: %d\n", inode.first_byte);
        }
        printf("\n");
    }
}

/**
 * @brief Liste les utilisateurs sur le systeme
 * 
 */
void com_listusers(){
    for(int i = 0; i < NB_USERS; i++){
        if(strcmp(virtual_disk_sos.users_table[i].login, "0"))
            printf("%d: %s\n", i, virtual_disk_sos.users_table[i].login);
    }
    printf("\n");
}

/**
 * @brief Ajoute un utilisateur dans la table s'il reste de la place
 * 
 */
void com_adduser(){
    if(session.userid == ROOT_UID){
        if(virtual_disk_sos.super_block.number_of_users < NB_USERS){
            char login[MAX_WORD];
            int exist = 0;
            do{ 
                exist = 0;
                printf("login(not 0): ");
                scanf("%s", login);
                while(getchar() != '\n'){}
                int i = 0;
                while(i < NB_USERS){
                    if(!strcmp(virtual_disk_sos.users_table[i].login, login)){
                        printf("Username not available\n");
                        exist = 1;
                    }
                    i++;
                }

            }while(!strcmp(login, "0") || exist);

            char password[MAX_WORD];
            char hashRes[SHA256_BLOCK_SIZE*2 + 1];
            printf("password: ");
            scanf("%s", password);
            sha256ofString((BYTE *)password,hashRes);
            while(getchar() != '\n'){}

            int i = 1, found = 0;
            while(i < NB_USERS && !found){
                if(!strcmp(virtual_disk_sos.users_table[i].login, "0")){
                    found = 1;
                    strcpy(virtual_disk_sos.users_table[i].login, login);
                    strcpy(virtual_disk_sos.users_table[i].passwd, hashRes);
                    virtual_disk_sos.super_block.number_of_users++;
                }
                i++;
            }
        }else
            printf("adduser: Maximum number of users reached\n");
    }else
        printf("adduser: Permission denied\n");
    
}

/**
 * @brief Recupere les droits en ecriture et lecture de l'utilisateur en cours sur le fichier dont l'indice est en parametre
 * 
 * @param indice 
 * @return int 
 */
int get_rights(int indice){
    if(virtual_disk_sos.inodes[indice].uid == session.userid)
        return virtual_disk_sos.inodes[indice].uright;
    else
        return virtual_disk_sos.inodes[indice].oright;
}

/**
 * @brief Ecrit sur la sortie le contenu du fichier de nom filename
 * 
 * @param filename 
 */
void com_cat(char filename[]){
    int found = 0;
    for(int i = 0; i < virtual_disk_sos.super_block.number_of_files; i++){
        if(!strcmp(virtual_disk_sos.inodes[i].filename, filename)){
            found = 1;
            if(get_rights(i) >= Rw){
                file_t file;
                read_file(filename, &file);
                fwrite(file.data, file.size, 1, stdout);
                printf("\n");
            }else
                printf("cat: %s: Permission denied\n", filename);
        }
    }
    if(!found)
        printf("cat: %s: No such file\n", filename);
}

/**
 * @brief Efface le fichier de nom filename
 * 
 * @param filename 
 */
void com_rm(char filename[]){
    int found = 0;
    for(int i = 0; i < virtual_disk_sos.super_block.number_of_files; i++){
        if(!strcmp(virtual_disk_sos.inodes[i].filename, filename)){
            found = 1;
            if(get_rights(i)%2 != 0){
                delete_file(filename);
            }else
                printf("rm: %s: Permission denied\n", filename);
        }
    }
    if(!found)
        printf("rm: %s: No such file\n", filename);
}

/**
 * @brief Cree un fichier de nom filename si il n'y a pas deja un autre fichier de nom filename sur le systeme
 * 
 * @param filename 
 */
void com_cr(char filename[]){
    int already_exist = 0, i = 0;
    while(i < virtual_disk_sos.super_block.number_of_files && !already_exist){
        if(!strcmp(virtual_disk_sos.inodes[i].filename, filename))
            already_exist = 1;
        i++;
    }
    if(!already_exist){
        if(virtual_disk_sos.super_block.number_of_files < INODE_TABLE_SIZE){
            init_inode(filename, 0, virtual_disk_sos.super_block.first_free_byte);
        }else{
            printf("Inode space required\n");
        }
    }else
        printf("cr: %s: File with same name alreay exist\n", filename);
    
}

/**
 * @brief Permet a l'utilisateur de modifier le fichier de nom filename s'il a les droits
 * 
 * @param filename 
 */
void com_edit(char filename[]){
    int found = 0;
    for(int i = 0; i < virtual_disk_sos.super_block.number_of_files; i++){
        if(!strcmp(virtual_disk_sos.inodes[i].filename, filename)){
            found = 1;
            if(get_rights(i)%2 != 0){
                file_t file;
                printf("(Pour fermer la fenetre d'edition taper sur ¤ puis sur entree)\n");
                scanf("%[^¤]", file.data);
                while(getchar() != '\n'){}
                file.size = strlen((char *)file.data)+1;
                write_file(filename, file);

            }else
                printf("edit: %s: Permission denied\n", filename);
        }
    }
    if(!found)
        printf("edit: %s: No such file\n", filename);
}

/**
 * @brief Ecrit un fichier de l'ordinateur sur le systeme virtuel
 * 
 * @param filename 
 */
void com_load(char filename[]){
    load_file_from_host(filename);
}

/**
 * @brief Ecrit sur l'ordinateur un fichier du systeme virtuel
 * 
 * @param filename 
 */
void com_store(char filename[]){
    store_file_to_host(filename);
}

/**
 * @brief Efface un utlisateur et tout ses fichiers si l'utilisateur courant a les droits et n'essaie pas d'effacer l'utilisateur root
 * 
 * @param login 
 */
void com_rmuser(char login[]){
    if(session.userid == ROOT_UID){
        int i = 0;
        while( i < NB_USERS && strcmp(virtual_disk_sos.users_table[i].login, login))
            i++;

        if(i == 0)
            printf("rmuser: Permission denied\n");
        else if(i < NB_USERS){
            for(int j = 0; j < virtual_disk_sos.super_block.number_of_files; j++){
                if(virtual_disk_sos.inodes[j].uid == i)
                    delete_file(virtual_disk_sos.inodes[j].filename);
            }
            strcpy(virtual_disk_sos.users_table[i].login, "0");
            virtual_disk_sos.super_block.number_of_users--;
        }else
            printf("rmuser: No user found\n");
            
            
    }else
        printf("rmuser: Permission denied\n");
    
}

/**
 * @brief Donne la propriete du fichier de nom filename a l'utilisateur de nom login
 * 
 * @param filename 
 * @param login 
 */
void com_chown(char filename[], char login[]){
    int found = 0, j = 0;
    while( j < NB_USERS && strcmp(virtual_disk_sos.users_table[j].login, login))
        j++;
    
    for(int i = 0; i < virtual_disk_sos.super_block.number_of_files; i++){
        if(!strcmp(virtual_disk_sos.inodes[i].filename, filename)){
            found = 1;
            if(j >= NB_USERS)
                printf("chown: No user found\n");
            else if(get_rights(i)%2 != 0){
                virtual_disk_sos.inodes[i].uid = j;
                strcpy(virtual_disk_sos.inodes[i].mtimestamp, timestamp());
            }else
                printf("chown: %s: Permission denied\n", filename);
        }
    }
    if(!found)
        printf("chown: %s: No such file\n", filename);
}

/**
 * @brief Modifie les droits du fichier de nom filename pour les autres utlisateurs (others)
 * 
 * @param filename 
 * @param droit 
 */
void com_chmod(char filename[], int droit){
    int found = 0;
    
    for(int i = 0; i < virtual_disk_sos.super_block.number_of_files; i++){
        if(!strcmp(virtual_disk_sos.inodes[i].filename, filename)){
            found = 1;
            if(droit > RW || droit < rw)
                printf("chown: Invalid rights\n");
            else if(get_rights(i)%2 != 0){
                virtual_disk_sos.inodes[i].oright = droit;
                strcpy(virtual_disk_sos.inodes[i].mtimestamp, timestamp());
            }else
                printf("chmod: %s: Permission denied\n", filename);
        }
    }
    if(!found)
        printf("chmod: %s: No such file\n", filename);
}

/**
 * @brief Traite les commandes que l'utilisateur courant tape
 * 
 */
void command_interpreter(){
    login_step();
    cmd_t command;
    int quit_itp = 0;
    char line[MAX_WORDS_COMMAND*MAX_WORD];
    command.tabArgs = malloc(MAX_WORDS_COMMAND*sizeof(char *));

    for(int i = 0; i < MAX_WORDS_COMMAND; i++){
        command.tabArgs[i] = malloc(MAX_WORD*sizeof(char));
    }

    do{
        printf("$ ");
        fgets(line, MAX_WORDS_COMMAND*MAX_WORD, stdin);
        fill_command(&command, line);

        if(command.nbArgs == 1){

            if(!strcmp(command.tabArgs[0], "help"))
                com_help();

            else if(!strcmp(command.tabArgs[0], "quit"))
                quit_itp = 1;

            else if(!strcmp(command.tabArgs[0], "ls"))
                com_ls(0);

            else if(!strcmp(command.tabArgs[0], "listusers"))
                com_listusers();

            else if(!strcmp(command.tabArgs[0], "adduser"))
                com_adduser();

            else
                printf("%s: command not found or incorrect, use command \"help\" for more infos\n", command.tabArgs[0]);

        }else if(command.nbArgs == 2){

            if(!strcmp(command.tabArgs[0], "ls") && !strcmp(command.tabArgs[1], "-l"))
                com_ls(1);

            else if(!strcmp(command.tabArgs[0], "cat"))
                com_cat(command.tabArgs[1]);

            else if(!strcmp(command.tabArgs[0], "rm"))
                com_rm(command.tabArgs[1]);

            else if(!strcmp(command.tabArgs[0], "cr"))
                com_cr(command.tabArgs[1]);

            else if(!strcmp(command.tabArgs[0], "edit"))
                com_edit(command.tabArgs[1]);

            else if(!strcmp(command.tabArgs[0], "load"))
                com_load(command.tabArgs[1]);

            else if(!strcmp(command.tabArgs[0], "store"))
                com_store(command.tabArgs[1]);

            else if(!strcmp(command.tabArgs[0], "rmuser"))
                com_rmuser(command.tabArgs[1]);

            else
                printf("%s: command not found or incorrect, use command \"help\" for more infos\n", command.tabArgs[0]);
            
        }else if(command.nbArgs == 3){

            if(!strcmp(command.tabArgs[0], "chown"))
                com_chown(command.tabArgs[1], command.tabArgs[2]);

            else if(!strcmp(command.tabArgs[0], "chmod"))
                com_chmod(command.tabArgs[1], atoi(command.tabArgs[2]));

            else
                printf("%s: command not found or incorrect, use command \"help\" for more infos\n", command.tabArgs[0]);
        }

    }while(!quit_itp);
    shut_system(1, 0);
}