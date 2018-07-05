/**************************************************************************/
// magic_ne.cpp - spells/skills written for Necromancers
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

/**************************************************************************/
const struct flag_type morbid_renames[] =
{
	{"cutting breezes",			DAM_NONE,		true},
	{"blasts of force",			DAM_BASH,		true},
	{"sharp forms",				DAM_PIERCE,		true},
	{"slashing metal",			DAM_SLASH,		true},
	{"flames",					DAM_FIRE,		true},
	{"arctic cold",				DAM_COLD,		true},
	{"jolts of lightning",		DAM_LIGHTNING,	true},
	{"hissing liquids",			DAM_ACID,		true},
	{"vapors of poison",		DAM_POISON,		true},
	{"unholy blasts",			DAM_NEGATIVE,	true},
	{"holy chanting",			DAM_HOLY,		true},
	{"pure energy",				DAM_ENERGY,		true},
	{"psychic echoes",			DAM_MENTAL,		true},
	{"a rotting smell",			DAM_DISEASE,	true},
	{"the sound of water",		DAM_DROWNING,	true},
	{"blinding light",			DAM_LIGHT,		true},
	{"otherworldly forces",		DAM_OTHER,		true},
	{"echoes of pain",			DAM_HARM,		true},
	{"whispers of suggestion",	DAM_CHARM,		true},
	{"ungodly noises",			DAM_SOUND,		true},
	{"visions",					DAM_ILLUSION,	true},
    { NULL,						0,				   0}
};
/**************************************************************************/

SPRESULT spell_wyldfire( int sn, int level, char_data *ch, void *vo,int  )
{
	char_data *victim = (char_data *) vo;
	int dam;
	int necro_dam;

	dam		= dice(level, 6)+10;
    necro_dam     = dice(UMAX((ambient_death(ch) * 5), 1), 6) + 10;

	switch(check_immune(victim,DAM_NEGATIVE))
	{
	case(IS_IMMUNE):
		necro_dam = 0;
		break;
	case(IS_RESISTANT):
		necro_dam -= necro_dam/2;
		break;
	case(IS_VULNERABLE):
		necro_dam += necro_dam;
		break;
	}

	ch->println("You harness liquid death into a weapon!");
	act( "$n pulls darkness to $mself and energy erupts!",ch, NULL, NULL, TO_ROOM );

	if ( saves_spell( level, victim, DAMTYPE(sn)))
		dam /= 2;

	damage_spell( ch, victim, dam, sn, DAMTYPE(sn),true);
	return FULL_MANA;
}
/**************************************************************************/
int ambient_death(char_data *ch)
{
	return ch->in_room->death_energy;
}
/*
	char_data *victim;
	char_data *v_next;
	int room_energy;
	int people_energy;
	int number_present;

	room_energy = ch->in_room->room_death;
	number_present = count_people_room( ch, 0 );
	people_energy = 0;

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        people_energy += victim->death_energy;
		if(victim->next_in_room)
			v_next = victim->next_in_room;
		else
			break;
    }
	
	return (room_energy + (people_energy / 100 / number_present));
}
*************************************************************************
int get_room_death(char_data *ch)
{
	int room_energy;

	room_energy = ch->in_room->room_death;
	return room_energy;
}
*************************************************************************/
/*************************************************************************
int get_people_death(char_data *ch)
{
	char_data *victim;
	char_data *v_next;
	int people_energy;
	int number_present;

	number_present = count_people_room( ch, 0 );
	people_energy = 0;

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        people_energy += victim->death_energy;
    }
	
	return (people_energy / 100 / number_present);
}
**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/


/*
SPRESULT spell_lesser_raising( int sn, int level, char_data *ch, void *vo, int )
{
    OBJ_DATA *obj, *cor;
    int i;

    obj = get_obj_here( ch, target_name );

    if(IS_GOOD(ch) && !IS_NPC(ch))
    {
        ch->printlnf( "Darkness burrows a little deeper inside of you.");
		ch->alliance_change--;
    }

    if ( obj == NULL )
    {
        ch->printlnf( "Raise what ?");
        return NO_MANA;
    }

    // Nothing but NPC corpses. 

    if( obj->item_type != ITEM_CORPSE_NPC )
    {
        if( obj->item_type == ITEM_CORPSE_PC )
            ch->printlnf( "You can't animate a players corpse!");
        else
            ch->printlnf( "You can't animate that!");
        return NO_MANA;
    }

    if( obj->level > (ch->level + 2) )
    {
        ch->printlnf( "You couldn't call forth such a great spirit.");
        return NO_MANA;
    }

    if( ch->pet != NULL )
    {
        ch->printlnf( "You already have a pet following you.");
        return NO_MANA;
    }

    int chance;
    chance = number_range(1,4);

    char_data *mob = NULL;
	
		case 1: mob = create_mobile( get_mob_index( 71+chance ), 0 ); // Zombie

    mob->level   = ch->level - 5;
    mob->max_hit = mob->level * 8 + number_range(mob->level * mob->level/4, mob->level * mob->level);
    mob->max_hit -= mob->level/10;
    mob->hit 		= mob->max_hit;
    mob->max_mana       = 100 + dice(mob->level,10);
    mob->mana           = mob->max_mana;
    for (i = 0; i < 3; i++)
        mob->armor[i]   = interpolate(mob->level,100,-100);
    mob->armor[3]   = interpolate(mob->level,100,0);

    for (i = 0; i < MAX_STATS; i++)
        mob->perm_stats[i] = 45 + mob->level/4;

    // You rang? 
    char_to_room( mob, ch->in_room );
    switch(chance)
	{
		case 1:
    	act( "$p springs to life as a hideous zombie!", ch, obj, NULL, TO_ROOM );
    	act( "$p springs to life as a hideous zombie!", ch, obj, NULL, TO_CHAR );
		break;
    

		case 2:
    	act( "$p springs to life as a skeleton warrior!", ch, obj, NULL, TO_ROOM );
    	act( "$p springs to life as a skeleton warrior!", ch, obj, NULL, TO_CHAR );
		break;

		case 3:
    {
    	act( "$p springs to life as a hideous shade!", ch, obj, NULL, TO_ROOM );
    	act( "$p springs to life as a hideous shade!", ch, obj, NULL, TO_CHAR );
    }

    if (chance == 4)
    {
    	act( "$p springs to life as a hideous wraith!", ch, obj, NULL, TO_ROOM );
    	act( "$p springs to life as a hideous wraith!", ch, obj, NULL, TO_CHAR );
    }

    for( cor = obj->contains; cor; cor = cor->next_content )
    {
	obj_from_obj( cor );
	obj_to_char( cor, mob );
    }

    extract_obj(obj);
    do_wear( mob, "all" );

    SET_BIT(mob->affected_by, AFF_CHARM);
    SET_BIT(mob->act, ACT_PET);
    mob->comm = COMM_NOTELL|COMM_NOSHOUT|COMM_NOCHANNELS;
    add_follower( mob, ch );
    mob->leader = ch;
    ch->pet = mob;
    do_say( mob, "Yessssss, massssssster...." );
    return FULL_MANA;
}
*/

/*******************************************************************************/
SPRESULT spell_morbid_rememberance(int, int, char_data *ch,void *vo,int target)
{
	OBJ_DATA *corpse;
	MOB_INDEX_DATA *mob;
	char buf[MIL];
	int last_damage[2];


	if (target == TARGET_OBJ)
	{
		corpse = (OBJ_DATA *) vo;
		mob = get_mob_index(corpse->value[0]);
		
		if(corpse->value[3] >= 1000)
		{
			last_damage[0] = UMAX(corpse->value[3] - 1000, 0);
			last_damage[1] = corpse->value[4];
		}else{
			last_damage[0] = corpse->value[3];
			last_damage[1] = UMAX(corpse->value[4] - 1000, 0);
		}
		
		if (corpse->item_type != ITEM_CORPSE_NPC)
		{
			ch->println( "You can only commune through corpses." );
			return HALF_MANA;
		}
		act( "A scene of death rises from blood from $p.", ch, corpse, NULL, TO_ALL);
		if(corpse->killer && str_cmp(corpse->killer, "self")){
			sprintf(buf,"Through the mist shadowy lines form %s.",corpse->killer );
		}else{
			sprintf( buf, "The crimson mists reveal only shadows.");
		}
		act( buf,ch, NULL, NULL, TO_ALL);
		sprintf( buf, "The mist reforms into %s%s%s as the blood settles.",
		morbid_renames[last_damage[0]].name,
		((last_damage[0] > 0 && last_damage[1] > 0) ? " and " : ""),
		(last_damage[1] > 0 ? morbid_renames[last_damage[1]].name : ""));
		act(buf, ch, NULL, NULL, TO_ALL);
		return FULL_MANA;
	}

	ch->println( "You can only commune through corpses." );
	return HALF_MANA;
}
/*******************************************************************************/
SPRESULT spell_necrotic_barrier( int sn, int level, char_data *ch, void *vo,int )
{
	char_data *victim = (char_data *) vo;
	AFFECT_DATA af;

	if ( is_affected( victim, sn ) )
	{
		if (victim == ch)
			ch->printf( "You've already drawn upon the spirits for this.\r\n" );
		else
			act("$N is already channeling the spirits into a shield.",ch,NULL,victim,TO_CHAR);
		return HALF_MANA;
	}

	af.where     = WHERE_AFFECTS;
	af.type      = sn;
	af.level     = level;
	af.duration  = 8 + level;
	af.location  = APPLY_AC;
	af.modifier  = URANGE(-1, ambient_death(ch)*-3, -60);
	af.bitvector = 0;
	affect_to_char( victim, &af );
	act( "A soft sound like whispering trails $n.", victim, NULL, NULL, TO_ROOM );
	victim->printf( "You hear whispering somewhere in the distance.\r\n" );
	return FULL_MANA;
}
/**************************************************************************/
SPRESULT spell_cadaver_strength( int sn, int level, char_data *ch, void *vo,int )
{
	char_data *victim = (char_data *) vo;
	AFFECT_DATA af;

	if ( is_affected( victim, sn ) )
	{
		if (victim == ch)
			ch->printf( "You've already drawn upon the spirits for this.\r\n" );
		else
			act("$N is already channeling the spirits into a shield.",ch,NULL,victim,TO_CHAR);
		return HALF_MANA;
	}

	af.where     = WHERE_AFFECTS;
	af.type      = sn;
	af.level     = level;
	af.duration  = 8 + level;
	af.location  = APPLY_ST;
	af.modifier  = URANGE(1, ambient_death(ch)*3, 60);
	af.bitvector = 0;
	affect_to_char( victim, &af );
	act( "$n's body goes rigid.", victim, NULL, NULL, TO_ROOM );
	victim->printf( "You feel your muscles clench and unclench powerfully.\r\n" );
	return FULL_MANA;
}
/**************************************************************************/
SPRESULT spell_blood_shield( int sn, int level, char_data *ch, void *vo,int  )
{
	char_data *victim = (char_data *) vo;
	AFFECT_DATA af;
	bool victim_found;
	int used_hp=0;

	if ( victim == NULL
	||	 victim == ch){
		victim = ch;
		victim_found = false;
	}
	else
		victim_found = true;


	if ( is_affected( ch, gsn_blood_shield ))
		return HALF_MANA;
	if(victim_found)
	{
		if(IS_SET(victim->in_room->room2_flags,ROOM2_ARENA) ||
		IS_SET(victim->in_room->room2_flags,ROOM2_WAR) )
		{
			ch->println("Not while in this combat zone, you cheater!");
			return FULL_MANA;
		}

		if (victim->position >= POS_STUNNED
		&&  victim->hit >= 0)
		{
			ch->println("They are not weak enough to use.");
			return FULL_MANA;
		}
		else{
			act("Blood rushes from $N's body as it dissolves away!", ch, NULL, victim, TO_ALL);
			victim->position = POS_DEAD;
			SET_BIT(victim->form, FORM_INSTANT_DECAY);
			kill_char(victim, ch);
		}
	}
	else{
		if(ch->hit <= ch->max_hit/4)
			ch->println("It would kill you to draw the strength from yourself.");
		else
			used_hp = ch->hit/2;
		ch->hit-= used_hp;
	}

	af.where     = WHERE_AFFECTS;
	af.type      = sn;
	if (victim == ch)
	{
		af.duration  = level*(used_hp * 100 / (ch->max_hit/2))/500;
		af.level     = level*(used_hp * 100 / (ch->max_hit/2))/100;
	}
	else
	{
		af.duration  = (level+victim->level)/5;
		af.level     = (level+victim->level)/2;
	}
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = 0;
	affect_to_char( ch, &af );
	af.where	  = WHERE_RESIST;
	af.bitvector = RES_HOLY;
	affect_to_char( ch, &af );
	af.where	  = WHERE_VULN;
	af.bitvector = VULN_NEGATIVE;
	ch->println("You are surrounded by swirling blood.");
	act("$n is surrounded by dancing droplets of blood!",ch,NULL,NULL,TO_ROOM);
	return FULL_MANA;
}
/**************************************************************************/
 
