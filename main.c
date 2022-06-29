#include "couche2.h"
#include "couche1.h"
#include "couche4.h"
#include "couche5.h"
#include "installeur.h"
#include "sos_defines.h"


virtual_disk_t virtual_disk_sos;
session_t session;


/* main de test à compiler avec timestamp.c */
int main(int argc, char *argv[]){
   
    if(argc != 2){
        printf("%s: usage: %s directory\n", argv[0], argv[0]);
        return 1;
    }

    DIR *dir = opendir(argv[1]);
    if(dir)
        closedir(dir);
    else{
        printf("Directory \"%s\" does not exist\n", argv[1]);
        return 1;
    }

    char syspath[32];
    strcpy(syspath, argv[1]);
    strcat(syspath, "/d0");
    

    char ans[32];
    do{ 
        printf("Want to (re)install system?(Y/N): ");
        scanf("%s", ans);

    }while(strcmp(ans, "Y") && strcmp(ans, "N"));

    if(!strcmp(ans, "Y")){
        
        int cr = install_OS(argv[1]);

        if(cr)
            return 3;

    }else{
        FILE *sysfile = fopen(syspath, "rb+");
        if(sysfile){
            fseek(sysfile, 0, SEEK_END);
            if(ftell(sysfile) == 0){
                printf("System \"%s\" not operational \n", syspath);
                return 4;
            }
                
            fclose(sysfile);
        }else{
            printf("System \"%s\" does not exist\n", syspath);
            return 2;
        }
        
    }

    init_disk_sos(argv[1]);
    printf("Launching command interpreter:\n");
    command_interpreter();

    return 0;
}