/**************************************************************************/
// magic_ra.cpp - spells/skills written by Rathern
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/
/***************************************************************************
 *  FILE: magic_ra.c - spells written by Rathern                           *
 *                                                                         *
 * summon_guardian - conjuration, summoning & wild magic (roleplay spell)  *
 * cause_fear      - phantasum, illusion                                   *
 * wind shield     - air, conjuration                                      *
 * spell_imprint   - for brew and scribe no realms                         *
 * brew $ scribe   - skills for magic some clsses                          *
 * soberness       - time & healing                                        *
 * drunkeness      - body                                                  * 
 ***************************************************************************/
#include "include.h"
#include "magic.h"
#include "msp.h"

/********************************/
/* START OF FUNCTION PROTOTYPES */

int get_direction( char *arg );
DECLARE_DO_FUN(do_bug		);
DECLARE_DO_FUN(do_wear		);

void do_fearful(char_data *);  /* in affects.c */
void do_fearmagic(char_data *);  /* in affects.c */
bool	remove_obj	args( (char_data *ch, int iWear, bool fReplace ) );

/*  END OF FUNCTION PROTOTYPES  */
/********************************/


/****************************************************************************
 *  spell_summon_guardian - by Rathern & Kalahn - September 97              *
 ****************************************************************************/
SPRESULT spell_summon_guardian( int , int , char_data *ch, void *, int )
{
    char_data *mob_guardian;
    MOB_INDEX_DATA *pMobIndex;
    char buf[MSL];

    if ((pMobIndex = get_mob_index(MOB_VNUM_SUMMON_GUARDIAN))) /* make sure mob exists */
    {
        mob_guardian = create_mobile( pMobIndex, 0 ); /*grab mobile SUMMON_GUARDIAN_MOB_VNUM */
    
        mob_guardian->level = ch->level;          /*set mobs level*/
        mob_guardian->max_hit = ch->level*10;     /*set mobs max hit points*/
        mob_guardian->hit = mob_guardian->max_hit* 2/3;   /*set mobs current hit points to a third its total*/
        mob_guardian->timer = ch->level;          /*set how long the mob will last*/
        char_to_room( mob_guardian, ch->in_room); /*send mob to summoner*/
    
        act("As $n completes $s spell, a large stone golem with wings, forms in the room.", ch,NULL,NULL,TO_ROOM);
        ch->println("A shimmering stone golem with wings fades into existence.");
    }
    else // mob with the vnum not found
    {
        sprintf(buf,"BUG: in spell_summon_guardian - missing mob vnum %d!\r\n", MOB_VNUM_SUMMON_GUARDIAN);
        wiznet(buf,NULL,NULL,WIZ_BUGS,0,AVATAR); /* put it on the bug wiznet channel */
        log_string( buf ); /* log the bug in the logs */
        do_bug ( ch, buf); /* report the bug into the bug file */
        ch->print( buf );
        return NO_MANA;
    }
	return FULL_MANA;
}    


/****************************************************************************
 *  spell_cause_fear      - by Rathern & Kalahn - September 97              *
 ****************************************************************************/
SPRESULT spell_cause_fear( int sn, int level, char_data *ch, void *vo,int )
{
    char_data *victim = (char_data *) vo;
    AFFECT_DATA af;
	
    if ( saves_spell( level, victim, DAMTYPE(sn))){
		ch->println("The spell fizzles and dies.");
		return FULL_MANA;
    }
	
    if ( IS_AFFECTED(victim, AFF_FEAR) || IS_AFFECTED2(victim, AFF2_FEAR_MAGIC) ){
		ch->println("They are already running for their life.");
		return HALF_MANA;
	}
	
	if (IS_SET(victim->imm_flags,IMM_FEAR)){
		ch->println( "They are unaffected by your fear." );
		return FULL_MANA;
	}
	
    af.where     = WHERE_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 1;
    if (ch->level > 25) af.duration +=1;
    if (ch->level > 50) af.duration +=1;
    if (ch->level > 75) af.duration +=1;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_FEAR;
    affect_to_char( victim, &af );
    victim->println("You panic as you are gripped by an incredible fear.");
    act( "$n screams and runs away.", victim, NULL, NULL, TO_ROOM );
    do_fearful(victim); // in magic.c 
    return FULL_MANA;
}

/****************************************************************************
 *  spell_fear_magic    - by Tibault - June 2000 (copy of spell_cause_fear) *
 ****************************************************************************/
SPRESULT spell_fear_magic( int sn, int level, char_data *ch, void *vo,int )
{
    char_data *victim = (char_data *) vo;
    AFFECT_DATA af;
	
    if ( saves_spell( level, victim, DAMTYPE(sn)))
    {
		ch->println("The spell fizzles and dies.");
		return FULL_MANA;
    }
	
    if (IS_SET(victim->imm_flags,IMM_FEAR))
    {
		ch->println( "They are unaffected by your fear." );
		return FULL_MANA;
    }
	
    if ( IS_AFFECTED2(victim, AFF2_FEAR_MAGIC) || IS_AFFECTED(victim, AFF_FEAR) )
	{
		ch->println("They are already running for their life.");
		return HALF_MANA;
	}
	
    af.where     = WHERE_AFFECTS2;
    af.type      = sn;
    af.level     = level;
    af.duration  = 1;
    if (ch->level > 25) af.duration +=1;
    if (ch->level > 50) af.duration +=1;
    if (ch->level > 75) af.duration +=1;
	af.location  = APPLY_ST;
	af.modifier  = -1 * (level / 10);
    af.bitvector = AFF2_FEAR_MAGIC;
    affect_to_char( victim, &af );
    victim->println("You panic as you are gripped by an incredible fear.");
    act( "$n screams and runs away.", victim, NULL, NULL, TO_ROOM );
    do_fearmagic(victim); /* in magic.c */
    return FULL_MANA;
}
/****************************************************************************
 *  spell_spell_imprint (brew,scribe) - by Jason Huang edited by Rathern    *
 ****************************************************************************/
SPRESULT spell_imprint( int sn, int, char_data *ch, void *vo )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    int       sp_slot, i, mana;
    char      buf[ MSL ];
	
	if (skill_table[sn].spell_fun == spell_null )
	{
        ch->println("That is not a spell.");
        return NO_MANA;
	}
	
    // counting the number of spells contained within	
    for (sp_slot = i = 1; i < 5; i++)
	{
		if (obj->value[i] != -1)
			sp_slot++;
	}
	
    if (sp_slot > 4)
    {
        act ("$p cannot contain any more spells.", ch, obj, NULL, TO_CHAR);
        return NO_MANA;
    }
	
	// scribe/brew costs 4 times the normal mana required to cast the spell
    mana = ((skill_table[sn].min_mana)*4);
	
    if ( !IS_NPC(ch) && ch->mana < mana )
    {
        ch->println("You don't have enough mana.");
        return NO_MANA;
    }
	
    if ( number_percent( ) > get_skill(ch,sn))
    {
        ch->println("You lost your concentration.");
        ch->mana -= mana / 2;
        return NO_MANA;
    }
	
    // executing the imprinting process	
    ch->mana -= mana;
    obj->value[sp_slot] = sn;
	
	
    // Making it successively harder to pack more spells 
	// into potions or scrolls - JH 
	if ( !IS_IMMORTAL(ch) )
	{
		switch( sp_slot )
		{
			
		default:
			bugf( "sp_slot has more than %d spells.", sp_slot );
			return NO_MANA;
			
		case 1:
			if ( number_percent() > 90 )
			{ 
				ch->printlnf( "The magic enchantment has failed --- the %s vanishes.",
					item_type_name(obj) );
				extract_obj( obj );
				check_improve(ch,gsn_scribe, false, 1);
				return FULL_MANA;
			}     
			break;
			
			
		case 2:
			if ( number_percent() > 45 )
			{ 
				ch->printlnf( "The magic enchantment has failed --- the %s vanishes.",
					item_type_name(obj) );        // (obj->item_type) );
				extract_obj( obj );
				check_improve(ch,gsn_scribe, false, 1);
				return FULL_MANA;
			}     
			break;
			
			
		case 3:
			if ( number_percent() > 20 )
			{ 
				ch->printlnf( "The magic enchantment has failed --- the %s vanishes.",
					item_type_name(obj) );
				extract_obj( obj );
				check_improve(ch,gsn_scribe, false, 1);
				return FULL_MANA;
			}     
			break;
		}
	}
	
	
    // labeling the item	
    free_string (obj->short_descr);
    sprintf ( buf, "a %s of ", item_type_name(obj) ); 
    for (i = 1; i <= sp_slot ; i++)
	{
		if (obj->value[i] != -1)
			
		{
			strcat (buf, skill_table[obj->value[i]].name);
			(i != sp_slot ) ? strcat (buf, ", ") : strcat (buf, "") ; 
		}
	}
	
	obj->short_descr = str_dup(buf);
	sprintf( buf, "scroll %s",  skill_table[sn].name);
	replace_string( obj->name, buf );
	sprintf( buf, "A scroll of %s.", skill_table[sn].name);
	replace_string( obj->description, buf );
	ch->printlnf( "You have imbued a new spell to the %s.", item_type_name(obj) );
	check_improve(ch,gsn_scribe, true, 3);
	return FULL_MANA;
}

/*****************************************************************************/
SPRESULT spell_brew_imprint( int sn, int, char_data *ch, void *vo )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    int       sp_slot, i, mana;
    char      buf[ MSL ];
	
	if (skill_table[sn].spell_fun == spell_null )
	{
        ch->println("That is not a spell.");
        return NO_MANA;
	}
	
    // counting the number of spells contained within	
    for (sp_slot = i = 1; i < 5; i++)
	{
		if (obj->value[i] != -1)
			sp_slot++;
	}
	
    if (sp_slot > 4)
    {
        act ("$p cannot contain any more spells.", ch, obj, NULL, TO_CHAR);
        return NO_MANA;
    }
	
	// scribe/brew costs 4 times the normal mana required to cast the spell
    mana = ((skill_table[sn].min_mana)*4);
	
    if ( !IS_NPC(ch) && ch->mana < mana )
    {
        ch->println("You don't have enough mana.");
        return NO_MANA;
    }
	
    if ( number_percent( ) > get_skill(ch,sn))
    {
        ch->println("You lost your concentration.");
        ch->mana -= mana / 2;
        return NO_MANA;
    }
	
    // executing the imprinting process	
    ch->mana -= mana;
    obj->value[sp_slot] = sn;
	
	
    // Making it successively harder to pack more spells 
	// into potions or scrolls - JH 
	if ( !IS_IMMORTAL(ch) )
	{
		switch( sp_slot )
		{
			
		default:
			bugf( "sp_slot has more than %d spells.", sp_slot );
			return NO_MANA;
			
		case 1:
			if ( number_percent() > 90 )
			{ 
				ch->printlnf( "The magic enchantment has failed --- the %s vanishes.",
					item_type_name(obj) );
				extract_obj( obj );
				check_improve(ch, gsn_brew, false, 1);
				return FULL_MANA;
			}     
			break;
			
			
		case 2:
			if ( number_percent() > 45 )
			{ 
				ch->printlnf( "The magic enchantment has failed --- the %s vanishes.",
					item_type_name(obj) );        // (obj->item_type) );
				extract_obj( obj );
				check_improve(ch, gsn_brew, false, 1);
				return FULL_MANA;
			}     
			break;
			
			
		case 3:
			if ( number_percent() > 20 )
			{ 
				ch->printlnf( "The magic enchantment has failed --- the %s vanishes.",
					item_type_name(obj) );
				extract_obj( obj );
				check_improve(ch, gsn_brew, false, 1);
				return FULL_MANA;
			}     
			break;
		}
	}
	
	
    // labeling the item	
    free_string (obj->short_descr);
    sprintf ( buf, "a %s of ", item_type_name(obj) ); 
    for (i = 1; i <= sp_slot ; i++)
	{
		if (obj->value[i] != -1)
			
		{
			strcat (buf, skill_table[obj->value[i]].name);
			(i != sp_slot ) ? strcat (buf, ", ") : strcat (buf, "") ; 
		}
	}
	
	obj->short_descr = str_dup(buf);
	sprintf( buf, "potion %s",  skill_table[sn].name);
	replace_string( obj->name, buf );
	sprintf( buf, "A potion of %s.", skill_table[sn].name);
	replace_string( obj->description, buf );
	ch->printlnf( "You have imbued a new spell to the %s.", item_type_name(obj) );
	check_improve(ch, gsn_brew, true, 3);
	return FULL_MANA;
}


/****************************************************************************
 *  VOID_DO_BREW        (brew,scribe) - by Jason Huang edited by Rathern    *
 ****************************************************************************/
void do_brew ( char_data *ch, char *argument )
{
    char arg[MIL];
    OBJ_DATA *obj;
    int sn;
    int dam;

	// Check if they're being ordered to do this
	if ( IS_SET( ch->dyn, DYN_IS_BEING_ORDERED )){
		if(ch->master){
			ch->master->println( "Not going to happen.\r\n");
		}
		return;
	}


    if ( !IS_NPC( ch )                                                  
	&& ch->level < skill_table[gsn_brew].skill_level[ch->clss] )
    {                                          
        ch->println("You do not know how to brew potions.");
        return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        ch->println("Brew what spell?");
        return;
    }

    // Do we have a vial to brew potions? 
    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
        if ( obj->item_type == ITEM_POTION && obj->wear_loc == WEAR_HOLD )
            break;
    }

    /* Interesting ... Most scrolls/potions in the mud have no hold
       flag; so, the problem with players running around making scrolls 
       with 3 heals or 3 gas breath from pre-existing scrolls has been 
       severely reduced. Still, I like the idea of 80% success rate for  
       first spell imprint, 25% for 2nd, and 10% for 3rd. I don't like the
       idea of a scroll with 3 ultrablast spells; although, I have limited
       its applicability when I reduced the spell->level to 1/3 and 1/4 of 
       ch->level for scrolls and potions respectively. --- JH */


    /* I will just then make two items, an empty vial and a parchment available
       in midgaard shops with holdable flags and -1 for each of the 3 spell
       slots. Need to update the midgaard.are files --- JH */

    if ( !obj )
    {
        ch->println("You are not holding a vial.");
        return;
    }

    if ( ( sn = skill_lookup(arg) )  < 0)
    {
        ch->println("You don't know any spells by that name.");
        return;
    }

    /* preventing potions of gas breath, acid blast, etc.; doesn't make sense
       when you quaff a gas breath potion, and then the mobs in the room are
       hurt. Those TAR_IGNORE spells are a mixed blessing. - JH */
  
    if ( (skill_table[sn].target != TAR_CHAR_DEFENSIVE) && 
         (skill_table[sn].target != TAR_CHAR_SELF)              ) 
    {
        ch->println("You cannot brew that spell.");
        return;
    }

        act( "$n begins preparing a potion.", ch, obj, NULL, TO_ROOM );
        act( "You begin to brew a potion.", ch, obj, NULL, TO_CHAR );

		if ( !IS_IMMORTAL( ch ))
	        WAIT_STATE( ch, skill_table[gsn_brew].beats ); 

    /* Check the skill percentage, memory and reasoning checks) */

    if ( !IS_NPC(ch) 
         && ( number_percent( ) > ch->pcdata->learned[gsn_brew] ||
              number_percent( ) > ((ch->modifiers[STAT_ME]-7)*5 + 
                                   (ch->modifiers[STAT_RE]-7)*3) ))
    {
        act( "$p explodes violently!", ch, obj, NULL, TO_CHAR );
        act( "$p explodes violently!", ch, obj, NULL, TO_ROOM );

		msp_to_room(MSPT_ACTION, MSP_SOUND_EXPLOSION, 
						0,
						ch,
						false,
						true);	

        dam = dice( ch->level, 10 );
        if ( saves_spell( ch->level, ch, DAM_ACID ) )
        dam /= 2;
        damage_spell( ch, ch, dam, gsn_acid_blast,DAM_ACID,true);
        extract_obj( obj );
        return;
    }

    /* took this outside of imprint codes, so I can make do_brew differs from
       do_scribe; basically, setting potion level and spell level --- JH */

    obj->level = ch->level/2;
    obj->value[0] = ch->level/4;
    obj->cost = number_range(obj->level*10, obj->level*75)/2;
    SET_BIT(obj->extra2_flags,OBJEXTRA2_PC_CRAFTED);
    spell_brew_imprint(sn, ch->level, ch, obj); /* in magic_r.c and interpt.h */ 
    msp_skill_sound(ch, gsn_brew);
}


/****************************************************************************
 *  VOID_DO_SCRIBE      (brew,scribe) - by Jason Huang edited by Rathern    *
 ****************************************************************************/
void do_scribe ( char_data *ch, char *argument )
{
    char arg[MIL];
    OBJ_DATA *obj;
    int sn;
    int dam;

	// Check if they're being ordered to do this
	if ( IS_SET( ch->dyn, DYN_IS_BEING_ORDERED )){
		if(ch->master){
			ch->master->println( "Not going to happen.");
		}
		return;
	}

    if ( !IS_NPC( ch )                                                  
	&& ch->level < skill_table[gsn_scribe].skill_level[ch->clss] )
	{
		ch->println("You are not able to scribe in your current condition.");
		return;
	}

	if ( IS_NPC( ch ) || IS_CONTROLLED( ch ) )
	{
		ch->println( "You are too charmed to concentrate on scribing." );
		return;
	}

	argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        ch->println("Scribe what spell?");
        return;
    }

    // Do we have a parchment to scribe spells? 
    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
        if ( obj->item_type == ITEM_SCROLL && obj->wear_loc == WEAR_HOLD )
            break;
    }

    if ( !obj )
    {
        ch->println("You are not holding a parchment.");
        return;
    }


    if ( ( sn = skill_lookup(arg) )  < 0)
    {
        ch->println("You don't know any spells by that name.");
        return;
    }

    if ( !IS_SPELL(sn))
    {
        ch->println("That isn't a spell.");
        return;
    }

	if ( IS_SET(skill_table[sn].flags, SKFLAGS_NO_SCRIBE) && !IS_IMMORTAL(ch) )
	{
		ch->println("It is impossible for a scroll to capture the essence of this spell.");
		return;
	}
    
    act( "$n begins writing a scroll.", ch, obj, NULL, TO_ROOM );
    act( "You begin to scribe a spell.", ch, obj, NULL, TO_CHAR );

	if ( !IS_IMMORTAL( ch )){
		WAIT_STATE( ch, skill_table[gsn_scribe].beats );
	}

    // Check the skill percentage, fcn(int,wis,skill) 
    if ( !IS_NPC(ch) 
         && ( number_percent( ) > ch->pcdata->learned[gsn_scribe] ||
              number_percent( ) > ((ch->modifiers[STAT_ME]-7)*5 + 
                                   (ch->modifiers[STAT_RE]-7)*3)) 
		 && !IS_IMMORTAL(ch) )
    {
        act( "$p bursts in flames!", ch, obj, NULL, TO_CHAR );
        act( "$p bursts in flames!", ch, obj, NULL, TO_ROOM );
    
        dam = dice(ch->level, 6)+10;
        if ( saves_spell( ch->level, ch, DAM_FIRE) ){
			dam /= 2;
		}
        damage_spell( ch, ch, dam, skill_lookup("fireball"),DAM_FIRE,true);
        extract_obj( obj );
		check_improve(ch,gsn_scribe, false, 1);
        return;
    }

    /* basically, making scrolls more potent than potions; also, scrolls
       are not limited in the choice of spells, i.e. scroll of enchant weapon
       has no analogs in potion forms --- JH */

    obj->level = ch->level*2/3;
    obj->value[0] = ch->level/3;
    obj->cost = number_range(obj->level*10, obj->level*75)/2;
    SET_BIT(obj->extra2_flags,OBJEXTRA2_PC_CRAFTED);
    spell_imprint(sn, ch->level, ch, obj); 
    msp_skill_sound(ch, gsn_scribe);

}


/****************************************************************************
 *  Starvation by Rathern                                                   *
 ****************************************************************************/
SPRESULT spell_starvation(int sn,int level,char_data *ch, void *vo,int )
{
    char_data *victim = (char_data *) vo;
    
    if (saves_spell(level, victim,DAMTYPE(sn)))
    {
		ch->println("Your spell fizzles and dies.");
		return FULL_MANA;
    } 
	
	if (IS_SET(victim->imm_flags,IMM_HUNGER))
	{
		ch->println( "They are unaffected by starvation." );
		return FULL_MANA;		
	}
	
    if (IS_NPC(victim))
    {
		act("$n looks extremely hungry all of a sudden.",victim,NULL,ch,TO_ROOM);
    }else{
		victim->println("The pains of hunger grip your stomach.");
		act("$n clutches at their stomach, they look in agony.",victim,NULL,NULL,TO_ROOM);
		victim->pcdata->condition[COND_HUNGER] = 0;
	}
    return FULL_MANA;
}


/****************************************************************************
 *  Dehydration by Rathern                                                  *
 ****************************************************************************/
SPRESULT spell_dehydration( int sn, int level, char_data *ch, void *vo,int )
{
	char_data *victim = (char_data *) vo;
	
	if (saves_spell(level , victim,DAMTYPE(sn)))
	{
		ch->println("Your spell fizzles and dies.");
		return FULL_MANA;
	} 
	
	if (IS_SET(victim->imm_flags,IMM_THIRST))
	{
		ch->println( "They are unaffected by thirst." );
		return FULL_MANA;
		
	}
	
	if (IS_NPC(victim))
	{
		act("$n looks extremely thirsty all of a sudden.",victim,NULL,ch,TO_CHAR);
    }
	else
	{
		victim->println("Your throat is dry and parched, you feel incredibly thirsty.");
		act("$n clutches at their throat, they look in agony.",victim,NULL,NULL,TO_ROOM); 
		victim->pcdata->condition[COND_THIRST] = 0;
	}
    return FULL_MANA;
}


/****************************************************************************
 *  wind_shield by Rathern                                                  *
 ****************************************************************************/
SPRESULT spell_wind_shield( int sn, int level, char_data *ch, void *vo, int )
{
    char_data *victim = (char_data *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
    {
		if (victim == ch)
			ch->println("You have that spell on you already.");
		else
			act("$N already has a shield of wind surrounding them.",ch,NULL,victim,TO_CHAR);
		return HALF_MANA;
    }

    if ( is_affected( victim, gsn_cyclone ) )
    {
		if (victim == ch)
			ch->println("The winds are protecting you as much as they can.");
		else
			act("$N is already protected by the winds.",ch,NULL,victim,TO_CHAR);
		return HALF_MANA;
    }


    af.where     = WHERE_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level/10;
    af.modifier  = ch->level/-2;
    af.location  = APPLY_AC;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    victim->println("You are enveloped within a whirling tomb of wind.");
    act("A roaring wind envelops $n.",victim,NULL,NULL,TO_ROOM);
    return FULL_MANA;
}


/****************************************************************************
 *  Sober by Rathern                                                        *
 ****************************************************************************/
SPRESULT spell_sober( int sn, int level, char_data *ch, void *vo,int )
{
	char_data *victim = (char_data *) vo;
	
	if (saves_spell(level , victim,DAMTYPE(sn)))
	{
		ch->println("Your spell fizzles and dies.");
		return FULL_MANA;
	} 

	if (IS_NPC(victim))
	{
		act("$n looks extremely sober all of a sudden.",victim,NULL,ch,TO_CHAR);
	}
	else
	{
		victim->println("Your feel suddenly more sober and awake.");
		act("$n looks more sober all of a sudden.",victim,NULL,NULL,TO_ROOM); 
		victim->pcdata->condition[COND_DRUNK] = 0;
	}
    return FULL_MANA;
}


/****************************************************************************
 *  Drunkenness by Rathern                                                  *
 ****************************************************************************/
SPRESULT spell_drunkeness( int sn, int level, char_data *ch, void *vo,int )
{
	char_data *victim = (char_data *) vo;

	if (saves_spell(level , victim,DAMTYPE(sn)))
	{
		ch->println("Your spell fizzles and dies.");
		return FULL_MANA;
	}

	if (IS_NPC(victim))
	{
		act("$n looks extremely drunk all of a sudden.",victim,NULL,ch,TO_CHAR);
	}
	else
	{
		victim->println("Your feel suddenly drunk all of a sudden.");
		act("$n looks drunk all of a sudden.",victim,NULL,NULL,TO_ROOM); 
		victim->pcdata->condition[COND_DRUNK] = 15;
	}
    return FULL_MANA;
}


/****************************************************************************
 *  Permanance by Kalahn & Rathern                                          *
 ****************************************************************************/
SPRESULT spell_permanance(int sn, int level, char_data *ch, void *vo, int )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;
	SPRESULT MANA_RESULT=FULL_MANA;
    int result, fail;
    bool hit_found = false, dam_found = false, ac_found = false;

    // do standard checks if it can be done here
//    if ((obj->item_type !=ITEM_WEAPON) && (obj->item_type !=ITEM_ARMOR))
    if ((obj->item_type != ITEM_ARMOR)
	  && (obj->item_type !=ITEM_WEAPON)
      && (obj->item_type != ITEM_LIGHT) 
	  && (obj->item_type != ITEM_TREASURE)
      && (obj->item_type != ITEM_CLOTHING)
	  && (obj->item_type != ITEM_CONTAINER)
	  && (obj->item_type != ITEM_GEM)
      && (obj->item_type != ITEM_JEWELRY))
    {
        ch->println("That isn't a suitable item.");
		return NO_MANA;
	}
	
	if (obj->wear_loc != -1)
	{
		ch->println("The item must be carried to have this spell cast on it.");
		return NO_MANA;
	}

	if (!IS_NPC(ch))
	{
		if (ch->pcdata->tired > 16)
		{
			ch->println( "You are not well-rested enough to cast this "
				"spell just now.  Have a nap first." );
			return NO_MANA;
		}
	}

	fail = 25;	// base 25% chance of failure

	// those who are skilled at permance have a better chance
	if (!IS_NPC(ch))
	{
		fail -= (ch->pcdata->learned[sn]/4);

		if (ch->pcdata->learned[sn]>75)
			fail -= (ch->pcdata->learned[sn]-75)*2;
	}

	// find the current bonuses - affect success
	for (paf=OBJECT_AFFECTS(obj);paf != NULL; paf = paf->next )
	{
		if ( paf->location == APPLY_HITROLL )
		{
			hit_found = true;
			fail += 2 * paf->modifier * paf->modifier;
		}
		else if (paf->location == APPLY_DAMROLL )
		{
			dam_found = true;
			fail += 2 * paf->modifier * paf->modifier;
		}
		else if ( paf->location == APPLY_AC )
		{
			ac_found = true;
			fail += 4 * paf->modifier * paf->modifier;
		}
		else
		{
			fail += 10;
		}
	}
	
	// apply other modifiers
	fail -= 3 * level/2;

	if (IS_OBJ_STAT(obj,OBJEXTRA_BLESS))
		fail -= 25;
	if (IS_OBJ_STAT(obj,OBJEXTRA_GLOW))
		fail -= 5;

	fail = URANGE(5,fail,95);
	result = number_percent();
	
    if (!IS_NPC(ch))
    {
        MANA_RESULT=ALL_MANA;
        ch->pcdata->tired += 25;
        ch->pcdata->condition[COND_THIRST] = 0;
        ch->pcdata->condition[COND_HUNGER] = 0;
    }

    // the moment of truth
    if (result < (fail / 5))  // item disolved
    {
        act("$p shivers and dissolves before your eyes!",ch,obj,NULL,TO_CHAR);
        act("$p shivers and dissolves before your eyes!",ch,obj,NULL,TO_ROOM);
        extract_obj(obj);
        return MANA_RESULT;
    }

    if (result < (fail / 2)) // item disenchanted
	{
		AFFECT_DATA *paf_next;

		act("$p glows brightly, then fades...oops.",ch,obj,NULL,TO_CHAR);
		act("$p glows brightly, then fades.",ch,obj,NULL,TO_ROOM);

        // free all affects 
        for (paf = obj->affected; paf; paf = paf_next)
        {
            paf_next = paf->next; 
            free_affect(paf);
        }
        obj->affected = NULL;
        obj->no_affects = true; // flag the object as no_affects so 
								// the olc template's affects arent used.
        // clear all flags 
        obj->extra_flags = 0;
		return MANA_RESULT;
	}

    if ( result <= fail )  // failed, no bad result
    {
		ch->println("Nothing seemed to happen.");
		return MANA_RESULT;
	}

	if (!obj->affected)
	{
		act("You couldn't find any effect on $p to make permanent.", ch, obj, NULL,TO_CHAR);
		return HALF_MANA;
	}

	// now make the enchants perm
	if (ac_found) 
    {
        for ( paf = obj->affected; paf != NULL; paf = paf->next)
        {
            if ( paf->location == APPLY_AC && paf->duration!=-1)
            {
                paf->duration = -1;
                act("The protective qualities are now permanent.",ch,obj,NULL,TO_CHAR);
            }
        }
    }
    if (dam_found) // damroll
    {
        for ( paf = obj->affected; paf != NULL; paf = paf->next)
        {
            if (paf->location == APPLY_DAMROLL && paf->duration!=-1)
            {
                paf->duration = -1;
                act("The damaging affect is now permanent.",ch,obj,NULL,TO_CHAR);
            }
        }
    }
    if (hit_found)
    {
        for ( paf = obj->affected; paf != NULL; paf = paf->next)
        {
            if ( paf->location == APPLY_HITROLL && paf->duration!=-1)
            {
                paf->duration = -1;
                act("The hitting affect is now permanent.",ch,obj,NULL,TO_CHAR);
            }
        }
    }
    obj->level+=5; // increase the level of the object
	return MANA_RESULT;
}

/****************************************************************************
 *  Blade Permanance by Kalahn                                              *
 ****************************************************************************/
SPRESULT spell_blade_permanance(int , int level, char_data *ch, void *vo, int)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;
    int result, fail;
    bool vorpal_found = false, sharp_found = false, frost_found= false,
        shocking_found= false, flame_found = false, vampiric_found= false;

    /* do standard checks if it can be done here */
    if ((obj->item_type !=ITEM_WEAPON))
    {
        ch->println("That isn't an suitable item.");
        return NO_MANA;
    }
    if (obj->wear_loc != -1)
    {
        ch->println("The item must be carried to have this spell cast on it.");
        return NO_MANA;
    }

    fail = 25;	// base 25% chance of failure

    // find the current bonuses - affect success 
    for (paf = OBJECT_AFFECTS(obj); paf; paf=paf->next )
    {
        if ( paf->bitvector== WEAPON_VORPAL && paf->duration != -1)
        {
            vorpal_found = true;
            fail += 60;
        }      
        else if ( paf->bitvector== WEAPON_SHARP && paf->duration != -1)
        {
            sharp_found = true;
            fail += 60;
        }      
        else if ( paf->bitvector== WEAPON_FLAMING && paf->duration != -1)
        {
            flame_found = true;
            fail += 60;
        }      
        else if ( paf->bitvector== WEAPON_SHOCKING && paf->duration != -1)
        {
            shocking_found = true;
            fail += 60;
        }      
        else if ( paf->bitvector== WEAPON_FROST && paf->duration != -1)
        {
            frost_found = true;
            fail += 60;
            
        }
        else if ( paf->bitvector== WEAPON_VAMPIRIC && paf->duration != -1)
        {
            vampiric_found = true;
            fail += 80;           
        }
        else
        {
            fail += 25;
        }
    }

    // apply other modifiers 
    fail -= 3 * level/2;

    if (IS_OBJ_STAT(obj,OBJEXTRA_BLESS)){
        fail -= 25;
    }
	if (IS_OBJ_STAT(obj,OBJEXTRA_GLOW)){
        fail -= 5;
	}

    fail = URANGE(5,fail,95);
    result = number_percent();

    // the moment of truth 
    if (result < (fail / 5))  // item disolved 
    {
        act("$p shivers and dissolves before your eyes!",ch,obj,NULL,TO_CHAR);
        act("$p shivers and dissolves before your eyes!",ch,obj,NULL,TO_ROOM);
        extract_obj(obj);
        return FULL_MANA;
    }

    if (result < (fail / 2)) // item disenchanted 
    {
        AFFECT_DATA *paf_next;
    
        act("$p glows brightly, then fades...oops.",ch,obj,NULL,TO_CHAR);
        act("$p glows brightly, then fades.",ch,obj,NULL,TO_ROOM);
    
        // free all affects 
        for (paf = obj->affected; paf != NULL; paf = paf_next)
        {
            paf_next = paf->next; 
            free_affect(paf);
        }
        obj->affected = NULL;
        obj->no_affects = true; // flag the object as no_affects so 
								// the olc template's affects arent used.
        // clear all flags 
        obj->extra_flags = 0;
        return FULL_MANA;
    }

    if ( result <= fail )  // failed, no bad result
    {
        ch->println("Nothing seemed to happen.");
        return FULL_MANA;
    }

    if (!obj->affected) // if it doesn't have any local affects, noone has 'enchanted' it
    {
        act("You couldn't find any effect on $p to make permanent.",
            ch, obj, NULL,TO_CHAR);
        return HALF_MANA;
    }

    // now make the enchants perm 
    if (vorpal_found)
    {
        for ( paf = obj->affected; paf != NULL; paf = paf->next)
        {
            if ( paf->bitvector== WEAPON_VORPAL && paf->duration != -1)
            {
                paf->duration = -1;
                act("The vorpal affect is now permanent.",ch,obj,NULL,TO_CHAR);
            }
        }
    }
    if (sharp_found)
    {
        for ( paf = obj->affected; paf != NULL; paf = paf->next)
        {
            if ( paf->bitvector== WEAPON_SHARP && paf->duration != -1)
            {
                paf->duration = -1;
                act("The sharp affect is now permanent.",ch,obj,NULL,TO_CHAR);
            }
        }
    }
    if (flame_found)
    {
        for ( paf = obj->affected; paf != NULL; paf = paf->next)
        {
            if ( paf->bitvector== WEAPON_FLAMING && paf->duration != -1)
            {
                paf->duration = -1;
                act("The fiery aura appears to be permanent now.",ch,obj,NULL,TO_CHAR);
            }
        }
    }
    if (shocking_found)
    {
        for ( paf = obj->affected; paf != NULL; paf = paf->next)
        {
            if ( paf->bitvector== WEAPON_SHOCKING && paf->duration != -1)
            {
                paf->duration = -1;
                act("The sparks of electricity appear to be more predictable and stable now.",ch,obj,NULL,TO_CHAR);
            }
        }
    }
    if (frost_found)
    {
        for ( paf = obj->affected; paf != NULL; paf = paf->next)
        {
            if ( paf->bitvector== WEAPON_FROST && paf->duration != -1)
            {
                paf->duration = -1;
                act("The frost on the blade is now permanent.",ch,obj,NULL,TO_CHAR);
            }
        }
    }
    if (vampiric_found)
    {
        for ( paf = obj->affected; paf != NULL; paf = paf->next)
        {
            if ( paf->bitvector== WEAPON_VAMPIRIC && paf->duration != -1)
            {
                paf->duration = -1;
                act("The vampiric affects are now permanent.",ch,obj,NULL,TO_CHAR);
            }
        }
    }

	return FULL_MANA;
}


/****************************************************************************
 *  steel_breath by Rathern                                                 *
 ****************************************************************************/
SPRESULT spell_steel_breath( int sn, int level, char_data *ch, void *,int )
{
    char_data *vch;
    char_data *vch_next;
    int dam,hp_dam,dice_dam,hpch;

    act("$n breathes out a rain of deadly steel!",ch,NULL,NULL,TO_ROOM);
    act("You breath out a rain of deadly steel.",ch,NULL,NULL,TO_CHAR);

    hpch = UMAX(16,ch->hit);
    hp_dam = number_range(hpch/15+1,8);
    dice_dam = dice(level,12);

    dam = UMAX(hp_dam + dice_dam/10,dice_dam + hp_dam/10);

    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
		vch_next = vch->next_in_room;
		
		if (is_safe_spell(ch,vch,true)
			||  (IS_NPC(ch) && IS_NPC(vch) 
			&&   (ch->fighting == vch || vch->fighting == ch)))
			continue;
		
		// died from last attack, or got out of room for some other reason
		if(vch->in_room!=ch->in_room){
			continue;
		}
		
		if (saves_spell(level,vch,DAMTYPE(sn)))
		{
			damage_spell(ch,vch,dam/2,sn,DAMTYPE(sn),true);
		}
		else
		{
			damage_spell(ch,vch,dam,sn,DAMTYPE(sn),true);
		}
    }
	return FULL_MANA;
}


/****************************************************************************
 *  shadow_breath by Rathern                                                *
 ****************************************************************************/
SPRESULT spell_shadow_breath( int sn, int level, char_data *ch, void *,int )
{
    char_data *vch;
    char_data *vch_next;
    int dam, hp_dam, dice_dam, hpch, dam2;
    int temp_hps;

    act("$n breathes out a cloud of blackness!",ch,NULL,NULL,TO_ROOM);
    act("You breath out a cloud of blackness.",ch,NULL,NULL,TO_CHAR);

    hpch = UMAX(16,ch->hit);
    hp_dam = number_range(hpch/15+1,8);
    dice_dam = dice(level,12);

    dam = UMAX(hp_dam + dice_dam/10,dice_dam + hp_dam/10);

    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
        vch_next = vch->next_in_room;

        if (is_safe_spell(ch,vch,true)
            ||  (IS_NPC(ch) && IS_NPC(vch) 
            &&   (ch->fighting == vch || vch->fighting == ch)))
            continue;

        if(ch==vch)
        {
            ch->println("You cannot target yourself!");
            return NO_MANA;
        }

        /* vampiric touch */
        dam2=dice(level, 7)/abs(4+ch->alliance);
    
        if(saves_spell(level, vch, DAMTYPE(sn)))
        {
            dam2 /= 2;
        }

        temp_hps=vch->hit; // backup the current hitpoints
        if (damage_spell(ch, vch, dam2, sn, DAMTYPE(sn), true)) // do the damage
		{
			if(vch->hit<-10)
			{
				vch->hit=-10;
			}
    
			// gives caster victims hps
			if(vch!=NULL)
			{
			   ch->hit+= temp_hps-vch->hit;
			}   

		   // energy drain   
			if ( saves_spell( level, vch,DAMTYPE(sn)) )
			{
				vch->println("You feel a momentary chill.");
				return FULL_MANA;
			}
       
			if ( vch->level <= 2 )
			{
				dam2     = ch->hit + 1;
			}
			else
			{
				gain_exp( vch, 0 - number_range( level/2, 3 * level / 2 ) );
				vch->mana    /= 2;
				vch->move    /= 2;
				dam2      = dice(1, level/4);
    
				vch->println("You feel your life slipping away!!!");
				ch->println("Wow....what a rush!");
			}

			// If they died from an attack up above dont kill them again
			if (vch->pksafe==30) 
				return FULL_MANA;

			// died from last attack, or got out of room for some other reason
			if(vch->in_room!=ch->in_room){
				continue;
			}

			temp_hps=vch->hit;

			damage_spell( ch, vch, dam, sn, DAMTYPE(sn) ,true);

			if(vch->hit<-10) vch->hit=-10;
			{
				ch->hit+=temp_hps-vch->hit;
			}
			   
			// If they died from an attack up above dont kill them again
			if (vch->pksafe==30) 
				return FULL_MANA;

			// died from last attack, or got out of room for some other reason
			if(vch->in_room!=ch->in_room){
				continue;
			}
			if (saves_spell(level,vch,DAMTYPE(sn)))
			{
				damage_spell(ch,vch,dam/2,sn,DAMTYPE(sn),true);
			}
			else
			{
				damage_spell(ch,vch,dam,sn,DAMTYPE(sn),true);
			}
		}
    }
	return FULL_MANA;
}


/****************************************************************************
 *  element_ring by Kalahn & Rathern                                        *
 ****************************************************************************/
SPRESULT spell_element_ring( int , int level, char_data *ch, void *vo, int target)
{
    // negative effects of ring
    ch->println("You call on the power of the ring!");

    // spells stored in ring
    spell_animal_essence(skill_lookup("animal essence"), level, ch, ch, target);
    spell_earthquake(gsn_earthquake, level, ch, NULL, TARGET_NONE);
    spell_frostball( gsn_frostball, level, ch, vo, target);
    spell_downdraft( gsn_downdraft, level, ch, vo, target);
    spell_fireball(  gsn_fireball, level, ch, vo, target);

    if(!IS_NPC(ch) && ch->level<LEVEL_IMMORTAL)
	{
		ch->println("You feel tired.");
		act("$n looks drained and tired.",ch,NULL,NULL,TO_ROOM);
		ch->pcdata->tired += 35;
	}
	return FULL_MANA;
}

/****************************************************************************
 *  spell_summon_vyr - by Rathern - September 97                            *
 ****************************************************************************/

/* stay as i have set it.  Multiple copies of the mobs shouldnt be able to be created, at 
 the moment they can.

 If you ae feeling really wounderful, the statue can only be used
 once every 24 mud hours, if you can somehow do that then you can remove the negative 
 effects of using the statue, those effects are just an increase in the tired counter.

 i remarked the negative effects part cause it was crashing the spell,
 can you also have a look at that :) 
*/
SPRESULT spell_summon_vyr( int , int , char_data *ch, void *, int )
{
    char_data *mob_vyr, *pMobCheck;
    MOB_INDEX_DATA *pMobIndex;
	int vtimer;
    char buf[MSL];


    if (number_bits(1))
	{	// good
		pMobIndex = get_mob_index(MOB_VNUM_VYR_GOOD);

		// ensure good mob exists
		if (!pMobIndex) 
		{
			sprintf(buf,"BUG: in spell_summon_vyr - missing host mob!!! <vnum %d>\r\n",
				MOB_VNUM_VYR_GOOD);
			wiznet(buf,NULL,NULL,WIZ_BUGS,0,AVATAR); // put it on the bug wiznet channel
			log_string( buf ); // log the bug in the logs 
			do_bug ( ch, buf); // report the bug into the bug file
			ch->print( buf );
			return NO_MANA;
		}

		vtimer = ch->level/3;	// set how long the good mob will last
	}
	else // bad
	{
		pMobIndex = get_mob_index(MOB_VNUM_VYR_BAD);

		// ensure bad mob exists
		if (!pMobIndex) 
		{
			sprintf(buf,"BUG: in spell_summon_vyr - missing host mob!!! <vnum %d>\r\n",
				MOB_VNUM_VYR_BAD);
			wiznet(buf,NULL,NULL,WIZ_BUGS,0,AVATAR); // put it on the bug wiznet channel
			log_string( buf ); // log the bug in the logs 
			do_bug ( ch, buf); // report the bug into the bug file
			ch->print( buf );
			return NO_MANA;
		}

		vtimer = ch->level/10; // set how long the bad mob will last
	}

	// check for another vyr already in room
	pMobCheck = get_char_room( ch, pMobIndex->player_name);
	if ( pMobCheck ) 
	{
		ch->println("Nothing happens.");
		act("$n looks drained and tired.",ch,NULL,NULL,TO_ROOM);
        if (!IS_NPC(ch))
		{
    		ch->pcdata->tired += 20;
        }
		return FULL_MANA;
	}
    
	mob_vyr = create_mobile( pMobIndex, 0 ); // create Vyr mobile 
    mob_vyr->level = UMAX(ch->level-10,1);// set mobs level
    mob_vyr->max_hit = ch->level*10;      // set mobs max hit points
    mob_vyr->hit = mob_vyr->max_hit*2/3;  // set mobs current hit points to a third its total
    mob_vyr->timer = vtimer;			  // set how long the mob will last
    char_to_room( mob_vyr, ch->in_room);  // send mob to summoner

   // negative effects of statue
	ch->println("You call on the power of the statue!.");
	ch->println("You feel tired.");
	act("$n looks drained and tired.",ch,NULL,NULL,TO_ROOM);
    if (!IS_NPC(ch)){
    	ch->pcdata->tired += 30;
    }

    act("As $n brandishes the statuette a form appears in the room!", ch,NULL,NULL,TO_ROOM);
    ch->println("A shimmering form appears in the room.");  

	return FULL_MANA;
}    

/**************************************************************************/
// - Kal April 98
SPRESULT spell_summon_justice( int sn, int , char_data *ch, void *, int )
{
	OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *pObj;
    char buf[MSL];
	int tempwizi;

	// can't be cast by mobs
	if (IS_NPC(ch))
	{
		ch->println("Players only sorry.");
		return NO_MANA;
	}

	// check the spell hasn't been cast within the laston week
	if ((current_time - (7*24*60*60)) < ch->pcdata->last_used[sn])
	{
		ch->println("Nothing happens.");

		sprintf (buf, "%s cast summon justice, was declined last used %s ago.\r\n",
			ch->name, timediff(current_time, ch->pcdata->last_used[sn]) );
        wiznet(buf,NULL,NULL,WIZ_SECURE,0,AVATAR); // put it on the secure channel 
		log_string( buf ); // log the bug in the logs 
		return NO_MANA;
	}

	// check object exists 
	pObjIndex = get_obj_index(OBJ_VNUM_SUMMON_JUSTICE);

    if (!pObjIndex) // object with the vnum not found
    {
        sprintf(buf,"BUG: in spell_summon_justice - missing object vnum %d!\r\n", OBJ_VNUM_SUMMON_JUSTICE);
        wiznet(buf,NULL,NULL,WIZ_BUGS,0,AVATAR); // put it on the bug wiznet channel
        log_string( buf ); // log the bug in the logs 
        do_bug ( ch, buf); // report the bug into the bug file
        ch->print( buf ); // tell the char what happened
        return NO_MANA;
    }

	// record the last time the spell was used
	ch->pcdata->last_used[sn] = current_time; 

	//  make the object
    pObj= create_object(pObjIndex);
    obj_to_char(pObj, ch);

	pObj->timer = 30+(ch->level/5);

	// tell the room what happened    
	act("A bright light surrounds $n\r\n"
	"Leaving $p in $s hand!",ch,pObj,NULL,TO_ROOM);
    ch->println("A flash of lightening fills the room,");
	act("Then from nowhere a $p appears!",ch,pObj,NULL,TO_CHAR);

	// put the object in characters hand (use wizi to hide some of it)
	tempwizi = INVIS_LEVEL(ch);
	INVIS_LEVEL(ch)= LEVEL_IMMORTAL;
	remove_obj( ch, WEAR_WIELD, true ); // remove their current object
	wear_obj( ch, pObj, true, false );	// wield the justice object
	INVIS_LEVEL(ch)= tempwizi;

	return FULL_MANA;
}

/**************************************************************************/
// Rathern April 98
SPRESULT spell_thorny_feet( int sn, int level, char_data *ch, void *vo, int )
{
    char_data *victim = (char_data *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
    {
        if (victim == ch){
          ch->println("You have that spell on you already.");
		}else{
          act("$N appears to already be afflicted with thorns.", ch,NULL,victim,TO_CHAR);
		}
        return HALF_MANA;
    }

    if (saves_spell(level,victim,DAMTYPE(sn)))
    {
        ch->println("They seem unaffected.");
        return HALF_MANA;
    }   

    af.where     = WHERE_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level/20 + 1;
    af.modifier  = -5000;
    af.location  = APPLY_MOVE;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    victim->println("Your legs and feet are covered with thorns, you can't MOVE!");
    act("$n suddenly appears to be covered with thorns.",victim,NULL,NULL,TO_ROOM);
    return FULL_MANA;
}

/**************************************************************************/
SPRESULT spell_unholy_aura( int sn, int level, char_data *ch, void *vo, int )
{
	char_data *victim = (char_data *) vo;
	AFFECT_DATA af;

	if ( !IS_IMMORTAL(ch) && (!IS_EVIL(victim) || !IS_EVIL(ch)))
	{
		if(ch == victim)
		{
			ch->println("You are not unholy enough to cast this spell.");
			return NO_MANA;
		}	
		ch->println("They are too righteous!");
		return HALF_MANA;
	}

	if(!is_affected(victim,sn))
	{
		af.where      = WHERE_AFFECTS;
		af.type		  = sn;
		af.level	  = level;
		af.duration	  = level/5;
		af.location	  = APPLY_AC;
		af.bitvector  = 0;
		af.modifier	  = -level;
		affect_to_char( victim, &af);
		af.where	  = WHERE_RESIST;
		af.modifier	  = 0;
		af.location	  = APPLY_NONE;
		af.bitvector  = RES_HOLY;
		affect_to_char( victim, &af);
		victim->println("You are surrounded by a red aura.");
		act("$n is surrounded with a red aura.", ch, NULL, victim, TO_NOTVICT);
	}
	else
	{
		if(ch == victim)
			ch->println("You are already protected by divine magic.");
		else
			ch->println("They are already protected.");
        return HALF_MANA;
	}

    return FULL_MANA;
}
/**************************************************************************/
SPRESULT spell_despair( int sn, int level, char_data *ch, void *vo, int )
{
 	char_data *victim = (char_data *) vo;
 	AFFECT_DATA af;
 
 	victim->println("A wave of hopelessness & despair passes over you.");
 
 	if (victim->fighting || victim->position == POS_FIGHTING)
 		stop_fighting (victim, false);
 
 	af.where     = WHERE_AFFECTS2;
 	af.type      = sn;
 	af.level     = level;
 	af.duration  = level / 2;
 	af.location  = APPLY_ST;
 	af.modifier = -1 * (level / 5);
 	af.bitvector = AFF2_DESPAIR;
 	affect_to_char (victim, &af);
 
 	af.where = WHERE_AFFECTS2;
 	af.type = sn;
 	af.level = level;
 	af.duration = level / 2;
 	af.location = APPLY_HITROLL;
 	affect_to_char (victim, &af);
 	af.modifier = -15;
 	af.bitvector = AFF2_DESPAIR;
 	af.location = APPLY_DAMROLL;
 	affect_to_char (victim, &af);
 
 	act( "$n falls to the ground in utter despair and hopelessness.", victim, NULL, NULL, TO_ROOM );
 	return FULL_MANA;
}
 
SPRESULT spell_power_word_stun( int sn, int level, char_data *ch, void *vo, int )
{
     char_data *victim = (char_data *) vo;
     AFFECT_DATA af;
 
     if (is_affected(victim, sn) )
     {
 	ch->printlnf("They are already stunned from another word of power.");
         return NO_MANA;
     }
 
     if (victim == ch)
     {
     ch->printlnf("You can not direct a word of power at yourself.");
     return NO_MANA;
     }
 
     act("`#`Y$n gestures at $N and utters the word, `R'Stun'`^.",ch,NULL,victim,TO_NOTVICT);
     act("`#`Y$n gestures at you and utters the word, `R'Stun'`^.",ch,NULL,victim,TO_VICT);
     act("`#`YYou gesture at $N and invoke a word of `Sdark power.`^",ch,NULL,victim,TO_CHAR);
 	 
	 if(IS_IMMORTAL(victim))
	 {
		 act("Such foolishness.. this attacking of the gods.", ch, NULL, victim, TO_CHAR);
		 act("Wonder which is better, $s confusion or $s disbelief?", ch, NULL, victim, TO_VICT);
		 return FULL_MANA;
	 }

     if (saves_spell(level+2,victim,DAM_NEGATIVE) )
     {
 	act("`#`W$n reels from the `Sdark energy`W but resists the power.`^",victim,NULL,NULL,TO_ROOM);
  	act("`#`WYou feel the `Sdark energy`W blast at you, but you resist the stun.`^",victim,NULL,NULL,TO_CHAR);
 	return HALF_MANA;
     }
	
     af.where = WHERE_AFFECTS;
     af.type = sn;
     af.location = APPLY_HITROLL;
     af.modifier = -4;
     af.duration = 1;
     af.bitvector = 0;
     af.level = level;
     affect_to_char(victim,&af);
     af.location = APPLY_QU;
     af.modifier = -5;
     affect_to_char(victim,&af);
 
     act("`#The `Wword of power`^ sends $N to the ground with `Cstunning force!`^",ch,NULL,victim,TO_NOTVICT);
     act("`#`W$N is crushed to the floor by your `Cword of power!`^",ch,NULL,victim,TO_CHAR);
     victim->printlnf("`#`WYou are sent crashing to the ground as the `Cword`^ stuns you!`^");
     victim->position = POS_STUNNED;
     return FULL_MANA;
 }
 
SPRESULT spell_power_word_kill( int sn, int level, char_data *ch, void *vo, int )
{
 	char_data *victim = (char_data *) vo;
     	int dam;
 
    if(ch->race != race_lookup("beholder") )
    {
     	act("`#$n points a finger at $N and utters the word, `R'Die'`^.",ch,NULL,victim,TO_NOTVICT);
	     act("`#$n points a finger at you and utters the word, `R'Die'`^.",ch,NULL,victim,TO_VICT);
     	ch->println("You intone a word of unholy power.");
    }
     if(IS_SET(victim->imm_flags,IMM_NEGATIVE))
 	return HALF_MANA;
	 //Ooooo ego deflation time
	 if(IS_VAMPIRE(victim)
	||	IS_IMMORTAL(victim)
	|| IS_SET(victim->form, FORM_UNDEAD)
	||	saves_spell(level/2,victim,DAMTYPE(sn)))
     {
		ch->println("They somehow they seem to be unaffected.");
    	dam = dice(level,14);
     	dam /= 3;
     	damage(ch,victim,dam,sn,DAMTYPE(sn),true);
     	return FULL_MANA;
     }
 
 	act("`#`Y$N shudders in shock as $S heart explodes!`^", ch, NULL,victim,TO_NOTVICT);
 	victim->println("You feel your heart rupture in a violent explosion of pain!");
    if(ch->race != race_lookup("beholder") )
    {
 	act("`#`YYour word of power vaporises $N's heart, killing $M instantly!`^",ch,NULL,victim,TO_CHAR);
    }
    if(ch->race == race_lookup("beholder") )
    {
 	act("`#`YYour death ray vaporises $N's heart, killing $M instantly!`^",ch,NULL,victim,TO_CHAR);
    }

 	victim->position = POS_DEAD;
 
 
 // Arena Code by IXLIAM
 
  if (victim->position == POS_DEAD && !IS_NPC(victim) && !IS_NPC(ch)
     && (IS_SET(victim->in_room->room2_flags,ROOM2_ARENA)))
     {
 
     /*
     ch->pcdata->akills += 1;
     victim->pcdata->adeaths += 1; 
     */
 
     /* the dead man */
     stop_fighting(victim,true);
     char_from_room(victim);
     char_to_room(victim,get_room_index(50019));
     victim->println("`#`RYou have `YLOST!!!`^");
     affect_strip(ch,gsn_plague);
     affect_strip(ch,gsn_despair);
     affect_strip(ch,gsn_poison);
     affect_strip(ch,gsn_blindness);
     affect_strip(ch,gsn_sleep);
     affect_strip(ch,gsn_curse);
     affect_strip(ch,gsn_cause_fear);
     affect_strip(ch,gsn_fear_magic);
     affect_strip(ch,gsn_vanish);
     affect_strip(ch,gsn_neck_thrust);
     affect_strip(ch,gsn_charm_person);
     affect_strip(ch,gsn_pine_needles);
     affect_strip(ch,gsn_wrath);
     victim->move = victim->max_move;
     victim->hit = victim->max_hit;
     victim->mana = victim->max_mana;
     update_pos( victim );
     do_look(victim, "auto");
 
     /* the winner :) */
     stop_fighting(ch,true);
     char_from_room(ch);
     char_to_room(ch,get_room_index(50000));
     ch->println("`#`RYou have `YWON!!!!`^");
     info_broadcast(victim, "%s defeated %s in the Arena.", ch->name, victim->name);
     affect_strip(ch,gsn_wrath);
     affect_strip(ch,gsn_despair);
     affect_strip(ch,gsn_plague);
     affect_strip(ch,gsn_poison);
     affect_strip(ch,gsn_blindness);
     affect_strip(ch,gsn_sleep);
     affect_strip(ch,gsn_curse);
     affect_strip(ch,gsn_cause_fear);
     affect_strip(ch,gsn_fear_magic);
     affect_strip(ch,gsn_vanish);
     affect_strip(ch,gsn_neck_thrust);
     affect_strip(ch,gsn_charm_person);
     affect_strip(ch,gsn_pine_needles);
     ch->hit = ch->max_hit;
     ch->mana = ch->max_mana;
     ch->move = ch->max_move;
     update_pos( ch );
     do_look(ch, "auto");
   
     if (IS_SET(ch->act2,ACT2_PLR_CHALLENGER))
     REMOVE_BIT(ch->act2,ACT2_PLR_CHALLENGER);
     if (IS_SET(victim->act2,ACT2_PLR_CHALLENGER))
     REMOVE_BIT(victim->act2,ACT2_PLR_CHALLENGER);
     if (IS_SET(victim->act2,ACT2_PLR_CHALLENGED))
     REMOVE_BIT(victim->act2,ACT2_PLR_CHALLENGED);
     if (IS_SET(ch->act2,ACT2_PLR_CHALLENGED))
     REMOVE_BIT(ch->act2,ACT2_PLR_CHALLENGED);
     arena_is_busy = false;
     return NO_MANA;
     }
 
 // End of Arena Code
 
 
 // War Battlefield Code by IXLIAM
 
  if (victim->position == POS_DEAD && !IS_NPC(victim) && !IS_NPC(ch)
     && (IS_SET(victim->in_room->room2_flags,ROOM2_WAR)))
     {
 
     /* the dead man */
     stop_fighting(victim,true);
     char_from_room(victim);
     char_to_room(victim,get_room_index(50020));
     victim->println("`#`YYou have been `RKILLED `Yand removed from battle !`^");
 
     OBJ_DATA  	*loser;
     loser = create_object(get_obj_index(50097)); // Loser timed token
     loser->timer = 8; // Number of ticks they cannot battle
     obj_to_char( loser, victim ); // Send token to loser
     
     if (( loser = get_obj_token( ch, "50098")) != NULL)
 	{ 
 	   obj_from_char (loser);
 	   extract_obj (loser);
 	}
 	
     if (( loser = get_obj_token( ch, "50099")) != NULL)
 	{ 
 	   obj_from_char (loser);
 	   extract_obj (loser);
 	}
 
     affect_strip(ch,gsn_wrath);
     affect_strip(ch,gsn_plague);
     affect_strip(ch,gsn_despair);
     affect_strip(ch,gsn_poison);
     affect_strip(ch,gsn_blindness);
     affect_strip(ch,gsn_sleep);
     affect_strip(ch,gsn_curse);
     affect_strip(ch,gsn_cause_fear);
     affect_strip(ch,gsn_fear_magic);
     affect_strip(ch,gsn_vanish);
     affect_strip(ch,gsn_neck_thrust);
     affect_strip(ch,gsn_charm_person);
     affect_strip(ch,gsn_pine_needles);
     victim->hit = victim->max_hit;
     victim->mana = victim->max_mana;
     victim->move = victim->max_move;
     update_pos( victim );
     do_look(victim, "auto");
     if (IS_SET(victim->act2,ACT2_WAR_BLUE)) 
 	{
 	   info_broadcast(victim, "`#`Y[`BBLUE TEAM`Y]`W %s has been killed in the Battlefield.", victim->name);
     	   REMOVE_BIT(victim->act2,ACT2_WAR_BLUE);
 	}
     if (IS_SET(victim->act2,ACT2_WAR_RED))
 	{
 	   info_broadcast(victim, "`#`Y[`RRED TEAM`Y]`W %s has been killed in the Battlefield.", victim->name);
    	   REMOVE_BIT(victim->act2,ACT2_WAR_RED);
 	}
      return NO_MANA;
     }
 
 
     	kill_char(victim, ch); 
    	return FULL_MANA;
 
     
 
}
 
SPRESULT spell_hallucinate( int sn, int level, char_data *ch, void *vo, int )
{
     char_data *victim = (char_data *) vo;
     AFFECT_DATA af;
 
     if ( IS_AFFECTED2( victim, AFF2_HALLUCINATE ) )
     {
 	ch->println("They are already hallucinating.");
 	return NO_MANA;
     }
     
     if ( IS_AFFECTED( victim, AFF_BLIND ) || saves_spell( level, victim, DAM_MENTAL ))
     {
 	ch->println("You have failed.");
 	return HALF_MANA;
     }
 
     if (IS_SET(victim->in_room->room2_flags,ROOM2_ARENA))
     {
 	ch->println("You spell fizzles and dies.");
 	return NO_MANA;
     }
 
     if (IS_SET(victim->in_room->room2_flags,ROOM2_WAR))
     {
 	ch->println("You spell fizzles and dies.");
 	return NO_MANA;
     }
     af.where = WHERE_AFFECTS;
     af.type = sn;
     af.level	 = level;
     af.duration  = number_fuzzy( level / 6 );
     af.location  = APPLY_IN;
     af.modifier  = -4;
     af.bitvector = 0;
     affect_to_char(victim,&af);
     af.where = WHERE_AFFECTS2;
     af.bitvector = AFF2_HALLUCINATE;
     affect_to_char(victim,&af);
 
     act("`#$N's body is `?su`?rr`?ou`?nd`?ed `?by `?da`?nc`?in`?g l`?ig`?hts`^.",ch,NULL,victim,TO_ROOM);
     victim->printlnf("`#`W`?Thous`?ands `?of `?danc`?ing `?lig`?hts `?surr`?ound `?you.`^");
     return FULL_MANA;
}

SPRESULT spell_harmonize( int sn, int level, char_data *ch, void *vo, int )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;

    if( !IS_SET( obj->extra_flags, OBJEXTRA_EVIL )
	&& !IS_SET( obj->extra_flags, OBJEXTRA_ANTI_GOOD )
	&& !IS_SET( obj->extra_flags, OBJEXTRA_ANTI_EVIL )
	&& !IS_SET( obj->extra_flags, OBJEXTRA_ANTI_NEUTRAL ) 
	&& !IS_SET( obj->extra2_flags, OBJEXTRA2_ANTI_LAW )
	&& !IS_SET( obj->extra2_flags, OBJEXTRA2_ANTI_CHAOS )
	&& !IS_SET( obj->extra2_flags, OBJEXTRA2_ANTI_BALANCE ) )
    {
	ch->printlnf( "Nothing happens." );
	return NO_MANA;
    }

    if( !IS_NPC( ch ) && number_percent( ) < ch->pcdata->learned[sn] *
	( 33 +
	 ( 33 * ( ch->level - obj->level ) / (float)LEVEL_HERO ) ) / 100.0 )
    {
	REMOVE_BIT( obj->extra_flags, OBJEXTRA_EVIL );
	REMOVE_BIT( obj->extra_flags, OBJEXTRA_ANTI_GOOD );
	REMOVE_BIT( obj->extra_flags, OBJEXTRA_ANTI_EVIL );
	REMOVE_BIT( obj->extra_flags, OBJEXTRA_ANTI_NEUTRAL );
	REMOVE_BIT( obj->extra2_flags, OBJEXTRA2_ANTI_LAW );
	REMOVE_BIT( obj->extra2_flags, OBJEXTRA2_ANTI_CHAOS );
	REMOVE_BIT( obj->extra2_flags, OBJEXTRA2_ANTI_BALANCE );
	act( "$p hums briefly, then lies quiet.", ch, obj, NULL, TO_CHAR );
	act( "$p hums briefly, then lies quiet.", ch, obj, NULL, TO_ROOM );
	return FULL_MANA;
    }

    obj->wear_flags = OBJWEAR_TAKE;	/* Useless   */
    obj->cost = 0;			/* Worthless */
    obj->timer = 1;
    act( "$p blazes brightly, then turns grey.", ch, obj, NULL, TO_CHAR );
    act( "$p blazes brightly, then turns grey.", ch, obj, NULL, TO_ROOM );
    return HALF_MANA;
}

SPRESULT spell_animate_dead( int sn, int level, char_data *ch, void *vo, int )
{
    OBJ_DATA *obj, *cor;
    int i;

    obj = get_obj_here( ch, target_name );

    if(IS_GOOD(ch))
    {
        ch->printlnf( "Your soul is too pure to use such magic.");
        return NO_MANA;
    }

    if ( obj == NULL )
    {
        ch->printlnf( "Animate what ?");
        return NO_MANA;
    }

    /* Nothing but NPC corpses. */

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
	
    if (chance == 1)
    	mob = create_mobile( get_mob_index( 72 ), 0 ); // Zombie
    if (chance == 2)
    	mob = create_mobile( get_mob_index( 73 ), 0 ); // Skeleton
    if (chance == 3)
    	mob = create_mobile( get_mob_index( 74 ), 0 ); // Shade
    if (chance == 4)
    	mob = create_mobile( get_mob_index( 75 ), 0 ); // Wraith

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

    /* You rang? */
    char_to_room( mob, ch->in_room );
    if (chance == 1)
    {
    	act( "$p springs to life as a hideous zombie!", ch, obj, NULL, TO_ROOM );
    	act( "$p springs to life as a hideous zombie!", ch, obj, NULL, TO_CHAR );
    }

    if (chance == 2)
    {
    	act( "$p springs to life as a skeleton warrior!", ch, obj, NULL, TO_ROOM );
    	act( "$p springs to life as a skeleton warrior!", ch, obj, NULL, TO_CHAR );
    }

    if (chance == 3)
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

SPRESULT spell_turn_undead( int sn, int level, char_data *ch, void *vo, int )
{
    char_data *victim = (char_data *)vo;

    if(IS_EVIL(ch))
    {

    	if(!IS_SET(victim->act, ACT_UNDEAD) && !IS_SET(victim->form, FORM_UNDEAD) &&
       	   !IS_VAMPIRE(victim) && victim->race != race_lookup("lich"))
    	{
		ch->printlnf( "You have failed.");
		return NO_MANA;
    	}
	if(!IS_NPC(victim))
	{
		ch->printlnf( "You cannot control them!");
		return NO_MANA;
	}

     	act("$n thrusts the symbol of their deity at $N.",ch,NULL,victim,TO_NOTVICT);
     	act("$n thrusts the symbol of their deity at you.",ch,NULL,victim,TO_VICT);

    	if( victim->level >= level
		|| saves_spell( level, victim, DAMTYPE(sn) ) )
	{
		ch->printlnf( "You have failed.");
		return FULL_MANA;
    	}

	stop_fighting( victim, true );
    	SET_BIT(victim->affected_by, AFF_CHARM);
    	victim->comm = COMM_NOTELL|COMM_NOSHOUT|COMM_NOCHANNELS;
    	add_follower( victim, ch );
    	victim->leader = ch;
    	ch->pet = victim;
    	do_say( victim, "Yessssss, massssssster...." );

	    if( ch->fighting )
	    {
		victim->position = POS_FIGHTING;
		victim->fighting = ch->fighting;
	    }

	return FULL_MANA;	
    }

    if( victim->level >= level
	|| saves_spell( level, victim, DAMTYPE(sn) ) )
    {
	ch->printlnf( "You have failed.");
	return FULL_MANA;
    }

    if(!IS_SET(victim->act, ACT_UNDEAD) && !IS_SET(victim->form, FORM_UNDEAD) &&
       !IS_VAMPIRE(victim) && victim->race != race_lookup("lich"))
    {
	ch->printlnf( "You have failed.");
	return NO_MANA;
    }

     	act("$n thrusts the symbol of their deity at $N.",ch,NULL,victim,TO_NOTVICT);
     	act("$n thrusts the symbol of their deity at you.",ch,NULL,victim,TO_VICT);

     	if (saves_spell(level/2,victim,DAMTYPE(sn)))
     	{
    		act( "$n screams and runs away.", victim, NULL, NULL, TO_ROOM );
		stop_fighting( victim, true );
    		do_fearful(victim);
     		return FULL_MANA;
     	}
 
 	act("`#`Y$N is suddenly destroyed !`^", ch, NULL,victim,TO_NOTVICT);
 	victim->println("You feel your very essence destroyed in a violent explosion of pain!");
 	act("`#`YYour divine power destroys $N, killing $M instantly!`^",ch,NULL,victim,TO_CHAR);
 	victim->position = POS_DEAD;
 
 // Arena Code by IXLIAM
 
  if (victim->position == POS_DEAD && !IS_NPC(victim) && !IS_NPC(ch)
     && (IS_SET(victim->in_room->room2_flags,ROOM2_ARENA)))
     {
 
     /*
     ch->pcdata->akills += 1;
     victim->pcdata->adeaths += 1; 
     */
 
     /* the dead man */
     stop_fighting(victim,true);
     char_from_room(victim);
     char_to_room(victim,get_room_index(50019));
     victim->println("`#`RYou have `YLOST!!!`^");
     affect_strip(ch,gsn_plague);
     affect_strip(ch,gsn_despair);
     affect_strip(ch,gsn_poison);
     affect_strip(ch,gsn_blindness);
     affect_strip(ch,gsn_sleep);
     affect_strip(ch,gsn_curse);
     affect_strip(ch,gsn_cause_fear);
     affect_strip(ch,gsn_fear_magic);
     affect_strip(ch,gsn_vanish);
     affect_strip(ch,gsn_neck_thrust);
     affect_strip(ch,gsn_charm_person);
     affect_strip(ch,gsn_pine_needles);
     affect_strip(ch,gsn_wrath);
     victim->move = victim->max_move;
     victim->hit = victim->max_hit;
     victim->mana = victim->max_mana;
     update_pos( victim );
     do_look(victim, "auto");
 
     /* the winner :) */
     stop_fighting(ch,true);
     char_from_room(ch);
     char_to_room(ch,get_room_index(50000));
     ch->println("`#`RYou have `YWON!!!!`^");
     info_broadcast(victim, "%s defeated %s in the Arena.", ch->name, victim->name);
     affect_strip(ch,gsn_wrath);
     affect_strip(ch,gsn_despair);
     affect_strip(ch,gsn_plague);
     affect_strip(ch,gsn_poison);
     affect_strip(ch,gsn_blindness);
     affect_strip(ch,gsn_sleep);
     affect_strip(ch,gsn_curse);
     affect_strip(ch,gsn_cause_fear);
     affect_strip(ch,gsn_fear_magic);
     affect_strip(ch,gsn_vanish);
     affect_strip(ch,gsn_neck_thrust);
     affect_strip(ch,gsn_charm_person);
     affect_strip(ch,gsn_pine_needles);
     ch->hit = ch->max_hit;
     ch->mana = ch->max_mana;
     ch->move = ch->max_move;
     update_pos( ch );
     do_look(ch, "auto");
   
     if (IS_SET(ch->act2,ACT2_PLR_CHALLENGER))
     REMOVE_BIT(ch->act2,ACT2_PLR_CHALLENGER);
     if (IS_SET(victim->act2,ACT2_PLR_CHALLENGER))
     REMOVE_BIT(victim->act2,ACT2_PLR_CHALLENGER);
     if (IS_SET(victim->act2,ACT2_PLR_CHALLENGED))
     REMOVE_BIT(victim->act2,ACT2_PLR_CHALLENGED);
     if (IS_SET(ch->act2,ACT2_PLR_CHALLENGED))
     REMOVE_BIT(ch->act2,ACT2_PLR_CHALLENGED);
     arena_is_busy = false;
     return NO_MANA;
     }
 
 // End of Arena Code
 
 
 // War Battlefield Code by IXLIAM
 
  if (victim->position == POS_DEAD && !IS_NPC(victim) && !IS_NPC(ch)
     && (IS_SET(victim->in_room->room2_flags,ROOM2_WAR)))
     {
 
     /* the dead man */
     stop_fighting(victim,true);
     char_from_room(victim);
     char_to_room(victim,get_room_index(50020));
     victim->println("`#`YYou have been `RKILLED `Yand removed from battle !`^");
 
     OBJ_DATA  	*loser;
     loser = create_object(get_obj_index(50097)); // Loser timed token
     loser->timer = 8; // Number of ticks they cannot battle
     obj_to_char( loser, victim ); // Send token to loser
     
     if (( loser = get_obj_token( ch, "50098")) != NULL)
 	{ 
 	   obj_from_char (loser);
 	   extract_obj (loser);
 	}
 	
     if (( loser = get_obj_token( ch, "50099")) != NULL)
 	{ 
 	   obj_from_char (loser);
 	   extract_obj (loser);
 	}
 
     affect_strip(ch,gsn_wrath);
     affect_strip(ch,gsn_plague);
     affect_strip(ch,gsn_despair);
     affect_strip(ch,gsn_poison);
     affect_strip(ch,gsn_blindness);
     affect_strip(ch,gsn_sleep);
     affect_strip(ch,gsn_curse);
     affect_strip(ch,gsn_cause_fear);
     affect_strip(ch,gsn_fear_magic);
     affect_strip(ch,gsn_vanish);
     affect_strip(ch,gsn_neck_thrust);
     affect_strip(ch,gsn_charm_person);
     affect_strip(ch,gsn_pine_needles);
     victim->hit = victim->max_hit;
     victim->mana = victim->max_mana;
     victim->move = victim->max_move;
     update_pos( victim );
     do_look(victim, "auto");
     if (IS_SET(victim->act2,ACT2_WAR_BLUE)) 
 	{
 	   info_broadcast(victim, "`#`Y[`BBLUE TEAM`Y]`W %s has been killed in the Battlefield.", victim->name);
     	   REMOVE_BIT(victim->act2,ACT2_WAR_BLUE);
 	}
     if (IS_SET(victim->act2,ACT2_WAR_RED))
 	{
 	   info_broadcast(victim, "`#`Y[`RRED TEAM`Y]`W %s has been killed in the Battlefield.", victim->name);
    	   REMOVE_BIT(victim->act2,ACT2_WAR_RED);
 	}
      return NO_MANA;
     }

     	kill_char(victim, ch); 
    	return FULL_MANA;
}

SPRESULT spell_destroy_cursed( int sn, int level, char_data *ch, void *vo, int )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char_data *victim = (char_data *)vo;
    bool yesno = false;

    for( obj = victim->carrying; obj; obj = obj_next )
    {
	obj_next = obj->next_content;

	if( (IS_SET( obj->extra_flags, OBJEXTRA_NODROP )
	    || IS_SET( obj->extra_flags, OBJEXTRA_NOREMOVE )) 
            && !IS_SET( obj->extra_flags, OBJEXTRA_NOUNCURSE )
	    && obj->wear_loc == WEAR_NONE)
	{
	    act( "You convulse as you toss $p to the ground, destroying it.",
		victim, obj, NULL, TO_CHAR );
	    act( "$n convulses as $e tosses $p to the ground, destroying it.",
		victim, obj, NULL, TO_ROOM );
	    extract_obj( obj );
	    yesno = true;
	}
    }

    if( ch != victim && yesno )
    {
	ch->printlnf( "Ok.");
    	return FULL_MANA;
    }

    ch->printlnf( "Nothing happens.");
    return HALF_MANA;

}

SPRESULT spell_summon_undead( int sn, int level, char_data *ch, void *vo, int )
{
    char_data *mob;
    int number = 1;
    int i, loop;

    act( "$n gestures dramatically and undead crawl up from the ground around $m.",
	 ch, NULL, NULL, TO_NOTVICT );
    for( i = 0; i < ch->level; i += 20 )
	if( number_bits( 1 ) == 0 )
	    number++;
    for( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
	if( IS_NPC( mob ) && mob->master == ch && number_bits( 2 ) != 0 )
	    number--;
    }
    if( number < -1 )
    {
	ch->printlnf( "There is too litle life force left in the room.");
	return NO_MANA;
    }

    number = number_range(1,10);
    MOB_INDEX_DATA *pMob = NULL;
    for( loop = 0; loop < number; loop++ )
    {

	int chance;
    	chance = number_range(1,4);
	switch( chance )
	{
	case 1:
	    pMob = get_mob_index( 72 );
	    break;
	case 2:
	    pMob = get_mob_index( 73 );
	    break;
	case 3:
	    pMob = get_mob_index( 74 );
	    break;
	case 4:
	    pMob = get_mob_index( 75 );
	    break;
	}
	mob = create_mobile( pMob, 0 );
	mob->level = ch->level - (loop * 12);
	if(mob->level < 1) mob->level = 1;
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

	char_to_room( mob, ch->in_room );

	act( "$n claws its way up from the ground!", mob, NULL, NULL, TO_ROOM );

    	SET_BIT(mob->affected_by, AFF_CHARM);
    	mob->comm = COMM_NOTELL|COMM_NOSHOUT|COMM_NOCHANNELS;
    	add_follower( mob, ch );
    	mob->leader = ch;

	    if( ch->fighting )
	    {
		mob->position = POS_FIGHTING;
		mob->fighting = ch->fighting;
	    }
    }
    return FULL_MANA;
}

SPRESULT spell_mass_plague( int sn, int level, char_data *ch, void *vo, int )
{
    char_data *vch;
    AFFECT_DATA af;
    af.where		= WHERE_AFFECTS;
    af.type		= sn;
    af.level		= level * 3/4;
    af.duration		= 20 + level / 10;
    af.location		= APPLY_ST;
    af.modifier		= -10;
    af.bitvector	= AFF_PLAGUE;

    for( vch = ch->in_room->people; vch; vch = vch->next_in_room )
    {
	if( IS_AFFECTED( vch, AFF_PLAGUE ) )
	    continue;

	if (saves_spell(level,vch,DAMTYPE(sn))
	|| (IS_NPC(vch) && IS_SET(vch->act,ACT_UNDEAD))
	|| HAS_CLASSFLAG(vch, CLASSFLAG_PLAGUE_IMMUNITY))
	{
		if (ch == vch){
			ch->printf( "You feel momentarily ill, but it passes." );
		}else{
			act("$N seems to be unaffected.",ch,NULL,vch,TO_CHAR);
		}
		continue;
	}

	if ( IS_ICIMMORTAL(vch)){
		act( "$n doesn't appear to be even slightly affected by your magic.", vch, NULL, NULL, TO_ROOM );
		continue;
	}

	affect_join(vch,&af);

	vch->printlnf( "You scream in agony as plague sores erupt from your skin." );
	act("$n screams in agony as plague sores erupt from $s skin.",vch,NULL,NULL,TO_ROOM);
    }
    return FULL_MANA;
}

SPRESULT spell_polymorph( int sn, int level, char_data *ch, void *vo, int )
{
    char_data *victim = (char_data *)vo;
    AFFECT_DATA af;
    MOB_INDEX_DATA *pMob = NULL;
    char      buf [MIL];

    if( IS_SET( victim->affected_by2, AFF2_POLYMORPH ) )
    {
	act( "$E is already polymorphed.", ch, NULL, victim, TO_CHAR );
	return NO_MANA;
    }

    if (victim->altformed == true)
    {
	if(victim == ch)
	   ch->println("Not in your present form.");
	else
	   act( "$E doesn't seem to be affected.", ch, NULL, victim, TO_CHAR );
	return NO_MANA;
    }

    if( IS_NPC(victim) )
    {
	ch->printlnf("Not on NPC's!");
	return NO_MANA;
    }

    int chance;
    chance = number_range(1,17);
    switch( chance )
    {
	case 1:
	    pMob = get_mob_index( 3019 );
	    break;
	case 2:
	    pMob = get_mob_index( 6066 );
	    break;
	case 3:
	    pMob = get_mob_index( 52021 );
	    break;
	case 4:
	    pMob = get_mob_index( 52007 );
	    break;
	case 5:
	    pMob = get_mob_index( 2485 );
	    break;
	case 6:
	    pMob = get_mob_index( 3010 );
	    break;
	case 7:
	    pMob = get_mob_index( 4404 );
	    break;
	case 8:
	    pMob = get_mob_index( 15722 );
	    break;
	case 9:
	    pMob = get_mob_index( 60000 );
	    break;
	case 10:
	    pMob = get_mob_index( 46101 );
	    break;
	case 11:
	    pMob = get_mob_index( 23629 );
	    break;
	case 12:
	    pMob = get_mob_index( 23605 );
	    break;
	case 13:
	    pMob = get_mob_index( 7717 );
	    break;
	case 14:
	    pMob = get_mob_index( 22721 );
	    break;
	case 15:
	    pMob = get_mob_index( 4407 );
	    break;
	case 16:
	    pMob = get_mob_index( 52031 );
	    break;
	default:
	    pMob = get_mob_index( 21802 );
	    break;
    }

    char_data *polymob;
    polymob 			= create_mobile( pMob, 0 );   
    victim->poly_short 		= polymob->short_descr;
    victim->poly_description 	= polymob->description;

    sprintf(buf, "$n suddenly changes into %s.", victim->poly_short );
    act(buf,victim,NULL,victim,TO_NOTVICT);

    af.where     = WHERE_AFFECTS2;
    af.type      = sn;
    af.level     = level;
    af.duration  = level + 12;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF2_POLYMORPH;
    affect_to_char( victim, &af );

    if(victim == ch)
	victim->printf( "You cloak yourself in the appearance of %s.", victim->poly_short );
    else
    {
	victim->printf( "You appearance suddenly changes into %s.", victim->poly_short );
	ch->printf( "%s suddenly changes into %s.", victim->short_descr, victim->poly_short );
    }

    return FULL_MANA;
}

SPRESULT spell_firestorm( int sn, int level, char_data *ch, void *vo, int )
{
    char_data *victim = (char_data *)vo;
	int        dam, i;

	for(i = 1; i < 4; i++)
	{
		dam		= dice(level, 6)+10;
		if ( saves_spell( level, victim, DAMTYPE(sn)))
			dam /= 2;
		

		if ( victim->in_room == ch->in_room )
		{
			act("`#`RDrops of `rfire `Brain `Rshower down upon `W$N!`^",ch,NULL,victim,TO_ROOM);
			act("`#`RDrops of `rfire `Brain `Rshower down upon `W$N!`^",ch,NULL,victim,TO_CHAR);
			damage_spell( ch, victim, dam, sn, DAMTYPE(sn),true);
		}
	}
	return FULL_MANA;
}

SPRESULT spell_hold_person( int sn, int level, char_data *ch, void *vo, int )
{
    	char_data *victim = (char_data *)vo;

	if ( saves_spell( level, victim, DAMTYPE(sn)))
	{
		act("$N is not affected!",ch,NULL,victim,TO_CHAR);
		return FULL_MANA;
	}

	if ( victim->in_room == ch->in_room )
	{
		act("$N suddenly stops moving!",ch,NULL,victim,TO_ROOM);
		act("$N suddenly stops moving!",ch,NULL,victim,TO_CHAR);
		act("You suddenly cannot move!",ch,NULL,victim,TO_VICT);
		stop_fighting(victim, true);
		stop_fighting(ch, true);

		AFFECT_DATA af;
     		af.where 	= WHERE_AFFECTS;
     		af.type 	= sn;
     		af.level	= level;
     		af.duration  	= 3;
     		af.location  	= APPLY_ST;
     		af.modifier  	= -20;
     		af.bitvector 	= 0;
     		affect_to_char(victim,&af);
	}
	return FULL_MANA;
}

SPRESULT spell_mark_rune( int sn, int level, char_data *ch, void *vo, int )
{
    OBJ_DATA *rune;
    ROOM_INDEX_DATA *location;

    location = ch->in_room;
    rune = get_eq_char(ch, WEAR_HOLD);
	
	if (	IS_SET(location->room_flags,		ROOM_NO_RECALL	)
		||  IS_SET(location->room_flags,	ROOM_NO_SUMMON	)
		||  IS_SET(location->room_flags,	ROOM_NO_PORTAL	)
		||  IS_SET(location->room_flags,	ROOM_ARENA	)
		||  IS_SET(location->room_flags,	ROOM_PET_SHOP	)
		||  IS_SET(location->room_flags,	ROOM_IMP_ONLY	)
		||  IS_SET(location->room_flags,	ROOM_GODS_ONLY	)
		||  IS_SET(location->room_flags,	ROOM_NEWBIES_ONLY)
		||  IS_SET(location->room_flags,	ROOM_NOWHERE	)
		||  IS_SET(location->room_flags,	ROOM_ANTIMAGIC	)
		||  IS_SET(location->room_flags,	ROOM_OOC	)
		||  IS_SET(location->room_flags,	ROOM_INN	)
		||  IS_SET(location->room_flags,	ROOM_PRIVATE	)
		||  IS_SET(location->room_flags,	ROOM_SOLITARY	))
	{
        ch->printlnf( "This is not the appropiate place to mark a runestone.");
        return NO_MANA;
    }   

    if (is_name( ch->in_room->area->file_name, "houses.are" ) )
    {
        ch->printlnf( "This is not the appropiate place to mark a runestone.");
        return NO_MANA;
    }

    if (rune == NULL || rune->item_type != ITEM_RUNE)
    {
        ch->printlnf("You need to hold a blank runestone for this spell to work.");
        return NO_MANA;
    }

    if (rune->value[0] == 1)
    {
	ch->printlnf("This runestone has already been marked.");
	return NO_MANA;
    }

    if (rune != NULL && rune->item_type == ITEM_RUNE)
    {
        act("You raise $p towards the sky.",ch,rune,NULL,TO_CHAR);
	act("$n raises $p towards the sky.",ch,rune,NULL,TO_ROOM);
        act("You invoke all your power to mark $p!",ch,rune,NULL,TO_CHAR);
	act("$n invokes all of their power to mark $p!",ch,rune,NULL,TO_ROOM);

    }

	rune->value[0] = 1;				/* Marking the rune */
	rune->value[1] = ch->perm_stats[STAT_RE]/5;	/* Number of charges */
	rune->value[2] = ch->in_room->vnum;		/* Destiny Vnum */

	char buf[MSL];

	sprintf( buf, "a runestone to %s", ch->in_room->name);
	free_string( rune->short_descr );
	rune->short_descr = str_dup( buf );

    	sprintf( buf, "A runestone to %s has been left here.", ch->in_room->name);
	free_string( rune->description );
	rune->description = str_dup( buf );
	return FULL_MANA;
}

SPRESULT spell_invoke_rune( int sn, int level, char_data *ch, void *vo, int )
{
	ROOM_INDEX_DATA *start;
	ROOM_INDEX_DATA *end;
	OBJ_DATA *rune;

	start	= ch->in_room;
    	rune = get_eq_char(ch, WEAR_NONE);

	if (IS_SET(start->room_flags, ROOM_NO_RECALL) ||
	    IS_SET(start->room_flags, ROOM_OOC) ||
	    IS_AFFECTED(ch, AFF_CURSE))
	{
        	ch->printlnf( "You have failed.");
        	return FULL_MANA;
    }   

    if (rune == NULL || rune->item_type != ITEM_RUNE)
    {
        ch->printlnf("The marked rune has to be in your inventory for this spell to work.");
        return NO_MANA;
    }

    if (rune->value[0] != 1)
    {
	ch->printlnf("This rune hasn't been marked yet.");
	return NO_MANA;
    }

	act("$n invokes the power of a rune to travel.",ch, NULL, NULL, TO_ROOM);
	act("$n dissapears before your eyes.",ch, NULL, NULL, TO_ROOM);
	act("You invoke the power of $p to travel.",ch, rune, NULL, TO_CHAR);
	end = get_room_index(rune->value[2]);
	char_from_room(ch);
	char_to_room(ch, end);
	rune->value[1] -= 1;
	act( "$n appears in the room.", ch, NULL, NULL, TO_ROOM );
	do_look(ch, "auto");

    if ( rune->value[1] == 0 ) /* When v1 reaches 0, the rune dissapears */
    {
	act( "$p explodes in many fragments.", ch, rune, NULL, TO_ROOM );
	act( "$p explodes in many fragments.", ch, rune, NULL, TO_CHAR );
	extract_obj(rune);
    }

    return FULL_MANA;
}
/**************************************************************************/
SPRESULT spell_wall_of_fire( int sn, int level, char_data *ch, void *vo, int )
{
	ROOM_INDEX_DATA *in_room;
	ROOM_INDEX_DATA *to_room = NULL;
	EXIT_DATA *pexit;
    	int door = *(int *)vo;

	in_room	= ch->in_room;

    	if (door==-1 || door==4 || door==5)
	{
        	ch->printlnf( "You have failed.");
        	return FULL_MANA;
    	} 

	if (IS_SET(in_room->room_flags, ROOM_OOC))
	{
        	ch->printlnf( "You have failed.");
        	return FULL_MANA;
    	}

	if ( ( pexit = in_room->exit[door] ) == NULL || ( to_room = pexit->u1.to_room   ) == NULL)
	{
        	ch->printlnf( "You have failed.");
        	return FULL_MANA;
    	}

	if (IS_SET(pexit->exit_info, EX_CLOSED))
	{
        	ch->printlnf( "You have failed.");
        	return FULL_MANA;
    	}

	obj_data *old_wall1 = get_obj_list(ch,"wall",ch->in_room->contents);
	if(old_wall1 != NULL)
	{
		bool dispeled=false;	
		switch (door)
		{
			case 0:	
				if ((old_wall1->pIndexData->vnum == OBJ_WALL_OF_ICE_N) ||
				    (old_wall1->pIndexData->vnum == OBJ_WALL_OF_THORNS_N))
				{
					act( "$p suddenly vanishes before your eyes!", ch, old_wall1, NULL, TO_ROOM );
					act( "Your wall of fire destroys $p before your eyes!", ch, old_wall1, NULL, TO_CHAR );
					extract_obj(old_wall1);
					ch->in_room = to_room;
					obj_data *old_wall2 = get_obj_list(ch,"wall",ch->in_room->contents);
					if(old_wall2 != NULL)
					{
						act( "$p suddenly vanishes before your eyes!", ch, old_wall2, NULL, TO_ROOM );
						extract_obj(old_wall2);
					}
				ch->in_room = in_room;
				dispeled=true;
				break;
				}
			case 1:	
				if ((old_wall1->pIndexData->vnum == OBJ_WALL_OF_ICE_E) ||
				    (old_wall1->pIndexData->vnum == OBJ_WALL_OF_THORNS_E))
				{
					act( "$p suddenly vanishes before your eyes!", ch, old_wall1, NULL, TO_ROOM );
					act( "Your wall of fire destroys $p before your eyes!", ch, old_wall1, NULL, TO_CHAR );
					extract_obj(old_wall1);
					ch->in_room = to_room;
					obj_data *old_wall2 = get_obj_list(ch,"wall",ch->in_room->contents);
					if(old_wall2 != NULL)
					{
						act( "$p suddenly vanishes before your eyes!", ch, old_wall2, NULL, TO_ROOM );
						extract_obj(old_wall2);
					}
				ch->in_room = in_room;
				dispeled=true;
				break;
				}
			case 2:	
				if ((old_wall1->pIndexData->vnum == OBJ_WALL_OF_ICE_S) ||
				    (old_wall1->pIndexData->vnum == OBJ_WALL_OF_THORNS_S))
				{
					act( "$p suddenly vanishes before your eyes!", ch, old_wall1, NULL, TO_ROOM );
					act( "Your wall of fire destroys $p before your eyes!", ch, old_wall1, NULL, TO_CHAR );
					extract_obj(old_wall1);
					ch->in_room = to_room;
					obj_data *old_wall2 = get_obj_list(ch,"wall",ch->in_room->contents);
					if(old_wall2 != NULL)
					{
						act( "$p suddenly vanishes before your eyes!", ch, old_wall2, NULL, TO_ROOM );
						extract_obj(old_wall2);
					}
				ch->in_room = in_room;
				dispeled=true;
				break;
				}
			case 3:	
				if ((old_wall1->pIndexData->vnum == OBJ_WALL_OF_ICE_W) ||
				    (old_wall1->pIndexData->vnum == OBJ_WALL_OF_THORNS_W))
				{
					act( "$p suddenly vanishes before your eyes!", ch, old_wall1, NULL, TO_ROOM );
					act( "Your wall of fire destroys $p before your eyes!", ch, old_wall1, NULL, TO_CHAR );
					extract_obj(old_wall1);
					ch->in_room = to_room;
					obj_data *old_wall2 = get_obj_list(ch,"wall",ch->in_room->contents);
					if(old_wall2 != NULL)
					{
						act( "$p suddenly vanishes before your eyes!", ch, old_wall2, NULL, TO_ROOM );
						extract_obj(old_wall2);
					}
				ch->in_room = in_room;
				dispeled=true;
				break;
				}
			case 6:	
				if ((old_wall1->pIndexData->vnum == OBJ_WALL_OF_ICE_NE) ||
				    (old_wall1->pIndexData->vnum == OBJ_WALL_OF_THORNS_NE))
				{
					act( "$p suddenly vanishes before your eyes!", ch, old_wall1, NULL, TO_ROOM );
					act( "Your wall of fire destroys $p before your eyes!", ch, old_wall1, NULL, TO_CHAR );
					extract_obj(old_wall1);
					ch->in_room = to_room;
					obj_data *old_wall2 = get_obj_list(ch,"wall",ch->in_room->contents);
					if(old_wall2 != NULL)
					{
						act( "$p suddenly vanishes before your eyes!", ch, old_wall2, NULL, TO_ROOM );
						extract_obj(old_wall2);
					}
				ch->in_room = in_room;
				dispeled=true;
				break;
				}
			case 7:	
				if ((old_wall1->pIndexData->vnum == OBJ_WALL_OF_ICE_SE) ||
				    (old_wall1->pIndexData->vnum == OBJ_WALL_OF_THORNS_SE))
				{
					act( "$p suddenly vanishes before your eyes!", ch, old_wall1, NULL, TO_ROOM );
					act( "Your wall of fire destroys $p before your eyes!", ch, old_wall1, NULL, TO_CHAR );
					extract_obj(old_wall1);
					ch->in_room = to_room;
					obj_data *old_wall2 = get_obj_list(ch,"wall",ch->in_room->contents);
					if(old_wall2 != NULL)
					{
						act( "$p suddenly vanishes before your eyes!", ch, old_wall2, NULL, TO_ROOM );
						extract_obj(old_wall2);
					}
				ch->in_room = in_room;
				dispeled=true;
				break;
				}
			case 8:	
				if ((old_wall1->pIndexData->vnum == OBJ_WALL_OF_ICE_SW) ||
				    (old_wall1->pIndexData->vnum == OBJ_WALL_OF_THORNS_SW))
				{
					act( "$p suddenly vanishes before your eyes!", ch, old_wall1, NULL, TO_ROOM );
					act( "Your wall of fire destroys $p before your eyes!", ch, old_wall1, NULL, TO_CHAR );
					extract_obj(old_wall1);
					ch->in_room = to_room;
					obj_data *old_wall2 = get_obj_list(ch,"wall",ch->in_room->contents);
					if(old_wall2 != NULL)
					{
						act( "$p suddenly vanishes before your eyes!", ch, old_wall2, NULL, TO_ROOM );
						extract_obj(old_wall2);
					}
				ch->in_room = in_room;
				dispeled=true;
				break;
				}
			case 9:	
				if ((old_wall1->pIndexData->vnum == OBJ_WALL_OF_ICE_NW) ||
				    (old_wall1->pIndexData->vnum == OBJ_WALL_OF_THORNS_NW))
				{
					act( "$p suddenly vanishes before your eyes!", ch, old_wall1, NULL, TO_ROOM );
					act( "Your wall of fire destroys $p before your eyes!", ch, old_wall1, NULL, TO_CHAR );
					extract_obj(old_wall1);
					ch->in_room = to_room;
					obj_data *old_wall2 = get_obj_list(ch,"wall",ch->in_room->contents);
					if(old_wall2 != NULL)
					{
						act( "$p suddenly vanishes before your eyes!", ch, old_wall2, NULL, TO_ROOM );
						extract_obj(old_wall2);
					}
				ch->in_room = in_room;
				dispeled=true;
				break;
				}
			default:
				break;
		}
		if(dispeled==false)
		{
        		ch->printlnf( "You have failed. There is already a wall here.");
	        	return FULL_MANA;
		}
		return NO_MANA;
    	}

	int vnum1=0, vnum2=0;

	switch (door)
	{
		case 0:
			vnum1=OBJ_WALL_OF_FIRE_N;
			vnum2=OBJ_WALL_OF_FIRE_S;
			break;
		case 1:
			vnum1=OBJ_WALL_OF_FIRE_E;
			vnum2=OBJ_WALL_OF_FIRE_W;
			break;
		case 2:
			vnum1=OBJ_WALL_OF_FIRE_S;
			vnum2=OBJ_WALL_OF_FIRE_N;
			break;
		case 3:
			vnum1=OBJ_WALL_OF_FIRE_W;
			vnum2=OBJ_WALL_OF_FIRE_E;
			break;
		case 6:
			vnum1=OBJ_WALL_OF_FIRE_NE;
			vnum2=OBJ_WALL_OF_FIRE_SW;
			break;
		case 7:
			vnum1=OBJ_WALL_OF_FIRE_SE;
			vnum2=OBJ_WALL_OF_FIRE_NW;
			break;
		case 8:
			vnum1=OBJ_WALL_OF_FIRE_SW;
			vnum2=OBJ_WALL_OF_FIRE_NE;
			break;
		case 9:
			vnum1=OBJ_WALL_OF_FIRE_NW;
			vnum2=OBJ_WALL_OF_FIRE_SE;
			break;
		default:
	        	ch->printlnf( "You have failed.");
        		return FULL_MANA;
			break;	
	}

	OBJ_DATA *wall1, *wall2;
     	wall1 = create_object(get_obj_index(vnum1));
     	wall2 = create_object(get_obj_index(vnum2));
     	wall1->timer = 5 * ch->level/5;
     	wall2->timer = 5 * ch->level/5;

	obj_to_room(wall1, in_room);
	obj_to_room(wall2, to_room);

	act( "$p suddenly `#`Rb`rl`Ra`rz`Re`rs`^ up before you.", ch, wall1, NULL, TO_ROOM );
	act( "$p suddenly `#`Rb`rl`Ra`rz`Re`rs`^ up before you.", ch, wall1, NULL, TO_CHAR );
	ch->in_room = to_room;
	act( "$p suddenly `#`Rb`rl`Ra`rz`Re`rs`^ up before you.", ch, wall2, NULL, TO_ROOM );
	ch->in_room = in_room;
	return NO_MANA;
}
/**************************************************************************/
SPRESULT spell_wall_of_ice( int sn, int level, char_data *ch, void *vo, int )
{
	ROOM_INDEX_DATA *in_room;
	ROOM_INDEX_DATA *to_room = NULL;
	EXIT_DATA *pexit;
    	int door = *(int *)vo;

	in_room	= ch->in_room;

    	if (door==-1 || door==4 || door==5)
	{
        	ch->printlnf( "You have failed.");
        	return FULL_MANA;
    	} 

	if (IS_SET(in_room->room_flags, ROOM_OOC))
	{
        	ch->printlnf( "You have failed.");
        	return FULL_MANA;
    	}  

	if ( ( pexit = in_room->exit[door] ) == NULL || ( to_room = pexit->u1.to_room   ) == NULL)
	{
        	ch->printlnf( "You have failed.");
        	return FULL_MANA;
    	}

	if (IS_SET(pexit->exit_info, EX_CLOSED))
	{
        	ch->printlnf( "You have failed.");
        	return FULL_MANA;
    	}

	obj_data *old_wall1 = get_obj_list(ch,"wall",ch->in_room->contents);
	if(old_wall1 != NULL)
	{
		bool dispeled=false;	
		switch (door)
		{
			case 0:	
				if (old_wall1->pIndexData->vnum == OBJ_WALL_OF_FIRE_N)
				{
					act( "$p suddenly vanishes before your eyes!", ch, old_wall1, NULL, TO_ROOM );
					act( "Your wall of ice destroys $p before your eyes!", ch, old_wall1, NULL, TO_CHAR );
					extract_obj(old_wall1);
					ch->in_room = to_room;
					obj_data *old_wall2 = get_obj_list(ch,"wall",ch->in_room->contents);
					if(old_wall2 != NULL)
					{
						act( "$p suddenly vanishes before your eyes!", ch, old_wall2, NULL, TO_ROOM );
						extract_obj(old_wall2);
					}
				ch->in_room = in_room;
				dispeled=true;
				break;
				}
			case 1:	
				if (old_wall1->pIndexData->vnum == OBJ_WALL_OF_FIRE_E)
				{
					act( "$p suddenly vanishes before your eyes!", ch, old_wall1, NULL, TO_ROOM );
					act( "Your wall of ice destroys $p before your eyes!", ch, old_wall1, NULL, TO_CHAR );
					extract_obj(old_wall1);
					ch->in_room = to_room;
					obj_data *old_wall2 = get_obj_list(ch,"wall",ch->in_room->contents);
					if(old_wall2 != NULL)
					{
						act( "$p suddenly vanishes before your eyes!", ch, old_wall2, NULL, TO_ROOM );
						extract_obj(old_wall2);
					}
				ch->in_room = in_room;
				dispeled=true;
				break;
				}
			case 2:	
				if (old_wall1->pIndexData->vnum == OBJ_WALL_OF_FIRE_S)
				{
					act( "$p suddenly vanishes before your eyes!", ch, old_wall1, NULL, TO_ROOM );
					act( "Your wall of ice destroys $p before your eyes!", ch, old_wall1, NULL, TO_CHAR );
					extract_obj(old_wall1);
					ch->in_room = to_room;
					obj_data *old_wall2 = get_obj_list(ch,"wall",ch->in_room->contents);
					if(old_wall2 != NULL)
					{
						act( "$p suddenly vanishes before your eyes!", ch, old_wall2, NULL, TO_ROOM );
						extract_obj(old_wall2);
					}
				ch->in_room = in_room;
				dispeled=true;
				break;
				}
			case 3:	
				if (old_wall1->pIndexData->vnum == OBJ_WALL_OF_FIRE_W)
				{
					act( "$p suddenly vanishes before your eyes!", ch, old_wall1, NULL, TO_ROOM );
					act( "Your wall of ice destroys $p before your eyes!", ch, old_wall1, NULL, TO_CHAR );
					extract_obj(old_wall1);
					ch->in_room = to_room;
					obj_data *old_wall2 = get_obj_list(ch,"wall",ch->in_room->contents);
					if(old_wall2 != NULL)
					{
						act( "$p suddenly vanishes before your eyes!", ch, old_wall2, NULL, TO_ROOM );
						extract_obj(old_wall2);
					}
				ch->in_room = in_room;
				dispeled=true;
				break;
				}
			case 6:	
				if (old_wall1->pIndexData->vnum == OBJ_WALL_OF_FIRE_NE)
				{
					act( "$p suddenly vanishes before your eyes!", ch, old_wall1, NULL, TO_ROOM );
					act( "Your wall of ice destroys $p before your eyes!", ch, old_wall1, NULL, TO_CHAR );
					extract_obj(old_wall1);
					ch->in_room = to_room;
					obj_data *old_wall2 = get_obj_list(ch,"wall",ch->in_room->contents);
					if(old_wall2 != NULL)
					{
						act( "$p suddenly vanishes before your eyes!", ch, old_wall2, NULL, TO_ROOM );
						extract_obj(old_wall2);
					}
				ch->in_room = in_room;
				dispeled=true;
				break;
				}
			case 7:	
				if (old_wall1->pIndexData->vnum == OBJ_WALL_OF_FIRE_SE)
				{
					act( "$p suddenly vanishes before your eyes!", ch, old_wall1, NULL, TO_ROOM );
					act( "Your wall of ice destroys $p before your eyes!", ch, old_wall1, NULL, TO_CHAR );
					extract_obj(old_wall1);
					ch->in_room = to_room;
					obj_data *old_wall2 = get_obj_list(ch,"wall",ch->in_room->contents);
					if(old_wall2 != NULL)
					{
						act( "$p suddenly vanishes before your eyes!", ch, old_wall2, NULL, TO_ROOM );
						extract_obj(old_wall2);
					}
				ch->in_room = in_room;
				dispeled=true;
				break;
				}
			case 8:	
				if (old_wall1->pIndexData->vnum == OBJ_WALL_OF_FIRE_SW)
				{
					act( "$p suddenly vanishes before your eyes!", ch, old_wall1, NULL, TO_ROOM );
					act( "Your wall of ice destroys $p before your eyes!", ch, old_wall1, NULL, TO_CHAR );
					extract_obj(old_wall1);
					ch->in_room = to_room;
					obj_data *old_wall2 = get_obj_list(ch,"wall",ch->in_room->contents);
					if(old_wall2 != NULL)
					{
						act( "$p suddenly vanishes before your eyes!", ch, old_wall2, NULL, TO_ROOM );
						extract_obj(old_wall2);
					}
				ch->in_room = in_room;
				dispeled=true;
				break;
				}
			case 9:	
				if (old_wall1->pIndexData->vnum == OBJ_WALL_OF_FIRE_NW)
				{
					act( "$p suddenly vanishes before your eyes!", ch, old_wall1, NULL, TO_ROOM );
					act( "Your wall of ice destroys $p before your eyes!", ch, old_wall1, NULL, TO_CHAR );
					extract_obj(old_wall1);
					ch->in_room = to_room;
					obj_data *old_wall2 = get_obj_list(ch,"wall",ch->in_room->contents);
					if(old_wall2 != NULL)
					{
						act( "$p suddenly vanishes before your eyes!", ch, old_wall2, NULL, TO_ROOM );
						extract_obj(old_wall2);
					}
				ch->in_room = in_room;
				dispeled=true;
				break;
				}
			default:
				break;
		}
		if(dispeled==false)
		{
        		ch->printlnf( "You have failed. There is already a wall here.");
	        	return FULL_MANA;
		}
		return NO_MANA;
    	}

	int vnum1=0, vnum2=0;

	switch (door)
	{
		case 0:
			vnum1=OBJ_WALL_OF_ICE_N;
			vnum2=OBJ_WALL_OF_ICE_S;
			break;
		case 1:
			vnum1=OBJ_WALL_OF_ICE_E;
			vnum2=OBJ_WALL_OF_ICE_W;
			break;
		case 2:
			vnum1=OBJ_WALL_OF_ICE_S;
			vnum2=OBJ_WALL_OF_ICE_N;
			break;
		case 3:
			vnum1=OBJ_WALL_OF_ICE_W;
			vnum2=OBJ_WALL_OF_ICE_E;
			break;
		case 6:
			vnum1=OBJ_WALL_OF_ICE_NE;
			vnum2=OBJ_WALL_OF_ICE_SW;
			break;
		case 7:
			vnum1=OBJ_WALL_OF_ICE_SE;
			vnum2=OBJ_WALL_OF_ICE_NW;
			break;
		case 8:
			vnum1=OBJ_WALL_OF_ICE_SW;
			vnum2=OBJ_WALL_OF_ICE_NE;
			break;
		case 9:
			vnum1=OBJ_WALL_OF_ICE_NW;
			vnum2=OBJ_WALL_OF_ICE_SE;
			break;
		default:
	        	ch->printlnf( "You have failed.");
        		return FULL_MANA;
			break;	
	}

	OBJ_DATA *wall1, *wall2;
     	wall1 = create_object(get_obj_index(vnum1));
     	wall2 = create_object(get_obj_index(vnum2));
     	wall1->timer = 5 * ch->level/5;
     	wall2->timer = 5 * ch->level/5;

	obj_to_room(wall1, in_room);
	obj_to_room(wall2, to_room);

	act( "$p suddenly `#`Yissues forth`^ from the ground before you.", ch, wall1, NULL, TO_ROOM );
	act( "$p suddenly `#`Yissues forth`^ from the ground before you.", ch, wall1, NULL, TO_CHAR );
	ch->in_room = to_room;
	act( "$p suddenly `#`Yissues forth`^ from the ground before you.", ch, wall2, NULL, TO_ROOM );
	ch->in_room = in_room;
	return NO_MANA;
}
/**************************************************************************/
SPRESULT spell_wall_of_stone( int sn, int level, char_data *ch, void *vo, int )
{
	ROOM_INDEX_DATA *in_room;
	ROOM_INDEX_DATA *to_room = NULL;
	EXIT_DATA *pexit;
    	int door = *(int *)vo;

	in_room	= ch->in_room;

    	if (door==-1 || door==4 || door==5)
	{
        	ch->printlnf( "You have failed.");
        	return FULL_MANA;
    	} 

	if (IS_SET(in_room->room_flags, ROOM_OOC))
	{
        	ch->printlnf( "You have failed.");
        	return FULL_MANA;
    	}  

	if ( ( pexit = in_room->exit[door] ) == NULL || ( to_room = pexit->u1.to_room   ) == NULL)
	{
        	ch->printlnf( "You have failed.");
        	return FULL_MANA;
    	}

	if (IS_SET(pexit->exit_info, EX_CLOSED))
	{
        	ch->printlnf( "You have failed.");
        	return FULL_MANA;
    	}

	obj_data *old_wall = get_obj_list(ch,"wall",ch->in_room->contents);
	if(old_wall != NULL)
	{
        	ch->printlnf( "You have failed. There is already a wall here.");
        	return FULL_MANA;
    	}

	int vnum1=0, vnum2=0;

	switch (door)
	{
		case 0:
			vnum1=OBJ_WALL_OF_STONE_N;
			vnum2=OBJ_WALL_OF_STONE_S;
			break;
		case 1:
			vnum1=OBJ_WALL_OF_STONE_E;
			vnum2=OBJ_WALL_OF_STONE_W;
			break;
		case 2:
			vnum1=OBJ_WALL_OF_STONE_S;
			vnum2=OBJ_WALL_OF_STONE_N;
			break;
		case 3:
			vnum1=OBJ_WALL_OF_STONE_W;
			vnum2=OBJ_WALL_OF_STONE_E;
			break;
		case 6:
			vnum1=OBJ_WALL_OF_STONE_NE;
			vnum2=OBJ_WALL_OF_STONE_SW;
			break;
		case 7:
			vnum1=OBJ_WALL_OF_STONE_SE;
			vnum2=OBJ_WALL_OF_STONE_NW;
			break;
		case 8:
			vnum1=OBJ_WALL_OF_STONE_SW;
			vnum2=OBJ_WALL_OF_STONE_NE;
			break;
		case 9:
			vnum1=OBJ_WALL_OF_STONE_NW;
			vnum2=OBJ_WALL_OF_STONE_SE;
			break;
		default:
	        	ch->printlnf( "You have failed.");
        		return FULL_MANA;
			break;	
	}

	OBJ_DATA *wall1, *wall2;
     	wall1 = create_object(get_obj_index(vnum1));
     	wall2 = create_object(get_obj_index(vnum2));
     	wall1->timer = 5 * ch->level/5;
     	wall2->timer = 5 * ch->level/5;

	obj_to_room(wall1, in_room);
	obj_to_room(wall2, to_room);

	act( "$p suddenly `#`Ysprings forth`^ from the ground before you.", ch, wall1, NULL, TO_ROOM );
	act( "$p suddenly `#`Ysprings forth`^ from the ground before you.", ch, wall1, NULL, TO_CHAR );
	ch->in_room = to_room;
	act( "$p suddenly `#`Ysprings forth`^ from the ground before you.", ch, wall2, NULL, TO_ROOM );
	ch->in_room = in_room;
	return NO_MANA;
}
/**************************************************************************/
SPRESULT spell_wall_of_thorns( int sn, int level, char_data *ch, void *vo, int )
{
	ROOM_INDEX_DATA *in_room;
	ROOM_INDEX_DATA *to_room = NULL;
	EXIT_DATA *pexit;
    	int door = *(int *)vo;

	in_room	= ch->in_room;

    	if (door==-1 || door==4 || door==5)
	{
        	ch->printlnf( "You have failed.");
        	return FULL_MANA;
    	} 

	if (IS_SET(in_room->room_flags, ROOM_OOC))
	{
        	ch->printlnf( "You have failed.");
        	return FULL_MANA;
    	}  

	if ( ( pexit = in_room->exit[door] ) == NULL || ( to_room = pexit->u1.to_room   ) == NULL)
	{
        	ch->printlnf( "You have failed.");
        	return FULL_MANA;
    	}

	if (IS_SET(pexit->exit_info, EX_CLOSED))
	{
        	ch->printlnf( "You have failed.");
        	return FULL_MANA;
    	}


	obj_data *old_wall = get_obj_list(ch,"wall",ch->in_room->contents);
	if(old_wall != NULL)
	{
        	ch->printlnf( "You have failed. There is already a wall here.");
        	return FULL_MANA;
    	}

	int vnum1=0, vnum2=0;

	switch (door)
	{
		case 0:
			vnum1=OBJ_WALL_OF_THORNS_N;
			vnum2=OBJ_WALL_OF_THORNS_S;
			break;
		case 1:
			vnum1=OBJ_WALL_OF_THORNS_E;
			vnum2=OBJ_WALL_OF_THORNS_W;
			break;
		case 2:
			vnum1=OBJ_WALL_OF_THORNS_S;
			vnum2=OBJ_WALL_OF_THORNS_N;
			break;
		case 3:
			vnum1=OBJ_WALL_OF_THORNS_W;
			vnum2=OBJ_WALL_OF_THORNS_E;
			break;
		case 6:
			vnum1=OBJ_WALL_OF_THORNS_NE;
			vnum2=OBJ_WALL_OF_THORNS_SW;
			break;
		case 7:
			vnum1=OBJ_WALL_OF_THORNS_SE;
			vnum2=OBJ_WALL_OF_THORNS_NW;
			break;
		case 8:
			vnum1=OBJ_WALL_OF_THORNS_SW;
			vnum2=OBJ_WALL_OF_THORNS_NE;
			break;
		case 9:
			vnum1=OBJ_WALL_OF_THORNS_NW;
			vnum2=OBJ_WALL_OF_THORNS_SE;
			break;
		default:
	        	ch->printlnf( "You have failed.");
        		return FULL_MANA;
			break;	
	}

	OBJ_DATA *wall1, *wall2;
     	wall1 = create_object(get_obj_index(vnum1));
     	wall2 = create_object(get_obj_index(vnum2));
     	wall1->timer = 5 * ch->level/5;
     	wall2->timer = 5 * ch->level/5;

	obj_to_room(wall1, in_room);
	obj_to_room(wall2, to_room);

	act( "$p suddenly `#`ggrows`^ from the ground before you.", ch, wall1, NULL, TO_ROOM );
	act( "$p suddenly `#`ggrows`^ from the ground before you.", ch, wall1, NULL, TO_CHAR );
	ch->in_room = to_room;
	act( "$p suddenly `#`ggrows`^ from the ground before you.", ch, wall2, NULL, TO_ROOM );
	ch->in_room = in_room;
	return NO_MANA;
}
/**************************************************************************/
SPRESULT spell_scorpion_claw( int sn, int level, char_data *ch, void *vo,int )
{
    char_data *victim = (char_data *) vo;
	AFFECT_DATA af;
	int dam;
	int num_claw;

    act("$n creates a lash of poison!",ch,NULL,NULL,TO_ROOM);
    act("You form your magic into a lash of pure poison!",ch,NULL,NULL,TO_CHAR);	
	//start claw strikes
	num_claw = level/16 + 1;
	dam		= dice(level/4,5);		
	while (num_claw > 0)
	{
		dam		= dice(level/4,5);
		if ( saves_spell( level, victim,DAMTYPE(sn)) )
			dam /= 2;
		damage_spell( ch, victim, dam, sn, DAMTYPE(sn),true);
		if ( !saves_spell( level, victim,DAM_POISON) )
		{
			switch(check_immune(victim,DAM_POISON))
			{
				case(IS_IMMUNE):
					break;
				case(IS_RESISTANT):
					if(number_range(1,5) == 1)
					{
						af.where     = WHERE_AFFECTS;
						af.type      = gsn_poison;
						af.level     = level;
						af.duration  = (level/3);
						af.location  = APPLY_ST;
						af.modifier  = -4;
						af.bitvector = AFF_POISON;
						affect_join( victim, &af );
						victim->printf( "You feel very sick.\r\n" );
					}
					break;
				case(IS_VULNERABLE):
					if(number_range(1,3) == 1)
					{
						af.where     = WHERE_AFFECTS;
						af.type      = gsn_poison;
						af.level     = level;
						af.duration  = (level/3);
						af.location  = APPLY_ST;
						af.modifier  = -4;
						af.bitvector = AFF_POISON;
						affect_join( victim, &af );
						victim->printf( "You feel very sick.\r\n" );
					}
					break;
				default:
					if(number_range(1,4) == 1)
					{
						af.where     = WHERE_AFFECTS;
						af.type      = gsn_poison;
						af.level     = level;
						af.duration  = (level/3);
						af.location  = APPLY_ST;
						af.modifier  = -4;
						af.bitvector = AFF_POISON;
						affect_join( victim, &af );
						victim->printf( "You feel very sick.\r\n" );
					}
					break;
			}
		}
		num_claw -= 1;
	}
	return FULL_MANA;
}
/**************************************************************************/
SPRESULT spell_rotten_odor( int sn, int level, char_data *ch, void *vo, int )
{
    char_data *victim = (char_data *) vo;
    AFFECT_DATA af;

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

		af.where	= WHERE_AFFECTS;
		af.level	= 100;
		af.duration 	= -1;
		af.location	= APPLY_NONE;
		af.modifier 	= 0;
		af.bitvector	= 0;

    	affect_to_char( victim, &af );
	af.type		= gsn_smell_rotten;
	ch->println ("You begin to emit a putrid stench from your body.");
	act( "$n begins to foul the air with thier putrid stench.", ch, NULL, NULL, TO_ROOM );
    return NO_MANA;
}
SPRESULT spell_rose_odor( int sn, int level, char_data *ch, void *vo, int )
{
    char_data *victim = (char_data *) vo;
    AFFECT_DATA af;

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

		af.where	= WHERE_AFFECTS;
		af.level	= 100;
		af.duration 	= -1;
		af.location	= APPLY_NONE;
		af.modifier 	= 0;
		af.bitvector	= 0;

    	affect_to_char( victim, &af );
	af.type		= gsn_smell_rotten;
	ch->println ("You begin to emit a putrid stench from your body.");
	act( "$n begins to foul the air with thier putrid stench.", ch, NULL, NULL, TO_ROOM );
    return NO_MANA;
}
SPRESULT spell_jasmine_odor( int sn, int level, char_data *ch, void *vo, int )
{
    char_data *victim = (char_data *) vo;
    AFFECT_DATA af;

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

		af.where	= WHERE_AFFECTS;
		af.level	= 100;
		af.duration 	= -1;
		af.location	= APPLY_NONE;
		af.modifier 	= 0;
		af.bitvector	= 0;

    	affect_to_char( victim, &af );
	af.type		= gsn_smell_rotten;
	ch->println ("You begin to emit a putrid stench from your body.");
	act( "$n begins to foul the air with thier putrid stench.", ch, NULL, NULL, TO_ROOM );
    return NO_MANA;
}
SPRESULT spell_sewer_odor( int sn, int level, char_data *ch, void *vo, int )
{
    char_data *victim = (char_data *) vo;
    AFFECT_DATA af;

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

		af.where	= WHERE_AFFECTS;
		af.level	= 100;
		af.duration 	= -1;
		af.location	= APPLY_NONE;
		af.modifier 	= 0;
		af.bitvector	= 0;

    	affect_to_char( victim, &af );
	af.type		= gsn_smell_rotten;
	ch->println ("You begin to emit a putrid stench from your body.");
	act( "$n begins to foul the air with thier putrid stench.", ch, NULL, NULL, TO_ROOM );
    return NO_MANA;
}
SPRESULT spell_soap_odor( int sn, int level, char_data *ch, void *vo, int )
{
    char_data *victim = (char_data *) vo;
    AFFECT_DATA af;

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

		af.where	= WHERE_AFFECTS;
		af.level	= 100;
		af.duration 	= -1;
		af.location	= APPLY_NONE;
		af.modifier 	= 0;
		af.bitvector	= 0;

    	affect_to_char( victim, &af );
	af.type		= gsn_smell_rotten;
	ch->println ("You begin to emit a putrid stench from your body.");
	act( "$n begins to foul the air with thier putrid stench.", ch, NULL, NULL, TO_ROOM );
    return NO_MANA;
}
SPRESULT spell_cinnamon_odor( int sn, int level, char_data *ch, void *vo, int )
{
    char_data *victim = (char_data *) vo;
    AFFECT_DATA af;

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

		af.where	= WHERE_AFFECTS;
		af.level	= 100;
		af.duration 	= -1;
		af.location	= APPLY_NONE;
		af.modifier 	= 0;
		af.bitvector	= 0;

    	affect_to_char( victim, &af );
	af.type		= gsn_smell_rotten;
	ch->println ("You begin to emit a putrid stench from your body.");
	act( "$n begins to foul the air with thier putrid stench.", ch, NULL, NULL, TO_ROOM );
    return NO_MANA;
}
SPRESULT spell_corpse_odor( int sn, int level, char_data *ch, void *vo, int )
{
    char_data *victim = (char_data *) vo;
    AFFECT_DATA af;

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

		af.where	= WHERE_AFFECTS;
		af.level	= 100;
		af.duration 	= -1;
		af.location	= APPLY_NONE;
		af.modifier 	= 0;
		af.bitvector	= 0;

    	affect_to_char( victim, &af );
	af.type		= gsn_smell_rotten;
	ch->println ("You begin to emit a putrid stench from your body.");
	act( "$n begins to foul the air with thier putrid stench.", ch, NULL, NULL, TO_ROOM );
    return NO_MANA;
}
SPRESULT spell_rotfood_odor( int sn, int level, char_data *ch, void *vo, int )
{
    char_data *victim = (char_data *) vo;
    AFFECT_DATA af;

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

		af.where	= WHERE_AFFECTS;
		af.level	= 100;
		af.duration 	= -1;
		af.location	= APPLY_NONE;
		af.modifier 	= 0;
		af.bitvector	= 0;

    	affect_to_char( victim, &af );
	af.type		= gsn_smell_rotten;
	ch->println ("You begin to emit a putrid stench from your body.");
	act( "$n begins to foul the air with thier putrid stench.", ch, NULL, NULL, TO_ROOM );
    return NO_MANA;
}
SPRESULT spell_freshbread_odor( int sn, int level, char_data *ch, void *vo, int )
{
    char_data *victim = (char_data *) vo;
    AFFECT_DATA af;

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

		af.where	= WHERE_AFFECTS;
		af.level	= 100;
		af.duration 	= -1;
		af.location	= APPLY_NONE;
		af.modifier 	= 0;
		af.bitvector	= 0;

    	affect_to_char( victim, &af );
	af.type		= gsn_smell_rotten;
	ch->println ("You begin to emit a putrid stench from your body.");
	act( "$n begins to foul the air with thier putrid stench.", ch, NULL, NULL, TO_ROOM );
    return NO_MANA;
}
SPRESULT spell_mownhay_odor( int sn, int level, char_data *ch, void *vo, int )
{
    char_data *victim = (char_data *) vo;
    AFFECT_DATA af;

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

		af.where	= WHERE_AFFECTS;
		af.level	= 100;
		af.duration 	= -1;
		af.location	= APPLY_NONE;
		af.modifier 	= 0;
		af.bitvector	= 0;

    	affect_to_char( victim, &af );
	af.type		= gsn_smell_rotten;
	ch->println ("You begin to emit a putrid stench from your body.");
	act( "$n begins to foul the air with thier putrid stench.", ch, NULL, NULL, TO_ROOM );
    return NO_MANA;
}
SPRESULT spell_freshlinen_odor( int sn, int level, char_data *ch, void *vo, int )
{
    char_data *victim = (char_data *) vo;
    AFFECT_DATA af;

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

		af.where	= WHERE_AFFECTS;
		af.level	= 100;
		af.duration 	= -1;
		af.location	= APPLY_NONE;
		af.modifier 	= 0;
		af.bitvector	= 0;

    	affect_to_char( victim, &af );
	af.type		= gsn_smell_rotten;
	ch->println ("You begin to emit a putrid stench from your body.");
	act( "$n begins to foul the air with thier putrid stench.", ch, NULL, NULL, TO_ROOM );
    return NO_MANA;
}
SPRESULT spell_incense_odor( int sn, int level, char_data *ch, void *vo, int )
{
    char_data *victim = (char_data *) vo;
    AFFECT_DATA af;

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

		af.where	= WHERE_AFFECTS;
		af.level	= 100;
		af.duration 	= -1;
		af.location	= APPLY_NONE;
		af.modifier 	= 0;
		af.bitvector	= 0;

    	affect_to_char( victim, &af );
	af.type		= gsn_smell_rotten;
	ch->println ("You begin to emit a putrid stench from your body.");
	act( "$n begins to foul the air with thier putrid stench.", ch, NULL, NULL, TO_ROOM );
    return NO_MANA;
}
SPRESULT spell_smoke_odor( int sn, int level, char_data *ch, void *vo, int )
{
    char_data *victim = (char_data *) vo;
    AFFECT_DATA af;

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

		af.where	= WHERE_AFFECTS;
		af.level	= 100;
		af.duration 	= -1;
		af.location	= APPLY_NONE;
		af.modifier 	= 0;
		af.bitvector	= 0;

    	affect_to_char( victim, &af );
	af.type		= gsn_smell_rotten;
	ch->println ("You begin to emit a putrid stench from your body.");
	act( "$n begins to foul the air with thier putrid stench.", ch, NULL, NULL, TO_ROOM );
    return NO_MANA;
}
/**************************************************************************/


