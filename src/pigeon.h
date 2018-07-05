/**************************************************************************/
// pigeon.h - 
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/

#ifndef PIGEON_H
#define PIGEON_H

/**************************************************************************/
// data structures first
typedef struct	pigeon_type PIGEON_DATA;

struct pigeon_type
{
  struct pigeon_type * next;
	char * pcname;
	char * name;
	char * short_descr;
	char * scribed_by;
	char * scribed_time;
	char * original_txt;
	char * text;
	int lv0;
	int lv1;
	int lv2;
	int lv3;
	int lv4;
};
/**************************************************************************/
//prototypes
void load_pigeon_db( void );
void save_pigeon_db( void );
void queue_pigeon(char * pcname, char * name, char * short_descr, char * scribed_by, char * scribed_time, char * original_txt, char * text, int lv0, int lv1, int lv2, int lv3, int lv4);
void check_pending_pigeon(char_data *ch);


// semilocalized globals
extern PIGEON_DATA *pigeon_list;

#endif // PIGEON_H
