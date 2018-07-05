/*
 * Copyright (c) 2018 CCS/UPM - GMRV/URJC.
 *
 * Authors: Juan Pedro Brito Méndez <juanpedro.brito@upm.es>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef RANDUTILS_H_
#define RANDUTILS_H_

#include <stdlib.h>
#include <time.h>
#include <string.h>

//flags for the extended randstr function
#define RAND_STR_CAPITOL_LETTERS    0x00000001    //65 through 90
#define RAND_STR_LOWER_CASE_LETTERS    0x00000002    //97 through 122
#define RAND_STR_NUMBERS        0x00000004    //48 through 57
#define RAND_STR_SYMBOLS_1        0x00000008    //32 through 47
#define RAND_STR_SYMBOLS_2        0x00000010    //58 through 64
#define RAND_STR_SYMBOLS_3        0x00000020    //91 through 96
#define RAND_STR_SYMBOLS_4        0x00000040    //123 through 126
#define RAND_STR_NON_PRINTING      0x00000080    //1 through 31 and 127
#define RAND_STR_EXTENDED_ASCII      0x00000100    //128 through 255

/**
 *  Use this first function to seed the random number generator, call this before any of the other functions
 */
void initrand ( );

/**	
 *  Generates a psuedo-random integer between 0 and 32767
 */
int randint ( );

/**	
 * Generates a psuedo-random integer between 0 and max  
 */
int randint ( int max );

/**	
 * Generates a psuedo-random integer between min and max  
 */
int randint ( int min, int max );

/**
 * Generates a psuedo-random float between 0.0 and 0.999...  
 */
float randfloat ( );

/**
 * Generates a psuedo-random float between 0.0 and max  
 */
float randfloat ( float max );

/**	
 * Generates a psuedo-random float between min and max  
 */
float randfloat ( float min, float max );

/**
 * Generates a psuedo-random double between 0.0 and 0.999...  
 */
double randdouble ( );

/**	
 * Generates a psuedo-random double between 0.0 and max  
 */
double randdouble ( double max );

/**
 * Generates a psuedo-random double between min and max 
 */
double randdouble ( double min, double max );

/**	
 *  Generates a random string with explicit lenght
 */
char *randstr ( char *str, unsigned long length );

/**	
 *  Generates a random string with explicit lenght
 */
char *randstr ( char *str, unsigned long length, unsigned long flags );

#endif /*DEFS_H_*/
