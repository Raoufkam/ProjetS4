/*******************************************************************
*    Couche 5 :                                                    *
*    Interprete de commande                                        *
*    Projet S4                                                     *
*                                                                  *
*    P.Alibert - Univ. Toulouse III Paul-Sabatier   2021-2022      *
*                                                                  *
*******************************************************************/

#ifndef couche5_h
#define couche5_h

#include "sos_defines.h"

#define MAX_WORDS_COMMAND 3
#define MAX_WORD 32

/**
 * @brief Traite les commandes que l'utilisateur courant tape
 * 
 */
void command_interpreter();

#endif