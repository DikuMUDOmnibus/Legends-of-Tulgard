/**************************************************************************/
// event.cpp - Special Event Feature
/***************************************************************************
 * Whispers of Times Lost (c)2001-2003 Brad Wilson                         *
 * >> To use this source code, you must fully comply with all the licenses *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 ***************************************************************************
 * >> Original Diku Mud copyright (c)1990, 1991 by Sebastian Hammer,       *
 *    Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, & Katja Nyboe.   *
 * >> Merc Diku Mud improvements copyright (C) 1992, 1993 by Michael       *
 *    Chastain, Michael Quan, and Mitchell Tse.                            *
 * >> ROM 2.4 is copyright 1993-1995 Russ Taylor and has been brought to   *
 *    you by the ROM consortium: Russ Taylor(rtaylor@pacinfo.com),         *
 *    Gabrielle Taylor(gtaylor@pacinfo.com) & Brian Moore(rom@rom.efn.org) *
 * >> Oblivion 1.2 is copyright 1996 Wes Wagner                            *
 **************************************************************************/

#include "include.h" // dawn standard includes
#include "tables.h"
#include "lookup.h"
#include "ictime.h"

void do_event (char_data *ch, char *argument)
{
	
}
/**************************************************************************/
void event (char_data *ch, int number, char *argument)
{
	if(number = 4)
		demon_event(ch,"");


	return;
}
/**************************************************************************/
void demon_event (char_data *ch, char *argument)
{
	if(current_time>event_time)
	{
		event = 0;
		wiznet("mEVENT> Demon Invasion Ended.",ch,NULL,WIZ_QUESTING,0,LEVEL_IMMORTAL);
		return;
	}

	if (!IS_NPC(ch) && !IS_OOC(ch) && !IS_IMMORTAL(ch) &&
         IS_SET(ch->act,PLR_CAN_ADVANCE) && (ch->in_room->sector_type != SECT_CITY &&
  	 ch->in_room->sector_type != SECT_WATER_SWIM && ch->in_room->sector_type != SECT_UNDERWATER &&
	 ch->in_room->sector_type != SECT_AIR && !IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) )
	{ 
	    char_data *demon;
	    int chance, level, leveladj, bonus;

	    chance = number_range(1,100);
	    if(chance<20)
	    {
		leveladj = number_range(1,10);
		bonus = number_range(1,4);
		if(bonus == 1){
			level = ch->level - leveladj;
		}
		else { level = ch->level + leveladj; }
		if(bonus == 2)
			level = ch->level - 1;	

    		demon = create_mobile( get_mob_index(60000),0);
		demon->level = level;
		bonus = number_range(25,200);
		demon->max_hit = ch->max_hit + bonus;
		demon->hit = demon->max_hit;
	 	reset_char(demon);
    		char_to_room( demon, ch->in_room );
		act( "`#A `Rflaming gate`^ appears and a $n steps out.`^", demon, NULL, NULL, TO_ROOM );
		update_pos(demon);
		char qbuf[MSL];
		sprintf (qbuf, "`mEVENT> Demon appears to %s in room %d`x\r\n", 
				ch->name, ch->in_room->vnum); 
		wiznet(qbuf,ch,NULL,WIZ_QUESTING,0,LEVEL_IMMORTAL);
		if(IS_GOOD(ch))
		{
			demon->set_pdelay(4);
			act( "$n snarls and attacks!!", demon, NULL, NULL, TO_ROOM );
			multi_hit( ch, demon, TYPE_UNDEFINED );
		}
		if(IS_EVIL(ch))
		{
			act( "$n looks around for a victim.", demon, NULL, NULL, TO_ROOM );
		}
	    }
 	}
	return;
}
/**************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/


