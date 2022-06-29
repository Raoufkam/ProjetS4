#ifndef __OS_DEFINES__
#define __OS_DEFINES__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "sha256.h"
#include "sha256_utils.h"


// pour les droits : minuscule = pas le droit, majuscule = droit ok
#define rw 0 // 00
#define rW 1 // 01
#define Rw 2 // 10
#define RW 3 // 11
#define ROOT_UID 0

#define FILENAME_MAX_SIZE 32 // nb max de caractères des noms de fichiers
#define MAX_FILE_SIZE (2*1024) // taille max d'un fichier (uchar)
#define INODE_TABLE_SIZE 10 // taille fixe = nb max de fichiers dans le SE
#define MAX_MSG 1024 // uchar
#define TIMESTAMP_SIZE 26 // avec \0 et pour que ça tombe juste avec les blocs
#define NB_USERS 5

#define BLOCK_SIZE 4 // octets
#define SUPER_BLOCK_SIZE 4 // nb blocs
#define INODES_START SUPER_BLOCK_SIZE*BLOCK_SIZE+1 // en octets
#define INODE_SIZE (FILENAME_MAX_SIZE/BLOCK_SIZE+6+(TIMESTAMP_SIZE*2)/BLOCK_SIZE)
//= 27 en blocs = 8 blocs de filename+6 uint+timestamps

typedef unsigned int uint; // même taille que int
typedef unsigned char uchar; // 8 bits non signés = octet

// structure pour la table des utilisateurs
typedef struct user_s{
  char login[FILENAME_MAX_SIZE];
  char passwd[SHA256_BLOCK_SIZE*2 + 1]; // 64 hex char = haché du passwd
} user_t;

// pour gérer l'uid tout le long de la session
typedef struct session_s{
  int userid;
} session_t;

/* Type of a block of data */
typedef struct block_s{
    uchar data[BLOCK_SIZE]; // une case par octet
} block_t;

/* Type of the pseudo-inode structure */
typedef struct inode_s{
  // type file vs dir
  char filename[FILENAME_MAX_SIZE]; // dont '\0'
  uint size; // du fichier en octets
  uint uid; //user id proprio
  uint uright; //owner's rights between 0 and 3 coding rw in binary
  uint oright; // other's right idem
  char ctimestamp[TIMESTAMP_SIZE]; // date creation : 26 octets
  char mtimestamp[TIMESTAMP_SIZE]; // date dernière modif. : 26 octets
  uint nblock; // nblock du fichier = (size+BLOCK_SIZE-1)/BLOCK_SIZE ?
  uint first_byte; // number of the first byte on the virtual disk
} inode_t;

/* Type of the inode table */
typedef inode_t inode_table_t[INODE_TABLE_SIZE];// la taille est fixe
typedef user_t users_table_t[NB_USERS]; // taille fixe. L'indice est l'uid.

typedef struct super_block_s{
  uint number_of_files; // dans super bloc
  uint number_of_users; // idem
  uint nb_blocks_used ; //
  uint first_free_byte; // premier octet libre
  } super_block_t;

/* Type of the virtual disk system */
typedef struct virtual_disk_s {
    super_block_t super_block;
    users_table_t users_table; // tableau
    inode_table_t inodes;// tableau
    FILE *storage; //fichier vdisk du système de fichiers
} virtual_disk_t;

// structure pour les fichiers
typedef struct file_s{
  uint size; // Size of file in bytes with pading ie compléter le dernier bloc avec des zéros
  uchar data [MAX_FILE_SIZE] ; // only text files
} file_t ;

// structure pour gérer les commandes de l'interprète
typedef struct cmd_s{
    char ** tabArgs; // tableau des arguments
    int nbArgs; // nombre d'arguments
} cmd_t;

#endif // __OS_DEFINES__