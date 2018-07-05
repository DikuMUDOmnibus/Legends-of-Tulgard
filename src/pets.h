/**************************************************************************/
// pets.h - Pet Code by Ixliam
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/

#ifndef KENNEL_H
#define KENNEL_H

/**************************************************************************/
// data structures first
typedef struct	kennel_type KENNEL_DATA;

struct kennel_type
{
  struct kennel_type * next;
	int petnum;
};
/**************************************************************************/
//prototypes
void load_kennel_db( void );
void save_kennel_db( void );

// semilocalized globals
extern KENNEL_DATA *kennel_list;

#endif // KENNEL_H
