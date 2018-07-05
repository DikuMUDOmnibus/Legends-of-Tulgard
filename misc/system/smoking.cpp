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
void update_smell (char_data *ch )
{
	if (!ch->pcdata->odor)
		ch->pcdata->odor = 0;
	if (ch->pcdata->odor <= 100)
	{
		affect_parentspellfunc_strip( ch, gsn_smell_rose);
		affect_parentspellfunc_strip( ch, gsn_smell_jasmine);
		affect_parentspellfunc_strip( ch, gsn_smell_soap);
		affect_parentspellfunc_strip( ch, gsn_smell_cinnamon);
		affect_parentspellfunc_strip( ch, gsn_smell_freshlinen);
		affect_parentspellfunc_strip( ch, gsn_smell_incense );
		affect_parentspellfunc_strip( ch, gsn_smell_freshbread);
	}
	if (ch->pcdata->odor <= 0)
	{
		ch->pcdata->odor = 500;
		affect_parentspellfunc_strip( ch, gsn_smell_rotten);
		affect_parentspellfunc_strip( ch, gsn_smell_rose);
		affect_parentspellfunc_strip( ch, gsn_smell_jasmine);
		affect_parentspellfunc_strip( ch, gsn_smell_sewer);
		affect_parentspellfunc_strip( ch, gsn_smell_soap);
		affect_parentspellfunc_strip( ch, gsn_smell_cinnamon);
		affect_parentspellfunc_strip( ch, gsn_smell_corpse);
		affect_parentspellfunc_strip( ch, gsn_smell_rotfood);
		affect_parentspellfunc_strip( ch, gsn_smell_freshbread);
		affect_parentspellfunc_strip( ch, gsn_smell_mownhay);
		affect_parentspellfunc_strip( ch, gsn_smell_freshlinen);
		affect_parentspellfunc_strip( ch, gsn_smell_incense );
		affect_parentspellfunc_strip( ch, gsn_smell_smoke );

		AFFECT_DATA	af;
		af.where	= WHERE_AFFECTS;
		af.level	= 100;
		af.duration 	= -1;
		af.location	= APPLY_NONE;
		af.modifier 	= 0;
		af.bitvector	= 0;

		if(ch->race == race_lookup("lich")){
			af.type		= gsn_smell_corpse;
			ch->println ("The pungent odor of decaying corpses begins to eminate from your body.");
			act( "$n begins to foul the air with the stench of `Sdeath`x.", ch, NULL, NULL, TO_ROOM );
		}
		else if(ch->race == race_lookup("cambion")){
			af.type		= gsn_smell_smoke;
			ch->println ("The odor of `#`Ssmoke`^ begins to eminate from your body.");
			act( "The strong aroma of `#`Ssmoke`^ begins to linger about $n.", ch, NULL, NULL, TO_ROOM );
		}
		else if(ch->race == race_lookup("wemic")){
			af.type		= gsn_smell_mownhay;
			ch->println ("You begin to spell like freshly cut hay.");
			act( "$n begins to smell like `yfresh-cut hay.`x", ch, NULL, NULL, TO_ROOM );
		}
		else if(ch->race == race_lookup("centaur")){
			af.type		= gsn_smell_mownhay;
			ch->println ("You begin to spell like freshly cut hay.");
			act( "$n begins to smell like `yfresh-cut hay.`x", ch, NULL, NULL, TO_ROOM );
		}
		else if(ch->race == race_lookup("troll")){
			af.type		= gsn_smell_sewer;
			ch->println ("You begin to emit the stench of decaying sewage from your body.");
			act( "$n begins to foul the air with the stench of `gdecaying sewage`x.", ch, NULL, NULL, TO_ROOM );
		}
		else if(ch->race == race_lookup("orc")){
			af.type		= gsn_smell_sewer;
			ch->println ("You begin to emit the stench of decaying sewage from your body.");
			act( "$n begins to foul the air with the stench of `gdecaying sewage`x.", ch, NULL, NULL, TO_ROOM );
		}
		else if(ch->race == race_lookup("half-orc")){
			af.type		= gsn_smell_sewer;
			ch->println ("You begin to emit the stench of decaying sewage from your body.");
			act( "$n begins to foul the air with the stench of `gdecaying sewage`x.", ch, NULL, NULL, TO_ROOM );
		}
		else if(ch->race == race_lookup("cyclops")){
			af.type		= gsn_smell_sewer;
			ch->println ("You begin to emit the stench of decaying sewage from your body.");
			act( "$n begins to foul the air with the stench of `gdecaying sewage`x.", ch, NULL, NULL, TO_ROOM );
		}
		else if(ch->race == race_lookup("goblin")){
			af.type		= gsn_smell_rotfood;
			ch->println ("You begin to emit the stench of rotten food from your body.");
			act( "$n begins to foul the air with the stench of `?r`?o`?t`?t`?e`?n `?f`?o`?o`?d`x.", ch, NULL, NULL, TO_ROOM );
		}
		else if(ch->race == race_lookup("hobgoblin")){
			af.type		= gsn_smell_rotfood;
			ch->println ("You begin to emit the stench of rotten food from your body.");
			act( "$n begins to foul the air with the stench of `?r`?o`?t`?t`?e`?n `?f`?o`?o`?d`x.", ch, NULL, NULL, TO_ROOM );
		}
		else if(ch->race == race_lookup("lizardman")){
			af.type		= gsn_smell_rotfood;
			ch->println ("You begin to emit the stench of rotten food from your body.");
			act( "$n begins to foul the air with the stench of `?r`?o`?t`?t`?e`?n `?f`?o`?o`?d`x.", ch, NULL, NULL, TO_ROOM );
		}
		else if(ch->race == race_lookup("demon")){
			af.type		= gsn_smell_smoke;
			ch->println ("The odor of `#`Ssmoke`^ begins to eminate from your body.");
			act( "The strong aroma of `#`Ssmoke`^ begins to linger about $n.", ch, NULL, NULL, TO_ROOM );
		}
		else if(ch->race == race_lookup("angel")){
			af.type		= gsn_smell_jasmine;
			ch->println ("The subtle, alluring aroma of `#`Mjasmine`^ lingers about you.");
			act( "The subtle, alluring aroma of `#`Mjasmine`^ begins to linger about $n.", ch, NULL, NULL, TO_ROOM );
		}
		else if(ch->race == race_lookup("dryad")){
			af.type		= gsn_smell_rose;
			ch->println ("The fresh scent of roses lingers about you.");
			act( "The fragrence of `#`Rroses`^ begins to linger about $n.", ch, NULL, NULL, TO_ROOM );
		}
		else if(ch->race == race_lookup("faerie")){
			af.type		= gsn_smell_rose;
			ch->println ("The fresh scent of `#`Rroses`^ lingers about you.");
			act( "The fragrence of `#`Rroses`^ begins to linger about $n.", ch, NULL, NULL, TO_ROOM );
		}
		else 
		{	af.type		= gsn_smell_rotten;
			ch->println ("You begin to emit a putrid stench from your body.");
			act( "$n begins to foul the air with thier putrid stench.", ch, NULL, NULL, TO_ROOM );

		}

		affect_to_char( ch, &af );
	}
}
/**************************************************************************/
void do_sniff( char_data *ch, char *argument )
{
	char_data *victim;

	ch->println("You sniff the air around you.");

	for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
	{
		if ( victim == ch )
			continue;
		
		if ( get_trust(ch) < INVIS_LEVEL(victim))
			continue;
		
		if ( can_see( ch, victim ) )
		{
    			if ( is_affected(victim, gsn_smell_rotten))
    			{
				ch->printlnf( "%s fouls the air with a `gputrid stench`x.", PERS(victim,ch) ); 
			    } else
			    if ( is_affected(victim, gsn_smell_rose))
			    {
				ch->printlnf( "The smell of fresh `#`Rroses`^ lingers about %s`x.", PERS(victim,ch) ); 
			    } else
			    if ( is_affected(victim, gsn_smell_jasmine))
			    {
				ch->printlnf( "The subtle, alluring aroma of `#`Mjasmine`^ lingers about %sx.", PERS(victim,ch) ); 
 			   } else
			    if ( is_affected(victim, gsn_smell_sewer))
			    {
				ch->printlnf( "%s fouls the air with the stench of `gdecaying sewage`x.", PERS(victim,ch) ); 
			    } else
			    if ( is_affected(victim, gsn_smell_soap))
			    {
				ch->printlnf( "%s smells like they just took a bath.", PERS(victim,ch) ); 
			    } else
			    if ( is_affected(victim, gsn_smell_cinnamon))
			    {
				ch->printlnf( "%s smells of `ycinnamon `wand `rspice`x.", PERS(victim,ch) ); 
			    } else
			    if ( is_affected(victim, gsn_smell_corpse))
			    {
				ch->printlnf( "%s fouls the air with the stench of `Sdeath`x.", PERS(victim,ch) ); 
			    } else
			    if ( is_affected(victim, gsn_smell_rotfood))
 			   {
				ch->printlnf( "%s fouls the air with the stench of `?r`?o`?t`?t`?e`?n `?f`?o`?o`?d`x.", PERS(victim,ch) ); 
			    } else
			    if ( is_affected(victim, gsn_smell_freshbread))
			    {
				ch->printlnf( "%s smells of `Wfreshly baked bread`x.", PERS(victim,ch) ); 
			    } else
			    if ( is_affected(victim, gsn_smell_mownhay))
			    {
				ch->printlnf( "%s smells like they have rolled in `ywet hay`x.", PERS(victim,ch) ); 
			    } else
			    if ( is_affected(victim, gsn_smell_freshlinen))
			    {
				ch->printlnf( "%s smells of fresh linen`x.", PERS(victim,ch) ); 
			    } else
			    if ( is_affected(victim, gsn_smell_incense ))
			    {
				ch->printlnf( "%s smells of `cfine incense`x.", PERS(victim,ch) ); 
			    } else
			    if ( is_affected(victim, gsn_smell_smoke ))
			    {
				ch->printlnf( "%s fouls the air with the smell of `Ssmoke and ash`x.", PERS(victim,ch) ); 
 			    }	
		}
	}
	act( "$n sniffs the air thoroughly.", ch, NULL, NULL, TO_ROOM );
	return;
}
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

