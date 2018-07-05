/**************************************************************************/
// vampire.cpp - vampire powers & shapechanging code
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
#include "altform.h"
#include "gameset.h"
#include "tables.h"
#include "lookup.h"
#include "ictime.h"

/* command procedures needed */
DECLARE_DO_FUN(kill_char  );
DECLARE_DO_FUN(do_look    );
void dismount( char_data *);
void laston_update_char(char_data *ch);

/**************************************************************************/
void wereswitch( char_data *ch )
{
    	if ( ch->altform.timer > 0)
    	{ 
	   	ch->println("You are unable to summon the beast from within.");
	   	return;
    	}

    	if (ch->position == POS_SLEEPING)
    	{
		ch->println("You can't do that while sleeping.");
		return;
    	}

    	if ( ch->fighting ) 
    	{
		ch->println("You seem to be too busy do anything.");
		return;
    	}

    	if(!ch->altform.short_descr || !ch->altform.description)
    	{
		ch->altform.type 	= ALTTYPE_LYCANTHROPE;
		ch->altform.subtype 	= WERE_WOLF;
		setup_altform(ch, ch->altform.type, ch->altform.subtype);
    	}

	act("$n collapses on the ground in agony and transforms into a $t!", ch,were_types[ch->altform.subtype].name,NULL,TO_ROOM);
	ch->printlnf("Pain shoots through your body as you transform into a %s.", were_types[ch->altform.subtype].name);

    	if (form_alternate(ch) == true)
    	{
		SET_BIT(ch->affected_by2, AFF2_NO_VAMP);
    		WAIT_STATE( ch, 10 );
		return;
    	}
    	ch->println("`#`RThere is an error somehow and you cannot transform. Please see an IMMORTAL.`^");
    	return;
}
/**************************************************************************/
void werereturn( char_data *ch)
{
   	if( (time_info.day == 1 && time_info.hour> 20) || 
	    (time_info.day == 2 && time_info.hour < 4) ) /* 8pm - 5am */
    	{
		ch->println( "The power of the beast has overwhelmed you.");
		ch->println( "You cannot change back until daylight.");
		return;
    	}

    	if ( ch->altform.timer > 0)
    	{ 
	   	ch->println("The power of the beast has overwhelmed you.");
	   	return;
    	}

    	if (ch->position == POS_SLEEPING)
    	{
		ch->println("You can't do that while sleeping.");
		return;
    	}

    	if ( ch->fighting ) 
    	{
		ch->println("You seem to be too busy do anything.");
		return;
    	}

    	if(!ch->altform.short_descr || !ch->altform.description)
    	{
		ch->altform.type 	= ALTTYPE_LYCANTHROPE;
		ch->altform.subtype 	= WERE_WOLF;
		setup_altform(ch, ch->altform.type, ch->altform.subtype);
    	}

	ch->println( "Pain fills your body as you transform back." );
	act("A $t collapses on the ground in agony and transforms into $n!", ch, were_types[ch->altform.subtype].name, NULL, TO_ROOM);

    	if (form_original(ch) == true)
    	{
		SET_BIT(ch->affected_by2, AFF2_NO_VAMP);
    		WAIT_STATE( ch, 10 );
		return;
	}

    	ch->println("`#`RThere is an error somehow and you cannot transform. Please see an IMMORTAL.`^");
    	return;
}
/**************************************************************************/
void do_werechange( char_data *ch, char *)
{
    	if(IS_NPC(ch))
    	{
		do_huh(ch,"");
		return;
    	}

    	if(!IS_WEREBEAST(ch))
    	{
		do_huh(ch,"");
		return;
    	}
	if(IS_WEREFORM(ch))
	{
		werereturn(ch);
		return;
	}else {
		wereswitch (ch);
		return;
    	}
}
/**************************************************************************/
void do_werebeast( char_data *ch, char *argument )
{
    char_data *victim;
	char arg1[MIL];
	char arg2[MIL];
	char arg3[MIL];

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	argument = one_argument(argument, arg3);
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	ch->println( "They have apparently departed the realm.");
	return;
    }
    if (arg1[0] == '\0')
    {
		ch->println( "Werebeast whom ?" );
		return;
    }
	
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
		ch->println( "They aren't here." );
		return;
    }
	
    if ( IS_NPC(victim) )
    {
		ch->println( "Not on NPC's." );
		return;
    }

    if(!IS_WEREBEAST(victim))
    {

	if( !IS_SET(race_table[victim->race]->flags, RACEFLAG_CAN_WEREBEAST ) )
	{
		ch->printlnf( "You cannot make %s a werebeast.", victim->name );
		return;
	}

	victim->altform.type = ALTTYPE_LYCANTHROPE;
	for(int i = 0; i <= WERE_CROCODILE; i++)
	{
		if(!str_prefix(arg2, were_types[i].name))
		{
			victim->altform.subtype = i;
			break;
		}
		else
			victim->altform.subtype = -1;
	}

	if(victim->altform.subtype == -1)
	{
		ch->println("What breed?");
		victim->altform.subtype = 0;
		return;
	}

	if(setup_werebeast(victim) == true)
	{
		if(!is_number(arg3) && !IS_NULLSTR(arg3))
		{
			ch->println("That isn't a number.");
			return;
		}else
			victim->werecircle = atoi(arg3);


		if(victim->werecircle < 1 || victim->werecircle > 13)
		{
			ch->println("Invalid circle. Defaulted to 13");
			victim->werecircle = 13;
		}
		SET_BIT(victim->affected_by2, AFF2_LYCANTHROPE);
		act( "$N is now a $t.", ch, were_types[victim->altform.subtype].name, victim, TO_CHAR);
		act( "You are now a $t.", ch, were_types[victim->altform.subtype].name, victim, TO_VICT);
		victim->gnosis = 5;
		return;
	}
	ch->printlnf( "There was an error in making %s a werebeast.", victim->name );
	return;
    } 

    if(IS_WEREBEAST(victim))
    {
	if(IS_WEREFORM(victim))
	{
    		if (form_original(ch) == true)
    		{
			victim->println( "Pain fills your body as you transform back." );
			act("A large werebeast collapses on the ground in agony and transforms into $n!", victim, NULL, NULL, TO_ROOM);
			SET_BIT(victim->affected_by2, AFF2_NO_VAMP);
		}
	}
	REMOVE_BIT(victim->affected_by2, AFF2_LYCANTHROPE);
	ch->printlnf( "%s is no longer a Werebeast.", victim->name );
	victim->printlnf( "You are no longer a Werebeast." );
	victim->gnosis = 0;
	victim->werecircle = 0;
	victim->altform.type = 0;
	victim->altform.subtype = 0;
	return;
    } 
    ch->println("There was an error.");
    return;
}
/**************************************************************************/
void dragonswitch( char_data *ch )
{
    if (ch->position == POS_SLEEPING)
    {
	ch->println("You can't do that while sleeping.");
	return;
    }

    if ( ch->fighting ) 
    {
	ch->println("You seem to be too busy do anything.");
	return;
    }

  	if ( !IS_IMMORTAL(ch) && ch->altform.timer > 0)
    	{ 
	   	ch->println("You are too weak to shapeshift now.");
	   	return;
    	}

    if (ch->position == POS_SLEEPING)
    {
	ch->println("You can't do that while sleeping.");
	return;
    }


    	if(ch->altform.short_descr==NULL)
    	{
    		ch->println("`#`RYour alternate form descriptions are not set. Please fix it.`^");
		replace_string(ch->altform.short_descr, ch->short_descr);
    	}

    	if(ch->altform.description==NULL)
    	{
    		ch->println("`#`RYour alternate form descriptions are not set. Please fix it.`^");
		replace_string(ch->altform.description, ch->description);
    	}

    	act("$n suddenly transforms into a humanoid form!", ch,NULL,NULL,TO_ROOM);
	ch->println("You transform into your humanoid form.");

    if (form_alternate(ch) == true)
    {
    	WAIT_STATE( ch, 10 );
	return;
    }
    ch->println("`#`RYou are not setup to transform. Please see an IMMORTAL.`^");
    return;
}
/**************************************************************************/
void dragonreturn( char_data *ch)
{
    	if ( !IS_IMMORTAL(ch) && ch->altform.timer > 0)
    	{ 
	   	ch->println("You are too weak to shapeshift now.");
	   	return;
    	}

    	if (ch->position == POS_SLEEPING)
    	{
		ch->println("You can't do that while sleeping.");
		return;
    	}

    	if ( ch->fighting ) 
    	{
		ch->println("You seem to be too busy do anything.");
		return;
    	}

    	if(ch->altform.short_descr==NULL)
    	{
    		ch->println("`#`RYour alternate form descriptions are not set. Please fix it.`^");
		replace_string(ch->altform.short_descr, ch->short_descr);
    	}

    	if(ch->altform.description==NULL)
    	{
    		ch->println("`#`RYour alternate form descriptions are not set. Please fix it.`^");
		replace_string(ch->altform.description, ch->description);
    	}

    	if (form_original(ch) == true)
    	{
		ch->println( "You transform back into your dragon form." );
		act("$n transforms suddenly into a dragon!", ch,NULL,NULL,TO_ROOM);
    		WAIT_STATE( ch, 10 );
		return;
	}

    	ch->println("`#`RYou are not setup to transform. Please see an IMMORTAL.`^");
    	return;
}
/**************************************************************************/
void do_dragonchange( char_data *ch, char *)
{
    	if(IS_NPC(ch))
    	{
		do_huh(ch,"");
		return;
    	}

	if(IS_DRAGON(ch) && !IS_DRAGONFORM(ch)) 
	{
		dragonswitch (ch);
		return; 
	}
	else if(IS_DRAGONFORM(ch))
	{
		dragonreturn (ch);
 		return; 
   	}

   	do_huh(ch,"");
   	return;
}
/**************************************************************************/
void do_embrace( char_data *ch, char *argument )
{
	char_data *victim;
	char buf[MSL];
    	bool can_sire = true;

	argument = one_argument( argument, buf );
	
	if ( IS_NPC(ch) )  
		return;

        if (!IS_NPC(ch) && !IS_VAMPIRE(ch))
	{
		do_huh(ch, "");
		return;
	}

    if ( ( victim = get_char_room( ch, buf ) ) == NULL )
    {
		ch->println( "They aren't here.");
		return;
    }

	if ( IS_NPC(victim) )
	{
		ch->println( "Not on NPC's.");
		return;
	}

	if ( IS_IMMORTAL (victim) || IS_AFFECTED2(victim, AFF2_HIGHLANDER) )
	{
		ch->println( "Not on Immortal's.");
		return;
	}

	if ( !IS_ACTIVE (victim) )
	{
		ch->println( "They are not active.");
		return;
	}

	if ( ch == victim )
	{
		ch->println( "You cannot bite yourself.");
		return;
	}

	if ( buf[0] == '\0' )
	{
		ch->println( "Who do you want to embrace ?");
		return;
	}


    if (!IS_VAMPAFF(ch,VAM_FANGS))
    {
	   ch->printlnf("First you better get your fangs out!");
	   return;
    }
    
    if (IS_VAMPAFF(ch,VAM_DISGUISED) || IS_VAMPAFF(ch,VAM_BAT) 
        || IS_VAMPAFF(ch,VAM_MIST) || IS_VAMPAFF(ch,VAM_WOLF) 
        || IS_VAMPAFF(ch,VAM_HUMAN))
    {
	   ch->printlnf("You must reveal your true nature to bite someone.");
	   return;
    }

    if ( class_table[victim->clss].class_cast_type == CCT_BARD )
    {
	   ch->printlnf( "They cannot be made a vampire." );
	   return;
    }

   if (!IS_NPC(victim) && (victim->level < 20) )
	{
		ch->println( "They are too weak to be made into a childe.");
		return;
	}

   if (!IS_NPC(victim) && (victim->level > 60) )
	{
		ch->println( "They are too powerful to be made into a childe.");
		return;
	}

    if (ch->vampgen == 1 || ch->vampgen == 2) can_sire = true;
/*  if (IS_VAMPAFF(ch,VAM_PRINCE)) can_sire = true;
    if (IS_VAMPAFF(ch,VAM_CAN_SIRE)) can_sire = true;	*/
    if (ch->vampgen > 12) can_sire = false;
    if (ch->level < 29) can_sire = false;

    if (!can_sire)
    {
	   ch->printlnf("You are not able to create any childer.");
	   return;
    }
    
    int clancount;
    clancount = 0;
    if (IS_VAMPAFF(ch,VAM_PROTEAN))       clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_CELERITY))      clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_FORTITUDE))     clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_POTENCE))       clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_OBFUSCATE))     clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_OBTENEBRATION)) clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_SERPENTIS))     clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_AUSPEX))        clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_DOMINATE))      clancount = clancount + 1;
    
    if ( clancount < 2 )
    {
	   ch->printlnf( "First you need to master 2 disciplines." );
	   return;
    }

        if (!IS_NPC(victim) && !IS_SET(race_table[victim->race]->flags, RACEFLAG_CAN_VAMPIRE))
	{
		ch->println( "You can't make them into a vampire.");
		return;
	}
        if (!IS_NPC(victim) && IS_VAMPIRE(victim))
	{
		ch->println( "But they are already a vampire.");
		return;
	}

	if(!IS_SET(victim->act2, ACT2_ALLOWEMBRACE))
	{
		ch->println("They refuse to be embraced by you.");
		return; 
	}

	if (ch->bloodpool < 20 )
	{
		ch->println("You don't have enough blood to create a childe!");
		return;
	}


	ch->bloodpool -= 20;
	act("You sink your teeth into $N, draining thier blood.", ch, NULL, victim, TO_CHAR);
	act("$n sinks $s teeth into $N.", ch, NULL, victim, TO_NOTVICT);
	act("$n sinks $s teeth into your neck and drains your blood.", ch, NULL, victim, TO_VICT);
	act("You slice your wrist open and feed $N your blood.", ch, NULL, victim, TO_CHAR);
	act("$n slices open $s wrist and feeds $N $s blood.", ch, NULL, victim, TO_NOTVICT);
	act("$n slices open $s wrist and you drink their blood from it.", ch, NULL, victim, TO_VICT);

	REMOVE_BIT(ch->vampire, VAM_CAN_SIRE); 
	REMOVE_BIT(victim->act2, ACT2_ALLOWEMBRACE);
        SET_BIT(victim->affected_by2, AFF2_VAMP_BITE);

	CClanType* pClan;
	int rank;
	pClan = clan_lookup("Coven");
	rank=pClan->minRank();
	victim->clan  = pClan;
	victim->clanrank = rank;

	ch->printlnf( "They are now a %s of #`RT`rhe `RC`roven`^.",
		pClan->clan_rank_title(rank) );
	victim->printlnf("You are now a %s of `#`RT`rhe `RC`roven`^.",
		pClan->clan_rank_title(rank) );

	if(ch->vampgen == 12)
	{
		victim->bloodpool = 20;
		victim->vampgen = 13;
	} else
	if(ch->vampgen == 11)
	{
		victim->bloodpool = 30;
		victim->vampgen = 12;
	} else
	if(ch->vampgen == 10)
	{
		victim->bloodpool = 40;
		victim->vampgen = 11;
	} else
	if(ch->vampgen == 9)
	{
		victim->bloodpool = 50;
		victim->vampgen = 10;
	} else
	if(ch->vampgen == 8)
	{
		victim->bloodpool = 60;
		victim->vampgen = 9;
	} else
	if(ch->vampgen == 7)
	{
		victim->bloodpool = 70;
		victim->vampgen = 8;
	} else
	if(ch->vampgen == 6)
	{
		victim->bloodpool = 80;
		victim->vampgen = 7;
	} else
	if(ch->vampgen == 5)
	{
		victim->bloodpool = 90;
		victim->vampgen = 6;
	} else
	if(ch->vampgen == 4)
	{
		victim->bloodpool = 100;
		victim->vampgen = 5;
	} else
	if(ch->vampgen == 3)
	{
		victim->bloodpool = 125;
		victim->vampgen = 4;
	} else
	if(ch->vampgen == 2)
	{
		victim->bloodpool = 150;
		victim->vampgen = 3;
	} else
	if(ch->vampgen == 1)
	{
		victim->bloodpool = 175;
		victim->vampgen = 2;
	}

	if(IS_VAMPAFF(ch, VAM_BRUJAH))
	{
		victim->println("`RYou will awake from death reborn as a vampire of the Brujah Clan.`x");
		SET_BIT(victim->vampire, VAM_BRUJAH);
	}
	if(IS_VAMPAFF(ch, VAM_ASSAMITE))
	{
		victim->println("`RYou will awake from death reborn as a vampire of the Assamite Clan.`x");
		SET_BIT(victim->vampire, VAM_ASSAMITE);
	}
	if(IS_VAMPAFF(ch, VAM_CAPPADOCIAN))
	{
		victim->println("`RYou will awake from death reborn as a vampire of the Cappadocian Clan.`x");
		SET_BIT(victim->vampire, VAM_CAPPADOCIAN);
	}
	if(IS_VAMPAFF(ch, VAM_GANGREL))
	{
		victim->println("`RYou will awake from death reborn as a vampire of the Gangrel Clan.`x");
		SET_BIT(victim->vampire, VAM_GANGREL);
	}
	if(IS_VAMPAFF(ch, VAM_MALKAVIAN))
	{
		victim->println("`RYou will awake from death reborn as a vampire of the Malkavian Clan.`x");
		SET_BIT(victim->vampire, VAM_MALKAVIAN);
	}
	if(IS_VAMPAFF(ch, VAM_NOSFERATU))
	{
		victim->println("`RYou will awake from death reborn as a vampire of the Nosferatu Clan.`x");
		SET_BIT(victim->vampire, VAM_NOSFERATU);
	}
	if(IS_VAMPAFF(ch, VAM_TOREADOR))
	{
		victim->println("`RYou will awake from death reborn as a vampire of the Toreador Clan.`x");
		SET_BIT(victim->vampire, VAM_TOREADOR);
	}
	if(IS_VAMPAFF(ch, VAM_TREMERE))
	{
		victim->println("`RYou will awake from death reborn as a vampire of the Tremere Clan.`x");
		SET_BIT(victim->vampire, VAM_TREMERE);
	}
	if(IS_VAMPAFF(ch, VAM_VENTRUE))
	{
		victim->println("`RYou will awake from death reborn as a vampire of the Ventrue Clan.`x");
		SET_BIT(victim->vampire, VAM_VENTRUE);
	}

	victim->println("`RYou will awake from death reborn as a vampire.`x");
	victim->position = POS_DEAD;
	victim->alliance = -3;
    	kill_char(victim, victim);
	victim->pknorecall = 0;
	victim->pknoquit = 0;
	return;
}
/**************************************************************************/
void do_feed( char_data *ch, char *argument )
{
	char_data *victim;
	char buf[MSL];

	argument = one_argument( argument, buf );

	if ( IS_NPC(ch) )  
		return;

        if (!IS_NPC(ch) && !IS_VAMPIRE(ch))
	{
		do_huh(ch, "");
		return;
	}

	if (IS_AFFECTED2(ch, AFF2_SHADOWPLANE))
	{
		ch->println( "Not while in shadowplane.");
		return;
        }
	if ( ( victim = get_char_room( ch, buf ) ) == NULL )
        {
		ch->println( "They aren't here.");
		return;
        }

	if ( IS_IMMORTAL (victim) && !IS_VAMPIRE(victim))
	{
		ch->println( "Not on Immortals.");
		return;
	}

	if ( ( victim = get_char_room( ch, buf ) ) == NULL )
	{
		ch->println( "They aren't here.");
		return;
	}

	if ( ch == victim )
	{
		ch->println( "That serves no purpose.");
		return;
	}

    if (!IS_VAMPAFF(ch,VAM_FANGS))
    {
	   ch->printlnf("First you better get your fangs out!");
	   return;
    }
    
    if (IS_VAMPAFF(ch,VAM_DISGUISED) || IS_VAMPAFF(ch,VAM_BAT) 
        || IS_VAMPAFF(ch,VAM_MIST) || IS_VAMPAFF(ch,VAM_WOLF) 
        || IS_VAMPAFF(ch,VAM_HUMAN))
    {
	   ch->printlnf("You must reveal your true nature to bite someone.");
	   return;
    }

	if (IS_VAMPIRE(victim) && IS_SET(victim->act2, ACT2_ALLOWFEED))
	{
		act("Your jaw opens wide as you drink from $N.", ch, NULL, victim, TO_CHAR);
		act("$n's jaw opens wide as they drink from $N.", ch, NULL, victim, TO_NOTVICT);
		act("$n's jaw opens wide as they drink from you.", ch, NULL, victim, TO_VICT);
		ch->bloodpool += 5;
		victim->bloodpool -=5;
		return;
	}

	if (victim->subdued == true || IS_SET(victim->act2, ACT2_ALLOWFEED))
	{

		if(ch->vampgen == 13 && ch->bloodpool >= 20 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 20;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 12 && ch->bloodpool >= 30 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 30;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 11 && ch->bloodpool >= 40 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 40;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 10 && ch->bloodpool >= 50 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 50;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 9 && ch->bloodpool >= 60 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 60;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 8 && ch->bloodpool >= 70 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 70;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 7 && ch->bloodpool >= 80 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 80;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 6 && ch->bloodpool >= 90 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 90;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 5 && ch->bloodpool >= 100 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 100;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 4 && ch->bloodpool >= 125 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 125;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 3 && ch->bloodpool >= 150 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 150;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 2 && ch->bloodpool >= 175 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 175;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 1 && ch->bloodpool >= 200 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 200;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		}		


		act("Your jaw opens wide as you feed upon $N's lifeblood.", ch, NULL, victim, TO_CHAR);
		act("$n's jaw opens wide as they feed upon $N's lifeblood.", ch, NULL, victim, TO_NOTVICT);
		act("$n's jaw opens wide as they feed upon your lifeblood.", ch, NULL, victim, TO_VICT);

		victim->hit -= 20;
		ch->bloodpool += 2;

		if(ch->vampgen == 13 && ch->bloodpool >= 20)
		{
			ch->bloodpool = 20;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 12 && ch->bloodpool >= 30)
		{
			ch->bloodpool = 30;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 11 && ch->bloodpool >= 40)
		{
			ch->bloodpool = 40;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 10 && ch->bloodpool >= 50)
		{
			ch->bloodpool = 50;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 9 && ch->bloodpool >= 60)
		{
			ch->bloodpool = 60;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 8 && ch->bloodpool >= 70)
		{
			ch->bloodpool = 70;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 7 && ch->bloodpool >= 80)
		{
			ch->bloodpool = 80;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 6 && ch->bloodpool >= 90)
		{
			ch->bloodpool = 90;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 5 && ch->bloodpool >= 100)
		{
			ch->bloodpool = 100;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 4 && ch->bloodpool >= 125)
		{
			ch->bloodpool = 125;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 3 && ch->bloodpool >= 150)
		{
			ch->bloodpool = 150;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 2 && ch->bloodpool >= 175)
		{
			ch->bloodpool = 175;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 1 && ch->bloodpool >= 200)
		{
			ch->bloodpool = 200;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} 
		gain_condition( ch, COND_THIRST, 10 );
		ch->println( "The blood feels like liquid fire going down your throat." );
		if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			ch->printlnf( "Your thirst for blood is quenched." );
		return;
	}

	if (HAS_CONFIG(ch, CONFIG_ACTIVE) && HAS_CONFIG(victim, CONFIG_ACTIVE))
	{

		if(ch->vampgen == 13 && ch->bloodpool >= 20 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 20;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 12 && ch->bloodpool >= 30 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 30;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 11 && ch->bloodpool >= 40 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 40;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 10 && ch->bloodpool >= 50 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 50;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 9 && ch->bloodpool >= 60 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 60;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 8 && ch->bloodpool >= 70 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 70;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 7 && ch->bloodpool >= 80 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 80;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 6 && ch->bloodpool >= 90 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 90;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 5 && ch->bloodpool >= 100 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 100;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 4 && ch->bloodpool >= 125 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 125;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 3 && ch->bloodpool >= 150 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 150;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 2 && ch->bloodpool >= 175 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 175;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		} else
		if(ch->vampgen == 1 && ch->bloodpool >= 200 && ch->pcdata->condition[COND_THIRST] > 30)
		{
			ch->bloodpool = 200;
			ch->printlnf( "Your thirst is already quenched." );
			return;
		}

		int dam;
		act("Your jaw opens wide as you feed upon $N's lifeblood.", ch, NULL, victim, TO_CHAR);
		act("$n's jaw opens wide as they feed upon $N's lifeblood.", ch, NULL, victim, TO_NOTVICT);
		act("$n's jaw opens wide as they feed upon your lifeblood.", ch, NULL, victim, TO_VICT);
		ch->bloodpool += 2;
		dam = 15 + (ch->level/5+1);
		damage( ch, victim, dam, TYPE_UNDEFINED, DAM_PIERCE, false);

		if(ch->vampgen == 13 && ch->bloodpool >= 20)
		{
			ch->bloodpool = 20;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 12 && ch->bloodpool >= 30)
		{
			ch->bloodpool = 30;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 11 && ch->bloodpool >= 40)
		{
			ch->bloodpool = 40;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 10 && ch->bloodpool >= 50)
		{
			ch->bloodpool = 50;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 9 && ch->bloodpool >= 60)
		{
			ch->bloodpool = 60;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 8 && ch->bloodpool >= 70)
		{
			ch->bloodpool = 70;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 7 && ch->bloodpool >= 80)
		{
			ch->bloodpool = 80;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 6 && ch->bloodpool >= 90)
		{
			ch->bloodpool = 90;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 5 && ch->bloodpool >= 100)
		{
			ch->bloodpool = 100;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 4 && ch->bloodpool >= 125)
		{
			ch->bloodpool = 125;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 3 && ch->bloodpool >= 150)
		{
			ch->bloodpool = 150;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 2 && ch->bloodpool >= 175)
		{
			ch->bloodpool = 175;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		} else
		if(ch->vampgen == 1 && ch->bloodpool >= 200)
		{
			ch->bloodpool = 200;
			if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
			{
				ch->printlnf( "Your thirst for blood is quenched." );
				return;
			}
		}

		return;
	}

	ch->println("They must either be willing or subdued to feed from them.");
	victim->printlnf("%s wishes to feed from your blood. Type `#`YALLOWFEED`# to allow it.", ch->name);
	return;
}
/**************************************************************************/
void do_empower( char_data *ch, char * )
{
	if ( IS_NPC(ch) )  
		return;

        if (!IS_NPC(ch) && !IS_VAMPIRE(ch))
	{
		do_huh(ch, "");
		return;
	}
	
	if(ch->bloodpool <10)
	{
		ch->println("You do not have enough blood to empower yourself.");
		return;
	}

	ch->bloodpool -= 10;
	ch->move = ch->max_move;
	ch->pcdata->tired = 0;
	ch->println("Your blood burns away as you empower your body with energy.");
	ch->println("You are no longer tired and feel refreshed.");
	return;
}

void do_allowfeed( char_data *ch, char *)
{
    if (!IS_NPC(ch) || IS_VAMPIRE(ch))
    {
	if(IS_WEREBEAST(ch))
	{
		ch->println("You cannot do that, you are a werebeast.");
		return;
	}

	if(IS_SET(ch->act2, ACT2_ALLOWFEED))
	{
		REMOVE_BIT(ch->act2, ACT2_ALLOWFEED);
		ch->println("Vampires may no longer feed from you at will.");
		return; 
	}
	else
		SET_BIT(ch->act2, ACT2_ALLOWFEED);
		ch->println("Vampires may now feed from you at will.");
		return; 
   }
   do_huh(ch,"");
   return;
}

void do_allowembrace( char_data *ch, char *)
{
    if (!IS_NPC(ch) && !IS_VAMPIRE(ch))
    {
	if(IS_WEREBEAST(ch))
	{
		ch->println("You cannot do that, you are a werewolf.");
		return;
	}

	if (IS_AFFECTED2(ch, AFF2_NO_VAMP))
	{
		REMOVE_BIT(ch->act2, ACT2_ALLOWEMBRACE);
		ch->println("You are unable to be turned into a vampire.");
		return; 
	}

   if (ch->level > 60)
	{
		ch->println( "You are too powerful to be embraced.");
		return;
	}

	if(IS_SET(ch->act2, ACT2_ALLOWEMBRACE))
	{
		REMOVE_BIT(ch->act2, ACT2_ALLOWEMBRACE);
		ch->println("You refuse to be embraced by a vampire.");
		return; 
	}
	else
		SET_BIT(ch->act2, ACT2_ALLOWEMBRACE);
		ch->println("You willingly allow yourself to be embraced by a vampire.");
		return; 
   }
   do_huh(ch,"");
   return;
}
/***************************************************************************/
void do_wolf_call( char_data *ch, char *)
{
    char_data *animal;
    AFFECT_DATA af;
    char buf[MSL];
    int a_level;

    if(IS_NPC(ch))
	return;

    if(!IS_VAMPIRE(ch) && !IS_WEREBEAST(ch))
    {
	do_huh(ch, "");
        return;
    }

    if(IS_VAMPIRE(ch) && !IS_VAMPAFF(ch, VAM_DOMINATE))
    {
	ch->println("You are not skilled in the Dominate Discipline.");
        return;
    }

    if (is_affected(ch,gsn_wolf_call))
    {
	ch->println("You can't call more wolves yet.");
	return;
    }

    if (IS_WEREBEAST(ch) && ch->mana <= 70)
    {
        ch->println("You do not have the mana.");
        return;
    }


    if (IS_VAMPIRE(ch) && ch->bloodpool < 7)
    {
        ch->println("You do not have enough blood.");
        return;
    }

    if ( ch->pet != NULL )
    {
	ch->printlnf( "You already have a pet." );
	return;
    }

    if ( (ch->in_room->sector_type != SECT_FOREST)
    && (ch->in_room->sector_type != SECT_HILLS)
    && (ch->in_room->sector_type != SECT_MOUNTAIN) 
    && (ch->in_room->sector_type != SECT_TRAIL)
    && (ch->in_room->sector_type != SECT_FIELD) 
    && (ch->in_room->sector_type != SECT_SWAMP)) 
    {
    act("$n calls out into the surroundings but nothing comes.",ch,0,0,TO_ROOM);
    ch->println("You call forth for a wolf but nothing responds.");
    return;
    }

    if (number_percent() > 75)
    {
	ch->println("You call out for wolves but none respond.");
	if(IS_WEREBEAST(ch))
		ch->mana -= 35;
	if(IS_VAMPIRE(ch))
		ch->bloodpool -= 3;
	return;
    }

    if(IS_WEREBEAST(ch))
	ch->mana -= 70;
    if(IS_VAMPIRE(ch))
	ch->bloodpool -= 7;

    a_level = ch->level;

    act("$n calls out to the wild and is heard!.",ch,0,0,TO_ROOM);
    ch->println("Your call to the wild is heard!");

	    animal = create_mobile(get_mob_index(3033), 0 );
	    SET_BIT(animal->act, ACT_PET);
 	    SET_BIT(animal->affected_by, AFF_CHARM);
	    animal->comm = COMM_NOTELL|COMM_NOSHOUT|COMM_NOCHANNELS;
	    animal->level = a_level;
	    animal->max_hit = ch->max_hit + dice(a_level, 5);
	    animal->damroll += a_level*3/4;
	    animal->hitroll += a_level/3;
	    animal->alliance = ch->alliance;

	    sprintf( buf, "%sA neck tag says 'I belong to %s'.\r\n",
			animal->description, ch->short_descr );
	    free_string( animal->description );
	    animal->description = str_dup( buf );

	    char_to_room(animal,ch->in_room);
	    add_follower(animal,ch);
	    animal->leader = ch;
	    ch->pet = animal;
	    act("$n arrives to journey with $N.",animal,NULL,ch,TO_NOTVICT);
	    animal->hit = animal->max_hit;

    af.where = WHERE_AFFECTS;
    af.level = ch->level;
    af.duration = 48;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    af.type = gsn_wolf_call;
    affect_to_char(ch,&af);
    return;
}
/***************************************************************************/
void do_mask( char_data *ch, char *argument )
{
    char_data *victim;
    char      arg [MIL];
    char      buf [MIL];
    
    argument = one_argument( argument, arg );
    
    if (IS_NPC(ch)) 
	 	return;

    if ( !IS_VAMPIRE(ch))
    {
	   do_huh(ch,"");
	   return;
    }
    
    if (!IS_VAMPAFF(ch,VAM_OBFUSCATE) && !IS_IMMORTAL(ch))
    {
	   ch->printlnf("You are not trained in the Obfuscate discipline.");
	   return;
    }
    
    if ( arg[0] == '\0' )
    {
	   ch->printlnf( "Change to look like whom?");
	   return;
    }
    
    
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	   ch->printlnf( "They aren't here." );
	   return;
    }
    
    if( victim->level > ch->level )
    {
	   ch->printlnf("They are too powerful to change into.");
	   return;
    }
    
    if ( IS_NPC(victim) )
    {
	   ch->printlnf( "Not on NPC's." );
	   return;
    }

    if( !IS_ACTIVE(victim) && !IS_IMMORTAL(ch))
    {
	   ch->printlnf( "They are not ACTIVE." );
	   return;
    }
    
    if ( IS_IMMORTAL(victim) && !IS_IMMORTAL(ch) )
    {
	   ch->printlnf( "You wish." );
	   return;
    }
    
    if ( ch == victim )
    {
	   if (!IS_VAMPAFF(ch,VAM_DISGUISED))
	   {
		  ch->printlnf( "You already look like yourself!" );
		  return;
	   }
	   sprintf(buf, "Your form shimmers and transforms into %s.",ch->name);
	   act(buf,ch,NULL,victim,TO_CHAR);
	   sprintf(buf, "%s's form shimmers and transforms into %s.",ch->short_descr, ch->orig_short_descr );
	   act(buf,ch,NULL,victim,TO_ROOM);
	   REMOVE_BIT(ch->vampire, VAM_DISGUISED);

   	   replace_string(ch->short_descr, ch->orig_short_descr); 
   	   replace_string(ch->description,ch->orig_description); 
	   return;
    }

	if ( ch->bloodpool < 40 )
    	{
	   	ch->printlnf("You have insufficient blood.");
	   	return;
    	}
	ch->bloodpool -= number_range(30,40);

    if (IS_VAMPAFF(ch,VAM_DISGUISED))
    {
    	sprintf(buf, "Your form shimmers and transforms into a clone of %s.",victim->short_descr);
    	act(buf,ch,NULL,victim,TO_CHAR);
    	sprintf(buf, "%s's form shimmers and transforms into a clone of %s.", ch->short_descr, victim->short_descr);
    	act(buf,ch,NULL,victim,TO_NOTVICT);
    	sprintf(buf, "%s's form shimmers and transforms into a clone of you!",ch->short_descr);
    	act(buf,ch,NULL,victim,TO_VICT);
    	SET_BIT(ch->vampire, VAM_DISGUISED);

   	replace_string(ch->short_descr, victim->short_descr); 
   	replace_string(ch->description,victim->description);
	return;
    }

    sprintf(buf, "Your form shimmers and transforms into a clone of %s.",victim->short_descr);
    act(buf,ch,NULL,victim,TO_CHAR);
    sprintf(buf, "$n's form shimmers and transforms into a clone of %s.", victim->short_descr);
    act(buf,ch,NULL,victim,TO_NOTVICT);
    sprintf(buf, "%s's form shimmers and transforms into a clone of you!",PERS(ch, victim));
    act(buf,ch,NULL,victim,TO_VICT);
    SET_BIT(ch->vampire, VAM_DISGUISED);

   	replace_string(ch->orig_short_descr, ch->short_descr); 
   	replace_string(ch->short_descr, victim->short_descr);
   	replace_string(ch->orig_description, ch->description); 
   	replace_string(ch->description, victim->description);

    return;
}
/***************************************************************************/
void do_vampire( char_data *ch, char *argument )
{
    char_data *victim;

    if ( ( victim = get_char_world( ch, argument ) ) == NULL )
    {
	ch->println( "They have apparently departed the realm.");
	return;
    }
	
    if (argument[0] == '\0')
    {
		ch->println( "Vampire whom ?" );
		return;
    }
	
    if ( ( victim = get_char_world( ch, argument ) ) == NULL )
    {
		ch->println( "They aren't here." );
		return;
    }
	
    if ( IS_NPC(victim) || IS_AFFECTED2(victim, AFF2_HIGHLANDER))
    {
		ch->println( "Not on NPC's or Highlanders." );
		return;
    }

    if( !IS_ACTIVE(victim) && !IS_IMMORTAL(ch))
    {
	   ch->printlnf( "They are not ACTIVE." );
	   return;
    }

    if ( class_table[victim->clss].class_cast_type == CCT_BARD )
    {
	   ch->printlnf( "They cannot be made a vampire." );
	   return;
    }
    if(!IS_VAMPIRE(victim) && IS_IMMORTAL(victim))
    {
	SET_BIT(victim->vampire, VAM_PROTEAN);
	SET_BIT(victim->vampire, VAM_CELERITY);
	SET_BIT(victim->vampire, VAM_FORTITUDE);
	SET_BIT(victim->vampire, VAM_POTENCE);
	SET_BIT(victim->vampire, VAM_OBFUSCATE);
	SET_BIT(victim->vampire, VAM_AUSPEX);
	SET_BIT(victim->vampire, VAM_OBTENEBRATION);
	SET_BIT(victim->vampire, VAM_SERPENTIS);
	SET_BIT(victim->vampire, VAM_PRINCE);
	SET_BIT(victim->vampire, VAM_DOMINATE);
	SET_BIT(victim->vampire, VAM_BRUJAH);
	SET_BIT(victim->vampire, VAM_ASSAMITE);
	SET_BIT(victim->vampire, VAM_CAPPADOCIAN);
	SET_BIT(victim->vampire, VAM_GANGREL);
	SET_BIT(victim->vampire, VAM_MALKAVIAN);
	SET_BIT(victim->vampire, VAM_NOSFERATU);
	SET_BIT(victim->vampire, VAM_TOREADOR);
	SET_BIT(victim->vampire, VAM_TREMERE);
	SET_BIT(victim->vampire, VAM_VENTRUE);
	SET_BIT(victim->vampire, VAM_IMM_STAKE);
	SET_BIT(victim->affected_by2, AFF2_VAMP_BITE);
	ch->printlnf( "%s is now a 1st Generation Immortal Vampire.", victim->name );
	victim->println( "You are now a 1st Generation Immortal Vampire.");
	victim->vampgen = 1;
	victim->bloodpool = 200;
	return;
}

    if(!IS_VAMPIRE(victim))
    {
	SET_BIT(victim->affected_by2, AFF2_VAMP_BITE);
	ch->printlnf( "%s is now a Vampire.", victim->name );
	victim->println( "You are now a Vampire.");
	victim->alliance = -3;
	victim->vampgen = 13;
	victim->bloodpool = 20;
	return;
    } 

    if(IS_VAMPIRE(victim))
    {
	if (IS_VAMPAFF(victim,VAM_DISGUISED) ) do_mask(ch,ch->name);
	REMOVE_BIT(victim->affected_by2, AFF2_VAMP_BITE);
	REMOVE_BIT(victim->vampire, VAM_FANGS);
	REMOVE_BIT(victim->vampire, VAM_CLAWS);
	REMOVE_BIT(victim->vampire, VAM_PROTEAN);
	REMOVE_BIT(victim->vampire, VAM_CELERITY);
	REMOVE_BIT(victim->vampire, VAM_FORTITUDE);
	REMOVE_BIT(victim->vampire, VAM_POTENCE);
	REMOVE_BIT(victim->vampire, VAM_OBFUSCATE);
	REMOVE_BIT(victim->vampire, VAM_AUSPEX);
	REMOVE_BIT(victim->vampire, VAM_OBTENEBRATION);
	REMOVE_BIT(victim->vampire, VAM_SERPENTIS);
	REMOVE_BIT(victim->vampire, VAM_PRINCE);
	REMOVE_BIT(victim->vampire, VAM_DOMINATE);
	REMOVE_BIT(victim->vampire, VAM_BRUJAH);
	REMOVE_BIT(victim->vampire, VAM_ASSAMITE);
	REMOVE_BIT(victim->vampire, VAM_CAPPADOCIAN);
	REMOVE_BIT(victim->vampire, VAM_GANGREL);
	REMOVE_BIT(victim->vampire, VAM_MALKAVIAN);
	REMOVE_BIT(victim->vampire, VAM_NOSFERATU);
	REMOVE_BIT(victim->vampire, VAM_TOREADOR);
	REMOVE_BIT(victim->vampire, VAM_TREMERE);
	REMOVE_BIT(victim->vampire, VAM_VENTRUE);
	REMOVE_BIT(victim->vampire, VAM_CAN_SIRE);

	REMOVE_BIT(victim->vampire, VAM_DISGUISED);
	REMOVE_BIT(victim->vampire, VAM_BAT);
	REMOVE_BIT(victim->vampire, VAM_WOLF);
	REMOVE_BIT(victim->vampire, VAM_MIST);
	REMOVE_BIT(victim->vampire, VAM_HUMAN);
	ch->printlnf( "%s is no longer a Vampire.", victim->name );
	victim->println( "You no longer a Vampire.");
	victim->vampgen = 0;
	victim->bloodpool = 0;
	return;
    } 
    ch->println("Bug in Vampire code.");
    return;
}
/***************************************************************************/
void do_vampclan( char_data *ch, char *argument )
{
    char_data *victim;
    char      arg1 [MIL];
    char      arg2 [MIL];
    argument = one_argument( argument, arg1);
    argument = one_argument( argument, arg2);

   
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	   ch->printlnf( "Syntax is: vampclan <target> <clan>");
	   return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
		ch->println( "They aren't here." );
		return;
    }
	
    if(!IS_VAMPIRE(victim))
    {
		ch->println( "They are not a vampire." );
		return;
    } 

    if( !IS_ACTIVE(victim) && !IS_IMMORTAL(ch))
    {
	   ch->printlnf( "They are not ACTIVE." );
	   return;
    }
	if (IS_VAMPAFF(victim,VAM_DISGUISED) ) do_mask(ch,ch->name);
	REMOVE_BIT(victim->vampire, VAM_FANGS);
	REMOVE_BIT(victim->vampire, VAM_CLAWS);
	REMOVE_BIT(victim->vampire, VAM_PROTEAN);
	REMOVE_BIT(victim->vampire, VAM_CELERITY);
	REMOVE_BIT(victim->vampire, VAM_FORTITUDE);
	REMOVE_BIT(victim->vampire, VAM_POTENCE);
	REMOVE_BIT(victim->vampire, VAM_OBFUSCATE);
	REMOVE_BIT(victim->vampire, VAM_AUSPEX);
	REMOVE_BIT(victim->vampire, VAM_OBTENEBRATION);
	REMOVE_BIT(victim->vampire, VAM_SERPENTIS);
	REMOVE_BIT(victim->vampire, VAM_PRINCE);
	REMOVE_BIT(victim->vampire, VAM_DOMINATE);
	REMOVE_BIT(victim->vampire, VAM_BRUJAH);
	REMOVE_BIT(victim->vampire, VAM_ASSAMITE);
	REMOVE_BIT(victim->vampire, VAM_CAPPADOCIAN);
	REMOVE_BIT(victim->vampire, VAM_GANGREL);
	REMOVE_BIT(victim->vampire, VAM_MALKAVIAN);
	REMOVE_BIT(victim->vampire, VAM_NOSFERATU);
	REMOVE_BIT(victim->vampire, VAM_TOREADOR);
	REMOVE_BIT(victim->vampire, VAM_TREMERE);
	REMOVE_BIT(victim->vampire, VAM_VENTRUE);
	REMOVE_BIT(victim->vampire, VAM_CAN_SIRE);
	REMOVE_BIT(victim->vampire, VAM_DISGUISED);
	REMOVE_BIT(victim->vampire, VAM_BAT);
	REMOVE_BIT(victim->vampire, VAM_WOLF);
	REMOVE_BIT(victim->vampire, VAM_MIST);
	REMOVE_BIT(victim->vampire, VAM_HUMAN);

    	if ( !str_cmp(arg2,"brujah"))
	{
		ch->printlnf( "%s is Brujah Clan Vampire.", victim->name );
		victim->println( "You are now a Brujah Clan Vampire.");
		SET_BIT(victim->vampire, VAM_BRUJAH);
		return;
	}

    	if ( !str_cmp(arg2,"assamite"))
	{
		ch->printlnf( "%s is Assamite Clan Vampire.", victim->name );
		victim->println( "You are now a Assamite Clan Vampire.");
		SET_BIT(victim->vampire, VAM_ASSAMITE);
		return;
	}

    	if ( !str_cmp(arg2,"cappadocian"))
	{
		ch->printlnf( "%s is Cappadocian Clan Vampire.", victim->name );
		victim->println( "You are now a Cappadocian Clan Vampire.");
		SET_BIT(victim->vampire, VAM_CAPPADOCIAN);
		return;
	}
    	if ( !str_cmp(arg2,"gangrel"))
	{
		ch->printlnf( "%s is Gangrel Clan Vampire.", victim->name );
		victim->println( "You are now a Gangrel Clan Vampire.");
		SET_BIT(victim->vampire, VAM_GANGREL);
		return;
	}
    	if ( !str_cmp(arg2,"malkavian"))
	{
		ch->printlnf( "%s is Malkavian Clan Vampire.", victim->name );
		victim->println( "You are now a Malkavian Clan Vampire.");
		SET_BIT(victim->vampire, VAM_MALKAVIAN);
		return;
	}
    	if ( !str_cmp(arg2,"nosferatu"))
	{
		ch->printlnf( "%s is Nosferatu Clan Vampire.", victim->name );
		victim->println( "You are now a Nosferatu Clan Vampire.");
		SET_BIT(victim->vampire, VAM_NOSFERATU);
		return;
	}
    	if ( !str_cmp(arg2,"toreador"))
	{
		ch->printlnf( "%s is Toreador Clan Vampire.", victim->name );
		victim->println( "You are now a Toreador Clan Vampire.");
		SET_BIT(victim->vampire, VAM_TOREADOR);
		return;
	}
    	if ( !str_cmp(arg2,"tremere"))
	{
		ch->printlnf( "%s is Tremere Clan Vampire.", victim->name );
		victim->println( "You are now a Tremere Clan Vampire.");
		SET_BIT(victim->vampire, VAM_TREMERE);
		return;
	}
    	if ( !str_cmp(arg2,"ventrue"))
	{
		ch->printlnf( "%s is Ventrue Clan Vampire.", victim->name );
		victim->println( "You are now a Ventrue Clan Vampire.");
		SET_BIT(victim->vampire, VAM_VENTRUE);
		return;
	}

   ch->printlnf( "Syntax is: vampclan <target> <clan>");
   return;

}

void do_vampchange( char_data *ch, char *argument )
{
    char arg[MIL];

    if(!IS_VAMPIRE(ch))
    {
	do_huh(ch,"");
	return;
    }

    if(IS_VAMPIRE(ch) && !IS_VAMPAFF(ch, VAM_PROTEAN) && !IS_IMMORTAL(ch))
    {
	ch->println("You are not skilled in the Protean Discipline.");
        return;
    }

    if (ch->position == POS_SLEEPING)
    {
	ch->println("You can't do that while sleeping.");
	return;
    }

    if ( ch->fighting ) 
    {
	ch->println("You seem to be too busy do anything.");
	return;
    }

    char_data *vamp;

    if (IS_VAMPAFF(ch, VAM_BAT))
    {
		ch->description = str_dup(ch->orig_description);
		ch->short_descr = str_dup(ch->orig_short_descr);

		REMOVE_BIT(ch->vampire, VAM_BAT);
    	act("A vampire bat suddenly transforms into $n!", ch, NULL, NULL, TO_ROOM);
    	ch->println("You transform into your vampiric form.");
    	save_char_obj( ch );
 	reset_char(ch);
    	update_pos( ch );
    	WAIT_STATE( ch, 10 );
    	return;
    }


    if (IS_VAMPAFF(ch, VAM_WOLF))
    {
		ch->description = str_dup(ch->orig_description);
		ch->short_descr = str_dup(ch->orig_short_descr);

		REMOVE_BIT(ch->vampire, VAM_WOLF);
    	ch->println("You transform into your vampiric form.");
    	act("A large wolf collapses on the ground in agony and transforms into $n!", ch, NULL, NULL, TO_ROOM);
    	save_char_obj( ch );
 	reset_char(ch);
    	update_pos( ch );
    	WAIT_STATE( ch, 10 );
    	return;
    }


    if (IS_VAMPAFF(ch, VAM_MIST))
    {
		REMOVE_BIT(ch->vampire, VAM_MIST);
		REMOVE_BIT(ch->affected_by2, AFF2_ETHEREAL);
		REMOVE_BIT(ch->act, ACT_IS_UNSEEN);
		REMOVE_CONFIG(ch, CONFIG_DISALLOWED_PKILL);
		act("$n suddenly appears from a cloud of mist!", ch,NULL,NULL,TO_ROOM);
    	ch->println("You transform into your vampiric form.");
    	save_char_obj( ch );
    	update_pos( ch );
    	WAIT_STATE( ch, 10 );
    	return;
    }

    if ( ch->bloodpool <20  ) 
    {
	ch->println("Your bloodpool is too low.");
	return;
    }

    argument=one_argument(argument,arg);
	
    if (arg[0] == '\0')
    {
	ch->println("`BSyntax: `xvampchange <form>");
	ch->println("You can transform into a bat, wolf, or mist.");
	return;
    }
	
    if (!str_prefix(arg,"bat"))
    {
		SET_BIT(ch->vampire, VAM_BAT);
		vamp = create_mobile( get_mob_index(60004), 0 );
      ch->bloodpool -= 20;
		act("$n suddenly transforms into a bat!", ch,NULL,NULL,TO_ROOM);
    
	ch->orig_short_descr 	= ch->short_descr;
    	ch->short_descr 	= vamp->short_descr;
    	ch->orig_description 	= ch->description;
    	ch->description 	= vamp->description;

    	extract_char(vamp,true);

    	affect_parentspellfunc_strip( ch, gsn_vampire);
    	AFFECT_DATA af;
    	af.where     = WHERE_AFFECTS;
    	af.type      = gsn_vampire;
    	af.level     = 120;
    	af.duration  = -1;
    	af.location  = APPLY_NONE;
    	af.modifier  = 0;
    	af.bitvector = 0;
    	af.bitvector = AFF_FLYING;
    	affect_to_char( ch, &af );

    	ch->println("Pain shoots through your body as you transform into a vampire bat.");
    	save_char_obj( ch );
    	update_pos( ch );
    	WAIT_STATE( ch, 10 );
    	return;
    }
    else
    if (!str_prefix(arg,"wolf"))
    {
		SET_BIT(ch->vampire, VAM_WOLF);
		vamp = create_mobile( get_mob_index(60005), 0 );
      ch->bloodpool -= 20;
		act("$n suddenly transforms into a wolf!", ch,NULL,NULL,TO_ROOM);

	ch->orig_short_descr 	= ch->short_descr;
    	ch->short_descr 	= vamp->short_descr;
    	ch->orig_description 	= ch->description;
    	ch->description 	= vamp->description;
    	extract_char(vamp,true);

    	affect_parentspellfunc_strip( ch, gsn_vampire);
    	AFFECT_DATA af;
    	af.where     = WHERE_AFFECTS;
    	af.type      = gsn_vampire;
    	af.level     = 120;
    	af.duration  = -1;
    	af.location  = APPLY_NONE;
    	af.modifier  = 0;
    	af.bitvector = 0;
    	af.bitvector = AFF_SNEAK;
    	affect_to_char( ch, &af );
    	af.bitvector = AFF_DETECT_HIDDEN;
    	affect_to_char( ch, &af );

    	affect_parentspellfunc_strip( ch, gsn_augment_hearing);

    	af.where	= WHERE_AFFECTS;
    	af.type      	= gsn_augment_hearing;
    	af.level	= 120;
    	af.duration  	= -1;
    	af.location  	= APPLY_NONE;
    	af.modifier  	= 0;
    	af.bitvector 	= 0;
    	affect_to_char( ch, &af );

    	ch->println("Pain shoots through your body as you transform into a wolf.");
    	save_char_obj( ch );
    	update_pos( ch );
    	WAIT_STATE( ch, 10 );
    	return;
    }
    else
    if (!str_prefix(arg,"mist"))
    {
		ch->orig_description = ch->description;
		ch->orig_short_descr = ch->short_descr;

		SET_BIT(ch->vampire, VAM_MIST);
		SET_BIT(ch->affected_by2, AFF2_ETHEREAL);
		SET_BIT(ch->act, ACT_IS_UNSEEN);
      ch->bloodpool -= 20;
		SET_CONFIG(ch, CONFIG_DISALLOWED_PKILL);
		act("$n suddenly transforms into a cloud of mist!", ch,NULL,NULL,TO_ROOM);
    	ch->println("You transform into an ethereal mist.");
    	save_char_obj( ch );
    	update_pos( ch );
    	WAIT_STATE( ch, 10 );
    	return;
    }
    else 
    {   
	ch->println("`BSyntax: `xvampchange <form>");
	ch->println("You can transform into a bat, wolf, or mist.");
	return;
    }
    return;
}


void do_vampdestroy( char_data *ch, char *argument )
{
    char arg[MIL];
    char_data *victim;

    if(!IS_VAMPIRE(ch))
    {
	ch->println("You are not a vampire.");
	return;
    }

    if( (is_name( "Coven", ch->clan->notename()) && ch->clanrank==0 ) || IS_IMMORTAL(ch) )  
    {	

  	if (ch->position == POS_SLEEPING)
	{
		ch->println("You can't do that while sleeping.");
		return;
    	}

    	if ( ch->bloodpool <10 && !IS_IMMORTAL(ch) ) 
    	{
		ch->println("Your bloodpool is too low.");
		return;
    	}

    	argument=one_argument(argument,arg);
	
    	if (arg[0] == '\0')
    	{
		ch->println("`BSyntax: `xvampdestroy <victim>");
		return;
    	}

    	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    	{
		ch->println("They aren't here.");
		return;
    	}

    	if ( !IS_VAMPIRE(victim) )
    	{
		ch->println("They are not a vampire.");
		return;
    	}

    	if ( IS_IMMORTAL(victim) )
    	{
		ch->println("Not on Immortals.");
		return;
    	}

    	if ( victim == ch )
    	{
		ch->println("You feel your blood burn a bit.");
		return;
    	}	

	if (ch->vampgen > victim->vampgen)
    	{
		ch->println("They are more powerful than you are.");
		return;
    	}

       	act("`#`Y$N suddenly erupts in flames and burns to ash!`^", ch, NULL,victim,TO_NOTVICT);
	victim->println("`#Your `rblood`^ boils suddenly and you erupt in `Rflames`^, burning to `Sash!`^");
	victim->println("You have been `RDESTROYED!!!`x");
	act("`#`YYou incinerate $N as they explode in `Rflames`^, turning to `Sash`R!`^",ch,NULL,victim,TO_CHAR);
	if(!IS_IMMORTAL(ch))
	{
		ch->bloodpool -= 10;
	}
	REMOVE_BIT(victim->affected_by2, AFF2_VAMP_BITE);
	SET_BIT(victim->affected_by2, AFF2_NO_VAMP);
	if (IS_VAMPAFF(victim,VAM_DISGUISED) ) do_mask(ch,ch->name);
	REMOVE_BIT(victim->vampire, VAM_FANGS);
	REMOVE_BIT(victim->vampire, VAM_CLAWS);
	REMOVE_BIT(victim->vampire, VAM_PROTEAN);
	REMOVE_BIT(victim->vampire, VAM_CELERITY);
	REMOVE_BIT(victim->vampire, VAM_FORTITUDE);
	REMOVE_BIT(victim->vampire, VAM_POTENCE);
	REMOVE_BIT(victim->vampire, VAM_OBFUSCATE);
	REMOVE_BIT(victim->vampire, VAM_AUSPEX);
	REMOVE_BIT(victim->vampire, VAM_OBTENEBRATION);
	REMOVE_BIT(victim->vampire, VAM_SERPENTIS);
	REMOVE_BIT(victim->vampire, VAM_PRINCE);
	REMOVE_BIT(victim->vampire, VAM_DOMINATE);
	REMOVE_BIT(victim->vampire, VAM_BRUJAH);
	REMOVE_BIT(victim->vampire, VAM_ASSAMITE);
	REMOVE_BIT(victim->vampire, VAM_CAPPADOCIAN);
	REMOVE_BIT(victim->vampire, VAM_GANGREL);
	REMOVE_BIT(victim->vampire, VAM_MALKAVIAN);
	REMOVE_BIT(victim->vampire, VAM_NOSFERATU);
	REMOVE_BIT(victim->vampire, VAM_TOREADOR);
	REMOVE_BIT(victim->vampire, VAM_TREMERE);
	REMOVE_BIT(victim->vampire, VAM_VENTRUE);
	REMOVE_BIT(victim->vampire, VAM_CAN_SIRE);
	REMOVE_BIT(victim->vampire, VAM_DISGUISED);
	REMOVE_BIT(victim->vampire, VAM_BAT);
	REMOVE_BIT(victim->vampire, VAM_WOLF);
	REMOVE_BIT(victim->vampire, VAM_MIST);
	REMOVE_BIT(victim->vampire, VAM_HUMAN);
	affect_parentspellfunc_strip( victim, gsn_vampire);
	victim->clan	= NULL;
	victim->clanrank = 0;
    	raw_kill(victim, victim);
	return;
    }else

    do_huh(ch, "");
    return;
}

void do_hypnotize( char_data *ch, char *argument )
{
    char arg[MIL];
    char_data *victim;

    if(!IS_VAMPIRE(ch))
    {
	do_huh(ch,"");
	return;
    }

    if(IS_VAMPIRE(ch) && !IS_VAMPAFF(ch, VAM_DOMINATE))
    {
	ch->println("You are not skilled in the Dominate Discipline.");
        return;
    }

  	if (ch->position == POS_SLEEPING)
	{
		ch->println("You can't do that while sleeping.");
		return;
    	}

    	if ( ch->bloodpool < 10  ) 
    	{
		ch->println("Your bloodpool is too low.");
		return;
    	}

	if (IS_AFFECTED2(ch, AFF2_SHADOWPLANE))
	{
		ch->println( "Not while in shadowplane.");
		return;
        }

    	argument=one_argument(argument,arg);
	
    	if (arg[0] == '\0')
    	{
		ch->println("`BSyntax: `xhypnotize <victim>");
		return;
    	}

    	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    	{
		ch->println("They aren't here.");
		return;
    	}

    	if ( IS_VAMPIRE(victim) )
    	{
		ch->println("Your power only works on mortals.");
		return;
    	}

    	if ( IS_IMMORTAL(victim) && !IS_IMMORTAL(ch) )
    	{
		ch->println("Not on Immortals.");
		return;
    	}

    act("$n gazes into $N's eyes.",ch,NULL,victim,TO_NOTVICT);
    act("$n gazes into your eyes.",ch,NULL,victim,TO_VICT);
    act("`#`YYou gaze into the eyes of $N to mesmerize them`^.",ch,NULL,victim,TO_CHAR);

    if (saves_spell(ch->level-1, victim, DAM_MENTAL) )
    {
	act("$n resists your dark power.",victim,NULL,NULL,TO_ROOM);
 	act("You feel a strange feeling come over you.",victim,NULL,NULL,TO_CHAR);
	ch->bloodpool -= 2;
	return;
    }

    ch->bloodpool -= 10;
    victim->subdued=true;
    victim->subdued_timer = 2; 
    victim->position=POS_RESTING;

    act("$N appears to be transfixed by $n's eyes.",ch,NULL,victim,TO_NOTVICT);
    act("`#`W$N is transfixed by your gaze.`^",ch,NULL,victim,TO_CHAR);
    victim->printlnf("`WYou feel transfixed by $n's eyes.`x");
    victim->position = POS_STUNNED;
    return;
}

void do_vampfear( char_data *ch, char *argument )
{
    char arg[MIL];
    char_data *victim;
    int attempt;
    ROOM_INDEX_DATA *was_in;
    bool found;
    EXIT_DATA *pexit;
    int door;

    if(!IS_VAMPIRE(ch))
    {
	do_huh(ch,"");
	return;
    }

    if(IS_VAMPIRE(ch) && !IS_VAMPAFF(ch, VAM_DOMINATE))
    {
	ch->println("You are not skilled in the Dominate Discipline.");
        return;
    }

  	if (ch->position == POS_SLEEPING)
	{
		ch->println("You can't do that while sleeping.");
		return;
    	}

    	if ( ch->bloodpool < 15  ) 
    	{
		ch->println("Your bloodpool is too low.");
		return;
    	}

    	argument=one_argument(argument,arg);
	
    	if (arg[0] == '\0')
    	{
		ch->println("`BSyntax: `xhypnotize <victim>");
		return;
    	}

    	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    	{
		ch->println("They aren't here.");
		return;
    	}

    	if ( IS_VAMPIRE(victim) )
    	{
		ch->println("Your power only works on mortals.");
		return;
    	}

    	if ( IS_IMMORTAL(victim) && !IS_IMMORTAL(ch) )
    	{
		ch->println("Not on Immortals.");
		return;
    	}

	
    if (IS_OOC(victim))
	return;
	
	// record the room they start in
    was_in = victim->in_room;
	
    switch(victim->position)
    {
	case POS_SLEEPING:
		victim->println("You have nightmares about vampires sucking your blood.\r\n");
		victim->println("You SCREAM and wake up, sweat running down your forehead.");
		victim->position = POS_STANDING;
		break;
		
	case POS_RESTING:
	case POS_SITTING:
	case POS_KNEELING:
		victim->println("You stand up, screaming in fright from an unknown presence nearby.");
		victim->position = POS_STANDING;
		
	case POS_FIGHTING:
	case POS_STANDING:
		if (victim->mounted_on!=NULL)
		{
			victim->println("You leap from the fiend you are sitting on in mortal terror.");
			dismount(victim);
		}
		victim->println("As a dark presence surrounds you, you scream for your life and run.");
    }
    
	ch->bloodpool -= 15;

	// do a 'flee' from room 
	for (attempt = 0; attempt < 10; attempt++ )
	{
		found = false;
		
		door = number_door( );
		
		if ( ( pexit = was_in->exit[door] ) == 0
			||   pexit->u1.to_room == NULL
			||   IS_SET(pexit->exit_info, EX_CLOSED)
			||   number_range(0,victim->daze) != 0
			||  (IS_NPC(victim)          
			&&   IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB )))
			continue;
		
		act("$n screams and runs out the room!", victim, NULL, NULL, TO_ROOM );
		move_char( victim, door, false, NULL);
		
		// keep going till we get out of the room
		if(victim->in_room != was_in )
			continue;
		
		if (!IS_NPC(victim))
			victim->println("You scream and run away in terror!");
		
		stop_fighting( victim, true ); // incase they were fighting 
		return;
	}
	victim->println("PANIC! You can't seem to escape the horror!");
	return;
}

void do_stake( char_data *ch, char *argument )
{
    char arg[MIL];
    char_data *victim;

    if(IS_VAMPIRE(ch))
    {
	do_huh(ch,"");
	return;
    }

  	if (ch->position == POS_SLEEPING)
	{
		ch->println("You can't do that while sleeping.");
		return;
    }

    	argument=one_argument(argument,arg);
	
    	if (arg[0] == '\0')
    	{
		ch->println("`BSyntax: `xstake <victim>");
		return;
    	}

    	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    	{
		ch->println("They aren't here.");
		return;
    	}

    	if(IS_SET(victim->in_room->room2_flags,ROOM2_ARENA) ||
      	  IS_SET(victim->in_room->room2_flags,ROOM2_WAR) )
    	{
		ch->println("Not while in this combat zone, you cheater!");
		return;
    	}

    	if( !IS_ACTIVE(victim))
    	{
	   ch->printlnf("They are not ACTIVE." );
	   return;
    	}

    	if( !IS_ACTIVE(ch))
    	{
	   ch->printlnf("You are not ACTIVE." );
	   return;
    	}

    OBJ_DATA *obj;
    bool foundstake = false;

    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
        if ( !str_cmp( obj->pIndexData->material, "stake" ) && obj->wear_loc == WEAR_HOLD )
	{
		foundstake = true;
            	break;
	}
    }

	if ( !foundstake ) 
	{
		ch->println("You need to be holding a stake to do that.");
		return;
	}

    	if ( IS_IMMORTAL(victim) )
    	{
		ch->println("Not on Immortals.");
		return;
    	}

    	if ( victim == ch )
    	{
		ch->println("You going to have a hard time doing that.");
		return;
    	}	

    	if (victim->position != POS_SLEEPING)
    	{
	   	ch->println( "You can only stake someone who is sleeping.");
	   	return;
    	}

    	if (IS_VAMPAFF(victim,VAM_IMM_STAKE) )
    	{
	   	ch->printlnf("They don't have a heart to stake!");
	   	return;
    	}

    	if ( !IS_VAMPIRE(victim) )
    	{
		obj_from_char(obj);
		extract_obj(obj);
		save_char_obj(ch);
		ch->printlnf("`#%s looks very angry `Yas you attempted tp `Wstake `Ythem!`^", victim->name);
		act( "`#$n tried to `Wstake`^ $N! `SBad idea.`^",  ch, NULL, victim, TO_NOTVICT );
		act( "`#$n `Yhas tried to `Wstake `Yyou!`^", ch, NULL, victim, TO_VICT);
		damage(ch,victim,1,0,DAM_NONE,false);
		return;
    	}

    	if (victim->position > POS_MORTAL)
    	{
		ch->println("They are not weak enough to stake.");
		return;
	}

    	if (victim->position == POS_SLEEPING)
    	{

		obj_from_char(obj);
		extract_obj(obj);
       		act("`#`Y$N suddenly turns to dust as $n drives a stake into their heart!`^", ch, NULL,victim,TO_NOTVICT);
		victim->printlnf("`#`YYou scream in anguish as %s drives a stake through your heart!`^", ch->name);
		victim->println("You have been `RDESTROYED!!!`x");
		act("`#`YYou drive a stake through the heart of $N and they turn to dust!`^",ch,NULL,victim,TO_CHAR);
		victim->bloodpool = 0;
		victim->vampgen = 0;

		REMOVE_BIT(victim->affected_by2, AFF2_VAMP_BITE);
		SET_BIT(victim->affected_by2, AFF2_NO_VAMP);

		/* Remove physical vampire attributes when you take mortal form */
		if (IS_VAMPAFF(victim,VAM_DISGUISED) ) do_mask(ch,ch->name);

		REMOVE_BIT(victim->vampire, VAM_FANGS);
		REMOVE_BIT(victim->vampire, VAM_CLAWS);
		REMOVE_BIT(victim->vampire, VAM_PROTEAN);
		REMOVE_BIT(victim->vampire, VAM_CELERITY);
		REMOVE_BIT(victim->vampire, VAM_FORTITUDE);
		REMOVE_BIT(victim->vampire, VAM_POTENCE);
		REMOVE_BIT(victim->vampire, VAM_OBFUSCATE);
		REMOVE_BIT(victim->vampire, VAM_AUSPEX);
		REMOVE_BIT(victim->vampire, VAM_OBTENEBRATION);
		REMOVE_BIT(victim->vampire, VAM_SERPENTIS);
		REMOVE_BIT(victim->vampire, VAM_PRINCE);
		REMOVE_BIT(victim->vampire, VAM_DOMINATE);
		REMOVE_BIT(victim->vampire, VAM_BRUJAH);
		REMOVE_BIT(victim->vampire, VAM_ASSAMITE);
		REMOVE_BIT(victim->vampire, VAM_CAPPADOCIAN);
		REMOVE_BIT(victim->vampire, VAM_GANGREL);
		REMOVE_BIT(victim->vampire, VAM_MALKAVIAN);
		REMOVE_BIT(victim->vampire, VAM_NOSFERATU);
		REMOVE_BIT(victim->vampire, VAM_TOREADOR);
		REMOVE_BIT(victim->vampire, VAM_TREMERE);
		REMOVE_BIT(victim->vampire, VAM_VENTRUE);
		REMOVE_BIT(victim->vampire, VAM_CAN_SIRE);
		REMOVE_BIT(victim->vampire, VAM_DISGUISED);
		REMOVE_BIT(victim->vampire, VAM_BAT);
		REMOVE_BIT(victim->vampire, VAM_WOLF);
		REMOVE_BIT(victim->vampire, VAM_MIST);
		REMOVE_BIT(victim->vampire, VAM_HUMAN);
		victim->clan	= NULL;
		victim->clanrank = 0;
		affect_parentspellfunc_strip( victim, gsn_vampire);
    		raw_kill(victim, victim);
		return;
	}

    	if (victim->position <= POS_MORTAL)
    	{

		obj_from_char(obj);
		extract_obj(obj);
       		act("`#`Y$N suddenly turns to dust as $n drives a stake into their heart!`^", ch, NULL,victim,TO_NOTVICT);
		victim->printlnf("`#`YYou scream in anguish as %s drives a stake through your heart!`^", ch->name);
		act("`#`YYou drive a stake through the heart of $N and they turn to dust!`^",ch,NULL,victim,TO_CHAR);
		victim->vampgen -= 1;

		if(victim->vampgen <= 14)
		{
			victim->printlnf("You have been `RDESTROYED!!!`x");
			victim->printlnf("Your memories of your days as a vampire fade into nothingness.");
			victim->bloodpool = 0;
			victim->vampgen = 0;

			REMOVE_BIT(victim->affected_by2, AFF2_VAMP_BITE);
			SET_BIT(victim->affected_by2, AFF2_NO_VAMP);

			/* Remove physical vampire attributes when you take mortal form */
			if (IS_VAMPAFF(victim,VAM_DISGUISED) ) do_mask(ch,ch->name);
	
			REMOVE_BIT(victim->vampire, VAM_FANGS);
			REMOVE_BIT(victim->vampire, VAM_CLAWS);
			REMOVE_BIT(victim->vampire, VAM_PROTEAN);
			REMOVE_BIT(victim->vampire, VAM_CELERITY);
			REMOVE_BIT(victim->vampire, VAM_FORTITUDE);
			REMOVE_BIT(victim->vampire, VAM_POTENCE);
			REMOVE_BIT(victim->vampire, VAM_OBFUSCATE);
			REMOVE_BIT(victim->vampire, VAM_AUSPEX);
			REMOVE_BIT(victim->vampire, VAM_OBTENEBRATION);
			REMOVE_BIT(victim->vampire, VAM_SERPENTIS);
			REMOVE_BIT(victim->vampire, VAM_PRINCE);
			REMOVE_BIT(victim->vampire, VAM_DOMINATE);
			REMOVE_BIT(victim->vampire, VAM_BRUJAH);
			REMOVE_BIT(victim->vampire, VAM_ASSAMITE);
			REMOVE_BIT(victim->vampire, VAM_CAPPADOCIAN);
			REMOVE_BIT(victim->vampire, VAM_GANGREL);
			REMOVE_BIT(victim->vampire, VAM_MALKAVIAN);
			REMOVE_BIT(victim->vampire, VAM_NOSFERATU);
			REMOVE_BIT(victim->vampire, VAM_TOREADOR);
			REMOVE_BIT(victim->vampire, VAM_TREMERE);
			REMOVE_BIT(victim->vampire, VAM_VENTRUE);
			REMOVE_BIT(victim->vampire, VAM_CAN_SIRE);
			REMOVE_BIT(victim->vampire, VAM_DISGUISED);
			REMOVE_BIT(victim->vampire, VAM_BAT);
			REMOVE_BIT(victim->vampire, VAM_WOLF);
			REMOVE_BIT(victim->vampire, VAM_MIST);
			REMOVE_BIT(victim->vampire, VAM_HUMAN);
			affect_parentspellfunc_strip( victim, gsn_vampire);
    			raw_kill(victim, victim);
			return;
		}
		victim->printlnf("`#`YYou have lost a generation!!!`^");
		raw_kill(victim, victim);
		return;
	}
}

/**************************************************************************/
void do_wereshort( char_data *ch, char *argument)
{
	char_data *victim;
	char arg[MIL];
	char oarg[MSL];

	sprintf( oarg,"%s", argument);
	
	argument = one_argument( argument, arg );
	
	if(IS_NULLSTR(arg)){
		ch->printlnf( "Syntax: altshort <playername> an alt short description for the player" );
		ch->printlnf( "This command is used to set alternate short descriptions for shapechangers." );
		return;
	}
	
	if(strlen(arg)<3){
		ch->printlnf( "You must use more than 2 characters to specify players name." );
		return;
	}

    if ( ( victim = get_whovis_player_world( ch, arg ) ) == NULL )
    {
		if ( ( victim = get_char_world( ch, arg ) ) == NULL )
		{
			ch->printlnf( "They aren't here." );
			return;
		}
	}

    // trim the spaces to the right of the short
    while ( !IS_NULLSTR(argument) && isspace(argument[strlen(argument)-1]))
    {
        argument[strlen(argument)-1]='\0';
    }

    // check we have a short left 
    if(argument[0] == '\0' )
    {
        ch->printlnf( "You must put in a short description." );
        return;
	}

    // make sure first char is lowercase 
    argument[0] = LOWER(argument[0]);

    if ((get_trust(victim)>= get_trust(ch))&& (ch != victim))
        ch->printlnf( "You can't set a short of someone a higher level or equal to you." );
    else
    {
		// questing wiznet
		if (TRUE_CH(ch))
		{
			char qbuf[MSL];
			sprintf (qbuf, "`mQUEST> %s alternate short %s`x\r\n", 
				TRUE_CH(ch)->name, oarg); 
			wiznet(qbuf,ch,NULL,WIZ_QUESTING,0,LEVEL_IMMORTAL);
		}

        // inform the imm and the room victim is in 
        ch->wraplnf("You have changed %s's alternate short description from '%s' to '%s'.\r\n",
                                victim->name, victim->altform.short_descr, argument);

        act( "$n is now known as '$T'", victim, NULL, argument, TO_ROOM );

        // change the short 
        victim->altform.short_descr = str_dup( argument);
		
        victim->wraplnf("Your alternate short description has been changed to '%s' by %s.\r\n",
			victim->orig_short_descr, can_see_who(victim, ch)?ch->name:"a mystery imm");

        if (strlen(argument)>55){
			ch->printlnf( "That alternate short descriptions is LONG... it has been set anyway...\r\n"
				"but consider the length when setting a short." );
		}
    }

    save_char_obj( victim );
	return ;
}

/**************************************************************************/
void do_wolfdesc( char_data *ch, char * )
{
	if(IS_NPC(ch))
        return;

	ch->println("Entering edit mode for you own alternate description." );
	ch->println("Type @ to finish editing your alternate description." );
	string_append(ch, &ch->altform.description);

	return;

}
/***************************************************************************/
void do_favor( char_data *ch, char *argument )
{
    char_data *victim;
    char      arg1 [MIL];
    char      arg2 [MIL];
    argument = one_argument( argument, arg1);
    argument = one_argument( argument, arg2);

    if (IS_NPC(ch)) 
	 	return;

    if ( !IS_VAMPAFF(ch, VAM_PRINCE) && !IS_IMMORTAL(ch) )
    {
	   do_huh(ch,"");
	   return;
    }
    
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	   ch->printlnf( "`#`BSyntax:`xfavor <target> <prince/sire>");
	   return;
    }
    
    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	   ch->printlnf( "They aren't here.");
	   return;
    }
    
    if ( IS_NPC(victim) )
    {
	   ch->printlnf( "Not on NPC's.");
	   return;
    }
    
    if ( ch == victim )
    {
	   ch->printlnf( "Not on yourself!");
	   return;
    }
    
    if ( !IS_VAMPIRE(victim))
    {
	   ch->printlnf( "But they are not a vampire!");
	   return;
    }

 if(!IS_IMMORTAL(ch))
 {
    if (IS_VAMPAFF(ch, VAM_BRUJAH) && !IS_VAMPAFF(victim, VAM_BRUJAH))
    {
	   ch->printlnf( "You can only grant your favor to someone in your bloodline.");
	   return;
    }

    if (IS_VAMPAFF(ch, VAM_VENTRUE) && !IS_VAMPAFF(victim, VAM_VENTRUE))
    {
	   ch->printlnf( "You can only grant your favor to someone in your bloodline.");
	   return;
    }

    if (IS_VAMPAFF(ch, VAM_ASSAMITE) && !IS_VAMPAFF(victim, VAM_ASSAMITE))
    {
	   ch->printlnf( "You can only grant your favor to someone in your bloodline.");
	   return;
    }
    if (IS_VAMPAFF(ch, VAM_CAPPADOCIAN) && !IS_VAMPAFF(victim, VAM_CAPPADOCIAN))
    {
	   ch->printlnf( "You can only grant your favor to someone in your bloodline.");
	   return;
    }
    if (IS_VAMPAFF(ch, VAM_GANGREL) && !IS_VAMPAFF(victim, VAM_GANGREL))
    {
	   ch->printlnf( "You can only grant your favor to someone in your bloodline.");
	   return;
    }
    if (IS_VAMPAFF(ch, VAM_MALKAVIAN) && !IS_VAMPAFF(victim, VAM_MALKAVIAN))
    {
	   ch->printlnf( "You can only grant your favor to someone in your bloodline.");
	   return;
    }
    if (IS_VAMPAFF(ch, VAM_NOSFERATU) && !IS_VAMPAFF(victim, VAM_NOSFERATU))
    {
	   ch->printlnf( "You can only grant your favor to someone in your bloodline.");
	   return;
    }
    if (IS_VAMPAFF(ch, VAM_TOREADOR) && !IS_VAMPAFF(victim, VAM_TOREADOR))
    {
	   ch->printlnf( "You can only grant your favor to someone in your bloodline.");
	   return;
    }
    if (IS_VAMPAFF(ch, VAM_TREMERE) && !IS_VAMPAFF(victim, VAM_TREMERE))
    {
	   ch->printlnf( "You can only grant your favor to someone in your bloodline.");
	   return;
    }
    
    if ( ch->vampgen >= victim->vampgen )
    {
	   ch->printlnf( "You can only grant your favor to someone of a lower generation.");
	   return;
    }
  }  
    if ( victim->vampgen > 12 )
    {
	   ch->printlnf( "There is no point in doing that, as they are unable to sire any childer.");
	   return;
    }
    
    if ( !str_cmp(arg2,"prince") && ( ch->vampgen == 1 || IS_IMMORTAL(ch)) )
    {
	   if (IS_VAMPAFF(victim, VAM_PRINCE))
      {
		  act("You remove $N's prince privilages!",ch,NULL,victim,TO_CHAR);
		  act("$n removes $N's prince privilages!",ch,NULL,victim,TO_NOTVICT);
		  act("$n removes your prince privilages!",ch,NULL,victim,TO_VICT);
		  REMOVE_BIT(victim->vampire,VAM_PRINCE);
        return;
      }
	   act("You make $N a prince!",ch,NULL,victim,TO_CHAR);
	   act("$n has made $N a prince!",ch,NULL,victim,TO_NOTVICT);
	   act("$n has made you a prince!",ch,NULL,victim,TO_VICT);
	   SET_BIT(victim->vampire, VAM_PRINCE);
	   return;
    }
    else if ( !str_cmp(arg2,"sire") && (ch->vampgen == 1 || IS_IMMORTAL(ch) || IS_VAMPAFF(ch, VAM_PRINCE)))
    {
	   if (IS_VAMPAFF(victim, VAM_CAN_SIRE)) 
      {
		  act("You remove $N's permission to sire a childe!",ch,NULL,victim,TO_CHAR);
		  act("$n has removed $N's permission to sire a childe!",ch,NULL,victim,TO_NOTVICT);
		  act("$n has remove your permission to sire a childe!",ch,NULL,victim,TO_VICT);
		  REMOVE_BIT(victim->vampire,VAM_CAN_SIRE);
		  return;
      }
	   act("You grant $N permission to sire a childe!",ch,NULL,victim,TO_CHAR);
	   act("$n has granted $N permission to sire a childe!",ch,NULL,victim,TO_NOTVICT);
	   act("$n has granted you permission to sire a childe!",ch,NULL,victim,TO_VICT);
	   SET_BIT(victim->vampire,VAM_CAN_SIRE);
	   return;
    }
    else 
	ch->printlnf( "You are unable to grant that sort of favor.");
    return;
}
/***************************************************************************/
void do_fangs( char_data *ch, char *argument )
{
    if (IS_NPC(ch)) 
	 	return;

    if ( !IS_VAMPIRE(ch))
    {
	   do_huh(ch,"");
	   return;
    }

    if (IS_VAMPAFF(ch,VAM_FANGS) )
    {
	ch->printlnf("Your fangs slide back into your gums.");
 	act("$n's fangs slide back into $s gums.",ch, NULL, NULL, TO_ROOM);
 	REMOVE_BIT(ch->vampire, VAM_FANGS);
	return;
    }
    ch->printlnf("Your fangs extend out of your gums.");
    act("A pair of sharp fangs extend from $n's mouth.",ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->vampire, VAM_FANGS);
    return;
}

void do_claws( char_data *ch, char *argument )
{
    if (IS_NPC(ch)) 
	 	return;

    if ( !IS_VAMPIRE(ch))
    {
	   do_huh(ch,"");
	   return;
    }

    if (!IS_VAMPAFF(ch,VAM_PROTEAN) && !IS_IMMORTAL(ch))
    {
	   ch->printlnf("You are not trained in the Protean discipline.");
	   return;
    }
    
    if (IS_VAMPAFF(ch,VAM_CLAWS) )
    {
	ch->printlnf("Your claws slide back under your nails.");
 	act("$n's claws slide back under $s nails.",ch, NULL, NULL, TO_ROOM);
	REMOVE_BIT(ch->vampire, VAM_CLAWS);
	return;
    }
    
    if ( ch->bloodpool < 10 )
    {
	   ch->printlnf("You have insufficient blood.");
	   return;
    }
    ch->bloodpool -= number_range(5,10);
    ch->printlnf("Sharp claws extend from under your finger nails.");
    act("Sharp claws extend from under $n's finger nails.",ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->vampire, VAM_CLAWS);
    return;
}
/***************************************************************************/
void do_poison( char_data *ch, char *argument )
{
    OBJ_DATA  *obj;
    AFFECT_DATA af;

    if (IS_NPC(ch)) 
	 	return;

    if ( !IS_VAMPIRE(ch))
    {
	   do_huh(ch,"");
	   return;
    }
        
    if (!IS_VAMPAFF(ch,VAM_SERPENTIS) && !IS_IMMORTAL(ch))
    {
	   ch->printlnf("You are not trained in the Serpentis discipline.");
	   return;
    }

	/* find out what */
	if (IS_NULLSTR(argument))
	{
		ch->println("Poison what item?");
		return;
	}
    
	obj =  get_obj_list(ch,argument,ch->carrying);

	if (obj== NULL)
	{
		ch->println("You don't have that item.");
		return;
	}
    
    if ( ch->bloodpool < 15 )
    {
	   ch->printlnf("You have insufficient blood.");
	   return;
    }

	if ((obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON)
	&& obj->value[3] == 0)
	{
			SET_BIT(obj->value[3], FOOD_POISONED);
    		ch->bloodpool -= number_range(5,15);
    		act("You run your tongue around $p, poisoning it.",ch,obj,NULL,TO_CHAR);
    		act("$n runs $s tongue around $p, poisoning it.",ch,obj,NULL,TO_ROOM);
		return;
	}

	if ( obj->item_type == ITEM_WEAPON )
	{
		if (IS_WEAPON_STAT( obj,WEAPON_FLAMING	)
		||  IS_WEAPON_STAT( obj,WEAPON_FROST	)
		||  IS_WEAPON_STAT( obj,WEAPON_VAMPIRIC	)
		||  IS_WEAPON_STAT( obj,WEAPON_SHOCKING	)
		||  IS_WEAPON_STAT( obj,WEAPON_HOLY		))
		{
			act("You can't seem to poison $p.",ch,obj,NULL,TO_CHAR);
			return;
		}

		if (obj->value[3] < 0 
		||  attack_table[obj->value[3]].damage == DAM_BASH)
		{
			ch->println("You can only poison edged weapons.");
			return;
		}

		if (IS_WEAPON_STAT(obj,WEAPON_POISON))
		{
			act("$p is already dripping with poison.",ch,obj,NULL,TO_CHAR);
			return;
		}

		af.where     = WHERE_WEAPON;
		af.type      = gsn_poison;
		af.level     = ch->level;
		af.duration  = ch->level / 4;
		af.location  = APPLY_NONE;
		af.modifier  = 0;
		af.bitvector = WEAPON_POISON;
		affect_to_obj(obj,&af);
   		ch->bloodpool -= number_range(5,15);
   		act("You run your tongue along $p, poisoning it.",ch,obj,NULL,TO_CHAR);
		act("$n runs $s tongue along $p, poisoning it.",ch,obj,NULL,TO_ROOM);
		return;
	}

	act("You can't poison $p.",ch,obj,NULL,TO_CHAR);
   	return;
}
/***************************************************************************/
void do_nightsight( char_data *ch, char *argument )
{
    if (IS_NPC(ch)) 
	 	return;

    if ( !IS_VAMPIRE(ch))
    {
	   do_huh(ch,"");
	   return;
    }

    if (!IS_VAMPAFF(ch,VAM_PROTEAN) && !IS_VAMPAFF(ch,VAM_OBTENEBRATION)
	   && !IS_VAMPAFF(ch,VAM_SERPENTIS))
    {
	   ch->printlnf("You are not trained in the correct disciplines.");
	   return;
    }
    if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT) )
    {
	   ch->printlnf("The red glow in your eyes fades.");
	   act("The red glow in $n's eyes fades.", ch, NULL, NULL, TO_ROOM);
	   REMOVE_BIT(ch->vampire, VAM_NIGHTSIGHT);
	   return;
    }
    if ( ch->bloodpool < 10 )
    {
	   ch->printlnf("You have insufficient blood.");
	   return;
    }
    ch->bloodpool -= number_range(5,10);
    ch->printlnf("Your eyes start glowing red.");
    act("$n's eyes start glowing red.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->vampire, VAM_NIGHTSIGHT);
    return;
}
/***************************************************************************/
void do_shadowsight( char_data *ch, char *argument )
{
    if (IS_NPC(ch)) 
	 	return;

    if ( !IS_VAMPIRE(ch))
    {
	   do_huh(ch,"");
	   return;
    }
    if (!IS_VAMPAFF(ch,VAM_OBTENEBRATION) )
    {
	   ch->printlnf("You are not trained in the Obtenebration discipline.");
	   return;
    }
    if (IS_AFFECTED2(ch, AFF2_SHADOWSIGHT))
    {
	   ch->printlnf("You can no longer see between planes.");
	   REMOVE_BIT(ch->affected_by2, AFF2_SHADOWSIGHT);
	   return;
    }
    if ( ch->bloodpool < 10 )
    {
	   ch->printlnf("You have insufficient blood.");
	   return;
    }
    ch->bloodpool -= number_range(5,10);
    ch->printlnf("You can now see into the shadows.");
    SET_BIT(ch->affected_by2, AFF2_SHADOWSIGHT);
    return;
}
/***************************************************************************/
void do_shadowplane( char_data *ch, char *argument )
{
    if (IS_NPC(ch)) 
	 return;

    if ( !IS_VAMPIRE(ch))
    {
	   do_huh(ch,"");
	   return;
    }
    if (!IS_VAMPAFF(ch, VAM_OBTENEBRATION) )
    {
	   ch->printlnf("You are not trained in the Obtenebration discipline.");
	   return;
    }


	if (!IS_AFFECTED2(ch, AFF2_SHADOWPLANE))
	{

    		if ( ch->bloodpool < 30 )
    		{
	   		ch->printlnf("You have insufficient blood.");
	   		return;
    		}

    	  ch->bloodpool -= number_range(15,30);
	  ch->printlnf("You fade into the plane of shadows.");
	  act("The shadows flicker and swallow up $n.",ch,NULL,NULL,TO_ROOM);
	  SET_BIT(ch->affected_by2, AFF2_SHADOWPLANE);
	  SET_BIT(ch->affected_by2, AFF2_SHADOWSIGHT);
	  do_look(ch,"auto");
	  return;
	}

	REMOVE_BIT(ch->affected_by2, AFF2_SHADOWPLANE);
	REMOVE_BIT(ch->affected_by2, AFF2_SHADOWSIGHT);
	ch->printlnf("You fade back into the real world.");
	act("The shadows flicker and $n fades into existance.",ch,NULL,NULL,TO_ROOM);
	do_look(ch,"auto");
	return;
}
/***************************************************************************/
void do_darkheart( char_data *ch, char *argument )
{
    if (IS_NPC(ch)) 
	 	return;

    if ( !IS_VAMPIRE(ch))
    {
	   do_huh(ch,"");
	   return;
    }
    if (!IS_VAMPAFF(ch,VAM_SERPENTIS) )
    {
	   ch->printlnf("You are not trained in the Serpentis discipline.");
	   return;
    }
    if (IS_VAMPAFF(ch,VAM_IMM_STAKE) )
    {
	   ch->printlnf("But you've already torn your heart out!");
	   return;
    }
    if ( ch->bloodpool < 100 )
    {
	   ch->printlnf("You have insufficient blood.");
	   return;
    }
    ch->bloodpool -= 100;
    ch->printlnf("You rip your heart from your body and toss it to the ground.");
    act("$n rips $s heart out and tosses it to the ground.", ch, NULL, NULL, TO_ROOM);

    char buf[MSL];
    OBJ_DATA *obj;
    char *name;

    name            = ch->short_descr;
    obj             = create_object( get_obj_index( OBJ_VNUM_TORN_HEART ));
    obj->timer      = number_range( 4, 7 );
		
    sprintf( buf, obj->short_descr, name );
    free_string( obj->short_descr );
    obj->short_descr = str_dup( buf );

    sprintf( buf, obj->description, name );
    free_string( obj->description );
    obj->description = str_dup( buf );
		
    if (obj->item_type == ITEM_FOOD)
    {
	if (IS_SET(ch->form,FORM_POISON))
		obj->value[3] = 1;
		else if (!IS_SET(ch->form,FORM_EDIBLE))
		obj->item_type = ITEM_TRASH;
    }

    if(IS_NPC(ch) && IS_SET(ch->affected_by2, AFF2_SHADOWPLANE))
   	SET_BIT(obj->extra2_flags, OBJEXTRA2_SHADOWPLANE);
    
    obj_to_room( obj, ch->in_room );

    ch->hit = ch->hit - number_range(20,60);
    update_pos(ch);
    if (ch->position == POS_DEAD)
    {
	   ch->printlnf( "You have been KILLED!!");
	   raw_kill(ch, ch);
	   return;
    }
    SET_BIT(ch->vampire, VAM_IMM_STAKE);
    return;
}
/***************************************************************************/
void do_mortal( char_data *ch, char *argument )
{
    if (IS_NPC(ch)) 
	 	return;

    if ( !IS_VAMPIRE(ch) && !IS_VAMPAFF(ch,VAM_HUMAN))
    {
	   do_huh(ch,"");
	   return;
    }
    
    if (!IS_VAMPAFF(ch,VAM_OBFUSCATE) )
    {
	   ch->printlnf("You are not trained in the Obfuscate discipline.");
	   return;
    }

    if (IS_VAMPIRE(ch) )
    {
	   if ( ch->bloodpool < 70 )
	   {
		  ch->printlnf("You don't have enough blood to assume mortal form.");
		  return;
	   }
	   /* Have to make sure they have enough blood to change back */
	   
	   /* Remove physical vampire attributes when you take mortal form */
	   if (IS_VAMPAFF(ch,VAM_DISGUISED) ) do_mask(ch,ch->name);
	   if (IS_AFFECTED2(ch,AFF2_SHADOWPLANE) ) do_shadowplane(ch,"");
	   if (IS_VAMPAFF(ch,VAM_FANGS) ) do_fangs(ch,"");
	   if (IS_VAMPAFF(ch,VAM_CLAWS) ) do_claws(ch,"");
	   if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT) ) do_nightsight(ch,"");
	   if (IS_AFFECTED2(ch,AFF2_SHADOWSIGHT) ) do_shadowsight(ch,"");
	   
	   ch->printlnf("Color returns to your skin and you warm up a little.");
	   act("Color returns to $n's skin.", ch, NULL, NULL, TO_ROOM);
	   
	   REMOVE_BIT(ch->affected_by2, AFF2_VAMP_BITE);
	   SET_BIT(ch->vampire, VAM_HUMAN);
	   return;
    }
    ch->printlnf("You skin pales and cools. You hunger for blood.");
    act("$n's skin pales slightly.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->affected_by2, AFF2_VAMP_BITE);
    REMOVE_BIT(ch->vampire, VAM_HUMAN);
    ch->bloodpool = 1;
    return;
}
/***************************************************************************/
void do_truesight( char_data *ch, char *argument )
{
    if (IS_NPC(ch)) 
	 	return;

    if ( !IS_VAMPIRE(ch))
    {
	   do_huh(ch,"");
	   return;
    }
    if (!IS_VAMPAFF(ch,VAM_AUSPEX) )
    {
	   ch->printlnf("You are not trained in the Auspex discipline.");
	   return;
    }
   
    if (IS_VAMPAFF(ch, VAM_TRUESIGHT) )
    {
	   REMOVE_BIT(ch->affected_by, AFF_DETECT_INVIS);
	   REMOVE_BIT(ch->affected_by, AFF_DETECT_MAGIC);
	   REMOVE_BIT(ch->affected_by, AFF_DETECT_HIDDEN);
	   ch->printlnf( "Your senses return to normal." );
	   return;
    }

    if ( ch->bloodpool < 10 )
    {
 	ch->printlnf("You don't have enough blood to use truesight.");
	return;
    }

   ch->bloodpool -= number_range(2,10);
   SET_BIT(ch->affected_by, AFF_DETECT_INVIS);
   SET_BIT(ch->affected_by, AFF_DETECT_MAGIC);
   SET_BIT(ch->affected_by, AFF_DETECT_HIDDEN);
   ch->printlnf( "Your senses increase to incredable proportions.");
   return;
}
/***************************************************************************/
void do_scry( char_data *ch, char *argument )
{
    char	buf[MSL];
    int		chance, random;
    char_data	*victim;
    char      	arg [MIL];
    
    argument = one_argument( argument, arg);
    
    if (IS_NPC(ch)) 
	 	return;

    if ( !IS_VAMPIRE(ch))
    {
	   do_huh(ch,"");
	   return;
    }
    
    if (!IS_VAMPAFF(ch,VAM_AUSPEX) )
    {
	   ch->printlnf("You are not trained in the Auspex discipline.");
	   return;
    }
    
    if ( arg[0] == '\0' )
    {
	   ch->printlnf( "Scry on whom?" );
	   return;
    }

	if (( victim = get_char_world( ch, arg )) == NULL
	 ||   victim == ch
	 ||   victim->in_room == NULL
	 ||   IS_OOC(victim)
	 ||   IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
	 ||   IS_SET(victim->in_room->room_flags, ROOM_ANTIMAGIC )
	 ||   IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
	 ||   IS_SET(victim->in_room->room_flags, ROOM_NOSCRY)
	 ||   IS_SET(victim->in_room->area->area_flags, AREA_NOSCRY)
	 ||   (!IS_NPC(victim) && victim->level >= LEVEL_IMMORTAL )) { // NOT trust
		ch->println("You failed.");
		return;
	}

    if ( ch->bloodpool < 25 )
    {
	   ch->printlnf("You have insufficient blood.");
	   return;
    }
    ch->bloodpool -= number_range(15,25);

	if(	IS_SET(victim->imm_flags, IMM_SCRY)	 
		|| (IS_SET(victim->res_flags, RES_SCRY) && number_range(1,3)!=1) )
	{
		ch->println("You failed.");
		return;
	}

	if ( is_affected( victim, gsn_detect_scry )) {
		chance = 50 - (( ch->level - victim->level ) * 3 );
		random = number_range( 1, 100);
		if ( chance >= random )
			victim->println("You feel someone watching you.");
		if (( chance /= 5 ) >= random )
			act( "$n is observing you.", ch, NULL, victim, TO_VICT );
	}

	sprintf(buf, "'%s' look", victim->name);
	do_at(ch, buf);

	return;
}
/***************************************************************************/
void do_journey( char_data *ch, char *argument )
{
    if (IS_NPC(ch)) 
	 	return;

    if ( !IS_VAMPIRE(ch))
    {
	   do_huh(ch,"");
	   return;
    }
    
    if (!IS_VAMPAFF(ch,VAM_AUSPEX) )
    {
	   ch->printlnf("You are not trained in the Auspex discipline.");
	   return;
    }
    
	ROOM_INDEX_DATA *pRoomIndex;

	if ( ch->in_room == NULL
	||   IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
	||   ch->fighting != NULL )
	{
		ch->println( "You failed." );
		return;
	}

    	if ( ch->bloodpool < 30 )
    	{
	   ch->printlnf("You have insufficient blood.");
	   return;
    	}
    	ch->bloodpool -= number_range(10,30);

	pRoomIndex = get_random_room(ch);

	act( "$n vanishes!", ch, NULL, NULL, TO_ROOM );
	char_from_room( ch );
	char_to_room( ch, pRoomIndex );
	act( "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
	do_look( ch, "auto" );

	/* 
	 * If someone is following the char, these triggers get activated
	 * for the followers before the char, but it's safer this way...
	 */
	if ( IS_NPC( ch)
	&& HAS_TRIGGER( ch, TRIG_ENTRY ))
		mp_percent_trigger( ch, NULL, NULL, NULL, TRIG_ENTRY );

	if ( !IS_NPC( ch ))
		mp_greet_trigger( ch );

	return;
}
/***************************************************************************/
void do_makedoll( char_data *ch, char *argument )
{
    char_data *victim;
    OBJ_DATA  *obj;
    char       buf [MIL];
    char       arg [MIL];
    char     part1 [MIL];
    char     part2 [MIL];
    int       worn;

    argument = one_argument( argument, arg );
	
    if ( IS_NPC(ch) )  
		return;

    if ( arg[0] == '\0')
    {
	   ch->printlnf("Who do you wish to make a voodoo doll of ?" );
	   return;
    }
    
    if (ch->gold < 50)
    {
	   ch->printlnf( "It costs 50 gold to create a voodoo doll.");
	   return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
		ch->println( "They aren't here.");
		return;
    }

	if ( IS_NPC(victim) )
	{
		ch->println( "Not on NPC's.");
		return;
	}

    
   if ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
   {
	  ch->printlnf( "You are not holding any body parts." );
	  return;
   }

   worn = WEAR_HOLD;

    
    if ( IS_NPC(victim) )
    {
	   ch->printlnf( "Not on NPC's.");
	   return;
    }

    if( !IS_ACTIVE(victim))
    {
	   ch->printlnf("They are not ACTIVE." );
	   return;
    }
   
    if      (obj->pIndexData->vnum == OBJ_VNUM_SEVERED_HEAD) sprintf(part1, "head %s",victim->name);
    else if (obj->pIndexData->vnum == OBJ_VNUM_TORN_HEART) sprintf(part1, "heart %s",victim->name);
    else if (obj->pIndexData->vnum == OBJ_VNUM_SLICED_ARM) sprintf(part1, "arm %s",victim->name);
    else if (obj->pIndexData->vnum == OBJ_VNUM_SLICED_LEG) sprintf(part1, "leg %s",victim->name);
    else if (obj->pIndexData->vnum == OBJ_VNUM_GUTS) sprintf(part1, "entrails %s",victim->name);
    else if (obj->pIndexData->vnum == OBJ_VNUM_BRAINS) sprintf(part1, "brain %s",victim->name);
    else
    {
	   sprintf(buf,  "%s isn't a part of %s!",obj->name,victim->name);
	   ch->printlnf(buf);
	   return;
    }
    
    sprintf(part2, obj->material);
    
    if ( str_cmp(victim->name,part2) )
    {
	   sprintf(buf, "That body part doesn't belong to %s!",victim->name);
	   ch->printlnf(buf);
	   return;
    }
    
    act("$p vanishes from your hand in a puff of smoke.", ch, obj, NULL, TO_CHAR);
    act("$p vanishes from $n's hand in a puff of smoke.", ch, obj, NULL, TO_ROOM);
    obj_from_char(obj);
    extract_obj(obj);
    
    obj = create_object( get_obj_index( 2005 ));
    
    sprintf(buf,  "%s voodoo doll",victim->name);
    free_string(obj->name);
    obj->name=str_dup(buf);
    
    sprintf(buf,  "a voodoo doll of %s",victim->name);
    free_string(obj->short_descr);
    obj->short_descr=str_dup(buf);
    
    sprintf(buf,  "A voodoo doll of %s lies here.",victim->name);
    free_string(obj->description);
    obj->description=str_dup(buf);
    obj->material = victim->name;

    obj_to_char(obj,ch);
    equip_char(ch,obj,worn);
    
    act("$p appears in your hand.", ch, obj, NULL, TO_CHAR);
    act("$p appears in $n's hand.", ch, obj, NULL, TO_ROOM);
    
    ch->gold -= 50;
    return;
}
/***************************************************************************/
void do_voodoo( char_data *ch, char *argument )
{
    char_data *victim;
    OBJ_DATA  *obj;
    char      arg1 [MIL];
    char      arg2 [MIL];
    char       buf [MIL];
    char     part1 [MIL];
    char     part2 [MIL];
    int dam;
    
    argument = one_argument( argument, arg1);
    argument = one_argument( argument, arg2);
    
    if ( arg1[0] == '\0')
    {
	   ch->printlnf("Who do you wish to use voodoo magic on?" );
	   return;
    }
    
    if ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
    {
	   ch->printlnf("You are not holding a voodoo doll." );
	   return;
    }
    
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	   ch->printlnf("They are not here." );
	   return;
    }
    
    if ( IS_NPC(victim) )
    {
	   ch->printlnf("Not on NPC's." );
	   return;
    }

    if( !IS_ACTIVE(victim))
    {
	   ch->printlnf("They are not ACTIVE." );
	   return;
    }

    sprintf(part2,obj->name);
    sprintf(part1, "%s voodoo doll",victim->name);
    
    if ( str_cmp(part1,part2) )
    {
	   sprintf(buf, "But you are holding %s, not %s!\n\r",obj->short_descr,victim->name);
	   ch->printlnf(buf);
	   return;
    }

    victim = get_char_world( ch, arg1 );

    if ( arg2[0] == '\0')
    {
	   ch->printlnf("You can 'stab', 'rip' or 'throw' the doll." );
    }
    else if ( !str_cmp(arg2, "stab") )
    {
	   act("You stab a pin through $p.", ch, obj, NULL, TO_CHAR); 
	   act("$n stabs a pin through $p.", ch, obj, NULL, TO_ROOM);
	   victim->printlnf("You feel an agonising pain in your chest!");
	   act("$n clutches their chest in agony!", victim,obj,NULL,TO_ROOM);
	   return;
    }
    else if ( !str_cmp(arg2, "rip") )
    {
	   act("You rip the head off of $p.", ch, obj, NULL, TO_CHAR);
	   act("$n rips the head off of $p.", ch, obj, NULL, TO_ROOM);
	   act("$p turns to dust.", ch, obj, NULL, TO_CHAR);
	   act("$p turns to dust.", ch, obj, NULL, TO_ROOM);
	   extract_obj(obj);

	AFFECT_DATA af;
	af.where		= WHERE_AFFECTS;
	af.type			= gsn_cause_headache;
	af.level		= ch->level;
	af.duration		= 10;
	af.location		= APPLY_SD;
	af.modifier		= - ch->level/5;
	af.bitvector	= 0;
	affect_to_char( victim, &af );

	af.where     = WHERE_AFFECTS;
	af.type      = gsn_blindness;
	af.level     = ch->level;
	af.location  = APPLY_HITROLL;
	af.modifier  = -4;
	af.duration  = 10;
	af.bitvector = AFF_BLIND;
	affect_to_char( victim, &af );

	victim->printlnf("You suddenly feel an excrutiating pain in your head!");
	act("$n grabs thier head and screams in excrutiating pain!", victim,obj,NULL,TO_ROOM);
	return;
    }
    else if ( !str_cmp(arg2, "throw") )
    {
	   act("You throw $p to the ground.", ch, obj, NULL, TO_CHAR);
	   act("$n throws $p to the ground.", ch, obj, NULL, TO_ROOM);
	   obj_from_char(obj);
	   obj_to_room(obj, ch->in_room);
	   if (victim->position == POS_FIGHTING)
		  stop_fighting(victim, true);
	   victim->printlnf("A strange force picks you up and hurls you to the ground!");
	   act("$n is hurled to the ground by a strange force.", victim,obj,NULL,TO_ROOM);
	   victim->position = POS_RESTING;

       dam = number_range(ch->level/4,ch->level/2);
       if ( dam < 0)
           dam = - dam;
	   victim->hit = victim->hit - dam;
	   update_pos(victim);
	   if (victim->position == POS_DEAD && !IS_NPC(victim))
	   {
		  raw_kill(ch,victim);
		  return;
	   }
    }
    else
    {
	   ch->printlnf("You can 'stab', 'rip' or 'throw' the doll." );
    }
    return;
}
/***************************************************************************/
void do_torpor( char_data *ch, char *argument )
{
    char arg[MIL];

    if ( !IS_VAMPIRE(ch))
    {
	   do_huh(ch,"");
	   return;
    }

	if (ch->position == POS_SLEEPING)
	{
		ch->println("You can't do that while sleeping.");
	        return;
	}

	if(IS_IMMORTAL(ch))
	{
		ch->println("Why would you want to do that ?");
	        return;
	}

	if ( ch->fighting ) 
	{
		ch->println("You seem to be too busy do anything.");
		return;
	}

	argument = one_argument(argument,arg);

    	if (arg[0] == '\0')
    	{
		ch->println("Type TORPOR CONFIRM to enter torpor.");
		return;
	}

	if(IS_NPC(ch) || ch->level!=LEVEL_HERO)
	{
		ch->println("You are not strong enough to enter torpor.");
		return;
	}

	if(ch->remort >= 7){
		ch->println("You are at maximum remort.");
		return;
	}


    if (!str_prefix(arg,"confirm"))
    {
	act("You dig a hole in the ground and enter torpor.", ch,NULL,ch,TO_CHAR);
	act( "$n digs a hole in the ground and enters torpor.", ch, NULL,ch, TO_ROOM );

	WAIT_STATE(ch,PULSE_VIOLENCE);

    	ch->exp   = exp_per_level(ch,ch->pcdata->points) * 20;
        ch->level    = 20;
        ch->max_hit  = 10;
        ch->max_mana = 100;
        ch->max_move = 100;
        ch->practice = 0;
        ch->hit      = ch->max_hit;
        ch->mana     = ch->max_mana;
        ch->move     = ch->max_move;
        advance_level( ch );
        ch->practice = 1;
	ch->train = 1;
	ch->gold = 0;
	ch->silver = 0;
	ch->bank = 0;
	ch->vampgen -= 1;
	if (ch->vampgen <= 1)
	    ch->vampgen = 2;
	ch->pcdata->rp_points = 0;

	ch->pcdata->condition[COND_THIRST]	= 48;
	ch->pcdata->condition[COND_FULL]	= 48;
	ch->pcdata->condition[COND_HUNGER]	= 48;
	ch->remort += 1;

	ch->pcdata->birthdate= current_time+ number_range(-ICTIME_IRLSECS_PER_YEAR,ICTIME_IRLSECS_PER_YEAR);
	ch->pcdata->birthyear_modifier=- (100 * ch->remort);
	ch->pcdata->birthyear_modifier++;
	ch->pcdata->birthdate-=ICTIME_IRLSECS_PER_YEAR;

	char subject[MIL];
	sprintf(subject,"%s has awoken from Torpor %d", ch->name, ch->remort);
	char text[MIL];
	sprintf(text,"%s has awoken from Torpor %d", ch->name, ch->remort);
	autonote(NOTE_INOTE, "remort system", subject, "imm", text, true);

	obj_data *obj;
	obj_data *obj_next;
  	for (obj = ch->carrying; obj != NULL; obj = obj_next)
    	{
	    obj_next = obj->next_content;
            unequip_char( ch, obj );
            obj_from_char( obj );
	    extract_obj(obj);
	}

	laston_update_char(ch);
    	save_char_obj(ch);
	char_from_room( ch );
	char_to_room(ch, get_room_index(1410) );
	ch->printlnf( "You dig yourself out of the ground, feeling weak but stronger with age." );
	act( "$n slowly crawls out from the ground.", ch, NULL, NULL, TO_ROOM );
	do_look( ch, "auto" );	
	return;
    }
}
/***************************************************************************/
void do_shapeshift( char_data *ch, char *argument )
{
    char_data *victim;
    char      arg [MIL];
    char      buf [MIL];
    
    argument = one_argument( argument, arg );
    
    if (IS_NPC(ch)) 
	 	return;

    if(IS_NULLSTR(ch->orig_short_descr))
    {
	ch->orig_short_descr = ch->short_descr;
	ch->orig_description = ch->description;
    }

    if (ch->race != race_lookup("changeling") && !IS_IMMORTAL(ch))
    {
	   do_huh(ch,"");
	   return;
    }
    
    if ( arg[0] == '\0' )
    {
	   ch->printlnf( "Shapeshift to look like whom?");
	   return;
    }
    
    
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	   ch->printlnf( "They aren't here." );
	   return;
    }
    
    if( victim->level > ch->level && !IS_IMMORTAL(ch) )
    {
	   ch->printlnf("They are too powerful to change into.");
	   return;
    }
    
    if ( IS_IMMORTAL(victim) && !IS_IMMORTAL(ch) )
    {
	   ch->printlnf( "You wish." );
	   return;
    }

 /*   if ( !IS_NPC(victim) && ch!=victim)
    {
	   ch->printlnf( "You wish." );
	   return;
    } */

    if( IS_IMMORTAL(victim) && victim->level > ch->level && IS_IMMORTAL(ch) )
    {
	   ch->printlnf("They are too powerful to change into.");
	   return;
    }
    
    if ( ch == victim )
    {
	   if (!IS_AFFECTED2(ch, AFF2_SHAPESHIFT))
	   {
		  ch->printlnf( "You already look like yourself!" );
		  return;
	   }
	   sprintf(buf, "Your form shimmers and shapeshifts into %s.",ch->name);
	   act(buf,ch,NULL,victim,TO_CHAR);
	   sprintf(buf, "%s's form shimmers and shapeshifts into %s.",ch->short_descr, ch->orig_short_descr );
	   act(buf,ch,NULL,victim,TO_ROOM);
	   REMOVE_BIT(ch->affected_by2, AFF2_SHAPESHIFT);

   	ch->short_descr = str_dup(ch->orig_short_descr); 
   	ch->description = str_dup(ch->orig_description);
	   return;
    }

	if ( ch->move < 30 )
    	{
	   	ch->printlnf("You are too weak to shapechange now.");
	   	return;
    	}
	ch->move -= number_range(15,30);

    if (IS_AFFECTED2(ch, AFF2_SHAPESHIFT))
    {
    	sprintf(buf, "Your form shimmers and shapeshifts into a clone of %s.",victim->short_descr);
    	act(buf,ch,NULL,victim,TO_CHAR);
    	sprintf(buf, "%s's form shimmers and shapeshifts into a clone of %s.", ch->short_descr, victim->short_descr);
    	act(buf,ch,NULL,victim,TO_NOTVICT);
    	sprintf(buf, "%s's form shimmers and shapeshifts into a clone of you!",ch->short_descr);
    	act(buf,ch,NULL,victim,TO_VICT);
    	SET_BIT(ch->affected_by2, AFF2_SHAPESHIFT);

   	ch->short_descr = str_dup(victim->short_descr); 
   	ch->description = str_dup(victim->description);

	return;
    }

    sprintf(buf, "Your form shimmers and shapeshifts into a clone of %s.",victim->short_descr);
    act(buf,ch,NULL,victim,TO_CHAR);
    sprintf(buf, "$n's form shimmers and shapeshifts into a clone of %s.", victim->short_descr);
    act(buf,ch,NULL,victim,TO_NOTVICT);
    sprintf(buf, "%s's form shimmers and shapeshifts into a clone of you!",PERS(ch, victim));
    act(buf,ch,NULL,victim,TO_VICT);
    SET_BIT(ch->affected_by2, AFF2_SHAPESHIFT);

   	ch->orig_short_descr = str_dup(ch->short_descr); 
   	ch->short_descr = str_dup(victim->short_descr); 
   	ch->orig_description = str_dup(ch->description);
   	ch->description = str_dup(victim->description);

    return;
}
/**************************************************************************/
void do_fealty( char_data *ch, char *argument )
{
    char_data *victim;
    char      arg [MIL];
    int rank = 0;
    int vrank = 0;
    
    argument = one_argument( argument, arg );
    
    if (IS_NPC(ch)) 
	 return;

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	   ch->printlnf( "They aren't here." );
	   return;
    }

    if(!victim->title)
    {
	ch->printlnf("You cannot swear fealty to them.");
	return;
    }    

    if(ch->title)
    {
	if(ch->title==TITLE_KING || ch->title==TITLE_QUEEN)
		rank = 10;
	if(ch->title==TITLE_PRINCE || ch->title==TITLE_PRINCESS)
		rank = 9;
	if(ch->title==TITLE_DUKE || ch->title==TITLE_DUCHESS)
		rank = 8;
	if(ch->title==TITLE_COUNT || ch->title==TITLE_COUNTESS)
		rank = 7;
	if(ch->title==TITLE_MARQUIS || ch->title==TITLE_MARQUESTA)
		rank = 6;
	if(ch->title==TITLE_BARON || ch->title==TITLE_BARONESS)
		rank = 5;
	if(ch->title==TITLE_LORD || ch->title==TITLE_LADY)
		rank = 4;
	if(ch->title==TITLE_MASTER || ch->title==TITLE_MISTRESS)
		rank = 4;
	if(ch->title==TITLE_SIR || ch->title==TITLE_DAME)
		rank = 4;
	if(ch->title==TITLE_PRIEST || ch->title==TITLE_PRIESTESS || ch->title==TITLE_ARCHMAGE)
		rank = 3;
    }

    if(victim->title)
    {
	if(victim->title==TITLE_KING || victim->title==TITLE_QUEEN)
		vrank = 10;
	if(victim->title==TITLE_PRINCE || victim->title==TITLE_PRINCESS)
		vrank = 9;
	if(victim->title==TITLE_DUKE || victim->title==TITLE_DUCHESS)
		vrank = 8;
	if(victim->title==TITLE_COUNT || victim->title==TITLE_COUNTESS)
		vrank = 7;
	if(victim->title==TITLE_MARQUIS || victim->title==TITLE_MARQUESTA)
		vrank = 6;
	if(victim->title==TITLE_BARON || victim->title==TITLE_BARONESS)
		vrank = 5;
	if(victim->title==TITLE_LORD || victim->title==TITLE_LADY)
		vrank = 4;
	if(victim->title==TITLE_SIR || victim->title==TITLE_DAME)
		vrank = 4;
	if(victim->title==TITLE_MASTER || victim->title==TITLE_MISTRESS)
		vrank = 4;
	if(victim->title==TITLE_PRIEST || victim->title==TITLE_PRIESTESS || victim->title==TITLE_ARCHMAGE)
		vrank = 3;
    }

    if(vrank<=rank)
    {
	ch->printlnf("They should be swearing fealty to you!");
	return;
    }

    ch->printlnf("You swear fealty and allegiance to %s %s.",capitalize( flag_string(title_flags, victim->title)), victim->name);
    if(ch->title)
    	victim->printlnf("%s %s swears fealty and allegiance to you.", capitalize( flag_string(title_flags, ch->title)), ch->name);
    else
    	victim->printlnf("%s swears fealty and allegiance to you.", ch->name);
    ch->fealty = victim->name;
    return;
}
/**************************************************************************/
void do_knight( char_data *ch, char *argument )
{
    char_data *victim;
    char      arg [MIL];
    int rank = 0;
    int vrank = 0;
    
    argument = one_argument( argument, arg );
    
    if (IS_NPC(ch)) 
	 return;

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	   ch->printlnf( "They aren't here." );
	   return;
    }

    if(ch->title)
    {
	if(ch->title==TITLE_KING || ch->title==TITLE_QUEEN)
		rank = 10;
	if(ch->title==TITLE_PRINCE || ch->title==TITLE_PRINCESS)
		rank = 9;
	if(ch->title==TITLE_DUKE || ch->title==TITLE_DUCHESS)
		rank = 8;
	if(ch->title==TITLE_COUNT || ch->title==TITLE_COUNTESS)
		rank = 7;
	if(ch->title==TITLE_MARQUIS || ch->title==TITLE_MARQUESTA)
		rank = 6;
	if(ch->title==TITLE_BARON || ch->title==TITLE_BARONESS)
		rank = 5;
	if(ch->title==TITLE_LORD || ch->title==TITLE_LADY)
		rank = 4;
	if(ch->title==TITLE_MASTER || ch->title==TITLE_MISTRESS)
		rank = 4;
	if(ch->title==TITLE_SIR || ch->title==TITLE_DAME)
		rank = 4;
	if(ch->title==TITLE_PRIEST || ch->title==TITLE_PRIESTESS || ch->title==TITLE_ARCHMAGE)
		rank = 3;
    }

    if(rank<6)
    {
	   ch->printlnf( "You cannot knight anyone." );
	   return;
    }

    if(victim->title)
    {
	if(victim->title==TITLE_KING || victim->title==TITLE_QUEEN)
		vrank = 10;
	if(victim->title==TITLE_PRINCE || victim->title==TITLE_PRINCESS)
		vrank = 9;
	if(victim->title==TITLE_DUKE || victim->title==TITLE_DUCHESS)
		vrank = 8;
	if(victim->title==TITLE_COUNT || victim->title==TITLE_COUNTESS)
		vrank = 7;
	if(victim->title==TITLE_MARQUIS || victim->title==TITLE_MARQUESTA)
		vrank = 6;
	if(victim->title==TITLE_BARON || victim->title==TITLE_BARONESS)
		vrank = 5;
	if(victim->title==TITLE_LORD || victim->title==TITLE_LADY)
		vrank = 4;
	if(victim->title==TITLE_MASTER || victim->title==TITLE_MISTRESS)
		vrank = 4;
	if(victim->title==TITLE_SIR || victim->title==TITLE_DAME)
		vrank = 4;
	if(victim->title==TITLE_PRIEST || victim->title==TITLE_PRIESTESS || victim->title==TITLE_ARCHMAGE)
		vrank = 3;
    }

    if(vrank>=4)
    {
	ch->printlnf("They cannot be knighted!");
	return;
    }

    if(victim->fealty != ch->name)
    {
	ch->printlnf("They have not sworn fealty to you yet.");
	return;
    }


    if (victim->clss == class_lookup("warrior"))
    {
   	ch->printlnf("You draw your weapon and knight %s.", victim->name);
  	victim->printlnf("%s %s has made you a knight of their realm.", capitalize( flag_string(title_flags, ch->title)), ch->name);
	if(victim->sex == 2)
	{	
    		victim->title = TITLE_DAME;
	}else
    		victim->title = TITLE_SIR;
	return;
    }

    if(victim->clss != class_lookup("paladin"))
    {
   	ch->printlnf("You draw your weapon and knight %s.", victim->name);
  	victim->printlnf("%s %s has made you a knight of their realm.", capitalize( flag_string(title_flags, ch->title)), ch->name);
	if(victim->sex == 2)
	{	
    		victim->title = TITLE_DAME;
	}else
    		victim->title = TITLE_SIR;
	return;
    }
    if(victim->clss != class_lookup("ranger")) 
    {
   	ch->printlnf("You draw your weapon and knight %s.", victim->name);
  	victim->printlnf("%s %s has made you a knight of their realm.", capitalize( flag_string(title_flags, ch->title)), ch->name);
	if(victim->sex == 2)
	{	
    		victim->title = TITLE_DAME;
	}else
    		victim->title = TITLE_SIR;
	return;
    }
    if(victim->clss != class_lookup("barbarian"))
    {
   	ch->printlnf("You draw your weapon and knight %s.", victim->name);
  	victim->printlnf("%s %s has made you a knight of their realm.", capitalize( flag_string(title_flags, ch->title)), ch->name);
	if(victim->sex == 2)
	{	
    		victim->title = TITLE_DAME;
	}else
    		victim->title = TITLE_SIR;
	return;
    }
    if(victim->clss != class_lookup("anti-paladin"))
    {
   	ch->printlnf("You draw your weapon and knight %s.", victim->name);
  	victim->printlnf("%s %s has made you a knight of their realm.", capitalize( flag_string(title_flags, ch->title)), ch->name);
	if(victim->sex == 2)
	{	
    		victim->title = TITLE_DAME;
	}else
    		victim->title = TITLE_SIR;
	return;
    }

    ch->printlnf("They do not possess the qualities to be a knight.");
    return;

}
/**************************************************************************/
void do_squire( char_data *ch, char *argument )
{
    char_data *victim;
    char      arg [MIL];
    int rank = 0;
    int vrank = 0;
    
    argument = one_argument( argument, arg );
    
    if (IS_NPC(ch)) 
	 return;

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	   ch->printlnf( "They aren't here." );
	   return;
    }

    if(!(ch->title==TITLE_KING)      || !(ch->title==TITLE_QUEEN)
	|| !(ch->title==TITLE_PRINCE)    || !(ch->title==TITLE_PRINCESS)
	|| !(ch->title==TITLE_MARQUESTA) || !(ch->title==TITLE_MARQUIS)
	|| !(ch->title==TITLE_DUKE)      || !(ch->title==TITLE_DUCHESS)
	|| !(ch->title==TITLE_BARON)     || !(ch->title==TITLE_BARONESS)
	|| !(ch->title==TITLE_LORD)      || !(ch->title==TITLE_LADY)
	|| !(ch->title==TITLE_SIR)       || !(ch->title==TITLE_DAME))
    {
	   ch->printlnf( "You cannot squire anyone." );
	   return;
    }

    rank = 4;

    if(victim->title)
    {
	if(victim->title==TITLE_KING || victim->title==TITLE_QUEEN)
		vrank = 10;
	if(victim->title==TITLE_PRINCE || victim->title==TITLE_PRINCESS)
		vrank = 9;
	if(victim->title==TITLE_DUKE || victim->title==TITLE_DUCHESS)
		vrank = 8;
	if(victim->title==TITLE_COUNT || victim->title==TITLE_COUNTESS)
		vrank = 7;
	if(victim->title==TITLE_MARQUIS || victim->title==TITLE_MARQUESTA)
		vrank = 6;
	if(victim->title==TITLE_BARON || victim->title==TITLE_BARONESS)
		vrank = 5;
	if(victim->title==TITLE_LORD || victim->title==TITLE_LADY)
		vrank = 4;
	if(victim->title==TITLE_MASTER || victim->title==TITLE_MISTRESS)
		vrank = 4;
	if(victim->title==TITLE_SIR || victim->title==TITLE_DAME)
		vrank = 4;
	if(victim->title==TITLE_PRIEST || victim->title==TITLE_PRIESTESS || victim->title==TITLE_ARCHMAGE)
		vrank = 5;
    }

    if(vrank>=4)
    {
	ch->printlnf("They cannot be squired!");
	return;
    }

    if(victim->fealty != ch->name)
    {
	ch->printlnf("They have not sworn fealty to you yet.");
	return;
    }


    if (victim->clss == class_lookup("warrior"))
    {
    	ch->printlnf("You draw your weapon and squire %s.", victim->name);
    	victim->printlnf("%s %s has made you their squire.", capitalize( flag_string(title_flags, ch->title)), ch->name);
   	victim->title = TITLE_SQUIRE;
    	return;
    }

    if(victim->clss != class_lookup("paladin"))
    {
    	ch->printlnf("You draw your weapon and squire %s.", victim->name);
    	victim->printlnf("%s %s has made you their squire.", capitalize( flag_string(title_flags, ch->title)), ch->name);
   	victim->title = TITLE_SQUIRE;
    	return;
    }
    if(victim->clss != class_lookup("ranger")) 
    {
    	ch->printlnf("You draw your weapon and squire %s.", victim->name);
    	victim->printlnf("%s %s has made you their squire.", capitalize( flag_string(title_flags, ch->title)), ch->name);
   	victim->title = TITLE_SQUIRE;
    	return;
    }
    if(victim->clss != class_lookup("barbarian"))
    {
    	ch->printlnf("You draw your weapon and squire %s.", victim->name);
    	victim->printlnf("%s %s has made you their squire.", capitalize( flag_string(title_flags, ch->title)), ch->name);
   	victim->title = TITLE_SQUIRE;
    	return;
    }
    if(victim->clss != class_lookup("anti-paladin"))
    {
    	ch->printlnf("You draw your weapon and squire %s.", victim->name);
    	victim->printlnf("%s %s has made you their squire.", capitalize( flag_string(title_flags, ch->title)), ch->name);
   	victim->title = TITLE_SQUIRE;
    	return;
    }

    ch->printlnf("They do not possess the qualities to be a knight.");
    return;

}
/***************************************************************************/
void do_entitle( char_data *ch, char *argument )
{
    char_data *victim;
    char      arg1 [MIL];
    char      arg2 [MIL];
    int rank = 0;
    int vrank = 0;
    
    argument = one_argument( argument, arg1);
    argument = one_argument( argument, arg2);
    
    if (IS_NPC(ch)) 
	 return;

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	   ch->printlnf( "They aren't here." );
	   return;
    }

    if(IS_NULLSTR(arg2))
    {
	   ch->printlnf( "Syntax: entitle <victim> <rank>" );
	   return;
    }

    if(ch->title)
    {
	if(ch->title==TITLE_KING || ch->title==TITLE_QUEEN)
		rank = 10;
	if(ch->title==TITLE_PRINCE || ch->title==TITLE_PRINCESS)
		rank = 9;
	if(ch->title==TITLE_DUKE || ch->title==TITLE_DUCHESS)
		rank = 8;
	if(ch->title==TITLE_COUNT || ch->title==TITLE_COUNTESS)
		rank = 7;
	if(ch->title==TITLE_MARQUIS || ch->title==TITLE_MARQUESTA)
		rank = 6;
	if(ch->title==TITLE_BARON || ch->title==TITLE_BARONESS)
		rank = 5;
	if(ch->title==TITLE_LORD || ch->title==TITLE_LADY)
		rank = 4;
	if(ch->title==TITLE_MASTER || ch->title==TITLE_MISTRESS)
		rank = 4;
	if(ch->title==TITLE_SIR || ch->title==TITLE_DAME)
		rank = 4;
	if(ch->title==TITLE_PRIEST || ch->title==TITLE_PRIESTESS || ch->title==TITLE_ARCHMAGE)
		rank = 3;
    }

    if(victim->title)
    {
	if(victim->title==TITLE_KING || victim->title==TITLE_QUEEN)
		vrank = 10;
	if(victim->title==TITLE_PRINCE || victim->title==TITLE_PRINCESS)
		vrank = 9;
	if(victim->title==TITLE_DUKE || victim->title==TITLE_DUCHESS)
		vrank = 8;
	if(victim->title==TITLE_COUNT || victim->title==TITLE_COUNTESS)
		vrank = 7;
	if(victim->title==TITLE_MARQUIS || victim->title==TITLE_MARQUESTA)
		vrank = 6;
	if(victim->title==TITLE_BARON || victim->title==TITLE_BARONESS)
		vrank = 5;
	if(victim->title==TITLE_LORD || victim->title==TITLE_LADY)
		vrank = 4;
	if(victim->title==TITLE_MASTER || victim->title==TITLE_MISTRESS)
		vrank = 4;
	if(victim->title==TITLE_SIR || victim->title==TITLE_DAME)
		vrank = 4;
	if(victim->title==TITLE_PRIEST || victim->title==TITLE_PRIESTESS || victim->title==TITLE_ARCHMAGE)
		vrank = 3;
    }

    if(rank<=4)
    {
	ch->printlnf("You cannot entitle anyone.");
	return;
    }

    if(rank<=vrank)
    {
	ch->printlnf("They outrank you!");
	return;
    }

    if (!str_prefix(arg2,"lord") || !str_prefix(arg2,"lady"))
    {
	if (rank>=6)
	{   
	    if(victim->sex == 2)
	    {
 	    	victim->title = TITLE_LADY;
	    } else victim->title = TITLE_LORD;

    		ch->printlnf("You award the title of %s to %s.",capitalize( flag_string(title_flags, victim->title)), victim->name);
    		victim->printlnf("%s awards you with the title of %s.", ch->name, capitalize( flag_string(title_flags, victim->title)));
		return;
	}
 	else {
	    ch->printlnf("You must be a Marquis/Marquesta to award that rank!");
	    return;
	}
    }

    if (!str_prefix(arg2,"master") || !str_prefix(arg2,"mistress"))
    {
	if (rank>=5)
	{   
	    if(victim->sex == 2)
	    {
 	    	victim->title = TITLE_MISTRESS;
	    } else victim->title = TITLE_MASTER;

    		ch->printlnf("You award the title of %s to %s.",capitalize( flag_string(title_flags, victim->title)), victim->name);
    		victim->printlnf("%s awards you with the title of %s.", ch->name, capitalize( flag_string(title_flags, victim->title)));
		return;
	}
 	else {
	    ch->printlnf("You must be a Baron/Baroness to award that rank!");
	    return;
	}
    }

    if (!str_prefix(arg2,"man-at-arms") )
    {
	if (rank>=4)
	{   
    		victim->title = TITLE_MAN_AT_ARMS;
    		ch->printlnf("You award the title of %s to %s.",capitalize( flag_string(title_flags, victim->title)), victim->name);
    		victim->printlnf("%s awards you with the title of %s.", ch->name, capitalize( flag_string(title_flags, victim->title)));
		return;
	}
 	else {
	    ch->printlnf("You must be a Lord/Lady to award that rank!");
	    return;
	}
    }

   ch->printlnf( "Syntax: entitle <victim> <rank>" );
   return;
}
/***************************************************************************/
void do_ignore(char_data *ch, char *argument)
{
  	char_data *victim;
  	char arg[MIL];
  	int pos;

  	one_argument(argument, arg);

  	if (IS_NPC(ch))
    		return;

  	if (arg[0] == '\0')
  	{
  		for (pos = 0; pos < MAX_IGNORE; pos++)
  		{
			if (!IS_NULLSTR(ch->pcdata->ignore[pos]))
			{
      				ch->printlnf("%d. %s",pos+1, ch->pcdata->ignore[pos]);
			}
    		}
		return;
  	}
	if ( !str_cmp( arg, "1" ))
    	{
		ch->pcdata->ignore[0] = NULL;
    		ch->println("Ignore #1 cleared.");
    		return;
	}
	if ( !str_cmp( arg, "2" ))
    	{
		ch->pcdata->ignore[1] = NULL;
    		ch->println("Ignore #2 cleared.");
    		return;
	}
	if ( !str_cmp( arg, "3" ))
    	{
		ch->pcdata->ignore[2] = NULL;
    		ch->println("Ignore #3 cleared.");
    		return;
	}
	if ( !str_cmp( arg, "4" ))
    	{
		ch->pcdata->ignore[3] = NULL;
    		ch->println("Ignore #4 cleared.");
    		return;
	}
	if ( !str_cmp( arg, "5" ))
    	{
		ch->pcdata->ignore[4] = NULL;
    		ch->println("Ignore #5 cleared.");
    		return;
	}
	if ( !str_cmp( arg, "6" ))
    	{
		ch->pcdata->ignore[5] = NULL;
    		ch->println("Ignore #6 cleared.");
    		return;
	}
	if ( !str_cmp( arg, "7" ))
    	{
		ch->pcdata->ignore[6] = NULL;
    		ch->println("Ignore #7 cleared.");
    		return;
	}
	if ( !str_cmp( arg, "8" ))
    	{
		ch->pcdata->ignore[7] = NULL;
    		ch->println("Ignore #8 cleared.");
    		return;
	}
	if ( !str_cmp( arg, "9" ))
    	{
		ch->pcdata->ignore[8] = NULL;
    		ch->println("Ignore #9 cleared.");
    		return;
	}
	if ( !str_cmp( arg, "10" ))
    	{
		ch->pcdata->ignore[9] = NULL;
    		ch->println("Ignore #10 cleared.");
    		return;
	}

  	if ((victim = get_char_world(ch, argument)) == NULL)
  	{
    		ch->println("They aren't here.");
    		return;
  	}

  	if (IS_NPC(victim) || IS_IMMORTAL(victim))
  	{
    		ch->println("I don't think so.");
    		return;
  	}

  	if (ch == victim)
  	{
    		ch->println("I don't think you really want to ignore yourself.");
    		return;
  	}

  	for (pos = 0; pos < MAX_IGNORE; pos++)
  	{
    		if (ch->pcdata->ignore[pos] == NULL)
      			break;

    		if (!str_cmp(victim->name, ch->pcdata->ignore[pos]))
    		{
      			ch->pcdata->ignore[pos] = NULL;
      			ch->printlnf("You stop ignoring %s.", victim->name);
      			victim->printlnf("%s stops ignoring you.", ch->name);
      			return;
    		}
  	}
 
  	if (pos >= MAX_IGNORE)
  	{
    		ch->println("You can't ignore anymore people.");
    		return;
  	}
 
  	ch->pcdata->ignore[pos] = victim->name;
  	ch->printlnf("You now ignore %s.", victim->name);
  	victim->printlnf("%s ignores you.", ch->name);
  	return;
}
/***************************************************************************/
/***************************************************************************/


