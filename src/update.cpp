/**************************************************************************/
// update.cpp - primarily update handlers
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
#include "recycle.h"
#include "altform.h"
#include "clan.h"
#include "lookup.h"
#include "nanny.h"
#include "track.h"
#include "msp.h"
#include "gameset.h"
#include "ictime.h"
#include "intro.h"
#include "lockers.h"
#include "random_obj.h"
#include "statset.h"
#include "dream.h"
#include "bard.h"

// function prototypes
DECLARE_DO_FUN( do_quit          );
DECLARE_DO_FUN( do_say			);
DECLARE_DO_FUN( do_land			);	// Kal
DECLARE_DO_FUN( do_look );
DECLARE_DO_FUN( do_vampchange ); // Ixliam
DECLARE_DO_FUN( do_claws ); // Ixliam
DECLARE_DO_FUN( do_nightsight ); // Ixliam
DECLARE_DO_FUN( do_mask ); // Ixliam
DECLARE_DO_FUN( do_truesight ); // Ixliam
DECLARE_DO_FUN( do_shadowsight ); // Ixliam
DECLARE_DO_FUN( do_buzz2); // Ixliam
DECLARE_DO_FUN( update_runic );
DECLARE_OSPEC_FUN( ospec_spitting_fire );
void reboot_autosave( char_data *ch);
void save_intro_database();
void laston_save(char_data *ch);
void room_aff_update( ROOM_INDEX_DATA *room );
void landchar( char_data *ch);
void do_fearful(char_data *ch );
int weapon_stat_lookup( int level, int pos, int die );
void quest_update();
void check_pending_pigeon(char_data *ch);
void denergy_update(ROOM_INDEX_DATA *room);
int max_gnosis args( (char_data *ch) );
int maint_time();

/*
 * Local functions.
 */
void	connections_update( void );
int     hit_gain( char_data *ch  );
int     mana_gain( char_data *ch );
int     move_gain( char_data *ch );
void    mobile_update( void );
void    weather_update( void );
void    char_update( void );
void    obj_update( void );
void    aggr_update( void );
void	moot_update( void );
void	event_update( void );
void	do_room_spec();
void    dismount( char_data *);  
void    event_activate( void );
void    event_check(char_data *ch, char *argument);
void 	dream_update( void );
void 	song_update( void );

// used for saving 
int		save_number = 0;

DECLARE_DO_FUN( do_pmote	);

// Include files necessary 
#include <signal.h>

// Interval in pulses after which to abort 
int     abort_threshold = 120;

#ifdef unix
#include <sys/resource.h>

bool    disable_timer_abort = false;
int     last_checkpoint;

/**************************************************************************/
// find number of CPU seconds spent in user mode so far 
int get_user_seconds()
{
	struct rusage rus;
	getrusage(RUSAGE_SELF, &rus);
	return rus.ru_utime.tv_sec;
}

bool alarm_handler_display_input_tail;
/**************************************************************************/
// this function is called periodically
void alarm_update()
{
	last_checkpoint = get_user_seconds();
	if (abort_threshold == BOOT_DB_ABORT_THRESHOLD)
	{
		abort_threshold = RUNNING_ABORT_THRESHOLD;
		logf("Used %d user CPU seconds during startup.", last_checkpoint);
	}
	alarm_handler_display_input_tail=true;
}

void write_last_command(void);
void nasty_signal_handler(int i);
/**************************************************************************/
char *get_compile_time (bool show_parent_codebase_version);
/**************************************************************************/
// Signal handler for alarm - suggested for use in MUDs by Fusion 
void alarm_handler(int signo)
{
	int usage_now = get_user_seconds();

	if(usage_now - last_checkpoint<2){
		return; // if we checked in within the last second, dont do anything
	}
	update_currenttime();
    logf("Alarm handler just triggered, %d user second%s since last checkin.",
		usage_now - last_checkpoint,
		usage_now - last_checkpoint==1?"":"s");
    
	// Has there gone abort_threshold CPU seconds without alarm_update? 
    if (!disable_timer_abort && (usage_now - last_checkpoint > abort_threshold ))
	{
		// For the log file 
		update_currenttime();
		log_string("alarm_handler(): Not checkpointed recently, aborting!");
		logf("signo=%d", signo);
		write_last_command();
//		nasty_signal_handler(signo);
		signal(signo, SIG_DFL);
		do_abort ();
		return; // this will create a better core file 
	}else if(usage_now - last_checkpoint>1){
		if(alarm_handler_display_input_tail){
			log_string("Last checkin longer than 1 second, displaying input tail.");
			// output the inputtail
			{
				bool found=false;
				int i; 
				logf("======INPUTTAIL LOG");
				for(i=(inputtail_index+1)%MAX_INPUTTAIL; 
							i!=inputtail_index; 
							i= (i+1)%MAX_INPUTTAIL){

					if(!IS_NULLSTR(inputtail[i])){
						log_string(inputtail[i]);
						found=true;
					}
				}
				if(!IS_NULLSTR(inputtail[inputtail_index])){
					log_string(inputtail[i]);
					found=true;
				}
				if(!found){
					log_string("No inputtail data to dump");
				}else{
					log_string("R = Room vnum, C = Connected state, E = olc editor mode... inputtail does not include force or ordered commands.");
				}
				log_string(get_compile_time(false));
			}
			alarm_handler_display_input_tail=false;
		}
	}

	
	// The timer resets to the values specified in it_interval automatically 
}

/**************************************************************************/
// Install signal alarm handler 
void init_alarm_handler()
{
#ifdef __CYGWIN__
	// signaling with virtual timer unsupported by cygwin
	// nor is alarm handler not supported native win32... this code is in a #ifdef unix only section
	return;
#endif
	log_string("Installing alarm signal handler.");

	abort_threshold = BOOT_DB_ABORT_THRESHOLD;

	// setup a SIGVTALRM signal handler
	struct sigaction sa;	
	sa.sa_handler = alarm_handler;
	sa.sa_flags = SA_RESTART; // Restart interrupted system calls 
	sigemptyset(&sa.sa_mask);

	if(sigaction(SIGVTALRM, &sa, NULL) < 0){ // setup handler for virtual timer 
		bugf("init_alarm_handler(): sigaction(SIGVTALRM, &sa, NULL) < 0 - error %d (%s)",
			errno, strerror( errno));
		exit(1);
	}
	last_checkpoint = get_user_seconds();

	// tell the OS to send us SIGVTALRM signals periodically
	struct itimerval itimer;
	itimer.it_interval.tv_usec = 0; // miliseconds
	itimer.it_interval.tv_sec  = ALARM_FREQUENCY;
	itimer.it_value.tv_usec = 0;
	itimer.it_value.tv_sec = ALARM_FREQUENCY;

	// start the timer - in that many CPU seconds, alarm_handler will be called 
	if (setitimer(ITIMER_VIRTUAL, &itimer, NULL) < 0){
		bugf("init_alarm_handler(): setitimer(ITIMER_VIRTUAL, &itimer, NULL) < 0 - error %d (%s)",
			errno, strerror( errno));
		exit(1);
	}
}

#else
void init_alarm_handler()
{
	return;
}
void ispell_init()
{	
	return;
}
#endif
/*****************************************************************/
void check_fishing ()
{
     char_data *ch;
     int bite;

     for ( ch = char_list; ch; ch = ch->next )
      {
	if ( ch->desc == NULL || ch->desc->connected_state != CON_PLAYING )
	   continue;

		if ( HAS_CONFIG2(ch, CONFIG2_FISHING) && 
		!IS_SET(ch->in_room->room2_flags, ROOM2_SALTWATER_FISH) &&
		!IS_SET(ch->in_room->room2_flags, ROOM2_FRESHWATER_FISH)  )
			{
			  REMOVE_CONFIG2(ch, CONFIG2_FISHING);
			  REMOVE_CONFIG2(ch, CONFIG2_FISH_ON);
			}

		if (HAS_CONFIG2(ch, CONFIG2_FISHING) && 
		!HAS_CONFIG2(ch, CONFIG2_FISH_ON))
		   {
			bite = number_range(1,10);
			if (bite >= 7 && bite <=8) {
			   ch->printlnf("Time goes by... not even a nibble.");
			} else if (bite >= 6) {
			   ch->printlnf("You feel a slight jingle on your line.");
			} else if (bite >= 4) {
			   ch->printlnf("You feel a very solid pull on your line.");
			   SET_CONFIG2(ch, CONFIG2_FISH_ON);
			} else if (bite >= 2) {
			   ch->printlnf("Your line suddenly jumps to life, FISH ON!!!.");
			   SET_CONFIG2(ch, CONFIG2_FISH_ON);
			}
		   }
	}
}
/**************************************************************************/
// Advancement stuff.
void advance_level( char_data *ch )
{
	if ( IS_NPC( ch )){
		return;
	}

	int add_hp=0;
	int add_mana;
	int add_move;
	int add_prac;

	if(GAMESETTING3(GAMESET3_GAIN_HP_WHEN_LEVELING)){
		if(ch->pcdata->perm_hit < race_table[ch->race]->max_hp){
			add_hp=	(ch->perm_stats[STAT_CO] +
						ch->perm_stats[STAT_SD] + 35) / 20;
			add_hp	 = add_hp   * 9/10;
			add_hp	 = UMAX( 2, add_hp);
			ch->max_hit  += add_hp;
			ch->pcdata->perm_hit   += add_hp;
			ch->pcdata->perm_hit=UMIN(ch->pcdata->perm_hit, race_table[ch->race]->max_hp);
		}
	}

	add_mana = (ch->perm_stats[STAT_PR] +
				ch->perm_stats[STAT_EM] +
				ch->perm_stats[STAT_IN]	+ 35) / 20;
	if(!class_table[ch->clss].fMana){
		add_mana /= 2;
	}
	add_mana = add_mana * 9/10;
	add_mana = UMAX( 2, add_mana );
	ch->max_mana += add_mana;

	add_move = number_range( 1, (ch->modifiers[STAT_CO] + ch->modifiers[STAT_QU])/10); 
	add_move = add_move * 9/10;
	add_move = UMAX( 6, add_move );
	ch->max_move += add_move;

	add_prac = 3 + ( ch->modifiers[STAT_CO] +
					 ch->modifiers[STAT_AG] +
					 ch->modifiers[STAT_SD] +
					 ch->modifiers[STAT_ME] +
					 ch->modifiers[STAT_RE] ) / 50 ;
	ch->practice += add_prac;

	if(GAMESETTING3(GAMESET3_GAIN_ONE_TRAIN_WHEN_LEVELING)){
		ch->train++;
	}else{
		ch->train+=2;
	}

	ch->pcdata->perm_mana  += add_mana;
	ch->pcdata->perm_move  += add_move;

	if(GAMESETTING3(GAMESET3_GAIN_HP_WHEN_LEVELING)){
		ch->printlnf(
			"Your gain is: %d/%d hp, %d/%d mana, %d/%d mv %d/%d prac.",
			add_hp,			ch->max_hit,
			add_mana,       ch->max_mana,
			add_move,       ch->max_move,
			add_prac,       ch->practice );
	}else{
		ch->printlnf(
			"Your gain is: %d/%d mana, %d/%d mv %d/%d prac.",
			add_mana,       ch->max_mana,
			add_move,       ch->max_move,
			add_prac,       ch->practice );
	}

	if(    !GAMESETTING(GAMESET_NO_LETGAINING_REQUIRED) 
		&& !GAMESETTING5(GAMESET5_DEDICATED_PKILL_STYLE_MUD))
	{
		if ( ch->level>game_settings->max_level_before_letgaining && !IS_LETGAINED(ch))
		{
			ch->printlnf("`YNOTE: You can't get any more xp once past level %d till you are letgained!`x",
				game_settings->max_level_before_letgaining); 
			ch->println("read `=Chelp letgain`x and `=Chelp requestletgain`x for more details.");
			return;
		}
	}

	ch->pcdata->last_level =( ch->played + (int) (current_time - ch->logon));

	reset_char(ch);
    on_level_learn(ch);
}   

/**************************************************************************/
int get_sublevels_for_level(int level)
{
	if(!GAMESETTING(GAMESET_SUBLEVELS_ENABLED)){
		return 0;
	}

	if (level>=50){
		if(level>90){
			return 15;
		}
		if(level>89){
			return 10;
		}
		if(level>85){
			return 9;
		}
		if(level>80){
			return 8;
		}
		if(level>75){
			return 7;
		}
		if(level>70){
			return 6;
		}
		if(level>65){
			return 5;
		}
		if(level>60){
			return 4;
		}
		// level is between 50 and 60
		switch (level){
			case 60:case 59: case 58: return 3;
			case 57:case 56: case 55: return 2;
			case 54:case 53: case 52: return 1;
			case 51:case 50: return 1;
			default:
				bugf("get_sublevels_for_level(): level = %d!", level);
				return 0;
		}

/*
		if(level>89){
			return 15;
		}
		if(level>85){
			return 10;
		}
		if(level>80){
			return 9;
		}
		if(level>75){
			return 8;
		}
		if(level>70){
			return 7;
		}
		if(level>65){
			return 6;
		}
		if(level>60){
			return 5;
		}
		// level is between 50 and 60
		switch (level){
			case 60:case 59: case 58: return 4;
			case 57:case 56: case 55: return 3;
			case 54:case 53: case 52: return 2;
			case 51:case 50: return 1;
			default:
				bugf("get_sublevels_for_level(): level = %d!", level);
				return 0;
		}
*/
		return 0;
	}else{
		return 0;
	}
}

/**************************************************************************/
void gain_exp( char_data *ch, int gain )
{
    char buf[MSL];

	// do nothing with mobs, imms, and gains of 0
	if ( IS_NPC(ch) || ch->level >= LEVEL_IMMORTAL || gain ==0)
		return;

	if(gain>0)
	{
		if(!GAMESETTING(GAMESET_NO_LETGAINING_REQUIRED)){
			// non letgained above level game_settings->max_level_before_letgaining
			if ( ch->level>game_settings->max_level_before_letgaining && !IS_LETGAINED(ch)){
				ch->printlnf("You can't get any more xp once past level %d till you are letgained.",
					game_settings->max_level_before_letgaining);
				ch->println("read `=Chelp letgain`x and `=Chelp requestletgain`x for more details.");
				return;
			}
		}
	
		// award the xp
		ch->exp = UMAX( exp_per_level(ch,ch->pcdata->points), ch->exp + gain );
		
		// calculate if they gained another level/sublevel uptil they are LEVEL_HERO
		while ( ch->level <= LEVEL_HERO // 10 sublevels for heros
			&& ch->exp >= exp_per_level(ch,ch->pcdata->points) * (ch->level+1))
		{
			// automatic letheroing support
			if(GAMESETTING3(GAMESET3_AUTOMATIC_LETHEROING)
				&& ch->level==LEVEL_HERO-1 
				&& ch->pcdata->sublevel>=get_sublevels_for_level(ch->level)
				&& !IS_SET(ch->act,PLR_CAN_HERO))
			{
				ch->println("You have been automatically letheroed!");
				SET_BIT(ch->act,PLR_CAN_HERO);
				logf("%s has been automatically letheroed", ch->name);
			}

			// lethero system
			if (ch->level==LEVEL_HERO-1 
				&& ch->pcdata->sublevel>=get_sublevels_for_level(ch->level)
				&& !IS_SET(ch->act,PLR_CAN_HERO))
			{
				if ( !HAS_CONFIG( ch, CONFIG_NOHEROMSG ))
					ch->println("You can't hero till you complete your hero quest!");
				sprintf(buf,"$N would have HEROED, but isn't letheroed!");
				wiznet(buf,ch,NULL,WIZ_LEVELS,0,0);
				ch->exp = (exp_per_level(ch,ch->pcdata->points) * (ch->level+1))-10;
			}
			else 
			{
				// gaining a sublevel
				if(ch->pcdata->sublevel < get_sublevels_for_level(ch->level)){
					ch->titlebar("`=\xa8You raise a sublevel!!`x");
					ch->exp= ch->exp - exp_per_level(ch,ch->pcdata->points);
					ch->pcdata->sublevel++;
					
					if(++ch->pcdata->sublevel_pracs>=SUBLEVELS_PER_PRAC ){
						ch->println("You gain 1 practice session for this sublevel.");
						ch->practice++;
						ch->pcdata->sublevel_pracs=0;
					}
					if(++ch->pcdata->sublevel_trains>=SUBLEVELS_PER_TRAIN){
						ch->println("You gain 1 training session for this sublevel.");
						ch->train++;
						ch->pcdata->sublevel_trains=0;
					}
					if(!IS_SET(ch->dyn,DYN_NO_SUBLEVEL_SPELL_LEARN)){
						on_level_learn(ch);
					};
				}else{ // gaining a full level
					if (ch->level!=LEVEL_HERO){
						ch->titlebar("`=\xa8You raise a level!!!`x");
						ch->level++;
						ch->pcdata->sublevel=0;
						sprintf(buf,"$N has attained level %d!",ch->level);
						wiznet(buf,ch,NULL,WIZ_LEVELS,0,0);
						advance_level( ch );
						update_runic(ch, "");
				    if (!IS_NPC(ch) && !is_affected( ch, gsn_beholder ) && ch != NULL && ch->race == race_lookup("beholder"))
				    {
					    affect_parentspellfunc_strip( ch, gsn_beholder);
					    AFFECT_DATA af;
					    af.where		= WHERE_AFFECTS;
					    af.type     	= gsn_beholder;
					    af.level		= 120;
					    af.duration  	= -1;
					    af.location  	= APPLY_AC;
					    af.modifier  	= -5 * (ch->level/5);
					    af.bitvector 	= 0;
					    affect_to_char( ch, &af );
				    }
	
						// heroing
						if (ch->level==LEVEL_HERO){
							ch->println("`?C`?o`?n`?g`?r`?a`?t`?u`?l`?a`?t`?i`?o`?n`?s`?!`?!`?!`x");
							ch->println("Please read help heroref for more information");
						}
					}else{ // gaining a full hero level
						ch->titlebar("`=\xa8You have gained a hero level!!!`x");
						ch->println("You gain 3 pracs and 1 train.");
						ch->practice	+= 3;
						ch->train		+= 1;
						ch->pcdata->hero_level_count++;
						ch->exp= ch->exp - exp_per_level(ch,ch->pcdata->points);
						on_level_learn(ch);

						if(GAMESETTING(GAMESET_SUBLEVELS_ENABLED)){
							ch->pcdata->sublevel=5; // so they have a buffer when they are killed
						}else{
							ch->pcdata->sublevel=0; 
						}
						sprintf(buf, "%s gained a hero level!!!", ch->name );
						wiznet(buf, ch, NULL, WIZ_LEVELS, 0, 0 );
						log_string(buf);
						update_runic(ch, "");
					}
				}
				REMOVE_BIT(ch->dyn,DYN_NO_SUBLEVEL_SPELL_LEARN);
				save_char_obj(ch);
			}
		}
	}
	else // negative gaining - death etc
	{
		bool fleeing=false;
		int xp_amount= gain * -1;

		if (gain>-400){ 
			// lose subtrains and subpracs (prevent cheating using flee just after leveling)
			// (anything apart from death - energy_drain, retreat...)
			fleeing=true; 
			SET_BIT(ch->dyn,DYN_NO_SUBLEVEL_SPELL_LEARN);	
		}

		// handle how the loss of sublevels affects the actual
		// ch->exp drops, drop_level() is only called in kill_char()

		// use sublevels to reduce the xp_amount loss 
		while(ch->pcdata->sublevel>0 && xp_amount>exp_per_level(ch,ch->pcdata->points)){
			ch->pcdata->sublevel--;
			ch->println("You have lost a sublevel!");
			if(fleeing){
				ch->println("You lose one subtrain and one subprac.");						
				ch->pcdata->sublevel_pracs--;
				ch->pcdata->sublevel_trains--;
			}
			xp_amount-=exp_per_level(ch,ch->pcdata->points);
		}

		// if they still have sublevels, and the xp_amount would cause a level drop
		// trade a sublevel for a levels worth of xp first
		if(	(ch->pcdata->sublevel > 0) && (xp_amount>=XP_PAST_LEVEL(ch))){
			ch->exp+=exp_per_level(ch,ch->pcdata->points);
			ch->println("You lose a sublevel!");
			ch->pcdata->sublevel--;
			if(fleeing){
				ch->println("You lose one subtrain and one subprac.");
				ch->pcdata->sublevel_pracs--;
				ch->pcdata->sublevel_trains--;
			}
		}
		// take away the xp
		ch->exp-=xp_amount;

		// can't get below level 1
		ch->exp = UMAX( exp_per_level(ch,ch->pcdata->points), ch->exp);

		// check, shouldn't be required
		if(ch->pcdata->sublevel<0){
			assert(ch->pcdata->sublevel);
			ch->pcdata->sublevel=0;
		}
		save_char_obj(ch);
	}
	return;

}
/**************************************************************************/
/*
 * Regeneration stuff.
 */
int hit_gain( char_data *ch )
{
    int gain;
    int number;

    if (ch->in_room == NULL)
    return 0;

    if ( IS_NPC(ch) )
    {
		gain =  1 + ch->level/3;
		
		// gain *100 what it should normally be, to reduce rounding errors 
		gain *=100;
		
		if (IS_AFFECTED(ch,AFF_REGENERATION))
			gain *= 2;
		
		switch(ch->position)
		{
		default :				gain /= 2;			break;
		case POS_SLEEPING:		gain = 3 * gain/2;	break;
		case POS_RESTING:							break;
		case POS_FIGHTING:		gain /= 3;			break;
		}
    }
    else  // healing on a character 
    {
		// gain *100 what it should normally be, to reduce rounding errors 
		gain = UMAX(300, (ch->modifiers[STAT_CO]*ch->max_hit/2) );
		
		if(gain<100) gain=100;
		
		/* fast healing code */
		number = number_percent();
		if (number < get_skill(ch,gsn_fast_healing))
		{
			gain += number * gain / 100;
			if (ch->hit < ch->max_hit)
				check_improve(ch,gsn_fast_healing,true,8);
		}
		
		switch ( ch->position )
		{
		default:            gain /= 3;          break;
		case POS_SLEEPING:  gain  = gain*3 /2;  break;
		case POS_RESTING:                       break;
		case POS_FIGHTING:  gain /= 5;          break;
		}
		
		if (IS_AFFECTED(ch,AFF_REGENERATION))
			gain *= 2;

		if (IS_WEREAFF(ch,WAFF_HIBERNATION))
			gain *= 2;

		if (IS_AFFECTED(ch, gsn_enhanced_healing))
			gain *= 2;
		
		if ( ch->pcdata->condition[COND_HUNGER] == 0 )
			gain /= 2;
		
		if ( ch->pcdata->condition[COND_THIRST] == 0 )
			gain /= 2;

   	/* Vampires should be tougher at night and weaker during the day. */
    	if ( IS_VAMPIRE(ch) )
    	{
	   	if (time_info.hour > 6 && time_info.hour < 19) /* 6 19 */
            switch ( ch->in_room->sector_type ) 
	     		{
					case (SECT_FIELD):
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
						gain /= 2;
					}
					break;
					case (SECT_CITY):	
					if( !IS_SET(ch->in_room->room_flags, ROOM_DARK) ||
		    	    	!IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
					{
						gain /= 2;
					}
					case (SECT_INSIDE):
						gain += gain * get_skill(ch, gsn_survival) / 50;
					if(get_skill(ch, gsn_survival) > 0)
						check_improve(ch, gsn_survival, true, 6);

	     		}	     
    	}
		
    }

    gain = gain * ch->in_room->heal_rate / 100;

	// support a global room hp regenerate scaling value
	if(game_settings->global_scale_hitpoints_regen_rate
		&& game_settings->global_scale_hitpoints_regen_rate!=100)
	{
		gain = gain * game_settings->global_scale_hitpoints_regen_rate/ 100;
	}
    
    // apply healing rate of objects 
    if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
		gain = gain * ch->on->value[3] / 100;
	
    if ( IS_AFFECTED(ch, AFF_POISON) )
		gain /= 4;
	
    if (IS_AFFECTED(ch, AFF_PLAGUE))
		gain /= 8;

    if (is_affected(ch, gsn_raging_fever))
		gain /= 8;

    if (IS_AFFECTED2(ch, AFF2_DESPAIR))
		gain /= 8;
	
    if (IS_AFFECTED(ch,AFF_HASTE) || IS_AFFECTED(ch,AFF_SLOW))
		gain /=2 ;
	
    if (IS_SET(race_table[ch->race]->flags, RACEFLAG_REGEN_SLOW_IN_LIGHT)
		&& ch->in_room!=NULL
		&& ch->in_room->sector_type != SECT_INSIDE
		&& ch->in_room->sector_type != SECT_CAVE
		&& !IS_SET( ch->in_room->affected_by, ROOMAFF_UTTERDARK )
		&& time_info.hour>7 && time_info.hour<20 )
		gain /=10;
	
    // reduce gain back down what it should normally be 
    gain /=100; 

    return UMIN(gain, UMAX(0, ch->max_hit - ch->hit) );
}

/**************************************************************************/
int mana_gain( char_data *ch )
{
	int gain;
	int number;

	if (ch->in_room == NULL)
		return 0;

	if ( IS_SET( ch->in_room->room_flags, ROOM_ANTIMAGIC )) {
		return 0;
	}

	if ( IS_NPC(ch) )
	{
		gain = 5 + ch->level;
		switch (ch->position)
		{
			default:               gain /= 2;              break;
			case POS_SLEEPING:     gain = 3 * gain/2;      break;
			case POS_RESTING:                              break;
			case POS_FIGHTING:     gain /= 3;              break;
		}
    }
    else
	{
		gain = ch->perm_stats[STAT_SD] * ch->max_mana / 400;
		number = number_percent();
		if (number < get_skill(ch,gsn_meditation))
		{
			gain += (number+10) * gain / 100;
			if (ch->mana < ch->max_mana)
				check_improve(ch,gsn_meditation,true,8);
		}
		if (!class_table[ch->clss].fMana)
			gain /= 2;

		switch ( ch->position )
		{
		default:               gain /= 4;		break;
		case POS_SLEEPING:						break;
		case POS_RESTING:      gain /= 2;		break;
		case POS_FIGHTING:     gain /= 6;		break;
		}
		
		if ( ch->pcdata->condition[COND_HUNGER] == 0 )
			gain /= 2;

		if ( ch->pcdata->condition[COND_THIRST] == 0 )
			gain /= 2;

   	/* Vampires should be tougher at night and weaker during the day. */
    	if ( IS_VAMPIRE(ch) )
    	{
	   	if (time_info.hour > 6 && time_info.hour < 19) /* 6 19 */
            switch ( ch->in_room->sector_type ) 
	     		{
					case (SECT_FIELD):
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
						gain /= 2;
					}
					case (SECT_CITY):
					if( !IS_SET(ch->in_room->room_flags, ROOM_DARK) ||
		    	    	!IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
					{
						gain /= 2;
					}
					case (SECT_INSIDE):
					gain += gain * get_skill(ch, gsn_survival) / 50;
					if(get_skill(ch, gsn_survival) > 0)
						check_improve(ch, gsn_survival, true, 6);
	     		}	     
    	}


	}
	
    if (IS_AFFECTED(ch,AFF_REGENERATION))
		gain += gain/3;
	
	gain = gain * ch->in_room->mana_rate / 100;

	// support a global room mana regenerate scaling value
	if(game_settings->global_scale_mana_regen_rate
		&& game_settings->global_scale_mana_regen_rate!=100)
	{
		gain = gain * game_settings->global_scale_mana_regen_rate/ 100;
	}
	
	if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
		gain = gain * ch->on->value[4] / 100;
	
	if ( IS_AFFECTED( ch, AFF_POISON ) )
		gain /= 4;
	
    if (IS_AFFECTED(ch, AFF_PLAGUE))
		gain /= 8;

	if (IS_WEREAFF(ch,WAFF_HIBERNATION))
	{
		if(IS_WEREFORM(ch))
			gain *= 2;
		else
			gain = gain * (100 + (100 * (13 - ch->werecircle) / 13)) / 100;
	}
    if (is_affected(ch, gsn_raging_fever))
		gain /= 8;

	if (IS_AFFECTED(ch,AFF_HASTE) || IS_AFFECTED(ch,AFF_SLOW))
		gain /=2 ;
	
    if (IS_AFFECTED2(ch, AFF2_DESPAIR))
		gain /= 8;
	
    if (IS_SET(race_table[ch->race]->flags, RACEFLAG_REGEN_SLOW_IN_LIGHT)
		&& ch->in_room!=NULL
		&& ch->in_room->sector_type != SECT_INSIDE
		&& ch->in_room->sector_type != SECT_CAVE
		&& !IS_SET( ch->in_room->affected_by, ROOMAFF_UTTERDARK )
		&& time_info.hour>7 && time_info.hour<20 )
		gain = 0;

	if(gain>30) gain=(gain-30)/3+30;
    
	return UMIN(gain, UMAX(0, ch->max_mana - ch->mana) );
}

/**************************************************************************/
int move_gain( char_data *ch )
{
    int gain;
	
	if (ch->in_room == NULL)
		return 0;
	
    if ( IS_NPC(ch) )
    {
		gain = ch->level;
    }
	else
    {
		gain = UMAX( 15, ch->level );
		
		switch ( ch->position )
		{
		case POS_SLEEPING: gain += ch->modifiers[STAT_CO];              break;
		case POS_RESTING:  gain += ch->modifiers[STAT_CO]/2;            break;
		}
		
		if ( ch->pcdata->condition[COND_HUNGER]   == 0 )
			gain /= 2;
		
		if ( ch->pcdata->condition[COND_THIRST] == 0 )
			gain /= 2;

   	/* Vampires should be tougher at night and weaker during the day. */
    	if ( IS_VAMPIRE(ch) )
    	{
	   	if (time_info.hour > 6 && time_info.hour < 19) /* 6 19 */
            switch ( ch->in_room->sector_type ) 
	     		{
					case (SECT_FIELD):
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
						gain /= 2;
					}
					case (SECT_CITY):
					if( !IS_SET(ch->in_room->room_flags, ROOM_DARK) ||
		    	    	!IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
					{
						gain /= 2;
					}
					case (SECT_INSIDE):
					gain += gain * get_skill(ch, gsn_survival) / 50;

	     		}	     
    	}
	}
	gain = gain * ch->in_room->heal_rate/100;

	// support a global room move regenerate scaling value
	if(game_settings->global_scale_movement_regen_rate
		&& game_settings->global_scale_movement_regen_rate!=100)
	{
		gain = gain * game_settings->global_scale_movement_regen_rate/ 100;
	}
	
    if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
		gain = gain * ch->on->value[3] / 100;
	
    if ( IS_AFFECTED(ch, AFF_POISON) )
		gain /= 4;
	
	if (IS_AFFECTED(ch, AFF_PLAGUE))
		gain /= 8;

	if (IS_WEREAFF(ch,WAFF_HIBERNATION))
		gain *= 2;

    if (is_affected(ch, gsn_raging_fever))
		gain /= 8;

    if (IS_AFFECTED2(ch, AFF2_DESPAIR))
		gain /= 8;
	
	if (IS_AFFECTED(ch,AFF_HASTE) || IS_AFFECTED(ch,AFF_SLOW))
		gain /=2 ;
	
	return UMIN(gain, UMAX(0, ch->max_move - ch->move) );
}

/**************************************************************************/
void gain_condition( char_data *ch, int iCond, int value )
{
    int condition;
	
	if ( value == 0 || IS_NPC(ch) ||
		ch->level<=1 || ch->level >= LEVEL_IMMORTAL)
		return;
	
    condition = ch->pcdata->condition[iCond];

    if (IS_VAMPIRE(ch) || (ch->race == race_lookup("gargoyle")) || IS_SET(ch->affected_by2, AFF2_STOCKADE))
	ch->pcdata->condition[COND_HUNGER] = 30;

    if(IS_SET(ch->wish, WISH_IMMFOOD))
    {
	ch->pcdata->condition[COND_HUNGER] = 30;
	ch->pcdata->condition[COND_THIRST] = 30;
    }

    if (IS_VAMPIRE(ch) && ch->bloodpool == 0)
	ch->pcdata->condition[COND_THIRST] = 0;

    if (ch->race == race_lookup("gargoyle") ||
	    IS_SET(ch->affected_by2, AFF2_STOCKADE))
	ch->pcdata->condition[COND_THIRST] = 30;

    if (condition == -1)
		return;

    ch->pcdata->condition[iCond]        = URANGE( 0, condition + value, 48 );

	if ( ch->pcdata->condition[iCond] == 0 )
    {
		switch ( iCond )
		{
		case COND_HUNGER:
			{
				if(IS_SET(ch->imm_flags,IMM_HUNGER)){
					ch->pcdata->condition[COND_HUNGER] = 20;
				}
				else
				{
					int dam_hunger = 0;
					ch->println("You are hungry.");
					if(GAMESETTING5(GAMESET5_HUNGER_AND_THIRST_CAUSES_DAMAGE)){
						if(ch->level < 5){
							dam_hunger = 1;
						}else if(ch->level < 10){
							dam_hunger = 2;
						}else if(ch->level < 20){
							dam_hunger = 3;
						}else if(ch->level < LEVEL_IMMORTAL){
							dam_hunger = 5;
						}
						if(IS_SET(ch->res_flags, RES_HUNGER))
							dam_hunger/=2;
						if(IS_SET(ch->vuln_flags, VULN_HUNGER))
							dam_hunger*=2;
						ch->hit -= dam_hunger;
					}
				}
			}
			break;
			
		case COND_THIRST:
			if(IS_VAMPIRE(ch))
			{
				ch->bloodpool -= 5;
				if(ch->bloodpool>0)
				{
					ch->println("You feel a slight thirst for blood.");
				}
				if(ch->bloodpool<1)
				{
					ch->bloodpool=0;
					ch->hit -= 15;
					ch->println("Your body hungers for the blood of the living.");
				}
			} else {
				int dam_thirst = 0;
				if(IS_SET(ch->imm_flags,IMM_THIRST)){
					ch->pcdata->condition[COND_THIRST] = 20;
				}else{			
					ch->println("You are thirsty.");
					if(GAMESETTING5(GAMESET5_HUNGER_AND_THIRST_CAUSES_DAMAGE)){
						if(ch->level < 5){
							dam_thirst= 1;
						}else if(ch->level < 10){
							dam_thirst= 2;
						}else if(ch->level < 20){
							dam_thirst= 3;
						}else if(ch->level < LEVEL_IMMORTAL){
							dam_thirst= 5;
						}
						if(IS_SET(ch->res_flags, RES_THIRST))
							dam_thirst/=2;
						if(IS_SET(ch->vuln_flags, VULN_THIRST))
							dam_thirst*=2;
						ch->hit -= dam_thirst;
					}
				}
			}
			break;
			
		case COND_DRUNK:
			if ( condition != 0 )
				ch->println("You are sober.");
			break;
		}
	}
	
    return;
}



/**************************************************************************/
// Mob autonomous action 
// - called every PULSE_MOBILE (once every 4 seconds by default)
void mobile_update( void )
{
    char_data *ch;
    char_data *ch_next;
    EXIT_DATA *pexit;
    int door;

	int number_in_game=mobile_count+1000;
	// note: number_in_game is used to reduce potential loops 
	// with a mobprog that creates new mobs.

    // Examine all mobs. 
	for ( ch = char_list; ch && --number_in_game>=0; ch = ch_next )
    {
		ch_next = ch->next;
		
		if ( !IS_NPC(ch) || ch->in_room == NULL || IS_AFFECTED(ch,AFF_CHARM))
			continue;
		
		if (ch->in_room->area->empty && !IS_SET(ch->act,ACT_UPDATE_ALWAYS))
			continue;
		
		// Examine call for special procedure 
		if ( ch->spec_fun != 0 ){
			if ( (*ch->spec_fun) ( ch ) )
				continue;
		}
		
		if (ch->pIndexData->pShop != NULL) // give showkeepers gold if they are low
		{
			if ((ch->gold * 100 + ch->silver) < ch->pIndexData->wealth*50)
			{
				ch->gold += ch->pIndexData->wealth * number_range(1,20)/5000;
				ch->silver += ch->pIndexData->wealth * number_range(1,20)/50;
			}
		}
		
		// Check triggers only if mobile still in default position
		if ( ch->position == ch->pIndexData->default_pos )
		{
			// Delay 
			if ( HAS_TRIGGER( ch, TRIG_DELAY) 
				&&   ch->mprog_delay > 0 )
			{
				if ( --ch->mprog_delay <= 0 )
				{
					mp_percent_trigger( ch, NULL, NULL, NULL, TRIG_DELAY );
					continue;
				}
			} 
			if ( HAS_TRIGGER( ch, TRIG_RANDOM) )
			{
				if( mp_percent_trigger( ch, NULL, NULL, NULL, TRIG_RANDOM ) )
					continue;
			}
		}
		
		// That's all for sleeping / busy monster, and empty zones 
		if ( ch->position != POS_STANDING || IS_CONTROLLED(ch) )
			continue;
		
		// Scavenge 
		if ( IS_SET(ch->act, ACT_SCAVENGER)
			&&   ch->in_room->contents != NULL
			&&   number_bits( 6 ) == 0 )
		{
			OBJ_DATA *obj;
			OBJ_DATA *obj_best;
			int max;
			
			max         = 1;
			obj_best    = 0;
			for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
			{
				if ( CAN_WEAR(obj, OBJWEAR_TAKE) && can_loot(ch, obj)
					&& obj->cost > max  && obj->cost > 0)
				{
					obj_best    = obj;
					max         = obj->cost;
				}
			}
			
			if ( obj_best )
			{
				obj_from_room( obj_best );
				obj_to_char( obj_best, ch );
				act( "$n gets $p.", ch, obj_best, NULL, TO_ROOM );
			}
		}
		
		/*
		// MOB MEMORY CHECK - Kerenos
		{
		char_data *rch;
		
		  for ( rch = ch->in_room->people; rch; rch = rch->next_in_room ) {
		  if ( mobRememberCH( ch, rch ) && can_see( ch, rch )) {
		  if ( IS_SET( ch->form, FORM_SENTIENT ))
		  do_say( ch, "Prepare to DIE!!!" );
		  else
		  act( "$n looks positively enraged and attacks you.", ch, NULL, rch, TO_CHAR );
		  multi_hit( ch, rch, TYPE_UNDEFINED );
		  break;
		  }
		  }
		  }
		*/
		
		// Wander 
		if ( !IS_SET(ch->act, ACT_DONT_WANDER)
			&& number_bits(3) == 0
			&&  ch->last_force+5 <= tick_counter // mobs after being forced/controlled 
												 // dont wander for a while
			&& !ch->ridden_by)
		{
			door = number_door();
			if ( ( pexit = ch->in_room->exit[door] ) != NULL
				&&   pexit->u1.to_room != NULL
				&&   !IS_SET(pexit->exit_info, EX_CLOSED)
				&&   !IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB)
				&& ( !IS_SET(ch->act, ACT_STAY_AREA)
				||   pexit->u1.to_room->area == ch->in_room->area ) 
				&& ( !IS_SET(ch->act, ACT_OUTDOORS)
				||   !IS_SET(pexit->u1.to_room->room_flags,ROOM_INDOORS)) 
				&& ( !IS_SET(ch->act, ACT_INDOORS)
				||   IS_SET(pexit->u1.to_room->room_flags,ROOM_INDOORS)))
			{
				bool make_move= true;
				
				// dont move a shopkeeper into a room with another shopkeeper in it
				if (IS_KEEPER(ch)){
					char_data *kch;				
					for ( kch = pexit->u1.to_room->people; kch; kch= kch->next_in_room){
						if (IS_KEEPER(kch)){
							make_move= false;
						}
					}
				}	
				
				// dont move a non AMPHIBIAN between land/water
				if (!IS_SET(ch->act, ACT_AMPHIBIAN)){
					if(IS_WATER_SECTOR(ch->in_room->sector_type)!=
						IS_WATER_SECTOR(pexit->u1.to_room->sector_type))
					{
						make_move=false;
					}
				}					
				


				if (IS_SET(ch->form,FORM_TERR_RESTRICT))
				{
					if(pexit->u1.to_room->sector_type == SECT_INSIDE &&
				           !IS_SET (ch->terr_restrict, TERR_INSIDE))
					{
						make_move=false;
					} else 
					if(pexit->u1.to_room->sector_type == SECT_CITY &&
				           !IS_SET (ch->terr_restrict, TERR_CITY))
					{
						make_move=false;
					} else
					if(pexit->u1.to_room->sector_type == SECT_FOREST &&
				           !IS_SET (ch->terr_restrict, TERR_FOREST))
					{
						make_move=false;
					} else
					if(pexit->u1.to_room->sector_type == SECT_FIELD &&
				           !IS_SET (ch->terr_restrict, TERR_FIELD))
					{
						make_move=false;
					} else
					if(pexit->u1.to_room->sector_type == SECT_HILLS &&
				           !IS_SET (ch->terr_restrict, TERR_HILLS))
					{
						make_move=false;
					} else
					if(pexit->u1.to_room->sector_type == SECT_MOUNTAIN &&
				           !IS_SET (ch->terr_restrict, TERR_MOUNTAIN))
					{
						make_move=false;
					} else
					if(pexit->u1.to_room->sector_type == SECT_WATER_SWIM &&
				           !IS_SET (ch->terr_restrict, TERR_WATER_SWIM))
					{
						make_move=false;
					} else
					if(pexit->u1.to_room->sector_type == SECT_WATER_NOSWIM &&
				           !IS_SET (ch->terr_restrict, TERR_WATER_NOSWIM))
					{
						make_move=false;
					} else
					if(pexit->u1.to_room->sector_type == SECT_SWAMP &&
				           !IS_SET (ch->terr_restrict, TERR_SWAMP))
					{
						make_move=false;
					} else
					if(pexit->u1.to_room->sector_type == SECT_AIR &&
				           !IS_SET (ch->terr_restrict, TERR_AIR))
					{
						make_move=false;
					} else
					if(pexit->u1.to_room->sector_type == SECT_DESERT &&
				           !IS_SET (ch->terr_restrict, TERR_DESERT))
					{
						make_move=false;
					} else
					if(pexit->u1.to_room->sector_type == SECT_CAVE &&
				           !IS_SET (ch->terr_restrict, TERR_CAVE))
					{
						make_move=false;
					} else
					if(pexit->u1.to_room->sector_type == SECT_UNDERWATER &&
				           !IS_SET (ch->terr_restrict, TERR_UNDERWATER))
					{
						make_move=false;
					} else
					if(pexit->u1.to_room->sector_type == SECT_SNOW &&
				           !IS_SET (ch->terr_restrict, TERR_SNOW))
					{
						make_move=false;
					} else
					if(pexit->u1.to_room->sector_type == SECT_ICE &&
				           !IS_SET (ch->terr_restrict, TERR_ICE))
					{
						make_move=false;
					} else
					if(pexit->u1.to_room->sector_type == SECT_TRAIL &&
				           !IS_SET (ch->terr_restrict, TERR_TRAIL))
					{
						make_move=false;
					} else
					if(pexit->u1.to_room->sector_type == SECT_LAVA &&
				           !IS_SET (ch->terr_restrict, TERR_LAVA))
					{
						make_move=false;
					}			
				}
	
				if (make_move)
				{
					move_char( ch, door, false, NULL);
				}
			}
		}
	 }
	 return;
}

/**************************************************************************/
// normal range -9 thru 9
// peak is 11 (dual ecllipse)
int get_magecastmod(void)
{
	int cm;
	int sect;
	// 35 =  7 eclipse     month 11 = 4  
	// 0  =  6     // months // 3 thru -3
	// 18 = -6

	cm = ((abs(time_info.day-17)- 9)*2/3) + ((abs(time_info.month-5)- 3));
	if (time_info.day==ICTIME_DAYS_PER_MONTH-1) // peak modifier
		cm++;
	if (time_info.month==ICTIME_MONTHS_PER_YEAR-1) // peak modifier
		cm++;
	
	for ( sect = 0; sect < SECT_MAX; sect++ )
	{
		
		if (time_info.day>(ICTIME_DAYS_PER_MONTH/2))
			weather_info[sect].moon_getting_brighter = true;
		else
			weather_info[sect].moon_getting_brighter = false;
	}

	return cm;
}

/**************************************************************************/
void resolve_stats();
void do_save_corpses(char_data *ch, char *);
void save_storage_db(char_data *ch, char *);
void save_races();
/**************************************************************************/
void maintence_saves( void )
{
	if (--resaveCounter<1)
	{
		log_string("Doing maintence saves...");
		log_string(">>> autosave olc");
		reboot_autosave(NULL);	// autosave olc work and lockers
		log_string(">>> autosave laston");
		// save intro database stuff
		save_intro_database();
		laston_save(NULL);		// autosave laston stuff
		log_string(">>> autosave intro database");
		save_intro_database(); // autosave laston stuff
		log_string(">>> autosave corpses");
		do_save_corpses(NULL,"");
		log_string(">>> autosave storage");
		save_storage_db(NULL,"");
		
		if(IS_SET( RACEEDIT_FLAGS, OLC_CHANGED )){
			log_string(">>> autosaving races");
			save_races();
			REMOVE_BIT( RACEEDIT_FLAGS, OLC_CHANGED );
		}

		do_hsave(NULL,"");
		resolve_stats();
		resort_top_roleplayers();

		resaveCounter=15;  // reset counter
	};
}

/**************************************************************************/
void quit_char(char_data *ch, char *argument, bool character_deleting );
/**************************************************************************/
// Update all chars, including mobs.
void char_update( void )
{
	char_data *ch;
	char_data *ch_next;
	char_data *ch_quit;
	char buf[MSL];
    int xp;

	ch_quit        = NULL;

	// update save counter
	++save_number%=10;

	for ( ch = char_list; ch != NULL; ch = ch_next )
    {
		AFFECT_DATA *paf;
		AFFECT_DATA *paf_next;
		
		ch_next = ch->next;
		ch->autologout= false;

		if ( IS_NPC( ch )
		&& HAS_TRIGGER( ch, TRIG_TICK ))
			mp_percent_trigger( ch, NULL, NULL, NULL, TRIG_TICK );

		if ( IS_NPC( ch )
		&& HAS_TRIGGER( ch, TRIG_EVENT ))
			mp_event_trigger( ch, NULL, NULL, NULL, TRIG_EVENT );

		if(!IS_NPC(ch) && get_skill(ch, gsn_fire_affinity) > 0)
		{
			SET_BIT(ch->imm_flags, IMM_FIRE);
			SET_BIT(ch->imm_flags, IMM_LIGHT);
		}

		if(!IS_NPC(ch) && get_skill(ch, gsn_air_affinity) > 0)
		{
			SET_BIT(ch->imm_flags, IMM_LIGHTNING);
			SET_BIT(ch->imm_flags, IMM_SOUND);
		}

		if(!IS_NPC(ch) && get_skill(ch, gsn_water_affinity) > 0)
		{
			SET_BIT(ch->imm_flags, IMM_COLD);
			SET_BIT(ch->imm_flags, IMM_DROWNING);
		}

		if(!IS_NPC(ch) && get_skill(ch, gsn_earth_affinity) > 0)
		{
			SET_BIT(ch->imm_flags, IMM_ACID);
			SET_BIT(ch->imm_flags, IMM_POISON);
		}

		// Mirage countdown
		if ( ch->mirage_hours ) {
			--ch->mirage_hours;
			if ( ch->mirage_hours < 1 )
				ch->mirage = 0;
		}

		if (!IS_NPC(ch))
		{
			ch->idle++;
			
			if(ch->pknorecall>0)
				ch->pknorecall--;
			
			if(ch->pknoquit>0)
				ch->pknoquit--;
			
			ch->pcdata->next_lay_countdown--;
			if (ch->pcdata->next_lay_countdown<1)
			{
				ch->pcdata->lays=UMIN(ch->pcdata->lays+1, ch->level/8+2);
				ch->pcdata->next_lay_countdown=GET_NEW_LAY_COUNTER(ch);
			}
						
			if(ch->altform.timer > 0)
				ch->altform.timer--;

            if (   !IS_OOC(ch)
				&& ch->idle<2
				&& ch->pcdata->karns<GET_MAX_KARN(ch))
			{
				ch->pcdata->next_karn_countdown--;
				if (ch->pcdata->next_karn_countdown<1)
				{
					ch->println("Before your eyes a large glowing ball of light appears!\r\n"
						"It moves towards you and enters your body... you have a sensation\r\n"
						"that seems familiar but you dont recognise it, then the feeling\r\n"
						"slowly fades to the point you are unaware of it.");
					
					ch->pcdata->next_karn_countdown=GET_NEW_KARN_COUNTER(ch);
					ch->pcdata->karns++;
				}
			}
			else
			{
                if (ch->idle>10 || (IS_OOC(ch) && ch->clan))
				{
					ch->pcdata->next_karn_countdown++;
				}
			}

			// autoafk system
			if(!GAMESETTING3(GAMESET3_AUTOAFK_DISABLED) && !IS_NPC(ch)){
				int autoafkafter=ch->pcdata->autoafkafter;
				if(!autoafkafter){
					if(ch->level<LEVEL_IMMORTAL){
						autoafkafter=5;
					}else{
						autoafkafter=15;
					}
				}
				if(ch->idle >= autoafkafter && !IS_SET(TRUE_CH(ch)->comm,COMM_AFK))
				{
					if(ch->level >= LEVEL_IMMORTAL){
						ch->println("Idle 15 Ticks. Auto AFK Enabled.");
					}else{
						ch->println("Idle 5 Ticks. Auto AFK Enabled.");
					}
					SET_BIT(TRUE_CH(ch)->comm,COMM_AFK);
					replace_string(ch->pcdata->afk_message, "Auto AFK");
				}
			}
			
			if (ch->level<LEVEL_IMMORTAL)
			{
				if ( ch->idle == 15 )
				{
					if ( ch->was_in_room == NULL && ch->in_room != NULL )
					{
						ch->was_in_room = ch->in_room;
						if ( ch->fighting != NULL )
							stop_fighting( ch, true );
						act( "$n disappears into the void.",
							ch, NULL, NULL, TO_ROOM );
						ch->println("You disappear into the void.");
						if (ch->level > 1)
							save_char_obj( ch );
						char_from_room( ch );
						char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
					}
				}
			}
			
			if (!IS_OOC(ch))
			{
				// RPS and vote gaining systems
				if(!IS_NPC(ch))
				{
					ch->pcdata->rp_count++;

					// update a nobles moots
					if(ch->pcdata->diplomacy)
					{
						ch->pcdata->voteupdate_count++;

						if(ch->pcdata->voteupdate_count>25){
							if(ch->pcdata->voteupdate_count>(ch->pcdata->dip_points/20)){
								ch->pcdata->dip_points+=ch->pcdata->diplomacy;
								ch->pcdata->voteupdate_count=0;
								if(!IS_IMMORTAL(ch)){
									ch->wrapln("You just gained some more votes... "
										"keep giving positive moots to everyone.");
								}					
							}else if(ch->pcdata->voteupdate_count>=60){ 
								ch->pcdata->dip_points+=ch->pcdata->diplomacy;
								ch->pcdata->voteupdate_count=0;
								
								if(!IS_IMMORTAL(ch)){
									ch->wrapln("You just gained some more votes... "
										"remember that those with less than 1200 votes get given votes "
										"more frequently so keep giving positive moots to players.");
								}					
							}
							
						}
					}

					if(ch->pcdata->rp_count % 10 == 0 &&
						ch->pcdata->rp_count != 0)
					{
						ch->pcdata->rp_count=0;

						if(GAMESETTING(GAMESET_DISABLE_RPS_SYSTEM)){
							ch->pcdata->merit=0;
						}

						if (ch->pcdata->merit==100){
							ch->pcdata->merit=85;
						}
						
						if (ch->pcdata->merit>65)
						{// anti autorps abuse system
							int dups=0, cdup=0;
							int current=0;
							int check=0;
							bool fail_audit = false;
							time_t timeafter = current_time - (60*20); // 20mins

							logf("Doing EMOTE audit on %s merit=%d (emote_index = %d)", 
									ch->name, ch->pcdata->merit ,ch->pcdata->emote_index);

							for (current=0; current<RPS_AUDIT_SIZE; current++)
							{
								if (IS_NULLSTR(ch->pcdata->emotecheck[current]))
									continue;
								if (ch->pcdata->emotetimes[current]<timeafter)
									continue;
								cdup=0;
								for (check=current+1; check<RPS_AUDIT_SIZE; check++)
								{	
									if (IS_NULLSTR(ch->pcdata->emotecheck[check]))
										continue;
									if (ch->pcdata->emotetimes[current]<timeafter)
										continue;
									if (!str_cmp(ch->pcdata->emotecheck[current],
										ch->pcdata->emotecheck[check]))
									{
										cdup++;
									}																		
								}
								if (cdup>5)
								{
									fail_audit=true;
								}
								dups+=cdup;
							}
							if (dups>8)
							{
								fail_audit=true;
							}
							if (fail_audit)
							{
								ch->pcdata->merit=0;
								logf("%s failed RPS emote audit (emote_index = %d)", 
									ch->name, ch->pcdata->emote_index);
								for (current=0; current<RPS_AUDIT_SIZE 
									&& !IS_NULLSTR(ch->pcdata->emotecheck[current]); current++)
								{
									logf("%2d> emote '%s'",current, ch->pcdata->emotecheck[current]);
								}
							}
						} // end of emote audit

						if (ch->pcdata->merit>70)
						{// anti autorps abuse system
							int dups=0, cdup=0;
							int current=0;
							int check=0;
							time_t timeafter = current_time - (60*20); // 20mins
							bool fail_audit = false;

							logf("Doing SAY audit on %s merit=%d (say_index = %d)", 
									ch->name, ch->pcdata->merit ,ch->pcdata->say_index);

							for (current=0; current<RPS_AUDIT_SIZE; current++)
							{
								if (IS_NULLSTR(ch->pcdata->saycheck[current]))
									continue;

								if (ch->pcdata->saytimes[current]<timeafter)
									continue;

								cdup=0;
								for (check=current+1; check<RPS_AUDIT_SIZE; check++)
								{	
									if (IS_NULLSTR(ch->pcdata->saycheck[check]))
										continue;

									if (ch->pcdata->saytimes[current]<timeafter)
										continue;

									if (!str_cmp(ch->pcdata->saycheck[current],
										ch->pcdata->saycheck[check]))
									{
										cdup++;
									}																		
								}
								if (cdup>5)
								{
									fail_audit=true;
								}
								dups+=cdup;
							}
							if (dups>8)
							{
								fail_audit=true;
							}
							if (fail_audit)
							{
								ch->pcdata->merit=0;
								logf("%s failed RPS say audit (say_index = %d)", 
									ch->name, ch->pcdata->say_index);
								for (current=0; current<RPS_AUDIT_SIZE
									&& !IS_NULLSTR(ch->pcdata->saycheck[current]); current++)
								{
									logf("%2d> say '%s'",current, ch->pcdata->saycheck[current]);
								}
							}
						} // end of say audit
						
						if (ch->pcdata->merit<0)
							ch->pcdata->merit=0;
						
//						xp=ch->pcdata->merit * (125-( (ch->level) * 100 / 77)); // old dawn1.1
//						xp=ch->pcdata->merit * (125-( (ch->level) * 100 / 70)); // dawn1.5 system till 14May99
//						xp=ch->pcdata->merit * (130-( (ch->level) * 100 / 85));	// dawn1.5 till 12Aug99
//						xp=ch->pcdata->merit * (180-( (ch->level) * 100 / 60)); // till 20 October 99
						xp=ch->pcdata->merit * (210-( (ch->level) * 100 / 60));
						// bonus for being letgained 
						if(IS_LETGAINED(ch) || GAMESETTING(GAMESET_NO_LETGAINING_REQUIRED)){
							xp*=3/2;
						}

						if((ch->pcdata->rp_points*2)>ch->exp){
							xp*=125; // extra 25% for those who have got over half their xp from RPS
							xp/=100;
						}

                        // reduced RPS percent system
                        if (!IS_NPC(ch) && ch->pcdata->reduce_rps_percent>0)
                        {
                            xp= (xp *ch->pcdata->reduce_rps_percent)/100;
                        }

						xp/=number_range(90,105);


						if(xp>0 && !IS_SET(ch->act,PLR_NORP))
							{
								ch->pcdata->rp_points+=xp;
								if ((ch->level<=LEVEL_HERO) && (ch->pcdata->xp_penalty<1))
								{
									ch->printlnf("`=\xa7You receive %d xp for your roleplaying efforts.`x", xp);
									gain_exp(ch,xp);
								}
								else
								{
									ch->printlnf("`=\xa7Your rps increases by %d for your roleplaying efforts.`x", xp);
								}
							
								sprintf(buf,"%s received %2d xp for RP. (merit = %2d, lvl = %2d)"
									,ch->name,xp,ch->pcdata->merit, ch->level );
								wiznet(buf,ch,NULL,WIZ_RPEXP,0,0);
								log_string(buf);
							
								// log their rps
								if (IS_SET(ch->act, PLR_LOG))
								{				 
									append_playerlog( ch, buf);
								}
							}
				
						ch->pcdata->merit=0;
					}
					
					if(ch->pcdata->did_ooc || ch->pcdata->did_ic)
					{
						if(ch->pcdata->did_ic && !ch->pcdata->did_ooc)
							ch->pcdata->merit+=10;
						else
							if(ch->pcdata->did_ooc && !ch->pcdata->did_ic )
								ch->pcdata->merit-=3;
					}
					else
						ch->pcdata->merit-=1;
					
					ch->pcdata->did_ooc=false;
					ch->pcdata->did_ic=false;
				}// end of RPS system
				
				// xp penalty system
				if(!IS_NPC(ch)&&ch->pcdata!=NULL)
				{
					if(ch->pcdata->xp_penalty>0)
						ch->pcdata->xp_penalty-=1;
				}
				
				//  ******************************
				//	 Tiredness related code below - doesn't affect on olc port
				if(!IS_NPC(ch) && (ch->level<LEVEL_IMMORTAL) && ch->desc!=NULL 
					&& !GAMESETTING5(GAMESET5_DEDICATED_OLC_BUILDING_MUD) 
					&& ch->level>5 && ch->pcdata!=NULL)
				{
					if(ch->position==POS_SLEEPING)
					{
						if(ch->pcdata->tired!=-1)
						{
							if(!IS_SET( ch->in_room->affected_by, ROOMAFF_BLIGHT))
							{
								ch->pcdata->tired-=3;
								if(ch->pcdata->tired<0)
									ch->pcdata->tired=0;
							}
						}
						if(ch->is_trying_sleep)
						{
							if((number_percent()<(4-ch->pcdata->tired)*25)
								&& !IS_AFFECTED(ch, AFF_SLEEP))
							{
								ch->position=POS_RESTING;
								dream_kill(ch);
								ch->println("You awake feeling refreshed.");
								act( "$n awakes very chipper and refreshed.", ch, NULL, NULL, TO_ROOM );
								ch->is_trying_sleep=false;
							}
						}
						else
						{
							if((number_percent()<(10-ch->pcdata->tired)*25)
								&& !IS_AFFECTED(ch, AFF_SLEEP))
							{
								ch->position=POS_RESTING;
								dream_kill(ch);
								ch->println("You awake somewhat groggy and confused.");
								act( "$n awakes very reluctantly.", ch, NULL, NULL, TO_ROOM );
							}
						}
						if(IS_WEREBEAR(ch))
							SET_WEREAFF(ch, WAFF_HIBERNATION);
					}
					else
					{
						if(ch->position>POS_RESTING && (ch->level>5) && (ch->pcdata->tired!=-1))
						{
							if(number_range(1,3)!=1)
							{
								if (IS_SET(race_table[ch->race]->flags, RACEFLAG_NEED_TWICE_AS_MUCH_SLEEP)){
									ch->pcdata->tired+=2;
								}else{
									ch->pcdata->tired++;
								}
							}
							if(IS_WEREAFF(ch, WAFF_HIBERNATION))
								REMOVE_WEREAFF(ch, WAFF_HIBERNATION);
						}
						
						
						if(ch->is_trying_sleep)
						{
							if(((number_percent()<(ch->pcdata->tired-10)*10+(ch->max_hit-ch->hit)/ch->max_hit*50))
								&& !IS_AFFECTED(ch, AFF_BERSERK)
								&& !is_affected(ch, gsn_rage) )
							{
								ch->position=POS_SLEEPING;
								ch->println("You drift off into the dreamscape.");
								act( "$n goes to sleep.", ch, NULL, NULL, TO_ROOM );
								dream_reset(ch);
							}
						}
						else
						{
							if((number_percent()<(ch->pcdata->tired-30)*7+
								(ch->max_hit-ch->hit)/(ch->max_hit==0?1:ch->max_hit)*25)
								&& !IS_AFFECTED(ch, AFF_BERSERK)
								&& !is_affected(ch, gsn_rage) )
							{
								if(ch->position!=POS_FIGHTING)
								{
									ch->position=POS_SLEEPING;
									dream_reset(ch);
									if (ch->mounted_on!=NULL)
									{
										dismount(ch);
										ch->println("You are so tired you fall off your mount!");
									}
									ch->println("You groggily collapse from exhaustion.");
									act( "$n collapses from exhaustion.", ch, NULL, NULL, TO_ROOM );
								}
								else
									ch->println("You are very battle weary.");
							}
							else if(ch->pcdata->tired>32)
									ch->println("You feel extremely tired.");
							else if(ch->pcdata->tired>25)
									ch->println("You feel very tired.");
							else if(ch->pcdata->tired>16)
									ch->println("You feel tired.");
						}
						if(ch->pcdata->tired<-1)
						{
							ch->pcdata->tired=0;
						}
					}
				}// end of Tiredness related code 
			} //!IS_OOC(ch)
		} // end of !IS_NPC(ch)
		//  **************************************
	
		// time doesn't pass in ooc or olconly areas
		if(ch->in_room && !IS_OOC(ch)
			&& !IS_SET(ch->in_room->area->area_flags, AREA_OLCONLY) )
		{
			if ( ch->expire_recall_inn>0 ) {
				ch->expire_recall_inn--;

				if ( ch->expire_recall_inn == 0 ) {
					ch->println("`YYour room at the inn has expired.`x");
					ch->recall_inn_room = 0;
				}
			}

			if(ch->pkool>0){
				ch->pkool-=1;
			}
			if(ch->pksafe>0){
				ch->pksafe-=1;
			}
			
			// budget fix for subdue
			if (ch->subdued_timer>0)
			{
				ch->subdued_timer--;
				if (ch->subdued_timer==0)
				{
					if (ch->hit<number_range(2,5))
					{
						ch->hit 	= number_range(2,5);
						ch->mana	= 0;
						ch->move	= number_range(1,20);
					}
					ch->subdued=false;
					update_pos( ch );
				}
			}
			
			if ( ch->position >= POS_STUNNED )
			{
				// check to see if we need to go home (mobs out of their area)
				if (IS_NPC(ch) && ch->zone != NULL && ch->zone != ch->in_room->area
					&& ch->desc == NULL &&	ch->fighting == NULL
					&& !IS_AFFECTED(ch,AFF_CHARM) && number_percent() < 15)
				{
					act("$n wanders on home.",ch,NULL,NULL,TO_ROOM);
					extract_char(ch,true);
					continue;
				}
		
				if(!IS_SET( ch->in_room->affected_by, ROOMAFF_BLIGHT))
				{
					if ( ch->hit < ch->max_hit){
						ch->hit+= hit_gain(ch);
					}else{
						ch->hit = ch->max_hit;
					}
					
					if ( ch->mana < ch->max_mana){
						ch->mana += mana_gain(ch);
					}else{
						ch->mana = ch->max_mana;
					}

					if(IS_WEREFORM(ch) 
					&& ch->in_room->sector_type != SECT_INSIDE
					&& ch->in_room->sector_type != SECT_CITY)
						ch->gnosis = 
						UMIN(max_gnosis(ch), ch->gnosis + number_range(1, 3 + (13 - ch->werecircle) / 2));
				}
				// movement gain - lose 1 to 2 mv per tick you 
				// are nonmagically flying
				if(IS_AFFECTED(ch, AFF_FLYING)){
					if(IS_SET(ch->dyn,DYN_NONMAGICAL_FLYING)
						&& (!IS_NPC(ch) || (IS_NPC(ch) && ch->master)))
					{
						ch->move-=number_range(2,5);
						if(ch->move<1){
							ch->println("You are so exhausted from your flying you land on the ground.");
							do_land(ch,"");
						}else if(number_range(1,10)==1){
							ch->println("All this flying is making you tired.");
						}
					}
				}else{
					if(!IS_SET( ch->in_room->affected_by, ROOMAFF_BLIGHT))
					{
						if ( ch->move < ch->max_move){
							ch->move += move_gain(ch);
						}else{
							ch->move = ch->max_move;
						}
					}
				}
			}
			
			if ( ch->position == POS_STUNNED )
				update_pos( ch );
			
/*********** Arena Code *******************/			

        if ( challenge_tme != 0)
		challenge_tme--;
	if (challenge_tme == 0 && IS_SET(ch->act2,ACT2_PLR_CHALLENGER)
	&& !IS_SET(ch->in_room->room2_flags,ROOM2_ARENA))
	{
		ch->println("`#`RThey have not responded, your challenge is canceled.");
		REMOVE_BIT(ch->act2,ACT2_PLR_CHALLENGER);
		is_challenge = false;
		return;
        }

/*********** Arena Code *******************/

			
			if ( !IS_NPC(ch) && ch->level < LEVEL_IMMORTAL )
			{
				OBJ_DATA *obj;
				
				if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
					&&   obj->item_type == ITEM_LIGHT
					&&   obj->value[2] > 0 )
				{
					if ( --obj->value[2] == 0 && ch->in_room != NULL )
					{
						--ch->in_room->light;
						act( "$p goes out.", ch, obj, NULL, TO_ROOM );
						act( "$p flickers and goes out.", ch, obj, NULL, TO_CHAR );
						extract_obj( obj );
					}
					else if ( obj->value[2] <= 5 && ch->in_room != NULL)
						act("$p flickers.",ch,obj,NULL,TO_CHAR);
				}
				
				if ((IS_IMMORTAL(ch) && ch->desc)|| IS_SWITCHED (ch))
					ch->timer = 0;
				
				if ( ++ch->timer >= 12 )
				{
					if ( ch->was_in_room == NULL && ch->in_room != NULL )
					{
						ch->was_in_room = ch->in_room;
						if ( ch->fighting != NULL )
							stop_fighting( ch, true );
						act( "$n disappears into the void.",
							ch, NULL, NULL, TO_ROOM );
						ch->println("You disappear into the void.");
						if (ch->level > 1)
							save_char_obj( ch );
						char_from_room( ch );
						char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
					}
				}
				
				gain_condition( ch, COND_DRUNK,  -1 );
				gain_condition( ch, COND_FULL, ch->size > SIZE_MEDIUM ? -4 : -2 );
				if((IS_SET(ch->res_flags, RES_THIRST) && number_range(1,3) == 1)
				|| !IS_SET(ch->res_flags, RES_THIRST))
					gain_condition( ch, COND_THIRST, -1 );
				if(IS_SET(ch->vuln_flags, VULN_THIRST))
					gain_condition( ch, COND_THIRST, -1 );
				if((IS_SET(ch->res_flags, RES_HUNGER) && number_range(1,3) == 1)
				|| !IS_SET(ch->res_flags, RES_HUNGER))
					gain_condition( ch, COND_HUNGER, ch->size > SIZE_MEDIUM ? -2 : -1);
				if(IS_SET(ch->vuln_flags, VULN_HUNGER))
					gain_condition( ch, COND_HUNGER, ch->size > SIZE_MEDIUM ? -2 : -1);

			}

			if(ch->fighting)
			{
				update_pos(ch);

				// lava 'fire' damage from the room
				if(!IS_NPC(ch) && ch->in_room 
					&& (ch->in_room->sector_type == SECT_LAVA)
					&& !IS_AFFECTED(ch, AFF_FLYING )
					&& !IS_SET(TRUE_CH(ch)->act, PLR_HOLYWALK))
				{
					if ( !IS_AFFECTED( ch, AFF_FLYING )
						&& check_immune(ch, DAM_FIRE) != IS_IMMUNE)
					{
						ch->println("You start to burn.");
						damage( ch, ch, UMAX(20, 250 - (ch->modifiers[STAT_CO])), gsn_plague,DAM_FIRE,false);
					}
				}
				
				if(ch->is_stunned){
					ch->is_stunned-=20;
					if(ch->is_stunned<=0){
						ch->position=POS_RESTING;
						ch->is_stunned=0;
						ch->println("You are no longer stunned.");
					}
				}

				if(ch->will_die || ch->hit<-10)
				{
					ch->will_die-=20;
					if(ch->will_die<=0 || ch->hit<-10)
					{
						ch->will_die=0;
						if ( !IS_NPC(ch) )
						{
							sprintf( log_buf, "%s died from their injuries at %d",
								ch->name, 	ch->in_room->vnum );
							log_string( log_buf );
							
							// Dying penalty:
							// 2/3 way back to previous level.
							if(!IS_NPC(ch))
								if(ch->level<21)
								{
									if ( ch->exp > exp_per_level(ch,ch->pcdata->points)
										* ch->level )
									{
										int lose_amount = (2 * (exp_per_level(ch,ch->pcdata->points)
											* ch->level - ch->exp)/3) + 50;
										
										if (lose_amount<-1000) /* dont lose more than 1000 xp */
											lose_amount = -1000;
										
										gain_exp( ch,lose_amount);
									}
								}
								else
								{
									gain_exp( ch, -500);
									if(ch->exp<exp_per_level(ch,ch->pcdata->points)* ch->level )
										drop_level(ch);
									check_perm_damage(ch);
								}
						}
						
						
						if (IS_NPC(ch))
							wiznet(log_buf,NULL,NULL,WIZ_MOBDEATHS,0,0);
						else
							wiznet(log_buf,NULL,NULL,WIZ_DEATHS,0,0);
						
						raw_kill( ch, ch );
					}
				}
			}
			
			// patch to prevent cheaters surrendering or using rescue
			// then backstabing straight away
			if (ch->cautious_about_backstab>0)
				ch->cautious_about_backstab--;

			// affects system
			for ( paf = ch->affected; paf != NULL; paf = paf_next )
			{
				paf_next = paf->next;
				if ( paf->duration > 0 )
				{
					paf->duration--;
					if (paf->level > 0 						
						&& number_range(0,4) == 0
						&& paf->where!=WHERE_OBJECTSPELL) 
					{
						paf->level--;  // spell strength fades with time
					}
				}
				else 
				{
					// spells duration >-1 wear off checks
					if ( paf->duration>-1 ) 
					{
						if ( paf_next == NULL
							||	 paf_next->type != paf->type
							||	 paf_next->duration > 0 )
						{
							if ( paf->type > 0 && skill_table[paf->type].msg_off )
							{
								ch->print( skill_table[paf->type].msg_off);
								ch->println("");
							}
							if ( paf->type == gsn_immolation )
							{
								if ( !IS_NPC(ch))
								{
									ch->mana = 0;
									ch->pcdata->tired += 40;
									ch->pcdata->condition[COND_THIRST] = 0;
									ch->pcdata->condition[COND_HUNGER] = 0;
									
								}
							}
						}					
						affect_remove( ch, paf );
					}
				}
			}

			// Careful with the damages here,
			//	 MUST NOT refer to ch after damage taken,
			//	 as it may be lethal damage (on NPC).
			// Drowning damage if ch->in_room sector is UNDERWATER
			if (ch->in_room 
				&& ( ch->in_room->sector_type == SECT_UNDERWATER 
				||   IS_SET( ch->in_room->affected_by, ROOMAFF_UNDERWATER ))
				&& !IS_AFFECTED(ch, AFF_OTTERLUNGS )
				&& !IS_SET(TRUE_CH(ch)->act, PLR_HOLYWALK)
				&& !IS_NPC(ch))
			{
				OBJ_DATA *obj2 = (get_eq_char(ch, WEAR_LIGHT));

				int dam = UMAX(20, 75 - (ch->modifiers[STAT_CO]));

// Redundant??	if (IS_SET(ch->vuln_flags, VULN_DROWNING)) dam *= 2;
			    if ( !obj2 && !IS_AFFECTED( ch, AFF_OTTERLUNGS ))
				{
					ch->println("Water fills your lungs as you start to drown.");
					damage( ch, ch, dam, gsn_plague,DAM_DROWNING,false);
				}
				if ( obj2 ) {
					if (!IS_SET( obj2->extra_flags, OBJEXTRA_OTTERLUNGS ))
					{
						ch->println("Water fills your lungs as you start to drown.");
						damage( ch, ch, dam, gsn_plague,DAM_DROWNING,false);
					}
				}
			}


	if(ch->will_die || ch->hit<-10)
	{
		ch->will_die-=20;
		if(ch->will_die<=0 || ch->hit<-10)
		{
			ch->will_die=0;
			if ( !IS_NPC(ch) )
			{
				sprintf( log_buf, "%s died from their wounds at %d",
					ch->name, 	ch->in_room->vnum );
				log_string( log_buf );
				
				// Dying penalty:
				// 2/3 way back to previous level.
				if(!IS_NPC(ch))
					if(ch->level<21)
					{
						if ( ch->exp > exp_per_level(ch,ch->pcdata->points)
							* ch->level )
						{
							int lose_amount = (2 * (exp_per_level(ch,ch->pcdata->points)
								* ch->level - ch->exp)/3) + 50;
							
							if (lose_amount<-1000) /* dont lose more than 1000 xp */
								lose_amount = -1000;
							
							gain_exp( ch,lose_amount);
						}
					}
					else
					{
						gain_exp( ch, -500);
						if(ch->exp<exp_per_level(ch,ch->pcdata->points)* ch->level )
							drop_level(ch);
						check_perm_damage(ch);
					}
			}
			
			
			if (IS_NPC(ch))
				wiznet(log_buf,NULL,NULL,WIZ_MOBDEATHS,0,0);
			else
				wiznet(log_buf,NULL,NULL,WIZ_DEATHS,0,0);
			
			raw_kill( ch, ch );
			}
		}


	/* End of bleeding update code */

	if ( !IS_NPC(ch) && !IS_IMMORTAL(ch) && 
	     (ch->race == race_lookup("illithid")) )
	{
		int sn;
		sn=skill_lookup("human");
		ch->pcdata->learned[sn] = 100;
		sn=skill_lookup("dwarven");
		ch->pcdata->learned[sn] = 100;
		sn=skill_lookup("trollish");
		ch->pcdata->learned[sn] = 100;
		sn=skill_lookup("elven");
		ch->pcdata->learned[sn] = 100;
		sn=skill_lookup("orcish");
		ch->pcdata->learned[sn] = 100;
		sn=skill_lookup("gnomish");
		ch->pcdata->learned[sn] = 100;
	}

/******* Forced Monthly Werewolf Shapechange ********************/

	if( !IS_NPC(ch) && IS_WEREBEAST(ch) && !IS_WEREFORM(ch)
             && (ch->desc != NULL) && !IS_IMMORTAL(ch)    ) 
	{
	   if( (time_info.day == 1 && time_info.hour> 20) ||
               (time_info.day == 2 && time_info.hour < 4) ) /* 8pm - 5am */
	   {

			ch->gnosis = UMAX(0, ch->gnosis - number_range(1, 3));

			if(ch->gnosis == 0)
			{
    		if(!ch->altform.short_descr || !ch->altform.description)
    		{
			ch->altform.type 	= ALTTYPE_LYCANTHROPE;
				ch->altform.subtype 	= number_range(WERE_WOLF, WERE_CROCODILE);
			setup_altform(ch, ch->altform.type, ch->altform.subtype);
    		}
    		if (form_alternate(ch) == true)
    		{
			ch->println("The power of the moon overwhelms you.");
    				act("$n collapses on the ground in agony and transforms into a Werebeast!", ch,NULL,NULL,TO_ROOM);
				ch->println("Pain shoots through your body as you transform into a Werebeast.");
    			WAIT_STATE( ch, 10 );
    		}
			}else
				ch->println("The beast rages within you under the full moon.");

	   }
	}

	if (!IS_NPC(ch) && IS_SET(ch->affected_by2, AFF2_STOCKADE))
	{
    		if(ch->pcdata->jail < current_time)
		{
			REMOVE_BIT(ch->affected_by2, AFF2_STOCKADE);
			ch->println("You are released from the stocks.");
    		}
	}

	if (!IS_NPC(ch) && IS_SET(ch->affected_by2, AFF2_PREGNANT) )
	{
		int chance = number_range(1,20);
		switch (chance)
		{
			case 1:		break;
			case 2:		break;
			case 3:		break;
			case 4:		break;
			case 5:
				ch->println("`#`YYou feel the baby move in your womb.`^");
				act( "$n is startled as her baby moves about inside her.", ch, NULL, NULL, TO_ROOM );
				break;
			case 6:		break;
			case 7:		break;
			case 8:		break;
			case 9:		break;
			case 10:
				ch->println("`#`YYou jump as your baby kicks in your womb.`^");
				act( "$n jumps as her baby kicks inside her.", ch, NULL, NULL, TO_ROOM );
				break;
			case 11:	break;
			case 12:	break;
			case 13:	break;
			case 14:	break;
			case 15:	break;
			case 16:	break;
			case 17:	break;
			case 18:	break;
			case 19:	break;
			case 20:	break;
			}

	}

	if (!IS_NPC(ch) && IS_SET(ch->affected_by2, AFF2_PREGNANT))
	{
    		if(ch->pcdata->pregnant < current_time)
		{
			REMOVE_BIT(ch->affected_by2, AFF2_PREGNANT);
			int labor = number_range(1,20);
			OBJ_DATA *baby;
		switch (labor)
		{
			case 1:	
			case 2:	
			case 3:	
			case 4:	
			case 5:
			case 6:	
			case 7:	
			case 8:	
			case 9:
				ch->println("`#`RYou begin to give birth to a baby `GGIRL.`^");
				act( "$n suddenly goes into labor.", ch, NULL, NULL, TO_ROOM );
				baby = create_object(get_obj_index(64));
				obj_to_char(baby, ch);
				break;
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
				ch->println("`#`RYour begin to give birth to a baby `CBOY.`^");
				act( "$n suddenly goes into labor.", ch, NULL, NULL, TO_ROOM );
				baby = create_object(get_obj_index(66));
				obj_to_char(baby, ch);
				break;
			case 20:
				ch->println("`#`RYou begin to go into labor, and its not just one!`^");
				act( "$n suddenly goes into labor.", ch, NULL, NULL, TO_ROOM );
				int chance = number_range(1,3);
				int sex = number_range(1,2);
				if(chance == 1)
				{
					ch->println("`#`YYou are having twins!`^");
					if(sex == 1)
					{
						baby = create_object(get_obj_index(64));
						ch->println("The first is a baby `GGIRL!!`x");
						obj_to_char(baby, ch);
					}
					if(sex == 2)
					{
						baby = create_object(get_obj_index(66));
						ch->println("The first is a baby `CBOY!!`x");
						obj_to_char(baby, ch);
					}
					sex = number_range(1,2);
					if(sex == 1)
					{
						baby = create_object(get_obj_index(64));
						ch->println("The next is a baby `GGIRL!!`x");
						obj_to_char(baby, ch);
					}
					if(sex == 2)
					{
						baby = create_object(get_obj_index(66));
						ch->println("The next is a baby `CBOY!!`x");
						obj_to_char(baby, ch);
					}
				} 
				if(chance == 2)
				{
					ch->println("`#`YYou are having triplettes!`^");
					if(sex == 1)
					{
						baby = create_object(get_obj_index(64));
						ch->println("The first is a baby `GGIRL!!`x");
						obj_to_char(baby, ch);
					}
					if(sex == 2)
					{
						baby = create_object(get_obj_index(66));
						ch->println("The first is a baby `CBOY!!`x");
						obj_to_char(baby, ch);
					}
					sex = number_range(1,2);
					if(sex == 1)
					{
						baby = create_object(get_obj_index(64));
						ch->println("The next is a baby `GGIRL!!`x");
						obj_to_char(baby, ch);
					}
					if(sex == 2)
					{
						baby = create_object(get_obj_index(66));
						ch->println("The next is a baby `CBOY!!`x");
						obj_to_char(baby, ch);
					}
					sex = number_range(1,2);
					if(sex == 1)
					{
						baby = create_object(get_obj_index(64));
						ch->println("The next is a baby `GGIRL!!`x");
						obj_to_char(baby, ch);
					}
					if(sex == 2)
					{
						baby = create_object(get_obj_index(66));
						ch->println("The next is a baby `CBOY!!`x");
						obj_to_char(baby, ch);
					}
				}
				if(chance == 3)
				{
					ch->println("`#`YYou are having quadtuplets!`^");
					if(sex == 1)
					{
						baby = create_object(get_obj_index(64));
						ch->println("The first is a baby `GGIRL!!`x");
						obj_to_char(baby, ch);
					}
					if(sex == 2)
					{
						baby = create_object(get_obj_index(66));
						ch->println("The first is a baby `CBOY!!`x");
						obj_to_char(baby, ch);
					}
					sex = number_range(1,2);
					if(sex == 1)
					{
						baby = create_object(get_obj_index(64));
						ch->println("The next is a baby `GGIRL!!`x");
						obj_to_char(baby, ch);
					}
					if(sex == 2)
					{
						baby = create_object(get_obj_index(66));
						ch->println("The next is a baby `CBOY!!`x");
						obj_to_char(baby, ch);
					}
					sex = number_range(1,2);
					if(sex == 1)
					{
						baby = create_object(get_obj_index(64));
						ch->println("The next is a baby `GGIRL!!`x");
						obj_to_char(baby, ch);
					}
					if(sex == 2)
					{
						baby = create_object(get_obj_index(66));
						ch->println("The next is a baby `CBOY!!`x");
						obj_to_char(baby, ch);
					}
					sex = number_range(1,2);
					if(sex == 1)
					{
						baby = create_object(get_obj_index(64));
						ch->println("The next is a baby `GGIRL!!`x");
						obj_to_char(baby, ch);
					}
					if(sex == 2)
					{
						baby = create_object(get_obj_index(66));
						ch->println("The next is a baby `CBOY!!`x");
						obj_to_char(baby, ch);
					}
				}
				break;
			}
    		}
	}

  if (!IS_NPC(ch) && (ch->race == race_lookup("gargoyle")))
  {
	if( (time_info.hour >= 7 ) && (time_info.hour <= 18) ) /* 7am - 7pm */
	{
		if (!IS_SET(ch->affected_by, AFF_SLOW))
		{
			ch->printlnf( "You feel yourself slow down." );
			SET_BIT(ch->affected_by, AFF_SLOW);
		}
	}
	else if (IS_SET(ch->affected_by, AFF_SLOW))
	{
		ch->printlnf( "You feel yourself return to normal." );
		REMOVE_BIT(ch->affected_by, AFF_SLOW);
	}

	if( time_info.hour == 12 )  /* Noon Stone */
	{
		if (!IS_SET(ch->affected_by2, AFF2_STONE_GARGOYLE)) 
		{
			ch->printlnf( "You turn into your statue form to rest." );
			act( "`#$n turns into a `Sstone statue.`^", ch, NULL, NULL, TO_ROOM );
			SET_BIT(ch->affected_by2, AFF2_STONE_GARGOYLE);
			SET_BIT(ch->affected_by2, AFF2_MUTE);
			landchar(ch);
			ch->position = POS_RESTING;
		}
	}
	else if (IS_SET(ch->affected_by2, AFF2_STONE_GARGOYLE))
	{
		ch->printlnf( "You are no longer in statue form." );
		act( "`#$n turns back into a flesh form.`^", ch, NULL, NULL, TO_ROOM );
		REMOVE_BIT(ch->affected_by2, AFF2_STONE_GARGOYLE);
		REMOVE_BIT(ch->affected_by2, AFF2_MUTE);
	}

    }

   // Highlander Buzz Check on TICK
	if (!IS_OOC(ch) && !IS_NPC(ch) && IS_AFFECTED2(ch, AFF2_HIGHLANDER))
	{
		do_buzz2(ch,"");
	}

    if(IS_IMMORTAL(ch) && ch->name != "Ixliam" && !IS_SET(ch->act, PLR_LOG))
    {
	SET_BIT(ch->act, PLR_LOG);
    }


/************** Begin Remort Adjustments *********************************/

	affect_parentspellfunc_strip( ch, gsn_wishes);

    if (!IS_NPC(ch) && IS_SET(ch->wish, WISH_SANCTUARY) &&
        !IS_AFFECTED( ch, AFF_SANCTUARY) && ch != NULL)
    {
		AFFECT_DATA af;
		af.where     = WHERE_AFFECTS;
		af.type      = gsn_wishes;
		af.level     = 100;
		af.duration  = -1;
		af.location  = APPLY_NONE;
		af.modifier  = 0;
		af.bitvector = 0;
		af.bitvector = AFF_SANCTUARY;
		affect_to_char( ch, &af );
    }
    if (!IS_NPC(ch) && IS_SET(ch->wish, WISH_VICEGRIP) &&
        !IS_AFFECTED2(ch, AFF2_VICEGRIP) && ch != NULL)
    {
		AFFECT_DATA af;
		af.where     = WHERE_AFFECTS2;
		af.type      = gsn_wishes;
		af.level     = 100;
		af.duration  = -1;
		af.location  = APPLY_NONE;
		af.modifier  = 0;
		af.bitvector = 0;
		af.bitvector = AFF2_VICEGRIP;
		affect_to_char( ch, &af );
    }
    if (!IS_NPC(ch) && IS_SET(ch->wish, WISH_REGEN) &&
        !IS_AFFECTED( ch, AFF_REGENERATION) && ch != NULL)
    {
		AFFECT_DATA af;
		af.where     = WHERE_AFFECTS;
		af.type      = gsn_wishes;
		af.level     = 100;
		af.duration  = -1;
		af.location  = APPLY_NONE;
		af.modifier  = 0;
		af.bitvector = 0;
		af.bitvector = AFF_REGENERATION;
		affect_to_char( ch, &af );
    }
    if (!IS_NPC(ch) && IS_SET(ch->wish, WISH_OTTERLUNGS) &&
        !IS_AFFECTED( ch, AFF_OTTERLUNGS) && ch != NULL)
    {
		AFFECT_DATA af;
		af.where     = WHERE_AFFECTS;
		af.type      = gsn_wishes;
		af.level     = 100;
		af.duration  = -1;
		af.location  = APPLY_NONE;
		af.modifier  = 0;
		af.bitvector = 0;
		af.bitvector = AFF_OTTERLUNGS;
		affect_to_char( ch, &af );
    }
    if (!IS_NPC(ch) && IS_SET(ch->wish, WISH_FLYING) &&
        !IS_AFFECTED( ch, AFF_FLYING) && ch != NULL)
    {
		AFFECT_DATA af;
		af.where     = WHERE_AFFECTS;
		af.type      = gsn_wishes;
		af.level     = 100;
		af.duration  = -1;
		af.location  = APPLY_NONE;
		af.modifier  = 0;
		af.bitvector = 0;
		af.bitvector = AFF_FLYING;
		affect_to_char( ch, &af );
    }
    if (!IS_NPC(ch) && IS_SET(ch->wish, WISH_SCRYPROOF) &&
        !IS_SET(ch->imm_flags, IMM_SCRY) && ch != NULL)
    {
	SET_BIT(ch->imm_flags, IMM_SCRY);
    }
    if (!IS_NPC(ch) && IS_SET(ch->wish, WISH_DARKVISION) &&
        !IS_AFFECTED( ch, AFF_DARK_VISION) && ch != NULL)
    {
		AFFECT_DATA af;
		af.where     = WHERE_AFFECTS;
		af.type      = gsn_wishes;
		af.level     = 100;
		af.duration  = -1;
		af.location  = APPLY_NONE;
		af.modifier  = 0;
		af.bitvector = 0;
		af.bitvector = AFF_DARK_VISION;
		affect_to_char( ch, &af );
    }
    if (!IS_NPC(ch) && IS_SET(ch->wish, WISH_HEARING) &&
        !is_affected( ch, gsn_augment_hearing ) && ch != NULL)
    {
	    affect_parentspellfunc_strip( ch, gsn_augment_hearing);
	    AFFECT_DATA af;
	    af.where	= WHERE_AFFECTS;
	    af.type      	= gsn_augment_hearing;
	    af.level	= 120;
	    af.duration  	= -1;
	    af.location  	= APPLY_NONE;
	    af.modifier  	= 0;
	    af.bitvector 	= 0;
	    affect_to_char( ch, &af );
    }

    if (!IS_NPC(ch) && !is_affected( ch, gsn_beholder ) && ch != NULL && ch->race == race_lookup("beholder"))
    {
	    affect_parentspellfunc_strip( ch, gsn_beholder);
	    AFFECT_DATA af;
	    af.where		= WHERE_AFFECTS;
	    af.type     	= gsn_beholder;
	    af.level		= 120;
	    af.duration  	= -1;
	    af.location  	= APPLY_AC;
	    af.modifier  	= -5 * (ch->level/5);
	    af.bitvector 	= 0;
	    affect_to_char( ch, &af );
    }

    if(event_active != 0)	// Check for active event and run if needed
    	event_check(ch, "");

    check_pending_pigeon(ch);

    if (!IS_NPC(ch) && (ch->remort > 0) && !is_affected(ch, gsn_remort) && ch != NULL)
    {
		AFFECT_DATA af;
		af.where     = WHERE_AFFECTS;
		af.type      = gsn_remort;
		af.level     = 120;
		af.duration  = -1;
		af.location  = APPLY_ATTRIBUTES;
		af.modifier  = 2 * ch->remort;
		af.bitvector = 0;
		affect_to_char( ch, &af );
		af.location  = APPLY_HIT;
		af.modifier  = 25 * ch->remort;
		af.bitvector = 0;
		affect_to_char( ch, &af );
  
    }
    
/************** End Remort Adjustments *********************************/

	if(IS_VAMPIRE(ch))
	{
		if(ch->bloodpool <= -1)
		   ch->bloodpool = 0;
		if(IS_VAMPAFF(ch, VAM_BAT))
		{
			ch->bloodpool -= 1;
			if(ch->bloodpool <= 1)
			{
				do_vampchange(ch, "");
			}
		}
		if(IS_VAMPAFF(ch, VAM_MIST))
		{
			ch->bloodpool -= 1;
			if(ch->bloodpool <= 1)
			{
				do_vampchange(ch, "");
			}
		}
		if(IS_VAMPAFF(ch, VAM_WOLF))
		{
			ch->bloodpool -= 1;
			if(ch->bloodpool <= 1)
			{
				do_vampchange(ch, "");
			}
		}
		if(IS_VAMPAFF(ch, VAM_CLAWS))
		{
			ch->bloodpool -= 1;
			if(ch->bloodpool <= 1)
			{
				do_claws(ch, "");
			}
		}
		if(IS_VAMPAFF(ch, VAM_NIGHTSIGHT))
		{
			ch->bloodpool -= 1;
			if(ch->bloodpool <= 1)
			{
				do_nightsight(ch, "");
			}
		}
		if(IS_VAMPAFF(ch, VAM_TRUESIGHT))
		{
			ch->bloodpool -= 1;
			if(ch->bloodpool <= 1)
			{
				do_truesight(ch, "");
			}
		}
		if(IS_VAMPAFF(ch, VAM_DISGUISED))
		{
			ch->bloodpool -= 1;
			if(ch->bloodpool <= 1)
			{
				char      buf [MIL];
	   			sprintf(buf, "Your form shimmers and transforms into %s.",ch->name);
	   			act(buf,ch,NULL,NULL,TO_CHAR);
	   			sprintf(buf, "%s's form shimmers and transforms into %s.",ch->short_descr, ch->orig_short_descr );
	   			act(buf,ch,NULL,NULL,TO_ROOM);
	   			REMOVE_BIT(ch->vampire, VAM_DISGUISED);
    	   			ch->short_descr = str_dup(ch->orig_short_descr);
    	   			ch->description = str_dup(ch->orig_description);
			}
		}
		if(IS_AFFECTED2(ch, AFF2_SHADOWSIGHT) && 
         !IS_AFFECTED2(ch, AFF2_SHADOWPLANE) && IS_VAMPIRE(ch))
		{
			ch->bloodpool -= 1;
			if(ch->bloodpool <= 1)
			{
				do_shadowsight(ch,"");
			}
		}

		if(ch->bloodpool <= -1)
		   ch->bloodpool = 0;

		affect_parentspellfunc_strip( ch, gsn_vampire);

		AFFECT_DATA af;
		af.where     = WHERE_AFFECTS;
		af.type      = gsn_vampire;
		af.level     = 120;
		af.duration  = -1;
		af.location  = APPLY_NONE;
		af.modifier  = 0;
		af.bitvector = 0;

		af.location	= APPLY_ST;
		if( ch->vampgen == 13)
			af.modifier  = 10;
		if( ch->vampgen == 12)
			af.modifier  = 15;
		if( ch->vampgen == 11)
			af.modifier  = 18;
		if( ch->vampgen == 10)
			af.modifier  = 20;
		if( ch->vampgen == 9)
			af.modifier  = 22;
		if( ch->vampgen == 8)
			af.modifier  = 24;
		if( ch->vampgen == 7)
			af.modifier  = 26;
		if( ch->vampgen == 6)
			af.modifier  = 28;
		if( ch->vampgen == 5)
			af.modifier  = 30;
		if( ch->vampgen == 4)
			af.modifier  = 35;
		if( ch->vampgen == 3)
			af.modifier  = 40;
		if( ch->vampgen == 2)
			af.modifier  = 45;
		if( ch->vampgen == 1)
			af.modifier  = 50;
		af.bitvector	= 0;
		affect_to_char( ch, &af );

		af.where      = WHERE_AFFECTS2;
		af.type	      = gsn_vampire;
		af.level      = 120;
		af.duration   = -1;
		af.location   = APPLY_NONE;
		af.modifier   = 0;

		af.where     = WHERE_VULN;
		af.type      = gsn_vampire;
		af.level     = 120;
		af.duration  = -1;
		af.location  = APPLY_NONE;
		af.modifier  = 0;
		af.bitvector = VULN_LIGHT;
		affect_to_char(ch, &af);
		af.bitvector = VULN_HOLY;
		affect_to_char(ch, &af);

		af.where     = WHERE_IMMUNE;
		af.type      = gsn_vampire;
		af.level     = 120;
		af.duration  = -1;
		af.location  = APPLY_NONE;
		af.modifier  = 0;
		af.bitvector = IMM_FEAR;
		affect_to_char(ch, &af);
		af.bitvector = IMM_DISEASE;
		affect_to_char(ch, &af);
		af.bitvector = IMM_SLEEP;
		affect_to_char(ch, &af);
		af.bitvector = IMM_CHARM;
		affect_to_char(ch, &af);
	}
	/* End Vampire Resistances */

	/* Crazy Item Options */

	OBJ_DATA  *wrist1, *wrist2, *finger1, *finger2;
	char_data *wch = NULL;
	AFFECT_DATA af;

	wrist1 = get_eq_char(ch, WEAR_WRIST_L);
	wrist2 = get_eq_char(ch, WEAR_WRIST_R);
	finger1 = get_eq_char(ch, WEAR_FINGER_L);
	finger2 = get_eq_char(ch, WEAR_FINGER_R);
	int chance;

	if ((wrist1!=NULL && wrist1->pIndexData->vnum==5097) ||
	    (wrist2!=NULL && wrist2->pIndexData->vnum==5097))
	{
		chance = number_range(1,10);
		if (chance == 4)
		{
			act( "`#$n's `GB`gracelet `wof the `GS`gerpent`^ eyes glow `Rred`^.", ch, NULL, NULL, TO_ROOM );
			ch->println( "`#Your `GB`gracelet `wof the `GS`gerpent's`^ eyes glow `Rred.`^");
		}
		if (chance == 5)
		{
			act( "`#$n's bracelet turns into a `Ygolden serpent`^ and slithers around their wrist.", ch, NULL, NULL, TO_ROOM );
			ch->println( "`#Your bracelet turns into a `Ygolden serpent`^ and slithers up around your wrist for a moment.");
		}
	}


	if ((finger1!=NULL && finger1->pIndexData->vnum==5095) ||
	    (finger2!=NULL && finger2->pIndexData->vnum==5095))
	{
		chance = number_range(1,20);
		if (chance == 5)
		{
			act( "`#$n's `SRing `wof `SCralaxtyr`^ glows with dark energy.", ch, NULL, NULL, TO_ROOM );
			ch->println( "`#Your `SRing `wof `SCralaxtyr`^ glows with dark energy.");

			for ( wch = ch->in_room->people; wch != NULL; wch = wch->next_in_room )
			{
				if(IS_SET(wch->act, ACT_IS_UNSEEN) || IS_SET(wch->imm_flags, IMM_FEAR) ||
				  (IS_SET(wch->res_flags, RES_FEAR) && number_range(1,3) != 1))
					continue;

				af.where     = WHERE_AFFECTS;
				af.type      = gsn_cause_fear;
				af.level     = 90;
				af.duration  = 3;
				af.location  = APPLY_NONE;
				af.modifier  = 0;
				af.bitvector = AFF_FEAR;
				affect_to_char( wch, &af );
				wch->println("You panic as you are gripped by an incredible fear.");
				act( "$n screams and runs away.", wch, NULL, NULL, TO_ROOM );
				do_fearful( wch ); /* in magic.c */
			}



		}
	}


		if (is_affected(ch, gsn_despair) && ch != NULL)
		{
	    		AFFECT_DATA *af;
	    		int dam;
		
			act( "$n cries in agony from hopelessness and despair.", ch, NULL, NULL, TO_ROOM );
			ch->println( "You cry in agony from the despair and hopelessness of it all.");

			for ( af = ch->affected; af != NULL; af = af->next )
			{
				if (af->type == gsn_despair)
					break;
			}

			if (af == NULL)
			{
				REMOVE_BIT(ch->affected_by2,AFF2_DESPAIR);
				continue;
			}


			dam = 20 + (af->level/5+1);
			if (ch->mana > 1)
				ch->mana -= dam;
			dam = 40 + (af->level/5+1);
			if (ch->move > 1)
				ch->move -= dam;
			dam = 15 + (ch->level/5+1);
				damage( ch, ch, dam, TYPE_UNDEFINED, DAM_MENTAL, false);

		}
		if(is_affected(ch, gsn_raging_fever))
		{
			int fever_dam;
			fever_dam = ch->level/5+1;
			ch->mana -= fever_dam;
			ch->move -= fever_dam;
			damage( ch, ch, fever_dam, gsn_plague,DAM_DISEASE,false);
			act("$n collapses as fever takes $m.",
				ch,NULL,NULL,TO_ROOM);
			ch->println("You feel your fever burn through your veins.");
		}

			if (is_affected(ch, gsn_plague) && ch != NULL)
			{
				AFFECT_DATA *af, plague;
				char_data *vch;
				int dam;
				
				if (ch->in_room == NULL)
					continue;
				
				act("$n writhes in agony as plague sores erupt from $s skin.",
					ch,NULL,NULL,TO_ROOM);
				ch->println("You writhe in agony from the plague.");
				for ( af = ch->affected; af != NULL; af = af->next )
				{
					if (af->type == gsn_plague)
						break;
				}
				
				if (af == NULL)
				{
					REMOVE_BIT(ch->affected_by,AFF_PLAGUE);
					continue;
				}
				
				if (af->level == 1)
					continue;
				
				plague.where			= WHERE_AFFECTS;
				plague.type 			= gsn_plague;
				plague.level			= af->level - 1;
				plague.duration 		= number_range(1,2 * plague.level);
				plague.location 		= APPLY_ST;
				plague.modifier 		= -10;
				plague.bitvector		= AFF_PLAGUE;
				
				// spread the plague to others
				for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
				{
					if (!saves_spell(plague.level - 2,vch,DAM_DISEASE)
						&&	!IS_ICIMMORTAL(vch)
						&&	!IS_AFFECTED(vch,AFF_PLAGUE) && number_bits(3) == 0)
					{
						vch->println("You feel hot and feverish.");
						act("$n shivers and looks very ill.",vch,NULL,NULL,TO_ROOM);
						affect_join(vch,&plague);
					}
				}
				
				dam = UMIN(ch->level,af->level/5+1);
				ch->mana -= dam;
				ch->move -= dam;
				damage( ch, ch, dam, gsn_plague,DAM_DISEASE,false);
			}
			else if ( IS_AFFECTED(ch, AFF_POISON) && ch != NULL
				&&   !IS_AFFECTED(ch,AFF_SLOW))
				
			{
				AFFECT_DATA *poison;
				
				poison = affect_find(ch->affected,gsn_poison);
				
				if (poison != NULL)
				{
					act( "$n shivers and suffers.", ch, NULL, NULL, TO_ROOM );
					ch->println("You shiver and suffer.");
					damage(ch,ch,poison->level/10 + 1,gsn_poison,
						DAM_POISON,false);
					if(IS_AFFECTED(ch, AFF_HASTE))
						damage(ch,ch,poison->level/10 + 1,gsn_poison,
							DAM_POISON,false);
				}
            }			
			else if ( ch->position == POS_INCAP && number_range(0,1) == 0)
			{
				damage( ch, ch, 1, TYPE_UNDEFINED, DAM_NONE,false);
			}
			else if ( ch->position == POS_MORTAL )
			{
				damage( ch, ch, 1, TYPE_UNDEFINED, DAM_NONE,false);
            }else if (ch->hit <= -50 ){
				// get rid of those that have less than -50 hp
				// - hack till subdue is written properly
				if (IS_IMMORTAL(ch)){
					ch->hit=ch->max_hit;
				}else{
					damage( ch, ch, 1, TYPE_UNDEFINED, DAM_NONE,false);
				}
            }
		}
	}

	// Autosave
	// Check that these chars still exist.
	for ( ch = player_list; ch; ch = ch_next )
	{
		ch_next = ch->next_player;
		
		if (ch->desc != NULL && ch->desc->connected_socket % 10 == save_number)
		{
			save_char_obj(ch);
		}
	}
	
	// replacement autoquit by Kal - June 98
	for ( ch = player_list; ch; ch = ch_next )
    {
		ch_next = ch->next_player;
		
		if (ch->idle>4)
		{
			if ((ch->idle>5) && IS_LINKDEAD(ch)){
				quit_char(ch, "confirm", true);
			}else if ((ch->idle>15) && !IS_IMMORTAL(ch)){
				quit_char(ch, "confirm", true);
			}else if (ch->level<100 && ch->idle>60 && !GAMESETTING2(GAMESET2_NO_AUTOLOGOFF_FOR_IMM)){
				quit_char(ch, "confirm", true);
			}

			if(ch->desc && HAS_CONFIG2(ch, CONFIG2_AUTOKEEPALIVE)){ 
				// anti 'idle timeout'/'keepalive' code
				// Some ADSL/Cable routers timeout inactive TCP 
				// connections after about 30 minutes.  Using IAC NOP
				// we can ensure data is sent over a tcp connection
				// while hopefully not displaying anything to the
				// players connection as IAC NOP should be ignored 
				// by all telnet clients, if a telnet client doesn't 
				// support a basic telnet option implementation
				// then that is there problem as the client 
				// is too primative *grin* - Kal, Jan 02 :)
				#define	IAC	255		// interpret as command
				#define	NOP	241		// no operation
				const   unsigned char    nop_str  [] = { IAC, NOP, '\0' };
				ch->desc->write((char*)&nop_str[0],2);
			}
		}
	}
	return;
}

/**************************************************************************/
// drop nonplaying idle connections etc after 3 minutes
void connections_update( void )
{
    connection_data *c, *c_next;
    for ( c = connection_list; c; c = c_next )
    {
		c_next = c->next;

        if (c->connected_state != CON_PLAYING 
			&& c->connected_state!= CON_FTP_COMMAND
			&& c->connected_state!= CON_FTP_DATA)
		{
			if ((current_time - c->idle_since)>3*60){
				c->write("\r\n\r\nYou have been idle too long, disconnecting.\r\n",0);
				logf("%s %s (%d) dropped (idle too long).", 
					CH(c)?CH(c)->name:"(unknown name)", c->remote_hostname, c->connected_socket);
				connection_close(c);
			}else if ((current_time - c->idle_since)>120){
				if(c->warned_about_idle==false){
					c->write("\r\nNotice: While creating a character, you can be idle for up to around 3 minutes\r\n"
						         "        before being disconnected, you have been idle for 2 minutes so far.\r\n",0);
					c->warned_about_idle=true;
				}
			}else{
				c->warned_about_idle=false;
			}
		}
	}
}
	
/**************************************************************************/
// Update all objs.
// This function is performance sensitive.
void obj_update( void )
{
	OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    AFFECT_DATA *paf, *paf_next;

    for ( obj = object_list; obj; obj = obj_next )
    {
		char_data *rch;
		char *message;
		int con=0;
		int pcon=0;
		int level=0;
		
		obj_next = obj->next;
		
		if (obj->carried_by){
			if (IS_OOC(obj->carried_by)){ // no update on objects in ooc rooms
				continue;
			}
			if(IS_SET(obj->extra2_flags, OBJEXTRA2_REGROWING)
			&& obj->wear_loc != WEAR_NONE
			&& obj->condition >= 20)
				obj->condition =  URANGE(obj->condition + 1, obj->condition + number_range(obj->level/20, obj->level/10), obj->pIndexData->condition);


		}
		
		if(obj->ospec_fun == ospec_lookup("ospec_spitting_fire"))
			ospec_spitting_fire(obj, NULL);


		if( (obj->item_type == ITEM_WEAPON || obj->item_type == ITEM_ARMOR) &&
	     	     obj->pIndexData->vnum != OBJ_VNUM_RANDOM_OBJ){
			if(obj->condition<obj->pIndexData->condition){

				con=obj->condition;
				pcon=obj->pIndexData->condition+1;

				if(obj->item_type==ITEM_WEAPON){
					obj->value[1]=con * obj->pIndexData->value[1]/pcon;
					obj->value[2]=con * obj->pIndexData->value[2]/pcon;
				}else{
					obj->value[0]=con * obj->pIndexData->value[0]/pcon;
					obj->value[1]=con * obj->pIndexData->value[1]/pcon;
					obj->value[2]=con * obj->pIndexData->value[2]/pcon;
					obj->value[3]=con * obj->pIndexData->value[3]/pcon;
				}
			}
		}
		
		if( obj->item_type == ITEM_ARMOR && obj->pIndexData->vnum == OBJ_VNUM_RANDOM_OBJ)
		{

			if(obj->condition<100)
			{
				con=obj->condition;
				int level = obj->level;

				if(CAN_WEAR(obj, OBJWEAR_NECK ))
				{
			       		obj->value[0] = con * (level/6)/101;
	 	              		obj->value[1] = con * (level/6)/101;
			            	obj->value[2] = con * (level/6)/101;
			               	obj->value[3] = con * (level/8)/101;
				}
				else
				if(CAN_WEAR(obj, OBJWEAR_FEET ) || CAN_WEAR(obj, OBJWEAR_LEGS ))
				{
		       			obj->value[0] = con * (level/5)/101;
	               			obj->value[1] = con * (level/5)/101;
	               			obj->value[2] = con * (level/5)/101;
	               			obj->value[3] = con * (level/6)/101;
				}
				else
				if(CAN_WEAR(obj, OBJWEAR_ARMS ) || CAN_WEAR(obj, OBJWEAR_HEAD ))
				{
		       			obj->value[0] = con * (level/4)/101;
	               			obj->value[1] = con * (level/4)/101;
	               			obj->value[2] = con * (level/4)/101;
	               			obj->value[3] = con * (level/5)/101;
				}
				else
				if(CAN_WEAR(obj, OBJWEAR_SHIELD ) || CAN_WEAR(obj, OBJWEAR_TORSO ))
				{
		       			obj->value[0] = con * (level/3)/101;
	               			obj->value[1] = con * (level/3)/101;
	               			obj->value[2] = con * (level/3)/101;
	               			obj->value[3] = con * (level/4)/101;
				}
				else
				if(CAN_WEAR(obj, OBJWEAR_ABOUT ))
				{
		       			obj->value[0] = con * (level/4)/101;
	               			obj->value[1] = con * (level/4)/101;
	              		 	obj->value[2] = con * (level/4)/101;
	               			obj->value[3] = con * (level/5)/101;
				}
				else
				if(CAN_WEAR(obj, OBJWEAR_WAIST ))
				{
		       			obj->value[0] = con * (level/5)/101;
	               			obj->value[1] = con * (level/5)/101;
	               			obj->value[2] = con * (level/5)/101;
	               			obj->value[3] = con * (level/7)/101;
				}
				else
				if(CAN_WEAR(obj, OBJWEAR_WRIST ))
				{
		  			obj->value[0] = con * (level/7)/101;
	       				obj->value[1] = con * (level/7)/101;
	       				obj->value[2] = con * (level/7)/101;
	       				obj->value[3] = con * (level/9)/101;
				}
				else
				if(CAN_WEAR(obj, OBJWEAR_FACE ) || CAN_WEAR(obj, OBJWEAR_EYES ) ||
				   CAN_WEAR(obj, OBJWEAR_EAR ) ||  CAN_WEAR(obj, OBJWEAR_FINGER ))
				{
		       			obj->value[0] = con * (level/10)/101;
	               			obj->value[1] = con * (level/10)/101;
	               			obj->value[2] = con * (level/10)/101;
	               			obj->value[3] = con * (level/10)/101;
				}
				else
				if(CAN_WEAR(obj, OBJWEAR_ANKLE ))
				{
		       			obj->value[0] = con * (level/8)/101;
	               			obj->value[1] = con * (level/8)/101;
	               			obj->value[2] = con * (level/8)/101;
	               			obj->value[3] = con * (level/10)/101;
				}
		
			}
		}
		if( obj->item_type == ITEM_WEAPON && obj->pIndexData->vnum == OBJ_VNUM_RANDOM_OBJ)
		{

			if(obj->condition<100)
			{
				con=obj->condition;
				level = obj->level;

	   			if (obj->value[0] == 4)
				{
					obj->value[1] = con * (level / weapon_stat_lookup( level, 0, 0 )) / 101;
					obj->value[2] = con * (level / weapon_stat_lookup( level, 0, 1 )) / 101;
				}
				if (obj->value[0] == 2)
				{
			   		obj->value[1] = con * (level / weapon_stat_lookup( level-8, 0, 0 )) / 101;
			   		obj->value[2] = con * (level / weapon_stat_lookup( level-8, 0, 1 )) / 101;
				}
				if( obj->value[0] == 1)
				{
					obj->value[1] = con * (level / weapon_stat_lookup( level, 0, 0 )) / 101;
					obj->value[2] = con * (level / weapon_stat_lookup( level, 0, 1 )) / 101;
				}
				if(obj->value[0] == 5)
				{
					obj->value[1] = con * (level / weapon_stat_lookup( level+3, 0, 0 )) / 101;
					obj->value[2] = con * (level / weapon_stat_lookup( level+3, 0, 1 )) / 101;
				}
				if(obj->value[0] == 7)
				{
					obj->value[1] = con * (level / weapon_stat_lookup( level-3, 0, 0 )) / 101;
					obj->value[2] = con * (level / weapon_stat_lookup( level-3, 0, 1 )) / 101;
				}
				if(obj->value[0] == 6)
				{
			   		obj->value[1] = con * (level / weapon_stat_lookup( level-2, 0, 0 )) / 101;
			   		obj->value[2] = con * (level / weapon_stat_lookup( level-2, 0, 1 )) / 101;
				}
			}
		}
		
		// go through affects and decrement 
		for ( paf = obj->affected; paf; paf = paf_next ){
			paf_next= paf->next;
			
			if(paf->duration == -1)
				continue;			

			if ( paf->duration > 0 ){
				paf->duration--;
				if (number_range(0,4) == 0 && paf->level > 0){
					paf->level--;  // spell strength fades with time 
				}
			}else if ( paf->duration < 0 ){
			}else{
				if ( paf_next == NULL
					||   paf_next->type != paf->type
					||   paf_next->duration > 0 )
				{
					if ( paf->type > 0 && skill_table[paf->type].msg_obj )
					{
						if (obj->carried_by != NULL)
						{
							rch = obj->carried_by;
							act(skill_table[paf->type].msg_obj,
								rch,obj,NULL,TO_CHAR);
						}
						if (obj->in_room != NULL
							&& obj->in_room->people != NULL)
						{
							rch = obj->in_room->people;
							act(skill_table[paf->type].msg_obj,
								rch,obj,NULL,TO_ALL);
						}
					}
				}
				
				affect_remove_obj( obj, paf );
			}
		}
		
		if( (obj->carried_by == NULL) && (obj->in_obj==NULL))
		{

			
			if(number_percent()<20 && !IS_SET(obj->extra_flags,OBJEXTRA_NO_DEGRADE))
			{
				if(obj->in_room != NULL && !IS_SET(obj->in_room->room2_flags, ROOM2_STORAGE))
				{
					obj->condition--;
				}
			}
		}
		
		
		if ( (obj->timer <= 0 || --obj->timer > 0) && obj->condition!=0 ) {
			// FALLING CODE
			if ( obj->in_room
				&& obj->in_room->sector_type == SECT_AIR
				&& ( obj->wear_flags & OBJWEAR_TAKE )
				&& !IS_SET( obj->wear_flags, OBJWEAR_FLOAT )
				&& obj->in_room->exit[5]
				&& obj->in_room->exit[5]->u1.to_room) 
			{
				ROOM_INDEX_DATA *new_room = obj->in_room->exit[5]->u1.to_room;
				if (( rch = obj->in_room->people ) != NULL ) {
					act( "$p falls away.", rch, obj, NULL, TO_ROOM );
					act( "$p falls away.", rch, obj, NULL, TO_CHAR );
				}
				obj_from_room(obj);
				obj_to_room(obj, new_room);
				if(( rch = obj->in_room->people ) != NULL ) {
					if( obj->in_room
						&& obj->in_room->sector_type == SECT_AIR
						&& obj->in_room->exit[5]
						&& obj->in_room->exit[5]->u1.to_room ) 
					{
						act( "$p floats by.", rch, obj, NULL, TO_ROOM );
						act( "$p floats by.", rch, obj, NULL, TO_CHAR );
					}else{
						if( obj->in_room->sector_type == SECT_AIR ){
							act( "$p floats in.", rch, obj, NULL, TO_ROOM );
							act( "$p floats in.", rch, obj, NULL, TO_CHAR );
						}else{
							act( "With a resounding thud, $p lands on the floor.", rch, obj, NULL, TO_ROOM );
							act( "With a resounding thud, $p lands on the floor.", rch, obj, NULL, TO_CHAR );
						}
					}				
				}
			}
			continue;
		}
		
		switch ( obj->item_type )
		{
		default:
			if ( (obj->pIndexData->vnum == 60002) || 
			     (obj->pIndexData->vnum == 1901))
			{
				message = "$p dries up.";
			} else
			{
				message = "$p crumbles into dust.";
			}
			break;
		case ITEM_FURNITURE: {
			if ( obj->pIndexData->vnum == OBJ_VNUM_FIRE ) {
				OBJ_DATA	*ashes;
				
				message = "$p dies out.";
				
				if ( get_obj_index(OBJ_VNUM_ASHES) == NULL )
				{
					bugf("Vnum %d not found for ashes!", OBJ_VNUM_ASHES);
					return;
				}
				
				ashes = create_object( get_obj_index( OBJ_VNUM_ASHES ));
				ashes->timer = 12;
				obj_to_room( ashes, obj->in_room );
			}
			if ( obj->pIndexData->vnum == OBJ_VNUM_DIVINE_LIGHT ) {
				message = "The mists dissipate.";
			}else{
				message = "$p crumbles into dust.";
			}
			break;
							 }
		case ITEM_FOUNTAIN:	  message = "$p dries up.";         break;
		case ITEM_CORPSE_NPC: message = "$p decays into dust."; break;
		case ITEM_CORPSE_PC:  message = "$p decays into dust."; break;
		case ITEM_FOOD:       message = "$p decomposes.";       break;
		case ITEM_POTION:     message = "$p has evaporated from disuse.";
			break;
		case ITEM_PORTAL:     message = "$p fades out of existence."; break;
			
		case ITEM_CAULDRON:
		case ITEM_CONTAINER:
		case ITEM_FLASK:
		case ITEM_MORTAR:
			if (CAN_WEAR(obj,OBJWEAR_FLOAT)){
				if (obj->contains){
					message =  "$p flickers and vanishes, spilling its contents on the floor.";
				}else{
					message = "$p flickers and vanishes.";
				}
			}else{
				message = "$p crumbles into dust.";
			}
			break;
		}
		
		if ( obj->carried_by != NULL )
		{
			if (IS_NPC(obj->carried_by)
				&&  obj->carried_by->pIndexData->pShop != NULL)
				
				obj->carried_by->silver += obj->cost/5;
			else
			{
				if ( obj->item_type != ITEM_TOKEN )
					act( message, obj->carried_by, obj, NULL, TO_CHAR );
				if ( obj->wear_loc == WEAR_FLOAT)
					act(message,obj->carried_by,obj,NULL,TO_ROOM);
			}
		}
		else if ( obj->in_room != NULL
			&& ( rch = obj->in_room->people ) != NULL ) {
			
			if ( !(obj->in_obj && obj->in_obj->pIndexData->vnum == OBJ_VNUM_PIT
				&& !CAN_WEAR(obj->in_obj,OBJWEAR_TAKE)))
			{
				if ( obj->item_type != ITEM_TOKEN ) {
					act( message, rch, obj, NULL, TO_ROOM );
					act( message, rch, obj, NULL, TO_CHAR );
				}
			}
		}
		
		if ((obj->item_type == ITEM_CORPSE_PC || obj->wear_loc == WEAR_FLOAT)
			&&  obj->contains)
		{   // save the contents 
			OBJ_DATA *t_obj, *next_obj;
			
			for (t_obj = obj->contains; t_obj; t_obj = next_obj)
			{
				next_obj = t_obj->next_content;
				obj_from_obj(t_obj);
				
				if(obj->in_obj){ // in another object 
					obj_to_obj(t_obj,obj->in_obj);
				}else if (obj->carried_by){  // carried 
					if (obj->wear_loc == WEAR_FLOAT){
						if (obj->carried_by->in_room == NULL){
							extract_obj(t_obj);
						}else{
							obj_to_room(t_obj,obj->carried_by->in_room);
						}
					}else{
						obj_to_char(t_obj,obj->carried_by);
					}
				}else if (obj->in_room == NULL){  // destroy it 
					extract_obj(t_obj);
				}else{ // to a room 
					obj_to_room(t_obj,obj->in_room);
				}
			}
		}
		extract_obj( obj );
	}
	return;

}
 
/**************************************************************************/
// Aggression handler - mobs attacking players
//		for each mortal PC
//			for each mob in room
//				aggress on some random PC
void aggr_update( void )
{
    char_data *wch;
    char_data *wch_next;
    char_data *ch;
    char_data *ch_next;
    char_data *vch;
    char_data *vch_next;
    char_data *victim;
	int charmcount, charmlevels;

    for ( wch = player_list; wch; wch = wch_next )
    {
		wch_next = wch->next_player;

		charmcount=0;
		charmlevels=0;
		
		if ( IS_NPC(wch)
			||   wch->level >= LEVEL_IMMORTAL
			||   wch->in_room == NULL 
			||	 IS_SET(wch->comm, COMM_BUILDING )
			||   wch->in_room->area->empty	
			||   IS_SET(wch->in_room->room_flags,ROOM_SAFE)
			)
			continue;
		
		
		for ( ch = wch->in_room->people; ch != NULL; ch = ch_next )
		{
			int count;
			
			ch_next        = ch->next_in_room;

			if( IS_NPC(ch) 
				&& ch->master==wch
				&& wch->level<LEVEL_IMMORTAL
				&& IS_AFFECTED(ch, AFF_CHARM))
			{
				charmcount++;
				charmlevels+=ch->level;
			}
			
			if (// !IS_NPC(ch)||
				!IS_UNSWITCHED_MOB(ch) // switched mobs && PC's dont aggy
				||   !IS_SET(ch->act, ACT_AGGRESSIVE)
				||   IS_SET(ch->in_room->room_flags,ROOM_SAFE)
				||   IS_AFFECTED(ch,AFF_CALM)
				||   ch->fighting != NULL
				||   IS_AFFECTED(ch, AFF_CHARM)
				||   !IS_AWAKE(ch)
				||   ( IS_SET(ch->act, ACT_WIMPY) && IS_AWAKE(wch) )
				||   !can_see( ch, wch ) 
				||   number_bits(1) == 0)
				continue;
			
			
			// aggy mobs dont kill someone who was just killed 
			// and sitting at recall
			if  ( ch->pksafe>0 && 	
				ch->in_room->vnum == get_recallvnum(ch)){
				continue;
			}

			if	( is_affected( wch, gsn_aura_of_temperance )
				&& (( 12 + wch->level ) > ch->level )){
				continue;
			}
			
				/*
				* Ok we have a 'wch' player character and a 'ch' npc aggressor.
				* Now make the aggressor fight a RANDOM pc victim in the room,
				*   giving each 'vch' an equal chance of selection.
			*/
			count       = 0;
			victim      = NULL;
			for ( vch = wch->in_room->people; vch != NULL; vch = vch_next )
			{
				vch_next = vch->next_in_room;
				
				if ( !IS_NPC(vch)
					&&   vch->level < LEVEL_IMMORTAL
					&&   ch->level >= vch->level - 5 
					&&   ( !IS_SET(ch->act, ACT_WIMPY) || !IS_AWAKE(vch) )
					&&   can_see( ch, vch ) )
				{
					if ( number_range( 0, count ) == 0 )
						victim = vch;
					count++;
				}
			}
			
			if ( victim == NULL )
				continue;
			
			multi_hit( ch, victim, TYPE_UNDEFINED );
		}

		// handle charmies turning on their masters		
		charmlevels+= charmcount*20;
		if ((number_range(0,wch->fighting?3:9)==0)
			&& 
			number_range(0,	charmlevels - ( wch->level*(wch->fighting?3:5) ) ) 
					> 75)
		{		
			for ( ch = wch->in_room->people; ch != NULL; ch = ch_next )
			{
				ch_next        = ch->next_in_room;
			
				if (IS_NPC(ch)
					&& number_range(0,4)==0
					&& ch->master==wch
					&& IS_AFFECTED(ch, AFF_CHARM))
				{		
					char buf[MIL];

					// ridden mobs can't turn on you
					if(wch->mounted_on==ch){
						if(number_range(0,get_skill(wch, gsn_riding))>30){
							sprintf(buf,"'%s' attempts to turn and buck @ off but fails.", wch->name);
							do_pmote(ch, buf);
						}else{
							sprintf(buf,"'%s' turns on @ and bucks off @.", wch->name);	
							dismount(wch);
							do_pmote(ch, buf);
							affect_strip(ch,gsn_charm_person);
							multi_hit( ch, wch, TYPE_UNDEFINED);
						}
					}else{
						affect_strip(ch, gsn_charm_person);
						REMOVE_BIT(ch->affected_by, AFF_CHARM);
						stop_follower(ch);
						ch->leader = NULL;
						sprintf(buf,"'%s' turns on @.", wch->name);
						do_pmote(ch, buf);
						multi_hit( ch, wch, TYPE_UNDEFINED);
					}
				}
			}
		}

	}

    return;
}
/**************************************************************************/
// Kal - Jan 2001
void roomecho_update( void )
{
   for(char_data *ch = player_list; ch != NULL; ch = ch->next_player){
	   if( ch->in_room && ch->in_room->echoes) {
		   for( room_echo_data *re = ch->in_room->echoes; re; re = re->next ){
			   bool display_echo=false;
			   if ( re->firsthour<=re->lasthour){
				   if(re->firsthour<= time_info.hour 
				    && re->lasthour>= time_info.hour 
					&& number_percent()<=re->percentage)
				   {
					display_echo=true;
				   }
			   }else{
				   if(re->firsthour>= time_info.hour 
				    && re->lasthour<= time_info.hour 
					&& number_percent()<=re->percentage)
				   {
					display_echo=true;
				   }
			   }
			   if(display_echo){
				   if(IS_IMMORTAL(ch) || IS_SET(ch->comm,COMM_BUILDING)){
					   ch->print("RoomEcho: ");
				   }
				   ch->print(re->echotext);
				   ch->println("`x");
			   }
		   }
	   }
   }
}
/**************************************************************************/
void maint_update( void )
{
    connection_data *d;

    for ( d = connection_list; d; d = d->next )
    {

    if(d->connected_state == CON_PLAYING && maint_time() == 0)
    {
	d->character->println( "`#`Y************************************" );
	d->character->println( "`#`Y*  SYSTEM MAINTENANCE INITIALIZED  *" );
	d->character->println( "`#`Y*        DISCONNECTING NOW         *" );
	d->character->println( "`#`Y************************************`x" );
	
	if(d->character->level != 100)
	{
		if(d->character->fighting)
			stop_fighting( d->character, true );
		quit_char(d->character, "confirm", true);
	}
    }
    if(d->connected_state == CON_PLAYING && maint_time() == 1)
    {
	d->character->println( "`#`Y************************************" );
	d->character->println( "`#`Y* 1 MINUTE TILL SYSTEM MAINTENANCE *" );
	d->character->println( "`#`Y*   PLEASE LOG OFF AT THIS TIME.   *" );
	d->character->println( "`#`Y************************************`x" );
    }
    if(d->connected_state == CON_PLAYING && maint_time() == 2)
    {
	d->character->println( "`#`Y************************************" );
	d->character->println( "`#`Y* 2 MINUTE TILL SYSTEM MAINTENANCE *" );
	d->character->println( "`#`Y*    PLEASE LOG OFF AT THIS TIME.   *" );
	d->character->println( "`#`Y************************************`x" );
    }
    if(d->connected_state == CON_PLAYING && maint_time() == 3)
    {
	d->character->println( "`#`Y************************************" );
	d->character->println( "`#`Y* 3 MINUTES TILL SYSTEM MAINTENANCE *" );
	d->character->println( "`#`Y*    PLEASE LOG OFF AT THIS TIME.   *" );
	d->character->println( "`#`Y************************************`x" );
    }
    if(d->connected_state == CON_PLAYING && maint_time() == 4)
    {
	d->character->println( "`#`Y************************************" );
	d->character->println( "`#`Y* 4 MINUTES TILL SYSTEM MAINTENANCE *" );
	d->character->println( "`#`Y*    PLEASE LOG OFF AT THIS TIME.   *" );
	d->character->println( "`#`Y************************************`x" );
    }
    if(d->connected_state == CON_PLAYING && maint_time() == 5)
    {
	d->character->println( "`#`Y************************************" );
	d->character->println( "`#`Y* 5 MINUTES TILL SYSTEM MAINTENANCE *" );
	d->character->println( "`#`Y*    PLEASE LOG OFF AT THIS TIME.   *" );
	d->character->println( "`#`Y************************************`x" );
    }
  }

}
/**************************************************************************/
// Daos - Dec. 2001
void areaecho_update( void )
{
   for(char_data *ch = player_list; ch; ch = ch->next_player)
   {
	   if( ch->in_room->area 		   && ch->in_room->area->echoes
		   && !IS_SET(ch->in_room->room2_flags, ROOM2_NO_AREA_ECHOES))
	   {
		   for( area_echo_data *ae = ch->in_room->area->echoes; ae; ae = ae->next )
		   {
			   bool display_echo=false;
			   if ( ae->firsthour<=ae->lasthour)
			   {
				   if(ae->firsthour<= time_info.hour 
				    && ae->lasthour>= time_info.hour 
					&& number_percent()<=ae->percentage)
				   {
					display_echo=true;
				   }
			   }else{
				   if(ae->firsthour>= time_info.hour 
				    && ae->lasthour<= time_info.hour 
					&& number_percent()<=ae->percentage)
				   {
					display_echo=true;
				   }
			   }
			   if(display_echo)
			   {
				   if(IS_IMMORTAL(ch) || IS_SET(ch->comm,COMM_BUILDING))
				   {
					   ch->print("AreaEcho: ");
				   }
				   ch->print(ae->echotext);
				   ch->println("`x");
			   }
		   }
	   }
   }
}

/**************************************************************************/
int	pulse_point;
void duel_update();
void process_mpqueue();
/**************************************************************************/
// Handle all kinds of updates.
// Called once per pulse from game loop.
// Random times to defeat tick-timing clients and players.
void update_handler( void )
{
	static  int     pulse_area;
	static  int     pulse_mobile;
	static  int     pulse_violence;
    	static  int     pulse_affects;
	static  int     pulse_minute;
	static  int     pulse_fishing;
	static  int     pulse_dream;
	static  int     pulse_song;
	
	if ( --pulse_area     <= 0 )
	{
		pulse_area      = PULSE_AREA;
		log_string("Updating Areas");
		number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 ); 
		area_update( );
		event_activate();
		quest_update( );
	}

	if ( --pulse_song <= 0)
	{
		pulse_song = PULSE_SONG;
		song_update( );
	}

	if ( --pulse_fishing <= 0)
	{
		pulse_fishing = PULSE_FISHING;
		check_fishing( );
	}

	if ( --pulse_dream <= 0)
	{
		pulse_dream = PULSE_DREAM;
		dream_update( );
	}

	if ( --pulse_mobile   <= 0 )
	{
		pulse_mobile    = PULSE_MOBILE; //Something in this
		mobile_update   ( );
	}

	
	if ( --pulse_violence <= 0 )
	{
		pulse_violence  = PULSE_VIOLENCE;
		violence_update ( );		
		duel_update();
	}
	
	if ( --pulse_affects <= 0 )    // update affects like fear etc
	{
		pulse_affects  = PULSE_AFFECTS; 
		affects_update ( );             // in affects.c
	}
	
	moot_update();
	
	if ( --pulse_point    <= 0 )
	{
		wiznet("TICK!",NULL,NULL,WIZ_TICKS,0,0);
		fulltime_log_string("Tick");
		tick_counter++;
		pulse_point     = number_range( PULSE_TICK / 2, 3 * PULSE_TICK / 2 );
		char_update();
		connections_update(); // drops idle connections
		maint_update();
		obj_update();
		event_update();
		roomecho_update();
		areaecho_update();
		maintence_saves(); // automatic saving of laston and olc etc		
	}
	
	if ( --pulse_minute <= 0)
	{
		pulse_minute= PULSE_MINUTE;
		weather_update ();
		tracktime_update(); // increase the tracktime variable and calls a 
							// reshuffle once every 4 days of continous running
							// when the unsigned short tracktime goes 
							// from 65k to 0.
	}

	// run the mob queue system
	process_mpqueue();
	
	aggr_update( );
	tail_chain( );
	return;
}
/**************************************************************************/
void moot_check_bring_forward();
/**************************************************************************/
void moot_update(void)
{
	if(moot->pulse<1){
		return;
	}

	moot->pulse--;

	if(moot->pulse<=0){
		resolve_moot();
	}else{
		if(moot->pulse%(20*PULSE_PER_SECOND)==0){
			broadcast_moot();
		}
		if(moot->pulse==PULSE_MOOT-16){
			moot_check_bring_forward();
		}
	}
	return;
}
/**************************************************************************/
void drop_level( char_data *ch )
{
	if(IS_NPC(ch)){
		return;
	}
	int add_mana;
	int add_move;
	int add_prac;
	
	ch->level--;
	ch->pcdata->sublevel=get_sublevels_for_level(ch->level);

	ch->println("You have lost a LEVEL!!!!!!");
	// swap sublevels for xp till reaching the balance
	while(ch->pcdata->sublevel>0 && XP_PAST_LEVEL(ch)<0)
	{
		ch->pcdata->sublevel--;
		ch->exp+=exp_per_level(ch,ch->pcdata->points);
		ch->println("You lose a sublevel!");
	}
	
	add_mana = 1 + (ch->perm_stats[STAT_PR] +
					ch->perm_stats[STAT_EM] +
					ch->perm_stats[STAT_IN]) / 20;
	
	if (!class_table[ch->clss].fMana)
		add_mana /= 2;

	add_move = 1 + number_range( 1,	(ch->modifiers[STAT_CO] + ch->modifiers[STAT_QU])/10);

	add_prac = 3 + ( ch->modifiers[STAT_CO] +
					 ch->modifiers[STAT_AG] +
					 ch->modifiers[STAT_SD] +
					 ch->modifiers[STAT_ME] +
					 ch->modifiers[STAT_RE] ) / 50 ;

	add_mana = add_mana * 9/10;
	add_move = add_move * 9/10;
	
	add_mana = UMAX( 2, add_mana );
	add_move = UMAX( 6, add_move );
	
	ch->max_mana -= add_mana;
	ch->max_move -= add_move;
	ch->practice -= add_prac;
	ch->train    -= 2;
	
	ch->pcdata->perm_mana -= add_mana;
	ch->pcdata->perm_move -= add_move;
	
	ch->printlnf(
		"Your gain is: -%d/%d mana, -%d/%d mv, -%d/%d prac.",
		add_mana,       ch->max_mana,
		add_move,       ch->max_move,
		add_prac,       ch->practice );
	return;
}
/**************************************************************************/
void check_perm_damage(char_data *ch)
{
	if ( IS_NPC( ch)){
		return;
	}

	int hp_loss, mana_loss, move_loss;
	int stat_loss;

	if(ch->perm_stats[STAT_CO]<number_percent() && ch->level>10 )
	{
		if(number_percent()<=50)
		{
			hp_loss=number_range(0,5);
			mana_loss=number_range(0,5);
			move_loss=number_range(0,5);

			ch->max_hit  -= hp_loss;
			ch->max_mana -= mana_loss;
			ch->max_move -= move_loss;
			ch->pcdata->perm_hit  -= hp_loss;
			ch->pcdata->perm_mana -= mana_loss;
			ch->pcdata->perm_move -= move_loss;

			ch->printlnf("You have lost %d hp, %d mana, and %d movement.",
								hp_loss, mana_loss, move_loss);
		}
		if(number_percent()<=7)
		{
			stat_loss=number_range(1,10);
			ch->perm_stats[STAT_ST]-=stat_loss;
			ch->printf("You have been weakened by your defeat by %d strength point%s.",
						stat_loss, stat_loss==1 ? "" : "s");
		}
		if(number_percent()<=5)
		{
			stat_loss=number_range(1,10);
			ch->perm_stats[STAT_RE]-=stat_loss;
			ch->printf("You have been feebleminded by your defeat by %d reason point%s.",
						stat_loss, stat_loss==1 ? "" : "s");
		}
		if(number_percent()<=5)
		{
			stat_loss=number_range(1,10);
			ch->perm_stats[STAT_IN]-=stat_loss;
			ch->printf("You have been disoriented by your defeat by %d intuition point%s.",
						stat_loss, stat_loss==1 ? "" : "s");
		}
		if(number_percent()<=5)
		{
			stat_loss=number_range(1,10);
			ch->perm_stats[STAT_QU]-=stat_loss;
			ch->printf("You have been slowed by your defeat by %d quickness point%s.",
						stat_loss, stat_loss==1 ? "" : "s");
		}
		if(number_percent()<=15)
		{
			stat_loss=number_range(1,10);
			ch->perm_stats[STAT_CO]-=stat_loss;
			ch->printf("You have been weakened by your defeat by %d constitution point%s.",
						stat_loss, stat_loss==1 ? "" : "s");
		}
	}else{
		ch->println("Luckily you have not suffered serious injury.");
	}
	return;
}
/**************************************************************************/
void do_heroxp( char_data *ch, int xp )
{
	char buf[MIL];
	if ( IS_NPC( ch )){
		return;
	}
	if(xp<0){
		ch->pcdata->heroxp= UMAX(0, ch->pcdata->heroxp-xp);
		return;
	}

	ch->pcdata->heroxp += xp;
	if(ch->pcdata->heroxp > 1000 ) {
		ch->print("You just gained a hero level!!!");
		ch->pcdata->heroxp -= 1000;
		ch->println("You gain 3 pracs and 1 train.");
		ch->practice	+= 3;
		ch->train		+= 1;

		sprintf(buf, "%s gained a hero level!!!", ch->name );
		wiznet(buf, ch, NULL, WIZ_LEVELS, 0, 0 );
		log_string(buf);
	}
	if ( ch->pcdata->heroxp < 0 ) ch->pcdata->heroxp = 0;
	return;
}
/**************************************************************************/
void room_update( AREA_DATA *pArea )
{
	ROOM_INDEX_DATA *room;
	int vnum;

	for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum ++ )
	{
		if ( (room = get_room_index(vnum)) )
		{
			room_aff_update(room);
//			denergy_update(room);
		}
	}
	return;
}
/**************************************************************************/
// Death energy Stuff
void denergy_update(ROOM_INDEX_DATA *room)
{
	ROOM_INDEX_DATA *pRoom;
	int exits = 0;
	int door;
	if(room->death_energy < sect_table[room->sector_type].death_energy)
		if((sect_table[room->death_energy].death_energy - room->death_energy) * 4 >= number_percent())
			room->death_energy++;
	else if(room->death_energy > sect_table[room->sector_type].death_energy)
		if((room->death_energy - sect_table[room->death_energy].death_energy) * 4 >= number_percent())
			room->death_energy--;
	
	for(door = 0;door<MAX_DIR; door++)
		if(room->exit[door] != NULL)
			exits++;

    for ( door = 0; door<MAX_DIR; door++ )
    {
        if ( ( room->exit[door] ) != NULL
            &&   (pRoom = room->exit[door]->u1.to_room) != NULL
            &&   !IS_SET(room->exit[door]->exit_info, EX_CLOSED) )
        {
			if(room->death_energy > pRoom->death_energy)
			{
				if(number_percent() <= (room->death_energy + pRoom->death_energy)
					                  +(pRoom->death_energy - sect_table[room->death_energy].death_energy) 
									  * 2 / exits)
					pRoom->death_energy++;
			}

		}
	}
	return;
}	
/**************************************************************************/
void room_aff_update( ROOM_INDEX_DATA *room )
{
	AFFECT_DATA *paf;
	AFFECT_DATA *paf_next;
	char_data *pChar;
	
	for ( paf = room->affected; paf != NULL; paf = paf_next )
	{
		paf_next	= paf->next;
		if ( paf->duration > 0 )
		{
			paf->duration--;
			if (number_range(0,4) == 0 && paf->level > 0)
				paf->level--;  // spell's level fades with time
		}
		else if ( paf->duration < 0 )
			;
		else
		{
			if ( paf_next == NULL
				||   paf_next->type != paf->type
				||   paf_next->duration > 0 )
			{
				// Display wear off message of spell
				if ( paf->type > 0 && skill_table[paf->type].msg_off )
				{
					for ( pChar = room->people; pChar; pChar = pChar->next_in_room )
					{
						act( "$t", pChar, skill_table[paf->type].msg_off, NULL, TO_CHAR );
						act( "$t", pChar, skill_table[paf->type].msg_off, NULL, TO_ROOM );
						break;
					}
				}
			}
			affect_remove_room( room, paf );
		}
	}
}
/**************************************************************************/
// WEATHER Stuff
static weather_influence_data	base_weather =
{
	SKY_CLEAR_RANGE, SKY_CLOUDY_RANGE, SKY_RAINY_RANGE, SKY_LIGHTNING_RANGE
};

static weather_influence_data	season_influence_data[] =
{
//	  CLEAR				CLOUDY			RAINY			LIGHTNING
	{ INFLUENCE_NONE,	INFLUENCE_NONE,	INFLUENCE_NONE,	INFLUENCE_M2	},	//	Winter
	{ INFLUENCE_M3,		INFLUENCE_M2,	INFLUENCE_NONE,	INFLUENCE_NONE	},	//	Spring
	{ INFLUENCE_NONE,	INFLUENCE_NONE,	INFLUENCE_NONE,	INFLUENCE_M2	},	//	Summer
	{ INFLUENCE_NONE,	INFLUENCE_NONE,	INFLUENCE_NONE,	INFLUENCE_M3	}	//	Autumn
};


static weather_influence_data	sector_weather_table[] =
{
//	  CLEAR				CLOUDY			RAINY			LIGHTNING
	{ INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE}, // SECT_INSIDE
	{ INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE}, // SECT_CITY
	{ INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_P1,   INFLUENCE_P1	}, // SECT_FIELD
	{ INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_M1  }, // SECT_FOREST
	{ INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_M2	}, // SECT_HILLS
	{ INFLUENCE_NONE, INFLUENCE_P2,	  INFLUENCE_P2,   INFLUENCE_NONE}, // SECT_MOUNTAIN
	{ INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE}, // SECT_WATER_SWIM
	{ INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE}, // SECT_WATER_NOSWIM
	{ INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE}, // SECT_SWAMP
	{ INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_M3,   INFLUENCE_NONE}, // SECT_AIR
	{ INFLUENCE_P4,   INFLUENCE_M4,   INFLUENCE_M4,   INFLUENCE_P4  }, // SECT_DESERT
	{ INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE}, // SECT_CAVE
	{ INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE}, // SECT_UNDERWATER
	{ INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE}, // SECT_SNOW
	{ INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE}, // SECT_ICE
	{ INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE}, // SECT_TRAIL
	{ INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE},  // SECT_LAVA
	{ INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE},  // SECT_VEHICLE
	{ INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE, INFLUENCE_NONE}  // SECT_GATEWAY
};


static int	weather_sector_conversion[SECT_MAX][SKY_MAX] =
{
	{ 0,1,2,3},	// SECT_INSIDE
	{ 0,1,2,3},	// SECT_CITY
	{ 0,1,2,3},	// SECT_FIELD
	{ 0,1,2,3},	// SECT_FOREST
	{ 0,1,2,3},	// SECT_HILLS
	{ 0,1,2,3},	// SECT_MOUNTAIN
	{ 0,1,2,3},	// SECT_WATER_SWIM
	{ 0,1,2,3},	// SECT_WATER_NOSWIM
	{ 0,1,2,3},	// SECT_SWAMP
	{ 0,1,2,3},	// SECT_AIR
	{ 0,0,0,3},	// SECT_DESERT
	{ 0,1,2,3},	// SECT_CAVE
	{ 0,1,2,3},	// SECT_UNDERWATER
	{ 0,1,2,3},	// SECT_SNOW
	{ 0,1,2,3}, // SECT_ICE
	{ 0,1,2,3}, // SECT_TRAIL
	{ 0,1,2,3},  // SECT_LAVA
	{ 0,1,2,3},  // SECT_GATEWAY
	{ 0,1,2,3}  // SECT_VEHICLE
};

static int master_weather_table[SECT_MAX][WEATHER_SEASON_MAX][SKY_MAX];

static char * weather_message[SECT_MAX][8] =
{
	// SECT_INSIDE (No Weather Messages)
	{
		// Cloudless
		"",
		"",
		// Cloudy
		"",
		"",
		// Rainy
		"",
		"",
		// Lightning
		"",
		""
	},
	// SECT_CITY
	{
		// Cloudless
		"Above the city, the clouds disperse.\r\n",
		"",
		// Cloudy
		"The soft drumbeat of rain on rooftops stops.\r\n",
		"Dark clouds gather far above the rooftops.\r\n",
		// Rainy
		"The echo of thunder rolling off nearby buildings fades.\r\n",
		"Rain begins to drench the city.\r\n",
		// Lightning
		"",
		"Bright flashes light up the sky and thunder crashes down.\r\n"
	},
	// SECT_FIELD
	{
		// Cloudless
		"A clear sky pokes through the cloud cover.\r\n",
		"",
		// Cloudy
		"The fields are no longer bathed in rain.\r\n",
		"The fields darken as clouds roll in, obscuring sky.\r\n",
		// Rainy
		"The peals of thunder fade into silence.\r\n",
		"The rains begin, drenching you to the bone.\r\n",
		// Lightning
		"",
		"Thunder and lightning assault the open field.\r\n"
	},
	// SECT_FOREST
	{
		// Cloudless
		"The trees breathe as the sky clears.\r\n",
		"",
		// Cloudy
		"The rain slows to a drizzle and then stops as suddenly as it started.\r\n",
		"The forest hushes as clouds gather, seeming to anticipate the rain.\r\n",
		// Rainy
		"The thunder that shook the forest rumbles one last time and is gone.\r\n",
		"The raindrops falling in the forest make a soothing sound.\r\n",
		// Lightning
		"",
		"With great force, a bolt of lightning strikes a tree near you.\r\n"
	},
	// SECT_HILLS
	{
		// Cloudless
		"The hilltops are no longer obscured by clouds.\r\n",
		"",
		// Cloudy
		"It stops raining.\r\n",
		"Silence settles on the countryside as clouds loom overhead.\r\n",
		// Rainy
		"The echoes of thunder on distant hills die away.\r\n",
		"The thirsty ground drink the rain as it begins to pour.\r\n",
		// Lightning
		"",
		"Lightning lights up the sky and thunder crashes all around you.\r\n"
	},
	// SECT_MOUNTAINS
	{
		// Cloudless
		"The mountains reach majestically towards a clear sky.\r\n",
		"",
		// Cloudy
		"The rain stops falling from blackened clouds.\r\n",
		"Heavy storm clouds gather.\r\n",
		// Rainy
		"The deafening thunder comes to a halt.\r\n",
		"Rain pours mercilessly down upon you.\r\n",
		// Lightning
		"",
		"Thunder roars and lightning crackles in the sky.\r\n"
	},
	//	SECT_WATER_SWIM
	{
		// Cloudless
		"The sky appears to clear from the cloud cover.\r\n",
		"",
		// Cloudy
		"The cacophony of rain dies down slowly.\r\n",
		"The water darkens as clouds form overhead.\r\n",
		// Rainy
		"The distant sound of thunder fades.\r\n",
		"The rainfall is almost overwhelming, seeming to come from every direction.\r\n",
		// Lightning
		"",
		"Flashes light the water around you as the air about you explodes in violent thunder.\r\n"
	},
	//	SECT_WATER_NO_SWIM
	{
		// Cloudless
		"The sky appears to clear from the cloud cover.\r\n",
		"",
		// Cloudy
		"The cacophony of rain dies down slowly.\r\n",
		"The water darkens as clouds form overhead.\r\n",
		// Rainy
		"The distant sound of thunder fades.\r\n",
		"The rainfall is almost overwhelming, seeming to come from every direction.\r\n",
		// Lightning
		"",
		"Flashes light the water around you as the air about you explodes in violent thunder.\r\n"
	},
	//	SECT_SWAMP
	{
		// Cloudless
		"The clouds overhead clear away.\r\n",
		"",
		// Cloudy
		"The rain stops falling.\r\n",
		"Clouds roll in, turning the sky dark and oppressive.\r\n",
		// Rainy
		"The distant sound of thunder fades away.\r\n",
		"The rain begins to fall turning the ground into a great mud pit.\r\n",
		// Lightning
		"",
		"Peals of thunder and arcs of lightning overwhelm your senses.\r\n",
	},
	// SECT_AIR
	{
		// Cloudless
		"The clouds around you dissipate.\r\n",
		"",
		// Cloudy
		"The rain falling around you tapers off.\r\n",
		"Clouds begin to form around you.\r\n",
		// Rainy
		"Flashes of lightning and thunderous claps die down.\r\n",
		"Rain forms all around you and makes it's descent to the ground.\r\n",
		// Lightning
		"",
		"Thunder shakes you about as lightning arcs dangerously close to you.\r\n"
	},
	// SECT_DESERT
	{
		// Cloudless
		"The cruel sky clears.\r\n",
		"",
		// Cloudy
		"",
		"",
		// Rainy
		"",
		"",
		// Lightning
		"",
		"Thunder rolls across the desert as an electrical storm comes to life.\r\n"
	},
	// SECT_CAVE (No Weather Messages)
	{
		// Cloudless
		"",
		"",
		// Cloudy
		"",
		"",
		// Rainy
		"",
		"",
		// Lightning
		"",
		""
	},
	// SECT_UNDERWATER (No Weather Messages)
	{
		// Cloudless
		"",
		"",
		// Cloudy
		"",
		"",
		// Rainy
		"",
		"",
		// Lightning
		"",
		""
	},
	// SECT_SNOW 
	{
		// Cloudless
		"A blinding glare is created as the sun shines down upon the snow.\r\n",
		"",
		// Cloudy
		"The clouds gather making it incredibly cold.\r\n",
		"",
		// Rainy
		"The rain sticks to the snow making it very slick and wet.\r\n",
		"",
		// Lightning
		"",
		""
	},
	// SECT_ICE 
	{
		// Cloudless
		"The sun shines creating a glare off the ice.\r\n",
		"",
		// Cloudy
		"",
		"",
		// Rainy
		"The rain sticks to the ice, making it more slick.\r\n",
		"",
		// Lightning
		"",
		"",
	},
	// SECT_TRAIL
	{
		// Cloudless
		"The sun gleams down on the open trail.\r\n",
		"Rays of sunlight break through the clouds.\r\n",
		// Cloudy
		"The clouds gather, darkening the trail ahead.\r\n",
		"The clouds darken making the trail hard to see.\r\n",
		// Rainy
		"Rain falls from the clouds above, making the trail very wet.\r\n",
		"High waters rise up onto the trail as a heavy downpour occurs.\r\n",
		// Lightning
		"Lightning flashes, illuminating the trail ahead of you.\r\n",
		"Violent roars of thunder clash as lightning flashes ahead of you.\r\n"
	},
	
	// SECT_LAVA
	{
		// Cloudless
		"",
		"",
		// Cloudy
		"",
		"",
		// Rainy
		"Rain falls on the molten lava creating a hot steam.\r\n",
		"The heavy downpour turns the steaming lava into rock.\r\n",
		// Lightning
		"",
		""
	},
 	// SECT_GATEWAY
 	{
 		// Cloudless
 		"The sun gleams down on the open trail.\r\n",
 		"Rays of sunlight break through the clouds.\r\n",
 		// Cloudy
 		"The clouds gather, darkening the trail ahead.\r\n",
 		"The clouds darken making the trail hard to see.\r\n",
 		// Rainy
 		"Rain falls from the clouds above, making the trail very wet.\r\n",
 		"High waters rise up onto the trail as a heavy downpour occurs.\r\n",
 		// Lightning
 		"Lightning flashes, illuminating the trail ahead of you.\r\n",
 		"Violent roars of thunder clash as lightning flashes ahead of you.\r\n"
	},
	// SECT_VEHICLE (No Weather Messages)
	{
		// Cloudless
		"",
		"",
		// Cloudy
		"",
		"",
		// Rainy
		"",
		"",
		// Lightning
		"",
		""
	}
};

/**************************************************************************/
int calculate_season( void )
{
	int season = 0;
	int day;

	day = time_info.month * ICTIME_DAYS_PER_MONTH + time_info.day;
	
	if ( day < ( ICTIME_DAYS_PER_YEAR / 8 )) {
		season = WEATHER_SEASON_WINTER;
	} else if ( day < ( ICTIME_DAYS_PER_YEAR / 8 ) * 3 ) {
		season = WEATHER_SEASON_SPRING;
	} else if ( day < ( ICTIME_DAYS_PER_YEAR / 8 ) * 5 ) {
		season = WEATHER_SEASON_SUMMER;
	} else if ( day < ( ICTIME_DAYS_PER_YEAR / 8 ) * 7 ) {
		season = WEATHER_SEASON_AUTUMN;
	} else {
		season = WEATHER_SEASON_WINTER;
	}
	return season;
}

/**************************************************************************/
void update_daylight( char buf[SECT_MAX][MSL])
{
	int sector;

	++time_info.hour;

	for ( sector = 0; sector < SECT_MAX; sector++ )
	{
		switch ( time_info.hour )
		{
		case HOUR_DAY_BEGIN:
			weather_info[sector].sunlight = SUN_LIGHT;
			if(event_active == 5)
			{
				strcat(buf[sector], "The sky remains dark from the eclipse.\r\n");
				break;
			}
			strcat(buf[sector], "Light begins to spill through the misty night.\r\n");
			break;
		case HOUR_SUNRISE:
			weather_info[sector].sunlight = SUN_RISE;
			if(event_active == 5)
			{
				strcat(buf[sector], "The sky remains dark from the eclipse.\r\n");
				break;
			}
			strcat(buf[sector], "The sun rises in the east.\r\n");
			break;
		case HOUR_SUNSET:
			weather_info[sector].sunlight = SUN_SET;
			if(event_active == 5)
			{
				strcat(buf[sector], "The sky remains dark from the eclipse.\r\n");
				break;
			}
			strcat(buf[sector], "The darkness of night begins to descend.\r\n");
			break;
		case HOUR_NIGHT_BEGIN:
			weather_info[sector].sunlight = SUN_DARK;
			if(event_active == 5)
			{
				strcat(buf[sector], "The sky remains dark from the eclipse.\r\n");
				break;
			}
			strcat(buf[sector], "The day turns night and the dark mist descends choking out the stars.\r\n" );
			break;
		}
	}

	// Mage Mod stuff here, made seperate switch to keep it more localized and readable

	for ( sector = 0; sector < SECT_MAX; sector++ )
	{
		
		switch ( time_info.hour )
		{
		case 1:
			weather_info[sector].mage_castmod = get_magecastmod();
			break;
			
		case 2:
			weather_info[sector].mage_castmod = get_magecastmod()*3/4;
			break;
			
		case 3:
			weather_info[sector].mage_castmod = get_magecastmod()/2;
			break;
		
		case 4:
			weather_info[sector].mage_castmod = get_magecastmod()/4;
			break;
		
		case  5:
			weather_info[sector].mage_castmod =0;
			break;
			
		case 20:
			weather_info[sector].mage_castmod = get_magecastmod()/4;
			break;
			
		case 21:
			weather_info[sector].mage_castmod = get_magecastmod()/2;
			break;
		
		case 22:
			weather_info[sector].mage_castmod = get_magecastmod()*3/4;
			break;

		case 23:
			weather_info[sector].mage_castmod = get_magecastmod();
			break;
		}
	}

	if ( time_info.hour == HOUR_MIDNIGHT )
	{
		time_info.hour = 0;
		time_info.day++;
	}

	if ( time_info.day >= ICTIME_DAYS_PER_MONTH-1 )
	{
		time_info.day = 0;
		time_info.month++;
	}

	if ( time_info.month >= ICTIME_MONTHS_PER_YEAR-1 )
	{
		time_info.month = 0;
		time_info.year++;
	}
}

/**************************************************************************/
void weather_update(void)
{
	char buf[SECT_MAX][MSL];
	connection_data *d;
	int season;
	int diff;
	int sect;
	int sky;
	int i;
	bool changed = false;
	int dir = 0;

	if (++time_info.minute<60)		//  Does an update only once per IC hour
		return;

	time_info.minute=0;
	
	// NULLIFY it all
	for ( i = 0; i < SECT_MAX; i++ )
	{
		buf[i][0] = '\0';
	}
	
	update_daylight(buf);			// Increment hour, check if day increments
									// check month increment, check year increment
									// get mage cast mods, handle day night messages

	season = calculate_season();	// Season isn't fully supported yet, possible ideas:
									//		1. Seasons affecting day length
									//      2. Seasons affecting druid season spheres?
									//
									// Season currently affects weather influence, ie rainier, drier, sunnier etc
	

	int number_in_game=mobile_count+1000;
	// note: number_in_game is used to reduce potential loops 
	// with a mobprog that creates new mobs.

	// do hour triggers
	{
		char_data *ch;
		for ( ch = char_list; ch && --number_in_game>=0; ch = ch->next )
		{
			if ( IS_NPC( ch )
				&& HAS_TRIGGER( ch, TRIG_HOUR ))
				mp_hour_trigger( ch );
		}
	}

	// go through all the sector types, so now instead of 1 pass, we have like 13 :)
	// good thing we're not running this mud on a C64
	
	for( sect = 0; sect < SECT_MAX; sect++ )
	{
		int change_factor;
		changed = false;
		sky = weather_info[sect].sky;
		
		weather_info[sect].mmhg += weather_info[sect].change;
		
		// See what it's like at the moment

		change_factor = number_range(1,50);/////////////////////////////////////
		
		// change will be positive
		if(weather_info[sect].sky == 0)
		{
			if ( weather_info[sect].mmhg > master_weather_table[sect][season][weather_info[sect].sky])
			{
				sky = weather_sector_conversion[sect][weather_info[sect].sky + 1];
				changed = true;
				dir = 0;
			}
		}
		// change will be negative
		else if(weather_info[sect].sky == SKY_MAX - 1)
		{
			if(weather_info[sect].mmhg < master_weather_table[sect][season][weather_info[sect].sky - 1])
			{
				sky = weather_sector_conversion[sect][weather_info[sect].sky - 1];
				changed = true;
				dir = -1;
			}
		}
		else
		{
			if(weather_info[sect].mmhg > master_weather_table[sect][season][weather_info[sect].sky + 1])
			{
				sky = weather_sector_conversion[sect][weather_info[sect].sky + 1];
				changed = true;
			}
			else if(weather_info[sect].mmhg < master_weather_table[sect][season][weather_info[sect].sky - 1])
			{
				sky = weather_sector_conversion[sect][weather_info[sect].sky - 1];
				changed = true;
				dir = -1;
			}
		}

		// Sky changed, reset counters
		if(changed)
		{
			// Determine amount to change the weather
			if(sky == 0)
			{
				weather_info[sect].change = change_factor;
				
				// reset mmhg in the middle of the current sky factor
				// weather_info[sect].mmhg = master_weather_table[sect][season][sky] / 2;
				// uncomment above to lean towards more sunshine
				weather_info[sect].mmhg = 0;
			}
			else if(sky == SKY_MAX - 1) // SKY_LIGHTNING
			{
				weather_info[sect].change =  -1 * change_factor;

				// reset mmhg in the middle of the current sky factor
				
				weather_info[sect].mmhg = (master_weather_table[sect][season][sky] - 
					master_weather_table[sect][season][sky - 1]) / 2 + 
					master_weather_table[sect][season][sky];
			}
			else
			{
				diff = ((number_range(1,10)) < 5) ? -1 : 1;
				weather_info[sect].change =  diff * change_factor;
				
				// reset mmhg in the middle of the current sky factor
				weather_info[sect].mmhg = (master_weather_table[sect][season][sky] - 
					master_weather_table[sect][season][sky - 1]) / 2 + 
					master_weather_table[sect][season][sky + dir];
			}
		}


		// Generate a proper change message
		
		if(changed)
		{
			if( sky < weather_info[sect].sky)
			{
				strcat(buf[sect], weather_message[sect][sky * 2]);
			}
			else
			{
				strcat(buf[sect], weather_message[sect][sky * 2 + 1]);
			}
			weather_info[sect].sky = sky;
		}
	}

	for ( d = connection_list; d != NULL; d = d->next )
	{
		if (   d->connected_state == CON_PLAYING
			&& IS_OUTSIDE(d->character)
			&& IS_AWAKE(d->character)
			&& IS_IC(d->character)
			&& !d->editor
			&& buf[d->character->in_room->sector_type][0] != '\0' )
		{
			d->character->printf( "%s", buf[d->character->in_room->sector_type] );

	if(!IS_OOC(d->character) && weather_info[d->character->in_room->sector_type].sky >= 2 &&
        d->character->in_room->sector_type != SECT_AIR && d->character->in_room->sector_type != SECT_INSIDE &&
        d->character->in_room->sector_type != SECT_WATER_SWIM && d->character->in_room->sector_type != SECT_WATER_NOSWIM &&
        d->character->in_room->sector_type != SECT_CAVE && d->character->in_room->sector_type != SECT_UNDERWATER &&
        d->character->in_room->sector_type != SECT_SNOW && d->character->in_room->sector_type != SECT_LAVA &&
	d->character->in_room->sector_type != SECT_VEHICLE &&
        d->character->in_room->sector_type != SECT_ICE && !IS_SET(d->character->in_room->room_flags, ROOM_INDOORS))
	{
		obj_data *o;
		o = get_obj_list( d->character, "puddle", d->character->in_room->contents );
		if ( o == NULL )
		{
			o=create_object(get_obj_index(60002)); // Rain Puddle
			o->timer = 7;
	        	obj_to_room( o, d->character->in_room );
		}
	}

	if(!IS_OOC(d->character) && weather_info[d->character->in_room->sector_type].sky >= 2 && !IS_SET(d->character->in_room->room_flags, ROOM_INDOORS) &&
        (d->character->in_room->sector_type == SECT_SNOW || d->character->in_room->sector_type == SECT_ICE))
	{
		obj_data *o;
		o = get_obj_list( d->character, "snow", d->character->in_room->contents );
		if ( o == NULL )
		{
			o=create_object(get_obj_index(60003)); // Snow
			// No timer on snow in ice or snow areas.
	        	obj_to_room( o, d->character->in_room );
		}
	}


		}
	}

	return;
}
/**************************************************************************/
// testing code so you don't have to wait for 12 hours to pass so you can 
// see how something works in the dark/light
// - Kal, Aug 03
void do_setichour(char_data *ch, char *argument)
{
	if(IS_NULLSTR(argument) || !is_number(argument)){
		ch->println("syntax: setichour <0-23>");
		ch->wrapln("This command makes the IC time jump to a given hour... "
			"the command was written to aid in developing the code and can be "
			"happily ignored by all non developers.");
		return;
	}

	int i=atoi(argument);

	if(i<0 || i>23){
		ch->println("The numeric value must be between 0 and 23");
		return;
	}
	time_info.minute=59;
	time_info.hour=i-1;
	weather_update();

	ch->printlnf("The time is now: %d:%02d%s",
		(time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
		time_info.minute,
		time_info.hour >= 12 ? "pm" : "am");

}

/**************************************************************************/
void init_weather(void)
{
	int sect, seas;

	for (sect = 0; sect < SECT_MAX; sect++)
	{
		weather_info[sect].change = number_range(1,50);
		weather_info[sect].mmhg = 200; // Something arbitrary
		weather_info[sect].sky = SKY_CLOUDLESS;
		
		for(seas = 0; seas < 4; seas++)		// 4 = Amount of seasons!!!
		{
			int cur = 0;
			
			// Each value is an accumulation of the preceeding
			cur += UMAX(base_weather.sky_clear +
				season_influence_data[seas].sky_clear + 
				sector_weather_table[sect].sky_clear, 0);
			
			master_weather_table[sect][seas][SKY_CLOUDLESS] = cur;
			
			cur += UMAX(base_weather.sky_cloudy + 
				season_influence_data[seas].sky_cloudy + 
				sector_weather_table[sect].sky_cloudy, 0);
			
			master_weather_table[sect][seas][SKY_CLOUDY] = cur;
			
			cur += UMAX(base_weather.sky_rainy + 
				season_influence_data[seas].sky_rainy + 
				sector_weather_table[sect].sky_rainy, 0);
			
			master_weather_table[sect][seas][SKY_RAINING] = cur;
			
			cur += UMAX(base_weather.sky_lightning + 
				season_influence_data[seas].sky_lightning + 
				sector_weather_table[sect].sky_lightning, 0);
			
			master_weather_table[sect][seas][SKY_LIGHTNING] = cur;
		}
	}
	weather_update();
}
/**************************************************************************/
void angel_event (char_data *ch, char *argument)
{

	if (!IS_NPC(ch) && !IS_OOC(ch) && !IS_IMMORTAL(ch) && IS_EVIL(ch) &&
         IS_SET(ch->act,PLR_CAN_ADVANCE) && (ch->in_room->sector_type != SECT_CITY &&
  	 ch->in_room->sector_type != SECT_WATER_SWIM && ch->in_room->sector_type != SECT_UNDERWATER &&
	 ch->in_room->sector_type != SECT_AIR && !IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) )
	{ 
	    char_data *angel;
	    int chance, level, leveladj, bonus;

	    chance = number_range(1,100);
	    if(chance<18)
	    {
		leveladj = number_range(1,10);
		bonus = number_range(1,4);
		if(bonus == 1){
			level = ch->level - leveladj;
		}
		else { level = ch->level + leveladj; }
		if(bonus == 2)
			level = ch->level - 1;	

    		angel = create_mobile( get_mob_index(60011),0);
		angel->level = level;
		bonus = number_range(25,200);
		angel->max_hit = ch->max_hit + bonus;
		angel->hit = angel->max_hit;
	 	reset_char(angel);
    		char_to_room( angel, ch->in_room );
		act( "`#A `Wbright light`^ flares and $n steps sudddenly appears!`^", angel, NULL, NULL, TO_ROOM );
		update_pos(angel);
		char qbuf[MSL];
		sprintf (qbuf, "`mEVENT> Angel appears to %s in room %d`x\r\n", 
				ch->name, ch->in_room->vnum); 
		wiznet(qbuf,ch,NULL,WIZ_QUESTING,0,LEVEL_IMMORTAL);
		if(IS_EVIL(ch))
		{
			angel->set_pdelay(4);
			act( "$n gives a battle cry and attacks!!", angel, NULL, NULL, TO_ROOM );
			multi_hit( ch, angel, TYPE_UNDEFINED );
		}
	    }
 	}
	return;
}
/**************************************************************************/
void demon_event (char_data *ch, char *argument)
{

	if (!IS_NPC(ch) && !IS_OOC(ch) && !IS_IMMORTAL(ch) && IS_GOOD(ch) &&
         IS_SET(ch->act,PLR_CAN_ADVANCE) && (ch->in_room->sector_type != SECT_CITY &&
  	 ch->in_room->sector_type != SECT_WATER_SWIM && ch->in_room->sector_type != SECT_UNDERWATER &&
	 ch->in_room->sector_type != SECT_AIR && !IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) )
	{ 
	    char_data *demon;
	    int chance, level, leveladj, bonus;

	    chance = number_range(1,100);
	    if(chance<18)
	    {
		leveladj = number_range(1,10);
		bonus = number_range(1,4);
		if(bonus == 1){
			level = ch->level - leveladj;
		}
		else { level = ch->level + leveladj; }
		if(bonus == 2)
			level = ch->level - 1;	

    		demon = create_mobile( get_mob_index(60000),0);
		demon->level = level;
		bonus = number_range(25,200);
		demon->max_hit = ch->max_hit + bonus;
		demon->hit = demon->max_hit;
	 	reset_char(demon);
    		char_to_room( demon, ch->in_room );
		act( "`#A `Rflaming gate`^ appears and a $n steps out.`^", demon, NULL, NULL, TO_ROOM );
		update_pos(demon);
		char qbuf[MSL];
		sprintf (qbuf, "`mEVENT> Demon appears to %s in room %d`x\r\n", 
				ch->name, ch->in_room->vnum); 
		wiznet(qbuf,ch,NULL,WIZ_QUESTING,0,LEVEL_IMMORTAL);
		if(IS_GOOD(ch))
		{
			demon->set_pdelay(4);
			act( "$n snarls and attacks!!", demon, NULL, NULL, TO_ROOM );
			multi_hit( ch, demon, TYPE_UNDEFINED );
		}
	    }
 	}
	return;
}
/**************************************************************************/
void tulgard_army_event (void)
{
	char_data *guard;
	int l, loop;

	guard = create_mobile(get_mob_index(60002), 0); // Tulgard Captain
	char_to_room( guard, get_room_index(3007));

	loop = number_range(10, 20);
	for (l=0; l < loop; ++l)
	{
		guard = create_mobile( get_mob_index(60009), 0); // Tulgard Soldier
		char_to_room( guard, get_room_index(3007));
	} 

	MPROG_LIST  *prg;
	char_data *mob, *mob_next_in_room;
	for(mob=guard->in_room->people; mob; mob=mob_next_in_room)
	{
		mob_next_in_room=mob->next_in_room;
		if(!IS_NPC(mob))
			continue;
		for ( prg = mob->pIndexData->mprogs; prg; prg = prg->next )
		{
			if ( prg->trig_type != TRIG_ACT)
				continue;
			program_flow( prg, mob, guard, NULL, NULL);
		}
	}
	return;
}
/**************************************************************************/
void ant_event (void)
{
	char_data *guard;
	int l, loop, room;
	ROOM_INDEX_DATA *pRoom;

	loop = number_range(15, 30);
	for (l=0; l < loop; ++l) // Tulgard Ants
	{
		guard = create_mobile( get_mob_index(60010), 0); // Ants
		room = number_range(3000, 3118);
		pRoom = get_room_index(room);
		if(pRoom == NULL)
			continue;
		if(IS_SET(pRoom->room_flags, ROOM_OOC))
			continue;
		char_to_room( guard, pRoom);
	} 
	loop = number_range(15, 30);
	for (l=0; l < loop; ++l) // Throytl Ants
	{
		guard = create_mobile( get_mob_index(60010), 0); // Ants
		room = number_range(27000, 27106);
		pRoom = get_room_index(room);
		if(pRoom == NULL)
			continue;
		if(IS_SET(pRoom->room_flags, ROOM_OOC))
			continue;
		char_to_room( guard, pRoom);
	} 
	loop = number_range(10, 30);
	for (l=0; l < loop; ++l) // Menzo Ants
	{
		guard = create_mobile( get_mob_index(60010), 0); // Ants
		room = number_range(620, 695);
		pRoom = get_room_index(room);
		if(pRoom == NULL)
			continue;
		if(IS_SET(pRoom->room_flags, ROOM_OOC))
			continue;
		char_to_room( guard, pRoom);
	} 
	loop = number_range(10, 20);
	for (l=0; l < loop; ++l) // Solace Ants
	{
		guard = create_mobile( get_mob_index(60010), 0); // Ants
		room = number_range(21600, 21707);
		pRoom = get_room_index(room);
		if(pRoom == NULL)
			continue;
		if(IS_SET(pRoom->room_flags, ROOM_OOC))
			continue;
		char_to_room( guard, pRoom);
	} 
	return;
}
/**************************************************************************/
void goblin_event (void)
{
	char_data *guard;
	int l,loop;
	ROOM_INDEX_DATA *pRoom;

	loop = number_range(5, 10);
	for (l=0; l < loop; ++l) // Goblins
	{
		guard = create_mobile( get_mob_index(60012), 0);
		pRoom = get_room_index(55966);
		char_to_room( guard, pRoom);
	} 

	loop = number_range(5, 15);
	for (l=0; l < loop; ++l) // Goblins
	{
		guard = create_mobile( get_mob_index(60012), 0);
		pRoom = get_room_index(55703);
		char_to_room( guard, pRoom);
	} 

	loop = number_range(5, 15);
	for (l=0; l < loop; ++l) // Goblins
	{
		guard = create_mobile( get_mob_index(60012), 0);
		pRoom = get_room_index(55689);
		char_to_room( guard, pRoom);
	} 

	loop = number_range(5, 15);
	for (l=0; l < loop; ++l) // Goblins
	{
		guard = create_mobile( get_mob_index(60012), 0);
		pRoom = get_room_index(57065);
		char_to_room( guard, pRoom);
	} 

	loop = number_range(5, 15);
	for (l=0; l < loop; ++l) // Goblins
	{
		guard = create_mobile( get_mob_index(60012), 0);
		pRoom = get_room_index(58110);
		char_to_room( guard, pRoom);
	} 

	loop = number_range(5, 20);
	for (l=0; l < loop; ++l) // Goblins
	{
		guard = create_mobile( get_mob_index(60012), 0);
		pRoom = get_room_index(59001);
		char_to_room( guard, pRoom);
	} 

	loop = number_range(5, 20);
	for (l=0; l < loop; ++l) // Goblins
	{
		guard = create_mobile( get_mob_index(60012), 0);
		pRoom = get_room_index(58695);
		char_to_room( guard, pRoom);
	} 

	loop = number_range(5, 20);
	for (l=0; l < loop; ++l) // Goblins
	{
		guard = create_mobile( get_mob_index(60012), 0);
		pRoom = get_room_index(53060);
		char_to_room( guard, pRoom);
	} 
	return;
}
/**************************************************************************/
void event_activate( void )
{
	if(GAMESETTING5(GAMESET5_DEDICATED_OLC_BUILDING_MUD))
	{
		event_active = 0;
		event_time = current_time - (6 * ICTIME_IRLSECS_PER_HOUR);
		return;
	}
 
	if(event_active == 0 && current_time>event_time) // Check for possible event
	{
		int chance;
		int event = 0;
		chance = number_range(1,100);
		if(chance<15)
		{
		    	event = number_range(1,8);
		    	if(event == 1)
			{
				event_active = 1;
				event_time = current_time + (2 * ICTIME_IRLSECS_PER_HOUR);
				return;	
			}
		    	if(event == 2)
			{
				return;
				// tulgard_army_event();
				// event_active = 2;
				// event_time = current_time + (2 * ICTIME_IRLSECS_PER_HOUR);
				// return;
			}
		    	if(event == 3)
			{
				ant_event();
				event_active = 3;
				event_time = current_time + (1 * ICTIME_IRLSECS_PER_HOUR);
				return;
			}
		    	if(event == 4)
			{
				event_active = 4;
				event_time = current_time + (2 * ICTIME_IRLSECS_PER_HOUR);
				return;	
			}
		    	if(event == 5)
			{
				event_active = 5;
				event_time = current_time + (1 * ICTIME_IRLSECS_PER_HOUR);
				return;	
			}
		    	if(event == 6)
			{
				goblin_event();
				event_active = 6;
				event_time = current_time + (24 * ICTIME_IRLSECS_PER_HOUR);
				return;
			}
		    	if(event == 7)
			{
				event_active = 7;
				event_time = current_time + (2 * ICTIME_IRLSECS_PER_HOUR);
				return;	
			}
				   
		}
	}

	return;
}
/**************************************************************************/
void event_update (void)
{
	if(GAMESETTING5(GAMESET5_DEDICATED_OLC_BUILDING_MUD))
	{
		event_active = 0;
		event_time = current_time - (6 * ICTIME_IRLSECS_PER_HOUR);
		return;
	}
 
	if(event_active == 0)
		return;

	if(current_time>event_time && event_active <= 10)
	{
		event_active = 0;
		event_time = current_time + (6 * ICTIME_IRLSECS_PER_HOUR);
		return;
	}
	return;
}
/**************************************************************************/
void event_check (char_data *ch, char *argument)
{
	if(event_active == 1)
		demon_event(ch,"");
	if(event_active == 4)
		angel_event(ch,"");
	return;
}
/**************************************************************************/
void do_event (char_data *ch, char *argument)
{
    char 	arg1[MIL];
    sh_int	event;

    argument = one_argument( argument, arg1 );

	if(GAMESETTING5(GAMESET5_DEDICATED_OLC_BUILDING_MUD))
	{
		event_active = 0;
		event_time = current_time - (6 * ICTIME_IRLSECS_PER_HOUR);
		do_huh(ch, "");
		return;
	}

    if ( arg1[0] == '\0' )
    {

    	if(event_active == 0)
		ch->printlnf("No event active.");
    	if(event_active == 1)
		ch->printlnf("Demon Invasion Active.");
   	 if(event_active == 2)
		ch->printlnf("Tulgard Army Active.");
   	 if(event_active == 3)
		ch->printlnf("Ant Invasion Active.");
    	if(event_active == 4)
		ch->printlnf("Angel Invasion Active.");
    	if(event_active == 5)
		ch->printlnf("Solar Eclipse Active.");
    	if(event_active == 6)
		ch->printlnf("Goblin Hordes Active.");
   	 if(event_active == 7)
		ch->printlnf("Magical Flux Active.");
	ch->println( "`BSyntax: `xevent <number>" );
	ch->println( "`y1-Demon Invasion 2-Tulgard Army 3-Ant Invastion 4-Angel Invasion" );
	ch->println( "`y5-Eclipse 6-Goblin Hordes 7-Magical Flux`x" );
	return;
    }

    if ( is_number( arg1 ) )
    {
	event = atoi( arg1);
    	if(event == 0)
	{
		event_active = 0;
		ch->println("Events Deactivated.");
		event_time = current_time;
		return;	
	}
    	if(event == 1)
	{
		event_active = 1;
		event_time = current_time + (4 * ICTIME_IRLSECS_PER_HOUR);
		ch->println("Demon Invasion Activated.");
		return;	
	}
    	if(event == 2)
	{
		event_active = 2;
		event_time = current_time + (6 * ICTIME_IRLSECS_PER_HOUR);
		ch->println("Tulgard Army not active at this time.");
		//tulgard_army_event();
		return;
	}
    	if(event == 3)
	{
		event_active = 3;
		event_time = current_time + (5 * ICTIME_IRLSECS_PER_HOUR);
		ch->println("Ant Invasion Activated.");
		ant_event();
		return;
	}
    	if(event == 4)
	{
		event_active = 4;
		event_time = current_time + (4 * ICTIME_IRLSECS_PER_HOUR);
		ch->println("Angel Invasion Activated.");
		return;	
	}
    	if(event == 5)
	{
		event_active = 5;
		event_time = current_time + (1 * ICTIME_IRLSECS_PER_HOUR);

		connection_data *d;
		for ( d = connection_list; d != NULL; d = d->next )
		{
			if ( d->connected_state == CON_PLAYING)
			{
				d->character->printlnf("A solar eclipse blackens the world, turning day into night.");
			}
		}

 		ch->printlnf("Eclipse Activated.");
		return;	
	}
    	if(event == 6)
	{
		event_active = 6;
		event_time = current_time + (6 * ICTIME_IRLSECS_PER_HOUR);
		ch->println("Goblin Hoardes Activated.");
		goblin_event();
		return;
	}
    	if(event == 7)
	{
		event_active = 7;
		event_time = current_time + (2 * ICTIME_IRLSECS_PER_HOUR);
		ch->println("Magical Flux Activated.");
		return;
	}

    }
    ch->println("Illegal event number.");
    return;
}
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
