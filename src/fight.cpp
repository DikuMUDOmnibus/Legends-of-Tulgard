/**************************************************************************/
// fight.cpp - 
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
#include "clan.h"
#include "duel.h"
#include "ictime.h"
#include "msp.h"

#define MAX_DAMAGE_MESSAGE 48
DECLARE_OSPEC_FUN( ospec_chameleon_mark );
void hunt_victim  args( (char_data *ch)); // Ixliam
void weapon_effects( char_data *ch, char_data *victim, obj_data *wield);
/* command procedures needed */
DECLARE_DO_FUN(do_backstab      );
DECLARE_DO_FUN(do_pbackstab		);
DECLARE_DO_FUN(do_circle		);
DECLARE_DO_FUN(do_emote         );
DECLARE_DO_FUN(do_berserk       );
DECLARE_DO_FUN(do_bash          );
DECLARE_DO_FUN(do_trip          );
DECLARE_DO_FUN(do_dirt          );
DECLARE_DO_FUN(do_flee          );
DECLARE_DO_FUN(do_kick          );
DECLARE_DO_FUN(do_disarm        );
DECLARE_DO_FUN(do_get           );
DECLARE_DO_FUN(do_recall        );
DECLARE_DO_FUN(do_yell          );
DECLARE_DO_FUN(do_gore			); // Kerenos
DECLARE_DO_FUN(do_disarm_trap	);
DECLARE_DO_FUN(do_visible		);
DECLARE_SPEC_FUN( spec_cast_mage );
DECLARE_SPEC_FUN( spec_cast_cleric );
DECLARE_DO_FUN(do_wolfreturn	); // Ixliam
DECLARE_DO_FUN(do_look);
DECLARE_DO_FUN(do_startprison   ); // Ixliam
DECLARE_DO_FUN(do_gaze          ); // Ixliam
DECLARE_DO_FUN(do_crush         ); // Ixliam
DECLARE_OSPEC_FUN( ospec_quicksilver_orb );
DECLARE_OSPEC_FUN( ospec_soulscream_shield );
OBJ_DATA  *random_object args(( int level )); // Random Objects
char *get_weapontype(OBJ_DATA *obj);
void landchar( char_data *ch);
int group_max_level_attacking_victim(char_data *killer, char_data *victim );
void group_bypass_killer_penatly(char_data *killer, char_data *victim, bool death);
void pkill_note_required_message(char_data *ch, char_data *victim, bool bypass_duel_used);
bool can_join_combat( char_data *attacker, char_data *victim);
int switch_update(char_data *ch);
int get_age(char_data *ch);
bool race_can_wear (char_data *ch, OBJ_DATA *obj);

char *  const   death_cry_dir_name[]=
{
    "the north", "the east", "the south", "the west", "above", "below", 
	"the northeast", "the southeast", "the southwest", "the northwest"
};

const struct flag_type corpse_dam_renames[] =
{
	{"pristine",	DAM_NONE,		true},
	{"battered ",		DAM_BASH,		true},
	{"gouged ",		DAM_PIERCE,		true},
	{"tattered ",		DAM_SLASH,		true},
	{"burnt ",		DAM_FIRE,		true},
	{"frozen ",		DAM_COLD,		true},
	{"blackened ",		DAM_LIGHTNING,		true},
	{"liquified ",		DAM_ACID,		true},
	{"sickly ",		DAM_POISON,		true},
	{"twisted ",		DAM_NEGATIVE,		true},
	{"pristine ",		DAM_HOLY,		true},
	{"charred ",		DAM_ENERGY,		true},
	{"clenched",	DAM_MENTAL,		true},
	{"pox-ridden ",		DAM_DISEASE,		true},
	{"bloated ",		DAM_DROWNING,		true},
	{"charred ",		DAM_LIGHT,		true},
	{"pristine",	DAM_OTHER,		true},
	{"rigid ",		DAM_HARM,		true},
	{"pristine",	DAM_CHARM,		true},
	{"battered ",		DAM_SOUND,		true},
	{"pristine",	DAM_ILLUSION,	true},
    { NULL,			0,				   0}
};
/*
 * Local functions.
 */
OBJ_DATA *shape_corpse      args((char_data *ch, obj_data *obj));
void	pkill_autonote		args(( char_data *ch, char_data *victim ));
void    check_assist		args(( char_data *ch, char_data *victim ));
bool    check_dodge			args(( char_data *ch, char_data *victim ));
bool    check_parry			args(( char_data *ch, char_data *victim ));
bool    check_shield_block	args(( char_data *ch, char_data *victim ));
bool    check_spinblock		args(( char_data *ch, char_data *victim ));
void    dam_message			args(( char_data *ch, char_data *victim, int dam,
									int dt, bool immune ));
void    death_cry			args(( char_data *ch, char_data *killer ));
void    group_gain			args(( char_data *ch, char_data *victim ));
int     xp_compute			args(( char_data *gch, char_data *victim,
									int total_levels));
bool    is_safe				args(( char_data *ch, char_data *victim ));
void    make_corpse			args(( char_data *ch, char *killer ));
void    one_hit				args(( char_data *ch, char_data *victim, int dt, bool second ));
void    two_hit				args(( char_data *ch, char_data *victim, int dt, bool second ));
void    mob_hit				args(( char_data *ch, char_data *victim, int dt ));
void	set_fighting		args(( char_data *ch, char_data *victim ));
void    disarm			    args(( char_data *ch, char_data *victim ));
void    entangle		    args(( char_data *ch, char_data *victim ));
void    mount				args(( char_data *, char_data * ));
void    dismount			args(( char_data * ));
void	show_list_to_char	args(( OBJ_DATA *list, char_data *ch, char * filter,
									bool fShort, bool fShowNothing ));
void    make_blood			args(( char_data *ch ));

/**************************************************************************/
// lame way to do this, but it was in the oblivion code and I 
// can't be bothered changing it cause the code is only called 
// when a pkill happens - it was originally put in update_handler() with 
// ran everytime violence_update() was called!!!
void check_death_update(void)
{
	char buf[MSL];

	char_data *victim;
	char_data *next_player;
    for ( victim = player_list; victim; victim = next_player )
    {
		next_player=victim->next_player;
		
		if(victim->pcdata->karns<0)
		{
			flush_char_outbuffer(victim);
			victim->printf(
				"You have been pkilled when you had no karns.  The system is if you\r\n"
				"are pkilled you lose a karn, if you are pkilled when you have no karns\r\n"
				"the death is considered permanent.\r\n"
				"Please keep in mind that it is bad RP to discuss this unless you think\r\n"
				"it was a OOC pkill.  If you consider it ooc please note it to admin\r\n"
				"we will see what we can do about it.\r\n");
			flush_char_outbuffer(victim);

			logf( "PERMDEATH: Closing link to %s.", victim->name);

			char subject[MIL];
			sprintf(subject,"`RPERMDEATH:`W %s!`x", victim->name);
			autonote(NOTE_INOTE, "Your friendly neighbourhood pkill system",
				subject, "imm", "Subject says it all.", true);
			flush_char_outbuffer(victim);
			autonote(NOTE_PKNOTE, "Your friendly neighbourhood pkill system",
				subject, "imm", "Subject says it all.", true);

			char filenamebuf[MIL];
			strcpy(filenamebuf, pfilename(victim->name, victim->pcdata->pfiletype));
			// disconnect them
			extract_char(victim,true);
			connection_close(victim->desc);

			// move them to the dead directory
			sprintf(buf,"mv %s %s &", filenamebuf, DEAD_DIR);
			logf("Systeming: '%s'", buf);
			system(buf);		
		}	
	}
}
/**************************************************************************/
/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void violence_update( void )
{
    char_data *ch;
    char_data *ch_next;
    char_data *victim;
	
    for ( ch = char_list; ch; ch = ch_next )
    {
		ch_next = ch->next;
		
	        /*
	         * Hunting mobs.
 	        */
	        if ( IS_NPC(ch) && ch->fighting == NULL 
		    && !IS_SET(ch->act, ACT_NO_HUNT)
	            && IS_AWAKE(ch) && ch->mobmemory != NULL )
	          {
	            hunt_victim(ch);
	            continue;
	          }
		
		if ( ( victim = ch->fighting ) == NULL || ch->in_room == NULL )
			continue;
		
		// do a room check
		if( ch->in_room!=ch->fighting->in_room){
			bug("violence_update(): ch->in_room != ch->fighting->in_room");
			stop_fighting(ch,false);
			continue;
		}
		
		if(ch->is_stunned)
		{
			ch->is_stunned-=1;
			if(ch->is_stunned<=0)
			{
				ch->position=POS_RESTING;
				ch->is_stunned=0;
				ch->println("You are no longer stunned.");
			}
		}

		obj_data *wpn;
		if((wpn = get_eq_char(ch, WEAR_WIELD)) != NULL
			&& wpn->ospec_fun == ospec_lookup("ospec_quicksilver_orb"))
				ospec_quicksilver_orb(wpn, ch);
		
		if ( IS_AWAKE(ch) && ch->in_room == victim->in_room ){
			multi_hit( ch, victim, TYPE_UNDEFINED );
			REMOVE_BIT(victim->dyn2, DYN2_DOING_ORBIT);
		}else{
			stop_fighting( ch, false ); // Violence_update - not awake/diff room
		}
		
		if (( victim = ch->fighting ) == NULL )
			continue;
		
		// Fun for the whole family!
		check_assist(ch,victim);
		
		if ( IS_NPC( ch ) )	{
			if ( HAS_TRIGGER( ch, TRIG_FIGHT ) )
				mp_percent_trigger( ch, victim, NULL, NULL, TRIG_FIGHT );
			if ( HAS_TRIGGER( ch, TRIG_HPCNT ) )
				mp_hprct_trigger( ch, victim );
      			if ((number_percent() >= 85) && (IS_NPC(ch)) && (!is_affected(ch,AFF_CHARM)))
				switch_update(ch);
		}
	}
	return;
}
/**************************************************************************/
int get_sublevels_for_level(int level);
// Called when a player/mob victim is killed:
// - victim position should be POS_DEAD when called 
// - handles the loss of karns, and reseting of karn countdowns
//   loss of xp when dieing
// - records mobkills, pkills, pkdefeat, pkools
// - does NOT handle pknorecall, pksafe
// - autolooting 
// - autonoting pkills
// - mob death trigger
void kill_char(char_data *victim, char_data *ch)
{
	if(victim->position != POS_DEAD ){
		bug("kill_char(): victim->position != POS_DEAD ");
		return;
	}

	if(!IS_VALID(ch)){
		bugf("kill_char(): ch (%d) is not valid!", ch->vnum());
	}

	ROOM_INDEX_DATA *deathroom;
	if(ch->in_room){
		deathroom=ch->in_room;
	}else if(victim->in_room){
		deathroom=victim->in_room;
		bug("kill_char(): ch->in_room==NULL, setting it to victim->in_room");
		ch->in_room=victim->in_room;
	}else{
		bugf("kill_char(): Couldn't find DEATHROOM!!!... setting it to limbo.");
		deathroom=get_room_index(ROOM_VNUM_LIMBO);
		assertp(deathroom);
	}
	
	group_gain( ch, victim );


	
	// record mob kill values
	if (!IS_NPC(ch) && IS_NPC(victim)){
		ch->pcdata->mkills++;
	}else if (IS_NPC(ch) && !IS_NPC(victim)){
		victim->pcdata->mdefeats++;
	}
	
	if ( !IS_NPC(victim) )
	{

	    if ( class_table[ch->clss].class_cast_type == CCT_BARD )
	    {
     		ch->songp = 0;
	    	ch->songn = 0;
	    }

	    OBJ_DATA *token;
	    if ( ( ( token = get_obj_token( victim, "10395")) != NULL) && victim->pcdata->karns>=0)
	    { 
		   obj_from_char (token);
		   extract_obj (token);
		   victim->println("You avoid your own death and suddenly vanish!");
		   ch->printlnf("%s avoids death at your hands and vanishes!", victim->name);
		   victim->position = POS_RESTING;
	 	   char_from_room(victim);
		   char_to_room(victim, get_room_index(3));
		   victim->hit = victim->max_hit;
		   do_look(victim, "auto");
		   return;
	    }


/************** ARREST **************************************************/

    if (IS_SET(victim->act2, ACT2_UNDERARREST) 
    && IS_AFFECTED2(ch, AFF2_BOUNTYHUNTER)
    && !IS_AWAKE(victim) && IS_NPC(ch))
  
    {
	victim->pknorecall = 0; 
	victim->pknoquit = 0;	
	act( "$n is placed in shackles and has been arrested.", victim, NULL, NULL, TO_ROOM );
	victim->println("You have been arrested and taken to prison.");
	stop_fighting( victim, true); // Damage - subdued
	victim->position=POS_RESTING;
	victim->hit=1;

	if(victim->pcdata->theft > 0 && victim->pcdata->murder == 0)
		info_broadcast(ch, "%s has been arrested by the guards for ROBBERY.", victim->name);
	
	if(victim->pcdata->theft == 0 && victim->pcdata->murder > 0)
		info_broadcast(ch, "%s has been arrested by the guards for MURDER.", victim->name);

	if(victim->pcdata->theft > 0 && victim->pcdata->murder > 0)
		info_broadcast(ch, "%s has been arrested by the guards for ROBBERY & MURDER.", victim->name);

	REMOVE_BIT(victim->act2, ACT2_UNDERARREST);
	sprintf( log_buf, "%s arrested by %s at %d", victim->name,
	(IS_NPC(ch) ? ch->short_descr : ch->name),
		ch->in_room->vnum );
	log_string( log_buf );
		
	wiznet(log_buf,NULL,NULL,WIZ_BETA,0,0);
	if(victim->pcdata->murder > 3 && victim->pcdata->murder < 50)
		victim->pcdata->murder = victim->pcdata->murder / 3;
	if(victim->pcdata->theft > 3)
		victim->pcdata->theft = victim->pcdata->theft / 3;

	if (victim->pcdata->theft == 0 && victim->pcdata->murder == 0 
				      && IS_WEREBEAST(victim))
	{
		info_broadcast(ch, "%s has been arrested by the guards for LYCANTHROPY.", victim->name);
		victim->pcdata->murder = 1;
	}

	do_startprison(victim, "");
	return;
    }


    if (IS_SET(victim->act2, ACT2_UNDERARREST) 
    && IS_AFFECTED2(ch, AFF2_BOUNTYHUNTER)
    && !IS_AWAKE(victim))
  
		{
		ch->pknorecall = 5;
		ch->pknoquit = 5;
		victim->pknorecall = 0; 
		victim->pknoquit = 0;	
		act( "$n is placed in shackles and has been arrested.", victim, NULL, NULL, TO_ROOM );
		victim->println("You have been arrested and taken to prison.");
		stop_fighting( victim, true); // Damage - subdued
		stop_fighting( ch, true); // Damage - subdued
		group_gain( ch, victim );
		victim->subdued= true;
		victim->subdued_timer = number_range(5,10);
		victim->position=POS_RESTING;
		victim->hit=1;
		int bounty;
		bounty= (victim->pcdata->theft + victim->pcdata->murder) * 50;

		if(bounty > 500)
			bounty = 500;

		ch->printlnf("`#`YYou receive a bounty of %d gold.`^", bounty);
		ch->gold += bounty;

		if(victim->pcdata->theft > 0 && victim->pcdata->murder == 0)
			info_broadcast(ch, "%s has been arrested for ROBBERY.", victim->name);
		
		if(victim->pcdata->theft == 0 && victim->pcdata->murder > 0)
			info_broadcast(ch, "%s has been arrested for MURDER.", victim->name);

		if(victim->pcdata->theft > 0 && victim->pcdata->murder > 0)
			info_broadcast(ch, "%s has been arrested for ROBBERY & MURDER.", victim->name);
		

		REMOVE_BIT(victim->act2, ACT2_UNDERARREST);
		sprintf( log_buf, "%s arrested by %s at %d",
			victim->name,
			(IS_NPC(ch) ? ch->short_descr : ch->name),
			ch->in_room->vnum );
		log_string( log_buf );
		
		wiznet(log_buf,NULL,NULL,WIZ_BETA,0,0);
		do_startprison(victim, "");
		return;
    }
/****************************************************************/


		if (IS_NPC(ch)){ // mob killed player
			sprintf( log_buf, "%s mkilled by %s at %d (kill_char())",
				victim->name,
				ch->short_descr,
				deathroom->vnum);
			
			// send the info
			info_broadcast(victim, "%s has been toasted by %s.",	victim->name, ch->short_descr);
			if(ch->race == race_lookup("vampire") 
			&& !IS_VAMPIRE(victim) && victim->clan<0
			&& !IS_WEREBEAST(victim)
			&& victim->race != race_lookup("kender") 
			&& victim->race != race_lookup("avian")

			&& victim->race != race_lookup("dragon")
			&& victim->race != race_lookup("cambion")
			&& victim->race != race_lookup("changeling")
			&& victim->race != race_lookup("cyclops")
			&& victim->race != race_lookup("goblin")
			&& victim->race != race_lookup("gnoll")

			&& victim->race != race_lookup("faerie") 
			&& victim->race != race_lookup("troll")
			&& victim->race != race_lookup("centaur") 
			&& victim->race != race_lookup("draconian")
			&& victim->race != race_lookup("minotaur") 
			&& victim->race != race_lookup("thri-kreen")
			&& victim->race != race_lookup("demon") 
			&& victim->race != race_lookup("lich")
			&& victim->race != race_lookup("angel")
    			&& class_table[victim->clss].class_cast_type != CCT_BARD
			&& victim->race != race_lookup("golem"))
			{
			    SET_BIT(victim->affected_by2, AFF2_VAMP_BITE);
			    victim->alliance = -3;
			    victim->println("`RYou will arise from your death as a vampire.`x");
				 int vclan = number_range(0,7);
	  			 switch(vclan) 
    			 {
    				case (0) :
							SET_BIT(victim->vampire, VAM_BRUJAH);
							break;
    				case (1) :
							SET_BIT(victim->vampire, VAM_ASSAMITE);
							break;
    				case (2) :
							SET_BIT(victim->vampire, VAM_CAPPADOCIAN);
							break;
    				case (3) :
							SET_BIT(victim->vampire, VAM_GANGREL);
							break;
    				case (4) :
							SET_BIT(victim->vampire, VAM_MALKAVIAN);
							break;
    				case (5) :
							SET_BIT(victim->vampire, VAM_TOREADOR);
							break;
    				case (6) :
							SET_BIT(victim->vampire, VAM_TREMERE);
							break;
    				case (7) :
							SET_BIT(victim->vampire, VAM_VENTRUE);
							break;
				 }

			    victim->vampgen = 13;
			    victim->bloodpool = 2;
			}

			// Log mobkills
			{
				char mklogbuf[MSL];
				sprintf( mklogbuf, "%s mkilled by %s at %d (kill_char())",
					victim->name,
					ch->short_descr,
					deathroom->vnum);
				append_datetime_ch_to_file( ch, MKILL_LOGFILE, mklogbuf);
			}
			
		}else{ // player killed player
			sprintf( log_buf, "%s pkilled by %s at %d (kill_char())",
				victim->name,
				ch->name,
				deathroom->vnum);

			// send the info
			info_broadcast(victim, "%s has been toasted by %s!", victim->name, ch->name);

		}
		
		log_string( log_buf );


		// ch didn't start the fight so they dont get the pkills on them
		// reducing their max karns		
		if(ch->pkool>0 && victim->level<5)
		{
				ch->pkkills--;
		}

		if(victim->level<11 && !IS_LETGAINED(victim) && GAMESETTING_LETGAINING_IS_REQUIRED){
			if(!GAMESETTING5(GAMESET5_DEDICATED_PKILL_STYLE_MUD)){
				ch->pkool+=4000;
			}
		}
		
		// pk system
		if(ch!=victim && !IS_NPC(ch) && !IS_NPC(victim))
		{
			ch->pkkills+=1;
			victim->pkdefeats+=1;
			if(!GAMESETTING5(GAMESET5_DEDICATED_PKILL_STYLE_MUD)){
				// new pk karn system
				if(group_max_level_attacking_victim(ch, victim)<=victim->level+10)
				{

					OBJ_DATA  *wrist1 = get_eq_char(victim, WEAR_WRIST_L);
					OBJ_DATA  *wrist2 = get_eq_char(victim, WEAR_WRIST_R);

					if (wrist1!=NULL && wrist1->pIndexData->vnum==5097)
					{
						act( "$n's $p dissolves as it absorbs a violent death.", victim, wrist1, NULL, TO_ROOM );
						act( "Your $p dissolves as it absorbs a violent death. You keep a karn!", victim, wrist1, NULL, TO_CHAR );
						extract_obj(wrist1);
					}
					else
					if (wrist2!=NULL && wrist2->pIndexData->vnum==5097)
					{
						act( "$n's $p dissolves as it absorbs a violent death.", victim, wrist2, NULL, TO_ROOM );
						act( "Your $p dissolves as it absorbs a violent death. You keep a karn!", victim, wrist2, NULL, TO_CHAR );
						extract_obj(wrist2);
					}else
					{
						victim->pcdata->karns--;
				
						if(GAMESETTING(GAMESET_NOPERMDEATH) && victim->pcdata->karns<0){
							victim->pcdata->karns=0;
						}
						if(victim->pcdata->karns) // if they actually lost a karn
						{
							victim->println(
								"From within your body you feel a surge of energy,\r\n"
								"a ball of light departs from your body and disappears!");
						}
					}
				}
				group_bypass_killer_penatly(ch, victim, true);


				if(GAMESETTING3(GAMESET3_KILLER_SYSTEM_ENABLED)
					&& !IS_SET(victim->dyn, DYN_STARTED_FIGHT))
				{
					// mark the attackers as killers if the victim didn't start the fight
					char_data *gch;
					for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
					{
						if ( !IS_NPC(gch) && gch->fighting==victim && is_same_group( gch, ch))
						{
							if(!IS_KILLER(gch)){
								ch->println( "*** You are now tagged as a KILLER!! ***");
							}
							gch->pcdata->killer_until=current_time+ (60*game_settings->killer_system_tagged_duration);
							save_char_obj( gch );				
						}
					}
				}

				// reduce the victims killer timer
				if(IS_KILLER(victim)){
					victim->pcdata->killer_until-=
						(60*game_settings->killer_system_death_reduction_duration);
					if(!IS_KILLER(victim)){
						victim->pcdata->killer_until=0;
					}					
				}

				// update duels to protect victim
				duel_protect_victim(victim);
				// karn checking is done at the end of this function
			}

			if(	ch->pknorecall==0){
				ch->pknorecall=5;
			}
			ch->pknoquit=UMAX(ch->pknorecall,ch->pknoquit);
			if(GAMESETTING5(GAMESET5_DEDICATED_PKILL_STYLE_MUD)){
				ch->pknoquit=UMIN(ch->pknoquit,2);
			}

			victim->pcdata->next_karn_countdown=
				UMIN(victim->pcdata->next_karn_countdown,GET_NEW_KARN_COUNTER(victim));
			
			// autonote the pkill
    			if(!IS_SET(victim->in_room->room2_flags,ROOM2_ARENA) &&
                          !IS_SET(victim->in_room->room2_flags,ROOM2_WAR) )
    			{
				pkill_autonote(ch, victim);
			}

			// deactivate autopkassist
			if (IS_SET(victim->dyn,DYN_AUTOPKASSIST))
			{
				victim->println("`YYour autopkassist setting has been deactivated.`x");
				REMOVE_BIT(victim->dyn,DYN_AUTOPKASSIST);
			}

			// remove thief status on death
			if(IS_THIEF(victim)){
				victim->println("You are no longer marked as a thief");
				victim->pcdata->thief_until=0;
			}

		}		
		
		// Dying penalty: 2/3 way back to previous level for those level 20 or lower
		{
			if(victim->level<21)
			{
				if ( victim->exp > exp_per_level(victim,victim->pcdata->points)
					* victim->level )
				{

					gain_exp( victim, (2 * (exp_per_level(victim,victim->pcdata->points)
						* victim->level - victim->exp)/3) + 50 );
				}
			}
			else
			{
				int xp_amount;
				if(victim->level>50){
					xp_amount=500+(get_sublevels_for_level( victim->level) *150);
				}else{
					xp_amount=500;
				}

				if(IS_HERO(victim)){
					if(GAMESETTING5(GAMESET5_HEROS_DONT_LOSE_XP_FOR_DYING)){
						victim->printlnf("You would have lost %d xp if you weren't a hero.", xp_amount);
					}else{
						victim->printlnf("You have lost %d xp.", xp_amount);
						gain_exp( victim, -xp_amount);
					}
				}else{					
					victim->printlnf("You have lost %d xp.", xp_amount);
					gain_exp( victim, -xp_amount);
				}

				if ( IS_HERO( victim )){
					do_heroxp( victim, -xp_amount);
				}

				if(victim->exp<exp_per_level(victim,victim->pcdata->points)* victim->level )
				{
					drop_level(victim);				
					check_perm_damage(victim);
				}else{
					if(number_range(1,get_sublevels_for_level( victim->level+5))<2){
						check_perm_damage(victim);
					}else{
						victim->println("Luckily you have not suffered serious injury.");
					}
				}
			}
		}
	}
	
	sprintf( log_buf, "%s [lvl %d] got toasted by %s at %s [lvl %d] [room %d] (%s)",
		(IS_NPC(victim) ? victim->short_descr : victim->name),
		victim->level,
		(IS_NPC(ch) ? ch->short_descr : ch->name),
		deathroom->name, ch->level, deathroom->vnum,
		deathroom->area->name);
	
	if (IS_NPC(victim)){
		wiznet(log_buf,NULL,NULL,WIZ_MOBDEATHS,0,0);
	}else{
		wiznet(log_buf,NULL,NULL,WIZ_DEATHS,0,0);
	}		
	
	// Death trigger mobprog trigger
	if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_DEATH) )
	{
		victim->position = POS_STANDING;
		mp_percent_trigger( victim, ch, NULL, NULL, TRIG_DEATH );
	}
	

	ch->mobmemory = NULL;
	victim->mobmemory = NULL;

	raw_kill( victim, ch );
	
	// handle players autolooting and autogold mob corpses
	if ( !IS_NPC(ch) && IS_NPC(victim) )
	{
		OBJ_DATA *corpse;
		
		corpse = get_obj_list( ch, "corpse", deathroom->contents );
		
		if(corpse && corpse->contains) // a corpse exists and isnt empty 
		{
			if (IS_SET(ch->act, PLR_AUTOLOOT))
			{
				do_get( ch, "all corpse" );
			}
			else
			{
				if(IS_SET(ch->act,PLR_AUTOGOLD) )
				{
					OBJ_DATA *coins=get_obj_list(ch,"gcash",corpse->contains);
					if (coins)
					{
						do_get(ch, "all.gcash corpse");
					}
				}
			}
		}
		if ( corpse
		&&   HAS_CONFIG(ch, CONFIG_AUTOEXAMINE)
		&&  !IS_SET( ch->act,PLR_AUTOLOOT ))
		{
			act( "$p holds:", ch, corpse, NULL, TO_CHAR );
			show_list_to_char( corpse->contains, ch, "", true, true );
		}
	}
	if(!IS_NPC(victim)){
		check_death_update();
	}
}
/**************************************************************************/
// for auto assisting 
// victim is being attacked by ch... will rch join in
void check_assist(char_data *ch,char_data *victim)
{
	char_data *rch, *rch_next;
	
	for (rch = ch->in_room->people; rch != NULL; rch = rch_next)
	{
		rch_next = rch->next_in_room;
		
		if (IS_AWAKE(rch) && rch->fighting == NULL)
		{			
			// quick check for ASSIST_PLAYER 
			if (!IS_NPC(ch) && IS_NPC(rch)
				&& IS_SET(rch->off_flags,ASSIST_PLAYERS)
				&&  rch->level + 6 > victim->level)
			{
				do_emote(rch,"screams and attacks!");
				multi_hit(rch,victim,TYPE_UNDEFINED);
				continue;
			}
			
			// PCs next 
			if (!IS_NPC(ch) || IS_AFFECTED(ch,AFF_CHARM))
			{
				if(	!is_same_group(ch,rch)
					|| !can_join_combat(rch, victim)
					|| is_safe(rch, victim)){
					continue;
				}

				if(IS_NPC(victim)){
					if(!IS_AFFECTED(rch,AFF_CHARM)
						&& !(!IS_NPC(rch) && IS_SET(rch->act,PLR_AUTOASSIST))){
						continue;
					}
				}else{
					if(IS_NPC(rch)){
						if(!IS_AFFECTED(rch,AFF_CHARM)){
							continue;
						}
					}else{
						if(!IS_SET(rch->dyn,DYN_AUTOPKASSIST)){
							rch->println("`YYou dont automatically assist because you dont have autopkassist is off.`x");
							continue;
						}
					}
				}
				// player attacking another player
				if (!IS_NPC(rch) && !IS_NPC(victim))
				{
					logf( "%s attacking %s at %d (check_assist()(charm) might start pkill)",
						rch->name,
						victim->name,
						rch->in_room->vnum );
				}
				multi_hit (rch,victim,TYPE_UNDEFINED);
				continue;
			}
			
			//for horse attacking characters
			if (victim->fighting && victim->fighting==rch->mounted_on)
			{
				// player attacking another player
				if (!IS_NPC(rch) && !IS_NPC(victim))
				{
					logf( "%s attacking %s at %d (check_assist() might start pkill)",
						rch->name,
						victim->name,
						rch->in_room->vnum );
				}
				
				multi_hit(rch, victim, TYPE_UNDEFINED);
				continue;
			}
			
			// now check the NPC cases			
			if (IS_NPC(ch) && !IS_AFFECTED(ch,AFF_CHARM))
			{
				if ( (IS_NPC(rch) && IS_SET(rch->off_flags,ASSIST_ALL))
					||   (IS_NPC(rch) && rch->helpgroup && rch->helpgroup == ch->group)
					||   (IS_NPC(rch) && rch->race == ch->race
					&& IS_SET(rch->off_flags,ASSIST_RACE))
					||   (IS_NPC(rch) && IS_SET(rch->off_flags,ASSIST_ALIGN)
					&&   ((IS_GOOD(rch)    && IS_GOOD(ch))
					||  (IS_EVIL(rch)    && IS_EVIL(ch))
					||  (IS_NEUTRAL(rch) && IS_NEUTRAL(ch)))) 
					||   (rch->pIndexData == ch->pIndexData
					&& IS_SET(rch->off_flags,ASSIST_VNUM)))
					
				{
					char_data *vch;
					char_data *target;
					int number;
					
					if (number_bits(1) == 0)
						continue;
					
               target = NULL;
               number = 0;
               for (vch = ch->in_room->people; vch; vch = vch->next_in_room)
               {
                  if (can_see(rch,vch)
                     &&  is_same_group(vch,victim)
                     &&  number_range(0,number) == 0)
                  {
                     target = vch;
                     number++;
                  }
               }
					
					if (target != NULL)
					{
						do_emote(rch,"screams and attacks!");
						multi_hit(rch,target,TYPE_UNDEFINED);
					}
				}       
			}
		}
	}
}


/**************************************************************************/
// Do one group of attacks.
void multi_hit( char_data *ch, char_data *victim, int dt )
{
	int chance;
	
	if(IS_MOUNTED(ch)) 
	{
		ch->println("Cant do that.");
		return;
	}

	if(!IS_NPC(ch) && !IS_NPC(victim) && ch!=victim)
	{

		if ( class_table[ch->clss].class_cast_type == CCT_BARD )
		{
     			ch->songp = 0;
	     		ch->songn = 0;
		}

		// transfer those to the void that are attacked 
		// while linkdead and dont have a pkill timer
		if ( victim->pknorecall ==0 
			&& IS_LINKDEAD(victim)
			&& victim->was_in_room == NULL 
			&& victim->in_room != NULL )
		{
			victim->was_in_room = victim->in_room;
			if ( victim->fighting != NULL ){
				stop_fighting( victim, true ); // Multi_hit linkdead transfer to void
			}
			act( "$n disappears into the void.", victim, NULL, NULL, TO_ROOM );
			victim->println("You disappear into the void.");
			if (victim->level > 1)
				save_char_obj( victim);
			char_from_room( victim);
			char_to_room( victim, get_room_index( ROOM_VNUM_LIMBO ) );
			return;
		}

		ch->pknorecall=15;
		victim->pknorecall=15; 
		
		if(GAMESETTING5(GAMESET5_DEDICATED_PKILL_STYLE_MUD)){
			ch->pknoquit	=2;
			victim->pknoquit=2;
		}else{
			ch->pknoquit	=20;
			victim->pknoquit=20;
		}

		if (IS_SET(victim->act2, ACT2_UNDERARREST) && 
		    IS_AFFECTED2(ch, AFF2_BOUNTYHUNTER))
		   {
			ch->pknorecall = 5;
			ch->pknoquit = 5;
			
			victim->pknorecall = 5; 
			victim->pknoquit = 5;	    
		   } 

	}
	
	// decrement the wait 
	if (ch->desc == NULL)
		ch->wait = UMAX(0,ch->wait - PULSE_VIOLENCE);
	
	if (ch->desc == NULL)
		ch->daze = UMAX(0,ch->daze - PULSE_VIOLENCE);
	
	
	// no attacks for stunnies -- just a check 
	if (ch->position < POS_RESTING)
		return;
	
	if(is_affected( ch, gsn_hold_person))
		return;

	if (IS_NPC(ch))
	{
		mob_hit(ch,victim,dt);
		return;
	}
	if(!IS_SET(ch->dyn2, DYN2_DOING_ORBIT))	
		one_hit( ch, victim, dt, false );
	
	if (ch->fighting != victim)
		return;
	
	if (IS_AFFECTED(ch,AFF_HASTE))
		one_hit(ch,victim,dt,false);

	/* VAMPIRE ATTACKS - Ixliam - March 2004*/

    	if (IS_VAMPAFF(ch, VAM_FANGS) && (dt != gsn_backstab)) 
	    	one_hit(ch,victim,dt,false);

   	/* IF CHARACTER HAS CELERITY, DO EXTRA ATTACKS */
    	if ( !IS_NPC(ch) && IS_VAMPAFF(ch, VAM_CELERITY) && (dt != gsn_backstab) )
    	{
	   	int celatt = 1;
	   	if ( get_age(ch) > 199 )
		  	celatt = 2;
	   	if ( get_age(ch) > 399 )
			celatt = 3;
	   	if ( get_age(ch) > 599 )
		  	celatt = 4;
	   	if (get_age(ch)>199
		&& number_percent() < ((13-ch->vampgen)* 5))
		  	celatt += 1;
	   	if ( celatt > 4 )
		  	celatt = 4;
		int l = 0;
	   	for (l=0; l < celatt; ++l)
	   	{
			one_hit(ch,victim,dt,false);
	   	}     
    	}

   	/* WISH EXTRA ATTACKS */
    	if ( !IS_NPC(ch) && IS_SET(ch->wish, WISH_ATTACK_1) && (dt != gsn_backstab) )
    	{
		one_hit(ch,victim,dt,false);
    	}
    	if ( !IS_NPC(ch) && IS_SET(ch->wish, WISH_ATTACK_2) && (dt != gsn_backstab) )
    	{
		one_hit(ch,victim,dt,false);
    	}
    	if ( !IS_NPC(ch) && IS_SET(ch->wish, WISH_ATTACK_3) && (dt != gsn_backstab) )
    	{
		one_hit(ch,victim,dt,false);
    	}
	
	// extra attack if defending your mount
	if (victim->fighting==ch->mounted_on)
		one_hit(ch,victim,dt,false);
	
	if (get_eq_char (ch, WEAR_SECONDARY))
	{
		if(GAMESETTING2(GAMESET2_NO_SECOND_SKILL_REQUIRED)){
			one_hit( ch, victim, dt, true );
			if ( ch->fighting != victim ){
				return;
			}
		}else{
			if(number_range(ch->get_skill(gsn_second), 100)>90){
				check_improve(ch,gsn_second,true,5);
				one_hit( ch, victim, dt, true );
				if ( ch->fighting != victim ){
					return;
				}
			}else{
				check_improve(ch,gsn_second,false,1);
			}
		}
	}
	
	if (get_eq_char (ch, WEAR_2WIELD))
	{
		if(number_range(ch->get_skill(gsn_triwield), 100)>90){
			check_improve(ch,gsn_triwield,true,5);
			two_hit( ch, victim, dt, false );
			if ( ch->fighting != victim ){
				return;
			}
		}else{
			check_improve(ch,gsn_triwield,false,1);
		}
	}

	if (get_eq_char (ch, WEAR_2SECONDARY))
	{
		if(number_range(ch->get_skill(gsn_quadwield), 100)>90){
			check_improve(ch,gsn_quadwield,true,5);
			two_hit( ch, victim, dt, true );
			if ( ch->fighting != victim ){
				return;
			}
		}else{
			check_improve(ch,gsn_quadwield,false,1);
		}
	}
	
	if ( ch->fighting != victim || dt == gsn_backstab )
		return;
	
    chance = get_skill(ch,gsn_second_attack)/2;
	
    if (IS_AFFECTED(ch,AFF_SLOW))
		chance /= 2;
	
	if ( number_percent( ) < chance )
	{
		one_hit( ch, victim, dt, false );
		check_improve(ch,gsn_second_attack,true,5);
		if ( ch->fighting != victim )
			return;
	}
	
	chance = get_skill(ch,gsn_third_attack)/3;
	
	if (IS_AFFECTED(ch,AFF_SLOW))
		chance = 0;
	
	if ( number_percent( ) < chance )
	{
		one_hit( ch, victim, dt, false );
		check_improve(ch,gsn_third_attack,true,6);
		if ( ch->fighting != victim )
			return;
	}
	
	chance = get_skill(ch,gsn_quad_attack)/2;
	
	if (IS_AFFECTED(ch,AFF_SLOW))
		chance = 0;
	
	if ( number_percent( ) < chance )
	{
		one_hit( ch, victim, dt, false );
		check_improve(ch,gsn_quad_attack,true,7);
		if ( ch->fighting != victim )
			return;
	}
	return;
}

/**************************************************************************/
// procedure for all mobile attacks 
void mob_hit (char_data *ch, char_data *victim, int dt)
{
    int chance,number;
    char_data *vch, *vch_next;
	
	one_hit(ch,victim,dt,false);
	
    if (ch->fighting != victim)
		return;
	
	// Area attack -- BALLS nasty! 
	
	// mobs get the extra attack too if defending mount
	if (victim->fighting==ch->mounted_on)
		one_hit(ch,victim,dt,false);
    
    if (IS_SET(ch->off_flags,OFF_AREA_ATTACK))
    {
		for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
		{
			vch_next = vch->next_in_room;
			if ((vch != victim && vch->fighting == ch))
				one_hit(ch,vch,dt,false);
		}
	}
	
	if (IS_AFFECTED(ch,AFF_HASTE)
		||  (IS_SET(ch->off_flags,OFF_FAST) && !IS_AFFECTED(ch,AFF_SLOW)))
		one_hit(ch,victim,dt,false);
	
    if (ch->fighting != victim || dt == gsn_backstab)
		return;
	
	chance = get_skill(ch,gsn_second_attack)/2;
	
    if (IS_AFFECTED(ch,AFF_SLOW) && !IS_SET(ch->off_flags,OFF_FAST))
		chance /= 2;
	
    if (number_percent() < chance)
	{
		one_hit(ch,victim,dt,false);
		if (ch->fighting != victim)
			return;
    }
	
	chance = get_skill(ch,gsn_third_attack)/4;
	
    if (IS_AFFECTED(ch,AFF_SLOW) && !IS_SET(ch->off_flags,OFF_FAST))
		chance = 0;
	
    if (number_percent() < chance)
    {
		one_hit(ch,victim,dt,false);
		if (ch->fighting != victim)
			return;
	}
	
	// oh boy!  Fun stuff!	
    if (ch->wait > 0)
		return;
	
    number = number_range(0,2);
	
    if (number == 1 && IS_SET(ch->act,ACT_MAGE)){
		spec_cast_mage(ch); 
		return;
    }
	
    if (number == 2 && IS_SET(ch->act,ACT_CLERIC)){   
		spec_cast_cleric(ch); 
		return;
    }
	
    // now for the skills
	number = number_range(0,10);
	
    switch(number) 
    {
    case (0) :
		if (IS_SET(ch->off_flags,OFF_BASH))
			do_bash(ch,"");
		break;
		
	case (1) :
		if (IS_SET(ch->off_flags,OFF_BERSERK) && !IS_AFFECTED(ch,AFF_BERSERK))
			do_berserk(ch,"");
		break;
		
		
    case (2) :
		if (IS_SET(ch->off_flags,OFF_DISARM) 
			|| (get_weapon_sn(ch) != gsn_hand_to_hand
			&& (IS_SET(ch->act,ACT_WARRIOR)
			||  IS_SET(ch->act,ACT_THIEF))))
			do_disarm(ch,"");
		break;
		
	case (3) :
		if (IS_SET(ch->off_flags,OFF_KICK))
			do_kick(ch,"");
		break;
		
    case (4) :
		if (IS_SET(ch->off_flags,OFF_KICK_DIRT))
			do_dirt(ch,"");
		break;
		
	case (5) :
		if (IS_SET(ch->off_flags,OFF_CIRCLE))
		{
			 do_circle(ch,"");
		}
		break; 
		
    case (6) :
		if (IS_SET(ch->off_flags,OFF_TRIP))
			do_trip(ch,"");
		break;
		
    case (7) :
		if (IS_SET(ch->off_flags,OFF_CRUSH))
		{
			/* do_crush(ch,"") */ ;
		}
		break;
    case (8) :
		if (IS_SET(ch->off_flags,OFF_BACKSTAB))
		{
			do_pbackstab(ch,"");
		}
    case (9) :
        if ( IS_SET( ch->off_flags, OFF_GORE ))
		{
			do_gore( ch, "" );
        break;
    }
    case (10) :
		if (IS_SET(ch->off_flags,OFF_GAZE))
		{
			do_gaze(ch,"");
		}
		break;
    }
}
	

/**************************************************************************/
// Hit one guy once.
void one_hit( char_data *ch, char_data *victim, int dt, bool second )
{
	OBJ_DATA *wield;
	int victim_ac;
	int thac0;
	int thac0_00;
	int thac0_32;
	int dam;
	int diceroll;
	int sn,skill;
	int dam_type;
	int result;
	int temp_hps;
	
	sn = -1;
	
	// just in case 
	if (victim == ch 
		|| ch == NULL		|| !IS_VALID(ch) 
		|| victim == NULL	|| !IS_VALID(victim))
	{
		return;
	}
	
	// Can't beat a dead char! and guard against weird room-leavings.
	if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
		return;

	OBJ_DATA *primary_weapon= get_eq_char( ch, WEAR_WIELD );
	OBJ_DATA *secondary_weapon= get_eq_char( ch, WEAR_SECONDARY);
	
	// Figure out the type of damage message, first find the weapon
	wield = second?secondary_weapon:primary_weapon;
	
	if ( dt == TYPE_UNDEFINED )
	{
		dt = TYPE_HIT;
		if ( wield != NULL && wield->item_type == ITEM_WEAPON ){
			dt += wield->value[3];
		}else{
			dt += ch->dam_type;
		}
	}

	if ( class_table[ch->clss].class_cast_type == CCT_BARD )
	{
     		ch->songp = 0;
     		ch->songn = 0;
	}

	if (dt < TYPE_HIT){
		if (wield != NULL){
			dam_type = attack_table[wield->value[3]].damage;
		}else{
			dam_type = attack_table[ch->dam_type].damage;
		}
	}else{
		dam_type = attack_table[dt - TYPE_HIT].damage;
	}
	
	if (dam_type == -1){
		dam_type = DAM_BASH;
	}
	
	// get the weapon skill 
	if(second){ // swap over the secondary and primary to get sn
		if(primary_weapon){
			primary_weapon->wear_loc = WEAR_SECONDARY;
		}
		secondary_weapon->wear_loc = WEAR_WIELD; 
		sn = get_weapon_sn(ch);
		if(primary_weapon){
			primary_weapon->wear_loc = WEAR_WIELD;
		}
		secondary_weapon->wear_loc = WEAR_SECONDARY;
	}else{
		sn = get_weapon_sn(ch);
	}
	skill = 20 + get_weapon_skill(ch,sn);
	
	if(second){
		if(!GAMESETTING2(GAMESET2_NO_SECOND_SKILL_REQUIRED)){
			skill*=(ch->get_skill(gsn_second)/100);
		}
	}
	
	if ( get_eq_char( ch, WEAR_LODGED_ARM )){
		skill -= 20;
	}

	if ( class_table[victim->clss].class_cast_type != CCT_BARD )
	{
     		victim->songp = 0;
     		victim->songn = 0;
	}
	
	if(HAS_CONFIG(ch,CONFIG_PRACSYS_TESTER)){
		skill+=20;
	}
	
	/*
	* Calculate to-hit-armor-clss-0 versus armor.
	*/
	if ( IS_NPC(ch) )
	{
		thac0_00 = 20;
		thac0_32 = -4;   /* as good as a thief */
		if (IS_SET(ch->act,ACT_WARRIOR))
			thac0_32 = -10;
		else if (IS_SET(ch->act,ACT_THIEF))
			thac0_32 = -4;
		else if (IS_SET(ch->act,ACT_CLERIC))
			thac0_32 = 2;
		else if (IS_SET(ch->act,ACT_MAGE))
			thac0_32 = 6;
	}
	else
	{
		thac0_00 = class_table[ch->clss].thac0_00;
		thac0_32 = class_table[ch->clss].thac0_32;
	}
	thac0  = interpolate( ch->level, thac0_00, thac0_32 );
	
	if (thac0 < 0)
		thac0 = thac0/2;
	
	if (thac0 < -5)
		thac0 = -5 + (thac0 + 5) / 2;
	
	thac0 -= GET_HITROLL(ch) * skill/100;	
	if(!IS_NPC(ch)){
		// increase affect to hitroll for low pc newbies
		if(ch->level<6){ 
			thac0-= (6-ch->level)*2;
		}
	}else{
		// decrease hitroll on low mobs when fighting newbies
		if(ch->level<6 && victim->level<6){ 
			thac0+= (6-victim->level)*2;
		}
	}
	
	// make faerie fire do something
	if(count_affected_by_base_spell( victim, gsn_faerie_fire )>0){
		thac0-=number_range(1,3);
	}
	
	thac0 += 5 * (100 - skill) / 100;
	
	if (dt == gsn_backstab)
		thac0 -= 10 * (100 - get_skill(ch,gsn_backstab));
	
	if (dt == gsn_circle)
		thac0 -= 7 * (100 - get_skill(ch,gsn_circle));
	
	switch(dam_type)
	{
	case(DAM_PIERCE):victim_ac = GET_AC(victim,AC_PIERCE)/10;       break;
	case(DAM_BASH):  victim_ac = GET_AC(victim,AC_BASH)/10;         break;
	case(DAM_SLASH): victim_ac = GET_AC(victim,AC_SLASH)/10;        break;
	default:         victim_ac = GET_AC(victim,AC_EXOTIC)/10;       break;
	}; 
	
	if (victim_ac < -15)
		victim_ac = (victim_ac + 15) / 5 - 15;
	
	if(victim->position==POS_FIGHTING)
		victim_ac -= victim->modifiers[STAT_QU]/5;
	
	if ( !can_see( ch, victim ) )
		victim_ac -= 4;
	
	if ( victim->position < POS_FIGHTING)
		victim_ac += 4;
	
	if (victim->position < POS_RESTING)
		victim_ac += 6;

	if(is_affected( victim, gsn_hold_person))
		victim_ac += 10;

  // Fighting Styles AC Modifiers
 	if( victim->style == STYLE_AGGRESSIVE )
      		victim_ac += 3;
   	else if( victim->style == STYLE_DEFENSIVE )
      		victim_ac -= 2;
   	else if( victim->style == STYLE_EVASIVE )
      		victim_ac -= 3;

		/*
		* The moment of excitement!
	*/
	while ( ( diceroll = number_bits( 5 ) ) >= 20 )
		;


	if ( diceroll == 0
		|| ( diceroll != 19 && diceroll < thac0 - victim_ac + num_enemies(ch)  - 
		num_enemies(victim)))
	{
		/* Miss. */
		damage( ch, victim, 0, dt, dam_type, true );
		tail_chain( );
		return;
	}
	
	/*
	* Hit.
	* Calc damage.
	*/
	if ( IS_NPC(ch) && wield == NULL)
	{
		dam = dice(ch->damage[DICE_NUMBER],ch->damage[DICE_TYPE]);
	}			
	else
	{
		if (sn != -1){
			check_improve(ch,sn,true,5);
		}
		
		if ( wield != NULL )
		{
			dam = dice(wield->value[1] ,wield->value[2]) * skill/100;
			// Send MSP sound message
			{
				char weapon_sound[MIL];
				sprintf(weapon_sound,"%s6.wav", get_weapontype( wield ));
				msp_to_room(MSPT_COMBAT, weapon_sound, 
					0, 
					ch,
					false,
					true);
			}
			
			// if weapon is above their level reduce the damage accordingly
			if (wield->level>ch->level)
			{
				dam=dam * ch->level / wield->level;
			}
			
			// if defending a mount get bunch of extra damage
			if (victim->fighting==ch->mounted_on)
			{
				dam=dam * 11/7;
			}
			
			// add damage bonus/penalty for mountedness:
			// - if riding <80 you get from 1 to 100 percent of original damage 
			// - if riding >=80 you get 100 to 120 percent 
			// - based on riding skill and random factors
			// - if defending your mount, these bonuses/penalties do not apply			
			if ((ch->mounted_on!=NULL)&&(!IS_NPC(ch))
				&&(victim->fighting!=ch->mounted_on))
			{
				if (get_skill(ch, gsn_riding)<80)
				{
					dam *=(((get_skill(ch, gsn_riding)+20) * number_range(10,100))
						/10000);
				}
				else
				{
					dam+=dam*((number_range(1,100))*(get_skill(ch, gsn_riding)-80))/10000;
					check_improve(ch, gsn_riding, true, 10);
				}
			}
			
			// no shield or second weapon = more damage 
			if (!get_eq_char(ch, WEAR_SECONDARY)
				&& !get_eq_char(ch,WEAR_SHIELD))  
				dam = dam * 6/5;
			
			// sharpness! 
			if (IS_WEAPON_STAT(wield,WEAPON_SHARP))
			{
				int percent;
				
				if ((percent = number_percent()) <= (skill / 8))
					dam = 2 * dam + (dam * 2 * percent / 100);
			}else if (IS_WEAPON_STAT(wield,WEAPON_VORPAL)) // can't have both effects
			{
				int percent;
				if((percent=number_percent()) <=(skill/33))
					dam = 6 * dam + (dam*4*percent/100);
			}
			
			if ( IS_GOOD(victim) 
				&& IS_WEAPON_STAT(wield,WEAPON_HOLY)
				&& IS_OBJ_STAT(wield,OBJEXTRA_ANTI_GOOD) 
				&& HAS_CLASSFLAG(ch, CLASSFLAG_DAMMODS_WITH_HOLYWEAPONS))
			{
				dam = dam + (victim->alliance/3)* (dam*5/4);
			}
			
			if ( IS_EVIL(victim) 
				&& IS_WEAPON_STAT(wield,WEAPON_HOLY)
				&& IS_OBJ_STAT(wield,OBJEXTRA_ANTI_EVIL) 
				&& HAS_CLASSFLAG(ch, CLASSFLAG_DAMMODS_WITH_HOLYWEAPONS))
			{
				dam = (sh_int) dam + (abs(victim->alliance)/3)*(dam*5/4);
			}
			
		}else{
			dam = number_range( 1 + 4 * skill/100, 2 * ch->level/3 * skill/100);
		}
	}
	
	// Bonuses
	if ( get_skill(ch,gsn_enhanced_damage) > 0 )
	{
		diceroll = number_percent();
		if (diceroll <= get_skill(ch,gsn_enhanced_damage))
		{
			check_improve(ch,gsn_enhanced_damage,true,6);
			dam += 2 * ( dam * diceroll/300);
		}
	}
	
  if ( wield != NULL && wield->item_type == ITEM_FIREARM )
   {
    	if ( wield->value[1] <= 0 )
    	{
		act( "`#`Y*CLICK* `cThe $p seems to be out of ammo.`^", ch, wield, NULL, TO_CHAR );
		wield->value[1] = 0;
		tail_chain( );
		return;
    	}

	wield->value[1] -= 1;
	if (number_percent() <= get_skill(ch,gsn_firearms))
	{
		act( "$n shoots $N with $p.", ch, wield, victim, TO_NOTVICT );
		act( "You shoot $N with $p.", ch, wield, victim, TO_CHAR );
		act( "$n shoots at you with $p.",ch, wield, victim, TO_VICT );
		obj_cast_spell( wield->value[2], wield->level, ch, victim, NULL );
	}
	else
	{
    		act( "You miss the target.", ch,wield,NULL,TO_CHAR);
    		act( "$n misses with a shot from $p.", ch,wield,NULL,TO_ROOM);
    		check_improve(ch,gsn_firearms,false,2);
	}

	int recoil = 0;
	if (IS_SET(wield->value[4], FIREARM_RECOIL_NONE) && ch->perm_stats[STAT_ST] < 25)
		recoil = 1;
	if (IS_SET(wield->value[4], FIREARM_RECOIL_LIGHT) && ch->perm_stats[STAT_ST] < 25)
		recoil = 2;
	if (IS_SET(wield->value[4], FIREARM_RECOIL_HEAVY) && ch->perm_stats[STAT_ST] < 25)
		recoil = 3;
	if (IS_SET(wield->value[4], FIREARM_RECOIL_EXTREME) && ch->perm_stats[STAT_ST] < 25)
		recoil = 4;
	if (IS_SET(wield->value[4], FIREARM_RECOIL_LIGHT) && ch->perm_stats[STAT_ST] < 45)
		recoil = 1;
	if (IS_SET(wield->value[4], FIREARM_RECOIL_HEAVY) && ch->perm_stats[STAT_ST] < 45)
		recoil = 2;
	if (IS_SET(wield->value[4], FIREARM_RECOIL_EXTREME) && ch->perm_stats[STAT_ST] < 45)
		recoil = 3;
	if (IS_SET(wield->value[4], FIREARM_RECOIL_HEAVY) && ch->perm_stats[STAT_ST] < 75)
		recoil = 1;
	if (IS_SET(wield->value[4], FIREARM_RECOIL_EXTREME) && ch->perm_stats[STAT_ST] < 75)
		recoil = 2;
	if (IS_SET(wield->value[4], FIREARM_RECOIL_EXTREME) && ch->perm_stats[STAT_ST] < 100)
		recoil = 1;

	if (recoil == 1)		
	{
		ch->println ("The recoil slows you down a bit.");
		WAIT_STATE(ch,1 * PULSE_VIOLENCE);
	}
	if (recoil == 2)
	{	
		ch->println ("The recoil sends throws you off balance.");
		WAIT_STATE(ch,2 * PULSE_VIOLENCE);
	}
	if (recoil == 3)
	{
		ch->println ("The strong recoil almost numbs your arm.");
		WAIT_STATE(ch,3 * PULSE_VIOLENCE);
	}
	if (recoil == 4)
	{	
		ch->println ("The recoil almost shatters your hand.");
		WAIT_STATE(ch,4 * PULSE_VIOLENCE);
	}
	
	tail_chain( );
	return;
   }

   if ( wield != NULL && wield->item_type == ITEM_WEAPON )
   {

	if ( strstr(wield->material, "iron") && IS_SET(victim->vuln_flags, VULN_IRON))
	{
		diceroll = number_percent();
		if (diceroll <= 75)
		{
			dam += 2 * ( dam * diceroll/300);
		}
	}

	if ( strstr(wield->material, "wood") && IS_SET(victim->vuln_flags, VULN_WOOD))
	{
		diceroll = number_percent();
		if (diceroll <= 75)
		{
			dam += 2 * ( dam * diceroll/300);
		}
	}

	if ( strstr(wield->material, "silver") && IS_SET(victim->vuln_flags, VULN_SILVER))
	{
		diceroll = number_percent();
		if (diceroll <= 75)
		{
			dam += 2 * ( dam * diceroll/300);
		}
	}
    }
	
    if (IS_AFFECTED2(ch, AFF2_POSSESSION))
    {
        diceroll = number_percent();        
		if (diceroll <= 50){
			dam += 2 * (dam * diceroll / 300);
		}
    }


    	if ( !IS_NPC(ch) && IS_VAMPAFF(ch, VAM_POTENCE) )
    	{
	   int ammount = (get_age(ch)/100);
	   if ( IS_VAMPAFF(ch, VAM_POTENCE) && ammount >= 7 )
		  ammount = 6;   
	   else if ( ammount >= 6 )
		  ammount = 5;
	   ammount += ( 13 - ch->vampgen);
	   dam += ammount;
    	}
    
    	/* Vampires should be tougher at night and weaker during the day. */
    	if ( IS_VAMPIRE(ch) )
    	{
	   if (time_info.hour > 6 && time_info.hour < 19) /* 6 19 */
             switch ( ch->in_room->sector_type ) 
	     {
		case (SECT_FIELD):
		case (SECT_CITY):
		case (SECT_FOREST):
		case (SECT_HILLS):
		case (SECT_MOUNTAIN):
		case (SECT_WATER_SWIM):
		case (SECT_DESERT):
		case (SECT_UNDERWATER):
		case (SECT_WATER_NOSWIM):
		case (SECT_SWAMP):
		case (SECT_AIR):
		case (SECT_ICE):
		case (SECT_SNOW):
		case (SECT_LAVA):
		case (SECT_TRAIL):
			if( !IS_SET(ch->in_room->room_flags, ROOM_DARK) ||
		    	    !IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
			{
				dam /= 2;
			}
	     }	     
    	}

	if ( get_skill(ch,gsn_ultra_damage) > 0 )
	{
		diceroll = number_percent();
		if (diceroll <= get_skill(ch,gsn_ultra_damage))
		{
			check_improve(ch,gsn_ultra_damage,true,7);
			dam += 3/2 * ( dam * diceroll/300);
		}
	}

   /*
    * Calculate Damage Modifiers from Victim's Fighting Style
    */
   if( victim->style == STYLE_AGGRESSIVE )
      dam = dam * (4 / 1); // 1.1
   else if( victim->style == STYLE_DEFENSIVE )
      dam = dam * (6 / 7); // .85
   else if( victim->style == STYLE_EVASIVE )
      dam = dam * (4 / 5); // .8

   /*
    * Calculate Damage Modifiers from Attacker's Fighting Style
    */
   if( ch->style == STYLE_AGGRESSIVE )
      dam = dam * (4 / 1); // 1.1
   else if( ch->style == STYLE_DEFENSIVE )
      dam = dam * (6 / 7);
   else if( ch->style == STYLE_EVASIVE )
      dam = dam * (4 / 5);
	
	if ( !IS_AWAKE(victim) ){
		dam *= 2;
	}else{
		if (victim->position < POS_FIGHTING){
			dam = dam * 3 / 2;
		}
	}
	
	if ( dt == gsn_backstab && wield != NULL){
		if ( wield->value[0] != 2 ){
			dam *= 2 + (ch->level / 10);
		}else {
			dam *= 2 + (ch->level / 8);
		}
	}

    if ( dt == gsn_circle && wield != NULL){
		if ( wield->value[0] != 2 ){	     
			dam *= 2 + (ch->level / 16);
		}else{
			dam *= 2 + (ch->level / 11);
		}
	}
	
	dam += GET_DAMROLL(ch) * UMIN(100,skill) /100;
	
	if ( dam <= 0 )
		dam = 1;
	
	
	result = damage( ch, victim, dam, dt, dam_type, true );
	
	// but do we have a funky weapon? 
	if (result && wield != NULL)
	{
		int dam;
		int dr; // damage result
		
		if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_POISON))
		{
			int level;
			AFFECT_DATA *poison, af;
			
			poison = affect_find(wield->affected,gsn_poison);
			if(poison){
				level = poison->level;
			}else{
				level = wield->level;
			}
			
			if (!saves_spell(level / 2,victim,DAM_POISON) 
				&& !HAS_CLASSFLAG(victim, CLASSFLAG_POISON_IMMUNITY)) 
			{
				victim->println("You feel poison coursing through your veins.");
				act("$n is poisoned by the venom on $p.",
					victim,wield,NULL,TO_ROOM);
				
				af.where     = WHERE_AFFECTS;
				af.type      = gsn_poison;
				af.level     = level * 3/4;
				af.duration  = level / 2;
				af.location  = APPLY_ST;
				af.modifier  = -1;
				af.bitvector = AFF_POISON;
				affect_join( victim, &af );
			}
			
			// weaken the poison if it's temporary
			if(poison){
				poison->level = UMAX(0,poison->level - 2);
				poison->duration = UMAX(0,poison->duration - 1);
				
				if (poison->level == 0 || poison->duration == 0){
					act("The poison on $p has worn off.",ch,wield,NULL,TO_CHAR);
				}
			}
		}
		
		// vampiric weapon effect
		if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_VAMPIRIC))
		{
			dam = number_range(1, wield->level / 10 + 1);

			temp_hps=victim->hit;
			damage(ch,victim,dam,0,DAM_NEGATIVE,false);
			if(victim->hit<-10){
				victim->hit=-10;
			}
			dr=(temp_hps-victim->hit)/2;
			ch->hit += dr;
			act("$p draws life from $N.",ch,wield,victim,TO_NOTVICT);
			act_with_autodam_to_char("$p draws life from $N.",ch,wield,victim,dr);
			dr=temp_hps-victim->hit;
			act_with_autodam_to_char("You feel $p drawing your life away.",victim,wield,NULL,dr);
		}
		
		// flaming weapon effect
		if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_FLAMING))
		{
			dam = number_range(1,wield->level / 4 + 1);
			dr=damage(ch,victim,dam,0,DAM_FIRE,false);				
			act("$N is burned by $p.",ch,wield,victim,TO_NOTVICT);
			act_with_autodam_to_char("$N is burned by $p.",ch,wield,victim,dr);
			act_with_autodam_to_char("$p sears your flesh.",victim,wield,NULL,dr);			
			fire_effect( (void *) victim,wield->level/2,dam,TARGET_CHAR);			
		}
		
		// frost weapon effect
		if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_FROST))
		{
			dam = number_range(1,wield->level / 6 + 2);
			dr=damage(ch,victim,dam,0,DAM_COLD,false);
			act("$p freezes $N.",ch,wield,victim,TO_NOTVICT);
			act_with_autodam_to_char("$p freezes $N.",ch,wield,victim,dr);
			act_with_autodam_to_char("The cold touch of $p surrounds you with ice.",
				victim,wield,NULL,dr);			
			cold_effect(victim,wield->level/2,dam,TARGET_CHAR);			
		}

		// suckle weapon effect - takes mana
		if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_SUCKLE))
        {
            dam = number_range(1, wield->level / 5 + 1);
            act("$p draws mystical energy from $n.",victim,wield,NULL,TO_ROOM);
            act("You feel $p drawing your mystical energy away.",
                victim,wield,NULL,TO_CHAR);
            victim->mana -= dam;
            ch->mana += dam/2;
        } 

		// enervated weapon effect - takes movements
        if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_ENERVATE))
        {
            dam = number_range(1, wield->level / 5 + 1);
            act("$p saps the strength from $n.",victim,wield,NULL,TO_ROOM);
            act("You feel $p sapping your strength away.",
                victim,wield,NULL,TO_CHAR);
            victim->move-= dam;
            ch->move += dam/2;
        }

	if (ch->fighting == victim && (wield->pIndexData->vnum == 5093))
	{
		int rot_chance;
		rot_chance = number_range(1,20);
		if (rot_chance>=18 &&
		   !IS_SET(victim->act,ACT_UNDEAD) &&
		   !IS_SET(victim->imm_flags,IMM_DISEASE) &&
		   !HAS_CLASSFLAG(victim, CLASSFLAG_PLAGUE_IMMUNITY))
		{
			AFFECT_DATA af;
			victim->printf( "You scream in `#`?a`?g`?o`?n`?y`^ as your skin begins to rot." );
			act("$n screams in `#`?a`?g`?o`?n`?y`^ as $s skin begins to rot.",victim,NULL,NULL,TO_ROOM);
			af.where     = WHERE_AFFECTS;
			af.type      = gsn_plague;
			af.level     = 100;
			af.duration  = 75;
			af.location  = APPLY_ST;
			af.modifier  = -15;
			af.bitvector = AFF_PLAGUE;
			affect_join( victim, &af );
		}
		if (rot_chance == 20)
		{
			OBJ_DATA *decay, *decay_next;
			int rotrandom = dice(2,10)-1;
			for ( decay = victim->carrying; decay != NULL; decay = decay_next)
			{
				decay_next = decay->next_content;
				if (decay->wear_loc == rotrandom)
					break;
			}
			if (decay!=NULL && !IS_SET( decay->extra2_flags, OBJEXTRA2_NODECAY ))
			{
				decay->timer = 10+rotrandom;
				act( "$p slowly starts to break apart.", victim, decay, NULL, TO_ROOM );
				act( "$p slowly starts to break apart as decay sets in.", victim, decay, NULL, TO_CHAR );
			}
		}
	}

	if (ch->fighting == victim && (wield->pIndexData->vnum == 5094))
	{
		if(victim->clss == class_lookup("mage") ||
		   victim->clss == class_lookup("spellfilcher"))
		{
			if (!IS_NPC(victim))
			{
				act("$n is drained by the power of $p.",victim,wield,NULL,TO_ROOM);
				act("You are drained by the power of $p.",victim,wield,NULL,TO_CHAR);
				int mana_drain=0;
				mana_drain = number_range(50,150);
				victim->mana -= mana_drain;
				if (victim->mana < 0) 
				{
					victim->mana = 0;
				}
			}

			int dispel_chance;
			dispel_chance = number_range(1,20);

			if(IS_NPC(victim) && !IS_SET(victim->act, ACT_MAGE))
			{
				dispel_chance = 0;
			}
		
			if (dispel_chance == 20)
			{

				act("$n is dispelled by $p.",victim,wield,NULL,TO_ROOM);
				act("You are dispelled by $p.",victim,wield,NULL,TO_CHAR);
				affect_parentspellfunc_strip(victim,gsn_regeneration);
				affect_parentspellfunc_strip(victim,gsn_resist_poison);
				affect_parentspellfunc_strip(victim,gsn_poison_immunity);
				affect_parentspellfunc_strip(victim,gsn_illusions_grandeur);
				affect_parentspellfunc_strip(victim,gsn_protection_fire);
				affect_parentspellfunc_strip(victim,gsn_protection_cold);
				affect_parentspellfunc_strip(victim,gsn_protection_acid);
				affect_parentspellfunc_strip(victim,gsn_protection_lightning);
				affect_parentspellfunc_strip(victim,gsn_holy_aura);
				affect_parentspellfunc_strip(victim,gsn_unholy_aura);
				affect_parentspellfunc_strip(victim,gsn_true_sight);
				affect_parentspellfunc_strip(victim,gsn_barkskin);
				affect_parentspellfunc_strip(victim,gsn_magic_resistance);
				affect_parentspellfunc_strip(victim,gsn_fire_shield);
				affect_parentspellfunc_strip(victim,gsn_chill_shield);
				affect_parentspellfunc_strip(victim,gsn_animal_essence);
				affect_parentspellfunc_strip(victim,gsn_armor);
				affect_parentspellfunc_strip(victim,gsn_bless);
				affect_parentspellfunc_strip(victim,gsn_detect_evil);
				affect_parentspellfunc_strip(victim,gsn_detect_scry);
				affect_parentspellfunc_strip(victim,gsn_detect_good);
				affect_parentspellfunc_strip(victim,gsn_detect_hidden);
				affect_parentspellfunc_strip(victim,gsn_detect_invis);
				affect_parentspellfunc_strip(victim,gsn_fly);
				affect_parentspellfunc_strip(victim,gsn_frenzy);
				affect_parentspellfunc_strip(victim,gsn_giant_strength);
				affect_parentspellfunc_strip(victim,gsn_haste);
				affect_parentspellfunc_strip(victim,gsn_invisibility);
				affect_parentspellfunc_strip(victim,gsn_mass_invis);
				affect_parentspellfunc_strip(victim,gsn_rage);
				affect_parentspellfunc_strip(victim,gsn_protection_good);
				affect_parentspellfunc_strip(victim,gsn_protection_evil);
				affect_parentspellfunc_strip(victim,gsn_pass_door);
				if (IS_AFFECTED(victim,AFF_SANCTUARY) && !is_affected(victim, gsn_sanctuary ))
				{
					REMOVE_BIT(victim->affected_by,AFF_SANCTUARY);
					act("The white aura around $n's body vanishes.",victim,NULL,NULL,TO_ROOM);
				}
				else
				{
					affect_parentspellfunc_strip(victim,gsn_sanctuary);
				}

			}
		}
	}
		// annealed weapon effect
        if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_ANNEALED))
        {
            dam = number_range(1,wield->level/5 + 2);
            dr=damage(ch,victim,dam,0,DAM_BASH,false);
            if (!saves_spell(wield->level/4 + dam/20,victim,DAM_BASH))
         	{
            	DAZE_STATE(victim,UMAX(12,wield->level/4 + dam/20));
            }
         	victim->printlnf("You see stars as your head `Crings`x!%s",
            autodamtext(victim, dr));
         	ch->printlnf("Your opponent looks `Bstunned`x!%s",
            autodamtext(ch, dr));
        }

		// shocking weapon effect
		if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_SHOCKING))
		{			
			dam = number_range(1,wield->level/5 + 2);
			dr=damage(ch,victim,dam,0,DAM_LIGHTNING,false);
			act("$N is struck by lightning from $p.",ch,wield,victim,TO_NOTVICT);
			act_with_autodam_to_char("$N is struck by lightning from $p.",ch,wield,victim,dr);
			act_with_autodam_to_char("You are shocked by $p",victim,wield,NULL,dr);			
			shock_effect(victim,wield->level/2,dam,TARGET_CHAR);
		}
	}
	tail_chain( );
	return;
}
/**************************************************************************/
// Hit one guy once - two hit is for multi-limbed PC's
void two_hit( char_data *ch, char_data *victim, int dt, bool second )
{
	OBJ_DATA *wield;
	int victim_ac;
	int thac0;
	int thac0_00;
	int thac0_32;
	int dam;
	int diceroll;
	int sn,skill;
	int dam_type;
	int result;
	
	sn = -1;
	
	// just in case 
	if (victim == ch 
		|| ch == NULL		|| !IS_VALID(ch) 
		|| victim == NULL	|| !IS_VALID(victim))
	{
		return;
	}
	
	// Can't beat a dead char! and guard against weird room-leavings.
	if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
		return;

	OBJ_DATA *primary_weapon= get_eq_char( ch, WEAR_2WIELD );
	OBJ_DATA *secondary_weapon= get_eq_char( ch, WEAR_2SECONDARY);

	// Figure out the type of damage message, first find the weapon
	wield = second?secondary_weapon:primary_weapon;
	
	if ( dt == TYPE_UNDEFINED )
	{
		dt = TYPE_HIT;
		if ( wield != NULL && wield->item_type == ITEM_WEAPON ){
			dt += wield->value[3];
		}else{
			dt += ch->dam_type;
		}
	}
	
	if (dt < TYPE_HIT){
		if (wield != NULL){
			dam_type = attack_table[wield->value[3]].damage;
		}else{
			dam_type = attack_table[ch->dam_type].damage;
		}
	}else{
		dam_type = attack_table[dt - TYPE_HIT].damage;
	}
	
	if (dam_type == -1){
		dam_type = DAM_BASH;
	}
	
	// get the weapon skill 
	if(second){ // swap over the secondary and primary to get sn
		if(primary_weapon){
			primary_weapon->wear_loc = WEAR_2SECONDARY;
		}
		secondary_weapon->wear_loc = WEAR_2WIELD; 
		sn = get_weapon_sn(ch);
		if(primary_weapon){
			primary_weapon->wear_loc = WEAR_2WIELD;
		}
		secondary_weapon->wear_loc = WEAR_2SECONDARY;
	}else{
		sn = get_weapon_sn(ch);
	}
	skill = 20 + get_weapon_skill(ch,sn);
	
	if(second)
	{
		skill*=(ch->get_skill(gsn_quadwield)/100);
	}
	
	if ( get_eq_char( ch, WEAR_LODGED_ARM )){
		skill -= 20;
	}
	
	/*
	* Calculate to-hit-armor-clss-0 versus armor.
	*/
	if ( IS_NPC(ch) )
	{
		thac0_00 = 20;
		thac0_32 = -4;   /* as good as a thief */
		if (IS_SET(ch->act,ACT_WARRIOR))
			thac0_32 = -10;
		else if (IS_SET(ch->act,ACT_THIEF))
			thac0_32 = -4;
		else if (IS_SET(ch->act,ACT_CLERIC))
			thac0_32 = 2;
		else if (IS_SET(ch->act,ACT_MAGE))
			thac0_32 = 6;
	}
	else
	{
		thac0_00 = class_table[ch->clss].thac0_00;
		thac0_32 = class_table[ch->clss].thac0_32;
	}
	thac0  = interpolate( ch->level, thac0_00, thac0_32 );
	
	if (thac0 < 0)
		thac0 = thac0/2;
	
	if (thac0 < -5)
		thac0 = -5 + (thac0 + 5) / 2;
	
	thac0 -= GET_HITROLL(ch) * skill/100;	
	if(!IS_NPC(ch)){
		// increase affect to hitroll for low pc newbies
		if(ch->level<6){ 
			thac0-= (6-ch->level)*2;
		}
	}else{
		// decrease hitroll on low mobs when fighting newbies
		if(ch->level<6 && victim->level<6){ 
			thac0+= (6-victim->level)*2;
		}
	}
	
	// make faerie fire do something
	if(count_affected_by_base_spell( victim, gsn_faerie_fire )>0){
		thac0-=number_range(1,3);
	}
	
	thac0 += 5 * (100 - skill) / 100;
	
	if (dt == gsn_backstab)
		thac0 -= 10 * (100 - get_skill(ch,gsn_backstab));
	
	if (dt == gsn_circle)
		thac0 -= 7 * (100 - get_skill(ch,gsn_circle));
	
	switch(dam_type)
	{
	case(DAM_PIERCE):victim_ac = GET_AC(victim,AC_PIERCE)/10;       break;
	case(DAM_BASH):  victim_ac = GET_AC(victim,AC_BASH)/10;         break;
	case(DAM_SLASH): victim_ac = GET_AC(victim,AC_SLASH)/10;        break;
	default:         victim_ac = GET_AC(victim,AC_EXOTIC)/10;       break;
	}; 
	
	if (victim_ac < -15)
		victim_ac = (victim_ac + 15) / 5 - 15;
	
	if(victim->position==POS_FIGHTING)
		victim_ac -= victim->modifiers[STAT_QU]/5;
	
	if ( !can_see( ch, victim ) )
		victim_ac -= 4;
	
	if ( victim->position < POS_FIGHTING)
		victim_ac += 4;
	
	if (victim->position < POS_RESTING)
		victim_ac += 6;

	if(is_affected( victim, gsn_hold_person))
		victim_ac += 10;
	
		/*
		* The moment of excitement!
	*/
	while ( ( diceroll = number_bits( 5 ) ) >= 20 )
		;
	
	if ( diceroll == 0
		|| ( diceroll != 19 && diceroll < thac0 - victim_ac + num_enemies(ch)  - 
		num_enemies(victim)))
	{
		/* Miss. */
		damage( ch, victim, 0, dt, dam_type, true );
		tail_chain( );
		return;
	}
	
	/*
	* Hit.
	* Calc damage.
	*/
	if ( IS_NPC(ch) && wield == NULL)
	{
		dam = dice(ch->damage[DICE_NUMBER],ch->damage[DICE_TYPE]);
	}			
	else
	{
		if (sn != -1){
			check_improve(ch,sn,true,5);
		}
		
		if ( wield != NULL )
		{
			dam = dice(wield->value[1] ,wield->value[2]) * skill/100;
			// Send MSP sound message
			{
				char weapon_sound[MIL];
				sprintf(weapon_sound,"%s6.wav", get_weapontype( wield ));
				msp_to_room(MSPT_COMBAT, weapon_sound, 
					0, 
					ch,
					false,
					true);
			}
			
			// if weapon is above their level reduce the damage accordingly
			if (wield->level>ch->level)
			{
				dam=dam * ch->level / wield->level;
			}
			
			// if defending a mount get bunch of extra damage
			if (victim->fighting==ch->mounted_on)
			{
				dam=dam * 11/7;
			}
			
			// add damage bonus/penalty for mountedness:
			// - if riding <80 you get from 1 to 100 percent of original damage 
			// - if riding >=80 you get 100 to 120 percent 
			// - based on riding skill and random factors
			// - if defending your mount, these bonuses/penalties do not apply			
			if ((ch->mounted_on!=NULL)&&(!IS_NPC(ch))
				&&(victim->fighting!=ch->mounted_on))
			{
				if (get_skill(ch, gsn_riding)<80)
				{
					dam *=(((get_skill(ch, gsn_riding)+20) * number_range(10,100))
						/10000);
				}
				else
				{
					dam+=dam*((number_range(1,100))*(get_skill(ch, gsn_riding)-80))/10000;
					check_improve(ch, gsn_riding, true, 10);
				}
			}
			
			// no shield or second weapon = more damage 
			if (!get_eq_char(ch, WEAR_2SECONDARY)
				&& !get_eq_char(ch,WEAR_SHIELD))  
				dam = dam * 6/5;
			
			// sharpness! 
			if (IS_WEAPON_STAT(wield,WEAPON_SHARP))
			{
				int percent;
				
				if ((percent = number_percent()) <= (skill / 8))
					dam = 2 * dam + (dam * 2 * percent / 100);
			}else if (IS_WEAPON_STAT(wield,WEAPON_VORPAL)) // can't have both effects
			{
				int percent;
				if((percent=number_percent()) <=(skill/33))
					dam = 6 * dam + (dam*4*percent/100);
			}
			
			if ( IS_GOOD(victim) 
				&& IS_WEAPON_STAT(wield,WEAPON_HOLY)
				&& IS_OBJ_STAT(wield,OBJEXTRA_ANTI_GOOD) 
				&& HAS_CLASSFLAG(ch, CLASSFLAG_DAMMODS_WITH_HOLYWEAPONS))
			{
				dam = dam + (victim->alliance/3)* (dam*5/4);
			}
			
			if ( IS_EVIL(victim) 
				&& IS_WEAPON_STAT(wield,WEAPON_HOLY)
				&& IS_OBJ_STAT(wield,OBJEXTRA_ANTI_EVIL) 
				&& HAS_CLASSFLAG(ch, CLASSFLAG_DAMMODS_WITH_HOLYWEAPONS))
			{
				dam = (sh_int) dam + (abs(victim->alliance)/3)*(dam*5/4);
			}
			
		}else{
			dam = number_range( 1 + 4 * skill/100, 2 * ch->level/3 * skill/100);
		}
	}
	
	// Bonuses
	if ( get_skill(ch,gsn_enhanced_damage) > 0 )
	{
		diceroll = number_percent();
		if (diceroll <= get_skill(ch,gsn_enhanced_damage))
		{
			check_improve(ch,gsn_enhanced_damage,true,6);
			dam += 2 * ( dam * diceroll/300);
		}
	}

   if ( wield != NULL && wield->item_type == ITEM_WEAPON )
   {

	if ( strstr(wield->material, "iron") && IS_SET(victim->vuln_flags, VULN_IRON))
	{
		diceroll = number_percent();
		if (diceroll <= 75)
		{
			dam += 2 * ( dam * diceroll/300);
		}
	}

	if ( strstr(wield->material, "wood") && IS_SET(victim->vuln_flags, VULN_WOOD))
	{
		diceroll = number_percent();
		if (diceroll <= 75)
		{
			dam += 2 * ( dam * diceroll/300);
		}
	}

	if ( strstr(wield->material, "silver") && IS_SET(victim->vuln_flags, VULN_SILVER))
	{
		diceroll = number_percent();
		if (diceroll <= 75)
		{
			dam += 2 * ( dam * diceroll/300);
		}
	}
    }
	
    if (IS_AFFECTED2(ch, AFF2_POSSESSION))
    {
        diceroll = number_percent();        
		if (diceroll <= 50){
			dam += 2 * (dam * diceroll / 300);
		}
    }
	
	if ( get_skill(ch,gsn_ultra_damage) > 0 )
	{
		diceroll = number_percent();
		if (diceroll <= get_skill(ch,gsn_ultra_damage))
		{
			check_improve(ch,gsn_ultra_damage,true,7);
			dam += 3/2 * ( dam * diceroll/300);
		}
	}
	
	if ( !IS_AWAKE(victim) ){
		dam *= 2;
	}else{
		if (victim->position < POS_FIGHTING){
			dam = dam * 3 / 2;
		}
	}
	
	if ( dt == gsn_backstab && wield != NULL){
		if ( wield->value[0] != 2 ){
			dam *= 2 + (ch->level / 10);
		}else {
			dam *= 2 + (ch->level / 8);
		}
	}

    if ( dt == gsn_circle && wield != NULL){
		if ( wield->value[0] != 2 ){	     
			dam *= 2 + (ch->level / 16);
		}else{
			dam *= 2 + (ch->level / 11);
		}
	}
	
	dam += GET_DAMROLL(ch) * UMIN(100,skill) /100;
	
	if ( dam <= 0 )
		dam = 1;

	result = damage( ch, victim, dam, dt, dam_type, true );
	// but do we have a funky weapon? 
	if (result < 0 && wield != NULL)
	{
		weapon_effects(ch, victim, wield);
	if ( class_table[ch->clss].class_cast_type == CCT_BARD )
	{
     		ch->songp = 0;
     		ch->songn = 0;
	}
	if ( class_table[victim->clss].class_cast_type == CCT_BARD )
	{
     		victim->songp = 0;
     		victim->songn = 0;
	}

	if (ch->fighting == victim && (wield->pIndexData->vnum == 5094))
	{
		if(victim->clss == class_lookup("mage") ||
		   victim->clss == class_lookup("spellfilcher"))
		{
			if (!IS_NPC(victim))
			{
				act("$n is drained by the power of $p.",victim,wield,NULL,TO_ROOM);
				act("You are drained by the power of $p.",victim,wield,NULL,TO_CHAR);
				int mana_drain=0;
				mana_drain = number_range(50,200);
				victim->mana -= mana_drain;
				if (victim->mana > 0) 
				{
					victim->mana = 0;
				}
			}

			int dispel_chance;
			dispel_chance = number_range(1,20);

			if(IS_NPC(victim) && !IS_SET(victim->act, ACT_MAGE))
			{
				dispel_chance = 0;
			}
		
			if (dispel_chance == 20)
			{

				act("$n is dispelled by $p.",victim,wield,NULL,TO_ROOM);
				act("You are dispelled by $p.",victim,wield,NULL,TO_CHAR);
				affect_parentspellfunc_strip(victim,gsn_regeneration);
				affect_parentspellfunc_strip(victim,gsn_resist_poison);
				affect_parentspellfunc_strip(victim,gsn_poison_immunity);
				affect_parentspellfunc_strip(victim,gsn_illusions_grandeur);
				affect_parentspellfunc_strip(victim,gsn_protection_fire);
				affect_parentspellfunc_strip(victim,gsn_protection_cold);
				affect_parentspellfunc_strip(victim,gsn_protection_acid);
				affect_parentspellfunc_strip(victim,gsn_protection_lightning);
				affect_parentspellfunc_strip(victim,gsn_holy_aura);
				affect_parentspellfunc_strip(victim,gsn_unholy_aura);
				affect_parentspellfunc_strip(victim,gsn_true_sight);
				affect_parentspellfunc_strip(victim,gsn_barkskin);
				affect_parentspellfunc_strip(victim,gsn_magic_resistance);
				affect_parentspellfunc_strip(victim,gsn_fire_shield);
				affect_parentspellfunc_strip(victim,gsn_chill_shield);
				affect_parentspellfunc_strip(victim,gsn_animal_essence);
				affect_parentspellfunc_strip(victim,gsn_armor);
				affect_parentspellfunc_strip(victim,gsn_bless);
				affect_parentspellfunc_strip(victim,gsn_detect_evil);
				affect_parentspellfunc_strip(victim,gsn_detect_scry);
				affect_parentspellfunc_strip(victim,gsn_detect_good);
				affect_parentspellfunc_strip(victim,gsn_detect_hidden);
				affect_parentspellfunc_strip(victim,gsn_detect_invis);
				affect_parentspellfunc_strip(victim,gsn_fly);
				affect_parentspellfunc_strip(victim,gsn_frenzy);
				affect_parentspellfunc_strip(victim,gsn_giant_strength);
				affect_parentspellfunc_strip(victim,gsn_haste);
				affect_parentspellfunc_strip(victim,gsn_invisibility);
				affect_parentspellfunc_strip(victim,gsn_mass_invis);
				affect_parentspellfunc_strip(victim,gsn_rage);
				affect_parentspellfunc_strip(victim,gsn_protection_good);
				affect_parentspellfunc_strip(victim,gsn_protection_evil);
				affect_parentspellfunc_strip(victim,gsn_pass_door);
				if (IS_AFFECTED(victim,AFF_SANCTUARY) && !is_affected(victim, gsn_sanctuary ))
				{
					REMOVE_BIT(victim->affected_by,AFF_SANCTUARY);
					act("The white aura around $n's body vanishes.",victim,NULL,NULL,TO_ROOM);
				}
				else
				{
					affect_parentspellfunc_strip(victim,gsn_sanctuary);
				}

			}
		}
	}

	}
	tail_chain( );
	return;
}

void weapon_effects( char_data *ch, char_data *victim, obj_data *wield)
{
		int dam;
		int dr;
		if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_POISON))
		{
			int level;
			AFFECT_DATA *poison, af;
			
			poison = affect_find(wield->affected,gsn_poison);
			if(poison){
				level = poison->level;
			}else{
				level = wield->level;
			}
			
			if (!saves_spell(level / 2,victim,DAM_POISON) 
				&& !HAS_CLASSFLAG(victim, CLASSFLAG_POISON_IMMUNITY)) 
			{
				victim->println("You feel poison coursing through your veins.");
				act("$n is poisoned by the venom on $p.",
					victim,wield,NULL,TO_ROOM);
				
				af.where     = WHERE_AFFECTS;
				af.type      = gsn_poison;
				af.level     = level * 3/4;
				af.duration  = level / 2;
				af.location  = APPLY_ST;
				af.modifier  = -1;
				af.bitvector = AFF_POISON;
				affect_join( victim, &af );
			}
			
			// weaken the poison if it's temporary
			if(poison){
				poison->level = UMAX(0,poison->level - 2);
				poison->duration = UMAX(0,poison->duration - 1);
				
				if (poison->level == 0 || poison->duration == 0){
					act("The poison on $p has worn off.",ch,wield,NULL,TO_CHAR);
				}
			}
		}
		
		// vampiric weapon effect
		if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_VAMPIRIC))
		{
			dam = number_range(1, wield->level / 10 + 1);

			int temp_hps=victim->hit;
			damage(ch,victim,dam,0,DAM_NEGATIVE,false);
			if(victim->hit<-10){
				victim->hit=-10;
			}
			dr=(temp_hps-victim->hit)/2;
			ch->hit += dr;
			act("$p draws life from $N.",ch,wield,victim,TO_NOTVICT);
			act_with_autodam_to_char("$p draws life from $N.",ch,wield,victim,dr);
			dr=temp_hps-victim->hit;
			act_with_autodam_to_char("You feel $p drawing your life away.",victim,wield,NULL,dr);
		}
		
		// flaming weapon effect
		if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_FLAMING))
		{
			dam = number_range(1,wield->level / 4 + 1);
			dr=damage(ch,victim,dam,0,DAM_FIRE,false);				
			act("$N is burned by $p.",ch,wield,victim,TO_NOTVICT);
			act_with_autodam_to_char("$N is burned by $p.",ch,wield,victim,dr);
			act_with_autodam_to_char("$p sears your flesh.",victim,wield,NULL,dr);			
			fire_effect( (void *) victim,wield->level/2,dam,TARGET_CHAR);			
		}
		
		// frost weapon effect
		if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_FROST))
		{
			dam = number_range(1,wield->level / 6 + 2);
			dr=damage(ch,victim,dam,0,DAM_COLD,false);
			act("$p freezes $N.",ch,wield,victim,TO_NOTVICT);
			act_with_autodam_to_char("$p freezes $N.",ch,wield,victim,dr);
			act_with_autodam_to_char("The cold touch of $p surrounds you with ice.",
				victim,wield,NULL,dr);			
			cold_effect(victim,wield->level/2,dam,TARGET_CHAR);			
		}

		// suckle weapon effect - takes mana
		if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_SUCKLE))
        {
            dam = number_range(1, wield->level / 5 + 1);
            act("$p draws mystical energy from $n.",victim,wield,NULL,TO_ROOM);
            act("You feel $p drawing your mystical energy away.",
                victim,wield,NULL,TO_CHAR);
            victim->mana -= dam;
            ch->mana += dam/2;
        } 

		// enervated weapon effect - takes movements
        if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_ENERVATE))
        {
            dam = number_range(1, wield->level / 5 + 1);
            act("$p saps the strength from $n.",victim,wield,NULL,TO_ROOM);
            act("You feel $p sapping your strength away.",
                victim,wield,NULL,TO_CHAR);
            victim->move-= dam;
            ch->move += dam/2;
        }
			// annealed weapon effect
        if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_ANNEALED))
        {
            dam = number_range(1,wield->level/5 + 2);
            dr=damage(ch,victim,dam,0,DAM_BASH,false);
            if (!saves_spell(wield->level/4 + dam/20,victim,DAM_BASH)){
				victim->printlnf("You see stars as your head `Crings`x!%s",
					autodamtext(victim, dr));
				ch->printlnf("Your opponent looks `Bstunned`x!%s",
					autodamtext(ch, dr));
				DAZE_STATE(victim,UMAX(12,wield->level/4 + dam/20));
            }
        }

		if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_SHOCKING))
		{			
			dam = number_range(1,wield->level/5 + 2);
			dr=damage(ch,victim,dam,0,DAM_LIGHTNING,false);
			act("$N is struck by lightning from $p.",ch,wield,victim,TO_NOTVICT);
			act_with_autodam_to_char("$N is struck by lightning from $p.",ch,wield,victim,dr);
			act_with_autodam_to_char("You are shocked by $p",victim,wield,NULL,dr);			
			shock_effect(victim,wield->level/2,dam,TARGET_CHAR);
		}
		return;
}
/**************************************************************************/
// Inflict damage from a hit.
// return the amount of hitpoints removed from the victim
int damage(char_data *ch,char_data *victim,int dam,int dt,int dam_type, bool show) 
{
	bool		immune;
	int			i;
	char_data	*rch;
	char_data	*rch_next = NULL;
	OBJ_DATA  	*loser;
    
    if ( !IS_VALID(victim) || victim->position == POS_DEAD ){
		return 0;
	}

    // doing damage system used to determine who starts fights
	if(    dt!=gsn_fire_shield
		&& dt!=gsn_chill_shield
		&& dt!=gsn_icy_armour
		&& dt!=gsn_fiery_armour 
		&& dt!=gsn_orbit
		&& dt!=gsn_blood_shield)
	{
		SET_BIT(ch->dyn,DYN_DOING_DAMAGE);
	}

	if(dt == gsn_orbit)
	{
		obj_data *wield;
		if ( (wield = get_eq_char( ch, WEAR_WIELD )) != NULL 
			&& wield->item_type == ITEM_WEAPON ){
			dt = TYPE_HIT + wield->value[3];
		}
		else {
			REMOVE_BIT(ch->dyn2, DYN2_DOING_ORBIT);
			return 0;
		}
	}
	//	Stop up any residual loopholes.
	if ( dam > 2200 && dt >= TYPE_HIT)
    {
		bugf( "Damage: %d: more than 2200 points! by %s.", dam, PERS(ch, NULL));
		dam = 2200;
		if (!IS_IMMORTAL(ch))
		{
			OBJ_DATA *obj;
			obj = get_eq_char( ch, WEAR_WIELD );
			ch->println("You really shouldn't cheat.");
			if (obj != NULL)
				extract_obj(obj);
			
		}
		
	}

	// scale the damage
	if(!GAMESETTING3(GAMESET3_EXPERIMENTAL_DAMAGE_SCALING)){
		// old damage scaling system originating from oblivion
		// too complicated for its own good.
		dam=dam*3/2;
		switch(UMIN(number_range(1,20),number_range(1,20)))
		{
		default :
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:  dam = dam / (victim->level*2/5+1) ;  break ;
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:  
		case 13: 
			if(ch->level>victim->level){
				dam=dam * (ch->level-victim->level+5)/5;
			}
			dam = dam / (victim->level*2/5+1);
			break;
		case 14:
		case 15: 
		case 16:
		case 17: 
			if(ch->level*11/10>victim->level){
				dam=dam * (ch->level*11/10-victim->level+5)/5;
			}
			dam = dam / (victim->level*2/5+1);
			break;
		case 18:
		case 19: 
			if(ch->level*12/10>victim->level){
				dam=dam * (ch->level*12/10-victim->level+5)/5;
			}
			dam = dam / (victim->level*2/5+1);
			break;
		case 20: 
			if(ch->level*15/10>victim->level){
				dam=dam * (ch->level*15/10-victim->level+5)/5;
			}
			dam = dam / (victim->level*2/5+1);
			break;
		}     
	}else{

		if (HAS_CONFIG2(ch, CONFIG2_FISHING))
			dam=dam*2;

		// increase/decrease the damage_scale_value 
		// we will use by upto 50%
		int scale=game_settings->damage_scale_value;
		if(number_range(1,2)==1){
			scale+=number_range(1,number_range(1,scale/2));
		}else{
			scale-=number_range(1,number_range(1,scale/2));
		}
		scale=URANGE(100, scale, 4000); 
		// apply the damage scale value
		scale=(victim->level * scale / 10000);
		if(scale){
			dam/=scale;
		}
	}
	
    if ( victim != ch )
    {
		
		//	Certain attacks are forbidden.
		//	Most other attacks are returned - set victim to fighting
		
		if ( is_safe( ch, victim ) ){
			REMOVE_BIT(ch->dyn,DYN_DOING_DAMAGE);
			return 0;
		}
		
		if ( victim->position > POS_STUNNED )
		{
			if ( ch->fighting == NULL ){
				set_fighting( ch, victim );
			}

			if ( victim->fighting == NULL ){ // moved from above
				set_fighting( victim, ch );
				if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_KILL )){
					mp_percent_trigger( victim, ch, NULL, NULL, TRIG_KILL );
					if(!IS_VALID(victim)){  // if the mob purged itself
						REMOVE_BIT(ch->dyn,DYN_DOING_DAMAGE);
						return 0;
					}					
				}
			}
			if (victim->timer <= 4){ // moved from above
				victim->position = POS_FIGHTING;
			}

			
			if ( IS_NPC(ch)
				&&   IS_NPC(victim)
				&&   IS_AFFECTED(victim, AFF_CHARM)
				&&   victim->master != NULL
				&&   victim->master->in_room == ch->in_room
				&&	 number_bits( 3 ) == 0 )
			{
				stop_fighting( ch, false ); // Damage
				multi_hit( ch, victim->master, TYPE_UNDEFINED );
				REMOVE_BIT(ch->dyn,DYN_DOING_DAMAGE);
				return 0;
			}
		}
		
		
		// More charm stuff.		
		if ( victim->master == ch ){
			stop_follower( victim );
		}
    }
	
	
	// Inviso attacks ... not.
	
	if ( IS_SET( ch->affected_by2, AFF2_TREEFORM ))
	{
		REMOVE_BIT( ch->affected_by2, AFF2_TREEFORM );
		affect_strip( ch, gsn_treeform );
		act( "A tree suddenly transforms into $n.", ch, NULL, NULL, TO_ROOM );
		ch->println( "You assume your normal form." );
	}

	if ( IS_SET( ch->affected_by2, AFF2_VANISH ))
	{
		REMOVE_BIT( ch->affected_by2, AFF2_VANISH );
		affect_strip ( ch, gsn_vanish );
		act( "A swirl of dust reveals $n.", ch, NULL, NULL, TO_ROOM );
		ch->println( "You shake the `#`Bfaerie-dust`^ from your body." );
	}

   	if ( IS_AFFECTED(ch, AFF_INVISIBLE) )
   	{
      		affect_parentspellfunc_strip( ch, gsn_invisibility );
      		affect_parentspellfunc_strip( ch, gsn_mass_invis );
      		REMOVE_BIT( ch->affected_by, AFF_INVISIBLE );
      		ch->println( "You fade into existence.");
      		act( "$n fades into existence.", ch, NULL, NULL, TO_ROOM );
   	}
   
   	if (    IS_SET( ch->affected_by, AFF_HIDE)
      		|| IS_SET( ch->affected_by, AFF_SNEAK))
   	{
      		REMOVE_BIT( ch->affected_by, AFF_HIDE      );
      		REMOVE_BIT( ch->affected_by, AFF_SNEAK     );
      		act( "$n is revealed from the shadows.", ch, NULL, NULL, TO_ROOM );
   	}
	
	// Set Mob Remember

	if ( victim->mobmemory ) {
	victim->mobmemory = ch;
	}
	else
	mobRememberSet( victim, ch );

	// Damage modifiers.
	
	if ( dam > 1 && !IS_NPC(victim)
		&&   victim->pcdata->condition[COND_DRUNK]  > 10 )
		dam = 9 * dam / 10;
	
	if ( dam > 1 && IS_AFFECTED(victim, AFF_SANCTUARY) )
		dam /= 2;

    	if ( !IS_NPC(victim) && IS_VAMPAFF(victim, VAM_FORTITUDE) )
    	{
	   int ammount = (get_age(victim)/100);
	   if ( IS_VAMPAFF(victim, VAM_FORTITUDE) && ammount >= 7 )
		  ammount = 6;   
	   else if ( ammount >= 6 )
		  ammount = 5;
	   ammount += ( 13 - ch->vampgen);
	   dam = dam * number_range(95-ammount,95) /100;
    	}
	
	// subdue modifications - make damage 60-90% of what it would have been
    if (IS_SET(ch->dyn,DYN_CURRENT_SUBDUE))
    {
		dam = dam * number_range(60,90) /100;
    }
	
    if ( dam > 1 && ((IS_AFFECTED(victim, AFF_PROTECT_EVIL) && IS_EVIL(ch) )
		||               (IS_AFFECTED(victim, AFF_PROTECT_GOOD) && IS_GOOD(ch) )))
		dam -= dam / 4;
	
	if(num_enemies(victim)>num_enemies(ch))
		dam = dam + dam * (num_enemies(victim) - num_enemies(ch))/2;
	
	immune = false;

	
	// Check for parry, and dodge.
	
    if ( dt >= TYPE_HIT && ch != victim)
    {
		if ( check_parry( ch, victim ) ){
			REMOVE_BIT(ch->dyn,DYN_DOING_DAMAGE);
			return 0;
		}
		if ( check_dodge( ch, victim ) ){
			REMOVE_BIT(ch->dyn,DYN_DOING_DAMAGE);
			return 0;
		}
		if ( check_shield_block(ch,victim)){
			REMOVE_BIT(ch->dyn,DYN_DOING_DAMAGE);
			return 0;
		}
		if ( (IS_SET(victim->dyn2, DYN2_DOING_SPINBLOCK)
			|| (IS_NPC(victim) && IS_SET(victim->off_flags, OFF_SPINBLOCK)))
		&&   check_spinblock(ch,victim)){
			REMOVE_BIT(ch->dyn,DYN_DOING_DAMAGE);
			return 0;
		}

	}
	
	// Let's check for Jinn affinities (kinda messy) clean up later
	if(	ch != victim
	&& (( (dt == DAM_LIGHT || dt == DAM_FIRE)
		&& get_skill(ch, gsn_fire_affinity) >= number_percent())
	||  ( (dt == DAM_POISON || dt == DAM_ACID)
		&& get_skill(ch, gsn_earth_affinity) >= number_percent())
	||  ( (dt == DAM_DROWNING || dt == DAM_COLD)
		&& get_skill(ch, gsn_water_affinity) >= number_percent())
	||  ( (dt == DAM_LIGHTNING || dt == DAM_SOUND)
		&& get_skill(ch, gsn_air_affinity) >= number_percent())))
	{
		REMOVE_BIT(ch->dyn,DYN_DOING_DAMAGE);
		victim->mana = UMAX(50, victim->mana + dam);
		act("Your attack flows into $N!", ch, NULL, victim, TO_CHAR);
		act("$n's attack flows into $N!", ch, NULL, victim, TO_NOTVICT);
		act("$n's attack flows into you.", ch, NULL, victim, TO_VICT);
		return 0;

	}

	switch(check_immune(victim,dam_type))
	{
	case(IS_IMMUNE):
		immune = true;
		dam = 0;
		break;
	case(IS_RESISTANT):
		dam -= dam/2;
		break;
	case(IS_VULNERABLE):
		dam += dam;
		break;
	}
	
	if(show){
		dam_message( ch, victim, dam, dt, immune );
	}
	
    if(dam == 0){
		REMOVE_BIT(ch->dyn,DYN_DOING_DAMAGE);
		return 0;
	}
	
	//	hurt the victim.
	victim->hit -= dam;

	// if we did damage, record if the level of the attacker is higher than
	// the previously highest recorded attacker.  This information is used
	// when allocating group xp, the amount of XP awarded reduces based on 
	// if you are more than 8 levels below the highest attacker's level
	// (the lower you are from that level, the less you get)
	if(dam>0){
		victim->highest_level_to_do_damage=
			UMAX(victim->highest_level_to_do_damage, ch->level);
	}

	//Storing damage types and precedence
	if(dam > 0)
	{
		if(victim->last_damage[1] >= 2000)
			victim->last_damage[1] = 1000+dam_type;
		else 
		{
			if(victim->last_damage[1] >= 1000)
				victim->last_damage[1]-=1000;
			victim->last_damage[0] = 1000+dam_type;
		}
	}
	else if(victim->last_damage[1] >= 2000)
		victim->last_damage[1]-=2000;

	//	inform the victim of their new state.
    update_pos( victim );
    
	// if the victim is riding a mount, check if they are knocked off it
	if(victim->mounted_on){
		long percent_of_max_hits;
		percent_of_max_hits=dam*100;
		percent_of_max_hits/=victim->max_hit;
		
		if(percent_of_max_hits>=10){
			if(!IS_NPC(victim)) // player
			{
				if ((number_range(1, 100)) <
					(105-get_skill(victim, gsn_riding))	*
					(percent_of_max_hits-9))
				{
					act("You are knocked from your mount!\r\n", victim, NULL, NULL, TO_CHAR);
					act("$n is knocked from $s mount!\r\n", victim, NULL, NULL, TO_ROOM);
					dismount(victim);
					check_improve(victim, gsn_riding, false, 1);
				}
			}else{ // mob
				if (number_range(1, 100)+((percent_of_max_hits-10)*3)>80){
					act("You are knocked from your mount!\r\n", victim, NULL, NULL, TO_CHAR);
					act("$n is knocked from %s mount!\r\n", victim, NULL, NULL, TO_ROOM);
					dismount(victim);
				}
			}
		}
	}
	
	if(dam>1)
	{
		if(is_affected(victim,gsn_fire_shield) && dt!=gsn_fire_shield){
			damage(victim,ch,UMIN(ch->max_hit,dam*3/4),gsn_fire_shield,DAM_FIRE,true);
		}
		if(is_affected(victim,gsn_blood_shield) && dt!=gsn_blood_shield){
			int blood_dam = damage(victim,ch,UMIN(ch->max_hit,dam*3/2),gsn_blood_shield,DAM_NEGATIVE,true);
			if(blood_dam > 0)
				ch->hit+=blood_dam;
		}
		if(is_affected(victim,gsn_chill_shield) && dt!=gsn_chill_shield){
			damage(victim,ch,UMIN(ch->max_hit,dam*3/4),gsn_chill_shield,DAM_COLD,true);
		}
		if(is_affected(victim,gsn_fiery_armour) && dt!=gsn_fiery_armour){
			damage(victim,ch,UMIN(ch->max_hit,dam*3/4),gsn_fiery_armour,DAM_FIRE,true);
		}
		if(is_affected(victim,gsn_icy_armour) && dt!=gsn_icy_armour){
			damage(victim,ch,UMIN(ch->max_hit,dam*3/4),gsn_icy_armour,DAM_COLD,true);
		}
		if(IS_SET(victim->dyn2, DYN2_DOING_ORBIT) && dt!=gsn_orbit){
			int orbit_damage = damage(victim,ch,UMIN(ch->max_hit,dam*3/2),gsn_orbit,DAM_NONE,true);
			if(orbit_damage > 0)
				weapon_effects(ch, victim, get_eq_char( ch, WEAR_WIELD ));
		}

	}
	
	if(100 * dam / victim->max_hit > (i = number_percent())){
		bash_eq(victim,i);
	}
	
    update_pos(ch);
	
/************** ARREST **************************************************/

    if (IS_SET(victim->act2, ACT2_UNDERARREST) 
    && IS_AFFECTED2(ch, AFF2_BOUNTYHUNTER)
    && !IS_AWAKE(victim) && IS_NPC(ch))
  
    {
	victim->pknorecall = 0; 
	victim->pknoquit = 0;	
	act( "$n is placed in shackles and has been arrested.", victim, NULL, NULL, TO_ROOM );
	victim->println("You have been arrested and taken to prison.");
	stop_fighting( victim, true); // Damage - subdued
	stop_fighting( ch, true); // Damage - subdued
	victim->position=POS_RESTING;
	victim->hit=1;

	if(victim->pcdata->theft > 0 && victim->pcdata->murder == 0)
		info_broadcast(ch, "%s has been arrested by the guards for ROBBERY.", victim->name);
	
	if(victim->pcdata->theft == 0 && victim->pcdata->murder > 0)
		info_broadcast(ch, "%s has been arrested by the guards for MURDER.", victim->name);

	if(victim->pcdata->theft > 0 && victim->pcdata->murder > 0)
		info_broadcast(ch, "%s has been arrested by the guards for ROBBERY & MURDER.", victim->name);

	REMOVE_BIT(victim->act2, ACT2_UNDERARREST);
	sprintf( log_buf, "%s arrested by %s at %d", victim->name,
	(IS_NPC(ch) ? ch->short_descr : ch->name),
		ch->in_room->vnum );
	log_string( log_buf );
		
	wiznet(log_buf,NULL,NULL,WIZ_BETA,0,0);
	if(victim->pcdata->murder > 3 && victim->pcdata->murder < 50)
		victim->pcdata->murder = victim->pcdata->murder / 3;
	if(victim->pcdata->theft > 3)
		victim->pcdata->theft = victim->pcdata->theft / 3;

	if (victim->pcdata->theft == 0 && victim->pcdata->murder == 0 
				      && IS_WEREBEAST(victim))
	{
		info_broadcast(ch, "%s has been arrested by the guards for being a WEREWOLF.", victim->name);
		victim->pcdata->murder = 1;
	}

	do_startprison(victim, "");
	return 0;
    }


    if (IS_SET(victim->act2, ACT2_UNDERARREST) 
    && IS_AFFECTED2(ch, AFF2_BOUNTYHUNTER)
    && !IS_AWAKE(victim))
  
		{
		ch->pknorecall = 5;
		ch->pknoquit = 5;
		victim->pknorecall = 0; 
		victim->pknoquit = 0;	
		act( "$n is placed in shackles and has been arrested.", victim, NULL, NULL, TO_ROOM );
		victim->println("You have been arrested and taken to prison.");
		stop_fighting( victim, true); // Damage - subdued
		stop_fighting(ch, true); // Damage - subdued
		group_gain( ch, victim );
		victim->subdued= true;
		victim->subdued_timer = number_range(5,10);
		victim->position=POS_RESTING;
		victim->hit=1;
		int bounty;
		bounty= (victim->pcdata->theft + victim->pcdata->murder) * 50;

		if(bounty > 500)
			bounty = 500;

		ch->printlnf("`#`YYou receive a bounty of %d gold.`^", bounty);
		ch->gold += bounty;

		if(victim->pcdata->theft > 0 && victim->pcdata->murder == 0)
			info_broadcast(ch, "%s has been arrested for ROBBERY.", victim->name);
		
		if(victim->pcdata->theft == 0 && victim->pcdata->murder > 0)
			info_broadcast(ch, "%s has been arrested for MURDER.", victim->name);

		if(victim->pcdata->theft > 0 && victim->pcdata->murder > 0)
			info_broadcast(ch, "%s has been arrested for ROBBERY & MURDER.", victim->name);
		

		REMOVE_BIT(victim->act2, ACT2_UNDERARREST);
		sprintf( log_buf, "%s arrested by %s at %d",
			victim->name,
			(IS_NPC(ch) ? ch->short_descr : ch->name),
			ch->in_room->vnum );
		log_string( log_buf );
		
		wiznet(log_buf,NULL,NULL,WIZ_BETA,0,0);
		do_startprison(victim, "");
		return 0;
    }



/****************************************************************/
	
	
	// Subdue System - Kalahn June 97, updated October 98
    if (IS_SET(ch->dyn,DYN_CURRENT_SUBDUE) 
		&& !IS_AWAKE(victim) && !IS_SET(ch->in_room->room2_flags,ROOM2_ARENA)
		&& !IS_SET(ch->in_room->room2_flags,ROOM2_WAR)
		&& (  IS_IMMORTAL(ch) 
		|| (number_percent()< UMAX(15,101-(GET_HITROLL(ch)/2)))
		)
		)
    {
		act( "$n appears to be subdued.", victim, NULL, NULL, TO_ROOM );
		victim->println("You have been subdued.");

		if(!IS_NPC(victim)){
			group_bypass_killer_penatly(ch, victim, false);

			// update duels to protect victim
			duel_protect_victim(victim);
		}

		stop_fighting( victim, true); // Damage - subdued
		
		group_gain( ch, victim );
		
		victim->subdued= true;
		victim->subdued_timer = number_range(10,20);
		victim->is_stunned = ch->level;
		victim->position=POS_STUNNED;

 	/************* VAMPIRE SUBDUAL *************/
	if ( !IS_NPC(victim) && IS_VAMPIRE(victim))
	{
		victim->subdued=true;
		victim->subdued_timer = 5;
		victim->position = POS_MORTAL;
		victim->hit = -10;
		update_pos(victim);
	}
 	/************* VAMPIRE SUBDUAL *************/

		
		if (IS_NPC(victim))
		{
			victim->no_xp = true;
			victim->pIndexData->count--;        
			// no more exchanging money with a weak money changer and then
			// subduing the mob and getting your silver back.
			if (IS_SET(victim->act,ACT_IS_CHANGER)){
				victim->gold=	number_range(1,UMIN(3,victim->gold));
				victim->silver= number_range(1,UMIN(350,victim->silver));
			}
		}

		sprintf( log_buf, "%s subdued by %s at %d",
			victim->name,
			(IS_NPC(ch) ? ch->short_descr : ch->name),
			ch->in_room->vnum );
		log_string( log_buf );
		
		wiznet(log_buf,NULL,NULL,WIZ_BETA,0,0);

		if(GAMESETTING5(GAMESET5_DEDICATED_PKILL_STYLE_MUD)){
			pkill_broadcast(log_buf);
		}else{
			if (!IS_NPC(victim))
			{
				sprintf( log_buf, "%s[%d] subdued by %s[%d] at %d",
					victim->name, victim->level,
					ch->name, ch->level,
					ch->in_room->vnum );
				wiznet(log_buf,NULL,NULL,WIZ_DEATHS,0,0);
			}
		}
    }
    else
    {
		switch( victim->position )
		{
		case POS_MORTAL:
			act( "$n is mortally wounded, and will die soon, if not aided.",
				victim, NULL, NULL, TO_ROOM );
			victim->println("You are mortally wounded, and will die soon, if not aided.");
			break;
			
		case POS_INCAP:
			act( "$n is incapacitated and will slowly die, if not aided.",
				victim, NULL, NULL, TO_ROOM );
			victim->println("You are incapacitated and will slowly die, if not aided.");
			break;
			
		case POS_STUNNED:
			act( "$n is stunned, but will probably recover.",
				victim, NULL, NULL, TO_ROOM );
			victim->println("You are stunned, but will probably recover.");
			break;
			
		case POS_DEAD:
			act( "$n is DEAD!!", victim, 0, 0, TO_ROOM );
			victim->println("You have been `rKILLED!!`x"); 
			break;
			
		default:
			if ( victim->hit < victim->max_hit / 4 )
			{
				victim->println("`YYou sure are `rBLEEDING!`x"); 
			}
			break;
		}

		// Sleep spells and extremely wounded folks.
		if ( !IS_AWAKE(victim) ){ 
			stop_fighting( victim, false ); // Damage - not awake victim
		}
		
	/* JAIL DEATH FIX */
	if ( !IS_NPC(victim) && 
	      is_name( victim->in_room->area->file_name, "jailarea.are" )&& 
              victim->in_room->vnum != 10398)
	{
		victim->position = POS_RESTING;
		victim->printlnf("`#`YDoctors rush in and revive you from certain death.`^");
		victim->subdued = false;
		victim->subdued_timer=0;

		victim->pcdata->tired=UMIN(0, victim->pcdata->tired);
		victim->pcdata->condition[COND_THIRST]=
			UMAX(30, victim->pcdata->condition[COND_THIRST]);
		victim->pcdata->condition[COND_HUNGER]=
			UMAX(30,victim->pcdata->condition[COND_HUNGER]);
		victim->pcdata->condition[COND_FULL]=
			UMIN(0,victim->pcdata->condition[COND_FULL]);
		victim->pcdata->condition[COND_DRUNK]=
			UMIN(0,victim->pcdata->condition[COND_DRUNK]);

		victim->hit 	= victim->max_hit;
		victim->mana	= victim->max_mana;
		victim->move	= victim->max_move;
    		victim->bleeding = 0;
    		victim->will_die = 0;
		update_pos( victim);
		do_look(victim, "auto");
		return 0;
	}


// Arena Code by IXLIAM

 if (victim->position == POS_DEAD && !IS_NPC(victim) && !IS_NPC(ch)
    && (IS_SET(victim->in_room->room2_flags,ROOM2_ARENA)))
    {

    ch->pcdata->akills += 1;
    victim->pcdata->adeaths += 1; 

    ch->pknorecall=0;
    ch->pknoquit  =0;
    victim->pknorecall=0; 
    victim->pknoquit  =0;

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
    REMOVE_BIT(ch->dyn,DYN_DOING_DAMAGE);
    tail_chain( );
    return 0;
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
    ch->pknorecall=0;
    ch->pknoquit  =0;
    victim->pknorecall=0; 
    victim->pknoquit  =0;
    victim->bleeding = 0;
    victim->will_die = 0;
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
     REMOVE_BIT(ch->dyn,DYN_DOING_DAMAGE);
     tail_chain( );
     return 0;
    }

// End of War Battlefield Code
		
		// Payoff for killing things.
		if ( victim->position == POS_DEAD )
		{
			int number_in_room=0;
			// ** get rch the safe way 
			// This code is necessary because a mob (ch) can trigger on 
			// on position acts ("is DEAD!!" etc)... and purge itself
			rch= NULL; 
			if(IS_VALID(ch) && ch->in_room){ 
				rch = ch->in_room->people;
				number_in_room=ch->in_room->number_in_room;
			}else if(IS_VALID(victim) && victim->in_room){
				rch = victim->in_room->people;
				number_in_room=victim->in_room->number_in_room;
			}
			if(!rch){
				logf("Not testing for roomdeath trigger cause victim room and ch room couldn't be found.");
			}

			// ** Check for the roomdeath triggers			
			// note: number_in_room is used to prevent a mobprog on two mobs in 
			// a room creating an endless loop by removing themself from
			// the room and putting themselves back in the room - Kal, June 01
			for ( ; rch && --number_in_room>=0; rch = rch_next )
			{
				rch_next = rch->next_in_room;
				
				if ( IS_NPC( rch ))
				{
					if ( HAS_TRIGGER( rch, TRIG_ROOMDEATH )
						&&	 can_see( rch, ch )
						&& rch!=victim)
					{
						logf("Running roomdeath trigger prog on mob '%s' %d", 
							PERS(rch, NULL), rch->pIndexData?rch->pIndexData->vnum:0);
						mp_percent_trigger( rch, ch, NULL, NULL, TRIG_ROOMDEATH );
					}
				}
			}
			kill_char(victim, ch);
			REMOVE_BIT(ch->dyn,DYN_DOING_DAMAGE);
			return dam;
		}

		if ( victim == ch ){
			REMOVE_BIT(ch->dyn,DYN_DOING_DAMAGE);
			return dam;
		}
		
		// Take care of link dead people.
		if ( !IS_NPC(victim) && victim->desc == NULL )
		{
			if ( number_range( 0, victim->wait ) == 0 
				&& HAS_CONFIG(victim, CONFIG_AUTORECALL))
			{
				do_recall( victim, "" );
				REMOVE_BIT(ch->dyn,DYN_DOING_DAMAGE);
				return dam;
			}
		}
		
		// Wimp out?
		if ( IS_NPC(victim) && dam > 0 && victim->wait < PULSE_VIOLENCE / 2)
		{
			if ( ( IS_SET(victim->act, ACT_WIMPY) && number_bits( 2 ) == 0
				&&   victim->hit < victim->max_hit / 5)
				||   ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL
				&&     victim->master->in_room != victim->in_room ) )
				do_flee( victim, "" );
		}
		
		if ( !IS_NPC(victim)
			&&   victim->hit > 0
			&&   victim->hit <= victim->wimpy
			&&   victim->wait < PULSE_VIOLENCE / 2 )
			do_flee( victim, "" );
		
		if ( !IS_NPC(victim)
			&&   victim->hit > 0
			&&   victim->hit <= victim->pcdata->panic
			&&   victim->wait < PULSE_VIOLENCE / 2 )
		{
			victim->println("P A N I C ! ! !");
			do_recall( victim, "" );
		}
    } // end of subdue else 
	
	REMOVE_BIT(ch->dyn,DYN_DOING_DAMAGE);
    tail_chain( );
    return dam;
}
/**************************************************************************/
// spells call this to inflict damage
// (dt = sn)
int damage_spell( char_data *ch, char_data *victim, int dam, int dt, 
		int dam_type, bool show )
{
//	char buf[MSL];
	if(victim->last_damage[1] >= 1000)
		victim->last_damage[1] += 1000;
	else
		victim->last_damage[1] += 2000;
	if(victim->last_damage[0] >= 1000)
		victim->last_damage[0] -= 1000;
	int damount=damage( ch, victim, dam, dt, dam_type, show );
	// code below in for monitoring reasons to fix up the AC system
/*	sprintf(buf,"%3d, %3d, %3d, \"%s\", \"%s\", %3d, \"%s\", %3d, %d", 
		dt, dam, dam_type, skill_table[dt].name, ch->name, ch->level, 
		victim->name, victim->level, damount);
	append_string_to_file( SPELL_DAMAGE_LOG,buf);
*/
	obj_data *mark;
	if(damount > 0
	&& (mark = get_eq_char(victim, WEAR_FACE)) != NULL
	&& mark->ospec_fun == ospec_lookup("ospec_chameleon_mark"))
		ospec_chameleon_mark(mark, victim);
	return (damount);
}
/**************************************************************************/
bool is_safe(char_data *ch, char_data *victim)
{
    char log_buf[MSL];
	
	if (victim->in_room == NULL || ch->in_room == NULL)
		return true;

	if (IS_SET(ch->act, ACT_IS_UNSEEN) &&
            IS_SET(ch->affected_by2, AFF2_VAMP_BITE ))
	{
		ch->println("You cannot attack while in mist form.");
		return true;
	}

    /* Ethereal people can only attack other ethereal people */
    if ( IS_SET(ch->affected_by2, AFF2_ETHEREAL) && !IS_SET(victim->affected_by2, AFF2_ETHEREAL) )
    {
	   ch->println( "You cannot while ethereal.");
	   return true;
    }
    if ( !IS_SET(ch->affected_by2, AFF2_ETHEREAL) && IS_SET(victim->affected_by2, AFF2_ETHEREAL) )
    {
	   ch->println( "You cannot fight an ethereal person.");
	   return true;
    }

    if(is_affected( ch, gsn_hold_person))
    {
	   ch->println( "You cannot fight while immobilized.");
	   return true;
    }

    if(is_affected( victim, gsn_hold_person))
    {
	   ch->println( "You cannot fight someone immobilized.");
	   return true;
    }
    
    /* You cannot attack across planes */
    if ( IS_AFFECTED2(ch, AFF2_SHADOWPLANE) && !IS_SET(victim->affected_by2, AFF2_SHADOWPLANE) )
    {
	   act( "You are too insubstantial!", ch, NULL, victim, TO_CHAR );
	   return true;
    }
    if ( !IS_AFFECTED2(ch, AFF2_SHADOWPLANE) && IS_SET(victim->affected_by2, AFF2_SHADOWPLANE) )
    {
	   act( "$E is too insubstantial!", ch, NULL, victim, TO_CHAR );
	   return true;
    }


// Arena & War Rooms
    	if(IS_SET(ch->in_room->room2_flags,ROOM2_ARENA) ||
           IS_SET(ch->in_room->room2_flags,ROOM2_WAR) )
        	return false;


	if (victim->fighting == ch || victim == ch)
		return false;
	
	if (IS_IMMORTAL(ch) && ch->level > LEVEL_IMMORTAL)
		return false;

	if(!IS_NPC(ch) && !IS_NPC(victim) && GAMESETTING(GAMESET_PEACEFUL_MUD)){
		ch->println("This is a peaceful mud... pkilling is not permitted.");
		return true;
	}

	// unseen mobs
	if ( !IS_IMMORTAL(ch) && 
		IS_NPC(victim) && IS_SET(victim->act, ACT_IS_UNSEEN))
	{
		return true;
	}

	if( IS_NPC(victim) && IS_SET(victim->act2,ACT2_AVOIDS_ALL_ATTACKS) ){
		ch->printlnf("%s avoids your attack.", PERS(victim, ch));
		return true;
	}

	if( IS_KEEPER(victim) && !GAMESETTING2(GAMESET2_CAN_ATTACK_SHOPKEEPERS)){
		ch->printlnf("%s avoids your attack.", PERS(victim, ch));
		return true;
	}
	
	// safe rooms
	if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
	{
		ch->println("Not in this room.");
		return true;
	}
	
	// ooc rooms
	if (IS_SET(victim->in_room->room_flags,ROOM_OOC))
	{
		ch->println("Not in an OOC room.");
		return true;
	}
	
	// optional: killing only permitted within ten levels of each other
	if (!IS_NPC(victim) && GAMESETTING4(GAMESET4_NO_PK_OUTSIDE_10_LEVELS)){	
		if ( (ch->level - victim->level) > 10  || (ch->level - victim->level) < -10 ) {
			ch->println("You may not pkill a character of more than 10 levels difference to your own." );
			return true;
		}
	}
	
	
    // killing mobiles 
    if (IS_NPC(victim))
    {
		if( ch->level<6 || (!IS_LETGAINED(ch) && GAMESETTING_LETGAINING_IS_REQUIRED) ){
			if ( IS_SET(victim->act, ACT_PET) 		
					&& victim->leader 
					&& !IS_NPC(victim->leader) 
					&& (victim->leader->pet == victim)
					&& IS_SET(victim->affected_by, AFF_CHARM))
			{
				ch->wrapln("That is a players pet, which could start a pkill fight, "
					"since you are either below level 6 or not letgained you can't "
					"start pkill combat.");
				return true;
			}
		}

		if (HAS_CONFIG(ch, CONFIG_DISALLOWED_PKILL))
		{
			if ( IS_SET(victim->act, ACT_PET) 		
					&& victim->leader 
					&& !IS_NPC(victim->leader) 
					&& (victim->leader->pet == victim)
					&& IS_SET(victim->affected_by, AFF_CHARM))
			{
				ch->wrapln("That is a players pet, which could start a pkill fight, "
					"since you are pkill restricted you can't start pkill combat.");
				return true;
			}
		}
		
		/* killing shopkeepers is now allowed */
		
		/*if (victim->pIndexData->pShop != NULL)
		*{
		*     ch->println("The shopkeeper wouldn't like that.");
		*     return true;
		*}
		*/
		
		/* killing healers, trainers, etc is now allowed */
		
		/*
		* if (IS_SET(victim->act,ACT_TRAIN)
		* ||  IS_SET(victim->act,ACT_PRACTICE)
		*  ||  IS_SET(victim->act,ACT_IS_HEALER)
		*  ||  IS_SET(victim->act,ACT_IS_CHANGER))
		*  {
		*     ch->println("I don't think Kalahn would approve.");
		*     return true;
		*  }
		*/
		
		/* checks that players can't kill certain types of mobs */
		if (!IS_NPC(ch))
		{
			/* no pets */
			/*    if (IS_SET(victim->act,ACT_PET))
			*   {
			*  act("But $N looks so cute and cuddly...",
			*       ch,NULL,victim,TO_CHAR);
			*  return true;
			*   }
			*/
			/* no charmed creatures unless owner */
			
			/*			if (IS_AFFECTED(victim,AFF_CHARM) && ch != victim->master)
			{
			ch->println("You don't own that monster.");
			return true;
			}
			*/
		}
    }
    else  // killing players
    {
		// being ordered to, and the master isn't letgained
		if(IS_SET( ch->dyn, DYN_IS_BEING_ORDERED ) && ch->master)
		{
			if((ch->master->level<6) || (!IS_LETGAINED(ch->master) && GAMESETTING_LETGAINING_IS_REQUIRED) ){
				// tell charmie
				ch->println("Your master isn't letgained or above level 6.");
				// tell master
				ch->master->println("You can not order a pet to pkill, steal from players etc,");
				ch->master->println("till level 6 and you have been letgained.");
				ch->master->println("(See HELP DEATH and HELP LETGAIN)");
				return true;
			}
		}
		
		// NPC doing the killing
		if (IS_NPC(ch))
		{
			// safe room check
			if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
			{
				ch->println("Not in this room.");
				return true;
			}
			
			// charmed mobs and pets cannot attack players while owned
			if (IS_AFFECTED(ch,AFF_CHARM) && ch->master != NULL
				&&  ch->master->fighting != victim)
			{
				ch->println("Players are your friends!");
				return true;
			}
			if (IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL 
				&& ch->master->level<ch->level )
			{
				ch->master->println( "Your pet realizes it is more powerfull than you and leaves." );
				act( "$n stops following you.", ch, NULL, ch->master, TO_VICT );
				act( "You stop following $N.",	ch, NULL, ch->master, TO_CHAR );

				REMOVE_BIT(ch->master->pet->act, ACT_PET);
				stop_fighting(ch,false);
				stop_follower(ch->master->pet);
				return true;
			}
			
		}
		else  // player killing another player
		{
			if ((ch->level<6) || (!IS_LETGAINED(ch) && GAMESETTING_LETGAINING_IS_REQUIRED) ){
				ch->println("You can not player kill, steal from players etc,");
				ch->println("till level 6 and you have been letgained.");
				ch->println("(See HELP DEATH and HELP LETGAIN)");
				return true;
			}

			if(HAS_CONFIG(ch, CONFIG_DISALLOWED_PKILL))
			{
				ch->println("You can not player kill, steal from players etc,");
				ch->println("due to a pkill restriction sorry.");
				return true;
			}

			if(HAS_CONFIG2(ch,CONFIG2_NOPKILL)){
				ch->println("You can't pkill others");
				return true;
			}
			if(HAS_CONFIG2(victim,CONFIG2_NOPKILL)){
				ch->println("A mysterious force prevents you from doing that.");
				return true;
			}

			if(((victim->level<11))&&(ch->pkool>0))
			{
				ch->println("You cannot kill the weak so soon after doing so.");
				return true;
			}
			if(victim->pksafe>0)
			{
				ch->println("He has died too recently, leave him alone.");
				return true;
			}
			
			if (!IS_NPC(ch) && !IS_NPC(victim))
			{
				sprintf( log_buf, "is_safe: %s might be starting a pkill fight with %s at %d!!!",
					ch->name, 
					victim->name,
					ch->in_room->vnum );
				log_string( log_buf );
				wiznet(log_buf,NULL,NULL,WIZ_BETA,0,0);
			}
			
			
		}
    }
    return false;
}

/**************************************************************************/
bool is_safe_spell(char_data *ch, char_data *victim, bool area )
{
	if (victim->in_room == NULL || ch->in_room == NULL)
		return true;
	
	if (victim == ch && area){
		return true;
	}
	
	if ( !IS_ICIMMORTAL(ch) && 
		IS_NPC(victim) && IS_SET(victim->act, ACT_IS_UNSEEN))
	{
		return true;
	}
	
	// Arena & War Rooms
    	if(IS_SET(ch->in_room->room2_flags,ROOM2_ARENA) ||
          IS_SET(ch->in_room->room2_flags,ROOM2_WAR) )
        	return false;

	if (victim->fighting == ch || victim == ch){
		return false;
	}

	if(area)
	{
		if (ch->fighting!=victim && victim->fighting!=ch 
			&& !can_initiate_combat( ch, victim, CIT_GENERAL | CIT_SILENT))
		{
			return true;
		}
	}else{
		if (ch->fighting!=victim && victim->fighting!=ch 
			&& !can_initiate_combat( ch, victim, CIT_GENERAL))
		{
			return true;
		}
	}
	
    if (IS_ICIMMORTAL(ch) && !area){
		return false;
	}
		
	// safe room? 
	if (IS_SET(victim->in_room->room_flags,ROOM_SAFE)){
		return true;
	}
	
	if (HAS_CONFIG(ch, CONFIG_DISALLOWED_PKILL))
	{
		return true;
	}
	
	// killing mobiles 
	if (IS_NPC(victim))
	{
		if (victim->pIndexData->pShop != NULL)
			return true;
		
		// no killing healers, trainers, etc 
		if (IS_SET(victim->act,ACT_TRAIN)
			||  IS_SET(victim->act,ACT_PRACTICE)
			||  IS_SET(victim->act,ACT_IS_HEALER)
			||  IS_SET(victim->act,ACT_IS_CHANGER))
			return true;
		
		if (!IS_NPC(ch))
		{
			// no pets 
			if (IS_SET(victim->act,ACT_PET))
				return true;
			
			// no charmed creatures unless owner 
			if (IS_AFFECTED(victim,AFF_CHARM) && (area || ch != victim->master))
				return true;
			
			// legal kill? -- cannot hit mob fighting non-group member 
			if (victim->fighting != NULL && !is_same_group(ch,victim->fighting))
				return true;
		}
		else
		{
			// area effect spells do not hit other mobs 
			if (area && !is_same_group(victim,ch->fighting))
				return true;
		}
	}
	// killing players 
	else
	{
		if (area && IS_ICIMMORTAL(victim)){
			return true;
		}

		if(HAS_CONFIG2(ch,CONFIG2_NOPKILL)){
			return true;
		}
		if(HAS_CONFIG2(victim,CONFIG2_NOPKILL)){
			return true;
		}
		
		// NPC doing the killing 
		if (IS_NPC(ch))
		{
			// charmed mobs and pets cannot attack players while owned 
			if (IS_AFFECTED(ch,AFF_CHARM) && ch->master != NULL
				&&  ch->master->fighting != victim)
				return true;
			
			// safe room?
			if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
				return true;
			
			// legal kill? -- mobs only hit players grouped with opponent
			if (ch->fighting != NULL && !is_same_group(ch->fighting,victim))
				return true;
		}		
		// player doing the killing
		else
		{
			if(ch->level<11)
				return true;
			
			if(victim->pksafe>0)
				return true;
			
			if ((victim->level<11)&&(ch->pkool>0))
				return true;
		}
		
		
	}
	
    if (!IS_NPC(ch) && !IS_NPC(victim))
    {
		sprintf( log_buf, "is_safe_spell: %s might be starting a pkill fight with %s at %d!!!",
			ch->name, 
			victim->name,
			ch->in_room->vnum );
		log_string( log_buf );
		wiznet(log_buf,NULL,NULL,WIZ_BETA,0,0);
    }
	return false;
}


/**************************************************************************/
// Check for parry.
bool check_parry( char_data *ch, char_data *victim )
{
	int chance;
	
	if ( !IS_AWAKE(victim) )
		return false;
	
	chance = get_skill(victim,gsn_parry) / 2;
	
	if ( get_eq_char( victim, WEAR_WIELD ) == NULL )
	{
		if (IS_NPC(victim))
			chance /= 2;
		else
			return false;
	}
	
	if (!can_see(ch,victim))
		chance /= 2;
	
	if ( number_percent( ) >= chance + victim->level - ch->level )
		return false;
	
	act( "`bYou parry $n's attack.`x", ch, NULL, victim, TO_VICT  );
	act( "`b$n parries your attack.`x", victim, NULL, ch, TO_VICT );
	check_improve(victim,gsn_parry,true,6);
	return true;
}
/**************************************************************************/
// Check for spinblock.

bool check_spinblock( char_data *ch, char_data *victim )
{
	int chance;
	obj_data *obj;
	if ( !IS_AWAKE(victim) )
		return false;
	
	chance = get_skill(victim,gsn_spinblock) / 2;
	
	if((obj = get_eq_char( victim, WEAR_WIELD )) == NULL 
	|| (  obj->item_type != ITEM_WEAPON)
	|| (obj->value[0] != WEAPON_STAFF)
	|| !IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
	{
		if(!IS_NPC(ch))
			REMOVE_BIT(victim->dyn2, DYN2_DOING_SPINBLOCK);
		return false;
	}
	if( victim->move < 1)
	{
		if(!IS_NPC(ch))
			REMOVE_BIT(victim->dyn2, DYN2_DOING_SPINBLOCK);
		return false;
	}

	if (!can_see(ch,victim))
		chance /= 2;
	
	if ( number_percent( ) >= chance + victim->level - ch->level )
		return false;
	if(!IS_NPC(victim) && victim->pcdata->tired!=-1)
	{
		if(victim->pcdata->tired>60 
		|| victim->move == 0)
		{
			victim->println("You stop spinning your staff as exhaustion sets in.");
			REMOVE_BIT(victim->dyn2, DYN2_DOING_SPINBLOCK);
			return false;
		}
		victim->pcdata->tired += (100 - chance) / 15;
	}	
	act( "`bYou spin your staff to block an attack.`x", ch, NULL, victim, TO_VICT  );
	act( "`b$n spins $s staff to block your attack.`x", victim, NULL, ch, TO_VICT );
	victim->move = UMAX( 0, victim->move - ((115 - chance) / 15));
	check_improve(victim,gsn_spinblock,true,6);
	return true;
}

/**************************************************************************/
// Check for shield block.
bool check_shield_block( char_data *ch, char_data *victim )
{
	int chance;
	obj_data *shield;
	if ( !IS_AWAKE(victim) )
		return false;
	
	chance = get_skill(victim,gsn_shield_block) / 5 + 3;
	
	
	if ( (shield = get_eq_char( victim, WEAR_SHIELD )) == NULL )
		return false;
	
	if ( number_percent( ) >= chance + victim->level - ch->level )
		return false;

	check_improve(victim,gsn_shield_block,true,6);
	if(shield->ospec_fun == ospec_lookup("ospec_soulscream"))
		return ospec_soulscream_shield(shield, ch);
	else{
		act( "`bYou block $n's attack with your shield.`x", 
			ch, NULL, victim, TO_VICT    );
		act( "`b$n blocks your attack with a shield.`x", 
			victim, NULL, ch, TO_VICT    );
		return true;
	}
}


/**************************************************************************/
// Check for dodge.
bool check_dodge( char_data *ch, char_data *victim )
{
	int chance;
	bool acrobat;
	if ( !IS_AWAKE(victim) )
		return false;
	
	chance = get_skill(victim,gsn_dodge) / 2;
	acrobat = false;
	
	if(!IS_NPC(victim)
	&& get_skill(victim, gsn_acrobatics) > 0
	&& victim->pcdata->tired < 20
	&& victim->move > (victim->move * 2 / 3))
	{
		chance += gsn_acrobatics / 5;
		acrobat = true;
	}
	
	if (!can_see(victim,ch))
		chance /= 2;
	
	if ( number_percent( ) >= chance + victim->level - ch->level )
		return false;
	
	//Evasion
	if(IS_SET(victim->dyn2, DYN2_EVADING)
	&& get_skill(victim, gsn_evasion))
	{
		char_data *pch;
		char_data *pnext;
		for(pch = victim->in_room->people; pch != NULL; pch = pnext)
		{
			pnext = pch->next_in_room;
			if(pch != victim //it's not me
			&& victim->fighting //I'm fighting someone
			&& pch->fighting //they're fighting someone
			&& victim->fighting == pch->fighting) //we're fighting the same person
				break; //this is the person. done.
		}
		if(pch != NULL 
		&& number_percent() <= get_skill(victim, gsn_evasion) / 3)
		{
			act( "`bYou dodge behind $N.`x", pch, NULL, victim, TO_VICT);
			act( "`b$n dodges behind a hapless victim!`x", victim, NULL, ch, TO_VICT);
			check_improve(victim,gsn_evasion,true,6);
			return true;
		}
	}

	//Acrobatics
	if(acrobat == true)
	{
		switch(number_range(1, 4))
		{
			case 1:	act( "`bYou somersault under $n's attack.`x", ch, NULL, victim, TO_VICT);
					act( "`b$n somersaults under your attack!`x", victim, NULL, ch, TO_VICT);
					break;
			case 2:	act( "`bYou spins away from $n's attack.`x", ch, NULL, victim, TO_VICT);
					act( "`b$n spins away from your attack!`x", victim, NULL, ch, TO_VICT);
					break;
			case 3:	act( "`bYou cartwheel past $n's attack.`x", ch, NULL, victim, TO_VICT);
					act( "`b$n cartwheels past your attack!`x", victim, NULL, ch, TO_VICT);
					break;
			case 4:	act( "`bYou flip over $n's attack.`x", ch, NULL, victim, TO_VICT);
					act( "`b$n flips over your attack!`x", victim, NULL, ch, TO_VICT);
					break;
		}
		check_improve(victim,gsn_acrobatics,true,6);
	}else{
	act( "`bYou dodge $n's attack.`x", ch, NULL, victim, TO_VICT);
	act( "`b$n dodges your attack.`x", victim, NULL, ch, TO_VICT);
	}
	check_improve(victim,gsn_dodge,true,6);
	return true;
}

/**************************************************************************/
// Set position of a victim based on their current hp
void update_pos( char_data *victim )
{
    	// Immortals die in Arena & War Battlefield Rooms
 	if (!IS_NPC(victim) && victim->level >= LEVEL_IMMORTAL && victim->hit < 1 &&
	    !IS_SET(victim->in_room->room2_flags,ROOM2_WAR) &&
	    !IS_SET(victim->in_room->room2_flags,ROOM2_ARENA) )
	{
		victim->hit = 1;
	}


	/* JAIL DEATH FIX */
	if ( !IS_NPC(victim) && victim->hit < 1 &&  is_name( victim->in_room->area->file_name, "jailarea.are" )&& 
              victim->in_room->vnum != 10398 )
	{
		victim->hit = 1;
	}


	// GAMESET2_SHOPKEEPERS_CAN_BE_KILLED flag turned off
	if(IS_KEEPER(victim) 
		&& !GAMESETTING2(GAMESET2_SHOPKEEPERS_CAN_BE_KILLED)
		&& victim->hit < 1 )
	{
		victim->hit=20;
		victim->max_hit+=20;
		victim->level+=number_range(2,20);
		victim->highest_level_to_do_damage+=10;
	}

	if ( victim->hit > 0 )
	{
		if ( victim->position <= POS_STUNNED )
			victim->position = POS_STANDING;
		return;
	}
	
	if ( IS_NPC(victim) && victim->hit < 1 )
	{
		victim->position = POS_DEAD;
		return;
    }
	
    if ( victim->hit <= -11 )
    {
		victim->position = POS_DEAD;
		return;
    }
	
	if ( victim->hit <= -6 ) victim->position = POS_MORTAL;
    else if ( victim->hit <= -3 ) victim->position = POS_INCAP;
    else                          victim->position = POS_STUNNED;
	
    return;
}

/**************************************************************************/
// Start fights.
void set_fighting( char_data *ch, char_data *victim )
{
	char		buf[MSL];
	OBJ_DATA	*obj;

    if(ch->fighting)
    {
		bug("Set_fighting: ch already fighting");
		return;
    }
	if(!IS_NPC(ch))
	{
		// wizi imms wont start fighting on area attacks by PC's
		if (!IS_SET(ch->dyn,DYN_DOING_DAMAGE) && INVIS_LEVEL(victim)> ch->level)
		{
			victim->println("You dont return the attack, your wizi above them.");
			return;
		}
		// unseen mobs wont attack the ch
		if (IS_NPC(victim) 	&& !IS_SET(ch->dyn,DYN_DOING_DAMAGE)
			&& IS_SET(victim->act, ACT_IS_UNSEEN)
			&& !HAS_HOLYLIGHT(ch))
		{
			victim->println("You dont return the attack, your UNSEEN to them.");
			return;
		}
	}

	if( is_affected( ch, gsn_sneak )){
		affect_strip( ch, gsn_sneak );
	}

	if( is_affected( victim, gsn_sneak )){
		affect_strip( victim, gsn_sneak );
	}

    if( IS_AFFECTED(ch, AFF_SLEEP) ){
		affect_strip( ch, gsn_sleep );
	}

    ch->fighting = victim;
    ch->position = POS_FIGHTING;

	// Auto draw weapons from sheathed to wield
	if ( get_eq_char( ch, WEAR_WIELD ) == NULL
	&&   (get_eq_char( ch, WEAR_SHEATHED ) != NULL
	||	  get_eq_char( ch, WEAR_CONCEALED) != NULL ))
	{

		if((obj = get_eq_char( ch, WEAR_SHEATHED )) == NULL)
			obj = get_eq_char(ch, WEAR_CONCEALED);
		equip_char( ch, obj, WEAR_WIELD );
		REMOVE_CONFIG2( ch, CONFIG2_SHEATHED );
		act( "You hastily draw $p.", ch, obj, NULL, TO_CHAR );
		act( "$n hastily draws $p.", ch, obj, NULL, TO_ROOM );
		WAIT_STATE( ch, PULSE_VIOLENCE );
	}
	if ( get_eq_char( victim, WEAR_WIELD ) == NULL
	&&   get_eq_char( victim, WEAR_SHEATHED ) != NULL )
	{
		obj = get_eq_char( victim, WEAR_SHEATHED );
		equip_char( victim, obj, WEAR_WIELD );
		REMOVE_CONFIG2( ch, CONFIG2_SHEATHED );
		act( "You hastily draw $p.", victim, obj, NULL, TO_CHAR );
		act( "$n hastily draws $p.", victim, obj, NULL, TO_ROOM );
		WAIT_STATE( victim, PULSE_VIOLENCE );
	}


    if (TRUE_CH(ch)->pcdata && IS_SET(TRUE_CH(ch)->act,PLR_AUTOSUBDUE)){
		SET_BIT(ch->dyn,DYN_CURRENT_SUBDUE);
		ch->println("You are subduing in this fight.");
	}else{
		REMOVE_BIT(ch->dyn,DYN_CURRENT_SUBDUE);
		ch->println("You are fighting to kill in this fight.");
    }

	// who started the fight and pksafeness stuff
	if(IS_SET(ch->dyn,DYN_DOING_DAMAGE) 
		==IS_SET(victim->dyn,DYN_DOING_DAMAGE))
	{
		// check for mobprogs that trigger when someone starts combat 
		// with a spell, and the mobprog triggers a spell in return
		// (this is only in the first round of combat)
		if(IS_SET(ch->dyn,DYN_RUNNING_MOBPROG_CMD)){
			// victim started the fight, ch is mobprog in return
			logf("set_fighting(): ch->dyn,DYN_RUNNING_MOBPROG_CMD is true.");
			logf("set_fighting(): victim (%s) started fight with %s, ch=%s", 
				victim->name, 
				victim->fighting?victim->fighting->name:"victim->fighting==NULL",
				ch->name);
			victim->pksafe=0;
		}else if(IS_SET(victim->dyn,DYN_RUNNING_MOBPROG_CMD)){
			// ch started the fight, victim is mobprog in return
			logf("set_fighting(): victim->dyn,DYN_RUNNING_MOBPROG_CMD is true.");
			logf("set_fighting(): ch (%s) started fight with %s, vict=%s", 
				ch->name, 
				ch->fighting?ch->fighting->name:"ch->fighting==NULL",
				victim->name);
			ch->pksafe=0;
		}else{
			bugf("set_fighting(): ch->dyn,DYN_DOING_DAMAGE "
				"equals victim->dyn,DYN_DOING_DAMAGE)!");
			if(!IS_NPC(ch) && !IS_NPC(victim)){
				do_abort();
			}
		}
	}else if(IS_SET(ch->dyn,DYN_DOING_DAMAGE)){
		// ch started the fight
		if(!IS_NPC(victim) || !IS_NPC(ch->fighting)){
			logf("set_fighting(): ch (%s) started fight with %s, vict=%s", 
				ch->name, ch->fighting->name, victim->name);
			SET_BIT(ch->dyn, DYN_STARTED_FIGHT);
			REMOVE_BIT(victim->dyn, DYN_STARTED_FIGHT);
		}
		ch->pksafe=0;
	}else{
		// victim started the fight
		if(!IS_NPC(ch) || (victim->fighting && !IS_NPC(victim->fighting))){
			logf("set_fighting(): victim (%s) started fight with %s, ch=%s", 
				victim->name, 
				victim->fighting?victim->fighting->name:"victim->fighting==NULL",
				ch->name);
			SET_BIT(victim->dyn, DYN_STARTED_FIGHT);
			REMOVE_BIT(ch->dyn, DYN_STARTED_FIGHT);
		}
		victim->pksafe=0;
	}


	// faults with mobs caused by bad use of olc checks only below here
	// - WILL BE MOVED INTO ASAVE LATER PROBABLY
	// check the level on both parties
	if (victim->level==0)
	{	
		sprintf( buf, "`1Victim = [%5d] %s (%s) `1Room = [%5d] %s"
			"`1Attacker = %s`1"
			"Note - victims level has been set to 90 after the fight",
			IS_NPC(victim) ? victim->pIndexData->vnum : 0,
			victim->short_descr, victim->name,
			victim->in_room->vnum,
			victim->in_room->name, ch->name);

		victim->level=1;
		if (IS_NPC(victim) && victim->pIndexData->level==0)
		{
			victim->pIndexData->level=90;
		}

		autonote(NOTE_SNOTE, "set_fighting()", 
			"victim->level==0", "olc"	, buf, true);

		SET_BIT( victim->pIndexData->area->olc_flags, OLCAREA_CHANGED );
	}
	if (ch->level==0)
	{	
		sprintf( buf, "`1ch = [%5d] %s (%s) `1Room = [%5d] %s"
			"`1Attacker = %s`1"
			"Note - ch level has been set to 90 after the fight",
			IS_NPC(ch) ? ch->pIndexData->vnum : 0,
			ch->short_descr, ch->name,
			ch->in_room->vnum,
			ch->in_room->name, victim->name);

		ch->level=1;
		if (IS_NPC(ch) && ch->pIndexData->level==0)
		{
			ch->pIndexData->level=90;
		}

		autonote(NOTE_SNOTE, "set_fighting()",
			"ch->level==0", "olc"	, buf, true);
		SET_BIT( ch->pIndexData->area->olc_flags, OLCAREA_CHANGED );
	}

	// check the max hp on both parties - note olc if there is a problem
	if (victim->max_hit==0)
	{	
		sprintf( buf, "`1Victim = [%5d] %s (%s) `1Room = [%5d] %s"
			"`1Attacker = %s`1"
			"Note - victims hitdice has been set to 1+1+(level*4) after the fight",
			IS_NPC(victim) ? victim->pIndexData->vnum : 0,
			victim->short_descr, victim->name,
			victim->in_room->vnum,
			victim->in_room->name, ch->name);

		victim->max_hit=1;

		if(IS_NPC(victim))
		{
			victim->pIndexData->hit[DICE_NUMBER] = 1;
			victim->pIndexData->hit[DICE_TYPE]   = 1;
			victim->pIndexData->hit[DICE_BONUS]  = victim->level*4;
		}
		autonote(NOTE_SNOTE, "set_fighting()",
			"victim->max_hit==0", "olc"	, buf, true);

        SET_BIT( victim->pIndexData->area->olc_flags, OLCAREA_CHANGED );
	}
	if (ch->max_hit==0)
	{	
		sprintf( buf, "`1Ch= [%5d] %s (%s) `1Room = [%5d] %s"
			"`1Attacker = %s`1"
			"Note - ch's hitdice has been set to 1+1+(level*4) after the fight",
			IS_NPC(ch) ? ch->pIndexData->vnum : 0,
			ch->short_descr, ch->name,
			ch->in_room->vnum,
			ch->in_room->name, victim->name);

		ch->max_hit=1;

		if(IS_NPC(ch))
		{
			ch->pIndexData->hit[DICE_NUMBER] = 1;
			ch->pIndexData->hit[DICE_TYPE]   = 1;
			ch->pIndexData->hit[DICE_BONUS]  = ch->level*4;
		}

		autonote(NOTE_SNOTE, "set_fighting()", 
			"ch->max_hit==0", "olc"	, buf, true);
		SET_BIT( ch->pIndexData->area->olc_flags, OLCAREA_CHANGED );
	}
    return;
}


/**************************************************************************/
// Stop fights.
void stop_fighting( char_data *ch, bool fBoth )
{
    char_data *fch;

    for ( fch = char_list; fch != NULL; fch = fch->next )
    {
		if ( fch == ch || ( fBoth && fch->fighting == ch ) )
		{
			fch->fighting   = NULL;
			//REMOVE_BIT(fch->dyn,DYN_CURRENT_SUBDUE);
			REMOVE_BIT(fch->dyn,DYN_IS_CUTTING_OFF);
			REMOVE_BIT(fch->dyn2, DYN2_DOING_ORBIT);
			fch->position   = IS_NPC(fch) ? fch->default_pos : POS_STANDING;
			update_pos( fch );
		}
    }

    return;
}

/**************************************************************************/
// Instantly decay a corpse - eg wraith called from make_corpse
void instant_decay_corpse(char_data *ch)
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    act( "`w$n `Ydisappears`w into thin air right before your eyes!", ch, NULL, NULL, TO_ROOM );

    if ( IS_NPC(ch) )
    {
	if ( ch->gold > 0 )
	{
	    obj_to_room( create_money( ch->gold, ch->silver ), ch->in_room );
	    act( "A pile of coins falls to the ground from the space\r\nonce occupied by $n.", ch, NULL, NULL, TO_ROOM );
	    ch->gold = 0;
	    ch->silver = 0;
	}
    }
    else /* a player corpse */
    {
	if (ch->gold > 1 || ch->silver > 1)
	{
	    obj_to_room( create_money( ch->gold/2, ch->silver/2 ), ch->in_room );
	    ch->gold -= ch->gold/2;
	    ch->silver -= ch->silver/2;
	}
    }

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
	bool floating = false;

	obj_next = obj->next_content;

	if (obj->wear_loc == WEAR_FLOAT)
	     floating = true;

	obj_from_char( obj );

	if (obj->item_type == ITEM_POTION)
	     obj->timer = number_range(500,1000);
	if (obj->item_type == ITEM_SCROLL)
	     obj->timer = number_range(1000,2500);

	if (IS_SET(obj->extra_flags,OBJEXTRA_ROT_DEATH) && !floating)
	{
		if (obj->contains != NULL)
		{
			OBJ_DATA *in, *in_next;
	
			act("$p dissolves,scattering its contents.",
			ch,obj,NULL,TO_ROOM);
			for (in = obj->contains; in != NULL; in = in_next)
			{
				in_next = in->next_content;
				obj_from_obj(in);
				obj_to_room(in,ch->in_room);
			}
		 }
		 else
			act("$p dissolves.", ch,obj,NULL,TO_ROOM);
		extract_obj(obj);
	}
	REMOVE_BIT(obj->extra_flags,OBJEXTRA_VIS_DEATH);
    
/*	if ( IS_SET( obj->extra_flags, OBJEXTRA_INVENTORY ) )
	     extract_obj( obj );
	else */

	if (floating)
	{
	    if (IS_OBJ_STAT(obj,OBJEXTRA_ROT_DEATH)) /* get rid of it! */
	    { 
		if (obj->contains != NULL)
		{
		    OBJ_DATA *in, *in_next;
	
		    act("$p evaporates,scattering its contents.",
		    ch,obj,NULL,TO_ROOM);
		    for (in = obj->contains; in != NULL; in = in_next)
		    {
			in_next = in->next_content;
			obj_from_obj(in);
			obj_to_room(in,ch->in_room);
		    }
		 }
		 else
		    act("$p evaporates.", ch,obj,NULL,TO_ROOM);

		 extract_obj(obj);
	    }
	    else
	    {
		act("$p falls to the floor.",ch,obj,NULL,TO_ROOM);
		obj_to_room(obj,ch->in_room);
	    }
	}
	else
	{           
	    act("$p falls to the floor from the space\r\nonce occupied by $n.",ch,obj,NULL,TO_ROOM);
	    obj_to_room( obj, ch->in_room );
	}
    }

    return;

}
/**************************************************************************/
// find the players morgue vnum in order of priority
// 1st - a manually set vnum *
// 2nd - clan morgue *
// 3rd - class morgue point*
// 4th - default racial morgue*
// 5th - gamewide default morgue
// * not yet implemented
int get_morguevnum(char_data *ch)
{
	int vnum=game_settings->roomvnum_morgue;

	if(get_room_index(class_table[ch->clss].morgue)){
		vnum=class_table[ch->clss].morgue;
	} else if(get_room_index(race_table[ch->race]->morgue)){
		vnum=race_table[ch->race]->morgue;
	}
	return vnum;
};
/**************************************************************************/
void do_save_corpses(char_data *ch, char *);
/**************************************************************************/
// Make a corpse out of a character.
void make_corpse( char_data *ch, char_data *killer )
{
	char *killed_by;
	if ( ch == killer ){
		killed_by = "someone";
	}else{
		killed_by = killer->short_descr;
	}

    char buf[MSL];
    OBJ_DATA *corpse;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char *name;
	bool keeper;

	ROOM_INDEX_DATA *target_room=NULL;

	if(GAMESETTING(GAMESET_MORGUE_ENABLED) && !IS_NPC(ch))
	{
		target_room=get_room_index(race_table[ch->race]->death_room);
	}

	if(IS_NPC(ch))
		target_room=ch->in_room;

	if(!target_room)
        {
		target_room=ch->in_room;
		bug("No racial death room found!");
		logf("BUG: No Death/Morgue room found for %s.", TRUE_CH(ch)->name);
	}

	if (IS_THIEF(ch))
		target_room=ch->in_room;

    // reduce money on shopkeeper corpses to stop
    // people killing the shopkeepers for money
	keeper = IS_KEEPER(ch);
    if (keeper)
    {
		if (ch->gold>5 || ch->silver>500 )
		{
			if (ch->gold>5)
			{
				ch->gold= number_range(1,5);
			}
			else
			{
				ch->gold= number_range(1,ch->gold);
			}

			if (ch->silver>500)
			{
				ch->silver=number_range(1,500);
			}
			else
			{
				ch->silver=number_range(1,ch->silver);
			}
		}
    }

    if (IS_SET(ch->form, FORM_INSTANT_DECAY))
    {
		instant_decay_corpse(ch);
		return;
    }

    if (ch->race == race_lookup("flame draconian") || 
        ch->race == race_lookup("bozak draconian") ||
        ch->race == race_lookup("aurak draconian") )
    {	
        char_data *vch;
        char_data *vch_next;
	int dam, level;

	level=ch->level;

	act( "$n explodes in a `#`Rcloud of flame`^!!", ch, NULL, NULL, TO_ROOM );		

        for ( vch = ch->in_room->people; vch; vch = vch_next )
        {
            vch_next = vch->next_in_room;

	    if (vch->in_room == NULL)
		continue;

	    if ( IS_NPC(vch) && IS_SET(vch->act, ACT_IS_UNSEEN))
	    {
		continue;
	    }

	    if ( vch->in_room == ch->in_room )
	    {
		if ( vch != ch)
            	{ 
			dam = dice(level, 6)+10;
			if ( saves_spell( level, vch, DAM_FIRE))
				dam /= 2;
			damage_spell( ch, vch, dam, gsn_fireball, DAM_FIRE,true);
			level -=5;
			if (level < 0)
				break;
		}
	    }
        }
	if (IS_NPC(ch)) {
		instant_decay_corpse(ch);
		return;
	}
    }

    if (ch->race == race_lookup("frost draconian") || 
	ch->race == race_lookup("sivak draconian"))
    {	
        char_data *vch;
        char_data *vch_next;
	int dam, level;

	level=ch->level;

	act( "$n turns to `#`Csolid ice`^ and explodes!!", ch, NULL, NULL, TO_ROOM );		

        for ( vch = ch->in_room->people; vch; vch = vch_next )
        {
            vch_next = vch->next_in_room;

	    if (vch->in_room == NULL)
		continue;

	    if ( IS_NPC(vch) && IS_SET(vch->act, ACT_IS_UNSEEN))
	    {
		continue;
	    }

	    if ( vch->in_room == ch->in_room )
	    {
		if ( vch != ch)
            	{ 
			dam = dice(level, 6)+10;
			if ( saves_spell( level, vch, DAM_COLD))
				dam /= 2;
			damage_spell( ch, vch, dam, gsn_ice_storm, DAM_COLD,true);
			level -=5;
			if (level < 0)
				break;
		}
	    }
        }
	if (IS_NPC(ch)) {
		instant_decay_corpse(ch);
		return;
	}
    }

    if (ch->race == race_lookup("venom draconian") || 
        ch->race == race_lookup("kapak draconian") )
    {	
        char_data *vch;
        char_data *vch_next;
	AFFECT_DATA af;

	act( "$n explodes in a `#`Gcloud of poisonous fumes`^!!", ch, NULL, NULL, TO_ROOM );		

        for ( vch = ch->in_room->people; vch; vch = vch_next )
        {
            vch_next = vch->next_in_room;

	    if (vch->in_room == NULL)
		continue;

	    if ( IS_NPC(vch) && IS_SET(vch->act, ACT_IS_UNSEEN))
	    {
		continue;
	    }

	    if ( vch->in_room == ch->in_room )
	    {
		if ( vch != ch)
            	{ 
			if ( saves_spell( ch->level, vch,DAMTYPE(gsn_poison))
			||   HAS_CLASSFLAG(vch, CLASSFLAG_POISON_IMMUNITY)
			||   IS_SET(vch->imm_flags, IMM_POISON) )
			{
				act("$n turns slightly green, but it passes.",vch,NULL,NULL,TO_ROOM);
				vch->printf( "You feel momentarily ill, but it passes.\r\n" );
			}
			else {
				af.where     = WHERE_AFFECTS;
				af.type      = gsn_poison;
				af.level     = ch->level;
				af.duration  = (ch->level/2);
				af.location  = APPLY_ST;
				af.modifier  = -4;
				af.bitvector = AFF_POISON;
				affect_join( vch, &af );
				vch->printf( "You feel very sick.\r\n" );
				act("$n looks very ill.",vch,NULL,NULL,TO_ROOM);
			}
		}
	    }
        }
	if (IS_NPC(ch)) {
		instant_decay_corpse(ch);
		return;
	}
    }

    if (ch->race == race_lookup("vapor draconian") )
    {	
        char_data *vch;
        char_data *vch_next;
	int dam, level;

	level=ch->level;

	act( "$n explodes in a `#`Sspray of corrosive acid`^!!", ch, NULL, NULL, TO_ROOM );		

        for ( vch = ch->in_room->people; vch; vch = vch_next )
        {
            vch_next = vch->next_in_room;

	    if (vch->in_room == NULL)
		continue;

	    if ( IS_NPC(vch) && IS_SET(vch->act, ACT_IS_UNSEEN))
	    {
		continue;
	    }

	    if ( vch->in_room == ch->in_room )
	    {
		if ( vch != ch)
            	{ 
			dam = dice(level, 6)+10;
			if ( saves_spell( level, vch, DAM_ACID))
				dam /= 2;
			damage_spell( ch, vch, dam, gsn_acid_blast,DAM_ACID,true);
			level -=5;
			if (level < 0)
				break;
		}
	    }
        }
	if (IS_NPC(ch)) {
		instant_decay_corpse(ch);
		return;
	}
    }

    if (ch->race == race_lookup("lightning draconian"))
    {	
        char_data *vch;
        char_data *vch_next;
	int dam, level;

	act( "$n explodes in a `#`Sspray of corrosive acid`^!!", ch, NULL, NULL, TO_ROOM );		

	level=ch->level;

        for ( vch = ch->in_room->people; vch; vch = vch_next )
        {
            vch_next = vch->next_in_room;

	    if (vch->in_room == NULL)
		continue;

	    if ( IS_NPC(vch) && IS_SET(vch->act, ACT_IS_UNSEEN))
	    {
		continue;
	    }

	    if ( vch->in_room == ch->in_room )
	    {
		if ( vch != ch)
            	{ 
			dam = dice(level, 6)+10;
			if ( saves_spell( level, vch, DAM_LIGHTNING))
				dam /= 2;
			damage_spell( ch, vch, dam, gsn_lightning_bolt,DAM_LIGHTNING,true);
			level -=5;
			if (level < 0)
				break;
		}
	    }
        }
	if (IS_NPC(ch)) {
		instant_decay_corpse(ch);
		return;
	}
    }

    if (ch->race == race_lookup("baaz draconian") && killer!=ch)
    {
    	obj_data *weapon;

	if ((weapon = get_eq_char(killer, WEAR_WIELD)) != NULL)
	{
		if (IS_AFFECTED2(killer, AFF2_VICEGRIP))
		{
			act("$n's $p pulls free in the stone corpse of $N.",killer, weapon, ch, TO_ROOM);
			act("$p pulls free as $N turns to stone.",killer, weapon, ch, TO_CHAR);
		} else
		{
			act("$n's $p gets stuck in the stone corpse of $N.",killer, weapon, ch, TO_ROOM);
			act("$p gets stuck as $N turns to stone.",killer, weapon, ch, TO_CHAR);
			obj_from_char( weapon );
			obj_to_room( weapon, ch->in_room );
		}
	}
	if (IS_NPC(ch)) {
		instant_decay_corpse(ch);
		return;
	}
    }

    if ( IS_NPC(ch) )
    {
		name        = ch->short_descr;
		corpse      = create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC));
		corpse->timer   = number_range( 3, 6 );

    		if (IS_SET(ch->form, FORM_NON_SKINABLE))
    		{
			SET_BIT(corpse->extra2_flags, OBJEXTRA2_NON_SKINABLE);
		}

		if(IS_SET(ch->act2, ACT2_CRIME) && !IS_NPC(killer) &&
		   IS_SET(killer->affected_by2, AFF2_BOUNTYHUNTER)) {
			killer->println("`#`YYour bounty hunter license is revoked for your murderous act!!!`^");
			REMOVE_BIT(killer->affected_by2, AFF2_BOUNTYHUNTER);
		}

		if(IS_SET(ch->act2, ACT2_CRIME) && !IS_NPC(killer))
			killer->pcdata->murder += 1;

		if(IS_SET(ch->act2, ACT2_MURDER_5) && !IS_NPC(killer))
			killer->pcdata->murder += 4;

		if(IS_SET(ch->act2, ACT2_MURDER_10) && !IS_NPC(killer))
			killer->pcdata->murder += 9;

		if(IS_SET(ch->act2, ACT2_MURDER_25) && !IS_NPC(killer))
			killer->pcdata->murder += 24;

		if(IS_SET(ch->act2, ACT2_MURDER_50) && !IS_NPC(killer))
			killer->pcdata->murder += 49;

		if(IS_SET(ch->act2, ACT2_MURDER_100) && !IS_NPC(killer))
			killer->pcdata->murder += 99;

		if ( ch->gold > 0 || ch->silver > 0 )
		{
			obj = create_money( ch->gold, ch->silver );
			if(IS_AFFECTED2(ch, AFF2_SHADOWPLANE))
				SET_BIT(obj->extra2_flags, OBJEXTRA2_SHADOWPLANE);
			obj_to_obj( obj, corpse );
			ch->gold = 0;
			ch->silver = 0;
		}

		if (GAMESETTING4(GAMESET4_RANDOM_OBJECTS_ENABLED) &&
		    !IS_SET(ch->act2, ACT2_NO_RANDOM_OBJ))
		{
			OBJ_DATA		*random;
			if ( (number_percent() > 50) && IS_NPC(ch) )
			{
				random = random_object( ch->level );
				obj_to_obj( random, corpse );
			}
		}

		//corpse->cost = 0;
		if(IS_AFFECTED2(ch, AFF2_SHADOWPLANE))
			SET_BIT(corpse->extra2_flags, OBJEXTRA2_SHADOWPLANE);
		corpse->cost = ch->level * number_range(3,4);
		replace_string(corpse->killer, killed_by );
    }
    else // a player corpse
    {
		name        = ch->short_descr;
		if (ch->master){
			stop_follower(ch);
		}
		corpse      = create_object(get_obj_index(OBJ_VNUM_CORPSE_PC));
		corpse->timer   = number_range( 35, 50 );
		corpse->killer = str_dup( killed_by );

		if (!is_clan(ch)){
			replace_string(corpse->owner, ch->name);
		}else{
			corpse->owner = NULL;
			if (ch->gold > 1 || ch->silver > 1)
			{
				obj_to_obj( create_money( ch->gold / 2, ch->silver/2 ), corpse );
				ch->gold -= ch->gold/2;
				ch->silver -= ch->silver/2;
			}
		}    
		corpse->cost = 0;
	}

	corpse->level = 0;

	// remove thief status on death
	if(IS_THIEF(ch)){
		ch->println("You are no longer marked as a thief");
		ch->pcdata->thief_until=0;
	}

	sprintf( buf, corpse->short_descr, name );
	replace_string( corpse->short_descr, buf );
	sprintf( buf, corpse->description, name );
	replace_string( corpse->description, buf);

	if(IS_NPC(ch))
		corpse = shape_corpse(ch, corpse);
	
	//Changes for necromancy
	corpse->value[3] = ch->last_damage[0];
	corpse->value[4] = ch->last_damage[1];

	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
		bool floating = false;

		obj_next = obj->next_content;

		// dont take tokens out of the players inventory
		// extract_char() handles the destruction of 
		// dropdeath tokens (obj->value[0], TOKEN_DROPDEATH)
		if (obj->item_type == ITEM_TOKEN){
			continue;
		}

		if (obj->wear_loc == WEAR_FLOAT){
			 floating = true;
		}

		obj_from_char( obj );
		if(IS_NPC(ch) && IS_AFFECTED2(ch, AFF2_SHADOWPLANE))
			SET_BIT(obj->extra2_flags, OBJEXTRA2_SHADOWPLANE);

		if (obj->item_type == ITEM_POTION){
			 obj->timer = number_range(500,1000);
		}
		if (obj->item_type == ITEM_SCROLL){
			 obj->timer = number_range(1000,2500);
		}

		if (IS_SET(obj->extra_flags,OBJEXTRA_ROT_DEATH) && !floating){
			obj->timer = number_range(5,10);
			REMOVE_BIT(obj->extra_flags,OBJEXTRA_ROT_DEATH);
		}
		REMOVE_BIT(obj->extra_flags,OBJEXTRA_VIS_DEATH);
    
		// shop keepers
		if (keeper){
			if (number_range(1,10)<2){
				obj->timer = number_range(2,5);
			}else{
				extract_obj(obj);
				continue;
			}
		}

		/* if( IS_SET( obj->extra_flags, OBJEXTRA_INVENTORY )){
			 extract_obj( obj );
		}else */{
			if (floating)
			{
				if (IS_OBJ_STAT(obj,OBJEXTRA_ROT_DEATH)) // get rid of it!
				{ 
					if (obj->contains != NULL)
					{
						OBJ_DATA *in, *in_next;
						act("$p evaporates,scattering its contents.", ch,obj,NULL,TO_ROOM);
						for (in = obj->contains; in != NULL; in = in_next)
						{
							in_next = in->next_content;
							obj_from_obj(in);
							obj_to_room(in,target_room);
						}
					}
					else
					{
						act("$p evaporates.", ch,obj,NULL,TO_ROOM);
					}
					extract_obj(obj);
				}
				else
				{
					act("$p falls to the floor.",ch,obj,NULL,TO_ROOM);
					obj_to_room(obj,target_room);
				}
			}
			else
			{
				obj_to_obj( obj, corpse );
			}
		}
	}
    obj_to_room( corpse, target_room );

	// if it is a player, resave the corpses
	if(!IS_NPC(ch)){
		do_save_corpses(NULL, "");
	}
	return;
}
/**************************************************************************/
void make_blood( char_data *ch )
{
   OBJ_DATA *obj;

   obj = create_object( get_obj_index( 1901 ));
   obj->timer = number_range( 2, 4 );
   obj->value[1] = number_range( 3, UMIN( 5, ch->level ) );
   obj_to_room( obj, ch->in_room );
}

/**************************************************************************/
// Improved Death_cry contributed by Diavolo.
void death_cry( char_data *ch, char_data *killer )
{
	ROOM_INDEX_DATA *was_in_room;
    char *msg;
    int door;
    int vnum;
	
    // no death cry at this stage for a vanishing body
    if (IS_SET(ch->form, FORM_INSTANT_DECAY))
    {
		return;
    }
	
	vnum = 0;
	msg = "You hear $n's death cry.";
	
    switch ( number_bits(4))
    {
    case  0: msg  = "$n hits the ground ... DEAD."; 
		break;
    case  1: 
		if (ch->material == 0)
		{
			msg  = "$n splatters blood on your armor.";         
			break;
		}
    case  2:                                                    
		if (IS_SET(ch->parts,PART_GUTS))
		{
			msg = "$n spills $s guts all over the floor.";
			vnum = OBJ_VNUM_GUTS;
		}
		break;
    case  3: 
		if (IS_SET(ch->parts,PART_HEAD))
		{
			msg  = "$n's severed head plops on the ground.";
			vnum = OBJ_VNUM_SEVERED_HEAD;
		}
		break;
    case  4: 
		if (IS_SET(ch->parts,PART_HEART))
		{
			msg  = "$n's heart is torn from $s chest.";
			vnum = OBJ_VNUM_TORN_HEART;                         
		}
		break;
    case  5: 
		if (IS_SET(ch->parts,PART_ARMS))
		{
			msg  = "$n's arm is sliced from $s dead body.";
			vnum = OBJ_VNUM_SLICED_ARM;                         
		}
		break;
	case  6:
		if (IS_SET(ch->parts,PART_LEGS))
		{
			msg  = "$n's leg is sliced from $s dead body.";
			vnum = OBJ_VNUM_SLICED_LEG;                         
		}
		break;
	case 7:
		if (IS_SET(ch->parts,PART_BRAINS))
		{
			msg = "$n's head is shattered, and $s brains splash all over you.";
			vnum = OBJ_VNUM_BRAINS;
		}
    }
	
    if ( vnum != 0 )
    {
		char buf[MSL];
		OBJ_DATA *obj;
		char *name;
		
		name            = ch->short_descr;
		obj             = create_object( get_obj_index( vnum ));
		obj->timer      = number_range( 4, 7 );
		
		sprintf( buf, obj->short_descr, name );
		free_string( obj->short_descr );
		obj->short_descr = str_dup( buf );
		
		sprintf( buf, obj->description, name );
		free_string( obj->description );
		obj->description = str_dup( buf );
		if (!IS_NPC(ch))
			obj->material = ch->name;
		
		if (obj->item_type == ITEM_FOOD)
		{
			if (IS_SET(ch->form,FORM_POISON))
				obj->value[3] = 1;
			else if (!IS_SET(ch->form,FORM_EDIBLE))
				obj->item_type = ITEM_TRASH;
		}

		if(IS_NPC(ch) && IS_AFFECTED2(ch, AFF2_SHADOWPLANE))
			SET_BIT(obj->extra2_flags, OBJEXTRA2_SHADOWPLANE);

		obj_to_room( obj, ch->in_room );
    }

	if ( number_range(1,100) <= get_skill(killer, gsn_stifle ))
	{
		act( "You managed to kill $N before they could let out a final cry.", killer, NULL, ch, TO_CHAR );
		check_improve(ch,gsn_stifle,true,3);
		return;
	}

    act( msg, ch, NULL, NULL, TO_ROOM );


	// if ( IS_NPC(ch) )
	if ( IS_SET( ch->form, FORM_SENTIENT )
		|| !IS_NPC(ch))
		msg = "You hear someone's death cry";		
	else
		msg = "You hear something's death cry";

	
    was_in_room = ch->in_room;

	if (was_in_room==NULL){
		return;
	}

    for ( door = 0; door < MAX_DIR; door++ )
    {
		EXIT_DATA *pexit;
		char tempmsg[MIL];

		sprintf( tempmsg, "%s from %s.", msg, death_cry_dir_name[rev_dir[door]]);
		if ( ( pexit = was_in_room->exit[door] ) != NULL
			&&   pexit->u1.to_room != NULL
			&&   pexit->u1.to_room != was_in_room )
		{
			ch->in_room = pexit->u1.to_room;
			act( tempmsg, ch, NULL, NULL, TO_ROOM );
		}
	}
    ch->in_room = was_in_room;
	
	return;
}


/**************************************************************************/
void raw_kill( char_data *victim, char_data *killer )
{
    int		i;
    stop_fighting( victim, true ); //Raw_kill
    death_cry( victim, killer );

    if(killer->in_room->sector_type)
    {

    	if(  (killer->in_room->sector_type == SECT_UNDERWATER) ||
    	     (killer->in_room->sector_type == SECT_WATER_SWIM) ||
    	     (killer->in_room->sector_type == SECT_WATER_NOSWIM))
    	{
      		act("$n's blood slowly clouds the surrounding water.", victim, NULL, NULL, TO_ROOM );
    	} else if( killer->in_room->sector_type == SECT_AIR ) {
    		act("$n's blood sprays wildly through the air.", victim, NULL, NULL, TO_ROOM );
    	} else {
      		make_blood( victim );
    	}
    } else {
      	make_blood( victim );
    }
	
    if (victim->mounted_on)
		dismount(victim);
	
    if (victim->ridden_by)
    {
		victim->ridden_by->println("Your mount has been killed, you fall to the ground!");
		dismount(victim->ridden_by);
    }

    make_corpse( victim, killer );
    victim->pksafe=30;
	
	if(GAMESETTING5(GAMESET5_DEDICATED_PKILL_STYLE_MUD)){
		victim->pksafe=3;
		if(victim->gold<400){
			victim->gold+=number_range(40,400);
			victim->println("`GYOUR GOLD HAS BEEN BOOSTED UP.`x"); 
		}
		victim->pknoquit=2;
		victim->println("`YYOU ARE PK SAFE FOR 3 MINUTES!!!. PKNOQUIT AT 2MINS :)`x");
	}else{
		victim->pknoquit=0;
	}
    victim->pknorecall=0;
	
    if ( IS_NPC(victim) )
    {
		victim->pIndexData->killed++;
		kill_table[URANGE(0, victim->level, MAX_LEVEL-1)].killed++;
		extract_char( victim, true );
		return;
    }
    
    extract_char( victim, false );   
	while ( victim->affected ){
		affect_remove( victim, victim->affected );
	}
    victim->affected_by = race_table[victim->race]->aff;
	victim->subdued = false;
    for (i = 0; i < 4; i++){
		victim->armor[i]= 100;
	}
    victim->position    = POS_RESTING;
    victim->hit         = UMAX( 1, victim->hit  );
    victim->mana        = UMAX( 1, victim->mana );
	victim->move		= UMAX( 1, victim->move );
    victim->bleeding = 0;
    victim->will_die = 0;
    if( !IS_NPC(victim))
    {
    	victim->pcdata->condition[COND_HUNGER] = 30;
    	victim->pcdata->condition[COND_THIRST] = 30;
	if(IS_SET(victim->affected_by2, AFF2_SHADOWPLANE)) 
	{
		REMOVE_BIT(victim->affected_by2, AFF2_SHADOWPLANE);
		REMOVE_BIT(victim->affected_by2, AFF2_SHADOWSIGHT);
	}
	if(IS_SET(victim->affected_by2, AFF2_ETHEREAL))
		REMOVE_BIT(victim->affected_by2, AFF2_ETHEREAL);
    }

	reset_char(victim); // easiest way to fix problems with death bugs
	landchar(victim);	// make sure they die on the ground
	save_char_obj(victim);
    return;
}
/**************************************************************************/
void group_bypass_killer_penatly(char_data *killer, char_data *victim, bool death)
{
	assert(!IS_NPC(victim));
	bool bypass_duel_used=false;

	char_data *gch;

	if(GAMESETTING2(GAMESET2_BYPASSDUEL_REDUCES_KARNS)){
		for ( gch = killer->in_room->people; gch; gch = gch->next_in_room )
		{
			if ( !IS_NPC(gch) && gch->fighting==victim && is_same_group( gch, killer))
			{

				if(gch->duels){
					if(gch->duels->is_bypassingduel(victim)){
						bool lose_karn=false;
						bypass_duel_used=true;

						if(death){ // lose a karn 
							lose_karn=true;
						}else{ // lose a karn some of the time
							gch->duel_subdues_before_karn_loss--;
							if(gch->duel_subdues_before_karn_loss<0){
								lose_karn=true;
								gch->duel_subdues_before_karn_loss=4;
							}
						}


						OBJ_DATA  *wrist1 = get_eq_char(victim, WEAR_WRIST_L);
						OBJ_DATA  *wrist2 = get_eq_char(victim, WEAR_WRIST_R);

						if (wrist1!=NULL && wrist1->pIndexData->vnum==5097)
						{
							act( "$n's $p dissolves as it absorbs a violent death.", victim, wrist1, NULL, TO_ROOM );
							act( "Your $p dissolves as it absorbs a violent death. You keep a karn!", victim, wrist1, NULL, TO_CHAR );
							extract_obj(wrist1);
							lose_karn=false;
						}
						else
						if (wrist2!=NULL && wrist2->pIndexData->vnum==5097)
						{
							act( "$n's $p dissolves as it absorbs a violent death.", victim, wrist2, NULL, TO_ROOM );
							act( "Your $p dissolves as it absorbs a violent death. You keep a karn!", victim, wrist2, NULL, TO_CHAR );
							extract_obj(wrist2);
							lose_karn=false;
						}

						if(lose_karn){
							gch->pcdata->karns--;
							if(GAMESETTING(GAMESET_NOPERMDEATH) && victim->pcdata->karns<0){
								victim->pcdata->karns=0;
							}
							if(gch->pcdata->karns<0) // if they had a karn to lose
							{
								gch->println(
									"`xFrom within your body you feel a `Rburning sensation`x from within,\r\n"
									"a ball of light departs from your body and disappears!");
							}else{
								gch->println(						
									"`xFrom within your body you feel a `Rburning sensation`x from within,\r\n"
									"you collapse to the ground, and pass out.");

							}
							save_char_obj(gch);
						}
					}
				}

			}
		}
	}

	// tell all those who are required to write a pknote to do so

    if(!IS_SET(killer->in_room->room2_flags,ROOM2_ARENA) &&
       !IS_SET(killer->in_room->room2_flags,ROOM2_WAR) )
    {

	// Highlander Check
	if(IS_AFFECTED2(victim, AFF2_HIGHLANDER) && IS_AFFECTED2(killer, AFF2_HIGHLANDER)) 
	{
		killer->train += 10;
		victim->train -= 10;
		victim->printlnf("You are no longer a Highlander! You lose 10 trains!");
		killer->printlnf("You have absorbed the Highlanders energy and gained 10 trains!");
		REMOVE_BIT( victim->affected_by2, AFF2_HIGHLANDER );
		SET_BIT( victim->affected_by2, AFF2_NO_HIGHLANDER );
		highlander_broadcast(victim, "%s has been `#`YD`ye`YC`ya`YP`yi`YT`ya`YT`ye`YD`^ by %s.", victim->name, killer->name);
	}
	else
	{
		pkill_note_required_message(victim, victim, bypass_duel_used);
		for ( gch = killer->in_room->people; gch; gch = gch->next_in_room )
		{
			if ( !IS_NPC(gch) && gch->fighting==victim && is_same_group( gch, killer))
			{
				pkill_note_required_message(gch, victim, bypass_duel_used);
			}
		}
	}
    }
}
/**************************************************************************/
// calculate the maximum level in a group attacking a victim
int group_max_level_attacking_victim(char_data *killer, char_data *victim )
{
	int max_level = 0;
	char_data *gch;
	for ( gch = killer->in_room->people; gch; gch = gch->next_in_room )
	{
		if ( is_same_group( gch, killer) 
			  && gch->fighting==victim 
			  && max_level<gch->level)
		{
			max_level=gch->level;

		}
	}
	return max_level;
}
/**************************************************************************/
void group_gain( char_data *ch, char_data *victim )
{
	char_data *gch;
	char_data *lch;
	int xp;
	int members;
	int group_levels;
	int max_group_level;
	
	/*
	* Monsters don't get kill xp's or alignment changes.
	* P-killing doesn't help either.
	* Dying of mortal wounds or poison doesn't give xp to anyone!
	*/
	if ( victim == ch )
		return;

	// no xp gained for pkilling
	if(!IS_NPC(victim)){ 
		return;
	}

	if ( !ch || !ch->in_room || !victim)
	{
		if (!ch){
			bug("group_gain(): ch == NULL!");
		}else{
			if (!ch->in_room){
				bug("group_gain(): ch->in_room == NULL!");
			}else{
				bug("group_gain(): victim == NULL!");	
			}
		}
		return;
	}

	
	members = 0;
	group_levels = 0;
	max_group_level= 0; // max level in killer group
	for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
	{
		if ( is_same_group( gch, ch ) )
		{
			if( victim->level<11 && !IS_NPC(ch) && !IS_NPC(victim) 
				&& !IS_LETGAINED(victim) && GAMESETTING_LETGAINING_IS_REQUIRED )
			{
				if(!GAMESETTING5(GAMESET5_DEDICATED_PKILL_STYLE_MUD)){
					gch->pkool+=4000;
				}
			}

			// if it isn't a mount being ridden then include it
			if(!IS_MOUNTED(gch)){
				members++;
				group_levels += IS_NPC(gch) ? gch->level / 2 : gch->level;
				max_group_level=UMAX(max_group_level, gch->level);
			}
		}
	}
	
	if ( members == 0 ){
		bug("Group_gain: members=0.");
		members=1;
		group_levels=ch->level;
		max_group_level=ch->level;
	}
	
	lch = (ch->leader != NULL) ? ch->leader : ch;
	
	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		OBJ_DATA *obj;
		OBJ_DATA *obj_next;
		
		if ( !is_same_group( gch, ch ) || IS_NPC(gch)){
			continue;
		}
		
		/*      Taken out, add it back if you want it
			if ( gch->level - lch->level >= 5 )
			{
			gch->println("You are too high for this group.");
			continue;
			}
			
			  if ( gch->level - lch->level <= -5 )
			  {
			  gch->println("You are too low for this group.");
			  continue;
			  }
		*/
		
		xp = xp_compute( gch, victim, group_levels);

		// Kal Jan 2004, based on Ixliam's code
		if(GAMESETTING5(GAMESET5_ADDICT_XP_BONUS)){
			char_data *tempch, *addict=NULL;
     		for ( tempch= player_list; tempch; tempch= tempch->next_player)
			{
				if ( !IS_IMMORTAL(tempch) 
					&& tempch->level>10
					&& !IS_OOC(tempch)
					&& tempch->idle<2)
				{
					addict=tempch;
				}
			}
			if(xp && addict && gch && addict== gch && dice(1,4)==1) 
			{
				gch->println("`YADDICT EXP BONUS!!!`x");
				// give xp in range (xp+1) and (xp+1)*150% 
				// to the player who has been on the longest
				xp = number_range(xp+1, (xp+1)*3/2); 
			}
		}
		
		if(IS_SET(gch->comm, COMM_BUILDING)){
			gch->printlnf("`=\xa6If you weren't in building mode "
				"you would receive %d xp.`x", xp );
		}else{
			gch->printlnf("`=\xa6You receive %d experience points.`x", xp );
			gain_exp( gch, xp );
		}
		
		if (!IS_IMMORTAL( ch )) {

			for ( obj = ch->carrying; obj != NULL; obj = obj_next )
			{
				obj_next = obj->next_content;
				if ( obj->wear_loc == WEAR_NONE )
					continue;
			
				if ( ( IS_OBJ_STAT(obj, OBJEXTRA_ANTI_EVIL)  && IS_EVIL(ch) )
					||   ( IS_OBJ_STAT(obj, OBJEXTRA_ANTI_GOOD)    && IS_GOOD(ch) )
					||   ( IS_OBJ_STAT(obj, OBJEXTRA_ANTI_NEUTRAL) && IS_NEUTRAL(ch))
					|| ( IS_OBJ2_STAT(obj, OBJEXTRA2_ANTI_CHAOS)   && IS_TEND_CHAOTIC(ch))
					|| ( IS_OBJ2_STAT(obj, OBJEXTRA2_ANTI_LAW)     && IS_TEND_LAWFUL(ch))
					|| ( IS_OBJ2_STAT(obj, OBJEXTRA2_ANTI_BALANCE) && IS_TEND_NEUTRAL(ch))) 
				{
					act( "You are zapped by $p.", ch, obj, NULL, TO_CHAR );
					act( "$n is zapped by $p.",   ch, obj, NULL, TO_ROOM );
					obj_from_char( obj );
					obj_to_room( obj, ch->in_room );
				}

				if(race_can_wear(ch, obj) == false)
				{
					act( "$p doesn't appear to fit you any longer.", ch, obj, NULL, TO_CHAR );
					act( "$n appears unable to wear $p any longer.",   ch, obj, NULL, TO_ROOM );
					obj_from_char( obj );
					obj_to_room( obj, ch->in_room );
				}
			}
		}
	}
	return;
}

/**************************************************************************/
/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 */
int xp_compute( char_data *gch, char_data *victim, int total_levels)
{
	char buf[MSL];
	int xp,base_exp;
	int level_range;
	int level;
	int lower_xp_level;

	// abort the whole process if we can
	if(	victim->no_xp	// mob previously subdued
		|| IS_NPC(gch)  // mobs dont get xp
		|| gch->pcdata->xp_penalty>0  // player still has an outstanding xp penalty
		|| IS_AFFECTED(gch, AFF_CHARM)){ // charmed player
		return 0;
	}

	level=gch->level; 
	lower_xp_level=victim->highest_level_to_do_damage-8;
	if(level<lower_xp_level){
		level= lower_xp_level + (lower_xp_level - level);
	}

	level_range = victim->level - level;
	
	// compute the base exp 
	switch (level_range)
	{
	default :	base_exp =  0;	break;
	case -9 :	base_exp =  1;	break;
	case -8 :	base_exp =  2;	break;
	case -7 :	base_exp =  5;	break;
	case -6 :	base_exp = 10;	break;
	case -5 :	base_exp = 12;	break;
	case -4 :	base_exp = 13;	break;
	case -3 :	base_exp = 15;	break;
	case -2 :	base_exp = 17;	break;
	case -1 :	base_exp = 19;	break;
	case  0 :	base_exp = 20;	break;
	case  1 :	base_exp = 25;	break;
	case  2 :	base_exp = 55;	break;
	case  3 :	base_exp = 75;	break;
	case  4 :	base_exp = 80;	break;
	}

	if (level_range > 4){
		base_exp = 80 + 7 * (level_range - 4);
	}

	xp = base_exp;

	// calculate exp multiplier 
	if(IS_NPC(victim)){
		xp = xp * victim->pIndexData->xp_mod / 100;
	}


	// do alignment computations 
	if (IS_GOOD(gch)){
		if (IS_EVIL(victim))
			xp*=14/10;
		else if (IS_NEUTRAL(victim))
			xp*=12/10;
	}else if (IS_NEUTRAL(gch)){
		if (IS_EVIL(victim))
			xp*=12/10;
		else if (IS_GOOD(victim))
			xp*=12/10;
	}else if (IS_EVIL(gch)){
		if (IS_NEUTRAL(victim))
			xp*=115/100;
		else
			if (IS_GOOD(victim))
				xp*=13/10;
	}
	
	// more exp at the low levels 
	if (level < 6){
		xp = 10 * xp / (level + 2);
	}
	// reduced xp percent system
	if (!IS_NPC(gch) && gch->pcdata->reduce_xp_percent>0)
	{
		xp= (xp *gch->pcdata->reduce_xp_percent)/100;
	}

	// global xp scaling system
	if(game_settings->global_xp_scale_value!=100){
		xp=xp*game_settings->global_xp_scale_value/100;
	}
	
	// randomize the rewards 
	xp = number_range(xp * 3/4, xp);
	// adjust for grouping
	xp = ( 6 * xp * level/( UMAX(1,total_levels -1) ) ) / 5;

	// different xp scales for letgained and non-letgained
	if(IS_LETGAINED(gch) || GAMESETTING(GAMESET_NO_LETGAINING_REQUIRED)){
		if(level<5)
			xp=xp*45/10;
		else if(level<10)
			xp=xp*275/100;
		else if(level<15)
			xp=xp*2;
		else if(level<20)
			xp=xp*14/10;
		else if(level<25)
			xp=xp*12/10;
	}else{
		if(level<5)
			xp=xp*275/100;
		else if(level<10)
			xp=xp*2;
		else if(level<15)
			xp=xp*14/10;
		else if(level<20)
			xp=xp*12/10;
	}

	// less XP the higher the level over 45
	if (level > 45 ){
		xp =  25 * xp / (level - 20);
	}

	// reduce xp for 40mins after a level is gained by upto 40%
	if(!IS_NPC(gch))
	{
		if(level>15)
		{
			int playedtime=GET_SECONDS_PLAYED(gch);
			int playval=playedtime-gch->pcdata->last_level;
			if(playval<2400) // 40 Minutes
			{
				playedtime=2400-playval; // number of seconds before 40mins after last level 
				float less_percent=	1- ((float)playedtime/6000);
				float newxp=(float)xp*less_percent;
				
				if(newxp>(float)xp)
				{
					bugf("xp_compute: newxp=%f, xp=%f, lvl=%d, pv=%d, pt=%d",
						newxp, (float)xp, level, playval, playedtime);
				}
				else
				{
					xp=(int)newxp;
				}
			}
		}	
	}
	
	// big XP notification
	if((xp>150-level) && (level>10) && (victim->level>12))
	{
		sprintf(buf, "BIG EXP: %s <%d> gained %d xp by killing %s [vnum: %d, lvl: %d]",
			gch->name,
			level,
			xp,
			victim->short_descr,
			(victim->pIndexData) ? victim->pIndexData->vnum : 0,
			victim->level);
		log_string(buf);
		wiznet(buf,gch,NULL,WIZ_SECURE,0,get_trust(gch));
		
		/* if((victim->pIndexData != NULL) && (victim->pIndexData->level>9) )
			victim->pIndexData->level-=2; */
	}
	
	// log their xp
	if (!IS_NPC(gch) && xp>0 && IS_SET(gch->act, PLR_LOG))
	{
		sprintf(buf, "->>>>>>>>>EXP LOG: %s gained %d xp by killing %s [vnum: %d]",
			gch->name,
			xp,
			victim->short_descr,
			(victim->pIndexData) ? victim->pIndexData->vnum : 0);
		append_playerlog( gch, buf);
	}
	return xp;
}

/**************************************************************************/
void dam_message( char_data *ch, char_data *victim,int dam,int dt,bool immune )
{
	char buf1[256], buf2[256], buf3[256];
	char bf[MSL];
	const char *vs;
	const char *vp;
	const char *attack="unset_attack_value_in_dam_message";
	char punct;

	if (ch == NULL || victim == NULL)
		return;

    if(GAMESETTING3(GAMESET3_STORM_DAMAGE_MESSAGES)){
        if( dam > victim->max_hit ) { vs="`SSLAYS`x"; vp="`SSLAYS`x";}
        else if( dam == 0 ) { vs="`Gmiss"; vp="`Gmisses";}
        else if( dam <= 1 ) { vs="`Bscratch"; vp="`Bscratches";}
        else if( dam <= 2 ) { vs="`Bbruise"; vp="`Bbruises";}
        else if( dam <= 3 ) { vs="`Bgraze"; vp="`Bgrazes";}
        else if( dam <= 4 ) { vs="`rhit"; vp="`rhits"; }
        else if( dam <= 5 ) { vs="`rinjure"; vp="`rinjures";}
        else if( dam <= 6 ) { vs="`rwound"; vp="`rwounds";}
        else if( dam <= 7 ) { vs="`rscar"; vp="`rscars";}
        else if( dam <= 8 ) { vs="`rmangle"; vp="`rmangles";}
        else if( dam <= 9 ) { vs="`Rmaul"; vp="`Rmauls";}
        else if( dam <= 10 ) { vs="`RDECIMATE"; vp="`RDECIMATES";}
        else if( dam <= 12 ) { vs="`RDEVASTATE"; vp="`RDEVASTATES";}
        else if( dam <= 14 ) { vs="`RMAIM"; vp="`RMAIMS";}
        else if( dam <= 16 ) { vs="`RCRIPPLE"; vp="`RCRIPPLES";}
        else if( dam <= 18 ) { vs="`RMUTILATE"; vp="`RMUTILATES";}
        else if( dam <= 20 ) { vs="`RDISMEMBER"; vp="`RDISMEMBERS";}
        else if( dam <= 22 ) { vs="`RDISEMBOWEL"; vp="`RDISEMBOWELS";}
        else if( dam <= 24 ) { vs="`RMASSACRE"; vp="`RMASSACRES";}
        else if( dam <= 26 ) { vs="`RVICTIMIZE"; vp="`RVICTIMIZES";}
        else if( dam <= 28 ) { vs="`RTEAR INTO"; vp="`RTEARS INTO";}
        else if( dam <= 30 ) { vs="`ROBLITERATE"; vp="`ROBLITERATES";}
        else if( dam <= 32 ) { vs="`YBLAST"; vp="`YBLASTS";}
        else if( dam <= 34 ) { vs="`R-= BUTCHER =-"; vp="`R-= BUTCHERS =-";}
        else if( dam <= 36 ) { vs="`W-=* OVERWHELM *=-"; vp="`W -=* OVERWHELMS *=-";}
        else if( dam <= 38 ) { vs="`r***`RDEMOLISH`r***"; vp="`r***`RDEMOLISHES`r***";}
        else if( dam <= 40 ) { vs="`r****`R SHRED `r****"; vp="`r**** `RSHREDS `r****";}
        else if( dam <= 43 ) { vs="`c-=`C*`c=- `CDESTROY `c-=`C*`c=-"; vp="`c-=`C*`c=- `CDESTROYS `c-=`C*`c=-";}
        else if( dam <= 47 ) { vs="`R*** `BPULVERIZE `R***"; vp="`R*** `BPULVERIZES `R***";}
        else if( dam <= 50 ) { vs="`c-== `WVAPORIZE `c==-"; vp="`c-== `WVAPORIZES `c==-";}
        else if( dam <= 53 ) { vs="`c-=`W*`c= `RSMITE `c=`W*`c=-"; vp="`c-=`W*`c= SMITES `c=`W*`c=-";}
        else if( dam <= 57 ) { vs="`R>>> RIP APART <<<"; vp="`R>>> RIPS APART <<<";}
        else if( dam <= 60 ) { vs="`R<`r-`R=`r-`R> `WTORMENT `R<`r-`R=`r-`R>"; vp="`R<`r-`R=`r-`R> `WTORMENTS `R<`r-`R=`r-`R>";} 
        else if( dam <= 65 ) { vs="`R<`r-`R=`r-`R> `BRAVAGE `R<`r-`R=`r-`R>"; vp="`R<`r-`R=`r-`R> `BRAVAGES `R<`r-`R=`r-`R>";}
        else if( dam <= 70 ) { vs="`G<> `RDISFIGURE `G<>"; vp="`G<> `RDISFIGURES `G<>";}
        else if( dam <= 75 ) { vs="`R<`c*`R> `rFISSURE `R<`c*`R>"; vp="`R<`c*`R> `rFISSURES `R<`c*`R>";}
        else if( dam <= 80 ) { vs="`b<><><> `YSUNDER `b<><><>"; vp="`b<><><> `YSUNDERS `b<><><>";}
        else if( dam <= 85 ) { vs="`R<*><*> `GWASTE `R<*><*>"; vp="`R<*><*> `GWASTES `R<*><*>";}
        else if( dam <= 90 ) { vs="`RA`rN`RN`rI`RH`rI`RL`rA`RT`rE"; vp="`RA`rN`RN`rI`RH`rI`RL`rA`RT`rE`RS";}
        else if( dam <= 95 ) { vs="`WA`sSSASSINATE"; vp="`WA`SSSASSINATES";}
        else if( dam <= 100) { vs="`R</\\> EVISCERATE </\\>"; vp="`R</\\> EVISCERATES </\\>";}
        else if( dam <= 110) { vs="`B</^\\> `REXTERMINATE `B</^\\>"; vp="`B</^\\> `REXTERMINATES `B</^\\>";}
        else if( dam <= 120) { vs="`B<=-=> `cERADICATE `B<=-=>"; vp="`B<=-=> `cERADICATES `B<=-=>";}
        else if( dam <= 130) { vs="`R+`r.`R+`r.`R+ `RC`rR`RU`rC`RI`rF`RY `R+`r.`R+`r.`R+"; vp="`R+`r.`R+`r.`R+ `RC`rR`RU`rC`RI`rF`RI`rE`RS `R+`r.`R+`r.`R+";}
        else if( dam <= 140) { vs="`WSLAUGHTER"; vp="`WSLAUGHTERS";}
        else if( dam <= 150) { vs="`YNEUTRALIZE"; vp="`YNEUTRALIZES";}
        else if( dam <= 160) { vs="`RNULLIFY"; vp="`RNULLIFIES";}
        else if( dam <= 170) { vs="`BENGULF"; vp="`BENGULFS";}
        else if( dam <= 180) { vs="`RL`rA`RY `RW`rA`RS`rT`RE `rT`RO"; vp="`RL`rA`RY`rS `RW`rA`RS`rT`RE `RT`rO";}
        else if( dam <= 190) { vs="`BW`bR`BE`bA`BK `bH`BA`bV`BO`bC `BU`bP`BO`bN"; vp="`BW`bR`BE`bA`BK`bS `BH`bA`BV`BO`BC `BU`bP`BO`bN";}
        else if( dam <= 200) { vs="`CC`cO`wM`WP`CL`cE`wT`WE`CL`cY `wR`WU`CI`cN"; vp="`CC`cO`wM`WP`CL`cE`wT`WE`CL`cY `wR`WU`CI`cN`wS";}
        else if( dam <= 210) { vs="`WD`wO `WU`wN`WS`wP`WE`wA`WK`wA`WB`wL`WE `WT`wH`WI`wN`WG`wS `WT`wO"; vp="`WD`wO`WE`wS `WU`wN`WS`wP`WE`wA`WK`wA`WB`wL`WE `WT`wH`WI`wN`WG`wS `WT`wO";}
        else  { vs="`WD`wO `RILLEGAL `WT`wH`WI`wN`WG`wS `WT`wO"; vp="`WD`wO`WE`wS `RILLEGAL `WT`wH`WI`wN`WG`wS `WT`wO";}
	}else{
		if ( dam > victim->max_hit ) { vs = "SLAY"; vp = "SLAYS";			}
		else if ( dam == 0  ) { vs = "miss";		vp = "misses";			}
		else if ( dam <= 2  ) { vs = "scratch";		vp = "scratches";		}
		else if ( dam <= 4  ) { vs = "graze";		vp = "grazes";			}
		else if ( dam <= 6  ) { vs = "hit";			vp = "hits";			}
		else if ( dam <= 8  ) { vs = "injure";		vp = "injures"; 		}
		else if ( dam <= 10 ) { vs = "wound";		vp = "wounds";			}
		else if ( dam <= 12 ) { vs = "maul";		vp = "mauls";			}
		else if ( dam <= 14 ) { vs = "decimate";	vp = "decimates";		}
		else if ( dam <= 16 ) { vs = "devastate";	vp = "devastates";		}
		else if ( dam <= 20 ) { vs = "maim";		vp = "maims";			}
		else if ( dam <= 25 ) { vs = "MUTILATE";	vp = "MUTILATES";		}
		else if ( dam <= 30 ) { vs = "DISEMBOWEL";	vp = "DISEMBOWELS"; 	}
		else if ( dam <= 40 ) { vs = "DISMEMBER";	vp = "DISMEMBERS";		}
		else if ( dam <= 50 ) { vs = "MASSACRE";	vp = "MASSACRES";		}
		else if ( dam <= 60 ) { vs = "MANGLE";		vp = "MANGLES"; 		}
		else if ( dam <= 70 ) { vs = "*** DEMOLISH ***";
					  vp = "*** DEMOLISHES ***";							}
		else if ( dam <=  80 ) { vs = "*** DEVASTATE ***";
					 vp = "*** DEVASTATES ***";		 						}
		else if ( dam <= 100)  { vs = "=== OBLITERATE ===";
					 vp = "=== OBLITERATES ===";							}
		else if ( dam <= 125)	{ vs = ">>> ANNIHILATE <<<";
					 vp = ">>> ANNIHILATES <<<";							}
		else if ( dam <= 150)  { vs = "<<< ERADICATE >>>";
					 vp = "<<< ERADICATES >>>"; 							}
		else				   { vs = "do UNSPEAKABLE things to";
					 vp = "does UNSPEAKABLE things to"; 					}
	}

	char vsbuf[MSL], vpbuf[MSL];
	sprintf(vsbuf, "`#%s`^", vs);
	sprintf(vpbuf, "`#%s`^", vp);
	vs=vsbuf;
	vp=vpbuf;

    punct   = (dam <= 24) ? '.' : '!';

    if ( dt == TYPE_HIT )
    {
		if (ch  == victim)
		{
			sprintf( buf1, "$n %s $melf%c",vp,punct);
			sprintf( buf2, "You %s yourself%c",vs,punct);
		}
		else
		{
			 sprintf( buf1, "$n %s $N%c",  vp, punct );
			 sprintf( buf2, "You %s $N%c", vs, punct );
			 sprintf( buf3, "$n %s you%c", vp, punct );
		}
	}
	else
	{
		if ( dt >= 0 && dt < MAX_SKILL )
		{
			attack = skill_table[dt].noun_damage;
		}
		else 
		{
			if ( dt >= TYPE_HIT
				&& dt <= TYPE_HIT + MAX_DAMAGE_MESSAGE)
			{
				attack = attack_table[dt - TYPE_HIT].noun;
			}else{
				 bugf( "Dam_message: bad dt %d.", dt );
				 dt  = TYPE_HIT;
				 attack  = attack_table[0].name;
			}
		}

		if(attack==NULL){
			bugf("dam_message(): NULL attack value... dt=%d, TYPE_HIT=%d, ch=%s, victim=%s", 
				dt, TYPE_HIT, PERS(ch, NULL), PERS(victim, NULL));
		}

		if (immune)
		{
			if (ch == victim)
			{
				sprintf(buf1,"$n is unaffected by $s own %s.",attack);
				sprintf(buf2,"Luckily, you are immune to that.");
			} 
			else
			{
				sprintf(buf1,"$N is unaffected by $n's %s!",attack);
				sprintf(buf2,"$N is unaffected by your %s!",attack);
				sprintf(buf3,"$n's %s is powerless against you.",attack);
			}
		}
		else
		{
			if (ch == victim)
			{
				sprintf( buf1, "$n's %s %s $m%c",attack,vp,punct);
				sprintf( buf2, "Your %s %s you%c",attack,vp,punct);
			}
			else
			{
				sprintf( buf1, "$n's %s %s $N%c",  attack, vp, punct );
				sprintf( buf2, "Your %s %s $N%c",  attack, vp, punct );
				sprintf( buf3, "$n's %s %s you%c", attack, vp, punct );
			}
		}
	}

	// do autodamage for the attacker
	if(dam && !GAMESETTING2(GAMESET2_NO_AUTODAMAGE_COMMAND) 
		&& HAS_CONFIG2(ch, CONFIG2_AUTODAMAGE))
	{
		strcat(buf2, FORMATF(" [%d]", dam));
	}

	// display the damage message
    if (ch == victim)
    {
		act(buf1,ch,NULL,NULL,TO_ROOM);

		act(buf2,ch,NULL,NULL,TO_CHAR);
    }
    else
    {
		act( buf1, ch, NULL, victim, TO_NOTVICT );
		sprintf(bf,"`g%s`x", buf2);
		act( bf, ch, NULL, victim, TO_CHAR );

		// do autodamage for the victim
		if(dam && !GAMESETTING2(GAMESET2_NO_AUTODAMAGE_COMMAND) 
			&& HAS_CONFIG2(victim, CONFIG2_AUTODAMAGE))
		{
			strcat(buf3, FORMATF(" [%d]", dam));
		}

		sprintf(bf,"`R%s`x",buf3);
		act( bf, ch, NULL, victim, TO_VICT );
    }
    return;
}


/**************************************************************************/
/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm( char_data *ch, char_data *victim )
{
	OBJ_DATA *obj;
	
	if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
	if ( ( obj = get_eq_char( victim, WEAR_SECONDARY) ) == NULL )

	{
		if ( ( obj = get_eq_char( victim, WEAR_SECONDARY) ) == NULL ){
			
			return;
		}
	}
	
	if ( IS_OBJ_STAT(obj,OBJEXTRA_NOREMOVE))
	{
		act("$S weapon won't budge!",ch,NULL,victim,TO_CHAR);
		act("$n tries to disarm you, but your weapon won't budge!",
			ch,NULL,victim,TO_VICT);
		act("$n tries to disarm $N, but fails.",ch,NULL,victim,TO_NOTVICT);
		return;
	}
	
    act( "$n DISARMS you and sends your weapon flying!",
		ch, NULL, victim, TO_VICT    );
    act( "You disarm $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n disarms $N!",  ch, NULL, victim, TO_NOTVICT );
	
    obj_from_char( obj );
    if ( IS_OBJ_STAT(obj,OBJEXTRA_NODROP) || IS_OBJ_STAT(obj,OBJEXTRA_INVENTORY) )
		obj_to_char( obj, victim );
    else
    {
		obj_to_room( obj, victim->in_room );
		if (IS_NPC(victim) && victim->wait == 0 && can_see_obj(victim,obj))
			get_obj(victim,obj,NULL);
    }
	
    return;
}

/**************************************************************************/
void entangle( char_data *ch, char_data *victim )
{
	OBJ_DATA *obj;
	
	if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
	if ( ( obj = get_eq_char( victim, WEAR_SECONDARY) ) == NULL )
	{
		if ( ( obj = get_eq_char( victim, WEAR_SECONDARY) ) == NULL ){
			return;
		}
	}
	
	if ( IS_OBJ_STAT(obj,OBJEXTRA_NOREMOVE))
	{
		act("$S weapon won't budge!",ch,NULL,victim,TO_CHAR);
		act("$n tries to entangle your weapon but your weapon won't budge!",ch,NULL,victim,TO_VICT);
		act("$n tries to entangle $N's weapon, but fails.",ch,NULL,victim,TO_NOTVICT);
		return;
	}
	
    act( "$n `Bentangles`x $p and grabs it!!",	ch, obj, victim, TO_VICT    );
    act( "You `Bentangle`x $N's weapon!",  ch, NULL, victim, TO_CHAR    );
    act( "$n `Bentangles`x $N's weapon!",  ch, NULL, victim, TO_NOTVICT );
	
    obj_from_char( obj );
    if ( IS_OBJ_STAT(obj,OBJEXTRA_NODROP) || IS_OBJ_STAT(obj,OBJEXTRA_INVENTORY) )
		obj_to_char( obj, victim );
    else
    {
		obj_to_char( obj, ch );
    }
    return;
}

/**************************************************************************/
void do_berserk( char_data *ch, char *)
{
	int chance, hp_percent;
	
	if ((chance = get_skill(ch,gsn_berserk)) == 0
		||  (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_BERSERK))
		||  (!IS_NPC(ch)
		&&   ch->level < skill_table[gsn_berserk].skill_level[ch->clss]))
	{
		ch->println("You turn red in the face, but nothing happens.");
		return;
	}
	
	if (IS_AFFECTED(ch,AFF_BERSERK) || is_affected(ch,gsn_berserk)
		||  is_affected(ch,gsn_frenzy))
	{
		ch->println("You get a little madder.");
		return;
	}
	
	if (IS_AFFECTED(ch,AFF_CALM))
	{
		ch->println("You're feeling to mellow to berserk.");
		return;
	}
	
    if (ch->mana < 50)
    {
		ch->println("You can't get up enough energy.");
		return;
    }
	
    /* modifiers */
	
    /* fighting */
    if (ch->position == POS_FIGHTING)
		chance += 10;
	
    /* damage -- below 50% of hp helps, above hurts */
    hp_percent = 100 * ch->hit/ch->max_hit;
    chance += 25 - hp_percent/2;
	
    if (number_percent() < chance)
    {
		AFFECT_DATA af;
		
		WAIT_STATE(ch,PULSE_VIOLENCE);
		ch->mana -= 50;
		ch->move /= 2;
		
		/* heal a little damage */
		ch->hit += ch->level * 2;
		ch->hit = UMIN(ch->hit,ch->max_hit);
		
		ch->println("Your pulse races as you are consumed by rage!");
		act("$n gets a wild look in $s eyes.",ch,NULL,NULL,TO_ROOM);
		check_improve(ch,gsn_berserk,true,2);
		
		af.where        = WHERE_AFFECTS;
		af.type         = gsn_berserk;
		af.level        = ch->level;
		af.duration     = number_fuzzy(ch->level / 8);
		af.modifier     = UMAX(1,ch->level/5);
		af.bitvector    = AFF_BERSERK;
		
		af.location     = APPLY_HITROLL;
		affect_to_char(ch,&af);
		
		af.location     = APPLY_DAMROLL;
		affect_to_char(ch,&af);
		
		af.modifier     = UMAX(10,10 * (ch->level/5));
		af.location     = APPLY_AC;
		affect_to_char(ch,&af);
    }
	
    else
    {
		WAIT_STATE(ch,3 * PULSE_VIOLENCE);
		ch->mana -= 25;
		ch->move /= 2;
		
		ch->println("Your pulse speeds up, but nothing happens.");
		check_improve(ch,gsn_berserk,false,2);
	}
}

/**************************************************************************/
void do_bash( char_data *ch, char *argument )
{
	char arg[MIL];
	char_data *victim;
	int chance;
	int dr; // damage result
	
	one_argument(argument,arg);
	
	if ( (chance = get_skill(ch,gsn_bash)) == 0
		||      (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_BASH))
		||      (!IS_NPC(ch)
		&&       ch->level < skill_table[gsn_bash].skill_level[ch->clss]))
	{
		if (!IS_CONTROLLED(ch))
		{
			ch->println("Bashing? What's that?");
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
	
	if (victim->position < POS_FIGHTING)
	{
		act("You'll have to let $M get back up first.",ch,NULL,victim,TO_CHAR);
		return;
	}

	if (victim == ch)
	{
		ch->println("You try to bash your brains out, but fail.");
		return;
	}

	if (IS_AFFECTED(victim, AFF_FLYING) && !IS_AFFECTED(ch, AFF_FLYING))
	{
		ch->println("You can't reach them since they are flying and you are not.");
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
	
    // modifiers 
	
    // size  and weight 
    chance += ch->carry_weight / 250;
    chance -= victim->carry_weight / 200;
	
    if (ch->size < victim->size){
		chance += (ch->size - victim->size) * 15;
    }else{
		chance += (ch->size - victim->size) * 10; 
	}
	
	
    // stats 
    chance += ch->modifiers[STAT_ST];
    chance -= victim->modifiers[STAT_QU];
    chance -= GET_AC(victim,AC_BASH) /25;
    // speed
    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
		chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
		chance -= 30;
	
    // level 
    chance += (ch->level - victim->level);
	
    if (IS_CONTROLLED(ch)){
        chance*=3;
    };
	
    if (!IS_NPC(victim) 
		&& chance < get_skill(victim,gsn_dodge) )
		{   /*
			act("$n tries to bash you, but you dodge it.",ch,NULL,victim,TO_VICT);
			act("$N dodges your bash, you fall flat on your face.",ch,NULL,victim,TO_CHAR);
			WAIT_STATE(ch,skill_table[gsn_bash].beats);
		return;*/
		chance -= 3 * (get_skill(victim,gsn_dodge) - chance);
    }
	
    // now the attack 
    if (number_percent() < chance )
    {	
		DAZE_STATE(victim, 3 * PULSE_VIOLENCE);
		WAIT_STATE(ch,skill_table[gsn_bash].beats);
		victim->position = POS_RESTING;
		dr=damage(ch,victim,number_range(2,2 + 2 * ch->size + chance/20),gsn_bash,
			DAM_BASH,false);

		if(dr){
			act("$n sends you sprawling with a powerful bash!$t", 
				ch, autodamtext(victim, dr) ,victim,TO_VICT);
			act("You powerfully slam into $N, and send $M flying!$t",
				ch, autodamtext(ch, dr) ,victim,TO_CHAR);
			act("$n sends $N sprawling with a powerful bash.",
				ch,NULL,victim,TO_NOTVICT);
		}else{
			act("$n sends you sprawling with a bash!$t",
				ch, autodamtext(victim, dr),victim,TO_VICT);
			act("You slam into $N, and send $M flying!$t",
				ch, autodamtext(ch, dr) ,victim,TO_CHAR);
			act("$n sends $N sprawling with a bash.",
				ch,NULL,victim,TO_NOTVICT);
		}
		check_improve(ch,gsn_bash,true,1);
    }
	else
	{
		damage(ch,victim,0,gsn_bash,DAM_BASH,false);
		act("You fall flat on your face!", ch,NULL,victim,TO_CHAR);
		act("$n falls flat on $s face.", ch,NULL,victim,TO_NOTVICT);
		act("You evade $n's bash, causing $m to fall flat on $s face.",
			ch,NULL,victim,TO_VICT);
		check_improve(ch,gsn_bash,false,1);
		ch->position = POS_RESTING;
		WAIT_STATE(ch,skill_table[gsn_bash].beats * 3/2); 
    }
}

/**************************************************************************/
void do_dirt( char_data *ch, char *argument )
{
    char arg[MIL];
    char_data *victim;
    int chance;
    int size_mod;

    one_argument(argument,arg);

    if ( (chance = get_skill(ch,gsn_dirt_kicking)) == 0
    ||   (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_KICK_DIRT))
    ||   (!IS_NPC(ch)
    &&    ch->level < skill_table[gsn_dirt_kicking].skill_level[ch->clss]))
    {
		ch->println("You get your feet dirty.");
		return;
    }

    if (arg[0] == '\0')
    {
		victim = ch->fighting;
		if (victim == NULL)
		{
		    ch->println("But you aren't in combat!");
		    return;
		}
    }
	else if ((victim = get_char_room(ch,arg)) == NULL)
	{
		ch->println("They aren't here.");
		return;
    }

	if (IS_AFFECTED(victim,AFF_BLIND))
	{
		act("$E's already been blinded.",ch,NULL,victim,TO_CHAR);
		return;
	}

	if (IS_AFFECTED( ch, AFF_FLYING))
	{
		ch->println("You can't dirt kick while you fly.");
		return;
	}

	if (victim == ch)
    {
		ch->println("Very funny.");
		return;
    }

    if (is_safe(ch,victim))
		return;

	if ( !can_initiate_combat( ch, victim, CIT_GENERAL )) return;

    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
		act("But $N is such a good friend!",ch,NULL,victim,TO_CHAR);
		return;
    }

    /* modifiers */

    /* dexterity */
    chance += ch->modifiers[STAT_QU];
    chance -= 2 * victim->modifiers[STAT_QU];
    
    /* mount size*/
	if (victim->mounted_on)
	{
		size_mod = (victim->mounted_on->size)-(ch->size)+1; 
		if (size_mod<=0)
			size_mod=1;
		chance/=size_mod;
	}
    
    /* speed  */
	if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
		chance += 10;
	if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
		chance -= 25;

    /* level */
    chance += (ch->level - victim->level) * 2;

    /* sloppy hack to prevent false zeroes */
	if (chance % 5 == 0)
		chance += 1;

    /* terrain */

	switch(ch->in_room->sector_type)
	{
		case(SECT_INSIDE):              chance -= 20;   break;
		case(SECT_CITY):                chance -= 10;   break;
		case(SECT_FIELD):               chance +=  5;   break;
		case(SECT_FOREST):                              break;
		case(SECT_HILLS):                               break;
		case(SECT_MOUNTAIN):            chance -= 10;   break;
		case(SECT_DESERT):              chance += 10;   break;
		case(SECT_CAVE):				chance +=  5;	break;
		case(SECT_LAVA):				chance -= 90;   break;
		default:						chance	=  0;	break;
    }

	if ( chance == 0 )
    {
		ch->println("There isn't any dirt to kick.");
		return;
    }

    /* now the attack */
	if (number_percent() < chance)
	{
		AFFECT_DATA af;
		act("$n is blinded by the dirt in $s eyes!",victim,NULL,NULL,TO_ROOM);
		act("$n kicks dirt in your eyes!",ch,NULL,victim,TO_VICT);

		if (!IS_SET(ch->imm_flags,(IMM_BASH & IMM_WEAPON))){
			damage(ch,victim,number_range(2,5),gsn_dirt_kicking,DAM_NONE,false);
		}

		victim->println("You can't see a thing!");
		check_improve(ch,gsn_dirt_kicking,true,2);
		WAIT_STATE(ch,skill_table[gsn_dirt_kicking].beats);

		if (!IS_AFFECTED(victim,AFF_BLIND)) {

			af.where        = WHERE_AFFECTS;
			af.type         = gsn_dirt_kicking;
			af.level        = ch->level;
			af.duration     = 0;
			af.location     = APPLY_HITROLL;
			af.modifier     = -4;
			af.bitvector    = AFF_BLIND;

			affect_to_char(victim,&af);
		}
		else
		{
			act("$E's already been blinded.",ch,NULL,victim,TO_CHAR);
		}
    }
    else
	{
		act("$n is totally missed by the dirt!",victim,NULL,NULL,TO_ROOM);       
		act("$n fails to kick dirt properly!",ch,NULL,victim,TO_VICT);
		damage(ch,victim,0,gsn_dirt_kicking,DAM_NONE,false);
		WAIT_STATE(ch,skill_table[gsn_dirt_kicking].beats*2);
    }
    return;
}

/**************************************************************************/
void do_trip( char_data *ch, char *argument )
{
    char arg[MIL];
    char_data *victim;
    int chance;

    one_argument(argument,arg);

    if ( (chance = get_skill(ch,gsn_trip)) == 0
    ||   (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_TRIP))
    ||   (!IS_NPC(ch) 
	  && ch->level < skill_table[gsn_trip].skill_level[ch->clss]))
    {
		ch->println("Tripping?  What's that?");
		return;
    }

// standard no mount checks etc
	if(IS_RIDING(ch)) 
	{
		ch->println("You can't trip others while riding a creature.");
		return;
	}


    if (arg[0] == '\0')
    {
		victim = ch->fighting;
		if (victim == NULL)
		{
			ch->println("But you aren't fighting anyone!");
			return;
		}
    }else if ((victim = get_char_room(ch,arg)) == NULL)
    {
		ch->println("They aren't here.");
		return;
    }

    if (is_safe(ch,victim))
	return;

    if ((IS_AFFECTED(victim,AFF_FLYING))
       || (ch->mounted_on))
    {
		act("$S feet aren't on the ground.",ch,NULL,victim,TO_CHAR);
		return;
    }

    if (victim->position < POS_FIGHTING)
    {
		act("$N is already down.",ch,NULL,victim,TO_CHAR);
		return;
    }

    if (victim == ch)
    {
		ch->println("You fall flat on your face!");
		WAIT_STATE(ch,2 * skill_table[gsn_trip].beats);
		act("$n trips over $s own feet!",ch,NULL,NULL,TO_ROOM);
		return;
    }

	if ( !can_initiate_combat( ch, victim, CIT_GENERAL )) return;

    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
		act("$N is your beloved master.",ch,NULL,victim,TO_CHAR);
		return;
    }

    /* modifiers */

    /* size */
    if (ch->size < victim->size)
	chance += (ch->size - victim->size) * 10;  /* bigger = harder to trip */

    /* dex */
    chance += ch->modifiers[STAT_QU];
    chance -= victim->modifiers[STAT_QU] * 3 / 2;

    /* speed */
    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
	chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
	chance -= 20;

    // factor in level to chance of success
    chance += (ch->level - victim->level) * 2;

    // now the attack
    if (number_percent() < chance)
    {
		act("$n trips you and you go down!",ch,NULL,victim,TO_VICT);
		act("You trip $N and $N goes down!",ch,NULL,victim,TO_CHAR);
		act("$n trips $N, sending $M to the ground.",ch,NULL,victim,TO_NOTVICT);
		check_improve(ch,gsn_trip,true,1);
		
		msp_to_room(MSPT_COMBAT, MSP_SOUND_TRIP, 0,
					ch,
					true,
					false);

		msp_to_room(MSPT_COMBAT, MSP_SOUND_TRIP, 0,
					victim,
					true,
					false);

		DAZE_STATE(victim,2 * PULSE_VIOLENCE);
		WAIT_STATE(ch,skill_table[gsn_trip].beats);
		victim->position = POS_RESTING;
		damage(ch,victim,number_range(2, 2 +  2 * victim->size),gsn_trip,
			DAM_BASH,true);
    }
    else
    {
		damage(ch,victim,0,gsn_trip,DAM_BASH,true);
		WAIT_STATE(ch,skill_table[gsn_trip].beats*2/3);
		check_improve(ch,gsn_trip,false,1);
    } 
}

/**************************************************************************/
void do_kill( char_data *ch, char *argument )
{
    char arg[MIL];
    char_data *victim;
	char buf[MSL];
	
	sprintf (buf,"%s", argument);
	
    one_argument( argument, arg );
	
    if ( arg[0] == '\0' )
    {
		ch->println("Kill whom?");
		return;
    }
	
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
		ch->println("They aren't here.");
		return;
    }
	
	// checks for preventing the start of a pkill fight
    if ( !IS_NPC(victim) ) // directly attacking the player
    {
		if(GAMESETTING(GAMESET_PEACEFUL_MUD)){
			ch->wrapln("That is a player you are trying to attack. "
				"This is a peaceful mud where no pkilling is allowed.");
		}else{
			ch->wraplnf("That is a player you are trying to kill, "
				"type pkill %s if you really want to do that.", buf );
		}
		return;
    }else{
		if ( IS_SET(victim->act, ACT_PET) 		
				&& victim->leader 
				&& !IS_NPC(victim->leader) 
				&& (victim->leader->pet == victim)
				&& IS_SET(victim->affected_by, AFF_CHARM))
		{
			if(GAMESETTING(GAMESET_PEACEFUL_MUD)){
				ch->wrapln("That is a players pet you are trying to attack. "
					"This is a peaceful mud where no pkilling is allowed.");
			}else{
				ch->wraplnf("That is a players pet, which could start a pkill fight, "
					"type pkill %s if you really want to do that.", buf );
			}
			return;
		}
	}
	
    if ( victim == ch )
    {
		ch->println("You hit yourself.  Ouch!");
		multi_hit( ch, ch, TYPE_UNDEFINED );
		return;
    }
	
    if ( is_safe( ch, victim ) )
		return;

	if ( !can_initiate_combat( ch, victim, CIT_GENERAL )) return;	

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
		act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
		return;
    }
	
    if ( ch->position == POS_FIGHTING )
    {
		ch->println("You do the best you can!");
		return;
    }
	
    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}

/**************************************************************************/
void do_pkill( char_data *ch, char *argument )
{
    char arg[MIL];
    char oarg[MIL];
    char_data *victim;

	if(!IS_NPC(ch) && GAMESETTING(GAMESET_PEACEFUL_MUD)){
		ch->println("This is a peaceful mud... pkilling is not permitted.");
		return;
	}

	sprintf(oarg, "%s", argument);

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
		ch->println("Pkill whom?");
		return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
		ch->println("They aren't here.");
		// lag those who spam it
		if (ch->desc && ch->desc->repeat>3){
			WAIT_STATE( ch, 3 * PULSE_VIOLENCE );
		}
		return;
    }

	// log it if is a pkill attack
    if ( !IS_NPC(victim) )
    {	
		char log_buf[MSL];

        sprintf( log_buf, "Log %s: pkill %s - (found %s)", 
			PERS(ch,NULL), oarg, PERS(victim,NULL));

        log_string( log_buf );
        wiznet(log_buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
    }


    if ( victim == ch )
    {
		ch->println("You hit yourself.  Ouch!");
		multi_hit( ch, ch, TYPE_UNDEFINED );
		return;
    }

    if ( is_safe( ch, victim ) )
		return;

	if ( !can_initiate_combat( ch, victim, CIT_GENERAL )) return;

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
		act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
		return;
    }

    if ( ch->position == POS_FIGHTING )
    {
		ch->println("You do the best you can!");
		return;
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}

/**************************************************************************/
void do_pbackstab( char_data *ch, char *argument )
{
    char arg[MIL];
    char oarg[MIL];
    char_data *victim;
    OBJ_DATA *obj, *osec;

	if(!IS_NPC(ch) && GAMESETTING(GAMESET_PEACEFUL_MUD)){
		ch->println("This is a peaceful mud... pkilling is not permitted.");
		return;
	}

	if(IS_MOUNTED(ch)){
		ch->println("You can't backstab cause you are being ridden.");
		return;
	}

	if(IS_RIDING(ch)){
		ch->println("You can't backstab while riding a creature.");
		return;
	}

	sprintf(oarg, "%s", argument);
    one_argument( argument, arg );
	
    if (arg[0] == '\0')
    {
		ch->println("Pbackstab whom?");
		return;
    }
	
    if (ch->fighting != NULL)
    {
		ch->println("You're facing the wrong end.");
		return;
    }
	
    else if ((victim = get_char_room(ch,arg)) == NULL)
    {
		ch->println("They aren't here.");
		return;
    }
	
    if ( victim == ch )
    {
		ch->println("How can you sneak up on yourself?");
		return;
    }

	
    if ( !can_initiate_combat( ch, victim, CIT_GENERAL )) return;

    if(!IS_SET(victim->in_room->room2_flags,ROOM2_ARENA) &&
       !IS_SET(victim->in_room->room2_flags,ROOM2_WAR) )
   {

	// log it if is a pkill attack
      if ( !IS_NPC(victim) ){	
		char log_buf[MSL];
		
        sprintf( log_buf, "Log %s: pbackstab %s - (found %s)", 
			PERS(ch,NULL), oarg, PERS(victim,NULL));

        log_string( log_buf );
        wiznet(log_buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
      }
   }
	
    if ( !IS_AFFECTED(ch, AFF_FLYING) && IS_RIDING(victim))
	{
		ch->println("You can't backstab someone on a mount unless you are flying.");
		return;
	}
	
        if ( is_safe( ch, victim ) )
		return;

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL
	&& ( obj = get_eq_char(ch, WEAR_CONCEALED) ) == NULL )
    {
		ch->println("You need to wield a dagger to backstab.");
		return;
    }
	else if( obj->wear_loc == WEAR_CONCEALED)
	{
		if(number_percent( ) >= get_skill(ch,gsn_conceal)){
			act("You fumble as you pull $p out.",ch, obj, NULL, TO_CHAR);
			act("$p falls next to $n.", ch, obj, NULL, TO_ROOM);
			obj_from_char(obj);
			obj_to_room(obj, ch->in_room);
			check_improve(ch, gsn_conceal, false, 2);
			return;
		}
		else{
			act("You draw $p from it's hiding place!",ch, obj, NULL, TO_CHAR);
			act("$p appears in $n's hands from nowhere!", ch, obj, NULL, TO_ROOM);
			equip_char(ch, obj, WEAR_WIELD);
			check_improve(ch, gsn_conceal, true, 3);
		}
	}

	// must use a dagger to backstab
	if (obj->item_type != ITEM_WEAPON
		|| obj->value[0]!=WEAPON_DAGGER)
	{
		ch->println("You need to wield a dagger to backstab.");
		return;	
	}
	
	osec=get_eq_char (ch, WEAR_SECONDARY);
	if (osec)
	{
		if (osec->item_type != ITEM_WEAPON
			|| osec->value[0]!=WEAPON_DAGGER)
		{
			ch->println("Your secondary weapon must be a dagger as well to backstab.");
			return;	
		}		
	}

    if ( victim->hit < victim->max_hit / 2)
    {
		act( "$N is hurt and suspicious ... you can't sneak up.",
			ch, NULL, victim, TO_CHAR );
		return;
    }
	
    WAIT_STATE( ch, skill_table[gsn_backstab].beats );
	
	if ( IS_AFFECTED(victim, AFF_FLYING) 
		&& !IS_AFFECTED(ch, AFF_FLYING) 
		&& number_range(1,3)==1)
    {
		ch->println("You attempt to backstab them, but they fly out of the way and you miss.");
		check_improve(ch,gsn_backstab,false,1);
		damage( ch, victim, 0, gsn_backstab,DAM_NONE,true);
    }else{
		if ( number_percent( ) < get_skill(ch,gsn_backstab)
			|| ( get_skill(ch,gsn_backstab) >= 2 && !IS_AWAKE(victim) ) )
		{
			check_improve(ch,gsn_backstab,true,1);
			multi_hit( ch, victim, gsn_backstab );

			msp_to_room(MSPT_COMBAT, MSP_SOUND_BACKSTAB, 
					0, 
					ch,
					false,
					true);

			msp_to_room(MSPT_COMBAT, MSP_SOUND_BACKSTAB, 
					0, 
					victim,
					true,
					false);

		}
		else
		{
			check_improve(ch,gsn_backstab,false,1);
			damage( ch, victim, 0, gsn_backstab,DAM_NONE,true);
		}
	}
    return;
}

/**************************************************************************/ 
void do_circle( char_data *ch, char *argument )
{
    char_data * victim;
    OBJ_DATA  * obj;
    int         chance;

	if ((chance = get_skill(ch,gsn_circle)) == 0)
	{
		ch->println("What would you know about circling people?");
		return;
	}

    obj=get_eq_char( ch, WEAR_WIELD);
    if(!obj || obj->value[0]!=WEAPON_DAGGER){
		ch->println("You must wield a dagger to circle.");
        return;
    }

	victim = ch->fighting;
    if(!victim){
		ch->println("You aren't fighting anyone.");
		return;
    }

	if (IS_AFFECTED(ch,AFF_BLIND)){
		ch->println("You are blinded and can't find a path to circle them.");
		WAIT_STATE( ch, skill_table[gsn_circle].beats );
		return;
	}

	if(can_see(victim, ch) && !IS_AFFECTED(victim,AFF_BLIND))
	{
		if(!GAMESETTING3(GAMESET3_CIRCLE_ALLOWED_WITHOUT_CLEAR_SHOT))
		{
		if ( victim->fighting == ch ){
			act( "$N turns with you, not offering you a clear shot.", ch, NULL, victim, TO_CHAR );
			return;
		}
	}
	}else
	
	if ( get_skill( ch,gsn_circle ) > number_range(10,125)) {
   		multi_hit( ch, victim, gsn_circle );
   		check_improve( ch, gsn_circle, true, 1 );
	}else{
   		damage(ch, victim, 0, gsn_circle, DAM_NONE, true );
        	check_improve( ch, gsn_circle, false, 1 );
	}
    WAIT_STATE( ch, skill_table[gsn_circle].beats );
}

/**************************************************************************/
void do_backstab( char_data *ch, char *argument )
{
    char arg[MIL];
    char_data *victim;
    OBJ_DATA *obj, *osec;
	char buf[MSL];

	// standard no mount checks etc
	if(IS_MOUNTED(ch)) 
	{
		ch->println("You can't backstab cause you are being ridden.");
		return;
	}
	if(IS_RIDING(ch)) 
	{
		ch->println("You can't backstab while riding a creature.");
		return;
	}

	sprintf (buf,"%s", argument);
    one_argument( argument, arg );

    if (arg[0] == '\0')
    {
		ch->println("Backstab whom?");
		return;
    }

    if (ch->fighting != NULL)
    {
		ch->println("You're facing the wrong end.");
		return;
    }
    else if ((victim = get_char_room(ch,arg)) == NULL)
    {
		ch->println("They aren't here.");
		if (ch->desc && ch->desc->repeat>3){
			WAIT_STATE( ch, 3 * PULSE_VIOLENCE );
		}
		return;
    }

    if ( victim == ch )
    {
		ch->println("How can you sneak up on yourself?");
		return;
    }


	if ( victim->fighting == ch )
    {
		ch->println("They already appear to be fighting you!");
		return;
    }

	// checks for preventing the start of a pkill fight
    if ( !IS_NPC(victim) ) // directly attacking the player
    {
		if(GAMESETTING(GAMESET_PEACEFUL_MUD)){
			ch->wrapln("That is a player you are trying to backstab. "
				"This is a peaceful mud where no pkilling is allowed.");
		}else{
			ch->wraplnf("That is a player your a trying to backstab, "
				"type pbs %s if you really want to do that.", buf );
		}
		return;
    }else if ( IS_SET(victim->act, ACT_PET) 		
		&& victim->leader 
		&& !IS_NPC(victim->leader) 
		&& (victim->leader->pet == victim)
		&& IS_SET(victim->affected_by, AFF_CHARM))
	{
		if(GAMESETTING(GAMESET_PEACEFUL_MUD)){
			ch->wrapln("That is a players pet you are trying to backstab. "
				"This is a peaceful mud where no pkilling is allowed.");
		}else{
			ch->wraplnf("That is a players pet, which could start a pkill fight, "
				"type pbs %s if you really want to do that.", buf );
		}
		return;
	}

	if ( !can_initiate_combat( ch, victim, CIT_GENERAL )) return;

    if ( !IS_AFFECTED(ch, AFF_FLYING) && IS_RIDING(victim))
	{
		ch->println("You can't backstab someone on a mount unless you are flying.");
		return;
	}

    if ( is_safe( ch, victim ) )
		return;

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL
	&& ( obj = get_eq_char(ch, WEAR_CONCEALED) ) == NULL )
	{
		ch->println("You need to wield a dagger to backstab.");
		return;
    }
	else if( obj->wear_loc == WEAR_CONCEALED)
	{
		if(number_percent( ) >= get_skill(ch,gsn_conceal)){
			act("You fumble as you pull $p out.",ch, obj, NULL, TO_CHAR);
			act("$p falls next to $n.", ch, obj, NULL, TO_ROOM);
			obj_from_char(obj);
			obj_to_room(obj, ch->in_room);
			check_improve(ch, gsn_conceal, false, 2);
			return;
		}
		else{
			act("You draw $p from it's hiding place!",ch, obj, NULL, TO_CHAR);
			act("$p appears in $n's hands from nowhere!", ch, obj, NULL, TO_ROOM);
			equip_char(ch, obj, WEAR_WIELD);
			check_improve(ch, gsn_conceal, true, 3);
		}
	}
	// must use a dagger to backstab
	if (obj->item_type != ITEM_WEAPON
		|| obj->value[0]!=WEAPON_DAGGER)
	{
		ch->println("You need to wield a dagger to backstab.");
		return;	
	}
	
	osec=get_eq_char (ch, WEAR_SECONDARY);
	if (osec)
	{
		if (osec->item_type != ITEM_WEAPON
			|| osec->value[0]!=WEAPON_DAGGER)
		{
			ch->println("Your secondary weapon must be a dagger as well to backstab.");
			return;	
		}		
	}


    if ( victim->hit < victim->max_hit / 3)
    {
		act( "$N is hurt and suspicious ... you can't sneak up.",
			ch, NULL, victim, TO_CHAR );
		return;
    }

    if (  victim->cautious_about_backstab>0)
    {
		act( "$N is too suspicious of your attack... you can't sneak up.",
			ch, NULL, victim, TO_CHAR );
		return;
    }


    WAIT_STATE( ch, skill_table[gsn_backstab].beats );
	if ( IS_AFFECTED(victim, AFF_FLYING) 
		&& !IS_AFFECTED(ch, AFF_FLYING) 
		&& number_range(1,3)==1)
    {
		ch->println("You attempt to backstab them, but they fly out of the way and you miss.");
		check_improve(ch,gsn_backstab,false,1);
		damage( ch, victim, 0, gsn_backstab,DAM_NONE,true);
    }else{
		if ( number_percent( ) < get_skill(ch,gsn_backstab)
			|| ( get_skill(ch,gsn_backstab) >= 2 && !IS_AWAKE(victim) ) )
		{
			msp_to_room(MSPT_COMBAT, MSP_SOUND_BACKSTAB, 
					0, 
					ch,
					false,
					true);

			msp_to_room(MSPT_COMBAT, MSP_SOUND_BACKSTAB, 
					0, 
					victim,
					true,
					false);

			check_improve(ch,gsn_backstab,true,1);
			multi_hit( ch, victim, gsn_backstab );
		}
		else
		{
			check_improve(ch,gsn_backstab,false,1);
			damage( ch, victim, 0, gsn_backstab,DAM_NONE,true);
		}
	}
    return;
}


/**************************************************************************/
void do_flee( char_data *ch, char *)
{
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    char_data *victim;
	OBJ_DATA *polearm;
    int attempt;
	
    if ( ( victim = ch->fighting ) == NULL )
    {
		if ( ch->position == POS_FIGHTING )
			ch->position = POS_STANDING;
		ch->println("You aren't fighting anyone.");
		return;
    }

	if( IS_AFFECTED(ch,AFF_SLOW) 
		&& number_range(1,4) < 4
		&& ch->mounted_on!=NULL)
	{
		ch->println("You feel too lethargic to run.");
		return;
	}

	// 65% chance those that are berserked/frenzied can't flee
	if ((IS_AFFECTED(ch,AFF_BERSERK) 
		|| is_affected(ch,gsn_frenzy)
		|| is_affected(ch,gsn_berserk))
		&& number_range(1,100)>35)
	{
		ch->println("Your RAGE within is so strong you can't draw yourself away from the fight!");
		return;
	}

	// Cuttoff code here

	if ( IS_NPC( victim )
	||   IS_SET( victim->dyn, DYN_IS_CUTTING_OFF ))
	{
		polearm = get_eq_char( victim, WEAR_WIELD );

		if ( polearm )
		{
			if ( polearm->value[0] == WEAPON_POLEARM )
			{
				int		chance = number_percent();
				bool	successful = false;

				if ( IS_NPC( victim )
				&& IS_SET( victim->act, ACT_WARRIOR )
				&& chance < ( victim->level/3 ))
					successful = true;
				else if ( chance + 50 < get_skill( victim, gsn_cutoff ))
					successful = true;

				if ( successful )
				{
					act( "$N blocks your escape with $p.", ch, polearm, victim, TO_CHAR );
					act( "$n tries to flee, but you cut $m off.", ch, NULL, victim, TO_VICT );
					act( "$n tries to flee, but $N makes escape impossible.", ch, NULL, victim, TO_NOTVICT );
					check_improve( ch, gsn_cutoff, true, 1 );
					return;
				}
			}
		}
	}
	// End Cuttoff addition

    was_in = ch->in_room;
    for ( attempt = 0; attempt < 10; attempt++ )
    {
		EXIT_DATA *pexit;
		int door;
		
		door = number_door( );
		
		if ( ( pexit = was_in->exit[door] ) == 0
			||   pexit->u1.to_room == NULL
			||   IS_SET(pexit->exit_info, EX_CLOSED)
			||   number_range(0,ch->daze) != 0
			|| ( IS_NPC(ch)
			&&   IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB) ) )
			continue;
		
		
		move_char( ch, door, false, NULL );
		if (ch->mounted_on!=NULL)
		{
			char_from_room(ch->mounted_on);
			char_to_room(ch->mounted_on, ch->in_room);
		}
		
		if ( ( now_in = ch->in_room ) == was_in )
			continue;
		
		ch->in_room = was_in;
		act( "$n has fled!", ch, NULL, NULL, TO_ROOM );
		ch->in_room = now_in;

		if ( !IS_NPC(ch) )
		{
			msp_to_room(MSPT_ACTION, MSP_SOUND_FLEE, 0,	ch, true, false);	

			ch->println("You flee from combat!");
			if( HAS_CLASSFLAG(ch, CLASSFLAG_SAFE_FLEE_FROM_COMBAT)
				&& (number_percent() < 3*(ch->level/2) ) )
			{
				ch->println("You snuck away safely.");
			}else{
				ch->printlnf("You lost %d exp.", game_settings->xp_loss_for_fleeing); 
				gain_exp( ch, game_settings->xp_loss_for_fleeing*-1 );
				if ( IS_HERO( ch )){
					do_heroxp( ch, game_settings->xp_loss_for_fleeing*-1 );
				}
			}
		}else{		//  IS NPC
			mobRememberClear( ch );
		}

		stop_fighting( ch, true ); // Do_Flee
		return;
    }
	
    ch->println("PANIC! You couldn't escape!");
    return;
}


/**************************************************************************/
// Kerenos - July 98
void do_retreat( char_data *ch, char *argument )
{
    ROOM_INDEX_DATA	*was_in;
    ROOM_INDEX_DATA	*now_in;
	OBJ_DATA		*polearm;
    char_data		*victim;
    EXIT_DATA		*pexit;
    int 		chance, door;
    char		arg[MIL];
	
    if (( victim = ch->fighting ) == NULL ) {
		if ( ch->position == POS_FIGHTING ){
			ch->position = POS_STANDING;
		}
		ch->println("You aren't fighting anyone.");
		return;
    }
	
    if (( chance = get_skill( ch, gsn_retreat )) == 0)
	{
		ch->println("You do not know how to retreat, try fleeing.");
		return;
    }
	
    if ( IS_AFFECTED( ch, AFF_SLOW )
		&& number_range( 1, 4 ) < 4
		&& ch->mounted_on != NULL ) {
		ch->println("You feel too lethargic to run.");
		return;
    }	
	
	// 85% chance those that are berserked/frenzied can't flee
	if ((IS_AFFECTED(ch,AFF_BERSERK) 
		|| is_affected(ch,gsn_frenzy)
		|| is_affected(ch,gsn_berserk))
		&& number_range(1,100)>85)
	{
		ch->println("Your RAGE within is so strong you dont want to retreat!");
		return;
	}

	// Cuttoff code here

	if ( IS_NPC( victim )
	||   IS_SET( victim->dyn, DYN_IS_CUTTING_OFF ))
	{
		polearm = get_eq_char( victim, WEAR_WIELD );

		if ( polearm )
		{
			if ( polearm->value[0] == WEAPON_POLEARM )
			{
				int		chance = number_percent();
				bool	successful = false;

				if ( IS_NPC( victim )
				&& IS_SET( victim->act, ACT_WARRIOR )
				&& chance < ( victim->level/5 ))
					successful = true;
				else if ( chance + 67 < get_skill( victim, gsn_cutoff ))
					successful = true;

				if ( successful )
				{
					act( "$N blocks your escape with $p.", ch, polearm, victim, TO_CHAR );
					act( "$n tries to flee, but you cut $m off.", ch, NULL, victim, TO_VICT );
					act( "$n tries to flee, but $N makes escape impossible.", ch, NULL, victim, TO_NOTVICT );
					check_improve( ch, gsn_cutoff, true, 1 );
					return;
				}
			}
		}
	}
	// End Cuttoff addition
	
    
	one_argument( argument, arg );
	
    if ( IS_NULLSTR( arg )) {
		ch->println("You must provide a direction in which to retreat.");
		return;
    }

	door = dir_lookup( arg );
	
    if ( door == -1 )
	{
		ch->println("Bad direction. Which way?"); 
		return;
    }
	

    was_in = ch->in_room;
	
    if (( pexit = was_in->exit[door] ) == 0
		||   pexit->u1.to_room == NULL
		||   IS_SET( pexit->exit_info, EX_CLOSED )
		||   number_range( 0, ch->daze ) != 0
		|| ( IS_NPC( ch )
		&&   IS_SET( pexit->u1.to_room->room_flags, ROOM_NO_MOB ))) {
		ch->println("You can't retreat in that direction.");
		return;
    }

	//  Do some stat checking
    if ( IS_SET( ch->off_flags, OFF_FAST ) || IS_AFFECTED( ch, AFF_HASTE ))
		chance += 10;
    if ( IS_SET( victim->off_flags, OFF_FAST ) || IS_AFFECTED( victim, AFF_HASTE ))
		chance -= 20;
    chance     += ch->modifiers[STAT_QU];
    chance     += ch->modifiers[STAT_SD];   // Character's resolve
    chance     -= victim->modifiers[STAT_QU];
    chance     -= victim->modifiers[STAT_AG];
	
    if ( number_percent() < chance )
    {
		move_char( ch, door, false, NULL );
		if ( ch->mounted_on != NULL ) {
			char_from_room( ch->mounted_on );
			char_to_room( ch->mounted_on, ch->in_room );
		}
		
        if (( now_in = ch->in_room ) == was_in ) {
			ch->println("That got you nowhere.");
			return;
		}  //  Loopy maze room check
		
		ch->in_room = was_in;	
        act("$n has retreated.", ch, NULL, NULL, TO_ROOM );
        ch->in_room = now_in;
		
		if ( !IS_NPC( ch )) {
			msp_to_room(MSPT_ACTION, MSP_SOUND_FLEE, 0,	ch, true, false);	
			ch->println("You have succesfully retreated from the fight.");
			if (ch->move)
			{
				ch->move= (ch->move*8/10)+1; //lose just under 20% of your moves
			}

			ch->println("You lost 5 exp.");
			gain_exp( ch, -5 );
			if ( IS_HERO( ch ))
				do_heroxp( ch, -10 );
		}
		check_improve( ch, gsn_retreat, true, 1);
        WAIT_STATE(ch,skill_table[gsn_trip].beats);
		stop_fighting( ch, true ); // Do_Retreat
		return;
    }
    else {
		ch->println("You couldn't get away.");
        WAIT_STATE( ch, skill_table[gsn_retreat].beats*2/3);
        check_improve( ch, gsn_retreat, false, 1 );
    }
    return;
}

/**************************************************************************/
void do_rescue( char_data *ch, char *argument )
{
    char arg[MIL];
    char_data *victim;
    char_data *fch;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
		ch->println("Rescue whom?");
		return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
		ch->println("They aren't here.");
		return;
    }

    if ( victim == ch )
    {
		ch->println("What about fleeing instead?");
		return;
    }

	// players can't rescue mobs
    if ( !IS_NPC(ch) && IS_NPC(victim) )
    {
		ch->println("Doesn't need your help!");
		return;
    }


    if ( (IS_AFFECTED2(ch, AFF2_SHADOWPLANE) &&
         !IS_AFFECTED2(victim, AFF2_SHADOWPLANE)) ||
        (!IS_AFFECTED2(ch, AFF2_SHADOWPLANE) &&
          IS_AFFECTED2(victim, AFF2_SHADOWPLANE)) )
    {
         ch->println( "You can't rescue someone on a different plane!");
         return;
    }

    if ( ch->fighting == victim )
    {
		ch->println("Too late.");
		return;
    }

    if ( ( fch = victim->fighting ) == NULL )
    {
		ch->println("That person is not fighting right now.");
		return;
    }

    WAIT_STATE( ch, skill_table[gsn_rescue].beats );
    if ( number_percent( ) > get_skill(ch,gsn_rescue))
    {
		ch->println("You fail the rescue.");
		check_improve(ch,gsn_rescue,false,1);
		return;
    }

	// The victim ($N) is being rescued
    act( "You rescue $N!",  ch, NULL, victim, TO_CHAR	 );
    act( "$n rescues you!", ch, NULL, victim, TO_VICT    );
    act( "$n rescues $N!",  ch, NULL, victim, TO_NOTVICT );
    check_improve(ch,gsn_rescue,true,1);

    stop_fighting( fch, false );   // Do_Rescue
    stop_fighting( victim, false );// Do_Rescue

	// stop cheaters backstabing straight away
	fch->cautious_about_backstab = 
		UMAX(fch->cautious_about_backstab, number_range(1,3));

	SET_BIT(ch->dyn,DYN_DOING_DAMAGE);
	if (!ch->fighting){
		set_fighting( ch, fch );
	}
	if (!fch->fighting){
		set_fighting( fch, ch );
	}
	REMOVE_BIT(ch->dyn,DYN_DOING_DAMAGE);
    return;
}


/**************************************************************************/
void do_kick( char_data *ch, char *argument)
{
    char_data *victim;
	
    if ( !IS_NPC(ch)
		&&   ch->level < skill_table[gsn_kick].skill_level[ch->clss] )
    {
		ch->println("You better leave the martial arts to fighters.");
		return;
    }
	
    if (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_KICK)){
		return;
	}
	
	if(IS_NULLSTR(argument))
	{
		victim = ch->fighting;
		if (victim == NULL)
		{
			ch->println("But you aren't fighting anyone!");
			return;
		}
	}
	else if ((victim = get_char_room(ch,argument)) == NULL)
	{
		ch->printlnf("'%s' is not here.", argument);
		return;
	}
	
	if (victim == ch)
	{
		ch->println("You try to kick yourself but fail.");
		return;
	}

	if (is_safe(ch,victim))
		return;
	
	if ( !can_initiate_combat( ch, victim, CIT_GENERAL )) return;
	
    WAIT_STATE( ch, skill_table[gsn_kick].beats );
    if ( get_skill(ch,gsn_kick) > number_percent())
    {
		damage(ch,victim,number_range( 1, ch->level ), gsn_kick,DAM_BASH,true);
		check_improve(ch,gsn_kick,true,1);
    }
    else
    {
		damage( ch, victim, 0, gsn_kick,DAM_BASH,true);
		check_improve(ch,gsn_kick,false,1);
    }
 
    return;
}


/**************************************************************************/
void do_disarm( char_data *ch, char *argument )
{
	char arg[MIL];
    char_data *victim;
    OBJ_DATA *obj;
    int chance,hth,ch_weapon,vict_weapon,ch_vict_weapon;

	one_argument( argument,arg );

	if ( !IS_NULLSTR( arg ) && !ch->fighting )
	{
		// disarm with an argument tries to disarm a trap
		// if the player is fighting, then it will continue
		do_disarm_trap( ch, arg );
		return;
	}

    hth = 0;

    if ((chance = get_skill(ch,gsn_disarm)) == 0)
    {
		ch->println("You don't know how to disarm opponents.");
		return;
    }

    if ( get_eq_char( ch, WEAR_WIELD ) == NULL 
    &&   ((hth = get_skill(ch,gsn_hand_to_hand)) == 0
    ||    (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_DISARM))))
    {
		ch->println("You must wield a weapon to disarm.");
		return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
		ch->println("You aren't fighting anyone.");
		return;
    }

    if (( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL ) 
	if (( obj = get_eq_char( victim, WEAR_SECONDARY) ) == NULL )
    {
		ch->println("Your opponent is not wielding a weapon.");
		return;
    }

	// No disarm when the victim has vicegrip
    if (IS_AFFECTED2(victim, AFF2_VICEGRIP)){
		act("Your opponent has an iron grip on their weapon! You have no chance!",ch,NULL,victim,TO_CHAR);
		check_improve(ch,gsn_disarm,false,1);
	    return;
	}

    // find weapon skills
    ch_weapon = get_weapon_skill(ch,get_weapon_sn(ch));
    vict_weapon = get_weapon_skill(victim,get_weapon_sn(victim));
    ch_vict_weapon = get_weapon_skill(ch,get_weapon_sn(victim));

    // modifiers

    // skill 
    if ( get_eq_char(ch,WEAR_WIELD) == NULL)
		chance = chance * hth/150;
    else
		chance = chance * ch_weapon/100;

    chance += (ch_vict_weapon/2 - vict_weapon) / 2; 

    // quickness/agility  vs. strength 
    chance += (ch->modifiers[STAT_QU] + ch->modifiers[STAT_AG])/2;
    chance -= 2 * victim->modifiers[STAT_ST];

	if(IS_NPC(ch)){
		chance = chance * 4/5;
	}

    // level 
    chance += (ch->level - victim->level) * 2;
 
    // and now the attack 
    if (number_percent() < chance)
    {
		WAIT_STATE( ch, skill_table[gsn_disarm].beats );
		disarm( ch, victim );
		check_improve(ch,gsn_disarm,true,1);
    }
    else
    {
		WAIT_STATE(ch,skill_table[gsn_disarm].beats);
		act("You fail to disarm $N.",ch,NULL,victim,TO_CHAR);
		act("$n tries to disarm you, but fails.",ch,NULL,victim,TO_VICT);
		act("$n tries to disarm $N, but fails.",ch,NULL,victim,TO_NOTVICT);
		check_improve(ch,gsn_disarm,false,1);
    }
    return;
}

/**************************************************************************/
void do_surrender( char_data *ch, char * )
{
    char_data *mob;

	mob = ch->fighting;
    if (!mob)
	{
		ch->println("But you're not fighting !");
		return;
    }

	if (!IS_NPC(mob)){
		ch->println("You can't surrender in pkill fights!");
		return;
	}

	// stop cheaters backstabing straight away
	mob->cautious_about_backstab = 
		UMAX(mob->cautious_about_backstab, number_range(2,5));

    act( "You surrender to $N!", ch, NULL, mob, TO_CHAR );
    act( "$n surrenders to you!", ch, NULL, mob, TO_VICT );
    act( "$n tries to surrender to $N!", ch, NULL, mob, TO_NOTVICT );
    stop_fighting( ch, true ); // Do_surrender
 
    if ( !IS_NPC( ch ) && IS_NPC( mob ) 
    &&   ( !HAS_TRIGGER( mob, TRIG_SURR ) 
	|| !mp_percent_trigger( mob, ch, NULL, NULL, TRIG_SURR ) ) )
    {
		act( "$N seems to ignore your cowardly act!", ch, NULL, mob, TO_CHAR );
		multi_hit( mob, ch, TYPE_UNDEFINED );
	}
}


/**************************************************************************/
void do_sla( char_data *ch, char *)
{
    ch->println("If you want to SLAY, spell it out.");
    return;
}

/**************************************************************************/
void do_tail( char_data *ch, char *argument )
{
    char_data *victim;
    char arg[MIL];
    int chance;

    one_argument(argument,arg);

    if ( (chance = get_skill(ch,gsn_tail)) == 0
	||	!IS_SET(ch->parts, PART_TAIL)
	||  (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_TAIL))
    ||  (!IS_NPC(ch)
	&&   ch->level < skill_table[gsn_tail].skill_level[ch->clss]))
    {
		ch->println("Do you even have a tail?");
		return;
    }

// standard no mount checks etc
	if(IS_RIDING(ch)) 
	{
		ch->println("You can't use your tail on others while riding a creature.");
		return;
	}


    if (arg[0] == '\0')
    {
		victim = ch->fighting;
		if (victim == NULL)
		{
			ch->println("But you aren't fighting anyone!");
			return;
		}
    }else if ((victim = get_char_room(ch,arg)) == NULL)
    {
		ch->println("They aren't here.");
		return;
    }

    if (is_safe(ch,victim))
	return;

    if ((IS_AFFECTED(victim,AFF_FLYING))
       || (ch->mounted_on))
    {
		act("$S feet aren't on the ground.",ch,NULL,victim,TO_CHAR);
		return;
    }

    if (victim->position < POS_FIGHTING)
    {
		act("$N is already down.",ch,NULL,victim,TO_CHAR);
		return;
    }

    if (victim == ch)
    {
		ch->println("You fall flat on your face!");
		WAIT_STATE(ch,2 * skill_table[gsn_tail].beats);
		act("$n trips over $s own tail!",ch,NULL,NULL,TO_ROOM);
		return;
    }

    if (is_safe(ch,victim))
		return;

    if ( IS_NPC(victim) &&
		victim->fighting != NULL &&
		!is_same_group(ch,victim->fighting))
	    {
			ch->println("Kill stealing is not permitted.");
			return;
	    }

    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
		act("But $N is your friend!",ch,NULL,victim,TO_CHAR);
		return;
    }

    chance += ch->carry_weight / 250;
    chance -= victim->carry_weight / 200;

    if (ch->size < victim->size)
		chance += (ch->size - victim->size) * 15;
    else
		chance += (ch->size - victim->size) * 10; 

    chance += ch->modifiers[STAT_ST];
    chance -= victim->modifiers[STAT_QU];
    chance -= GET_AC(victim,AC_BASH) /25;

    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
		chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
		chance -= 30;

    chance += (ch->level - victim->level);

    if (!IS_NPC(victim) 
	&& chance < get_skill( victim, gsn_dodge ))
    {   
		act("$n tries to smash you with a tail.",ch,NULL,victim,TO_VICT);
		act("$N dodges your tail, you falter off balance.",ch,NULL,victim,TO_CHAR);
		WAIT_STATE(ch,skill_table[gsn_tail].beats);
		return;
    }

    if (number_range(1, 100) < chance )
    {
		act("$n sweeps your feet from under you with $s tail!",	ch,NULL,victim,TO_VICT);
	    act("You slam a tail into $N, and send $M flying!",ch,NULL,victim,TO_CHAR);
	    act("$n sends $N sprawling with a powerful smash from a tail.",	ch,NULL,victim,TO_NOTVICT);
	    check_improve(ch,gsn_tail,true,1);

		DAZE_STATE(victim, 3 * PULSE_VIOLENCE);
	    WAIT_STATE(ch,skill_table[gsn_tail].beats);
		victim->position = POS_RESTING;
  damage(ch,victim,number_range(2,2 + 2 * ch->size + chance/20),gsn_tail,
	    DAM_BASH,false);
	
    }
    else
    {
           damage(ch,victim,0,gsn_tail,DAM_BASH,false);
		act("You fall flat on your face!", ch,NULL,victim,TO_CHAR);
		act("$n falls flat on $s face.", ch,NULL,victim,TO_NOTVICT);
	    act("You evade $n's tail, causing $m to falter and lose balance.", ch,NULL,victim,TO_VICT);
	    check_improve(ch,gsn_tail,false,1);
	    ch->position = POS_STANDING;
	    WAIT_STATE(ch,skill_table[gsn_tail].beats * 3/2); 
    }
}
/**************************************************************************/
void do_claw( char_data *ch, char *)
{
    char_data *victim;
	
    if ( !IS_NPC(ch)
		&&   (ch->level < skill_table[gsn_claw].skill_level[ch->clss] 
		|| get_skill(ch,gsn_claw)==0) )
    {
		ch->println("You do not know how to claw.");
		return;
    }
	
    if ( ( victim = ch->fighting ) == NULL )
    {
		ch->println("You aren't fighting anyone.");
		return;
    }
    
    WAIT_STATE( ch, skill_table[gsn_claw].beats );
    if ( get_skill(ch,gsn_claw) > number_percent())
    {
		damage(ch,victim,number_range( 1, ch->level+ch->level/2 ), gsn_claw,DAM_SLASH,true);
		check_improve(ch,gsn_claw,true,1);
    }
    else
    {
		damage( ch, victim,0, gsn_claw,DAM_SLASH,true);
		check_improve(ch,gsn_claw,false,1);
    }
    return;
}
/**************************************************************************/
void do_bite( char_data *ch, char *)
{
    char_data *victim;
	
    if ( !IS_NPC(ch)
		&&   (ch->level < skill_table[gsn_bite].skill_level[ch->clss] 
		|| get_skill(ch,gsn_bite)==0) )
    {
		ch->println("You do not know how to bite.");
		return;
    }
	
    if ( ( victim = ch->fighting ) == NULL )
    {
		ch->println("You aren't fighting anyone.");
		return;
    }
    
    WAIT_STATE( ch, skill_table[gsn_bite].beats );
    if ( get_skill(ch,gsn_bite) > number_percent())
    {
		damage(ch,victim,number_range( 1, ch->level+ch->level/2 ), gsn_bite,DAM_SLASH,true);
		check_improve(ch,gsn_bite,true,1);
    }
    else
    {
		damage( ch, victim,0, gsn_bite,DAM_PIERCE,true);
		check_improve(ch,gsn_bite,false,1);
    }
    return;
}
/**************************************************************************/
// Written by Ixliam July 2003
void do_crush( char_data *ch, char *)
{
    char_data *victim;
    int chance;

    if ( !IS_NPC(ch)
		&&   (ch->level < skill_table[gsn_crush].skill_level[ch->clss] 
		|| get_skill(ch,gsn_crush)==0) )
    {
		ch->println("You do not know how to crush.");
		return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
		ch->println("You aren't fighting anyone.");
		return;
    }

	if (IS_SET(ch->affected_by2, AFF2_STONE_GARGOYLE))
	{
		ch->printlnf( "Not while you are a statue." );
		return;
    	}

	if (IS_SET(ch->affected_by2, AFF2_STOCKADE))
	{
		ch->printlnf( "The stocks prevent you from doing that." );
		return;
   }


    chance  = 40 + ch->level;
    chance += ch->perm_stats[STAT_ST] - victim->perm_stats[STAT_ST];
    chance += 5 * (ch->level - ch->fighting->level);
    chance += 10;
    chance += 20 * (ch->size - victim->size);

    if ( number_percent() > chance )
	return;

    if ( IS_SET(ch->parts, PART_ARMS) )
    {
    	act( "$n grabs $N and crushes $M in $s arms!",  ch, NULL, victim, TO_NOTVICT );
    	act( "You grab $N and crush $M in your arms!",  ch, NULL, victim, TO_CHAR );
	act( "$n grabs you and crushes you in $s arms!", ch, NULL, victim, TO_VICT );
    }
    else if ( IS_SET(ch->parts, PART_TENTACLES) )
    {
    	act( "$n wraps $s tentacles around $N and squeezes!",  ch, NULL, victim, TO_NOTVICT );
    	act( "You wrap your tentacles around $N and squeeze!",  ch, NULL, victim, TO_CHAR );
	act( "$n wraps $s tentacles around you and squeezes!", ch, NULL, victim, TO_VICT );
    }
    else
    {
    	act( "$n crushes $N!",  ch, NULL, victim, TO_NOTVICT );
    	act( "You crush $N!",  ch, NULL, victim, TO_CHAR );
	act( "$n crushes you!", ch, NULL, victim, TO_VICT );
    }
    
    DAZE_STATE( ch->fighting, 3 * PULSE_VIOLENCE );
    WAIT_STATE( ch->fighting, 3 * PULSE_VIOLENCE );
    WAIT_STATE( ch, skill_table[gsn_crush].beats*2 );

    damage( ch, victim, 
 	dice(UMIN( 3, 4), UMAX(6, (ch->size - victim->size) * 10 * ch->level / MAX_LEVEL)),
        TYPE_UNDEFINED, DAM_BASH, false );
	check_improve(ch,gsn_crush,false,1);
}


/**************************************************************************/
// Written by Ixliam July 2003
void do_gaze( char_data *ch, char *)
{
    char_data *victim;
    OBJ_DATA  *statue;
    char buf[MSL];
	
    if ( ( victim = ch->fighting ) == NULL )
    {
		ch->println("You aren't fighting anyone.");
		return;
    }

	if (IS_SET(ch->affected_by2, AFF2_STONE_GARGOYLE))
	{
		ch->printlnf( "Not while you are a statue." );
		return;
    	}

	if (IS_SET(ch->affected_by2, AFF2_STOCKADE))
	{
		ch->printlnf( "The stocks prevent you from doing that." );
		return;
   }


    act( "You gaze on $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n gazes at $N with glowing eyes!",  ch, NULL, victim, TO_NOTVICT );
    act( "$n gazes at you with glowing eyes!", ch, NULL, victim, TO_VICT    );

    if (get_obj_wear(victim, "mirror") != NULL)
	  return;

    if (IS_IMMORTAL(victim))
	  return;

    if ( saves_spell( ch->level, victim, DAM_ENERGY ))
	{
	  return;
	}

    act( "`#`R$n's gaze has turned $N to stone!`^",  ch, NULL, victim, TO_NOTVICT );
    act( "`#`R$n has turned you to stone!`^", ch, NULL, victim, TO_VICT    );
    statue = create_object(get_obj_index(2028));
    statue->level = 0;
    sprintf( buf, statue->short_descr, victim->short_descr );
    free_string( statue->short_descr );
    statue->short_descr = str_dup( buf );
    sprintf( buf, statue->description, victim->short_descr );
    replace_string( statue->description, buf);
    obj_to_room( statue, victim->in_room );
    victim->position = POS_DEAD;
    kill_char(victim, ch);
    return;
}

/**************************************************************************/
// Written originally by Rathern, fixed poison by Kal July 99
void do_gouge( char_data *ch, char *)
{
    char_data *victim;
	
    if ( !IS_NPC(ch)
		&&   (ch->level < skill_table[gsn_gouge].skill_level[ch->clss] 
		|| get_skill(ch,gsn_gouge)==0) )
    {
		ch->println("You do not know how to gouge.");
		return;
    }
	
    /*
    if (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_gouge))
	return;
    */
	
    if ( ( victim = ch->fighting ) == NULL )
    {
		ch->println("You aren't fighting anyone.");
		return;
    }
	
    WAIT_STATE( ch, skill_table[gsn_gouge].beats );
    if ( get_skill(ch,gsn_gouge) > number_percent())
    {
		damage(ch,victim,number_range( 1, ch->level+ch->level/2 ), gsn_gouge,DAM_PIERCE,true);
		
		// chance of poisoning
		if ( saves_spell( ch->level*3/2, victim, DAM_POISON)
			|| HAS_CLASSFLAG(victim, CLASSFLAG_POISON_IMMUNITY))
		{
			act("$n turns slightly green, but it passes.",victim,NULL,NULL,TO_ROOM);
			victim->println("You feel momentarily ill, but it passes.");
			check_improve(ch,gsn_gouge,false,1);
		}else{
			AFFECT_DATA af;
			
			victim->println("You feel poison coursing through your veins.");
			act("$n looks very ill.",victim,NULL,NULL,TO_ROOM);
			
			af.where     = WHERE_AFFECTS;
			af.type      = gsn_poison;
			af.level     = ch->level;
			af.duration  = ch->level/2;
			af.location  = APPLY_ST;
			af.modifier  = -1;
			af.bitvector = AFF_POISON;
			affect_join( victim, &af );
			check_improve(ch,gsn_gouge,true,1);
		}
		
    }
    else
    {
		damage( ch, victim,0, gsn_gouge,DAM_PIERCE,true);
		check_improve(ch,gsn_gouge,false,1);
    }
}


/**************************************************************************/
// written by Kerenos - July 98
void do_gore( char_data *ch, char *argument )
{
    char_data * victim;
    OBJ_DATA  * obj;
    char        arg[MIL];
    int         chance;

    one_argument( argument, arg );

    obj = ( get_eq_char( ch, WEAR_HEAD ));

    if ( !obj && !IS_SET( ch->parts, PART_HORNS )) {
        ch->println("You have no horns to gore with.");
        return;
    }
    
	if ( obj ) {
        if ( !IS_OBJ_STAT( obj, OBJEXTRA_HORNED )) {
            act ( "There are no horns on $p!", ch, obj, NULL, TO_CHAR );
            return;
        }
    }

// standard no mount checks etc
	if(IS_MOUNTED(ch)) 
	{
		ch->println("You can't gore cause you are being ridden.");
		return;
	}
	if(IS_RIDING(ch)) 
	{
		ch->println("You can't gore others while riding a creature.");
		return;
	}

    if (( chance = get_skill( ch, gsn_gore )) == 0
        || ( IS_NPC( ch ) && !IS_SET( ch->off_flags, OFF_GORE))
        || ( !IS_NPC( ch )
			&& ch->level < skill_table[gsn_gore].skill_level[ch->clss])) {        
		ch->println("You are not proficient enough in that skill.");
        return;
    }

    if ( IS_NULLSTR(arg) ) {
        if (( victim = ch->fighting ) == NULL ) {
            ch->println("But you aren't fighting anyone!");
            return;
        }
    }
    else if (( victim = get_char_room( ch, arg )) == NULL ) {
        ch->printlnf("You can't seem to find '%s' here.", arg);
        return;
    }


// Inserted possible cheat deterents

	if ( is_safe( ch, victim ))
		return;

	if ( !can_initiate_combat( ch, victim, CIT_GENERAL )) return;
	
    if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim ) {
		act("But $N is your friend!",ch,NULL,victim,TO_CHAR);
		return;
    }

    WAIT_STATE( ch, skill_table[gsn_gore].beats*2);

    if ( get_skill( ch,gsn_gore ) > number_percent() ) {
        ch->println("You set your head down and charge!");
        if (damage( ch,victim,number_range( ch->level / 10, ch->level / 2 ),
			gsn_gore, DAM_PIERCE, true)) 
		{
			// successful gore
			victim->position = POS_RESTING;
	        WAIT_STATE( victim, skill_table[gsn_gore].beats*2 );
			check_improve( ch, gsn_gore, true, 1 );
		}
		else // unsuccessful gore
		{
			check_improve( ch, gsn_gore, false, 1 );
		}
        if ( number_percent() <= 50 ) {
            ch->println("You charge so wildly that you knock yourself down too.");
            act( "$n knocks himself down after charging you.",
                  ch, NULL, victim, TO_VICT );
            act( "With a wild flourish, $n goes down in a heap.",
                  ch, NULL, victim, TO_NOTVICT );
            ch->position = POS_RESTING;
        }
    }else{
        damage( ch, victim, 0, gsn_gore, DAM_PIERCE, false );
        check_improve( ch, gsn_gore, false, 1 );
		ch->println("You blindly charge past your mark, missing by a mile!");
        act("$n tries to gore $N but fails miserably.",
            ch, NULL, victim, TO_NOTVICT);
        act("$n runs right past you and trips a step later.",
            ch, NULL, victim, TO_VICT );
        check_improve( ch, gsn_gore, false, 1 );
        ch->position = POS_RESTING;        
    }
    return;
}

/**************************************************************************/
void do_subdue(char_data *ch, char *)
{
    if (!ch->fighting){
        ch->println("You aren't currently in combat, use autosubdue"); 
        return;
    }
	
    if (IS_SET(ch->dyn,DYN_CURRENT_SUBDUE))
    {
        ch->println("You will now fight this fight to the death.");
        REMOVE_BIT(ch->dyn,DYN_CURRENT_SUBDUE);
    }
    else
    {
        ch->println("You will attempt to subdue your current opponent.");
        SET_BIT(ch->dyn,DYN_CURRENT_SUBDUE);
    }
}

/**************************************************************************/
// tell a ch to write a pknote for their involvement in the death of victim
void pkill_note_required_message(char_data *ch, char_data *victim, bool bypass_duel_used)
{
	char subject[MSL], buf[MSL], buf2[MSL];

    if(!IS_SET(victim->in_room->room2_flags,ROOM2_ARENA) &&
       !IS_SET(victim->in_room->room2_flags,ROOM2_WAR) )
    {
	if (IS_NPC(ch) || IS_NPC(victim))
	{
		if(IS_NPC(ch)){
			bug("pkill_note_required_message: ch==NPC!");
		}
		if(IS_NPC(victim)){
			bug("pkill_note_required_message: victim==NPC!");
		}
		make_corefile();
	}

	// no pkill notes ever on a dedicated pkill style mud :)
	if(GAMESETTING5(GAMESET5_DEDICATED_PKILL_STYLE_MUD)){
		return;
	}

	if(ch==victim){
		sprintf( subject, "`RPkill note required from %s - was pkilled/subdued`x",ch->name);
	}else{
		sprintf( subject, "`RPkill note required from %s (involved in pk/subdue of %s)`x",	
			ch->name, victim->name);
	}

	// pkstats to imm and noble
	sprintf( buf2, "`1`?`#--===== `YPk status on players after the fight `^=====-`x"
		"`1  %-12s - karns = %d, pkills = %d, pkdefeats = %d, pkool = %d, %s"
		"`1  %-12s - karns = %d, pkills = %d, pkdefeats = %d, pkool = %d, %s"
		"`1  bypass duel used = %s, %s linkdead=%s",
		ch->name, ch->pcdata->karns, ch->pkkills, ch->pkdefeats, ch->pkool, IS_ACTIVE(ch)?"active":"peaceful",
		victim->name, victim->pcdata->karns, victim->pkkills, 
			victim->pkdefeats, victim->pkool, 
			IS_ACTIVE(victim)?"active":"peaceful",
			bypass_duel_used?"true":"false",
			ch->name, ch->desc?"false":"true");

	autonote(NOTE_PKNOTE, "p_anote()", subject, "immpkill noblepkill", buf2, true);

	if(ch==victim){
		if(!codehelp(ch, "pknote_required_from_victim", CODEHELP_ALL_BUT_PLAYERS)){
			ch->wrapln(
			"`WYou have just been involved in a fight that has ended in either your death or subduel. "
			"Player killing is allowed here so long as it is in done within a roleplaying "
			"context.  To ensure all pkills have sufficient IC basis we require you (and all others "
			"involved to write a note to `Ypknote to PKILL`W within the next 2 hours "
			"explaining the IC background of the pkill.  `RPLEASE WRITE AT LEAST 5 LINES!`x`1`1"
			"If you have to chased for a pkill note you will be mooted negatively.`1`1"
			"If you dont consider this kill was totally IC, or someone may claim it wasnt "
			"try to save a log of the fight and events leading up to this in support of your opinion.\r\n");
		}
	}else{
		if(!codehelp(ch, "pknote_required_from_attacker", CODEHELP_ALL_BUT_PLAYERS)){
			ch->wrapln(
			"`WYou have just been involved in a fight that has ended another player being killed or subdued. "
			"Player killing is allowed here so long as it is in done within a roleplaying "
			"context.  To ensure all pkills have sufficient IC basis we require you (and all others "
			"involved to write a note to `=Cpknote to PKILL`W within the next 2 hours "
			"If you have to chased for a pkill note you will be mooted negatively.`1`1"
			"explaining the IC background of the pkill.  `RPLEASE WRITE AT LEAST 5 LINES!`x`1`1"
			"If you dont consider this kill was totally IC, or someone may claim it wasnt "
			"try to save a log of the fight and events leading up to this in support of your opinion.\r\n");
		}
		

	}

	if(GAMESETTING2(GAMESET2_AUTONOTE_IMMPKILLS_TO_ADMIN) 
		&& (IS_IMMORTAL(ch) || !IS_NULLSTR(ch->pcdata->immtalk_name))){
		if(!IS_NULLSTR(ch->pcdata->immtalk_name)){
			sprintf(buf2,"%s (%s) involved in pkill/subduel of %s", 
				ch->name, ch->pcdata->immtalk_name, victim->name);
		}else{
			sprintf(buf2,"%s involved in pkill/subduel of %s", ch->name, victim->name);
		}
		autonote(NOTE_ANOTE, "p_anote()", "Pkill/subdue by immortal/mort with immtalk", "admin", buf2, true);
	}

	if(!GAMESETTING5(GAMESET5_DEDICATED_PKILL_STYLE_MUD)){
		if(!IS_IMMORTAL(ch) && !IS_SET(TRUE_CH(ch)->act, PLR_LOG)){
			SET_BIT(TRUE_CH(ch)->act, PLR_LOG);
			sprintf( buf, "Player log turned ON by doing a pkill");
			append_playerlog( TRUE_CH(ch), buf);
			logf("LOG set on %s by doing a pkill.", TRUE_CH(ch)->name);
		}
		if(!IS_IMMORTAL(victim) && !IS_SET(TRUE_CH(victim)->act, PLR_LOG)){
			SET_BIT(TRUE_CH(victim)->act, PLR_LOG);
			sprintf( buf, "Player log turned ON by doing a pkill");
			append_playerlog( TRUE_CH(victim), buf);
			logf("LOG set on %s by doing a pkill.", TRUE_CH(victim)->name);
		}
	}

    }
}
/**************************************************************************/
// note a pkill and remind the victim and killer to write a note
void pkill_autonote(char_data *ch, char_data *victim)
{
	char subject[MSL], buf[MSL], buf2[MSL];

    if(!IS_SET(victim->in_room->room2_flags,ROOM2_ARENA) &&
       !IS_SET(victim->in_room->room2_flags,ROOM2_WAR) )
    {
	if (IS_NPC(ch) || IS_NPC(victim))
	{
		bug("pkill_autonote: not both players");
		make_corefile();
	}

	sprintf( subject, "`RPkill: %s pkilled %s`x",	ch->name, victim->name);

	// pkstats to imm and noble
	sprintf( buf2, "`1`?`#--===== `YPk status on players after the fight `^=====-`x"
		"`1  %-12s - karns = %d, pkills = %d, pkdefeats = %d, pkool = %d, %s"
		"`1  %-12s - karns = %d, pkills = %d, pkdefeats = %d, pkool = %d, %s",
		ch->name, ch->pcdata->karns, ch->pkkills, ch->pkdefeats, ch->pkool, IS_ACTIVE(ch)?"active":"peaceful",
		victim->name, victim->pcdata->karns, victim->pkkills, 
			victim->pkdefeats, victim->pkool, IS_ACTIVE(victim)?"active":"peaceful");
	autonote(NOTE_PKNOTE, "p_anote()",subject, "immpkill noblepkill", buf2, true);

	// pkinfo to imm only
	sprintf( buf, "`Y%s`x (%s) lvl %d   started_fight=%s`1"
				  "logon how long ago: %s`1"
		          "from '%s' clan: %s`1`Rpkilled`x"
		"`1`Y%s`x (%s) lvl %d    started_fight=%s`1"
		"logon how long ago: %s`1"
		"from '%s' clan: %s`x`1Room: %d (%s in %s)`1"
		"`1"
		"autosub,currentsub  %s=%s,%s   %s=%s,%s`1",
		ch->name,		ch->short_descr,	ch->level, 
		(IS_SET(ch->dyn, DYN_STARTED_FIGHT)?"Yes":"No"), 
		short_timediff(current_time, ch->logon),
		(ch->desc? (ch->desc->remote_hostname?ch->desc->remote_hostname:"???"):"?LD?"),
					ch->clan->cname(),
					victim->name,	
					victim->short_descr, 
					victim->level, 
		(IS_SET(victim->dyn, DYN_STARTED_FIGHT)?"Yes":"No"),
		short_timediff(current_time, victim->logon),
		(victim->desc? (victim->desc->remote_hostname?victim->desc->remote_hostname:"???"):"?LD?"),
					victim->clan->cname(),
					ch->in_room->vnum, 
					ch->in_room->name, 
					ch->in_room->area->name,
					ch->name,
					IS_SET(ch->act,PLR_AUTOSUBDUE)?"ON":"OFF",
					IS_SET(ch->dyn,DYN_CURRENT_SUBDUE)?"ON":"OFF",
					victim->name,
					IS_SET(victim->act,PLR_AUTOSUBDUE)?"ON":"OFF",
					IS_SET(victim->dyn,DYN_CURRENT_SUBDUE)?"ON":"OFF"
		);
	autonote(NOTE_PKNOTE, "p_anote()","pkstatus on last note", "immpkilldetails", buf, true);

	if(GAMESETTING5(GAMESET5_DEDICATED_PKILL_STYLE_MUD)){
		TRUE_CH(ch)->pcdata->p9999kills++;
		TRUE_CH(victim)->pcdata->p9999defeats++;
		if(!IS_IMMORTAL(ch)){
			pkill_broadcast(
                "%s [Pk=%d.Pd=%d.L=%d] `RTOASTED`W %s [Pk=%d.Pd=%d.L=%d] at %s (in %s)",
				TRUE_CH(ch)->name,
				TRUE_CH(ch)->pcdata->p9999kills,
				TRUE_CH(ch)->pcdata->p9999defeats,
                TRUE_CH(ch)->level,
				TRUE_CH(victim)->name,
				TRUE_CH(victim)->pcdata->p9999kills,
				TRUE_CH(victim)->pcdata->p9999defeats,
                TRUE_CH(victim)->level,
				TRUE_CH(ch)->in_room->name,
				TRUE_CH(ch)->in_room->area->name);
		}

		// record max pkill
		if (p9999maxpk<TRUE_CH(ch)->pcdata->p9999kills){
			if (IS_IMMORTAL(ch)){
				pkill_broadcast("Immortal pkill record of %s not saved!",ch->name);
			}else{
				if (str_cmp(p9999maxpkname,TRUE_CH(ch)->name)){
					pkill_broadcast("PKILL RECORD BROKEN BY %s!",ch->name);
                    strcpy(p9999maxpkname,TRUE_CH(ch)->name);
                    p9999maxpklevel=TRUE_CH(ch)->level;
				}
				p9999maxpk=TRUE_CH(ch)->pcdata->p9999kills;
			}
		}
		// record max defeat
		if (p9999maxpd<TRUE_CH(victim)->pcdata->p9999defeats){
			if (str_cmp(p9999maxpdname,TRUE_CH(victim)->name)){
                pkill_broadcast("PKDEFEAT RECORD BROKEN BY %s, congratulations %s!",
					victim->name, ch->name);
				strcpy(p9999maxpdname,TRUE_CH(victim)->name);
			}
            p9999maxpdlevel=TRUE_CH(victim)->level;
			p9999maxpd=TRUE_CH(victim)->pcdata->p9999defeats;
		}
	}
   }
}

// MOB MEMORY STUFF

bool mobRememberCH( char_data *ch, char_data *victim )
{
	if ( !ch->mobmemory || ch->mobmemory != victim )
		return false;
	return true;
}

void mobRememberClear( char_data *ch )
{
	ch->mobmemory = NULL;
	
	return;
}

void mobRememberSet( char_data *ch, char_data *victim )
{
	if ( !IS_NEWBIE( ch ))
		ch->mobmemory = victim;

	return;
}
/**************************************************************************/
void do_becomeactive(char_data *ch, char *argument)
{
    if(IS_NPC(ch))
		return;

	// Check if they're being ordered to do this
	if ( IS_SET( ch->dyn, DYN_IS_BEING_ORDERED ))
	{
		if ( ch->master ){
			ch->master->println( "Not going to happen.");
		}
		return;
	}

	if(!GAMESETTING(GAMESET_NO_LETGAINING_REQUIRED) && !IS_LETGAINED(ch)){
		ch->println("You can't use this command unless letgained.");
		return;
	}

	if( IS_ACTIVE(ch)){
		ch->println("You are already an active player.");
		return;
	};

	if(GAMESETTING(GAMESET_PEACEFUL_MUD)){
		ch->println("The game is locked in peaceful mode, as a result you can't becomeactive.");
		return;
	}

	if(GAMESETTING2(GAMESET2_NO_DUEL_REQUIRED)){
		ch->println("The duel system is not enabled on this mud, so becoming active is pointless.");
		return;
	}

	if(HAS_CONFIG2(ch,CONFIG2_NOPKILL))
	{
		ch->println( "You are not allowed to become active." );
		return;
	}
 
	if ( str_cmp("confirm", argument)) {
		if(!codehelp(ch, "do_becomeactive_noargument", CODEHELP_ALL_BUT_PLAYERS)){
			ch->println("If you want to enable your active status type:");
			ch->println("  `=Cbecomeactive confirm`x.");
			ch->println("`RBE WARNED: `xThe active status once on is not able to be removed.");
		};
		return;
	}

	SET_CONFIG(ch, CONFIG_ACTIVE);	
	if(!codehelp(ch, "do_becomeactive_enabled", CODEHELP_ALL_BUT_PLAYERS)){
		ch->println("Your active status has been activated.  This can not be turned off.");
	}

	if(ch->level<=10){
		SET_BIT(ch->act, PLR_NOREDUCING_MAXKARN);
		ch->wrapln("Because you have changed from peaceful to active before level 10, the maximum "
			"number of karns you can attain will always be 5 regardless of the number of "
			"pkills you have been involved in.");
	}
	
	save_char_obj(ch);
}
/**************************************************************************/
bool mp_prekill_trigger( char_data *mob, char_data *attacker);
/**************************************************************************/
// doesn't display any message to anyone
bool can_initiate_combat_with_player( char_data *attacker, char_data *victim, int type)
{
	assert(!IS_NPC(victim));
	// prevent ordering starting pk combat
	if( !IS_NPC(victim) && IS_SET( attacker->dyn, DYN_IS_BEING_ORDERED ))
	{		
		if(attacker->master && !IS_NPC(attacker->master)){
			if(!can_initiate_combat(attacker->master, victim, type)){
//				attacker->println("You can't be ordered to initiate PK combat unless you master can attack them.");
//				attacker->master->println("You can't ordered people or mobs to initiate PK combat unless you can attack the target!" );
				return false;
			}
		}else{
//			attacker->println("You can't be ordered to initiate PK combat.");
			return false;
		}
	}

	if(IS_NPC(attacker)){ // nonordered mobs shouldn't get hindered by the code below
		return true;
	}

// Arena Rooms
        if(IS_SET(victim->in_room->room2_flags,ROOM2_ARENA) )
	{
        	return true;
	}

// War Battlefield Rooms
        if(IS_SET(victim->in_room->room2_flags,ROOM2_WAR) )
	{
        	return true;
	}

	if(GAMESETTING5(GAMESET5_DEDICATED_PKILL_STYLE_MUD)){
		return true;
	}

	// PREVENT KILL STEALING 
	if(IS_SET(type,CIT_GENERAL)){
		if ( IS_NPC(victim) &&
			victim->fighting != NULL &&
			!is_same_group(attacker,victim->fighting))
		{
//			attacker->println("Kill stealing is not permitted.");
			return false;
		}
	};

	if(IS_NPC(victim)){ // mobs dont require duelling
		return true;
	}
	
	if(victim==attacker){ // casting slow on yourself etc
		return true;
	}

	if(GAMESETTING2(GAMESET2_NO_DUEL_REQUIRED)){
		return true;
	}
 
	if(IS_SET(type,(CIT_GENERAL | CIT_AGRESSIVE_ACTION | CIT_CASTING_SPELL))){

		// pk duelsystem checks - only apply to morts
		if(    !IS_ICIMMORTAL(attacker))
		{
			// bypass duel system (spells can't make use of it)
			if(attacker->duels 
				&& !IS_SET(type,CIT_CASTING_SPELL) 
				&& !IS_SET(type,CIT_NO_BYPASSDUEL) 
				&& attacker->duels->is_bypassingduel(victim))
			{
				return true;
			}

			if(IS_ACTIVE(victim)){
				// check for peaceful attempting to attack an active player
				if(IS_PEACEFUL(attacker)){
					// peaceful must either bypassduel to attack a pk (or duel if they have been stealing)
					if(!attacker->duels || !attacker->duels->is_known(victim)){
						return false;
					}
					if(!attacker->duels->is_dueling(attacker, victim)){
						return false;
					}
					return true;
				}
			}else{
				// The victim is peaceful, duel is required regardless of the 
				// pk status of the attacker
				if(!attacker->duels || !attacker->duels->is_known(victim)){
					return false;
				}
				if(!attacker->duels->is_dueling(attacker, victim)){
					return false;
				}
				return true;
			}
		}
	}
	return true;
}
/**************************************************************************/
bool can_initiate_combat( char_data *attacker, char_data *victim, int type)
{
	// prekill trigger
	if ( IS_NPC(victim) &&  HAS_TRIGGER( victim, TRIG_PREKILL ) ){
		if(mp_prekill_trigger( victim, attacker )){
			return false;
		};
	}

	// nonordered mobs shouldn't get hindered by the code below
	if(IS_NPC(attacker) && !IS_SET( attacker->dyn, DYN_IS_BEING_ORDERED )){ 
		return true;
	}

// Arena Rooms
        if(IS_SET(attacker->in_room->room2_flags,ROOM2_ARENA) )
	{
        	return true;
	}

// War Battlefield Rooms
        if(IS_SET(attacker->in_room->room2_flags,ROOM2_WAR) )
	{
        	return true;
	}

	if(GAMESETTING5(GAMESET5_DEDICATED_PKILL_STYLE_MUD)){
		return true;
	}

	// attacking players pet checks
    if ( IS_NPC(victim)
		&& IS_SET(victim->act, ACT_PET) 
		&& victim->leader 
		&& !IS_NPC(victim->leader) 
		&& (victim->leader->pet == victim)
		&& IS_SET(victim->affected_by, AFF_CHARM))
	{
		if(!can_initiate_combat_with_player( attacker, victim->leader, (type | CIT_NO_BYPASSDUEL))){
		// can't attack, inform of situation
			// prevent ordering starting pk combat
			if(!IS_NPC(victim) && IS_SET( attacker->dyn, DYN_IS_BEING_ORDERED ))
			{		
				if(attacker->master && !IS_NPC(attacker->master)){
					if(!can_initiate_combat(attacker->master, victim, type)){
						if(!IS_SET(type,CIT_SILENT)){
							attacker->println("You can't be ordered to initiate PK combat with another players pet\r\n"
								"unless your master can attack the master of the pet you are attacking." );
							attacker->master->println("You can't ordered people or mobs to initiate combat with another players pet\r\n"
								"unless you can attack the owner of the pet yourself!" );
						}
						return false;
					}
				}else{
					if(!IS_SET(type,CIT_SILENT)){
						attacker->println("You can't be ordered to initiate PK combat with another players pet.");
					}
					return false;
				}
			}
			if(!IS_SET(type,CIT_SILENT)){
				attacker->master->println("You can't initiate combat with another players pet unless you can\r\n"
					"attack the owner of the pet yourself (without using bypass duel)!" );
			}
			return false;
		}
	}

	// prevent ordering starting pk combat
	if(!IS_NPC(victim) && IS_SET( attacker->dyn, DYN_IS_BEING_ORDERED ))
	{		
		if(attacker->master && !IS_NPC(attacker->master)){
			if(!can_initiate_combat_with_player(attacker->master, victim, type)){
				if(!IS_SET(type,CIT_SILENT)){
					attacker->println("You can't be ordered to initiate PK combat unless your master can attack them.");
					attacker->master->println("You can't ordered people or mobs to initiate PK combat unless you can attack the target!" );
				}
				return false;
			}
		}else{
			if(!IS_SET(type,CIT_SILENT)){
				attacker->println("You can't be ordered to initiate PK combat.");
			}
			return false;
		}
	}

	if(IS_SET(type,CIT_GENERAL)){
		// kill stealing check
		if ( IS_NPC(victim) &&
			victim->fighting != NULL &&
			!is_same_group(attacker,victim->fighting))
		{
			if(!IS_SET(type,CIT_SILENT)){
				attacker->println("Kill stealing is not permitted.");
			}
			return false;
		}
	};

	if(IS_NPC(victim)){ // mobs dont require duelling
		return true;
	}
	
	if(victim==attacker){ // casting slow on yourself etc
		return true;
	}


	if(!GAMESETTING2(GAMESET2_NO_DUEL_REQUIRED) 
		&& IS_SET(type,(CIT_GENERAL | CIT_AGRESSIVE_ACTION | CIT_CASTING_SPELL))){

		// pksystem checks - only apply to morts
		if( !IS_ICIMMORTAL(attacker))
		{
			// bypass duel system (spells can't make use of it)
			if(attacker->duels 
				&& !IS_SET(type,CIT_NO_BYPASSDUEL) 
				&& !IS_SET(type,CIT_CASTING_SPELL) 
				&& attacker->duels->is_bypassingduel(victim))
			{
				return true;
			}

			if(IS_ACTIVE(victim)){
				// check for peaceful attempting to attack an active player
				if(IS_PEACEFUL(attacker)){
					// peaceful must bypassduel (or duel a thief) to attack an active player
					if(!attacker->duels || !attacker->duels->is_known(victim)){
						if(!IS_SET(type,CIT_SILENT)){
							attacker->printlnf("%s is an active player, because you are a peaceful character, your options\r\n"
								"to initiate conflict with them are one of the following:\r\n"
								"  1. Become an active player yourself.\r\n"
								"  2. Bypassduel the duelling system.\r\n"
								"  3. If they have been stealing in the last 20 minutes, you may challenge them to a duel.",
								CPERS(victim, attacker));
						}
						return false;
					}
					if(!attacker->duels->is_dueling(attacker, victim)){
						if(!IS_SET(type,CIT_SILENT)){
							attacker->printlnf("You can't initiate conflict with %s at this point in time.",
								PERS(victim, attacker));
						}
						return false;
					}
					return true;
				}
			}else{
				// The victim is peaceful, duel is required regardless of the 
				// pk status of the attacker
				if(!attacker->duels || !attacker->duels->is_known(victim)){
					if(!IS_SET(type,CIT_SILENT)){
						attacker->println("You must duel them or bypass the dueling system before you attempt to initiate conflict!");
					}
					return false;
				}
				if(!attacker->duels->is_dueling(attacker, victim)){
					if(!IS_SET(type,CIT_SILENT)){
						attacker->printlnf("You can't initiate conflict with %s at this point in time.",
							PERS(victim, attacker));
					}
					return false;
				}
				return true;
			}
		}
	}
	return true;
}
/**************************************************************************/
// return true if the attacker can join the current combat (autoassist)
// - handles all the duel/bypass duel rules etc
bool can_join_combat( char_data *attacker, char_data *victim)
{
	// can always join in when fighting a mob
	// pets can always join 
	// can always join in on a dedicated pkill mud
	if(	   IS_NPC(attacker) 
		|| IS_NPC(victim) 
		|| GAMESETTING5(GAMESET5_DEDICATED_PKILL_STYLE_MUD)){
		return true;
	}

// Arena Rooms
        if(IS_SET(attacker->in_room->room2_flags,ROOM2_ARENA) )
	{
        	return true;
	}


// War Battlefield Rooms
        if(IS_SET(attacker->in_room->room2_flags,ROOM2_WAR))
	{
        	return true;
	}

	if(can_initiate_combat_with_player( attacker, victim, CIT_GENERAL)){
		return true;
	};

// Rules for group combat
//===attacker===victim=======condition===
//	* active	active		- can always
//	* active	peaceful	- normal + join in if victim->fighting is duelling victim && same group as victim->fighting
//	* peaceful	active		- normal + join in if victim->fighting is duelling victim && same group as victim->fighting
//	* peaceful	peaceful	- normal + join in if victim->fighting is duelling victim && same group as victim->fighting

	if(	victim->fighting 
		&& victim->fighting->fighting==victim
		&& is_same_group(victim->fighting, attacker) 
		&& victim->fighting->duels 
		&& victim->fighting->duels->is_dueling(victim->fighting, victim))
	{
		return true;
	}

	attacker->println("You need to duel or `=Cbypassduel`x to join in.");
	return false;
	
}
/**************************************************************************/
// able to avoid combat by fading
bool check_fade( char_data* ch, char_data* victim )
{
    int chance;

    if ( IS_NPC(victim) && !IS_SET(victim->off_flags, OFF_FADE) ){
        return false;
	}

    if ( !IS_NPC(victim) && !IS_AFFECTED2(victim, AFF2_FADE) ){
		return false;
	}

    if ( !IS_AWAKE(victim) ){
        return false;
	}

    chance = get_skill(victim, gsn_fade) / 3;

    if ( number_percent( ) >= chance + victim->level - ch->level){
        return false;
	}

    if (victim->is_stunned){
		return false;
	}

    act( "`bYou fade around $n's attack.`x", ch, NULL, victim, TO_VICT );
    act( "`b$n fades around your attack.`x", victim, NULL, ch, TO_VICT );
    check_improve(victim, gsn_fade, true, 6);
    return true;
}
/**************************************************************************/
int switch_update(char_data *ch)
{
  char_data *vch, *vch_next;
  /* switch for group */
  for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
      vch_next =vch->next_in_room;
      if ((vch->fighting == ch)&&(vch != ch->fighting)) {
	stop_fighting(ch,false);
	set_fighting(ch,vch);
	return(0);
      }
    }
  return(0);
}

/**************************************************************************/
void do_shoot( char_data *ch, char *argument )
{
    char arg[MIL];
    char arg2[MIL];
    char_data *victim;
    OBJ_DATA *gun, *obj;
    char *mode;
    int shots, number, recoil;

	if(!IS_NPC(ch) && GAMESETTING(GAMESET_PEACEFUL_MUD)){
		ch->println("This is a peaceful mud... pkilling is not permitted.");
		return;
	}

	if(IS_MOUNTED(ch)){
		ch->println("You can't shoot cause you are being ridden.");
		return;
	}

    argument = one_argument( argument, arg);
    argument = one_argument( argument, arg2 );

    mode = arg;
	
    if (arg2[0] == '\0' && ch->fighting == NULL)
    {
		ch->println("Shoot at whom or what ?");
		return;
    }

    if ( arg[0] == '\0' && ch->fighting == NULL)
    {
	ch->println ("Which mode ?");
	return;
    }
	
    else if ((victim = get_char_room(ch,arg2)) == NULL)
    {
		ch->println("They aren't here.");
		return;
    }
	
    if ( victim == ch )
    {
		ch->println("Suidice is a mortal sin.");
		return;
    }

    if ( ( gun = get_eq_char( ch, WEAR_WIELD ) ) == NULL)
    {
		ch->println("You need to wield a firearm to shoot.");
		return;
    }

    if (gun->item_type != ITEM_FIREARM)
    {
    	ch->println("You need to wield a firearm to shoot.");
	return;	
    }

    obj = NULL;
    if ( arg2[0] == '\0' )
    {
	if ( ch->fighting != NULL )
	{
	    victim = ch->fighting;
	}
	else
	{
	    ch->println( "Shoot whom or what?");
	    return;
	}
    }
    else
    {
	if ( ( victim = get_char_room ( ch, arg2 ) ) == NULL
	&&   ( obj    = get_obj_here  ( ch, arg2 ) ) == NULL )
	{
	    ch->println( "You can't find it.");
	    return;
	}
    }

    if ( ( victim = get_char_room ( ch, arg2 ) ) != NULL)
    {
    	if ( !can_initiate_combat( ch, victim, CIT_GENERAL )) return;

	if(!IS_SET(victim->in_room->room2_flags,ROOM2_ARENA) && !IS_SET(victim->in_room->room2_flags,ROOM2_WAR) )
	{

		// log it if is a pkill attack
	      	if ( !IS_NPC(victim) )
		{	
			char log_buf[MSL];
		
        		sprintf( log_buf, "Log %s: shoot - (found %s)", 
			PERS(ch,NULL), PERS(victim,NULL));

        		log_string( log_buf );
        		wiznet(log_buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
      		}
   	}
	
        if ( is_safe( ch, victim ) )
		return;
    } 


    if ( gun->value[1] <= 0 )
    {
	act( "The $p seems to be out of ammo.", ch, gun, NULL, TO_CHAR );
	gun->value[1] = 0;
	return;
    }

	if (( !str_cmp (mode, "dual")) && (IS_SET(gun->value[4], FIREARM_DUAL)))
	shots = 2;
	else if (( !str_cmp (mode, "burst")) && (IS_SET(gun->value[4], FIREARM_BURST)))
	shots = 3;
	else if (( !str_cmp (mode, "auto")) && (IS_SET(gun->value[4], FIREARM_AUTO)))
	shots = 5;
	else if (( !str_cmp (mode, "single")) && (IS_SET(gun->value[4], FIREARM_SINGLE)))
	shots = 1;
	else
	{
		ch->println ("That is not an accepted mode for that weapon.");
		return;
	}

	for(number = 0 ; number < shots ; number = number + 1)
	{
		if ( victim != NULL && victim->position == POS_DEAD)
			return;
		if ( ( victim = get_char_room ( ch, arg2 ) ) == NULL)
			return;
		gun->value[1] -= 1;
 		if (number_percent() <= get_skill(ch,gsn_firearms))
		{
			if ( victim != NULL && ( victim = get_char_room ( ch, arg2 ) ) != NULL)
			{
				act( "$n shoots $N with $p.", ch, gun, victim, TO_NOTVICT );
				act( "You shoot $N with $p.", ch, gun, victim, TO_CHAR );
				act( "$n shoots at you with $p.",ch, gun, victim, TO_VICT );
				obj_cast_spell( gun->value[2], gun->level, ch, victim, NULL );

			}
			else
			{
    				act( "$n shoots $P with $p.", ch, gun, obj, TO_ROOM );
    				act( "You shoot $P with $p.", ch, gun, obj, TO_CHAR );
				obj_cast_spell( gun->value[2], gun->level, ch, victim, obj );
			}
		}
		else
		{
    			act( "Your miss the target.", ch,gun,NULL,TO_CHAR);
    			act( "$n misses with a shot from $p.", ch,gun,NULL,TO_ROOM);
    			check_improve(ch,gsn_firearms,false,2);
			return;
		}

		recoil = 0;

		if (IS_SET(gun->value[4], FIREARM_RECOIL_NONE) && ch->perm_stats[STAT_ST] < 25)
			recoil = 1;
		if (IS_SET(gun->value[4], FIREARM_RECOIL_LIGHT) && ch->perm_stats[STAT_ST] < 25)
			recoil = 2;
		if (IS_SET(gun->value[4], FIREARM_RECOIL_HEAVY) && ch->perm_stats[STAT_ST] < 25)
			recoil = 3;
		if (IS_SET(gun->value[4], FIREARM_RECOIL_EXTREME) && ch->perm_stats[STAT_ST] < 25)
			recoil = 4;
		if (IS_SET(gun->value[4], FIREARM_RECOIL_LIGHT) && ch->perm_stats[STAT_ST] < 45)
			recoil = 1;
		if (IS_SET(gun->value[4], FIREARM_RECOIL_HEAVY) && ch->perm_stats[STAT_ST] < 45)
			recoil = 2;
		if (IS_SET(gun->value[4], FIREARM_RECOIL_EXTREME) && ch->perm_stats[STAT_ST] < 45)
			recoil = 3;
		if (IS_SET(gun->value[4], FIREARM_RECOIL_HEAVY) && ch->perm_stats[STAT_ST] < 75)
			recoil = 1;
		if (IS_SET(gun->value[4], FIREARM_RECOIL_EXTREME) && ch->perm_stats[STAT_ST] < 75)
			recoil = 2;
		if (IS_SET(gun->value[4], FIREARM_RECOIL_EXTREME) && ch->perm_stats[STAT_ST] < 100)
			recoil = 1;


		if (recoil == 1)		
		{
			ch->println ("The recoil slows you down a bit.");
			WAIT_STATE(ch,1 * PULSE_VIOLENCE);
		}
		if (recoil == 2)
		{	
			ch->println ("The recoil sends throws you off balance.");
			WAIT_STATE(ch,2 * PULSE_VIOLENCE);
		}
		if (recoil == 3)
		{
			ch->println ("The strong recoil almost numbs your arm.");
			WAIT_STATE(ch,3 * PULSE_VIOLENCE);
		}
		if (recoil == 4)
		{	
			ch->println ("The recoil almost shatters your hand.");
			WAIT_STATE(ch,4 * PULSE_VIOLENCE);
		}

	}

    return;
}

void do_reload( char_data *ch, char *argument )
{
    char arg1[MIL];
    char arg2[MIL];
    OBJ_DATA *ammo;
    OBJ_DATA *gun;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    /* not the ammo the character thinks he has */
    if ( ( ammo = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	ch->println( "You do not have that kind of ammo.");
	ch->println( "`#`BSyntax: `creload <ammo> <weapon>`^");
	return;
    }

    /* nope, that item cannot be loaded into a gun */
    if ( ammo->item_type != ITEM_AMMO )
    {
	ch->println( "You need ammo to reload.");
	ch->println( "`#`BSyntax: `creload <ammo> <weapon>`^");
	return;
    }

    if ( arg2[0] == '\0' )
    {
	ch->println( "You need a gun to reload.");
	ch->println( "`#`BSyntax: `creload <ammo> <weapon>`^");
	return;
    }
    else
    {
	if   (( gun    = get_obj_here  ( ch, arg2 ) ) == NULL)
	{
	    ch->println( "You can't find that gun.");
	    ch->println( "`#`BSyntax: `creload <ammo> <weapon>`^");
	    return;
	}
    }

    if (gun->item_type != ITEM_FIREARM)
    {
    	ch->println("You can only reload a gun.");
	ch->println( "`#`BSyntax: `creload <ammo> <weapon>`^");
	return;	
    }

	if (gun->value[3] != ammo->pIndexData->vnum)
	{
	    ch->println( "Thats the wrong ammo for that weapon.");
	    return;
	}
	
	
	gun->value[1] += ammo->value[0];

	if (gun->value[1] > gun->value[0])
	{
		gun->value[1] = gun->value[0];
	}

    	act( "$n reloads $p.", ch, gun, NULL, TO_ROOM );
    	act( "You reload $p.", ch, gun, NULL, TO_CHAR );
 

    	extract_obj( ammo );
    	return;
}

/*************************************************************************/
OBJ_DATA *shape_corpse(char_data *ch, obj_data *obj)
{
	char description[MIL];
	int last_damage[2];
    	EXTRA_DESCR_DATA *ed;


	if(ch->last_damage[0] >= 1000)
	{
		last_damage[0] = UMAX(ch->last_damage[0] - 1000, 0);
		last_damage[1] = ch->last_damage[1];
	}else{
		last_damage[0] = UMAX(ch->last_damage[1] - 1000, 0);
		last_damage[1] = ch->last_damage[0];
	}
	
	
	//Set up corpse flags for necromancy
	if(is_affected( ch, gsn_poison))
		SET_BIT(obj->value[1], CORPSE_POISONED);

	if(is_affected( ch, gsn_plague))
		SET_BIT(obj->value[1], CORPSE_PLAGUED);

	if((last_damage[0]) == DAM_HOLY)
		SET_BIT(obj->value[1], CORPSE_SEALED);

	strcpy(description, "This corpse has seen better days. ");
	
	switch(last_damage[0])
	{
		case DAM_NONE : 
			strcat(description, "There are no apparent signs of the cause of death");
			SET_BIT(obj->corpse_flags, CORPSE_DAM_NONE);
			if(last_damage[1] > 0)
				strcat(description, " but there are ");
			else
				strcat(description, ". ");
			break;
		case DAM_BASH : 
			strcat(description, "Most the body is covered with bruises and batter marks");
			SET_BIT(obj->corpse_flags, CORPSE_DAM_BASH);
			if(last_damage[1] > 0)
				strcat(description, " and a few areas are ");
			else
				strcat(description, ". ");
			break;
		case DAM_PIERCE : 
			strcat(description, "Throughout the corpse are puncture marks");
			SET_BIT(obj->corpse_flags, CORPSE_DAM_PIERCE);
			if(last_damage[1] > 0)
				strcat(description, " and portions of the body are ");
			else
				strcat(description, ". ");
			break;
		case DAM_SLASH : 
			strcat(description, "Lacerations crisscross the flesh of this corpse");
			SET_BIT(obj->corpse_flags, CORPSE_DAM_SLASH);
			if(last_damage[1] > 0)
				strcat(description, " and even a few places are ");
			else
				strcat(description, ". ");
			break;
		case DAM_FIRE : 
			strcat(description, "Layers of flesh peel off of the crispy corpse");
			SET_BIT(obj->corpse_flags, CORPSE_DAM_FIRE);
			if(last_damage[1] > 0)
				strcat(description, " and ");
			else
				strcat(description, ". ");
			break;
		case DAM_COLD : 
			strcat(description, "The frozen flesh on this body cracks");
			SET_BIT(obj->corpse_flags, CORPSE_DAM_COLD);
			if(last_damage[1] > 0)
				strcat(description, " and ");
			else
				strcat(description, ". ");
			break;
		case DAM_LIGHTNING : 
			strcat(description, "Jolts of energy flicker over the corpse occasionally");
			SET_BIT(obj->corpse_flags, CORPSE_DAM_LIGHTNING);
			if(last_damage[1] > 0)
				strcat(description, " and ");
			else
				strcat(description, ". ");
			break;
		case DAM_ACID : 
			strcat(description, "Much of this body has been partially melted");
			SET_BIT(obj->corpse_flags, CORPSE_DAM_ACID);
			if(last_damage[1] > 0)
				strcat(description, " and ");
			else
				strcat(description, ". ");
			break;
		case DAM_POISON : 
			strcat(description, "A green pallor colors most the extremities");
			SET_BIT(obj->corpse_flags, CORPSE_DAM_POISON);
			if(last_damage[1] > 0)
				strcat(description, " and ");
			else
				strcat(description, ". ");
			break;
		case DAM_NEGATIVE : 
			strcat(description, "The very flesh of this creature has been twisted");
			SET_BIT(obj->corpse_flags, CORPSE_DAM_NEGATIVE);
			if(last_damage[1] > 0)
				strcat(description, " and ");
			else
				strcat(description, ". ");
			break;
		case DAM_HOLY : 
			strcat(description, "A serene aura surrounds this corpse radiating out");
			SET_BIT(obj->corpse_flags, CORPSE_DAM_HOLY);
			if(last_damage[1] > 0)
				strcat(description, " and ");
			else
				strcat(description, ". ");
			break;
		case DAM_ENERGY : 
			strcat(description, "A tingling sensation emanates from this body");
			SET_BIT(obj->corpse_flags, CORPSE_DAM_ENERGY);
			if(last_damage[1] > 0)
				strcat(description, " and ");
			else
				strcat(description, ". ");
			break;
		case DAM_MENTAL : 
			strcat(description, "There are no apparent signs of the cause of death");
			SET_BIT(obj->corpse_flags, CORPSE_DAM_MENTAL);
			if(last_damage[1] > 0)
				strcat(description, " but there are ");
			else
				strcat(description, ". ");
			break;
		case DAM_DISEASE : 
			strcat(description, "Black spots have enveloped this corpse");
			SET_BIT(obj->corpse_flags, CORPSE_DAM_DISEASE);
			if(last_damage[1] > 0)
				strcat(description, " and ");
			else
				strcat(description, ". ");
			break;
		case DAM_DROWNING : 
			strcat(description, "Bloatmarks and a faint bluish sheen are spread over the body");
			SET_BIT(obj->corpse_flags, CORPSE_DAM_DROWNING);
			if(last_damage[1] > 0)
				strcat(description, " and ");
			else
				strcat(description, ". ");
			break;
		case DAM_LIGHT : 
			strcat(description, "There are no apparent signs of the cause of death");
			SET_BIT(obj->corpse_flags, CORPSE_DAM_LIGHT);
			if(last_damage[1] > 0)
				strcat(description, " but there are ");
			else
				strcat(description, ". ");
			break;
		case DAM_OTHER : 
			strcat(description, "There are no apparent signs of the cause of death");
			SET_BIT(obj->corpse_flags, CORPSE_DAM_OTHER);
			if(last_damage[1] > 0)
				strcat(description, " but there are ");
			else
				strcat(description, ". ");
			break;
		case DAM_HARM : 
			strcat(description, "The very flesh of this creature has been twisted ");
			SET_BIT(obj->corpse_flags, CORPSE_DAM_HARM);
			if(last_damage[1] > 0)
				strcat(description, " and ");
			else
				strcat(description, ". ");
			break;
		case DAM_CHARM : 
			strcat(description, "There are no apparent signs of the cause of death");
			SET_BIT(obj->corpse_flags, CORPSE_DAM_CHARM);
			if(last_damage[1] > 0)
				strcat(description, " but there are ");
			else
				strcat(description, ". ");
			break;
		case DAM_SOUND : 
			strcat(description, "Most the body is covered with bruises and batter marks");
			SET_BIT(obj->corpse_flags, CORPSE_DAM_SOUND);
			if(last_damage[1] > 0)
				strcat(description, " and a few areas are ");
			else
				strcat(description, ". ");
			break;
		case DAM_ILLUSION : 
			strcat(description, "There are no apparent signs of the cause of death");
			SET_BIT(obj->corpse_flags, CORPSE_DAM_ILLUSION);
			if(last_damage[1] > 0)
				strcat(description, " but there are ");
			else
				strcat(description, ". ");
			break;
	}

	switch(last_damage[1])
	{
		case DAM_BASH: strcat( description, "batter marks scattered everywhere. ");
					   break;
		case DAM_PIERCE: strcat( description, "gouges splayed across it. ");
					   break;
		case DAM_SLASH: strcat( description, "lacerations spread across the body. ");
					   break;
		case DAM_FIRE: strcat( description, "burn marks almost here and there. ");
					   break;
		case DAM_COLD: strcat( description, "frostbite on the extremities. ");
					   break;
		case DAM_LIGHTNING: strcat( description, "a smell of ozone permeates the area around it. ");
					   break;
		case DAM_ACID: strcat( description, "signs of deterioration on its flesh. ");
					   break;
		case DAM_POISON: strcat( description, "hives on the features of the corpse. ");
					   break;
		case DAM_NEGATIVE: strcat( description, "unholy distortions twisting this body. ");
					   break;
		case DAM_HOLY: strcat( description, "areas completely untouched by rot. ");
					   break;
		case DAM_ENERGY: strcat( description, "golden sparks jumping from the skin. ");
					   break;
		case DAM_MENTAL: strcat( description, "every muscle is clenched. ");
					   break;
		case DAM_DISEASE: strcat( description, "all of the extremities are dotted black. ");
					   break;
		case DAM_DROWNING: strcat( description, "patches of skin are bloated and wet. ");
					   break;
		case DAM_LIGHT: strcat( description, "severe flashburns across its surface. ");
					   break;
		case DAM_OTHER: strcat( description, "odd indentations over the thing. ");
					   break;
		case DAM_HARM: strcat( description, "horrible yellowing has developed upon it. ");
					   break;
		case DAM_CHARM: strcat( description, "every muscle has relaxed. ");
					   break;
		case DAM_SOUND: strcat( description, "the body vibrates constantly. ");
					   break;
		case DAM_ILLUSION: strcat( description, "the body wavers slightly. ");
					   break;
	}


//	sprintf( buf, corpse->short_descr, name );
//	replace_string( corpse->short_descr, buf );
//	sprintf( buf, corpse->description, name );
//	replace_string( corpse->description, buf);
//	sprintf(name, "corpse %s", ch->name);
//	obj->name = str_dup(name);
//	sprintf(short_descr, "the %scorpse of %s", corpse_dam_renames[last_damage[0]].name, ch->short_descr);
//	obj->short_descr = str_dup(short_descr);
//	strcat(short_descr, " is here.");
//	obj->description = str_dup(capitalize(short_descr));

	ed = new_extra_descr();
        ed->keyword         = str_dup( obj->name  );
        ed->description     = str_dup( description );
        ed->next            = obj->extra_descr;
        obj->extra_descr    = ed;
	return obj;
}	
/**************************************************************************/
void do_rearkick( char_data *ch, char *argument )
{
	char arg[MIL];
	char_data *victim;
	int chance;
	int dr; // damage result
	
	one_argument(argument,arg);
	
	if ( (chance = get_skill(ch,gsn_rearkick)) == 0
		||      (!IS_NPC(ch)
		&&       ch->level < skill_table[gsn_rearkick].skill_level[ch->clss]))
	{
		if (!IS_CONTROLLED(ch))
		{
			ch->println("How would you possibly kick with your rear legs?");
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
	
	if (victim->position < POS_FIGHTING)
	{
		act("You'll have to let $M get back up first.",ch,NULL,victim,TO_CHAR);
		return;
	}

	if (victim == ch)
	{
		ch->println("You just can't bend that way. Sorry.");
		return;
	}

	if (IS_AFFECTED(victim, AFF_FLYING) && !IS_AFFECTED(ch, AFF_FLYING))
	{
		ch->println("You can't reach them since they are flying and you are not.");
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
	
    // modifiers 
	
    // size  and weight 
    chance += ch->carry_weight / 250;
    chance -= victim->carry_weight / 200;
	
    if (ch->size < victim->size){
		chance += (ch->size - victim->size) * 15;
    }else{
		chance += (ch->size - victim->size) * 10; 
	}
	
	
    // stats 
    chance += ch->modifiers[STAT_ST];
    chance -= victim->modifiers[STAT_QU];
    chance -= GET_AC(victim,AC_BASH) /25;
    // speed
    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
		chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
		chance -= 30;
	
    // level 
    chance += (ch->level - victim->level);
	
    if (IS_CONTROLLED(ch)){
        chance*=3;
    };
	
    if (!IS_NPC(victim) 
		&& chance < get_skill(victim,gsn_dodge) )
		{   /*
			act("$n tries to bash you, but you dodge it.",ch,NULL,victim,TO_VICT);
			act("$N dodges your bash, you fall flat on your face.",ch,NULL,victim,TO_CHAR);
			WAIT_STATE(ch,skill_table[gsn_bash].beats);
		return;*/
		chance -= 3 * (get_skill(victim,gsn_dodge) - chance);
    }
	
    // now the attack 
    if (number_percent() < chance )
    {	
		DAZE_STATE(victim, 3 * PULSE_VIOLENCE);
		WAIT_STATE(ch,skill_table[gsn_rearkick].beats);
		victim->position = POS_RESTING;
		dr=damage(ch,victim,number_range(4,4 + 4 * ch->size + chance/20),gsn_rearkick,
			DAM_BASH,false);

		if(dr){
			act("$n slams you into the air with $s rear legs!$t", 
				ch, autodamtext(victim, dr) ,victim,TO_VICT);
			act("You slam your rear legs into $N, and send $M flying!$t",
				ch, autodamtext(ch, dr) ,victim,TO_CHAR);
			act("$n sends $N flying with $s rear legs!",
				ch,NULL,victim,TO_NOTVICT);
		}else{
			act("$n slams into you with $s rear legs!$t", 
				ch, autodamtext(victim, dr) ,victim,TO_VICT);
			act("You slam your rear legs into $N, and knock $M over.$t",
				ch, autodamtext(ch, dr) ,victim,TO_CHAR);
			act("$n knocks $N over with $s rear legs",
				ch,NULL,victim,TO_NOTVICT);
		}
		check_improve(ch,gsn_rearkick,true,1);
    }
	else
	{
		damage(ch,victim,0,gsn_rearkick,DAM_BASH,false);
		act("You fall flat on your face!", ch,NULL,victim,TO_CHAR);
		act("$n falls flat on $s face.", ch,NULL,victim,TO_NOTVICT);
		act("You evade $n's rear kick, causing $m to fall flat on $s face.",
			ch,NULL,victim,TO_VICT);
		check_improve(ch,gsn_rearkick,false,1);
		ch->position = POS_RESTING;
		WAIT_STATE(ch,skill_table[gsn_rearkick].beats * 3/2); 
    }
}
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/




