/**************************************************************************/
// highlander.cpp - Modification of the scan command
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
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

char *const distance[6]=
{
	"right here.", "nearby to the %s.", "not far %s.",
	"off in the distance %s.", "off in the far distance %s.", 
	"far far away to the %s."
};

void scanhigh_list2( ROOM_INDEX_DATA *scan_room, char_data *ch, sh_int depth, sh_int door);
void scanhigh_list( ROOM_INDEX_DATA *scan_room, char_data *ch, sh_int depth, sh_int door);
/**************************************************************************/
void do_hbuzz(char_data *ch, char *argument, int dist, int blocked)
{
	char arg1[MIL];
	ROOM_INDEX_DATA *scan_room;
	EXIT_DATA *pExit;
	sh_int door, depth;
	
    if (dist>5)
        dist = 5;
	
	argument = one_argument(argument, arg1);
	
	if (arg1[0] == '\0')
	{
		scanhigh_list(ch->in_room, ch, 0, -1);
		
        for (door=0;door<MAX_DIR;door++)
		{
			if ((  pExit = ch->in_room->exit[door]) != NULL 
				&& ( !IS_SET( pExit->exit_info, EX_CLOSED )
				|| ( IS_SET(  pExit->exit_info, EX_CLOSED)
				&& !blocked	)))
				scanhigh_list(pExit->u1.to_room, ch, 1, door);
		}
		return;
	}

	door = dir_lookup( arg1 );
	
    if ( door == -1 )
	{
		ch->println("Which way do you want to sense?");
		return;
	}
	
	act("You sense intently $T.", ch, NULL, dir_name[door], TO_CHAR);
	
	scan_room = ch->in_room;
	
	for (depth = 1; depth <= dist; depth++)
	{
        if (scan_room
            && ((pExit = scan_room->exit[door]) != NULL)
            && (   !IS_SET(pExit->exit_info, EX_CLOSED)
			|| (IS_SET(pExit->exit_info, EX_CLOSED) && !blocked)))
        {
			scan_room = pExit->u1.to_room;
			scanhigh_list(pExit->u1.to_room, ch, depth, door);
        }
        else
            continue;
	}
	return;
}
/**************************************************************************/
void do_buzz(char_data *ch, char *argument)
{
	if (IS_OOC(ch))
	{	
		return;
	}
	if(IS_NPC(ch))
	{	
		return;
	}
	if(IS_AFFECTED2(ch, AFF2_NO_HIGHLANDER))
	{	
		return;
	}
	if(!IS_AFFECTED2(ch, AFF2_HIGHLANDER))
	{	
		return;
	}	
	do_hbuzz(ch, argument, 3, true);
}

/**************************************************************************/
void scanhigh_list(ROOM_INDEX_DATA *scan_room, char_data *ch, sh_int depth, sh_int door)
{
	char_data *rch;
	
	if (scan_room == NULL)
		return;

	if ( IS_SET( scan_room->room_flags, ROOM_NOSCAN ))
		return;

	if ( door != -1 && scan_room->exit[rev_dir[door]] != NULL
		&& IS_SET(scan_room->exit[rev_dir[door]]->exit_info,EX_CLOSED) ){
		return;
	}

	for (rch=scan_room->people; rch != NULL; rch=rch->next_in_room)
	{
		if (rch == ch)
			continue;
		if (!IS_NPC(rch) && INVIS_LEVEL(rch)> get_trust(ch))
			continue;
		if (IS_NPC(rch))
			continue;
		if (IS_SET(rch->affected_by2, AFF2_HIGHLANDER) )
		{
			ch->println("`#`R**BUZZ** `YYou sense a Highlander nearby.`^");
			rch->println("`#`R**BUZZ** `YYou sense a Highlander nearby.`^");
		}
	}
	return;
}
/***************************************************************************/
void do_highlander( char_data *ch, char *argument )
{
	char arg[MIL];
	one_argument( argument, arg );

    if(!IS_IMMORTAL(ch) && ch->remort>0 && !IS_VAMPIRE(ch) && !IS_WEREBEAST(ch))
    {

	if(!IS_ACTIVE(ch))
	{
		ch->printlnf("You are not ACTIVE!");
		return;
	}

	if(IS_AFFECTED2(ch, AFF2_HIGHLANDER))
	{
		ch->printlnf("You are already a Highlander.");
		return;
	}

	if(IS_AFFECTED2(ch, AFF2_NO_HIGHLANDER))
	{
		ch->printlnf("You have been eliminated from the game already.");
		return;
	}

	if (!str_prefix(arg,"confirm"))
	{

		SET_BIT(ch->affected_by2, AFF2_HIGHLANDER);
		ch->println( "You are now a `#`YHIGHLANDER!!`^" );
		return;
	}

	ch->printlnf("Type HIGHLANDER CONFIRM to enter the game.");
	return;

    }
	
    if ( arg[0] == '\0' )
    {
		ch->println( "Highlander whom ?" );
		return;
    }
	
    char_data *victim;

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
		ch->println( "They aren't here." );
		return;
    }
	
    if ( IS_NPC(victim) )
    {
		ch->println( "Not on NPC's." );
		return;
    }

    if ( !IS_IMMORTAL(victim) && victim->remort == 0 || !IS_ACTIVE(victim) )
    {
		ch->println( "They have not remorted yet or are not active." );
		return;
    }

    if (IS_VAMPIRE(victim) || IS_WEREBEAST(victim))
    {
		ch->println( "Not on them." );
		return;
    }
	
    if( IS_AFFECTED2(victim, AFF2_NO_HIGHLANDER))
    {
		REMOVE_BIT(victim->affected_by2, AFF2_NO_HIGHLANDER);
		SET_BIT(victim->affected_by2, AFF2_HIGHLANDER);
		victim->println( "You are now a `#`YHIGHLANDER!!`^" );
		ch->println( "HIGHLANDER status activated." );

    }
    else
    {
		SET_BIT(victim->affected_by2, AFF2_NO_HIGHLANDER);
		REMOVE_BIT(victim->affected_by2, AFF2_HIGHLANDER);
		victim->println( "You are no longer a `#`YHighlander!`^" );
		ch->println( "HIGHLANDER status removed." );

    }

    save_char_obj( victim );
    return;
}
/**************************************************************************/
void do_hbuzz2(char_data *ch, char *argument, int dist, int blocked)
{
	char arg1[MIL];
	EXIT_DATA *pExit;
	sh_int door;
	
    if (dist>5)
        dist = 5;
	
	argument = one_argument(argument, arg1);
	
   for (door=0;door<MAX_DIR;door++)
	{
		if ((  pExit = ch->in_room->exit[door]) != NULL && ( !IS_SET( pExit->exit_info, EX_CLOSED )
			    || ( IS_SET(  pExit->exit_info, EX_CLOSED) && !blocked	)))
 			scanhigh_list2(pExit->u1.to_room, ch, 1, door);
	}
	return;
	
}
/**************************************************************************/
void do_buzz2(char_data *ch, char *argument)
{
	if (IS_OOC(ch))
	{	
		return;
	}
	if(IS_NPC(ch))
	{	
		return;
	}
	if(IS_AFFECTED2(ch, AFF2_NO_HIGHLANDER))
	{	
		return;
	}
	if(!IS_AFFECTED2(ch, AFF2_HIGHLANDER))
	{	
		return;
	}
	do_hbuzz2(ch, argument, 3, true);
}
/**************************************************************************/
void scanhigh_list2(ROOM_INDEX_DATA *scan_room, char_data *ch, sh_int depth, sh_int door)
{
	char_data *rch;
	
	if (scan_room == NULL)
		return;

	if ( IS_SET( scan_room->room_flags, ROOM_NOSCAN ))
		return;

	if ( door != -1 && scan_room->exit[rev_dir[door]] != NULL
		&& IS_SET(scan_room->exit[rev_dir[door]]->exit_info,EX_CLOSED) ){
		return;
	}

	for (rch=scan_room->people; rch != NULL; rch=rch->next_in_room)
	{
		if (rch == ch)
			continue;
		if (!IS_NPC(rch) && INVIS_LEVEL(rch)> get_trust(ch))
			continue;
		if (IS_NPC(rch))
			continue;
		if (IS_SET(rch->affected_by2, AFF2_HIGHLANDER))
			ch->println("`#`R**BUZZ** `YYou sense a Highlander nearby.`^");
	}
	return;
}
/***************************************************************************/

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
