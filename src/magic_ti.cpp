/**************************************************************************/
// magic_ti.cpp - spells/skills written by Tibault
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/
#include "include.h"
#include "magic.h"
#include "o_lookup.h"

int get_direction( char *arg );
/**************************************************************************/
SPRESULT spell_higher_learning( int sn, int level, char_data *ch, void *, int )
{
	AFFECT_DATA		af;
    char_data		*victim;
	connection_data *d;
	int				alignStray;

	if ( is_affected( ch, sn )) 
	{
	    ch->println("You already feel inspired.");
	    return HALF_MANA;
	}

	for (d = connection_list; d != NULL; d = d->next)
	{
		victim = d->character;
		if (victim && !IS_OOC(victim) && !IS_NPC(victim) && !is_affected( victim, sn) )
		{
			alignStray = abs(ch->alliance - victim->alliance);
			
			af.where		= WHERE_AFFECTS;
			af.type			= sn;
			af.location		= APPLY_RE;
			af.level		= level;
			if ( ch == victim ) {
				af.duration	= level/3;
				af.modifier	= level/20;
			} else {
				af.duration	= level/6;
				af.modifier	= level/8 - (level*alignStray/30);
			}
			af.bitvector	= 0;
			affect_to_char( victim, &af );

			if ( ch == victim ) {
				af.where		= WHERE_AFFECTS;
				af.type			= sn;
				af.location		= APPLY_SD;
				af.level		= level;
				af.duration		= 5;
				af.modifier		= -level/5;
				af.bitvector	= 0;
				affect_to_char( victim, &af );
				ch->println("You pray to your deity for inspiration throughout the realm!");
				ch->println("You feel a little light-headed.");
			} else {
				victim->println("You feel inspired by divine intervention.");
			}
		}
	}

	ch->pcdata->tired += 25;
    ch->pcdata->condition[COND_THIRST] = 0;
    ch->pcdata->condition[COND_HUNGER] = 0;

	return FULL_MANA;
}

/**************************************************************************/
SPRESULT spell_detect_treeform( int sn, int level, char_data *ch, void *vo,int )
{
	char_data *victim = (char_data *) vo;
	AFFECT_DATA af;

	if ( IS_AFFECTED2(victim, AFF2_DETECT_TREEFORM) )
	{
		if (victim == ch)
			ch->printf( "You area already one with nature.\r\n" );
		else
			act("$N is already in touch with nature.",ch,NULL,victim,TO_CHAR);
		return HALF_MANA;
	}

	af.where     = WHERE_AFFECTS2;
	af.type      = sn;
	af.level     = level;
	af.duration  = level/2;
	af.modifier  = 0;
	af.location  = APPLY_NONE;
	af.bitvector = AFF2_DETECT_TREEFORM;
	affect_to_char( victim, &af );

	victim->printf( "You feel more in touch with nature.\r\n" );
	act("$n is surrounded with a `#`ggreen`^ aura.", ch, NULL, victim, TO_NOTVICT);

	if ( ch != victim )
		ch->printf( "Ok.\r\n" );
	return FULL_MANA;
}

/*************************************************************************/
SPRESULT spell_gust( int sn, int level, char_data *ch, void *vo, int )
{
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    char_data *victim;
	char arg1[MIL], arg2[MIL], buf[MIL];
	EXIT_DATA *pexit;
    target_name = one_argument( target_name, arg1 );
    one_argument( target_name, arg2 );

    if ( arg1[0] == '\0' || target_name[0] == '\0' )
    {
        ch->printf( "Throw winds at who?\r\n" );
        return NO_MANA;
    }


    if (( victim = get_char_room( ch, arg1 ))==NULL
	&& ch->fighting == NULL)
    {
        ch->printf( "They aren't here.\r\n" );
        return NO_MANA;
    }
	else
	{
		victim = ch->fighting;
		one_argument(arg1, arg2);
	}

	was_in = victim->in_room;
    int door = get_direction(arg2);
	
	if(door == -1)
	{
        ch->printf( "Please indicate a direction.\r\n" );
        return NO_MANA;
	}

	if ( ( pexit = was_in->exit[door] ) == 0
		||   pexit->u1.to_room == NULL
		|| ( IS_NPC(victim)
		&&   IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB) ) )
	{
		act("You cannot drive $N into that direction.", ch, NULL, victim, TO_CHAR);
		return HALF_MANA;
	}
	
	if(!IS_SET(pexit->exit_info, EX_CLOSED))
	{
		move_char( victim, door, false, NULL );
		if (victim->mounted_on!=NULL)
		{
			char_from_room(victim->mounted_on);
			char_to_room(victim->mounted_on, victim->in_room);
		}

		act( "$n is hurled from the room!", victim, NULL, NULL, TO_ROOM );
		if ( ( now_in = victim->in_room ) == was_in )
			act( "$n is hurled into the room again!", victim, NULL, NULL, TO_ROOM );

		if ( !IS_NPC(victim) )
		{
			victim->println("You feel winds lift you and hurl you though the air!");
		}
		stop_fighting( victim, false );
		return FULL_MANA;
	}
	else
	{
		int dam;

		sprintf(buf, "%s is slammed into %s by the winds!",victim->name,
			(pexit->description[0] != '\0'? pexit->description : "a door") );
		act(buf, victim, NULL, NULL, TO_ROOM );
		sprintf(buf, "The winds slam you against %s!",
			(pexit->description[0] != '\0'? pexit->description : "a door") );
		act(buf, victim, NULL, NULL, TO_CHAR );
			dam		= dice(level, 6)+10;
		
		if ( saves_spell( level, victim, DAMTYPE(sn)))
			dam /= 2;
		damage_spell( ch, victim, dam, sn, DAMTYPE(sn),true);
		return FULL_MANA;
	}
}

/***************************************************************************/
SPRESULT spell_grant_life(int sn,int level, char_data *ch, void *vo,int )
{
	OBJ_DATA *obj = (OBJ_DATA *) vo;
	int fail;

	// do standard checks - if spell can be cast 
	if (!SET_BIT(obj->extra_flags, OBJEXTRA_NONMETAL))
	{
		ch->printf( "You cannot bring back life to something never living.\r\n" );
		return NO_MANA;
    }
	if (IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
	{
		ch->printf( "It cannot regain life.\r\n" );
		return NO_MANA;
	}

    if (obj->wear_loc != -1)
    {
        ch->printf( "The item must be carried to be enchanted.\r\n" );
        return NO_MANA;
    }

    fail = 25 + (obj->level - level);	// base 25% chance of failure 
	if(number_percent() < fail)
	{
		if(number_range(1, 4) == 1)//A quarter of the time BAD things happen.
		{
			act("$p dissolves into nothingness in your hands!", ch, obj, NULL, TO_CHAR);
			act("$p dissolves into nothingness!", ch, obj, NULL, TO_ROOM);
			return FULL_MANA;
		}
		else //Otherwise, failing just removes the nonmetal part of it:P
		{
			act("$p petrifies in you hands!", ch, obj, NULL, TO_CHAR);
			act("$p petrifies!", ch, obj, NULL, TO_ROOM);
			REMOVE_BIT(obj->extra_flags, OBJEXTRA_NONMETAL);
			return FULL_MANA;
		}
	}
	else
	{
		act("$p comes to life at your coaxing!", ch, obj, NULL, TO_CHAR);
		act("$p writhes in $n's hands!", ch, obj, NULL, TO_ROOM);
		SET_BIT(obj->extra2_flags, OBJEXTRA2_REGROWING);
		SET_BIT(obj->extra2_flags, OBJEXTRA2_NODECAY);
		return FULL_MANA;
	}
}

/**************************************************************************/
// Reath
SPRESULT spell_sanctify( int sn, int level, char_data *ch, void *vo, int  )
{
	OBJ_DATA *obj;
	AFFECT_DATA af;
	
	obj=(OBJ_DATA *) vo;
	
	if(obj->item_type != ITEM_WEAPON)
	{
		ch->println("You can only cast this upon weapons.");
		return NO_MANA;
	}
	else
	{
			if(IS_WEAPON_STAT(obj,WEAPON_HOLY))
			{
				ch->println("That weapon is already sanctified.");
				return HALF_MANA;
			}
			if(IS_SET(obj->extra_flags, OBJEXTRA_ANTI_GOOD)
			|| IS_SET(obj->extra_flags, OBJEXTRA_ANTI_EVIL))
			{
				af.where    = WHERE_WEAPON;
				af.type     = sn;
				af.level = UMAX(obj->level,level)/2;
				af.duration = (level / 2);
				af.location = APPLY_NONE;
				af.modifier = 0;
				af.bitvector= WEAPON_HOLY;
				affect_to_obj(obj, &af);
				if(IS_SET(obj->extra_flags, OBJEXTRA_ANTI_GOOD))
					act("$p fades and wavers as it gathers darkness.",ch,obj,NULL,TO_ALL);
				else
					act("$p shimmers and sparkles as it gathers light.",ch,obj,NULL,TO_ALL);
				return FULL_MANA;
			}
			else
			{
				ch->println("This weapon is not fit to be given such a blessing.");
				return HALF_MANA;
			}
	}
	return FULL_MANA;
}
/**************************************************************************/
SPRESULT spell_consecrate( int sn, int level, char_data *ch, void *vo, int  )
{
	OBJ_DATA *obj;
	
	obj=(OBJ_DATA *) vo;
	
	if(obj->item_type != ITEM_FOOD)
	{
		ch->println("You can only cast this upon food.");
		return NO_MANA;
	}
	else
	{
			if(IS_SET(obj->value[3], FOOD_CONSECRATED))
			{
				ch->println("That food has already been given your blessing.");
				return HALF_MANA;
			}
			if(!IS_SET(obj->value[3], FOOD_POISONED))
			{
				SET_BIT(obj->value[3], FOOD_CONSECRATED);
				act("$p grows more vibrant and succulent.",ch,obj,NULL,TO_ALL);
				return FULL_MANA;
			}
			else
			{
				REMOVE_BIT(obj->value[3], FOOD_POISONED);
				ch->println("The poison within dissolves under your magic.");
				return FULL_MANA;
			}
	}
	return FULL_MANA;
}
/**************************************************************************/
SPRESULT spell_second_wind( int sn, int level, char_data *ch, void *vo,int )
{
	char_data *victim = (char_data *) vo;
	AFFECT_DATA af;

	if ( is_affected(victim, gsn_second_wind ))
	{
		if (victim == ch)
			ch->printf( "You've already called for a second wind.\r\n" );
		else
			act("$N has already been granted a second wind.",ch,NULL,victim,TO_CHAR);
		return HALF_MANA;
	}

	af.where     = WHERE_AFFECTS;
	af.type      = sn;
	af.level     = level;
	af.duration  = number_range(15, 40);
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = 0;
	affect_to_char( victim, &af );
	act( "A soft wind blows past $n.", victim, NULL, NULL, TO_ROOM );
	victim->printf( "You feel a soft wind at your back.\r\n" );
	return FULL_MANA;
}

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/


