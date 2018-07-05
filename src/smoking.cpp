/**************************************************************************/
// gamble.cpp - Unfinished gambling system by Kerenos
/***************************************************************************
 * The Dawn of Time v1.69q (c)1997-2002 Michael Garratt                    *
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
#include "include.h"
#include "recycle.h"
#include "tables.h"

/**************************************************************************/
void do_smoke (char_data *ch, char *argument)
{
    OBJ_DATA *pObj;
    int amount, range;

    if ( argument[0] == '\0' )
    {
		ch->println( "Smoke what?\n\r");
		return;
    }
	if ( ( pObj = get_obj_carry( ch, argument ) ) == NULL )
	{
		ch->println( "You need a pipe if you want to smoke tabac.");
		return;
	}
	if ( pObj->item_type != ITEM_TABAC_PIPE)
	{
		ch->println( "You need a pipe if you want to smoke tabac.");
		return;
	}
	/* When a pipe is empty, it poofs, buy another pipe */
	if ( pObj->value[1] <= 0 )
	{
	    ch->println( "Your pipe is out of tabac." );
	    act( "$n frowns regretfully at $p.",  ch, pObj, NULL, TO_ROOM );
        extract_obj(pObj);
	    return;
	}
	/* do some randomness */
	range = number_range(1,4);
	switch( range )
	{
		default:
	    	act( "$n smokes tabac from $p.",  ch, pObj, NULL, TO_ROOM );
		    act( "You smoke tabac from $p.", ch, pObj, NULL, TO_CHAR );
	    break;
		case 1:
	    	act( "$n inhales deeply from $p.",  ch, pObj, NULL, TO_ROOM );
		    act( "You inhale deeply from $p.", ch, pObj, NULL, TO_CHAR );
	    break;
		case 2:
	    	act( "$n puffs on $p for a moment.",  ch, pObj, NULL, TO_ROOM );
		    act( "You puff on $p for a moment.", ch, pObj, NULL, TO_CHAR );
	    break;
		case 3:
	    	act( "$n draws deeply on $p.",  ch, pObj, NULL, TO_ROOM );
		    act( "You draw deeply $p.", ch, pObj, NULL, TO_CHAR );
	    break;
	}

	/* if the the char isn't already smoking, add some smoke
	 * if the char is not set to smoke, they cant exhale. can change
	 * this to act or whatever. in AWW, tabac flags are used for other
	 * things, as well.
	 */
	if ( !IS_SET(ch->dyn, DYN_SMOKING))
		SET_BIT(ch->dyn, DYN_SMOKING);

	/* Mix up the numbers a bit */
	amount = number_range(1,2);
    if (pObj->value[0] > 0)
    {
        pObj->value[1] -= amount;
        amount = number_range(1,6);
        switch ( amount )
        {
			case 1:
			    act( "$p crackles and fumes.",  ch, pObj, NULL, TO_ROOM );
			    act( "$p crackles and fumes.",  ch, pObj, NULL, TO_CHAR );
		    break;
		    case 2:
			    act( "$p leaks spiralling fingers of smoke.",  ch, pObj, NULL, TO_ROOM );
			    act( "$p leaks spiralling fingers of smoke.",  ch, pObj, NULL, TO_CHAR );
		    break;
		    case 3:
			    act( "$p spits tiny red sparks up from it's bowl.",  ch, pObj, NULL, TO_ROOM );
			    act( "$p spits tiny red sparks up from it's bowl.",  ch, pObj, NULL, TO_CHAR );
		    break;
		    case 4:
				act( "A thin curl of smoke leaks up from $p's bowl.", ch, pObj, NULL, TO_ROOM );
				act( "A thin curl of smoke leaks up from $p's bowl.", ch, pObj, NULL, TO_CHAR );
			break;
		    case 5:
				act( "Long threads of smoke rise up from $p", ch, pObj, NULL, TO_ROOM );
				act( "Long threads of smoke rise up from $p", ch, pObj, NULL, TO_CHAR );
			break;
			default:
			act( "$p glows a deep, dark red color.", ch, pObj, NULL, TO_ROOM );
			act( "$p glows a deep, dark red color.", ch, pObj, NULL, TO_CHAR );
			break;
		}
	}
    return;
}

void do_exhale(char_data *ch, char *argument)
{
	char arg[MIL];

	/* that smoking flag has to be set */
	if ( !IS_SET(ch->dyn, DYN_SMOKING))
	{
		ch->println("`wBut you aren't smoking from a pipe.`^");
		return;
	}

	one_argument( argument, arg );

	/* do the basics */
	if ( arg == NULL)
	{
		ch->println("You can use many different exhales to enhance your smoking pleasure.");
		ch->println("Try these others:");
		ch->println("RINGS LINES BALLS WEB PHOENIX HORNS or try EXHALE HELP");
		act("$n exhales some pale gray smoke.", ch, NULL, NULL, TO_ROOM );
		act("You exhale some pale gray smoke.", ch, NULL, NULL, TO_CHAR );
		REMOVE_BIT(ch->dyn, DYN_SMOKING);
		return;
	}
	/* send some help */
	if ( !str_prefix( arg, "help" ))
	{
		ch->println("Before you can exhale, you must first smoke from your pipe.");
		ch->println("After you have enjoyed that flavorful inhalation of vapors...");
		ch->println("You can use many different exhales to enhance your smoking pleasure.");
		ch->println("Try these different types of exhales to impress and astound:");
		ch->println("RINGS LINES BALLS WEB PHOENIX HORNS or try EXHALE HELP");
		return;
	}
	if ( !str_prefix( arg, "rings" ))
	{
		act("$n exhales large round smoke rings.", ch, NULL, NULL, TO_ROOM );
		act("You exhale large round smoke rings.", ch, NULL, NULL, TO_CHAR );
		REMOVE_BIT(ch->dyn, DYN_SMOKING);
		return;
	}
	if ( !str_prefix( arg, "lines" ))
	{
		act("$n exhales a long line of smoke.", ch, NULL, NULL, TO_ROOM );
		act("You exhale a long line of smoke.", ch, NULL, NULL, TO_CHAR );
		REMOVE_BIT(ch->dyn, DYN_SMOKING);
		return;
	}
	if ( !str_prefix( arg, "balls" ))
	{
		act("$n puffs a big round ball of smoke.", ch, NULL, NULL, TO_ROOM );
		act("You puff a big round ball of smoke.", ch, NULL, NULL, TO_CHAR );
		REMOVE_BIT(ch->dyn, DYN_SMOKING);
		return;
	}
	if ( !str_prefix( arg, "web" ))
	{
		act("$n exhales thin, wispy streams of smoke that merge to form a delicate web.", ch, NULL, NULL, TO_ROOM );
		act("You exhale thin, wispy streams of smoke that merge to form a delicate web.", ch, NULL, NULL, TO_CHAR );
		REMOVE_BIT(ch->dyn, DYN_SMOKING);
		return;
	}
	if ( !str_prefix( arg, "phoenix" ))
	{
		act("$n exhales lines of smoke that form into a phoenix, only to disperse into ashes.", ch, NULL, NULL, TO_ROOM );
		act("You exhale lines of smoke that form into a phoenix, only to disperse into ashes.", ch, NULL, NULL, TO_CHAR );
		REMOVE_BIT(ch->dyn, DYN_SMOKING);
		return;
	}
	if ( !str_prefix( arg, "horns" ))
	{
		act("$n exhales two lines of smoke that settle over $m like a pair of horns.", ch, NULL, NULL, TO_ROOM );
		act("You exhale two lines of smoke that settle over you like a pair of horns.", ch, NULL, NULL, TO_CHAR );
		REMOVE_BIT(ch->dyn, DYN_SMOKING);
		return;
	}
}

/**************************************************************************/


