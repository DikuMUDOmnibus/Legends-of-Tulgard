/**************************************************************************/
// skill_ti.cpp - skills written by Tibault
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/

#include "include.h"

void do_collectwater( char_data *ch);
void do_collect( char_data *ch, char *argument);
void do_pounce( char_data *ch, char *argument);
void landchar( char_data *ch); // act_move.cpp
void do_worship(char_data *ch, char *argument);
DECLARE_OSPEC_FUN(	ospec_mirage_cloak);

/**************************************************************************/
// - Tibault Jun 2000
void do_collect( char_data *ch, char *argument)
{
	char	arg[MIL];
	argument = one_argument(argument,arg);

	if ( !str_cmp(arg, "water" ) )
	{
		do_collectwater(ch);
		return;
	}
	else
	{
		ch->println("Collect what?");
		ch->println("Syntax is 'collect <object>'");
		ch->println("Only 'water' is available at this time.");
		return;
	}
}

/**************************************************************************/
// - Tibault Jun 2000, class flag tweaks by Kalahn
void do_collectwater( char_data *ch)
{
	OBJ_DATA *obj;
	int liquid = 0;
	bool foundFountain = false;

	// Check for a fountain with untainted water.
	for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	{
		if ( obj->item_type == ITEM_FOUNTAIN )
		{
			if ( ( liquid = obj->value[2] ) < 0 )
			{
				bugf( "Do_collectwater: bad liquid number %d.", liquid );
				liquid = obj->value[2] = 0;
			}

			if ( strstr(liq_table[liquid].liq_name, "untainted water") )
				break;
			else 
				// Found a fountain but not with correct liquid type.
				foundFountain = true;
		}
	}	
	if ( obj == NULL )
	{
		if ( foundFountain )
		ch->printlnf( "This %s will only dirty your dousing flask.", 
			liq_table[liquid].liq_name);
		else
			ch->println("Collect water from what?");
		return;
	}
    if ( ( liquid = obj->value[2] )  < 0 )
	{
		bugf( "Do_collectwater: bad liquid number %d.", liquid );
           liquid = obj->value[2] = 0;
    }
	if ( obj->value[1] <= 0 )
	{
		ch->println("It is already empty.");
		return;
	}

	// Check for empty dousing flask.
	for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
		if ( strstr(obj->pIndexData->material, "dousingflask") )
			break;
    }

	if ( obj == NULL )
	{
		ch->println("You do not seem to have an empty dousing flask!");
		return;
	}

	int objvnum=OBJ_VNUM_CRYSTAL_FLASK;
	if(!HAS_CLASSFLAG(ch, CLASSFLAG_CAN_COLLECT_WATER) && number_range(1,10)!=1){
		objvnum=OBJ_VNUM_BADCRYSTAL_FLASK; // classes that can't do it usually stuff up
	}

	if ( get_obj_index(objvnum) == NULL )
	{
		bugf("Vnum %d not found for do_collectwater!", objvnum);
		ch->printlnf( "Vnum %d not found for do_collect!, please report to the admin.", 
			objvnum);
		return;
	}

	extract_obj(obj);
	obj = create_object( get_obj_index( objvnum ));
	obj_to_char( obj, ch );
	act("$n expertly busies $mself collecting water in a dousing flask.",ch,NULL,NULL,TO_ROOM);
	if(HAS_CLASSFLAG(ch, CLASSFLAG_CAN_COLLECT_WATER)){
		ch->println("You collect water into your dousing flask.");
		WAIT_STATE( ch, 16 );
	}else{
		ch->println("You carefully collect water into your dousing flask.");
		WAIT_STATE( ch, 36 );
	}
	return;
}

/**************************************************************************/
// - Tibault Jun 2000
void do_pounce(char_data *ch, char *argument)
{
	char arg[MIL];
	char_data *victim;
	int chance;
	int beats;
	
	one_argument(argument,arg);
	
	if (( chance = get_skill(ch,gsn_pounce)) == 0
	||  ( IS_NPC(ch) )
	||  ( !IS_NPC(ch) && ch->level < skill_table[gsn_pounce].skill_level[ch->clss]))
	{
		if (!IS_CONTROLLED(ch))
		{
			ch->println("You do not know how to pounce!");
			return;
		}
	}
	
	if (arg[0] == '\0')
	{
		victim = ch->fighting;
		if (victim == NULL)
		{
			ch->println("But you aren't fighting anyone!");
			return;
		}
	}
	
	else if ((victim = get_char_room(ch,arg)) == NULL)
	{
		ch->println("They aren't here.");
		return;
	}
	
	if (victim == ch)
	{
		ch->println("You leap up, realizing the futility of pouncing on yourself.");
		return;
	}

	if ( IS_AFFECTED(ch, AFF_FLYING) )
	{
		ch->println("You cannot pounce from your mid-air position.");
		return;
	}

	if ( !IS_AFFECTED(victim, AFF_FLYING) )
	{
		ch->println("There is no need to exhaust yourself. They are not flying.");
		return;
	}
	
	if (is_safe(ch,victim))
		return;
	
	if ( !can_initiate_combat( ch, victim, CIT_GENERAL )) return;
    
	if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
		act("But $N is your friend!",ch,NULL,victim,TO_CHAR);
		return;
    }
	
    // Modifiers
	beats = skill_table[gsn_pounce].beats;
    if (ch->size < victim->size)
		chance += (ch->size - victim->size) * 15;
    else
		chance += (ch->size - victim->size) * 10; 
	
    // Stats
    chance += ch->modifiers[STAT_AG];
	chance += ch->modifiers[STAT_QU] / 2;
    chance -= victim->modifiers[STAT_QU];
    chance -= victim->modifiers[STAT_AG];
	chance -= victim->modifiers[STAT_IN] / 3;
    
	// Speed
    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
		chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
		chance -= 30;

	// Affects
	if ( IS_AFFECTED(ch, AFF_HIDE) && !IS_AFFECTED(victim, AFF_DETECT_HIDDEN) )
		chance +=20;
	if ( IS_AFFECTED(ch, AFF_INVISIBLE) && !IS_AFFECTED(victim, AFF_DETECT_INVIS) )
		chance += 15;
	if (   IS_AFFECTED(victim, AFF_BLIND) 
		|| IS_AFFECTED(victim, AFF_POISON) 
		|| IS_AFFECTED(victim, AFF_PLAGUE) )
		chance += 10;
	if ( is_affected(victim, AFF_FAERIE_FIRE) )
		chance += 10;
	if ( is_affected(victim, AFF_SLOW) || IS_AFFECTED(victim, AFF_WEAKEN) )
		chance += 10;
	if ( is_affected(victim, AFF_BLIND) )
		chance -= 15;
	if ( is_affected(victim, AFF_SLOW) )
		chance -= 10;
	if ( is_affected(victim, AFF_POISON) || IS_AFFECTED(victim, AFF_PLAGUE) )
		chance -= 10;

	if ( ch->position == POS_FIGHTING )
		chance -= 30;
	else
		beats /= 2;
	
    // Level
    chance += (ch->level - victim->level);
		
    if (!IS_NPC(victim) && chance < get_skill(victim,gsn_dodge) )
		chance -= 3 * (get_skill(victim,gsn_dodge) - chance);
	
    /* now the attack */
    if (number_percent() < chance )
    {	
		act("$n pounces onto you, sending you tumbling to the ground!",
			ch,NULL,victim,TO_VICT);
		act("You pounce onto $N, and send $M tumbling to the ground!",ch,NULL,victim,TO_CHAR);
		act("$n pounces onto $N, sending them tumbling to the ground.",
			ch,NULL,victim,TO_NOTVICT);
		check_improve(ch,gsn_pounce,true,1);
		
		landchar(victim);
		damage(ch,victim,0,gsn_pounce,DAM_BASH,false);
		WAIT_STATE(ch,beats);
    }
	else
	{
		damage(ch,victim,0,gsn_bash,DAM_BASH,false);
		act("You completely miss and fall flat on your face!",
			ch,NULL,victim,TO_CHAR);
		act("$n leaps into the air, only to fall flat on $s face a few moments later.",
			ch,NULL,victim,TO_NOTVICT);
		act("You evade $n's pounce, causing $m to fall flat on $s face.",
			ch,NULL,victim,TO_VICT);
		check_improve(ch,gsn_pounce,false,1);
		ch->position = POS_RESTING;
		damage(ch,victim,0,gsn_pounce,DAM_BASH,false);
		WAIT_STATE(ch,beats * 2); 
    }
}

/**************************************************************************/
void do_cloak( char_data *ch, char * )
{
	OBJ_DATA * obj = get_eq_char( ch, WEAR_ABOUT );
 	if ( !obj )
 	{
 		ch->println( "You are not wearing a cloak." );
 		return;
 	}
 	if (HAS_CONFIG2( ch, CONFIG2_CLOAKED))
 	{
 		REMOVE_CONFIG2( ch, CONFIG2_CLOAKED);
 		ch->println( "You let the hood on your cloak fall back from your face." );
 		act( "$n pulls back their hood.", ch, NULL, NULL, TO_ROOM );
 		return;
 	}

 	SET_CONFIG2( ch, CONFIG2_CLOAKED);
 	ch->println( "You pull the hood on your cloak tight around your face." );
 	act( "$n hides their face in a hood.", ch, NULL, NULL, TO_ROOM );
	if(obj->ospec_fun == ospec_lookup("ospec_mirage_cloak"))
		ospec_mirage_cloak(obj, ch);
 	return;
}
/**************************************************************************/
void do_bandage(char_data *ch, char *argument)
{
 	int chance;

 	if(ch->position != POS_RESTING)
 	{
 		ch->println("You need to rest in order to bandage yourself.");
 		return;
 	}
 
  	if(ch->bleeding > 0)
 	{
 		chance = number_range(1,10);
 		if ( chance < 3)
 		{
 			ch->println("You fail to bind your wounds.");
 			WAIT_STATE( ch, 6 );
 			return;
 		}
 
 		ch->bleeding = URANGE( 0, ch->bleeding - 10, 100 );
 		ch->println("You place a bandage over your skin to help stop the bleeding.");
 		act("$n places a bandage over $s skin, to help stop the bleeding.", ch, NULL, NULL, TO_ROOM);
 		WAIT_STATE( ch, 6 );
 		if (ch->bleeding == 0)
 		{
			ch->println("Your bleeding has stopped.");
 			ch->will_die = 0;
 		}
 		return;
 		
 	}
 
 	ch->println("You have no wounds to bandage.");
 	return;
}
/**************************************************************************/
void do_orbit( char_data *ch, char *argument)
{
	int skill;

    if (!(skill = get_skill(ch, gsn_orbit)))
    {
		do_huh(ch,"");
		return;
    }

	if (ch->fighting == NULL)
	{
		ch->println("But you aren't fighting anyone!");
		return;
	}

	obj_data *weapon = get_eq_char(ch,WEAR_WIELD);
	if (weapon == NULL || weapon->value[0] != WEAPON_FLAIL)
	{
		ch->println("You have to be using a flail for that!");
		return;
	}
		
    WAIT_STATE( ch, skill_table[gsn_kick].beats );
    if ( get_skill(ch,gsn_orbit) > number_percent())
    {
		ch->println("You spin your flail around yourself in a flurry!");
		SET_BIT(ch->dyn2, DYN2_DOING_ORBIT);
		check_improve(ch,gsn_kick,true,1);
    }
    else
    {
		ch->println("Your flail spins wildly and tangles.");
		check_improve(ch,gsn_kick,false,1);
    }
    return;
}
/**************************************************************************/
void do_infect( char_data *ch, char* argument )
{
     char_data *victim;
 	char arg[MIL];
	AFFECT_DATA af;
 	if(!IS_WEREBEAST(ch))
	{
	do_huh(ch,"");
	return;
	}

 	argument = one_argument(argument, arg);
	
     if (IS_NULLSTR(arg) 
	 ||	(victim = get_char_room(ch, arg)) == NULL)
	 {
		ch->println("You can't seem to find them.");
 		return;
	 }

	 if(!IS_SET(victim->act2, ACT2_ALLOWEMBRACE)
	 && !(ch->position == POS_FIGHTING && victim == ch->fighting))
	 {
		 ch->println("They will not allow you to come that close.");
		 return;
	 }

     act( "You bite $N!",  ch, NULL, victim, TO_CHAR    );
     act( "$n bites $N!",  ch, NULL, victim, TO_NOTVICT );
     act( "$n bites you!", ch, NULL, victim, TO_VICT    );
	 //damage(ch,victim,0,0,DAM_NONE,false);
     if(number_percent( ) > 2 * ch->level )
		 return;

     if(IS_VAMPIRE(victim))
 	return;

    if(IS_WEREBEAST(victim))
	return;

    if ( class_table[victim->clss].class_cast_type == CCT_BARD )
	return;

    if( IS_AFFECTED2(victim, AFF2_HIGHLANDER))
 	return;

    if( IS_AFFECTED2(victim, AFF2_NO_WEREWOLF))
 	return;

    if( IS_SET(victim->vampire, VAM_HUMAN))
 	return;
	
	if(is_affected(victim, gsn_raging_fever))
		return;

//Raging fever
		af.where     = WHERE_AFFECTS;
		af.type      = gsn_raging_fever;
		af.level     = victim->level;
		af.duration  = victim->level / 6;
	    	af.bitvector	= 0;
		af.location  = APPLY_NONE;
		if(IS_SET(victim->act2, ACT2_ALLOWEMBRACE)){
			if(IS_NPC(ch)){
				af.modifier  = number_range(WERE_WOLF, WERE_CROCODILE);
				af.modifier  += 1300; //Circle 13 - we're doublestacking.
			}else{
				af.modifier  = ch->altform.subtype;
				af.modifier  += ch->werecircle * 100;
			}
		}else
			af.modifier  = -1;
		affect_to_char( victim, &af );

		victim->altform.subtype = ch->altform.subtype;

     return;
}
/**************************************************************/
void do_allowinfect( char_data *ch, char *)
{
    if (!IS_NPC(ch) && !IS_WEREBEAST(ch))
    {
	if(IS_VAMPIRE(ch))
	{
		ch->println("You cannot do that, you are a vampire.");
		return;
	}

	if (IS_AFFECTED2(ch, AFF2_NO_WEREWOLF))
	{
		REMOVE_BIT(ch->act2, ACT2_ALLOWEMBRACE);
		ch->println("You are unable to be turned into a werewolf.");
		return; 
	}

//   if (ch->level > 60)
//	{
//		ch->println( "You are too powerful to be infected.");
//		return;
//	}

	if(IS_SET(ch->act2, ACT2_ALLOWEMBRACE))
	{
		REMOVE_BIT(ch->act2, ACT2_ALLOWEMBRACE);
		ch->println("You refuse to willingly be infected with lycanthropy.");
		return; 
	}
	else
		SET_BIT(ch->act2, ACT2_ALLOWEMBRACE);
		ch->println("You willingly allow yourself to be infected by lycanthropy.");
		return; 
   }
   do_huh(ch,"");
   return;
}

/**************************************************************************/
void do_mountable( char_data *ch, char *)
{
    if ( IS_NPC(ch) )
	return;

	if(!IS_SET(TRUE_CH(ch)->form, FORM_MOUNTABLE))
	{
		do_huh(ch, "");
		return;
	}

    if ( IS_SET(TRUE_CH(ch)->pcdata->pconfig, PCONFIG_MOUNTABLE) )
    {
        REMOVE_BIT(TRUE_CH(ch)->pcdata->pconfig, PCONFIG_MOUNTABLE);
        ch->println( "You now refuse to be ridden." );
    }
    else
    {
        SET_BIT(TRUE_CH(ch)->pcdata->pconfig, PCONFIG_MOUNTABLE);
        ch->println( "You now allow others to ride you." );
    }
    return;
}
/***************************************************************************/
void do_reinforce(char_data *ch, char *argument)
{
	char arg[MIL];
	OBJ_DATA *obj;
	argument = one_argument(argument, arg);
	int skill = get_skill(ch, gsn_reinforce);
	if(skill < 1)
	{
		do_huh(ch, "");
		return;
	}

	if(IS_NULLSTR(arg) ||
	(obj = get_obj_carry(ch, arg)) == NULL){
		ch->println("Reinforce what?");
		return;
	}

	if(obj->item_type != ITEM_WEAPON
	&& obj->item_type != ITEM_ARMOR
	&& obj->item_type != ITEM_CLOTHING
	&& obj->item_type != ITEM_JEWELRY
	&& obj->item_type != ITEM_FURNITURE)
	{
		ch->println("I'm not sure any dwarven ingenuity could reinforce that.");
		return;
	}

	if(obj->durability > obj->pIndexData->durability)
	{
		ch->println("It's already been reinforced unfortunately.");
		return;
	}
	
	if(number_percent() > skill - obj->level)
	{
		ch->println("A little adjustment here, a spot there. Done!!");
		WAIT_STATE(ch, skill_table[gsn_reinforce].beats );
		obj->durability = UMIN(10, obj->durability + number_range(1, 1 + (get_skill(ch, gsn_reinforce) / 3)));
		check_improve(ch, gsn_tinker, false, 4);

	}else{
		ch->println("Nope, no way to make it any better. None that come to mind anyways.");
		WAIT_STATE(ch, skill_table[gsn_reinforce].beats * 3 / 4);
		check_improve(ch, gsn_tinker, true, 3);

	}
	return;
}
/**************************************************************************/

void do_tinker(char_data *ch, char *argument)
{
    char arg[MIL];
    AFFECT_DATA *paf;
    OBJ_DATA *obj;
	int skill;
	
	skill = get_skill(ch,gsn_tinker);
	argument = one_argument(argument, arg);
	if(skill < 1)
	{
		do_huh(ch, "");
		return;
	}

	if(IS_NULLSTR(arg) ||
	(obj = get_obj_carry(ch, arg)) == NULL){
		ch->println("You can't tinker with things you don't have.");
		return;
	}

	if(obj->item_type != ITEM_WEAPON
	&& obj->item_type != ITEM_ARMOR
	&& obj->item_type != ITEM_CLOTHING
	&& obj->item_type != ITEM_JEWELRY
	&& obj->item_type != ITEM_FURNITURE)
	{
		ch->println("Even gnomish creativity has limits.");
		return;
    }


    if (obj->wear_loc != -1) {
        ch->println("The item must be carried to be tinkered with.");
        return;
    }
// Only one weird enchant per item 

    if (IS_SET(obj->extra2_flags,OBJEXTRA2_TINKERED)) {
        ch->println("It's already a gnomish masterpiece!");
        return;
    }

    act("$p begins to change rapidly in your hands.",ch,obj,NULL,TO_CHAR);
    act("$n begins to reshape $p delicately.",ch,obj,NULL,TO_ROOM);
	
	if(number_percent() < skill - obj->level)
	{
		act("$p falls apart.",ch,obj,NULL,TO_CHAR);
		act("$p falls apart.",ch,obj,NULL,TO_ROOM);
		extract_obj(obj);
		ch->mana -= 100;
		WAIT_STATE(ch, skill_table[gsn_tinker].beats *3/4);
		check_improve(ch, gsn_tinker, false, 3);
		return;
	}
	// Add affects depending on what type item this is 
    paf = new_affect();
    paf->where       = WHERE_OBJEXTRA;
    paf->type        = gsn_tinker;
    paf->level =  ch->level;
    paf->duration    = -1;
    paf->bitvector   = 0;

    if ( CAN_WEAR(obj, OBJWEAR_TORSO) ) {
       if (number_percent() <= 60) {
          paf->location = APPLY_HIT;
          paf->modifier = (((number_percent() % 6)) * 10);
          paf->next     = obj->affected;
          obj->affected = paf;
       }
       else {
          paf->location = APPLY_ST;
          paf->modifier = ((number_percent() % 6)) * 2 ;
          paf->next     = obj->affected;
          obj->affected = paf;
       }
    }else if ( CAN_WEAR(obj, OBJWEAR_HEAD) ) {
       if (number_percent() <= 60) {
          paf->location = APPLY_MANA;
          paf->modifier = (((number_percent() % 6)) * 10);
          paf->next     = obj->affected;
          obj->affected = paf;
       }
       else if (number_percent() <= 50) {
          paf->location = APPLY_IN;
          paf->modifier = ((number_percent() % 6)) * 2;
          paf->next     = obj->affected;
          obj->affected = paf;
       }
       else {
          paf->location = APPLY_ME;
          paf->modifier = ((number_percent() % 6)) * 2;
          paf->next     = obj->affected;
          obj->affected = paf;
       }
    }else if ( CAN_WEAR(obj, OBJWEAR_LEGS) ) {
       if (number_percent() <= 60) {
          paf->location = APPLY_MOVE;
          paf->modifier = (((number_percent() % 6)) * 10);
          paf->next     = obj->affected;
          obj->affected = paf;
       }
       else {
          paf->location = APPLY_CO;
          paf->modifier = ((number_percent() % 6)) * 2;
          paf->next     = obj->affected;
          obj->affected = paf;
       }
    }else if ( CAN_WEAR(obj, OBJWEAR_WIELD) ) {
       if (number_percent() <= 10) {
          paf->location = APPLY_HIT;
          paf->modifier = (((number_percent() % 6)) * 10);
          paf->next     = obj->affected;
          obj->affected = paf;
       }
       else if (number_percent() <= 10) {
          paf->location = APPLY_ST;
          paf->modifier = ((number_percent() % 6)) * 2;
          paf->next     = obj->affected;
          obj->affected = paf;
       }
       else if (number_percent() <= 10) {
          paf->location = APPLY_QU;
          paf->modifier = ((number_percent() % 6)) * 2;
          paf->next     = obj->affected;
          obj->affected = paf;
       }
    }else {
		if(number_percent() <= 50) {
          paf->location = APPLY_AC;
          paf->modifier = ((number_percent() % 10)) * 2;
          paf->next     = obj->affected;
          obj->affected = paf;
		}
		else {
          paf->location = APPLY_HIT;
          paf->modifier = ((number_percent() % 6)) * 2;
          paf->next     = obj->affected;
          obj->affected = paf;
		}
	}
		ch->mana -= 100;
		WAIT_STATE(ch, skill_table[gsn_tinker].beats *3/4);
		check_improve(ch, gsn_tinker, true, 2);
		act("$p forms into a gnomish masterpiece.",ch,obj,NULL,TO_CHAR);
		act("$n stops changing $p with a grin.",ch,obj,NULL,TO_ROOM);
		SET_BIT(obj->extra2_flags, OBJEXTRA2_TINKERED);
		return;
}	

int max_gnosis(char_data *ch)
{
	if(IS_WEREBEAST(ch))
	{
		int gp = 0;
		if(ch->werecircle == 13)
		{
			gp = 20;
		} else
		if(ch->werecircle == 12)
		{
			gp = 30;
		} else
		if(ch->werecircle == 11)
		{
			gp = 40;
		} else
		if(ch->werecircle == 10)
		{
			gp = 50;
		} else
		if(ch->werecircle == 9)
		{
			gp = 60;
		} else
		if(ch->werecircle == 8)
		{
			gp = 70;
		} else
		if(ch->werecircle == 7)
		{
			gp = 80;
		} else
		if(ch->werecircle == 6)
		{
			gp = 90;
		} else
		if(ch->werecircle == 5)
		{
			gp = 100;
		} else
		if(ch->werecircle == 4)
		{
			gp = 125;
		} else
		if(ch->werecircle == 3)
		{
			gp = 150;
		} else
		if(ch->werecircle == 2)
		{
			gp = 175;
		} else
		if(ch->werecircle == 1)
		{
			gp = 200;
		}
		return gp;
	}
	return 0;
}

//Small tricks for Jinun to do to display their "mastery":>
void do_display(char_data *ch, char *argument)
{
	if(get_skill(ch, gsn_fire_affinity))
	{
		obj_data *light;
		light = create_object( get_obj_index( OBJ_VNUM_LIGHT_BALL ));
		light->name = str_dup("ball untamed fire");
		light->short_descr = str_dup("a ball of `#`Wu`wn`Rt`ra`Rm`we`Wd`^ `Rfire`^");
		light->description = str_dup("a ball of `#`Wu`wn`Rt`ra`Rm`we`Wd`^ `Rfire`^ fire floats here.");
		light->material = str_dup("fire");
		obj_to_room( light, ch->in_room );
		act( "$n gestures and $p forms in the air!",   ch, light, NULL, TO_ROOM );
		act( "You gesture and $p forms from the light.", ch, light, NULL, TO_CHAR );
		return;
	}

	char arg[MIL];
	char_data *victim;
	argument = one_argument(argument, arg);
	victim = get_char_room(ch, arg);

	if(victim){
		if(get_skill(ch, gsn_water_affinity))
		{
			AFFECT_DATA af;
			if(is_affected(ch, gsn_otterlungs) && ch != victim)
			{
				act( "$n lightly kisses $N's brow.",   ch, NULL, victim, TO_NOTVICT );
				act( "You lightly kiss $N's brow.", ch, NULL, victim, TO_CHAR );
				act( "$n lightly kisses your brow.", ch, NULL, victim, TO_VICT );
				return;
			}
			af.where		= WHERE_AFFECTS;
			af.type			= gsn_otterlungs;
			af.level		= ch->level;
			af.duration		= 10;
			af.location		= APPLY_NONE;
			af.modifier		= 0;
			af.bitvector	= AFF_OTTERLUNGS;
			affect_to_char(victim, &af);
			if(ch != victim){
				act( "$n lightly kisses $N's brow.",   ch, NULL, victim, TO_NOTVICT );
				act( "You bestow the blessings of water on $N.", ch, NULL, victim, TO_CHAR );
				act( "A giddy feeling forms as $n kisses your brow!", ch, NULL, victim, TO_VICT );
			}else
				act( "You unleash the blessings of water.", ch, NULL, NULL, TO_CHAR );
			return;
		}

		if(get_skill(ch, gsn_earth_affinity))
		{
			AFFECT_DATA af;
			if(is_affected(victim, gsn_pass_without_trace) 
			|| IS_AFFECTED2(victim, AFF2_PASSWOTRACE))
			{
				act( "$n waves a hand at $N.",   ch, NULL, victim, TO_NOTVICT );
				act( "You wave a hand at $N.", ch, NULL, victim, TO_CHAR );
				act( "$n waves a hand at you.", ch, NULL, victim, TO_VICT );
				return;
			}
			af.where		= WHERE_AFFECTS;
			af.type			= gsn_pass_without_trace;
			af.level		= ch->level;
			af.duration		= 10;
			af.location		= APPLY_NONE;
			af.modifier		= 0;
			af.bitvector	= AFF2_PASSWOTRACE;
			affect_to_char(victim, &af);
			if(ch != victim){
				act( "$n waves a hand at $N.",   ch, NULL, victim, TO_NOTVICT );
				act( "You mask $N's earthly nature.", ch, NULL, victim, TO_CHAR );
				act( "$n waves a hand at you.", ch, NULL, victim, TO_VICT );
			}else
				act( "You mask your earthly nature.", ch, NULL, NULL, TO_CHAR );
			return;

		}

		if(get_skill(ch, gsn_air_affinity))
		{
			AFFECT_DATA af;
			if(is_affected(victim, gsn_fly) || IS_AFFECTED(victim, AFF_FLYING))
			{
				act( "$n blows on $N.",   ch, NULL, victim, TO_NOTVICT );
				act( "You blow on $N.", ch, NULL, victim, TO_CHAR );
				act( "$n blows on you.", ch, NULL, victim, TO_VICT );
				return;
			}
			af.where		= WHERE_AFFECTS;
			af.type			= gsn_fly;
			af.level		= ch->level;
			af.duration		= 10;
			af.location		= APPLY_NONE;
			af.modifier		= 0;
			af.bitvector	= AFF_FLYING;
			affect_to_char(victim, &af);
			if(ch != victim){
				act( "$n blows $N into the air!",   ch, NULL, victim, TO_NOTVICT );
				act( "You blow $N into the air.", ch, NULL, victim, TO_CHAR );
				act( "$n blows you into the air!", ch, NULL, victim, TO_VICT );
			}else{
				act( "You enter the air effortlessly.", ch, NULL, NULL, TO_CHAR );
				act( "$n enters the air with a leap!",   ch, NULL, NULL, TO_ROOM );
			}
			return;
		}
	}
	do_huh(ch, "");
	return;
}



