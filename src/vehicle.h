/**************************************************************************/
// vehicle.h - Vehicle movement & Combat
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/

#ifndef VEHICLE_H
#define VEHICLE_H

/**************************************************************************/
// data structures first
typedef struct	vehicle_type VEHICLE_DATA;

struct vehicle_type
{
  struct vehicle_type * next;
	int reset;
	int vnum;
};
/**************************************************************************/
//prototypes
void load_vehicle_db( void );
void save_vehicle_db( void );

// semilocalized globals
extern VEHICLE_DATA *vehicle_list;

#endif // VEHICLE_H
