/*
  V0.1 Initial Release   10/July/2006  RBR

*/

#ifndef COMPILER_H
#define COMPILER_H


/******************************************************************************
*******************              Commonly used types        *******************
******************************************************************************/
typedef unsigned char       BOOL;


// Data
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;

// Unsigned numbers
typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned long       UINT32;

// Signed numbers
typedef signed char         INT8;
typedef signed short        INT16;
typedef signed long         INT32;


//*****************************************************************************
//
// Define a boolean type, and values for true and false.
//xy  add
//*****************************************************************************

#ifndef TRUE
   #define TRUE 1
#endif

#ifndef FALSE
   #define FALSE 0
#endif

#ifndef NULL
   #define NULL 0
#endif

typedef  const char    ROMCHAR ;
#endif

