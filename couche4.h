/********************************************************************
 *    Couche 4 :                                                    *
 *    Gestion des fichiers et des droits                            *
 *    Projet S4                                                     *
 *                                                                  *
 *    K.Mohamed - Univ. Toulouse III Paul-Sabatier   2021-2022      *
 *                                                                  *
 ********************************************************************/

#ifndef couche4_h
#define couche4_h

#include "sos_defines.h"

/**
 * @brief Ecrit le fichier, dont le nom est dans la chaine filename, sur le systeme
 * 
 * @param filename 
 * @param fichier 
 * @return uint 
 */
uint write_file(char filename[], file_t fichier);

/**
 * @brief Recupere un fichier dans le systeme dont le nom correspond a la chaine filename
 * 
 * @param filename 
 * @param fichier 
 * @return uint 
 */
uint read_file(char filename[], file_t *fichier);
	
/**
 * @brief Efface un fichier du systeme dont le nom correspond a la chaine filename
 * 
 * @param filename 
 * @return uint 
 */
uint delete_file(char filename[]);

/**
 * @brief Ecrit un fichier de l'ordinateur sur le systeme virtuel
 * 
 * @param filename 
 */
void load_file_from_host(char filename[]);

/**
 * @brief Ecrit sur l'ordinateur un fichier du systeme virtuel
 * 
 * @param filename 
 */
void store_file_to_host(char filename[]);

#endif