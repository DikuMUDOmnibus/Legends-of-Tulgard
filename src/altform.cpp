/**************************************************************************/
// altform.cpp - Alternate Forms code by Reath
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with all the licenses *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/
#include "include.h"

void setup_altmodifiers(char_data *ch);
void setup_altbits(char_data *ch);
void setup_altappearance(char_data *ch);
void setup_altform(char_data *ch, int type, int subtype);
void setup_altshape(char_data *ch);


/***************************************************************************/
bool race_can_wear (char_data *ch, obj_data *obj)
{
	if(IS_NPC(ch) || IS_IMMORTAL(ch))
		return true;

	if( IS_SET(obj->wear_flags, OBJWEAR_FINGER) && 
           !IS_SET(ch->parts, PART_FINGERS))
		return false;

	if( IS_SET(obj->wear_flags, OBJWEAR_HEAD) && 
           !IS_SET(ch->parts, PART_HEAD))
		return false;

	if( IS_SET(obj->wear_flags, OBJWEAR_NECK) && 
           !IS_SET(ch->parts, PART_HEAD))
		return false;

	if( IS_SET(obj->wear_flags, OBJWEAR_HANDS) && 
           !IS_SET(ch->parts, PART_HANDS))
		return false;

	if( IS_SET(obj->wear_flags, OBJWEAR_ARMS) && 
           !IS_SET(ch->parts, PART_ARMS))
		return false;

	if( IS_SET(obj->wear_flags, OBJWEAR_SHIELD) && 
           !IS_SET(ch->parts, PART_ARMS))
		return false;

	if( IS_SET(obj->wear_flags, OBJWEAR_WAIST) && 
           !IS_SET(ch->form, FORM_BIPED) && !IS_SET(ch->form, FORM_CENTAUR) &&   
           !IS_SET(ch->form, FORM_INSECT) && !IS_SET(ch->form, FORM_DRAGON))
		return false;

	if( IS_SET(obj->wear_flags, OBJWEAR_WRIST) && 
           !IS_SET(ch->parts, PART_ARMS))
		return false;

	if( IS_SET(obj->wear_flags, OBJWEAR_ANKLE) && 
           !IS_SET(ch->parts, PART_LEGS))
		return false;

	if( IS_SET(obj->wear_flags, OBJWEAR_EAR) && 
           !IS_SET(ch->parts, PART_EAR))
		return false;

	if( IS_SET(obj->wear_flags, OBJWEAR_LEGS) && 
           (!IS_SET(ch->parts, PART_LEGS) || IS_SET(ch->form, FORM_CENTAUR)))
		return false;

	if( IS_SET(obj->wear_flags, OBJWEAR_FEET) && 
           (!IS_SET(ch->parts, PART_FEET) || IS_SET(ch->form, FORM_CENTAUR)))
		return false;

	if( IS_SET(obj->wear_flags, OBJWEAR_ABOUT) && 
           !IS_SET(ch->form, FORM_BIPED) && !IS_SET(ch->form, FORM_CENTAUR) &&   
           !IS_SET(ch->form, FORM_INSECT) && !IS_SET(ch->form, FORM_DRAGON))
		return false;

	if( IS_SET(obj->wear_flags, OBJWEAR_BACK) && 
           !IS_SET(ch->form, FORM_BIPED) && !IS_SET(ch->form, FORM_CENTAUR) &&   
           !IS_SET(ch->form, FORM_INSECT) && !IS_SET(ch->form, FORM_DRAGON))
		return false;

	return true;

}
/***************************************************************************/
//Function for the initial setup of a dragon character
bool setup_dragon(char_data *ch)
{
	long immune=0;
	bool found = false;

	ch->altform.type = ALTTYPE_DRAGONFORM;

	if ( get_skill(ch,gsn_acid_breath) > 0)
	{
		SET_BIT(immune, IMM_ACID);
		if(IS_GOOD(ch))
			ch->altform.subtype = DRAGON_COPPER;
		else if(IS_EVIL(ch))
			ch->altform.subtype = DRAGON_BLACK;
		else
			ch->altform.subtype = DRAGON_EMERALD;
		found = true;
	}
	if ( get_skill(ch,gsn_fire_breath) > 0)
	{
		SET_BIT(immune, IMM_FIRE);
		if(IS_GOOD(ch))
			ch->altform.subtype = DRAGON_GOLD;
		else if(IS_EVIL(ch))
			ch->altform.subtype = DRAGON_RED;
		found = true;
	}
	if ( get_skill(ch,gsn_frost_breath) > 0)
	{
		SET_BIT(immune, IMM_COLD);
		if(IS_GOOD(ch))
			ch->altform.subtype = DRAGON_SILVER;
		else if(IS_EVIL(ch))
			ch->altform.subtype = DRAGON_CLOUD;
		else
			ch->altform.subtype = DRAGON_WHITE;
		found = true;
	}
	if ( get_skill(ch,gsn_gas_breath) > 0)
	{
		SET_BIT(immune, IMM_POISON);
		if(IS_GOOD(ch))
			ch->altform.subtype = DRAGON_BRASS;
		else if(IS_EVIL(ch))
			ch->altform.subtype = DRAGON_GREEN;
		found = true;
	}
	if ( get_skill(ch,gsn_lightning_breath) > 0)
	{
		SET_BIT(immune, IMM_LIGHTNING);
		if(IS_GOOD(ch))
			ch->altform.subtype = DRAGON_BRONZE;
		else if(IS_EVIL(ch))
			ch->altform.subtype = DRAGON_CRYSTAL;
		else
			ch->altform.subtype = DRAGON_BLUE;
		found = true;
	}
	if ( get_skill(ch,gsn_shadow_breath) > 0)
	{
		ch->altform.subtype = DRAGON_SHADOW;
		found = true;
	}
	if ( get_skill(ch,gsn_steel_breath) > 0)
	{
		ch->altform.subtype = DRAGON_STEEL;
		found = true;
	}
	
	if(found == true)
	{
		SET_BIT(ch->imm_flags, immune);
		setup_altmodifiers(ch);
		setup_altbits(ch);
		setup_altshape(ch);
		save_char_obj( ch );
		return true;
	}
	return false;
}		
/***************************************************************************/
bool setup_werebeast(char_data *ch)
{
	long immune=0;
	bool found = false;

	if(!ch->altform.type)
	ch->altform.type = ALTTYPE_LYCANTHROPE;
	if (!ch->altform.subtype)
		ch->altform.subtype= WERE_WOLF;

	switch(ch->altform.subtype)
	{
		case WERE_WOLF: 
			SET_BIT(immune, IMM_CHARM);
			SET_BIT(immune, IMM_DISEASE);
			found = true;
			break;
		case WERE_BEAR: 
		SET_BIT(immune, IMM_CHARM);
		SET_BIT(immune, IMM_DISEASE);
		found = true;
			break;
		case WERE_BOAR: 
			SET_BIT(immune, IMM_CHARM);
			SET_BIT(immune, IMM_DISEASE);
			found = true;
			break;
		case WERE_COBRA: 
		        SET_BIT(immune, IMM_CHARM);
			SET_BIT(immune, IMM_DISEASE);
		        SET_BIT(immune, IMM_POISON);
		found = true;
			break;
		case WERE_CROCODILE: 
			SET_BIT(immune, IMM_CHARM);
			SET_BIT(immune, IMM_DISEASE);
			found = true;
			break;
		case WERE_BAT: 
			SET_BIT(immune, IMM_CHARM);
			SET_BIT(immune, IMM_DISEASE);
			found = true;
			break;
		case WERE_RAT: 
			SET_BIT(immune, IMM_CHARM);
			SET_BIT(immune, IMM_DISEASE);
			found = true;
			break;
		case WERE_TIGER: 
			SET_BIT(immune, IMM_CHARM);
			SET_BIT(immune, IMM_DISEASE);
			found = true;
			break;
	}

	if(found == true)
	{
		SET_BIT(ch->altform.immune, immune);
		if(IS_WEREFORM(ch)) {
			SET_BIT(ch->imm_flags, immune);
		}
		else {
			ch->imm_flags   = ch->imm_flags | race_table[ch->race]->imm;
		}

		if(!ch->altform.short_descr){
			setup_altappearance(ch);
		}
		setup_altmodifiers(ch);
		setup_altbits(ch);
		setup_altshape(ch);
		save_char_obj( ch );
		return true;
	}
	return false;
}		
/***************************************************************************/
//Changes old race-tranformers into the new system for those shapechanged
void revert_race1(char_data *ch)
{	
	// Move PC to OOC for safety reasons
	char_from_room( ch );
    	char_to_room( ch, get_room_index(30000));

	ch->race = ch->orig_race;
	char_data *shifter;
	shifter = create_mobile( get_mob_index(60008), 0 );  
	// Copy alternate descriptions to mob
	replace_string(shifter->short_descr, ch->short_descr);
	replace_string(shifter->description, ch->description);
	// Copy mobs alternate desc to new altform desc
	replace_string(ch->altform.short_descr, shifter->short_descr);
	replace_string(ch->altform.description, shifter->description);
	// Copy original descriptions to mob
	replace_string(shifter->short_descr, ch->orig_short_descr);
	replace_string(shifter->description, ch->orig_description);
	// Copy mobs original desc to normal player desc
	replace_string(ch->short_descr, shifter->short_descr);
	replace_string(ch->description, shifter->description);
	extract_char(shifter,true);

	ch->size  	= ch->size 	 | race_table[ch->race]->size;
	ch->imm_flags 	= ch->imm_flags  | race_table[ch->race]->imm;
	ch->res_flags 	= ch->res_flags  | race_table[ch->race]->res;
	ch->vuln_flags 	= ch->vuln_flags | race_table[ch->race]->vuln;
	ch->form 	= ch->form 	 | race_table[ch->race]->form;
	ch->parts 	= ch->parts 	 | race_table[ch->race]->parts;

    	// now restore the character to his/her true condition 
    	int stat;
    	for(stat = 0; stat < MAX_STATS; stat++)
    	{
            	ch->modifiers[stat]=0;
		if(GAMESETTING(GAMESET_USE_ROLEMASTER_MODIFIERS)){
			if(ch->perm_stats[stat] > 90){
				ch->modifiers[stat] = (ch->perm_stats[stat]-90)*2+20;
			}else if(ch->perm_stats[stat] > 70){
				ch->modifiers[stat] = ch->perm_stats[stat]-70;
			}else if(ch->perm_stats[stat] < 26){
				ch->modifiers[stat] = -26 + ch->perm_stats[stat]; 
			}
		}else{
			if(ch->perm_stats[stat] > 95){
				ch->modifiers[stat] = (ch->perm_stats[stat]-95)*3+45;
			}else if(ch->perm_stats[stat] > 85){
				ch->modifiers[stat] = (ch->perm_stats[stat]-85)*2+25;
			}else if(ch->perm_stats[stat] > 60){
					ch->modifiers[stat] = ch->perm_stats[stat]-60;
			}else if(ch->perm_stats[stat] < 26){
					ch->modifiers[stat] = -26 + ch->perm_stats[stat];
			}
		}
           	ch->modifiers[stat] += race_table[ch->race]->stat_modifier[stat];
    	}

	reset_char(ch);
	save_char_obj( ch );

	if(ch->altform.type == ALTTYPE_DRAGONFORM)
	{
		if(setup_dragon(ch) == true)
		{
			ch->println("\r\n");
			ch->println("`#`Y*******************************************`^");
			ch->println("`#`Y*        DRAGON CONVERSION SUCCESSFUL.    *`^");
			ch->println("`#`Y*******************************************`^");
			ch->println("\r\n");
			wiznet("$N DRAGON CONVERSION SUCCESSFUL.",ch,NULL,WIZ_SECURE,0,0);
			return;
		}
	    	ch->println("`#`RERROR IN DRAGON CONVERSION. Please see an IMMORTAL.`^");
		wiznet("`#`Y$N DRAGON CONVERSION FAILURE.`^",ch,NULL,WIZ_SECURE,0,0);
		return;
	}
	else if(ch->altform.type == ALTTYPE_LYCANTHROPE)
	{
		if(setup_werebeast(ch) == true)
		{
			save_char_obj( ch );
			ch->println("\r\n");
			ch->println("`#`Y*******************************************`^");
			ch->println("`#`Y*      WEREWOLF CONVERSION SUCCESSFUL.    *`^");
			ch->println("`#`Y*******************************************`^");
			ch->println("\r\n");
			wiznet("$N WEREWOLF CONVERSION SUCCESSFUL.",ch,NULL,WIZ_SECURE,0,0);
			return;
		}
	    	ch->println("`#`RERROR IN WEREWOLF CONVERSION. Please see an IMMORTAL.`^");
		wiznet("`#`Y$N WEREWOLF CONVERSION FAILURE.`^",ch,NULL,WIZ_SECURE,0,0);
		return;
	}
}
/***************************************************************************/
//Changes old race-tranformers into the new system for those not shapechanged
void revert_race2(char_data *ch)
{	
	// Move PC to OOC for safety reasons
	char_from_room( ch );
    	char_to_room( ch, get_room_index(30000));

	char_data *shifter;
	shifter = create_mobile( get_mob_index(60008), 0 );  
	// Copy alternate descriptions to mob
	replace_string(shifter->short_descr, ch->orig_short_descr);
	replace_string(shifter->description, ch->orig_description);
	// Copy mobs alternate desc to new altform desc
	replace_string(ch->altform.short_descr, shifter->short_descr);
	replace_string(ch->altform.description, shifter->description);
	extract_char(shifter,true);

	save_char_obj( ch );

	if(ch->altform.type == ALTTYPE_DRAGONFORM)
	{
		if(setup_dragon(ch) == true)
		{
			ch->println("\r\n");
			ch->println("`#`Y*******************************************`^");
			ch->println("`#`Y*        DRAGON CONVERSION SUCCESSFUL.    *`^");
			ch->println("`#`Y*******************************************`^");
			ch->println("\r\n");
			wiznet("$N DRAGON CONVERSION SUCCESSFUL.",ch,NULL,WIZ_SECURE,0,0);
			return;
		}
	    	ch->println("`#`RERROR IN DRAGON CONVERSION. Please see an IMMORTAL.`^");
		wiznet("`#`Y$N DRAGON CONVERSION FAILURE.`^",ch,NULL,WIZ_SECURE,0,0);
		return;
	}
	else if(ch->altform.type == ALTTYPE_LYCANTHROPE)
	{
		if(setup_werebeast(ch) == true)
		{
			ch->println("\r\n");
			ch->println("`#`Y*******************************************`^");
			ch->println("`#`Y*      WEREWOLF CONVERSION SUCCESSFUL.    *`^");
			ch->println("`#`Y*******************************************`^");
			ch->println("\r\n");
			wiznet("$N WEREWOLF CONVERSION SUCCESSFUL.",ch,NULL,WIZ_SECURE,0,0);
			return;
		}
	    	ch->println("`#`RERROR IN WEREWOLF CONVERSION. Please see an IMMORTAL.`^");
		wiznet("`#`Y$N WEREWOLF CONVERSION FAILURE.`^",ch,NULL,WIZ_SECURE,0,0);
		return;
	}
}
/***************************************************************************/
void setup_altform(char_data *ch, int alttype, int altsubtype)
{
	if(IS_NPC(ch) || !(alttype) || altsubtype < 0)
		return;

	ch->altform.type = alttype;
	ch->altform.subtype = altsubtype;
	setup_altmodifiers(ch);
	setup_altshape(ch);
	setup_altbits(ch);
	if(!ch->altform.short_descr)
		setup_altappearance(ch);
	return;
}

/***************************************************************************/
void setup_altmodifiers(char_data *ch)
{
	switch(ch->altform.type)
	{
		case ALTTYPE_LYCANTHROPE:	
			switch(ch->altform.subtype)
			{
				case WERE_WOLF:
					ch->altform.modifier[STAT_ST] =  30;
					ch->altform.modifier[STAT_QU] =  30;
					ch->altform.modifier[STAT_PR] = -20;
					ch->altform.modifier[STAT_EM] =  25;
					ch->altform.modifier[STAT_IN] = -50;
					ch->altform.modifier[STAT_CO] =  25;
					ch->altform.modifier[STAT_AG] =  40;
					ch->altform.modifier[STAT_SD] =   0;
					ch->altform.modifier[STAT_ME] = -35;
					ch->altform.modifier[STAT_RE] =   0;
					break;
				case WERE_BEAR:
					ch->altform.modifier[STAT_ST] =  70;
					ch->altform.modifier[STAT_QU] = -45;
					ch->altform.modifier[STAT_PR] =   5;
					ch->altform.modifier[STAT_EM] =  20;
					ch->altform.modifier[STAT_IN] = -25;
					ch->altform.modifier[STAT_CO] =  20;
					ch->altform.modifier[STAT_AG] =  20;
					ch->altform.modifier[STAT_SD] = -30;
					ch->altform.modifier[STAT_ME] =  10;
					ch->altform.modifier[STAT_RE] =   0;
					break;
				case WERE_BOAR:
					ch->altform.modifier[STAT_ST] =  35;
					ch->altform.modifier[STAT_QU] =  30;
					ch->altform.modifier[STAT_PR] = -10;
					ch->altform.modifier[STAT_EM] =   0;
					ch->altform.modifier[STAT_IN] = -40;
					ch->altform.modifier[STAT_CO] =  25;
					ch->altform.modifier[STAT_AG] = -10;
					ch->altform.modifier[STAT_SD] =  20;
					ch->altform.modifier[STAT_ME] =  -5;
					ch->altform.modifier[STAT_RE] =   0;
					break;
				case WERE_RAT:
					ch->altform.modifier[STAT_ST] = -55;
					ch->altform.modifier[STAT_QU] =  60;
					ch->altform.modifier[STAT_PR] = -35;
					ch->altform.modifier[STAT_EM] =   0;
					ch->altform.modifier[STAT_IN] =  30;
					ch->altform.modifier[STAT_CO] =  10;
					ch->altform.modifier[STAT_AG] =  30;
					ch->altform.modifier[STAT_SD] = -10;
					ch->altform.modifier[STAT_ME] =  10;
					ch->altform.modifier[STAT_RE] =   5;
					break;
				case WERE_COBRA:
					ch->altform.modifier[STAT_ST] =  45;
					ch->altform.modifier[STAT_QU] =  20;
					ch->altform.modifier[STAT_PR] = -10;
					ch->altform.modifier[STAT_EM] =  10;
					ch->altform.modifier[STAT_IN] = -50;
					ch->altform.modifier[STAT_CO] =  45;
					ch->altform.modifier[STAT_AG] =  20;
					ch->altform.modifier[STAT_SD] =  15;
					ch->altform.modifier[STAT_ME] = -35;
					ch->altform.modifier[STAT_RE] = -15;
					break;
				case WERE_CROCODILE:
					ch->altform.modifier[STAT_ST] =  40;
					ch->altform.modifier[STAT_QU] =  30;
					ch->altform.modifier[STAT_PR] =  20;
					ch->altform.modifier[STAT_EM] = -40;
					ch->altform.modifier[STAT_IN] = -20;
					ch->altform.modifier[STAT_CO] =  15;
					ch->altform.modifier[STAT_AG] =   5;
					ch->altform.modifier[STAT_SD] =  20;
					ch->altform.modifier[STAT_ME] = -20;
					ch->altform.modifier[STAT_RE] =  -5;
					break;
				case WERE_BAT:
					ch->altform.modifier[STAT_ST] =  10;
					ch->altform.modifier[STAT_QU] =   5;
					ch->altform.modifier[STAT_PR] =  15;
					ch->altform.modifier[STAT_EM] =   5;
					ch->altform.modifier[STAT_IN] =  -5;
					ch->altform.modifier[STAT_CO] = -15;
					ch->altform.modifier[STAT_AG] =  20;
					ch->altform.modifier[STAT_SD] = -10;
					ch->altform.modifier[STAT_ME] =  10;
					ch->altform.modifier[STAT_RE] =  10;
					break;
				case WERE_TIGER:
			ch->altform.modifier[STAT_ST] =  30;
			ch->altform.modifier[STAT_QU] =  30;
			ch->altform.modifier[STAT_PR] = -20;
			ch->altform.modifier[STAT_EM] =  25;
			ch->altform.modifier[STAT_IN] = -50;
			ch->altform.modifier[STAT_CO] =  25;
			ch->altform.modifier[STAT_AG] =  40;
			ch->altform.modifier[STAT_SD] =   0;
			ch->altform.modifier[STAT_ME] = -35;
			ch->altform.modifier[STAT_RE] =   0;
			break;
			}
			break;
		case ALTTYPE_DRAGONFORM:	
			ch->altform.modifier[STAT_ST] = -25;
			ch->altform.modifier[STAT_QU] = -15;
			ch->altform.modifier[STAT_PR] =  -5;
			ch->altform.modifier[STAT_EM] = -10;
			ch->altform.modifier[STAT_IN] =   0;
			ch->altform.modifier[STAT_CO] = -10;
			ch->altform.modifier[STAT_AG] = -15;
			ch->altform.modifier[STAT_SD] = 0;
			ch->altform.modifier[STAT_ME] = -10;
			ch->altform.modifier[STAT_RE] = -10;
			break;
	}
	return;
}
/***************************************************************************/
void setup_altshape(char_data *ch)
{
	switch(ch->altform.type)
	{
		case ALTTYPE_LYCANTHROPE :	
			SET_BIT(ch->altform.parts, PART_FANGS);
			SET_BIT(ch->altform.parts, PART_FINGERS);
			SET_BIT(ch->altform.parts, PART_EAR);
			SET_BIT(ch->altform.parts, PART_EYE);
			SET_BIT(ch->altform.parts, PART_FEET);
			SET_BIT(ch->altform.parts, PART_HANDS);
			SET_BIT(ch->altform.parts, PART_GUTS);
			SET_BIT(ch->altform.parts, PART_BRAINS);
			SET_BIT(ch->altform.parts, PART_HEART);
			SET_BIT(ch->altform.parts, PART_HEAD);
			SET_BIT(ch->altform.parts, PART_ARMS);
			SET_BIT(ch->altform.form, FORM_EDIBLE);
			SET_BIT(ch->altform.form, FORM_BIPED);
			switch(ch->altform.subtype)
			{
				case WERE_WOLF :
					ch->altform.size = SIZE_LARGE;
					SET_BIT(ch->altform.parts, PART_CLAWS);
					SET_BIT(ch->altform.parts, PART_TAIL);
					SET_BIT(ch->altform.parts, PART_LEGS);
					SET_BIT(ch->altform.form, FORM_MAMMAL);
					break;
				case WERE_RAT :
					ch->altform.size = SIZE_SMALL;
					SET_BIT(ch->altform.parts, PART_CLAWS);
					SET_BIT(ch->altform.parts, PART_TAIL);
					SET_BIT(ch->altform.parts, PART_LEGS);
					SET_BIT(ch->altform.form, FORM_MAMMAL);
					break;
				case WERE_BAT :
					ch->altform.size = SIZE_SMALL;
					SET_BIT(ch->altform.parts, PART_WINGS);
					SET_BIT(ch->altform.parts, PART_LEGS);
					SET_BIT(ch->altform.form, FORM_MAMMAL);
					break;
				case WERE_BOAR :
					ch->altform.size = SIZE_MEDIUM;
					SET_BIT(ch->altform.parts, PART_TUSKS);
					SET_BIT(ch->altform.parts, PART_LEGS);
					SET_BIT(ch->altform.form, FORM_MAMMAL);
					break;
				case WERE_BEAR :
					ch->altform.size = SIZE_HUGE;
					SET_BIT(ch->altform.parts, PART_CLAWS);
					SET_BIT(ch->altform.parts, PART_LEGS);
					SET_BIT(ch->altform.form, FORM_MAMMAL);
					break;
				case WERE_COBRA :
					ch->altform.size = SIZE_MEDIUM;
					SET_BIT(ch->altform.parts, PART_TAIL);
					SET_BIT(ch->altform.parts, PART_LONG_TONGUE);
					SET_BIT(ch->altform.form, FORM_REPTILE);
					break;
				case WERE_TIGER :
					ch->altform.size = SIZE_LARGE;
					SET_BIT(ch->altform.parts, PART_CLAWS);
					SET_BIT(ch->altform.parts, PART_TAIL);
					SET_BIT(ch->altform.parts, PART_LEGS);
					SET_BIT(ch->altform.form, FORM_MAMMAL);
					break;
				case WERE_CROCODILE :
					ch->altform.size = SIZE_HUGE;
					SET_BIT(ch->altform.parts, PART_TAIL);
					SET_BIT(ch->altform.parts, PART_LEGS);
					SET_BIT(ch->altform.form, FORM_REPTILE);
					break;
			}
			break;
		case ALTTYPE_DRAGONFORM :	
			ch->altform.size = SIZE_MEDIUM;
			SET_BIT(ch->altform.parts, PART_HEAD);
			SET_BIT(ch->altform.parts, PART_ARMS);
			SET_BIT(ch->altform.parts, PART_HANDS);
			SET_BIT(ch->altform.parts, PART_FEET);
			SET_BIT(ch->altform.parts, PART_EYE);
			SET_BIT(ch->altform.parts, PART_EAR);
			SET_BIT(ch->altform.parts, PART_GUTS);
			SET_BIT(ch->altform.parts, PART_BRAINS);
			SET_BIT(ch->altform.parts, PART_FINGERS);
			SET_BIT(ch->altform.parts, PART_LEGS);
			SET_BIT(ch->altform.form, FORM_MAMMAL);
			SET_BIT(ch->altform.form, FORM_BIPED);
			SET_BIT(ch->altform.form, FORM_SENTIENT);
			SET_BIT(ch->altform.form, FORM_EDIBLE);
			break;
	}
	return;
}
/***************************************************************************/
void setup_altbits(char_data *ch)
{
	switch(ch->altform.type)
	{
		case ALTTYPE_LYCANTHROPE :	
			SET_BIT(ch->altform.resist, RES_SLASH);
			SET_BIT(ch->altform.vuln, VULN_IRON);
			SET_BIT(ch->altform.vuln, VULN_SILVER);
			SET_BIT(ch->altform.aff, AFF_HASTE);
			switch(ch->altform.subtype)
			{
				case WERE_COBRA: SET_BIT(ch->altform.immune, IMM_POISON);
								 break;
				case WERE_BAT:	 SET_BIT(ch->altform.resist, RES_SOUND);
								 break;
				case WERE_RAT:	 SET_BIT(ch->altform.resist, RES_HUNGER);
								 SET_BIT(ch->altform.resist, RES_THIRST);
								 break;
			}
			break;
		case ALTTYPE_DRAGONFORM  :  
			break;
	}
	return;
}
/***************************************************************************/
void setup_altappearance(char_data *ch)
{
	if(!IS_NULLSTR(ch->altform.short_descr) || !IS_NULLSTR(ch->altform.description))
		return;

	switch(ch->altform.type)
	{
		case ALTTYPE_LYCANTHROPE : 
			switch(ch->altform.subtype){
				case WERE_WOLF:
			if(ch->sex == SEX_MALE)
			{
				ch->altform.short_descr = str_dup("a large grey male werebeast");
				ch->altform.description = str_dup("a large grey male werebeast\n\r");
			}else
			{	ch->altform.short_descr = str_dup("a large grey female werebeast");
				ch->altform.description = str_dup("a large grey female werebeast.\n\r");
			}
			break;
				case WERE_COBRA:
					if(ch->sex == SEX_MALE)
					{
						ch->altform.short_descr = str_dup("a male werecobra");
						ch->altform.description = str_dup("a male werecobra\n\r");
					}else
					{	ch->altform.short_descr = str_dup("a female werecobra");
						ch->altform.description = str_dup("a female werecobra.\n\r");
					}
					break;
				case WERE_BEAR:
					if(ch->sex == SEX_MALE)
					{
						ch->altform.short_descr = str_dup("a huge brown male werebear");
						ch->altform.description = str_dup("a huge brown male werebear\n\r");
					}else
					{	ch->altform.short_descr = str_dup("a huge brown female werebear");
						ch->altform.description = str_dup("a huge brown female werebear.\n\r");
					}
					break;
				case WERE_BOAR:
					if(ch->sex == SEX_MALE)
					{
						ch->altform.short_descr = str_dup("a black male wereboar");
						ch->altform.description = str_dup("a black male wereboar\n\r");
					}else
					{	ch->altform.short_descr = str_dup("a black female wereboar");
						ch->altform.description = str_dup("a black female wereboar.\n\r");
					}
					break;
				case WERE_TIGER:
					if(ch->sex == SEX_MALE)
					{
						ch->altform.short_descr = str_dup("a large grey male werebeast");
						ch->altform.description = str_dup("a large grey male werebeast\n\r");
					}else
					{	ch->altform.short_descr = str_dup("a large grey female werebeast");
						ch->altform.description = str_dup("a large grey female werebeast.\n\r");
					}
					break;
				case WERE_BAT:
					if(ch->sex == SEX_MALE)
					{
						ch->altform.short_descr = str_dup("a small male werebat");
						ch->altform.description = str_dup("a small male werebat\n\r");
					}else
					{	ch->altform.short_descr = str_dup("a small female werebat");
						ch->altform.description = str_dup("a small female werebat.\n\r");
					}
					break;
				case WERE_RAT:
					if(ch->sex == SEX_MALE)
					{
						ch->altform.short_descr = str_dup("a small male wererat");
						ch->altform.description = str_dup("a small male wererat\n\r");
					}else
					{	ch->altform.short_descr = str_dup("a small female wererat");
						ch->altform.description = str_dup("a small female wererat.\n\r");
					}
					break;
				case WERE_CROCODILE:
					if(ch->sex == SEX_MALE)
					{
						ch->altform.short_descr = str_dup("a huge male werecrocodile");
						ch->altform.description = str_dup("a huge male werecrocodile\n\r");
					}else
					{	ch->altform.short_descr = str_dup("a huge female werecrocodile");
						ch->altform.description = str_dup("a huge female werecrocodile.\n\r");
					}
					break;
			}
			break;
		case ALTTYPE_DRAGONFORM  : 
			if(ch->sex == SEX_MALE)
			{
			  	ch->altform.short_descr = str_dup("a male person");
				ch->altform.description = str_dup("a male person.\n\r");
			}else
			{  	ch->altform.short_descr = str_dup("a female person");
				ch->altform.description = str_dup("a female person.\n\r");
			}
			break;
	}
	return;
}
/***************************************************************************/
bool form_alternate(char_data *ch)
{
	bool success=false;

    	if(!ch->orig_short_descr)
    	{
		return false;
    	}
    	if(!ch->orig_description)
    	{
		return false;
    	}
	setup_altshape(ch);
	setup_altbits(ch);
	setup_altmodifiers(ch);
	if(ch->altform.size) {
		ch->size = ch->altform.size; }
	if(ch->altform.immune) {
		ch->imm_flags = ch->altform.immune; }
	if(ch->altform.resist) {
		ch->res_flags = ch->altform.resist; }
	if(ch->altform.vuln) {
		ch->vuln_flags = ch->altform.vuln; }
	if(ch->altform.form) {
		ch->form = ch->altform.form; }
	if(ch->altform.parts) {
		ch->parts = ch->altform.parts; }

	for(int i = 0; i < MAX_STATS;i++)
	{
		if(ch->altform.modifier[i])
		{
			ch->modifiers[i] = ch->altform.modifier[i];
			//WereCircle boosts all positive and negative scores by 1
			if(IS_WEREBEAST(ch) && ch->werecircle <= 10)
				ch->modifiers[i] += ch->altform.modifier[i] / 10 * (11 - ch->werecircle);
		}
	}
	switch(ch->altform.type)
	{
		case ALTTYPE_LYCANTHROPE :
			switch(ch->altform.subtype){
				case 0:
				default:

					affect_parentspellfunc_strip( ch, gsn_werewolf);
    					AFFECT_DATA af;
    					af.where     = WHERE_AFFECTS;
    					af.type      = gsn_werewolf;
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
    					ch->pcdata->learned[gsn_werewolf] = 100;
    					ch->pcdata->learned[gsn_tail] = 100;
    					ch->pcdata->learned[gsn_claw] = 100;
					ch->altformed = true;
					success = true;
					break;
			}
			break;

		case ALTTYPE_DRAGONFORM  : 
			if(ch->altform.subtype == DRAGON_RED)
			{
    				ch->pcdata->learned[gsn_fire_breath] = 0;
				ch->altformed = true;
				success = true;
			}
			if(ch->altform.subtype == DRAGON_BLUE)
			{
    				ch->pcdata->learned[gsn_lightning_breath] = 0;
				ch->altformed = true;
				success = true;
			}
			if(ch->altform.subtype == DRAGON_BLACK)
			{
    				ch->pcdata->learned[gsn_acid_breath] = 0;
				ch->altformed = true;
				success = true;
			}
			if(ch->altform.subtype == DRAGON_GREEN)
			{
    				ch->pcdata->learned[gsn_gas_breath] = 0;
				ch->altformed = true;
				success = true;
			}
			if(ch->altform.subtype == DRAGON_WHITE)
			{
    				ch->pcdata->learned[gsn_frost_breath] = 0;
				ch->altformed = true;
				success = true;
			}
			if(ch->altform.subtype == DRAGON_COPPER)
			{
    				ch->pcdata->learned[gsn_acid_breath] = 0;
				ch->altformed = true;
				success = true;
			}
			if(ch->altform.subtype == DRAGON_BRASS)
			{
    				ch->pcdata->learned[gsn_gas_breath] = 0;
				ch->altformed = true;
				success = true;
			}
			if(ch->altform.subtype == DRAGON_BRONZE)
			{
    				ch->pcdata->learned[gsn_lightning_breath] = 0;
				ch->altformed = true;
				success = true;
			}
			if(ch->altform.subtype == DRAGON_SILVER)
			{
    				ch->pcdata->learned[gsn_frost_breath] = 0;
				ch->altformed = true;
				success = true;
			}
			if(ch->altform.subtype == DRAGON_GOLD)
			{
    				ch->pcdata->learned[gsn_fire_breath] = 0;
				ch->altformed = true;
				success = true;
			}
			if(ch->altform.subtype == DRAGON_CRYSTAL)
			{
    				ch->pcdata->learned[gsn_lightning_breath] = 0;
				ch->altformed = true;
				success = true;
			}
			if(ch->altform.subtype == DRAGON_EMERALD)
			{
    				ch->pcdata->learned[gsn_acid_breath] = 0;
				ch->altformed = true;
				success = true;
			}
			if(ch->altform.subtype == DRAGON_CLOUD)
			{
    				ch->pcdata->learned[gsn_frost_breath] = 0;
				ch->altformed = true;
				success = true;
			}
			if(ch->altform.subtype == DRAGON_STEEL)
			{
    				ch->pcdata->learned[gsn_steel_breath] = 0;
				ch->altformed = true;
				success = true;
			}
			if(ch->altform.subtype == DRAGON_SHADOW)
			{
    				ch->pcdata->learned[gsn_shadow_breath] = 0;
				ch->altformed = true;
				success = true;
			}
			break;
	}
 
	if(success == true)
	{
    		char_data *shifter;
    		shifter = create_mobile( get_mob_index(60008), 0 );
		// Copy normal descriptions to mob
		replace_string(shifter->short_descr, ch->short_descr);
		replace_string(shifter->description, ch->description);
		// Copy normal desc to original player desc
		replace_string(ch->orig_short_descr, shifter->short_descr);
		replace_string(ch->orig_description, shifter->description);
		// Copy alternate desc to mob
    		replace_string(shifter->short_descr, ch->altform.short_descr);
    		replace_string(shifter->description, ch->altform.description);
		// Copy alternate mob desc to player
    		replace_string(ch->short_descr, shifter->short_descr);
    		replace_string(ch->description, shifter->description);
    		extract_char(shifter,true);
    		reset_char(ch);
    		save_char_obj( ch );
    		update_pos( ch );
		return true;
	}
	return false;
}
/***************************************************************************/
bool form_original(char_data *ch)
{
	bool success=false;

    	if(!ch->orig_short_descr)
    	{
		return false;
    	}
    	if(!ch->orig_description)
    	{
		return false;
    	}

	ch->size  	= ch->size 	 | race_table[ch->race]->size;
	ch->imm_flags 	= ch->imm_flags  | race_table[ch->race]->imm;
	ch->res_flags 	= ch->res_flags  | race_table[ch->race]->res;
	ch->vuln_flags 	= ch->vuln_flags | race_table[ch->race]->vuln;
	ch->form 	= ch->form 	 | race_table[ch->race]->form;
	ch->parts 	= ch->parts 	 | race_table[ch->race]->parts;

    	// now restore the character to his/her true condition 
    	int stat;
    	for(stat = 0; stat < MAX_STATS; stat++)
    	{
            	ch->modifiers[stat]=0;
		if(GAMESETTING(GAMESET_USE_ROLEMASTER_MODIFIERS)){
			if(ch->perm_stats[stat] > 90){
				ch->modifiers[stat] = (ch->perm_stats[stat]-90)*2+20;
			}else if(ch->perm_stats[stat] > 70){
				ch->modifiers[stat] = ch->perm_stats[stat]-70;
			}else if(ch->perm_stats[stat] < 26){
				ch->modifiers[stat] = -26 + ch->perm_stats[stat]; 
			}
		}else{
			if(ch->perm_stats[stat] > 95){
				ch->modifiers[stat] = (ch->perm_stats[stat]-95)*3+45;
			}else if(ch->perm_stats[stat] > 85){
				ch->modifiers[stat] = (ch->perm_stats[stat]-85)*2+25;
			}else if(ch->perm_stats[stat] > 60){
					ch->modifiers[stat] = ch->perm_stats[stat]-60;
			}else if(ch->perm_stats[stat] < 26){
					ch->modifiers[stat] = -26 + ch->perm_stats[stat];
			}
		}
           	ch->modifiers[stat] += race_table[ch->race]->stat_modifier[stat];
    	}

	switch(ch->altform.type)
	{
		case ALTTYPE_LYCANTHROPE :

			affect_parentspellfunc_strip( ch, gsn_werewolf);
   			affect_parentspellfunc_strip( ch, gsn_augment_hearing);
   			ch->pcdata->learned[gsn_werewolf] = 0;
   			ch->pcdata->learned[gsn_tail] = 0;
   			ch->pcdata->learned[gsn_claw] = 0;
			SET_BIT(ch->imm_flags, IMM_CHARM);
			SET_BIT(ch->imm_flags, IMM_DISEASE);
			ch->pcdata->learned[gsn_werewolf] = 0;
			ch->pcdata->learned[gsn_tail] = 0;
			ch->pcdata->learned[gsn_claw] = 0;

			if(ch->altform.subtype == WERE_WOLF)
			{
				ch->pcdata->learned[gsn_claw] = 0;
				ch->altformed = false;
				success = true;
			}
			if(ch->altform.subtype == WERE_BEAR)
			{
				ch->altformed = false;
				success = true;
			}
			if(ch->altform.subtype == WERE_BOAR)
			{
				ch->altformed = false;
				success = true;
			}
			if(ch->altform.subtype == WERE_TIGER)
			{
				ch->altformed = false;
				success = true;
			}
			if(ch->altform.subtype == WERE_BAT)
			{
				ch->altformed = false;
				success = true;
			}
			if(ch->altform.subtype == WERE_RAT)
			{
				ch->altformed = false;
				success = true;
			}
			if(ch->altform.subtype == WERE_COBRA)
			{
				ch->altformed = false;
				success = true;
			}
			if(ch->altform.subtype == WERE_CROCODILE)
			{
				ch->altformed = false;
				success = true;
			}
			break;

		case ALTTYPE_DRAGONFORM  : 
			if(ch->altform.subtype == DRAGON_RED)
			{
    				ch->pcdata->learned[gsn_fire_breath] = 100;
				ch->altformed = false;
				SET_BIT(ch->imm_flags, IMM_FIRE);
				success = true;
			}
			if(ch->altform.subtype == DRAGON_BLUE)
			{
    				ch->pcdata->learned[gsn_lightning_breath] = 100;
				ch->altformed = false;
				SET_BIT(ch->imm_flags, IMM_LIGHTNING);
				success = true;
			}
			if(ch->altform.subtype == DRAGON_BLACK)
			{
    				ch->pcdata->learned[gsn_acid_breath] = 100;
				ch->altformed = false;
				SET_BIT(ch->imm_flags, IMM_ACID);
				success = true;
			}
			if(ch->altform.subtype == DRAGON_GREEN)
			{
    				ch->pcdata->learned[gsn_gas_breath] = 100;
				ch->altformed = false;
				SET_BIT(ch->imm_flags, IMM_POISON);
				success = true;
			}
			if(ch->altform.subtype == DRAGON_WHITE)
			{
    				ch->pcdata->learned[gsn_frost_breath] = 100;
				ch->altformed = false;
				SET_BIT(ch->imm_flags, IMM_COLD);
				success = true;
			}
			if(ch->altform.subtype == DRAGON_COPPER)
			{
    				ch->pcdata->learned[gsn_acid_breath] = 100;
				ch->altformed = false;
				SET_BIT(ch->imm_flags, IMM_ACID);
				success = true;
			}
			if(ch->altform.subtype == DRAGON_BRASS)
			{
    				ch->pcdata->learned[gsn_gas_breath] = 100;
				ch->altformed = false;
				SET_BIT(ch->imm_flags, IMM_POISON);
				success = true;
			}
			if(ch->altform.subtype == DRAGON_BRONZE)
			{
    				ch->pcdata->learned[gsn_lightning_breath] = 100;
				ch->altformed = false;
				SET_BIT(ch->imm_flags, IMM_LIGHTNING);
				success = true;
			}
			if(ch->altform.subtype == DRAGON_SILVER)
			{
    				ch->pcdata->learned[gsn_frost_breath] = 100;
				ch->altformed = false;
				SET_BIT(ch->imm_flags, IMM_COLD);
				success = true;
			}
			if(ch->altform.subtype == DRAGON_GOLD)
			{
    				ch->pcdata->learned[gsn_fire_breath] = 100;
				ch->altformed = false;
				SET_BIT(ch->imm_flags, IMM_FIRE);
				success = true;
			}
			if(ch->altform.subtype == DRAGON_CRYSTAL)
			{
    				ch->pcdata->learned[gsn_lightning_breath] = 100;
				ch->altformed = false;
				SET_BIT(ch->imm_flags, IMM_LIGHTNING);
				success = true;
			}
			if(ch->altform.subtype == DRAGON_EMERALD)
			{
    				ch->pcdata->learned[gsn_acid_breath] = 100;
				ch->altformed = false;
				SET_BIT(ch->imm_flags, IMM_ACID);
				success = true;
			}
			if(ch->altform.subtype == DRAGON_CLOUD)
			{
    				ch->pcdata->learned[gsn_frost_breath] = 100;
				ch->altformed = false;
				SET_BIT(ch->imm_flags, IMM_COLD);
				success = true;
			}
			if(ch->altform.subtype == DRAGON_STEEL)
			{
    				ch->pcdata->learned[gsn_steel_breath] = 100;
				ch->altformed = false;
				success = true;
			}
			if(ch->altform.subtype == DRAGON_SHADOW)
			{
    				ch->pcdata->learned[gsn_shadow_breath] = 100;
				ch->altformed = false;
				success = true;
			}
			break;
	}

	if(success == true)
	{
    		char_data *shifter;
    		shifter = create_mobile( get_mob_index(60008), 0 );
		// Copy alternate descriptions to mob
		replace_string(shifter->short_descr, ch->short_descr);
		replace_string(shifter->description, ch->description);
		// Copy alternate desc to alternate player desc
		replace_string(ch->altform.short_descr, shifter->short_descr);
		replace_string(ch->altform.description, shifter->description);
		// Copy original desc to mob
    		replace_string(shifter->short_descr, ch->orig_short_descr);
    		replace_string(shifter->description, ch->orig_description);
		// Copy original desc to player
    		replace_string(ch->short_descr, shifter->short_descr);
    		replace_string(ch->description, shifter->description);
    		extract_char(shifter,true);
		ch->altform.timer = 5;
    		reset_char(ch);
    		save_char_obj( ch );
    		update_pos( ch );
		return true;
	}
	return false;
}
/***************************************************************************/
/***************************************************************************/

