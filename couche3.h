#ifndef couche3_h
#define couche3_h

/**
 * @brief Recupere les infos de la table utilisateur et les mets dans la structure table_users
 * 
 */
void read_users_table();

/**
 * @brief Recupere les infos dans la structure table_users et les ecrits sur le systeme
 * 
 */
void write_users_table();

/**
 * @brief Initialise l'utilisateur 0 (root) dans la structure table_users
 * 
 * @param login 
 * @param password 
 */
void create_root(char login[], char password[]);

#endif