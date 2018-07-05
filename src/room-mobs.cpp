/**************************************************************************/
// room-mobs.cpp - Custom mob features for WOTL
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

#include "include.h" // dawn standard includes
#include "magic.h"
#include "macros.h"

int weapon_stat_lookup( int level, int pos, int die );

DECLARE_DO_FUN(do_look);
DECLARE_DO_FUN(kill_char);
DECLARE_DO_FUN(do_startprison);

/**************************************************************************/
void do_license(char_data *ch, char *argument)
{
    char_data *mob;
    char arg[MIL];
    int cost;
	
	char_data *victim=ch; // default 'victim' of heal
	
	// exception in here so players can buy heals while badly hurt
	// but can't while sleeping
	if (ch->position == POS_SLEEPING)
	{
		ch->println("You can't do that while sleeping.");
        return;
	}
	
    // check for healer
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
        if ( IS_NPC(mob) && IS_SET(mob->act2, ACT2_LICENSE_DEALER) )
            break;
    }
	
    if ( mob == NULL )
    {
        ch->println("You can't do that here.");
        return;
    }
	
	if ( ch->fighting ) {
		ch->println("You seem to be too busy to buy anything now.");
		return;
	}
	
    argument=one_argument(argument,arg);
	
    if (IS_NULLSTR(arg))
    {
        // display price list 
		act("$N says 'I offer these licenses for sale:'",ch,NULL,mob,TO_CHAR);
		ch->println("  bounty:   Bounty Hunter        2000 gold");
		ch->println("");
		ch->println("  Type LICENSE <type> to puchase a license.");
		return;
    }
	
    if (!str_prefix(arg,"bounty"))
    {
	cost  = 200000;
	if (cost > (ch->gold * 100 + ch->silver))
		{
		act("$N says 'You do not have enough gold for the license.'",
		ch,NULL,mob,TO_CHAR);
		return;
		}

	if(IS_SET(victim->affected_by2, AFF2_BOUNTYHUNTER)){
		ch->println("You already have a license.");
		return; 
	}

	if(!IS_ACTIVE(ch)){
		ch->println("You must be ACTIVE to buy a license.");
		return; 
	}

	if(victim->pcdata->theft != 0){
		ch->println("We don't sell licenses to thieving scoundrels!");
		return; 
	}

	if(victim->pcdata->murder != 0){
		ch->println("We don't sell licenses to murderers!");
		return; 
	}

	SET_BIT(victim->affected_by2, AFF2_BOUNTYHUNTER);
	WAIT_STATE(ch,PULSE_VIOLENCE);
	act("$N says 'You are now a licensed bounty hunter.'",
	     ch,NULL,mob,TO_CHAR);
     }
	
	else if (!str_prefix(arg,"weapons"))
	{
		act("$N says 'I'm sorry, I am out of those. Please try back later.'",
		ch,NULL,mob,TO_CHAR);
		return;

		cost  = 500000;
		if (cost > (ch->gold * 100 + ch->silver))
		{
			act("$N says 'You do not have enough gold for the license.'",
			ch,NULL,mob,TO_CHAR);
			return;
		}
	}
	
	else
	{
		act("$N says 'Type LICENSE for a list of licenses.'",
			ch,NULL,mob,TO_CHAR);
		return;
    }


	deduct_cost(ch,cost);
	mob->gold += cost;
	limit_mobile_wealth(mob);
	return;
}
/**************************************************************************/

void autonote(int type, char *sender, char *subject, char *to, char *text, bool reformat);

/**************************************************************************/
void do_pulldeck(char_data *ch, char *argument)
{
    char_data *mob, *mob1;
    char arg[MIL];
    int card, item;
	
	// exception in here so players can buy heals while badly hurt
	// but can't while sleeping
	if (ch->position == POS_SLEEPING)
	{
		ch->println("You can't do that while sleeping.");
        return;
	}
	
    // check for healer
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
        if ( IS_NPC(mob) && IS_SET(mob->act2, ACT2_DECK_OF_FATE) )
            break;
    }
	
    if ( mob == NULL )
    {
        ch->println("You can't do that here.");
        return;
    }
	

	if (IS_NPC(ch))
	{
		ch->println("Not while transformed.");
		return;
	}

	if ( ch->fighting ) {
		ch->println("You seem to be too busy do anything.");
		return;
	}

	if ((ch->level<19) || (!IS_SET(ch->act,PLR_CAN_ADVANCE)))
	{
		ch->println("Sorry but you are not experienced enough or letgained.");
		return; 
	}

	if( IS_AFFECTED2(ch, AFF2_BAN_DECK))
	{
		ch->println("Sorry but you are not allowed to use the deck anymore.");
		return; 
	}


    argument=one_argument(argument,arg);
	
    if (arg[0] == '\0')
    {
        // display information
		act("$N says 'The deck is a mysterious thing.'",ch,NULL,mob,TO_CHAR);
		act("$N says 'Some things are good, some are bad.'",ch,NULL,mob,TO_CHAR);
		act("$N says 'All effects are permanent, good or bad.'",ch,NULL,mob,TO_CHAR);
		act("$N says 'The immortals will not reimburse for a bad card.'",ch,NULL,mob,TO_CHAR);
		act("$N says 'So do not complain to them if you get a bad card.'",ch,NULL,mob,TO_CHAR);
		act("$N says 'Type PULL CONFIRM to draw a card from the deck of fate.'",ch,NULL,mob,TO_CHAR);
		return;
    }
	
    if (!str_prefix(arg,"confirm"))
    {
	act("You draw a card from the deck of fate.", ch,NULL,mob,TO_CHAR);
	act( "`#$n pulls a card from the `YDeck of Fate`^.", ch, NULL, mob, TO_ROOM );
	WAIT_STATE(ch,PULSE_VIOLENCE);
	card = number_range(1,22);

	if (card == 1)
	{
		act("`#You draw the `YJester`^.", ch,NULL,mob,TO_CHAR);
		act( "$n draws the `YJester`^.", ch, NULL, NULL, TO_ROOM );
		act("`#You gain `Y1500 experience`^.", ch,NULL,mob,TO_CHAR);
		gain_exp(ch, 1500);
		return;
	}
	else
	if (card == 2)
	{
		act("`#You draw the `YFool`^.", ch,NULL,mob,TO_CHAR);
		act( "$n draws the `YFool`^.", ch, NULL, NULL, TO_ROOM );
		act("`#You lose `Y1500 experience`^.", ch,NULL,mob,TO_CHAR);
		if(IS_IMMORTAL(ch)){
			ch->println("Your immortality saves you from harm.");
			return;
		}
		gain_exp(ch, -1500);
		return;
	}
	else
	if (card == 3)
	{
		act("`#You draw the `YStar`^.", ch,NULL,mob,TO_CHAR);
		act( "$n draws the `YStar`^.", ch, NULL, NULL, TO_ROOM );
		act("`#You feel your `YCONSTITUTION`^ increase.", ch,NULL,mob,TO_CHAR);
		ch->perm_stats[STAT_CO] += 3;
		ch->potential_stats[STAT_CO] += 3;
		if (ch->perm_stats[STAT_CO] > 101) 
			ch->perm_stats[STAT_CO] = 101;
		if (ch->potential_stats[STAT_CO] > 101) 
			ch->potential_stats[STAT_CO] = 101;
		return;
	}
	else
	if (card == 4)
	{
		act("`#You draw the `YIdiot`^.", ch,NULL,mob,TO_CHAR);
		act( "$n draws the `YIdiot`^.", ch, NULL, NULL, TO_ROOM );
		act("`#You suddenly feel stupid as you lose 5 points `YINTUITION`^.", ch,NULL,mob,TO_CHAR);
		if(IS_IMMORTAL(ch)){
			ch->println("Your immortality saves you from harm.");
			return;
		}
		ch->perm_stats[STAT_IN] -= 5;
		ch->potential_stats[STAT_IN] -= 5;
		return;
	}
	else
	if (card == 5)
	{
		act("`#You draw the `YGem`^.", ch,NULL,mob,TO_CHAR);
		act( "$n draws the `YGem`^.", ch, NULL, NULL, TO_ROOM );
		act("`#Your pockets feel heavier as you gain `Y5,000 gold`^!", ch,NULL,mob,TO_CHAR);
		ch->gold += 5000;
		return;
	}
	else
	if (card == 6)
	{
		act("`#You draw the `YRuin`^.", ch,NULL,mob,TO_CHAR);
		act( "$n draws the `YRuin`^.", ch, NULL, NULL, TO_ROOM );
		act("`#You lose all of your wealth and riches. You are `YPOOR!`^", ch,NULL,mob,TO_CHAR);
		if(IS_IMMORTAL(ch)){
			ch->println("Your immortality saves you from harm.");
			return;
		}
		ch->gold = 0;
		ch->silver = 0;
		ch->bank = 0;
		return;
	}
	else
	if (card == 7)
	{
		act("`#You draw the `YMoon`^.", ch,NULL,mob,TO_CHAR);
		act( "$n draws the `YMoon`^.", ch, NULL, NULL, TO_ROOM );
		act("`#You are granted a free `YPARDON`^ from prison card.", ch,NULL,mob,TO_CHAR);
		obj_data *o;
		o=create_object(get_obj_index(10397)); // Pardon card
	        obj_to_char( o, ch );
		return;
	}
	else
	if (card == 8)
	{
		act("`#You draw the `YTalon`^.", ch,NULL,mob,TO_CHAR);
		act( "$n draws the `YTalon`^.", ch, NULL, NULL, TO_ROOM );
		act("`#Your `Ymagical items`^ are torn from you.", ch,NULL,mob,TO_CHAR);
		if(IS_IMMORTAL(ch)){
			ch->println("Your immortality saves you from harm.");
			return;
		}
		obj_data *obj;
		obj_data *obj_next;
	  	for (obj = ch->carrying; obj != NULL; obj = obj_next)
	    	{
		    obj_next = obj->next_content;
		    if(IS_SET(obj->extra_flags, OBJEXTRA_MAGIC))
		    {
		            unequip_char( ch, obj );
		            obj_from_char( obj );
			    extract_obj(obj);
			    act( "$p is torn from $n!", ch, obj, NULL, TO_ROOM );
			    act( "$p is torn from your grasp!", ch, obj, NULL, TO_CHAR );
		    }
		}
		save_char_obj(ch);;
		return;
	}
	else
	if (card == 9)
	{
		act("`#You draw the `YSerpent`^.", ch,NULL,mob,TO_CHAR);
		act( "$n draws the `YSerpent`^.", ch, NULL, NULL, TO_ROOM );
		act("`#Your `YPRESENCE`^ is now 101.", ch,NULL,mob,TO_CHAR);
		ch->perm_stats[STAT_PR] = 101;
		ch->potential_stats[STAT_PR] = 101;
		return;
	}
	else
	if (card == 10)
	{
		act("`#You draw the `YWeakling`^.", ch,NULL,mob,TO_CHAR);
		act( "$n draws the `YWeakling`^.", ch, NULL, NULL, TO_ROOM );
		act("`#You suddenly feel weak as you lose 5 points `YSTRENGTH`^.", ch,NULL,mob,TO_CHAR);
		if(IS_IMMORTAL(ch)){
			ch->println("Your immortality saves you from harm.");
			return;
		}
		ch->perm_stats[STAT_ST] -= 5;
		ch->potential_stats[STAT_ST] -= 5;
    		return;
	}
	else
	if (card == 11)
	{
		act("`#You draw the `YKey`^.", ch,NULL,mob,TO_CHAR);
		act( "$n draws the `YKey`^.", ch, NULL, NULL, TO_ROOM );
		act("`#You gain a `Ymagic item`^!", ch,NULL,mob,TO_CHAR);
		item = number_range(1,5);
		obj_data *o;

		if(item == 1)
		{
			o=create_object(get_obj_index(2488)); // Celestial Dagger
		        obj_to_char( o, ch );
		}
		else if(item == 2)
		{
			o=create_object(get_obj_index(2516)); // Whip of Anihilation
		        obj_to_char( o, ch );
		}
		else if(item == 3)
		{
			o=create_object(get_obj_index(2483)); // Axe of the Rager
		        obj_to_char( o, ch );
		}
		else if(item == 4)
		{
			o=create_object(get_obj_index(29603)); // Mace of Death
		        obj_to_char( o, ch );
		}
		else if(item == 5)
		{
			o=create_object(get_obj_index(29661)); // Berserver Sword
		        obj_to_char( o, ch );
		}
		act("You suddenly notice something in your inventory.", ch,NULL,mob,TO_CHAR);
		return;
	}
	else
	if (card == 12)
	{
		act("`#You draw the `YFlames`^.", ch,NULL,mob,TO_CHAR);
		act( "$n draws the `YFlames`^.", ch, NULL, NULL, TO_ROOM );
		act("`#You are sent straight to `RHELL`^.", ch,NULL,mob,TO_CHAR);
    		act( "$n has been sent to `#`RHELL!`^", ch, NULL, mob, TO_CHAR );
		char_from_room(ch);
		char_to_room(ch, get_room_index(10470));
		do_look(ch, "auto");
		return;
	}
	else
	if (card == 13)
	{
		act("`#You draw the `YFates`^.", ch,NULL,mob,TO_CHAR);
		act( "$n draws the `YFates`^.", ch, NULL, NULL, TO_ROOM );
		act("`#You will avoid your next `YDEATH`^.", ch,NULL,mob,TO_CHAR);
		obj_data *o;
		o=create_object(get_obj_index(10395)); // Avoid Death Token
	        obj_to_char( o, ch );
		return;
	}
	else
	if (card == 14)
	{
		OBJ_DATA  *statue;
		char buf[MSL];
		mob1 = mob;
		mob1->name = "Deck of Fate";
		mob1->short_descr = "Deck of Fate";
		act("`#You draw `YMedusa`^.", ch,NULL,mob,TO_CHAR);
		act( "$n draws `YMedusa`^.", ch, NULL, NULL, TO_ROOM );
		act("`#You have been turned to `YSTONE!!`^", ch,NULL,mob,TO_CHAR);
                statue = create_object(get_obj_index(2028));
                statue->level = 0;
                sprintf( buf, statue->short_descr, ch->short_descr );
                free_string( statue->short_descr );
		statue->short_descr = str_dup( buf );
    		sprintf( buf, statue->description, ch->short_descr );
    		replace_string( statue->description, buf);
    		obj_to_room( statue, get_room_index(3099) );
		if(IS_IMMORTAL(ch)){
			ch->println("Your immortality saves you from death.");
			return;
		}
    		ch->position = POS_DEAD;
    		kill_char(ch, mob1);
		return;
	}
	else
	if (card == 15)
	{
		act("`#You draw the `YTrainer`^.", ch,NULL,mob,TO_CHAR);
		act( "$n draws the `YTrainer`^.", ch, NULL, NULL, TO_ROOM );
		act("`#You gain `Y10 practices & 5 trains`^!", ch,NULL,mob,TO_CHAR);
		ch->practice += 10;
		ch->train += 5;
		return;
	}
	else
	if (card == 16)
	{
		act("`#You draw the `YHangman`^.", ch,NULL,mob,TO_CHAR);
		act( "$n draws the `YHangman`^.", ch, NULL, NULL, TO_ROOM );
		act("`#You have been convicted of `RMURDER`^ and imprisoned!", ch,NULL,mob,TO_CHAR);
		ch->pcdata->murder = 1;
		do_startprison(ch, "");
		return;
	}
	else
	if (card == 17)
	{
		act("`#You draw the `YKing`^.", ch,NULL,mob,TO_CHAR);
		act( "$n draws the `YKing`^.", ch, NULL, NULL, TO_ROOM );
		act("`#You gain `R500 Experience`^!", ch,NULL,mob,TO_CHAR);
		gain_exp(ch, 500);
		return;
	}
	else
	if (card == 18)
	{
		act("`#You draw the `YMists`^.", ch,NULL,mob,TO_CHAR);
		act( "$n draws the `YMists`^.", ch, NULL, NULL, TO_ROOM );
		ROOM_INDEX_DATA *pRoomIndex;
		pRoomIndex = get_random_room(ch);
		ch->printf( "Mists swirl around you and you find yourself somewhere else!\r\n" );
		act( "A large cloud of mists swirl around $n and they vanish!", ch, NULL, NULL, TO_ROOM );
		char_from_room( ch );
		char_to_room(ch, pRoomIndex );
		act( "$n slowly appears from a cloud of mists.", ch, NULL, NULL, TO_ROOM );
		do_look( ch, "auto" );
		return;
	}
	else
	if (card == 19)
	{
		act("`#You draw the `YKnight`^.", ch,NULL,mob,TO_CHAR);
		act( "$n draws the `YKnight`^.", ch, NULL, NULL, TO_ROOM );
		act("`#You gain a `YKNIGHT `^ as a follower.", ch,NULL,mob,TO_CHAR);
		if(IS_IMMORTAL(ch)){
			ch->println("Your immortality prevents the Knight from appearing.");
			return;
		}
    		char_data *pet;
		char buf[MSL];
    		pet = create_mobile( get_mob_index(10397), 0 );
		pet->comm = COMM_NOTELL|COMM_NOSHOUT|COMM_NOCHANNELS;

		sprintf( buf, "%sA neck tag says 'I belong to %s'.\r\n",
			pet->description, ch->short_descr );
		free_string( pet->description );
		pet->description = str_dup( buf );
    		char_to_room( pet, ch->in_room );
		pet->level = ch->level;
		pet->master = ch;
		pet->leader = ch;
		pet->max_hit = ch->max_hit + 50;
		SET_BIT(pet->act, ACT_PET);
		SET_BIT(pet->affected_by, AFF_CHARM);
	        SET_BIT(pet->dyn, DYN_IS_BEING_ORDERED);
		add_follower( pet, ch );
		ch->pet = pet;
		act( "$n now follows you.", pet, NULL, ch, TO_VICT );
		return;
	}
	else
	if (card == 20)
	{
		act("`#You draw `YDeath`^.", ch,NULL,mob,TO_CHAR);
		act( "$n draws `YDeath`^.", ch, NULL, NULL, TO_ROOM );
		if(IS_IMMORTAL(ch)){
			ch->println("Your immortality saves you from harm.");
			return;
		}
		act("`#`YYou LOSE A KARN!!!`^.", ch,NULL,mob,TO_CHAR);
		ch->pcdata->karns -= 1;
		char_from_room(ch);
		char_to_room(ch, get_room_index(10398));
		do_look(ch, "auto");
		return;
	}
	else
	if (card == 21)
	{
		act("`#You draw the `YSun`^.", ch,NULL,mob,TO_CHAR);
		act( "$n draws the `YSun`^.", ch, NULL, NULL, TO_ROOM );
		act("`#You gain `Y500 experience and an item`^.", ch,NULL,mob,TO_CHAR);
		gain_exp(ch, 500);
		item = number_range(1,5);
		obj_data *o;

			if(item == 1)
			{
				o=create_object(get_obj_index(2488)); // Celestial Dagger
			        obj_to_char( o, ch );
			}
			else if(item == 2)
			{
				o=create_object(get_obj_index(2516)); // Whip of Anihilation
			        obj_to_char( o, ch );
			}
			else if(item == 3)
			{
				o=create_object(get_obj_index(2483)); // Axe of the Rager
			        obj_to_char( o, ch );
			}
			else if(item == 4)
			{
				o=create_object(get_obj_index(29603)); // Mace of Death
			        obj_to_char( o, ch );
			}
			else if(item == 5)
			{
				o=create_object(get_obj_index(29661)); // Berserver Sword
			        obj_to_char( o, ch );
			}

		act("You suddenly notice something in your inventory.", ch,NULL,mob,TO_CHAR);
		return;
	}
	else
	if (card == 22)
	{
		act("`#You draw `YWrath`^.", ch,NULL,mob,TO_CHAR);
		act( "$n draws `YWrath`^.", ch, NULL, NULL, TO_ROOM );
		act( "`#`R$N has vanished in a cloud of blackness!`^",  mob, NULL, ch, TO_NOTVICT );
		act( "`#`RYou have vanished in a cloud of blackness!`^", mob, NULL, ch, TO_VICT );

		if(IS_IMMORTAL(ch)){
			ch->println("Your immortality saves you from harm.");
			return;
		}

			AFFECT_DATA af;
			ch->println("You feel poison coursing through your veins.");
			af.where     = WHERE_AFFECTS;
			af.type      = gsn_poison;
			af.level     = 125;
			af.duration  = 90;
			af.location  = APPLY_ST;
			af.modifier  = -20;
			af.bitvector = AFF_POISON;
			affect_join( ch, &af );

			ch->println("A wave of hopelessness & despair passes over you.");

			af.where     = WHERE_AFFECTS2;
			af.type      = gsn_despair;
			af.level     = 125;
			af.duration  = 80;
			af.location  = APPLY_IN;
			af.modifier = -30;
			af.bitvector = AFF2_DESPAIR;
			affect_to_char (ch, &af);

			af.where = WHERE_AFFECTS2;
			af.type = gsn_despair;
			af.level = 125;
			af.duration = 90;
			af.location = APPLY_HITROLL;
			affect_to_char (ch, &af);
			af.modifier = -20;
			af.bitvector = AFF2_DESPAIR;
			af.location = APPLY_DAMROLL;
			affect_to_char (ch, &af);
			act( "$n falls to the ground in utter despair and hopelessness.",  ch, NULL, NULL, TO_NOTVICT );

			ch->println("You feel cursed by the Deck of Fate.");
			af.where     = WHERE_AFFECTS;
			af.type      = gsn_curse;
			af.level     = 125;
			af.duration  = 90;
			af.location  = APPLY_QU;
			af.modifier  = -20;
			af.bitvector = AFF_CURSE;
			affect_join( ch, &af );

		char_from_room(ch);
		char_to_room(ch, get_room_index(2516));
		do_look(ch, "auto");
		return;
	}

	return;

     }
	

	else
	{
		act("$N says 'Type PULL on how to draw from the deck of fate.'",
			ch,NULL,mob,TO_CHAR);
		return;
    }

	return;
}

/**************************************************************************/
void do_repair(char_data *ch, char *argument)
{
    char_data *mob;
    char buf[MSL];
    char arg[MIL];
    obj_data *obj;
    int cost=0;
	
    if (ch->position == POS_SLEEPING)
    {
	ch->println("You can't do that while sleeping.");
        return;
    }
	
    // check for repairman
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
        if ( IS_NPC(mob) && IS_SET(mob->act2, ACT2_REPAIRMAN) )
            break;
    }
	
    if ( mob == NULL )
    {
        ch->println("You can't do that here.");
        return;
    }
	
	if ( ch->fighting ) 
	{
		ch->println("You seem to be too busy to buy anything now.");
		return;
	}
	
    argument=one_argument(argument,arg);
	
    if (IS_NULLSTR(arg))
    {
        // display price list 
		act("$N says 'I am able to repair your item for a small fee:'",ch,NULL,mob,TO_CHAR);
		ch->println("");
		ch->println("  Type REPAIR   <item> to repair it.");
		ch->println("  Type EVALUATE <item> to estimate the cost.");
		return;
    }

    if (!str_prefix(arg,"all"))
    {
		obj_data *obj_next;
		int totalcost=0;
		for (obj = ch->carrying; obj != NULL; obj = obj_next)
		{
		 	if(obj->condition > 39 && obj->condition < 100)
	   		{
				if(obj->cost == 0)
		   			totalcost += 250;
				else
		   			totalcost += obj->cost;
			}
		    	obj_next = obj->next_content;
		}	
		if (totalcost == 0)
	   	{
			act("$N says 'Your equipment is in perfect shape.'",
				ch,NULL,mob,TO_CHAR);
			return;
   		}
	   cost = totalcost*2;
	   sprintf( buf, "$n tells you 'It will cost %d silver and %d gold coins to repair everything'.",
		cost - (cost/100) * 100, cost/100 );
	   act( buf, mob, NULL, ch, TO_VICT );
	   if (cost > (ch->gold * 100 + ch->silver))
	   {
			act("$N says 'You don't have enough.'",
				ch,obj,mob,TO_CHAR);
			return;
	   }
		for (obj = ch->carrying; obj != NULL; obj = obj_next)
		{
		   obj_next = obj->next_content;
		   if(obj->condition > 39 && obj->condition < 100)
		  	{
				obj->condition = 100;

				if( (obj->item_type == ITEM_WEAPON || obj->item_type == ITEM_ARMOR) &&
	     				obj->pIndexData->vnum != OBJ_VNUM_RANDOM_OBJ)
				{
	
					if(obj->item_type==ITEM_WEAPON)
					{
						obj->value[1]= obj->pIndexData->value[1];
						obj->value[2]= obj->pIndexData->value[2];
					}
					else
					{
						obj->value[0]= obj->pIndexData->value[0];
						obj->value[1]= obj->pIndexData->value[1];
						obj->value[2]= obj->pIndexData->value[2];
						obj->value[3]= obj->pIndexData->value[3];
					}
				}
				if( obj->item_type == ITEM_ARMOR && obj->pIndexData->vnum == OBJ_VNUM_RANDOM_OBJ)
				{
					int level = obj->level;

					if(CAN_WEAR(obj, OBJWEAR_FINGER ))
					{
		       		obj->value[0] = level/10;
	               obj->value[1] = level/10;
	               obj->value[2] = level/10;
	               obj->value[3] = level/10;		
					}
					else
					if(CAN_WEAR(obj, OBJWEAR_NECK ))
					{
		       		obj->value[0] = level/6;
	               obj->value[1] = level/6;
	               obj->value[2] = level/6;
	               obj->value[3] = level/8;
					}
					else
					if(CAN_WEAR(obj, OBJWEAR_TORSO ))
					{
		       		obj->value[0] = level/3;
	               obj->value[1] = level/3;
	               obj->value[2] = level/3;
	               obj->value[3] = level/4;
					}
					else
					if(CAN_WEAR(obj, OBJWEAR_HEAD ))
					{
		       		obj->value[0] = level/4;
	               obj->value[1] = level/4;
	               obj->value[2] = level/4;
	               obj->value[3] = level/5;
					}
					else
					if(CAN_WEAR(obj, OBJWEAR_LEGS ))
					{
		       		obj->value[0] = level/5;
	               obj->value[1] = level/5;
	               obj->value[2] = level/5;
	               obj->value[3] = level/6;
					}
					else
					if(CAN_WEAR(obj, OBJWEAR_FEET ))
					{
		       		obj->value[0] = level/5;
	               obj->value[1] = level/5;
	               obj->value[2] = level/5;
	               obj->value[3] = level/6;
					}
					else
					if(CAN_WEAR(obj, OBJWEAR_ARMS ))
					{
		       		obj->value[0] = level/4;
	               obj->value[1] = level/4;
	               obj->value[2] = level/4;
	               obj->value[3] = level/5;
					}
					else
					if(CAN_WEAR(obj, OBJWEAR_SHIELD ))
					{
		    			obj->value[0] = level/3;
	               obj->value[1] = level/3;
	               obj->value[2] = level/3;
	               obj->value[3] = level/4;
					}
					else
					if(CAN_WEAR(obj, OBJWEAR_ABOUT ))
					{
		       		obj->value[0] = level/4;
	               obj->value[1] = level/4;
	               obj->value[2] = level/4;
	               obj->value[3] = level/5;
					}
					else
					if(CAN_WEAR(obj, OBJWEAR_WAIST ))
					{
		       		obj->value[0] = level/5;
	               obj->value[1] = level/5;
	               obj->value[2] = level/5;
	               obj->value[3] = level/7;
					}
					else
					if(CAN_WEAR(obj, OBJWEAR_WRIST ))
					{
		       		obj->value[0] = level/7;
	               obj->value[1] = level/7;
	               obj->value[2] = level/7;
	               obj->value[3] = level/9;
					}
					else
					if(CAN_WEAR(obj, OBJWEAR_HOLD ))
					{
		       		obj->value[0] = 0;
	               obj->value[1] = 0;
	               obj->value[2] = 0;
	               obj->value[3] = 0;
					}
					else
					if(CAN_WEAR(obj, OBJWEAR_FLOAT ))
					{
		       		obj->value[0] = 0;
	               obj->value[1] = 0;
	               obj->value[2] = 0;
	               obj->value[3] = 0;
					}
					else
					if(CAN_WEAR(obj, OBJWEAR_FACE ))
					{
		       		obj->value[0] = level/10;
	               obj->value[1] = level/10;
	               obj->value[2] = level/10;
	               obj->value[3] = level/10;
					}
					else
					if(CAN_WEAR(obj, OBJWEAR_EYES ))
					{
		       		obj->value[0] = level/10;
	               obj->value[1] = level/10;
	               obj->value[2] = level/10;
	               obj->value[3] = level/10;
					}
					else
					if(CAN_WEAR(obj, OBJWEAR_EAR ))
					{
		       		obj->value[0] = level/10;
	               obj->value[1] = level/10;
	               obj->value[2] = level/10;
	               obj->value[3] = level/10;
					}
					else
					if(CAN_WEAR(obj, OBJWEAR_ANKLE ))
					{
		       		obj->value[0] = level/8;
	               obj->value[1] = level/8;
	               obj->value[2] = level/8;
	               obj->value[3] = level/10;
					}
		
				}
			if( obj->item_type == ITEM_WEAPON && obj->pIndexData->vnum == OBJ_VNUM_RANDOM_OBJ)
			{
				int level = obj->level;
	   		if (obj->value[0] == 4)
			{
				obj->value[1] = weapon_stat_lookup( level, 0, 0 );
				obj->value[2] = weapon_stat_lookup( level, 0, 1 );
				if (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
				{
					obj->value[1] = weapon_stat_lookup( level+6, 0, 0 );
					obj->value[2] = weapon_stat_lookup( level+6, 0, 1 );
				}
			}
		if (obj->value[0] == 2)
			{
			   	obj->value[1] = weapon_stat_lookup( level-8, 0, 0 );
			   	obj->value[2] = weapon_stat_lookup( level-8, 0, 1 );
			}
		if( obj->value[0] == 1)
			{
				obj->value[1] = weapon_stat_lookup( level, 0, 0 );
				obj->value[2] = weapon_stat_lookup( level, 0, 1 );
				if (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
				{
					obj->value[1] = weapon_stat_lookup( level+6, 0, 0 );
					obj->value[2] = weapon_stat_lookup( level+6, 0, 1 );
				}
			}
		if(obj->value[0] == 5)
			{
				obj->value[1] = weapon_stat_lookup( level+3, 0, 0 );
				obj->value[2] = weapon_stat_lookup( level+3, 0, 1 );
				if (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
				{
					obj->value[1] = weapon_stat_lookup( level+9, 0, 0 );
					obj->value[2] = weapon_stat_lookup( level+9, 0, 1 );
				}
			}
		if(obj->value[0] == 7)
			{
				obj->value[1] = weapon_stat_lookup( level-3, 0, 0 );
				obj->value[2] = weapon_stat_lookup( level-3, 0, 1 );
			}
		if(obj->value[0] == 6)
			{
			   	obj->value[1] = weapon_stat_lookup( level-2, 0, 0 );
			   	obj->value[2] = weapon_stat_lookup( level-2, 0, 1 );
				if (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
				{
					obj->value[1] = weapon_stat_lookup( level+3, 0, 0 );
					obj->value[2] = weapon_stat_lookup( level+3, 0, 1 );
				}
			}
	}

	    }
	    
	}
	act("$N says 'Please wait while I repair everything.'", ch,NULL,mob,TO_CHAR);
	ch->set_pdelay(2);
	act("$N puts everything on the workbench.'", ch,NULL,mob,TO_ALL);
	ch->set_pdelay(3);
	act("$N works a bit on it.'", ch,NULL,mob,TO_ALL);
	ch->set_pdelay(5);
	act("$N hands over your equipment to you.'", ch,NULL,mob,TO_CHAR);
	act("$N hands $n's equipment back to them.'", ch,NULL,mob,TO_ROOM);
	deduct_cost(ch,cost);
	mob->gold += cost;
	limit_mobile_wealth(mob);
	save_char_obj(ch);
	ch->set_pdelay(6);
	act("$N says 'I have repaired everything. Nice doing business with you.'", ch,NULL,mob,TO_CHAR);
	return;
    }

    if ( (obj = get_obj_wear( ch, arg)) == NULL )
    {
	act("$N says 'You are not wearing or holding that.'",
		ch,NULL,mob,TO_CHAR);
	return;
    }

    if (obj->condition == 100)
    {
	act("$N says 'Why would I repair that ? It is in perfect shape.'",
		ch,NULL,mob,TO_CHAR);
	return;
    }

    if (obj->condition < 40)
    {
	act("$N says 'It is damaged beyond repair. I cannot repair $p.'",
		ch,obj,mob,TO_CHAR);
	return;
    }
	
    if(obj->cost == 0)
       cost += 500;
    else
       cost = obj->cost*2;

    if (cost > (ch->gold * 100 + ch->silver))
    {
    	sprintf( buf, "$n tells you 'It will cost %d silver and %d gold coins to repair $p'.",
		cost - (cost/100) * 100, cost/100 );
    	act( buf, mob, obj, ch, TO_VICT );
	act("$N says 'You don't have enough.'",
		ch,obj,mob,TO_CHAR);
	return;
    }

	obj->condition = 100;

	if( (obj->item_type == ITEM_WEAPON || obj->item_type == ITEM_ARMOR) &&
	     obj->pIndexData->vnum != 100)
	{
	
		if(obj->item_type==ITEM_WEAPON)
		{
			obj->value[1]= obj->pIndexData->value[1];
			obj->value[2]= obj->pIndexData->value[2];
		}
		else
		{
			obj->value[0]= obj->pIndexData->value[0];
			obj->value[1]= obj->pIndexData->value[1];
			obj->value[2]= obj->pIndexData->value[2];
			obj->value[3]= obj->pIndexData->value[3];
		}
	}
	if( obj->item_type == ITEM_ARMOR && obj->pIndexData->vnum == OBJ_VNUM_RANDOM_OBJ)
	{
		int level = obj->level;

		if(CAN_WEAR(obj, OBJWEAR_FINGER ))
		{
		       obj->value[0] = level/10;
	               obj->value[1] = level/10;
	               obj->value[2] = level/10;
	               obj->value[3] = level/10;		
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_NECK ))
		{
		       obj->value[0] = level/6;
	               obj->value[1] = level/6;
	               obj->value[2] = level/6;
	               obj->value[3] = level/8;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_TORSO ))
		{
		       obj->value[0] = level/3;
	               obj->value[1] = level/3;
	               obj->value[2] = level/3;
	               obj->value[3] = level/4;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_HEAD ))
		{
		       obj->value[0] = level/4;
	               obj->value[1] = level/4;
	               obj->value[2] = level/4;
	               obj->value[3] = level/5;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_LEGS ))
		{
		       obj->value[0] = level/5;
	               obj->value[1] = level/5;
	               obj->value[2] = level/5;
	               obj->value[3] = level/6;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_FEET ))
		{
		       obj->value[0] = level/5;
	               obj->value[1] = level/5;
	               obj->value[2] = level/5;
	               obj->value[3] = level/6;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_ARMS ))
		{
		       obj->value[0] = level/4;
	               obj->value[1] = level/4;
	               obj->value[2] = level/4;
	               obj->value[3] = level/5;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_SHIELD ))
		{
		       obj->value[0] = level/3;
	               obj->value[1] = level/3;
	               obj->value[2] = level/3;
	               obj->value[3] = level/4;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_ABOUT ))
		{
		       obj->value[0] = level/4;
	               obj->value[1] = level/4;
	               obj->value[2] = level/4;
	               obj->value[3] = level/5;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_WAIST ))
		{
		       obj->value[0] = level/5;
	               obj->value[1] = level/5;
	               obj->value[2] = level/5;
	               obj->value[3] = level/7;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_WRIST ))
		{
		       obj->value[0] = level/7;
	               obj->value[1] = level/7;
	               obj->value[2] = level/7;
	               obj->value[3] = level/9;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_HOLD ))
		{
		       obj->value[0] = 0;
	               obj->value[1] = 0;
	               obj->value[2] = 0;
	               obj->value[3] = 0;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_FLOAT ))
		{
		       obj->value[0] = 0;
	               obj->value[1] = 0;
	               obj->value[2] = 0;
	               obj->value[3] = 0;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_FACE ))
		{
		       obj->value[0] = level/10;
	               obj->value[1] = level/10;
	               obj->value[2] = level/10;
	               obj->value[3] = level/10;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_EYES ))
		{
		       obj->value[0] = level/10;
	               obj->value[1] = level/10;
	               obj->value[2] = level/10;
	               obj->value[3] = level/10;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_EAR ))
		{
		       obj->value[0] = level/10;
	               obj->value[1] = level/10;
	               obj->value[2] = level/10;
	               obj->value[3] = level/10;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_ANKLE ))
		{
		       obj->value[0] = level/8;
	               obj->value[1] = level/8;
	               obj->value[2] = level/8;
	               obj->value[3] = level/10;
		}
		
	}
	if( obj->item_type == ITEM_WEAPON && obj->pIndexData->vnum == OBJ_VNUM_RANDOM_OBJ)
	{
		int level = obj->level;

	   	if (obj->value[0] == 4)
			{
				obj->value[1] = weapon_stat_lookup( level, 0, 0 );
				obj->value[2] = weapon_stat_lookup( level, 0, 1 );
			}
		if (obj->value[0] == 2)
			{
			   	obj->value[1] = weapon_stat_lookup( level-7, 0, 0 );
			   	obj->value[2] = weapon_stat_lookup( level-7, 0, 1 );
			}
		if( obj->value[0] == 1)
			{
				obj->value[1] = weapon_stat_lookup( level, 0, 0 );
				obj->value[2] = weapon_stat_lookup( level, 0, 1 );
			}
		if(obj->value[0] == 5)
			{
				obj->value[1] = weapon_stat_lookup( level+3, 0, 0 );
				obj->value[2] = weapon_stat_lookup( level+3, 0, 1 );
			}
		if(obj->value[0] == 7)
			{
				obj->value[1] = weapon_stat_lookup( level-3, 0, 0 );
				obj->value[2] = weapon_stat_lookup( level-3, 0, 1 );
			}
		if(obj->value[0] == 6)
			{
			   	obj->value[1] = weapon_stat_lookup( level-2, 0, 0 );
			   	obj->value[2] = weapon_stat_lookup( level-2, 0, 1 );
			}
	}

	act("$N says 'Please wait while I repair $p.'", ch,obj,mob,TO_CHAR);
	ch->set_pdelay(1);
	act("$N puts $p on the workbench.'", ch,obj,mob,TO_ALL);
   ch->set_pdelay(2);
	act("$N works a bit on $p.'", ch,obj,mob,TO_ALL);
   ch->set_pdelay(3);
	act("$N looks at $p with a keen eye.'", ch,obj,mob,TO_ALL);
   ch->set_pdelay(4);
	act("$N hands over $p to you.'", ch,obj,mob,TO_CHAR);
   ch->set_pdelay(5);
	act("$N hands $p back to $n.'", ch,obj,mob,TO_ROOM);
	save_char_obj(ch);
	deduct_cost(ch,cost);
	mob->gold += cost;
	limit_mobile_wealth(mob);
   ch->set_pdelay(6);
	act("$N says 'I have repaired $p. Nice doing business with you.'", ch,obj,mob,TO_CHAR);
	return;
}
/**************************************************************************/
void do_evaluate(char_data *ch, char *argument)
{
    char_data *mob;
    obj_data *obj;
    char arg[MIL];
    char buf[MSL];
    int cost=0;
	
    if (ch->position == POS_SLEEPING)
    {
	ch->println("You can't do that while sleeping.");
        return;
    }
	
    // check for repairman
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
        if ( IS_NPC(mob) && IS_SET(mob->act2, ACT2_REPAIRMAN) )
            break;
    }
	
    if ( mob == NULL )
    {
        ch->println("You can't do that here.");
        return;
    }
	
	if ( ch->fighting ) {
		ch->println("You seem to be too busy to buy anything now.");
		return;
	}
	
    argument=one_argument(argument,arg);
	
    if (IS_NULLSTR(arg))
    {
        // display price list 
		act("$N says 'I am able to repair your item for a small fee:'",ch,NULL,mob,TO_CHAR);
		ch->println("");
		ch->println("  Type REPAIR   <item> to repair it.");
		ch->println("  Type EVALUATE <item> to estimate the cost.");
		return;
    }

    if (!str_prefix(arg,"all"))
    {
	int totalcost=0;
	obj_data *obj;
	obj_data *obj_next;
  	for (obj = ch->carrying; obj != NULL; obj = obj_next)
    	{
	    obj_next = obj->next_content;
  	    if(obj->condition>39 && obj->condition<100)
	    	if(obj->cost == 0)
		   totalcost += 500;
		else
		   totalcost += obj->cost;
	}

	if (totalcost == 0)
    	{
		act("$N says 'Your equipment is in perfect shape.'",
			ch,NULL,mob,TO_CHAR);
		return;
    	}

    	cost = totalcost*2;
    	sprintf( buf, "$n tells you 'It will cost %d silver and %d gold coins to repair everything'.",
		cost - (cost/100) * 100, cost/100 );
    	act( buf, mob, NULL, ch, TO_VICT );
	return;
    }


    if ( (obj = get_obj_wear( ch, arg)) == NULL )
    {
	act("$N says 'You are not wearing or holding that.'",
		ch,NULL,mob,TO_CHAR);
	return;
    }

    if (obj->condition == 100)
    {
	act("$N says 'Why would I repair that ? It is in perfect shape.'",
		ch,NULL,mob,TO_CHAR);
	return;
    }

    if (obj->condition < 40)
    {
	act("$N says 'It is damaged beyond repair. I cannot repair $p.'",
		ch,obj,mob,TO_CHAR);
	return;
    }

    if(obj->cost == 0)
       cost += 500;
    else
       cost = obj->cost*2;
    sprintf( buf, "$n tells you 'It will cost %d silver and %d gold coins to repair $p'.",
		cost - (cost/100) * 100, cost/100 );
    act( buf, mob, obj, ch, TO_VICT );

    return;
}

/**************************************************************************/
void do_forge(char_data *ch, char *argument)
{
    char_data *mob;
    obj_data *obj;
    char arg[MIL];
    char target[MIL];
    int cost=0;
	
    if (ch->position == POS_SLEEPING)
    {
	ch->println("You can't do that while sleeping.");
        return;
    }
	
    // check for repairman
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
        if ( IS_NPC(mob) && IS_SET(mob->act2, ACT2_FORGER) )
            break;
    }
	
    if ( mob == NULL )
    {
        ch->println("You can't do that here.");
        return;
    }
	
	if ( ch->fighting ) {
		ch->println("You seem to be too busy to buy anything now.");
		return;
	}
	
    argument=one_argument(argument,arg);
	one_argument(argument,target);
	
    if (IS_NULLSTR(arg))
    {
        // display price list 
		act("$N says 'I am able to forge the following on your items:'",ch,NULL,mob,TO_CHAR);
		ch->println("");
		ch->println("Type PURCHASE <affect> <item worn>");
		ch->println("  burnproof           1000 gold");
		ch->println("=========== Weapons ===========");
		ch->println("  sharp               1000 gold");
		ch->println("  vorpal              2000 gold");
		ch->println("  flaming             7500 gold");
		ch->println("  frost               7500 gold");
		ch->println("  lightning           7500 gold");
		ch->println("  poison             10000 gold");
		ch->println("  vampiric  (hp)     10000 gold");
		ch->println("  annealed (stun)    10000 gold");
		ch->println("  suckle   (mana)    10000 gold");
		ch->println("  enervate (moves)   10000 gold");
		return;
    }
  
    if ( (obj = get_obj_wear( ch, target)) == NULL )
    {
	act("$N says 'You are not wearing or holding that.'",
		ch,NULL,mob,TO_CHAR);
	return;
    }

    if (obj->condition < 100)
    {
	act("$N says 'Get it repaired first before I can work on $p.'",
		ch,obj,mob,TO_CHAR);
	return;
    }

    if (!str_prefix(arg,"burnproof"))
    {
	cost = 100000;
	if (IS_SET(obj->extra_flags, OBJEXTRA_BURN_PROOF))
	{
		act("$N says '$p is already burnproof.'",
			ch,obj,mob,TO_CHAR);
		return;		
	}

    	if (cost > (ch->gold * 100 + ch->silver))
    	{
		act("$N says 'You don't have enough gold.'",
			ch,obj,mob,TO_CHAR);
		return;	
	}

	SET_BIT(obj->extra_flags, OBJEXTRA_BURN_PROOF);

	deduct_cost(ch,cost);
	mob->gold += cost;
	limit_mobile_wealth(mob);
	save_char_obj(ch);
	act("$N says '$p is now burnproof. Nice doing business with you.'", ch,obj,mob,TO_CHAR);
	return;
    }

    if( obj->item_type != ITEM_WEAPON)
    {
	act("$N says '$p is not a weapon.'", ch,obj,mob,TO_CHAR);
	return;
    }

    if (!str_prefix(arg,"sharp"))
    {
	cost = 100000;
	if (IS_SET(obj->value[4], WEAPON_SHARP))
	{
		act("$N says '$p is already sharp.'",
			ch,obj,mob,TO_CHAR);
		return;		
	}

	if (IS_SET(obj->value[4], WEAPON_VORPAL))
	{
		act("$N says 'I cannot make $p any sharper.'",
			ch,obj,mob,TO_CHAR);
		return;		
	}

    	if (cost > (ch->gold * 100 + ch->silver))
    	{
		act("$N says 'You don't have enough gold.'",
			ch,obj,mob,TO_CHAR);
		return;	
	}

	SET_BIT(obj->value[4], WEAPON_SHARP);

	deduct_cost(ch,cost);
	mob->gold += cost;
	limit_mobile_wealth(mob);
	save_char_obj(ch);
	act("$N says '$p is now sharp. Nice doing business with you.'", ch,obj,mob,TO_CHAR);
	return;
    }

    if (!str_prefix(arg,"vorpal"))
    {
	cost = 200000;

	if (IS_SET(obj->value[4], WEAPON_VORPAL))
	{
		act("$N says 'I cannot make $p any sharper.'",
			ch,obj,mob,TO_CHAR);
		return;		
	}

    	if (cost > (ch->gold * 100 + ch->silver))
    	{
		act("$N says 'You don't have enough gold.'",
			ch,obj,mob,TO_CHAR);
		return;	
	}

	REMOVE_BIT(obj->value[4], WEAPON_SHARP);
	SET_BIT(obj->value[4], WEAPON_VORPAL);

	deduct_cost(ch,cost);
	mob->gold += cost;
	limit_mobile_wealth(mob);
	save_char_obj(ch);
	act("$N says '$p is now vorpal. Nice doing business with you.'", ch,obj,mob,TO_CHAR);
	return;
    }

    if (!str_prefix(arg,"flaming"))
    {
	cost = 750000;

	if (IS_SET(obj->value[4], WEAPON_FROST) ||
	    IS_SET(obj->value[4], WEAPON_VAMPIRIC) ||
	    IS_SET(obj->value[4], WEAPON_SHOCKING) ||
	    IS_SET(obj->value[4], WEAPON_POISON) ||
	    IS_SET(obj->value[4], WEAPON_SUCKLE) ||
	    IS_SET(obj->value[4], WEAPON_ENERVATE) ||
	    IS_SET(obj->value[4], WEAPON_ANNEALED) ||
	    IS_SET(obj->value[4], WEAPON_FLAMING))
	{
		act("$N says 'I cannot add anything else to $p.'",
			ch,obj,mob,TO_CHAR);
		return;		
	}

    	if (cost > (ch->gold * 100 + ch->silver))
    	{
		act("$N says 'You don't have enough gold.'",
			ch,obj,mob,TO_CHAR);
		return;	
	}

	SET_BIT(obj->value[4], WEAPON_FLAMING);

	deduct_cost(ch,cost);
	mob->gold += cost;
	limit_mobile_wealth(mob);
	save_char_obj(ch);
	act("$N says '$p is now laced with fire. Nice doing business with you.'", ch,obj,mob,TO_CHAR);
	return;
    }

 if (!str_prefix(arg,"frost"))
    {
	cost = 750000;

	if (IS_SET(obj->value[4], WEAPON_FROST) ||
	    IS_SET(obj->value[4], WEAPON_VAMPIRIC) ||
	    IS_SET(obj->value[4], WEAPON_SHOCKING) ||
	    IS_SET(obj->value[4], WEAPON_POISON) ||
	    IS_SET(obj->value[4], WEAPON_SUCKLE) ||
	    IS_SET(obj->value[4], WEAPON_ENERVATE) ||
	    IS_SET(obj->value[4], WEAPON_ANNEALED) ||
	    IS_SET(obj->value[4], WEAPON_FLAMING))
	{
		act("$N says 'I cannot add anything else to $p.'",
			ch,obj,mob,TO_CHAR);
		return;		
	}

    	if (cost > (ch->gold * 100 + ch->silver))
    	{
		act("$N says 'You don't have enough gold.'",
			ch,obj,mob,TO_CHAR);
		return;	
	}

	SET_BIT(obj->value[4], WEAPON_FROST);

	deduct_cost(ch,cost);
	mob->gold += cost;
	limit_mobile_wealth(mob);
	save_char_obj(ch);
	act("$N says '$p is now laced with frost. Nice doing business with you.'", ch,obj,mob,TO_CHAR);
	return;
    }

 if (!str_prefix(arg,"shocking"))
    {
	cost = 750000;

	if (IS_SET(obj->value[4], WEAPON_FROST) ||
	    IS_SET(obj->value[4], WEAPON_VAMPIRIC) ||
	    IS_SET(obj->value[4], WEAPON_SHOCKING) ||
	    IS_SET(obj->value[4], WEAPON_POISON) ||
	    IS_SET(obj->value[4], WEAPON_SUCKLE) ||
	    IS_SET(obj->value[4], WEAPON_ENERVATE) ||
	    IS_SET(obj->value[4], WEAPON_ANNEALED) ||
	    IS_SET(obj->value[4], WEAPON_FLAMING))
	{
		act("$N says 'I cannot add anything else to $p.'",
			ch,obj,mob,TO_CHAR);
		return;		
	}

    	if (cost > (ch->gold * 100 + ch->silver))
    	{
		act("$N says 'You don't have enough gold.'",
			ch,obj,mob,TO_CHAR);
		return;	
	}

	SET_BIT(obj->value[4], WEAPON_SHOCKING);

	deduct_cost(ch,cost);
	mob->gold += cost;
	limit_mobile_wealth(mob);
	save_char_obj(ch);
	act("$N says '$p is now shocking. Nice doing business with you.'", ch,obj,mob,TO_CHAR);
	return;
    }

 if (!str_prefix(arg,"vampiric"))
    {
	cost = 1000000;

	if (IS_SET(obj->value[4], WEAPON_VAMPIRIC) ||
	    IS_SET(obj->value[4], WEAPON_POISON) ||
	    IS_SET(obj->value[4], WEAPON_SUCKLE) ||
	    IS_SET(obj->value[4], WEAPON_ENERVATE) ||
	    IS_SET(obj->value[4], WEAPON_ANNEALED))
	{
		act("$N says 'I cannot add anything else to $p.'",
			ch,obj,mob,TO_CHAR);
		return;		
	}

    	if (cost > (ch->gold * 100 + ch->silver))
    	{
		act("$N says 'You don't have enough gold.'",
			ch,obj,mob,TO_CHAR);
		return;	
	}

	SET_BIT(obj->value[4], WEAPON_VAMPIRIC);

	deduct_cost(ch,cost);
	mob->gold += cost;
	limit_mobile_wealth(mob);
	save_char_obj(ch);
	act("$N says '$p is now vampiric. Nice doing business with you.'", ch,obj,mob,TO_CHAR);
	return;
    }

 if (!str_prefix(arg,"suckle"))
    {
	cost = 1000000;

	if (IS_SET(obj->value[4], WEAPON_VAMPIRIC) ||
	    IS_SET(obj->value[4], WEAPON_POISON) ||
	    IS_SET(obj->value[4], WEAPON_SUCKLE) ||
	    IS_SET(obj->value[4], WEAPON_ENERVATE) ||
	    IS_SET(obj->value[4], WEAPON_ANNEALED))
	{
		act("$N says 'I cannot add anything else to $p.'",
			ch,obj,mob,TO_CHAR);
		return;		
	}

    	if (cost > (ch->gold * 100 + ch->silver))
    	{
		act("$N says 'You don't have enough gold.'",
			ch,obj,mob,TO_CHAR);
		return;	
	}

	SET_BIT(obj->value[4], WEAPON_SUCKLE);

	deduct_cost(ch,cost);
	mob->gold += cost;
	limit_mobile_wealth(mob);
	save_char_obj(ch);
	act("$N says '$p will now drain mana. Nice doing business with you.'", ch,obj,mob,TO_CHAR);
	return;
    }

 if (!str_prefix(arg,"poison"))
    {
	cost = 1000000;

	if (IS_SET(obj->value[4], WEAPON_VAMPIRIC) ||
	    IS_SET(obj->value[4], WEAPON_POISON) ||
	    IS_SET(obj->value[4], WEAPON_SUCKLE) ||
	    IS_SET(obj->value[4], WEAPON_ENERVATE) ||
	    IS_SET(obj->value[4], WEAPON_ANNEALED))
	{
		act("$N says 'I cannot add anything else to $p.'",
			ch,obj,mob,TO_CHAR);
		return;		
	}

    	if (cost > (ch->gold * 100 + ch->silver))
    	{
		act("$N says 'You don't have enough gold.'",
			ch,obj,mob,TO_CHAR);
		return;	
	}

	SET_BIT(obj->value[4], WEAPON_POISON);

	deduct_cost(ch,cost);
	mob->gold += cost;
	limit_mobile_wealth(mob);
	save_char_obj(ch);
	act("$N says '$p is now laced with poison. Nice doing business with you.'", ch,obj,mob,TO_CHAR);
	return;
    }

 if (!str_prefix(arg,"annealed"))
    {
	cost = 1000000;

	if (IS_SET(obj->value[4], WEAPON_VAMPIRIC) ||
	    IS_SET(obj->value[4], WEAPON_POISON) ||
	    IS_SET(obj->value[4], WEAPON_SUCKLE) ||
	    IS_SET(obj->value[4], WEAPON_ENERVATE) ||
	    IS_SET(obj->value[4], WEAPON_ANNEALED))
	{
		act("$N says 'I cannot add anything else to $p.'",
			ch,obj,mob,TO_CHAR);
		return;		
	}

    	if (cost > (ch->gold * 100 + ch->silver))
    	{
		act("$N says 'You don't have enough gold.'",
			ch,obj,mob,TO_CHAR);
		return;	
	}

	SET_BIT(obj->value[4], WEAPON_ANNEALED);

	deduct_cost(ch,cost);
	mob->gold += cost;
	limit_mobile_wealth(mob);
	save_char_obj(ch);
	act("$N says '$p is now annealed. Nice doing business with you.'", ch,obj,mob,TO_CHAR);
	return;
    }


 if (!str_prefix(arg,"enervate"))
    {
	cost = 1000000;

	if (IS_SET(obj->value[4], WEAPON_VAMPIRIC) ||
	    IS_SET(obj->value[4], WEAPON_POISON) ||
	    IS_SET(obj->value[4], WEAPON_SUCKLE) ||
	    IS_SET(obj->value[4], WEAPON_ENERVATE) ||
	    IS_SET(obj->value[4], WEAPON_ANNEALED))
	{
		act("$N says 'I cannot add anything else to $p.'",
			ch,obj,mob,TO_CHAR);
		return;		
	}

    	if (cost > (ch->gold * 100 + ch->silver))
    	{
		act("$N says 'You don't have enough gold.'",
			ch,obj,mob,TO_CHAR);
		return;	
	}

	SET_BIT(obj->value[4], WEAPON_ENERVATE);

	deduct_cost(ch,cost);
	mob->gold += cost;
	limit_mobile_wealth(mob);
	save_char_obj(ch);
	act("$N says '$p is now enervated. Nice doing business with you.'", ch,obj,mob,TO_CHAR);
	return;
    }
	act("$N says 'I am able to forge the following on your items:'",ch,NULL,mob,TO_CHAR);
	ch->println("");
	ch->println("Type PURCHASE <affect> <item worn>");
	ch->println("  burnproof           1000 gold");
	ch->println("=========== Weapons ===========");
	ch->println("  sharp               1000 gold");
	ch->println("  vorpal              2000 gold");
	ch->println("  flaming             7500 gold");
	ch->println("  frost               7500 gold");
	ch->println("  lightning           7500 gold");
	ch->println("  poison             10000 gold");
	ch->println("  vampiric  (hp)     10000 gold");
	ch->println("  annealed (stun)    10000 gold");
	ch->println("  suckle   (mana)    10000 gold");
	ch->println("  enervate (moves)   10000 gold");
	return;
}
/**************************************************************************/
void do_stargate(char_data *ch, char *argument)
{
    	char arg1[MIL];
    	OBJ_DATA *portal = NULL;
    	OBJ_DATA *obj = NULL;
	
    	if (ch->position == POS_SLEEPING)
    	{
		ch->println("You can't do that while sleeping.");
        	return;
    	}

    	argument = one_argument( argument, arg1);

	obj = get_obj_list(ch,argument,ch->in_room->contents);

	if (obj == NULL)
	{
		ch->println("There is no Stellar Gateway here.");
		return;
	}

    	if (obj->pIndexData->vnum != 7900)
	{
		ch->println("There is no Stellar Gateway here.");
		return;
	}

    	if ( arg1[0] == '\0' && ch->fighting == NULL)
    	{
		ch->println ("Where did you want to set the Stellar Gateway to ?");
		return;
    	}
    	if ( arg1[1] == '\0' && ch->fighting == NULL)
    	{
		ch->println ("Where did you want to set the Stellar Gateway to ?");
		return;
    	}
    	if ( arg1[2] == '\0' && ch->fighting == NULL)
    	{
		ch->println ("Where did you want to set the Stellar Gateway to ?");
		return;
    	}
    	if ( arg1[3] == '\0' && ch->fighting == NULL)
    	{
		ch->println ("Where did you want to set the Stellar Gateway to ?");
		return;
    	}
    	if ( arg1[4] == '\0' && ch->fighting == NULL)
    	{
		ch->println ("Where did you want to set the Stellar Gateway to ?");
		return;
    	}

   	if (obj->item_type == ITEM_PORTAL)
	{
		ch->println("You must wait for the current portal to close.");
		return;
	}

	portal = create_object(get_obj_index(90));
	portal->timer = 2; 
	SET_BIT(portal->value[2],GATE_OPAQUE);

	act("$n presses the ancient symbols on the Stellar Gateway.",ch,NULL,NULL,TO_ROOM);
	act("You press the ancient symbols on the Stellar Gateway.",ch,NULL,NULL,TO_CHAR);

	//StupidWorld
	if ( !str_cmp( arg1, "a1z2e" ))
    	{
		portal->value[3] = 7900;
		obj_to_room(portal,ch->in_room);
		act("$p ripples inside the Stellar Gateway.",ch,portal,NULL,TO_ROOM);
		act("$p ripples inside the Stellar Gateway.",ch,portal,NULL,TO_CHAR);
		return;
    	}

    	// Arthos
	if ( !str_cmp( arg1, "b2r5f" ))
    	{
		portal->value[3] = 753;
		obj_to_room(portal,ch->in_room);
		act("$p shimmers inside the Stellar Gateway.",ch,portal,NULL,TO_ROOM);
		act("$p shimmers inside the Stellar Gateway.",ch,portal,NULL,TO_CHAR);
		return;
    	}

	// Olympus
	if ( !str_cmp( arg1, "c3o1a" ))
    	{
		portal->value[3] = 7601;
		obj_to_room(portal,ch->in_room);
		act("$p shimmers inside the Stellar Gateway.",ch,portal,NULL,TO_ROOM);
		act("$p shimmers inside the Stellar Gateway.",ch,portal,NULL,TO_CHAR);
		return;
    	}

	// Abaddon
	if ( !str_cmp( arg1, "d5b4c" ))
    	{
		portal->value[3] = 9460;
		obj_to_room(portal,ch->in_room);
		act("$p shimmers inside the Stellar Gateway.",ch,portal,NULL,TO_ROOM);
		act("$p shimmers inside the Stellar Gateway.",ch,portal,NULL,TO_CHAR);
		return;
    	}

	// Tiamat
	if ( !str_cmp( arg1, "a1a1a" ))
    	{
		portal->value[3] = 4131;
		obj_to_room(portal,ch->in_room);
		act("$p shimmers inside the Stellar Gateway.",ch,portal,NULL,TO_ROOM);
		act("$p shimmers inside the Stellar Gateway.",ch,portal,NULL,TO_CHAR);
		return;
    	}

	// Krynn
	if ( !str_cmp( arg1, "e2k3b" ))
    	{
		portal->value[3] = 82282;
		obj_to_room(portal,ch->in_room);
		act("$p shimmers inside the Stellar Gateway.",ch,portal,NULL,TO_ROOM);
		act("$p shimmers inside the Stellar Gateway.",ch,portal,NULL,TO_CHAR);
		return;
    	}

	act("$n fails to activate the Stellar Gateway.",ch,NULL,NULL,TO_ROOM);
	act("You fail to activate the Stellar Gateway.",ch,NULL,NULL,TO_CHAR);
	extract_obj(portal);
    	return;
}
/**************************************************************************/
