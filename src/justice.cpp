/**************************************************************************/
// justice.cpp - Justice code
/***************************************************************************
 * Whispers of Times Lost (c)2001-2005 Brad Wilson                         *
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
#include "tables.h"
#include "lookup.h"
#include "gameset.h"
#include "ictime.h"

// local functions
void judgement args( (char_data *ch, char *argument, int number, int mod));

DECLARE_DO_FUN(do_look);
DECLARE_DO_FUN(kill_char);
DECLARE_DO_FUN(do_startprison);
/**************************************************************************/
void do_bharrest(char_data *ch, char *argument)
{
    char_data *victim;


    char arg1 [MSL]; 
    char arg2 [MSL];
    char buf[MSL];
    buf[0] = '\0';
    int bounty;

    argument = one_argument( argument, arg1 );  /* Combine the arguments */
    strcpy( arg2, argument );
    
    if(IS_NULLSTR(arg1)){
		ch->println("Syntax:");
		ch->println("bhrrest <charname>");
		return; 
	}
    if((victim = get_char_room(ch,arg1)) == NULL){ 
		ch->printlnf("There is no player called '%s' here.", arg1);
		return; 
	}
    if(!IS_ACTIVE(ch)){
		ch->println("You must be ACTIVE to arrest and be a bounty hunter.");
		return; 
	}
    if(!IS_SET(ch->affected_by2, AFF2_BOUNTYHUNTER)){
		ch->println("Your not a bounty hunter. Go get a license if you want to arrest.");
		return; 
	}
    if(ch->pcdata->theft > 5 && ch->pcdata->murder == 0) {
		ch->println("Your license has been revoked for your acts of robbery.");
		REMOVE_BIT(ch->affected_by2, AFF2_BOUNTYHUNTER);
		return;
    }
    if(ch->pcdata->theft == 0 && ch->pcdata->murder > 2) {
		ch->println("Your license has been revoked for your murderous acts.");
		REMOVE_BIT(ch->affected_by2, AFF2_BOUNTYHUNTER);
		return;
    }
    if(ch->pcdata->theft > 1 && ch->pcdata->murder > 1) {
		ch->println("Your license has been revoked for your acts of robbery & murder.");
		REMOVE_BIT(ch->affected_by2, AFF2_BOUNTYHUNTER);
		return;
    }
    if(IS_NPC(victim)){
		ch->println( "You can only arrest players." );
		return;
	}
    if (!can_see( ch, victim )){ 
		ch->printlnf("There is no player called '%s' here.", arg1);
		return; 
	}
     if(victim->pcdata->theft == 0 && victim->pcdata->murder == 0)
	{
		ch->printlnf("%s is not wanted for questioning.", victim->name);
		return;
	}

    if(IS_IMMORTAL(victim)){
		ch->println( "You wish." );
		return;
	}

	if ((ch->level<6) || (!IS_SET(ch->act,PLR_CAN_ADVANCE)))
	{
		ch->println("Sorry but you are not experienced enough or letgained.");
		return; 
	}

	if ((victim->level<6) || (!IS_SET(victim->act,PLR_CAN_ADVANCE)))
	{
		ch->printlnf("Sorry but %s is not experienced enough or letgained.", victim->name);
		return; 
	}

    if(IS_NULLSTR(arg2)){
		ch->printlnf("`#To arrest type '`CBHARREST `Y%s CONFIRM`^'", victim->name);

		if(victim->pcdata->theft > 5 && victim->pcdata->murder == 0)
			ch->printlnf("%s is wanted for `#`RROBBERY.`^", victim->name);
		
		if(victim->pcdata->theft == 0 && victim->pcdata->murder > 2)
			ch->printlnf("%s is wanted for `#`RMURDER.`^", victim->name);

		if(victim->pcdata->theft > 1 && victim->pcdata->murder > 1)
			ch->printlnf("%s is wanted for `#`RROBBERY & MURDER.`Y", victim->name);

		bounty= (victim->pcdata->theft + victim->pcdata->murder) * 50;
		
		if (bounty > 500)
			bounty = 500;

		ch->printlnf("`#`YThe bounty of %s is %d gold.`^", victim->name, bounty);
			  		
		return; 
	}

	if( !str_prefix( arg2, "confirm")){
	ch->printlnf("`#`YYou attempt to arrest and subdue `C%s`Y.`^", victim->name);
    	victim->printlnf("`#`Y%s is placing you `RUNDER ARREST`^.", ch->name );
	victim->println("Type SURRENDER to submit to arrest and cease fighting.");
	SET_BIT(victim->act2, ACT2_UNDERARREST);
	multi_hit( ch, victim, TYPE_UNDEFINED );
    	return;
	}

	ch->println("Huh ?");
	return;
}
/**************************************************************************/
void do_accuse(char_data *ch, char *argument)
{
   	char_data *victim;
   	char arg1 [MSL]; 
   	TERRITORY_DATA *terr;

   	argument = one_argument( argument, arg1 );  /* Combine the arguments */
    
	if(IS_NPC(ch)) {
		do_huh(ch, "");
		return;
	}

  	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_1)) 
  	{
		terr = territory_number(1); }
  	else if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_2)) 
  	{
		terr = territory_number(2); }
  	else if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_3)) 
  	{
		terr = territory_number(3); }
  	else if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_4)) 
  	{
		terr = territory_number(4); }
  	else if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_5)) 
  	{
		terr = territory_number(5); }
  	else if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_6)) 
  	{
		terr = territory_number(6); }
  	else if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_7)) 
  	{
		terr = territory_number(7); }
  	else if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_8)) 
  	{
		terr = territory_number(8); }
  	else if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_9)) 
  	{
		terr = territory_number(9); }
  	else if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_10)) 
  	{
		terr = territory_number(10); }
  	else 
  	{
		ch->println("You cannot do that here.");
		return;
  	}

   	if(IS_NULLSTR(arg1)){
		ch->println("Syntax: accuse <name> <crime>");
		ch->printlnf("`#`SCrimes Available: `W[`Y%s`W]", flag_string(crime_flags, terr->crimeflags));
		return; 
	}
   	if((victim = get_char_room(ch,arg1)) == NULL){ 
		ch->printlnf("There is no player called '%s' here.", arg1);
		return; 
	}
   	if(IS_NPC(victim)){
		ch->println( "You can only accuse players of crimes." );
		return;
	}
   	if (!can_see( ch, victim )){ 
		ch->printlnf("There is no player called '%s' here.", arg1);
		return; 
	}
	if ((ch->level<10) || (!IS_SET(ch->act,PLR_CAN_ADVANCE)))
	{
		ch->println("Sorry but you are not experienced enough or letgained.");
		return; 
	}
	if(!terr->crimeflags)
	{
		ch->println("This area has no laws to break!");
		return; 
	}

    if(IS_IMMORTAL(victim)){
		ch->println( "You wish." );
		return;
	}
   	if( !str_prefix( argument, "murder") && IS_SET(terr->crimeflags, CRIME_MURDER))
	{
		argument = "murder";
       		act("$N has accused $n of murder.", victim, NULL, ch, TO_ALL);
		judge_broadcast(ch, "%s has been accused of %s by %s in %s.`^", victim->name, argument, ch->name, terr->name);   		return;
	}
   	if( !str_prefix( argument, "theft") && IS_SET(terr->crimeflags, CRIME_THEFT))
	{
		argument = "theft";
       		act("$N has accused $n of theft.", victim, NULL, ch, TO_ALL);
		judge_broadcast(ch, "%s has been accused of %s by %s in %s.`^", victim->name, argument, ch->name, terr->name);   		return;
	}
   	if( !str_prefix( argument, "weapons") && IS_SET(terr->crimeflags, CRIME_WEAPONS))
	{	
		argument = "weapons";
       		act("$N has accused $n of brandishing weapons.", victim, NULL, ch, TO_ALL);
		judge_broadcast(ch, "%s has been accused of %s by %s in %s.`^", victim->name, argument, ch->name, terr->name);   		return;
	}
   	if( !str_prefix( argument, "assault") && IS_SET(terr->crimeflags, CRIME_ASSAULT)) 
	{
		argument = "assault";
       		act("$N has accused $n of assault.", victim, NULL, ch, TO_ALL);
		judge_broadcast(ch, "%s has been accused of %s by %s in %s.`^", victim->name, argument, ch->name, terr->name);   		return;
	}
   	if( !str_prefix( argument, "extortion") && IS_SET(terr->crimeflags, CRIME_EXTORTION))
	{
		argument = "extortion";
       		act("$N has accused $n of extortion.", victim, NULL, ch, TO_ALL);
		judge_broadcast(ch, "%s has been accused of %s by %s in %s.`^", victim->name, argument, ch->name, terr->name);   		return;
	}
   	if( !str_prefix( argument, "treason") && IS_SET(terr->crimeflags, CRIME_TREASON))
	{
		argument = "high treason";
       		act("$N has accused $n of high treason.", victim, NULL, ch, TO_ALL);
		judge_broadcast(ch, "%s has been accused of %s by %s in %s.`^", victim->name, argument, ch->name, terr->name);   		return;
	}
   	if( !str_prefix( argument, "bad_magic") && IS_SET(terr->crimeflags, CRIME_BADMAGIC))
	{
		argument = "improper use of magic on another";
       		act("$N has accused $n of improper magic use on another.", victim, NULL, ch, TO_ALL);
		judge_broadcast(ch, "%s has been accused of %s by %s in %s.`^", victim->name, argument, ch->name, terr->name);   		return;
	}
   	if( !str_prefix( argument, "drunk") && IS_SET(terr->crimeflags, CRIME_DRUNK))
	{
		argument = "public drunkeness";
       		act("$N has accused $n of public drunkeness.", victim, NULL, ch, TO_ALL);
		judge_broadcast(ch, "%s has been accused of %s by %s in %s.`^", victim->name, argument, ch->name, terr->name);   		return;
	}
   	if( !str_prefix( argument, "bribery") && IS_SET(terr->crimeflags, CRIME_BRIBE))
	{
		argument = "bribery";
       		act("$N has accused $n of bribery.", victim, NULL, ch, TO_ALL);
		judge_broadcast(ch, "%s has been accused of %s by %s in %s.`^", victim->name, argument, ch->name, terr->name);   		return;
	}
  	if( !str_prefix( argument, "impersonation") && IS_SET(terr->crimeflags, CRIME_IMPERSONATE))
	{
		argument = "impersonation of another";
       		act("$N has accused $n of impersinating another.", victim, NULL, ch, TO_ALL);
		judge_broadcast(ch, "%s has been accused of %s by %s in %s.`^", victim->name, argument, ch->name, terr->name);   		return;
	}
   	if( !str_prefix( argument, "werewolf") && IS_SET(terr->crimeflags, CRIME_WEREWOLF))
	{
		argument = "being a werewolf";
       		act("$N has accused $n of being a werewolf.", victim, NULL, ch, TO_ALL);
		judge_broadcast(ch, "%s has been accused of %s by %s in %s.`^", victim->name, argument, ch->name, terr->name);   		return;
	}
   	if( !str_prefix( argument, "vampirism") && IS_SET(terr->crimeflags, CRIME_VAMPIRE)) 
	{
		argument = "being a vampire";
       		act("$N has accused $n of being a vampire.", victim, NULL, ch, TO_ALL);
		judge_broadcast(ch, "%s has been accused of %s by %s in %s.`^", victim->name, argument, ch->name, terr->name);   		return;
	}
   	if( !str_prefix( argument, "disease") && IS_SET(terr->crimeflags, CRIME_DISEASE)) 
	{
		argument = "spreading diseases";
       		act("$N has accused $n of spreading diseases.", victim, NULL, ch, TO_ALL);
		judge_broadcast(ch, "%s has been accused of %s by %s in %s.`^", victim->name, argument, ch->name, terr->name);   		return;
	}
   	if( !str_prefix( argument, "religion") && IS_SET(terr->crimeflags, CRIME_RELIGION)) 
	{
		argument = "practicing outlawed religion";
       		act("$N has accused $n of practicing outlawed religion.", victim, NULL, ch, TO_ALL);
		judge_broadcast(ch, "%s has been accused of %s by %s in %s.`^", victim->name, argument, ch->name, terr->name);   		return;
	}
   	if( !str_prefix( argument, "threats") && IS_SET(terr->crimeflags, CRIME_THREATS)) 
	{
		argument = "making threats against others";
       		act("$N has accused $n of making threats against others.", victim, NULL, ch, TO_ALL);
		judge_broadcast(ch, "%s has been accused of %s by %s in %s.`^", victim->name, argument, ch->name, terr->name);   		return;
	}
   	if( !str_prefix( argument, "disturb_peace") && IS_SET(terr->crimeflags, CRIME_DISTURB_PEACE)) 
	{
		argument = "disturbing the peace";
       		act("$N has accused $n of disturbing the peace.", victim, NULL, ch, TO_ALL);
		judge_broadcast(ch, "%s has been accused of %s by %s in %s.`^", victim->name, argument, ch->name, terr->name);   		return;
	}
   	if( !str_prefix( argument, "blackmail") && IS_SET(terr->crimeflags, CRIME_BLACKMAIL)) 
	{
		argument = "blackmail";
       		act("$N has accused $n of blackmail.", victim, NULL, ch, TO_ALL);
		judge_broadcast(ch, "%s has been accused of %s by %s in %s.`^", victim->name, argument, ch->name, terr->name);   		return;
	}
   	if( !str_prefix( argument, "evade_law") && IS_SET(terr->crimeflags, CRIME_EVADING_LAW)) 
	{
		argument = "evading the law";
       		act("$N has accused $n of evading the law.", victim, NULL, ch, TO_ALL);
		judge_broadcast(ch, "%s has been accused of %s by %s in %s.`^", victim->name, argument, ch->name, terr->name);   		return;
	}
   	if( !str_prefix( argument, "necromancy") && IS_SET(terr->crimeflags, CRIME_NECROMANCY)) 
	{
		argument = "necromancy";
       		act("$N has accused $n of necromancy.", victim, NULL, ch, TO_ALL);
		judge_broadcast(ch, "%s has been accused of %s by %s in %s.`^", victim->name, argument, ch->name, terr->name);   		return;
	}
   	if( !str_prefix( argument, "dragon") && IS_SET(terr->crimeflags, CRIME_DRAGON)) 
	{
		argument = "being in full dragon form";
       		act("$N has accused $n of being in full dragon form.", victim, NULL, ch, TO_ALL);
		judge_broadcast(ch, "%s has been accused of %s by %s in %s.`^", victim->name, argument, ch->name, terr->name);   		return;
	}
   	if( !str_prefix( argument, "undead") && IS_SET(terr->crimeflags, CRIME_UNDEAD)) 
	{
		argument = "undead acts";
       		act("$N has accused $n of undead acts.", victim, NULL, ch, TO_ALL);
		judge_broadcast(ch, "%s has been accused of %s by %s in %s.`^", victim->name, argument, ch->name, terr->name);   		return;
	}

	ch->println("That is not a crime worthy of punishment.");
	ch->println("Syntax: accuse <name> <crime>");
	ch->printlnf("`#`SCrimes Available: `W[`Y%s`W]", flag_string(crime_flags, terr->crimeflags));
	return;
}
/**************************************************************************/
void do_arrest(char_data *ch, char *argument)
{
   	char_data *victim;
   	char arg1 [MSL], arg2 [MSL]; 
   	TERRITORY_DATA *terr;

     	argument = one_argument(argument, arg1);
     	argument = one_argument(argument, arg2);
    
	if(IS_NPC(ch)) 
	{
		do_huh(ch, "");
		return;
	}

    	if(!IS_IMMORTAL(ch) && !IS_SET(ch->territory1, TERRITORY_GUARD))
    	{
		do_huh(ch, "");
		return;
    	}

	terr = territory_number(11);

 	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_1))
		terr = territory_number(1);
	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_2))
		terr = territory_number(2);
	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_3))
		terr = territory_number(3);
	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_4))
		terr = territory_number(4);
	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_5))
		terr = territory_number(5);
	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_6))
		terr = territory_number(6);
	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_7))
		terr = territory_number(7);
	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_8))
		terr = territory_number(8);
	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_9))
		terr = territory_number(9);
	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_10))
		terr = territory_number(10);

	if(!terr)
	{
		ch->printlnf("You cannot do that here.");
		return; 
	}
   	if(IS_NULLSTR(arg1)){
		ch->println("Syntax: arrest <name> <crime>");
		ch->printlnf("`#`SCrimes Available: `W[`Y%s`W]", flag_string(crime_flags, terr->crimeflags));
		return; 
	}

   	if(IS_NULLSTR(arg2)){
		ch->println("Syntax: arrest <name> <crime>");
		ch->printlnf("`#`SCrimes Available: `W[`Y%s`W]", flag_string(crime_flags, terr->crimeflags));
		return; 
	}
   	if((victim = get_char_room(ch,arg1)) == NULL){ 
		ch->printlnf("There is no player called '%s' here.", arg1);
		return; 
	}
   	if(IS_NPC(victim)){
		ch->println( "You can only arrest players of crimes." );
		return;
	}
   	if (!can_see( ch, victim )){ 
		ch->printlnf("There is no player called '%s' here.", arg1);
		return; 
	}

	if(!terr->crimeflags)
	{
		ch->println("This area has no laws to break!");
		return; 
	}

    	if(IS_IMMORTAL(victim)){
		ch->println( "You wish." );
		return;
	}

   	if( !str_prefix( arg2, "murder") && IS_SET(terr->crimeflags, CRIME_MURDER))
	{
		argument = "murder";
       		act("$N has arrested $n for murder.", victim, NULL, ch, TO_ALL);
		if(terr->number == 1)
			judgement(victim, "murder", 1, -3);
		if(terr->number == 2)
			judgement(victim, "murder", 2, -3);
		if(terr->number == 3)
			judgement(victim, "murder", 3, -3);
		if(terr->number == 4)
			judgement(victim, "murder", 4, -3);
		if(terr->number == 5)
			judgement(victim, "murder", 5, -3);
		if(terr->number == 6)
			judgement(victim, "murder", 6, -3);
		if(terr->number == 7)
			judgement(victim, "murder", 7, -3);
		if(terr->number == 8)
			judgement(victim, "murder", 8, -3);
		if(terr->number == 9)
			judgement(victim, "murder", 9, -3);
		if(terr->number == 10)
			judgement(victim, "murder", 10, -3);
   		return;
	}
   	if( !str_prefix( arg2, "theft") && IS_SET(terr->crimeflags, CRIME_THEFT))
	{
		argument = "theft";
       		act("$N has arrested $n for theft.", victim, NULL, ch, TO_ALL);
		if(terr->number == 1)
			judgement(victim, "theft", 1, -2);
		if(terr->number == 2)
			judgement(victim, "theft", 2, -2);
		if(terr->number == 3)
			judgement(victim, "theft", 3, -2);
		if(terr->number == 4)
			judgement(victim, "theft", 4, -2);
		if(terr->number == 5)
			judgement(victim, "theft", 5, -2);
		if(terr->number == 6)
			judgement(victim, "theft", 6, -2);
		if(terr->number == 7)
			judgement(victim, "theft", 7, -2);
		if(terr->number == 8)
			judgement(victim, "theft", 8, -2);
		if(terr->number == 9)
			judgement(victim, "theft", 9, -2);
		if(terr->number == 10)
			judgement(victim, "theft", 10, -2);
   		return;
	}
   	if( !str_prefix( arg2, "weapons") && IS_SET(terr->crimeflags, CRIME_WEAPONS))
	{	
		argument = "weapons";
       		act("$N has arrested $n for brandishing weapons.", victim, NULL, ch, TO_ALL);
		if(terr->number == 1)
			judgement(victim, "brandishing weapons", 1, 1);
		if(terr->number == 2)
			judgement(victim, "brandishing weapons", 2, 1);
		if(terr->number == 3)
			judgement(victim, "brandishing weapons", 3, 1);
		if(terr->number == 4)
			judgement(victim, "brandishing weapons", 4, 1);
		if(terr->number == 5)
			judgement(victim, "brandishing weapons", 5, 1);
		if(terr->number == 6)
			judgement(victim, "brandishing weapons", 6, 1);
		if(terr->number == 7)
			judgement(victim, "brandishing weapons", 7, 1);
		if(terr->number == 8)
			judgement(victim, "brandishing weapons", 8, 1);
		if(terr->number == 9)
			judgement(victim, "brandishing weapons", 9, 1);
		if(terr->number == 10)
			judgement(victim, "brandishing weapons", 10, 1);
   		return;
	}
   	if( !str_prefix( arg2, "assault") && IS_SET(terr->crimeflags, CRIME_ASSAULT)) 
	{
		argument = "assault";
       		act("$N has arrested $n for assault.", victim, NULL, ch, TO_ALL);
		if(terr->number == 1)
			judgement(victim, "assault", 1, -2);
		if(terr->number == 2)
			judgement(victim, "assault", 2, -2);
		if(terr->number == 3)
			judgement(victim, "assault", 3, -2);
		if(terr->number == 4)
			judgement(victim, "assault", 4, -2);
		if(terr->number == 5)
			judgement(victim, "assault", 5, -2);
		if(terr->number == 6)
			judgement(victim, "assault", 6, -2);
		if(terr->number == 7)
			judgement(victim, "assault", 7, -2);
		if(terr->number == 8)
			judgement(victim, "assault", 8, -2);
		if(terr->number == 9)
			judgement(victim, "assault", 9, -2);
		if(terr->number == 10)
			judgement(victim, "assault", 10, -2);
   		return;
	}
   	if( !str_prefix( arg2, "extortion") && IS_SET(terr->crimeflags, CRIME_EXTORTION))
	{
		argument = "extortion";
       		act("$N has arrested $n for extortion.", victim, NULL, ch, TO_ALL);
		if(terr->number == 1)
			judgement(victim, "extortion", 1, -1);
		if(terr->number == 2)
			judgement(victim, "extortion", 2, -1);
		if(terr->number == 3)
			judgement(victim, "extortion", 3, -1);
		if(terr->number == 4)
			judgement(victim, "extortion", 4, -1);
		if(terr->number == 5)
			judgement(victim, "extortion", 5, -1);
		if(terr->number == 6)
			judgement(victim, "extortion", 6, -1);
		if(terr->number == 7)
			judgement(victim, "extortion", 7, -1);
		if(terr->number == 8)
			judgement(victim, "extortion", 8, -1);
		if(terr->number == 9)
			judgement(victim, "extortion", 9, -1);
		if(terr->number == 10)
			judgement(victim, "extortion", 10, -1);
   		return;
	}
   	if( !str_prefix( arg2, "treason") && IS_SET(terr->crimeflags, CRIME_TREASON))
	{
		argument = "high treason";
       		act("$N has arrested $n for high treason.", victim, NULL, ch, TO_ALL);
		if(terr->number == 1)
			judgement(victim, "treason", 1, -2);
		if(terr->number == 2)
			judgement(victim, "treason", 2, -2);
		if(terr->number == 3)
			judgement(victim, "treason", 3, -2);
		if(terr->number == 4)
			judgement(victim, "treason", 4, -2);
		if(terr->number == 5)
			judgement(victim, "treason", 5, -2);
		if(terr->number == 6)
			judgement(victim, "treason", 6, -2);
		if(terr->number == 7)
			judgement(victim, "treason", 7, -2);
		if(terr->number == 8)
			judgement(victim, "treason", 8, -2);
		if(terr->number == 9)
			judgement(victim, "treason", 9, -2);
		if(terr->number == 10)
			judgement(victim, "treason", 10, -2);
   		return;
	}
   	if( !str_prefix( arg2, "bad_magic") && IS_SET(terr->crimeflags, CRIME_BADMAGIC))
	{
		argument = "improper use of magic on another";
       		act("$N has arrested $n for improper magic use on another.", victim, NULL, ch, TO_ALL);
		if(terr->number == 1)
			judgement(victim, "illegal use of magic", 1, 0);
		if(terr->number == 2)
			judgement(victim, "illegal use of magic", 2, 0);
		if(terr->number == 3)
			judgement(victim, "illegal use of magic", 3, 0);
		if(terr->number == 4)
			judgement(victim, "illegal use of magic", 4, 0);
		if(terr->number == 5)
			judgement(victim, "illegal use of magic", 5, 0);
		if(terr->number == 6)
			judgement(victim, "illegal use of magic", 6, 0);
		if(terr->number == 7)
			judgement(victim, "illegal use of magic", 7, 0);
		if(terr->number == 8)
			judgement(victim, "illegal use of magic", 8, 0);
		if(terr->number == 9)
			judgement(victim, "illegal use of magic", 9, 0);
		if(terr->number == 10)
			judgement(victim, "illegal use of magic", 10, 0);
   		return;
	}
   	if( !str_prefix( arg2, "drunk") && IS_SET(terr->crimeflags, CRIME_DRUNK))
	{
		argument = "public drunkeness";
       		act("$N has arrested $n for public drunkeness.", victim, NULL, ch, TO_ALL);
		if(terr->number == 1)
			judgement(victim, "public drunkeness", 1, 2);
		if(terr->number == 2)
			judgement(victim, "public drunkeness", 2, 2);
		if(terr->number == 3)
			judgement(victim, "public drunkeness", 3, 2);
		if(terr->number == 4)
			judgement(victim, "public drunkeness", 4, 2);
		if(terr->number == 5)
			judgement(victim, "public drunkeness", 5, 2);
		if(terr->number == 6)
			judgement(victim, "public drunkeness", 6, 2);
		if(terr->number == 7)
			judgement(victim, "public drunkeness", 7, 2);
		if(terr->number == 8)
			judgement(victim, "public drunkeness", 8, 2);
		if(terr->number == 9)
			judgement(victim, "public drunkeness", 9, 2);
		if(terr->number == 10)
			judgement(victim, "public drunkeness", 10, 2);
   		return;
	}
   	if( !str_prefix( arg2, "bribery") && IS_SET(terr->crimeflags, CRIME_BRIBE))
	{
		argument = "bribery";
       		act("$N has arrested $n for bribery.", victim, NULL, ch, TO_ALL);
		if(terr->number == 1)
			judgement(victim, "bribery", 1, 1);
		if(terr->number == 2)
			judgement(victim, "bribery", 2, 1);
		if(terr->number == 3)
			judgement(victim, "bribery", 3, 1);
		if(terr->number == 4)
			judgement(victim, "bribery", 4, 1);
		if(terr->number == 5)
			judgement(victim, "bribery", 5, 1);
		if(terr->number == 6)
			judgement(victim, "bribery", 6, 1);
		if(terr->number == 7)
			judgement(victim, "bribery", 7, 1);
		if(terr->number == 8)
			judgement(victim, "bribery", 8, 1);
		if(terr->number == 9)
			judgement(victim, "bribery", 9, 1);
		if(terr->number == 10)
			judgement(victim, "bribery", 10, 1);
   		return;
	}
  	if( !str_prefix( arg2, "impersonation") && IS_SET(terr->crimeflags, CRIME_IMPERSONATE))
	{
		argument = "impersonation of another";
       		act("$N has arrested $n for impersinating another.", victim, NULL, ch, TO_ALL);
		if(terr->number == 1)
			judgement(victim, "impersonation of another", 1, 0);
		if(terr->number == 2)
			judgement(victim, "impersonation of another", 2, 0);
		if(terr->number == 3)
			judgement(victim, "impersonation of another", 3, 0);
		if(terr->number == 4)
			judgement(victim, "impersonation of another", 4, 0);
		if(terr->number == 5)
			judgement(victim, "impersonation of another", 5, 0);
		if(terr->number == 6)
			judgement(victim, "impersonation of another", 6, 0);
		if(terr->number == 7)
			judgement(victim, "impersonation of another", 7, 0);
		if(terr->number == 8)
			judgement(victim, "impersonation of another", 8, 0);
		if(terr->number == 9)
			judgement(victim, "impersonation of another", 9, 0);
		if(terr->number == 10)
			judgement(victim, "impersonation of another", 10, 0);
   		return;
	}
   	if( !str_prefix( arg2, "werewolf") && IS_SET(terr->crimeflags, CRIME_WEREWOLF))
	{
		argument = "being a werewolf";
       		act("$N has arrested $n for being a werewolf.", victim, NULL, ch, TO_ALL);
		if(terr->number == 1)
			judgement(victim, "being a lycanthrope", 1, -1);
		if(terr->number == 2)
			judgement(victim, "being a lycanthrope", 2, -1);
		if(terr->number == 3)
			judgement(victim, "being a lycanthrope", 3, -1);
		if(terr->number == 4)
			judgement(victim, "being a lycanthrope", 4, -1);
		if(terr->number == 5)
			judgement(victim, "being a lycanthrope", 5, -1);
		if(terr->number == 6)
			judgement(victim, "being a lycanthrope", 6, -1);
		if(terr->number == 7)
			judgement(victim, "being a lycanthrope", 7, -1);
		if(terr->number == 8)
			judgement(victim, "being a lycanthrope", 8, -1);
		if(terr->number == 9)
			judgement(victim, "being a lycanthrope", 9, -1);
		if(terr->number == 10)
			judgement(victim, "being a lycanthrope", 10, -1);
   		return;
	}
   	if( !str_prefix( arg2, "vampirism") && IS_SET(terr->crimeflags, CRIME_VAMPIRE)) 
	{
		argument = "being a vampire";
       		act("$N has arrested $n for being a vampire.", victim, NULL, ch, TO_ALL);
		if(terr->number == 1)
			judgement(victim, "being a vampire", 1, -1);
		if(terr->number == 2)
			judgement(victim, "being a vampire", 2, -1);
		if(terr->number == 3)
			judgement(victim, "being a vampire", 3, -1);
		if(terr->number == 4)
			judgement(victim, "being a vampire", 4, -1);
		if(terr->number == 5)
			judgement(victim, "being a vampire", 5, -1);
		if(terr->number == 6)
			judgement(victim, "being a vampire", 6, -1);
		if(terr->number == 7)
			judgement(victim, "being a vampire", 7, -1);
		if(terr->number == 8)
			judgement(victim, "being a vampire", 8, -1);
		if(terr->number == 9)
			judgement(victim, "being a vampire", 9, -1);
		if(terr->number == 10)
			judgement(victim, "being a vampire", 10, -1);
   		return;
	}
   	if( !str_prefix( arg2, "disease") && IS_SET(terr->crimeflags, CRIME_DISEASE)) 
	{
		argument = "spreading diseases";
       		act("$N has arrested $n for spreading diseases.", victim, NULL, ch, TO_ALL);
		if(terr->number == 1)
			judgement(victim, "spreading disease", 1, -1);
		if(terr->number == 2)
			judgement(victim, "spreading disease", 2, -1);
		if(terr->number == 3)
			judgement(victim, "spreading disease", 3, -1);
		if(terr->number == 4)
			judgement(victim, "spreading disease", 4, -1);
		if(terr->number == 5)
			judgement(victim, "spreading disease", 5, -1);
		if(terr->number == 6)
			judgement(victim, "spreading disease", 6, -1);
		if(terr->number == 7)
			judgement(victim, "spreading disease", 7, -1);
		if(terr->number == 8)
			judgement(victim, "spreading disease", 8, -1);
		if(terr->number == 9)
			judgement(victim, "spreading disease", 9, -1);
		if(terr->number == 10)
			judgement(victim, "spreading disease", 10, -1);
   		return;
	}
   	if( !str_prefix( arg2, "religion") && IS_SET(terr->crimeflags, CRIME_RELIGION)) 
	{
		argument = "practicing outlawed religion";
       		act("$N has arrested $n for practicing outlawed religion.", victim, NULL, ch, TO_ALL);
		if(terr->number == 1)
			judgement(victim, "outlawed religion", 1, 0);
		if(terr->number == 2)
			judgement(victim, "outlawed religion", 2, 0);
		if(terr->number == 3)
			judgement(victim, "outlawed religion", 3, 0);
		if(terr->number == 4)
			judgement(victim, "outlawed religion", 4, 0);
		if(terr->number == 5)
			judgement(victim, "outlawed religion", 5, 0);
		if(terr->number == 6)
			judgement(victim, "outlawed religion", 6, 0);
		if(terr->number == 7)
			judgement(victim, "outlawed religion", 7, 0);
		if(terr->number == 8)
			judgement(victim, "outlawed religion", 8, 0);
		if(terr->number == 9)
			judgement(victim, "outlawed religion", 9, 0);
		if(terr->number == 10)
			judgement(victim, "outlawed religion", 10, 0);
   		return;
	}
   	if( !str_prefix( arg2, "threats") && IS_SET(terr->crimeflags, CRIME_THREATS)) 
	{
		argument = "making threats against others";
       		act("$N has arrested $n for making threats against others.", victim, NULL, ch, TO_ALL);
		if(terr->number == 1)
			judgement(victim, "making threats", 1, 1);
		if(terr->number == 2)
			judgement(victim, "making threats", 2, 1);
		if(terr->number == 3)
			judgement(victim, "making threats", 3, 1);
		if(terr->number == 4)
			judgement(victim, "making threats", 4, 1);
		if(terr->number == 5)
			judgement(victim, "making threats", 5, 1);
		if(terr->number == 6)
			judgement(victim, "making threats", 6, 1);
		if(terr->number == 7)
			judgement(victim, "making threats", 7, 1);
		if(terr->number == 8)
			judgement(victim, "making threats", 8, 1);
		if(terr->number == 9)
			judgement(victim, "making threats", 9, 1);
		if(terr->number == 10)
			judgement(victim, "making threats", 10, 1);
   		return;
	}
   	if( !str_prefix( arg2, "disturb_peace") && IS_SET(terr->crimeflags, CRIME_DISTURB_PEACE)) 
	{
		argument = "disturbing the peace";
       		act("$N has arrested $n for disturbing the peace.", victim, NULL, ch, TO_ALL);
		if(terr->number == 1)
			judgement(victim, "disturbing the peace", 1, 1);
		if(terr->number == 2)
			judgement(victim, "disturbing the peace", 2, 1);
		if(terr->number == 3)
			judgement(victim, "disturbing the peace", 3, 1);
		if(terr->number == 4)
			judgement(victim, "disturbing the peace", 4, 1);
		if(terr->number == 5)
			judgement(victim, "disturbing the peace", 5, 1);
		if(terr->number == 6)
			judgement(victim, "disturbing the peace", 6, 1);
		if(terr->number == 7)
			judgement(victim, "disturbing the peace", 7, 1);
		if(terr->number == 8)
			judgement(victim, "disturbing the peace", 8, 1);
		if(terr->number == 9)
			judgement(victim, "disturbing the peace", 9, 1);
		if(terr->number == 10)
			judgement(victim, "disturbing the peace", 10, 1);
   		return;
	}
   	if( !str_prefix( arg2, "blackmail") && IS_SET(terr->crimeflags, CRIME_BLACKMAIL)) 
	{
		argument = "blackmail";
       		act("$N has arrested $n for blackmail.", victim, NULL, ch, TO_ALL);
		if(terr->number == 1)
			judgement(victim, "blackmail", 1, 0);
		if(terr->number == 2)
			judgement(victim, "blackmail", 2, 0);
		if(terr->number == 3)
			judgement(victim, "blackmail", 3, 0);
		if(terr->number == 4)
			judgement(victim, "blackmail", 4, 0);
		if(terr->number == 5)
			judgement(victim, "blackmail", 5, 0);
		if(terr->number == 6)
			judgement(victim, "blackmail", 6, 0);
		if(terr->number == 7)
			judgement(victim, "blackmail", 7, 0);
		if(terr->number == 8)
			judgement(victim, "blackmail", 8, 0);
		if(terr->number == 9)
			judgement(victim, "blackmail", 9, 0);
		if(terr->number == 10)
			judgement(victim, "blackmail", 10, 0);
   		return;
	}
   	if( !str_prefix( arg2, "evade_law") && IS_SET(terr->crimeflags, CRIME_EVADING_LAW)) 
	{
		argument = "evading the law";
       		act("$N has arrested $n for evading the law.", victim, NULL, ch, TO_ALL);
		if(terr->number == 1)
			judgement(victim, "evading arrest", 1, 1);
		if(terr->number == 2)
			judgement(victim, "evading arrest", 2, 1);
		if(terr->number == 3)
			judgement(victim, "evading arrest", 3, 1);
		if(terr->number == 4)
			judgement(victim, "evading arrest", 4, 1);
		if(terr->number == 5)
			judgement(victim, "evading arrest", 5, 1);
		if(terr->number == 6)
			judgement(victim, "evading arrest", 6, 1);
		if(terr->number == 7)
			judgement(victim, "evading arrest", 7, 1);
		if(terr->number == 8)
			judgement(victim, "evading arrest", 8, 1);
		if(terr->number == 9)
			judgement(victim, "evading arrest", 9, 1);
		if(terr->number == 10)
			judgement(victim, "evading arrest", 10, 1);
   		return;
	}
   	if( !str_prefix( arg2, "necromancy") && IS_SET(terr->crimeflags, CRIME_NECROMANCY)) 
	{
		argument = "necromancy";
       		act("$N has arrested $n for necromancy.", victim, NULL, ch, TO_ALL);
		if(terr->number == 1)
			judgement(victim, "practicing necromancy", 1, 1);
		if(terr->number == 2)
			judgement(victim, "practicing necromancy", 2, 1);
		if(terr->number == 3)
			judgement(victim, "practicing necromancy", 3, 1);
		if(terr->number == 4)
			judgement(victim, "practicing necromancy", 4, 1);
		if(terr->number == 5)
			judgement(victim, "practicing necromancy", 5, 1);
		if(terr->number == 6)
			judgement(victim, "practicing necromancy", 6, 1);
		if(terr->number == 7)
			judgement(victim, "practicing necromancy", 7, 1);
		if(terr->number == 8)
			judgement(victim, "practicing necromancy", 8, 1);
		if(terr->number == 9)
			judgement(victim, "practicing necromancy", 9, 1);
		if(terr->number == 10)
			judgement(victim, "practicing necromancy", 10, 1);
   		return;
	}


 	if( !str_prefix( arg2, "dragon") && IS_SET(terr->crimeflags, CRIME_NECROMANCY)) 
	{
		argument = "dragon";
       		act("$N has arrested $n for being in dragon form.", victim, NULL, ch, TO_ALL);
		if(terr->number == 1)
			judgement(victim, "being in dragon form", 1, 2);
		if(terr->number == 2)
			judgement(victim, "being in dragon form", 2, 2);
		if(terr->number == 3)
			judgement(victim, "being in dragon form", 3, 2);
		if(terr->number == 4)
			judgement(victim, "being in dragon form", 4, 2);
		if(terr->number == 5)
			judgement(victim, "being in dragon form", 5, 2);
		if(terr->number == 6)
			judgement(victim, "being in dragon form", 6, 2);
		if(terr->number == 7)
			judgement(victim, "being in dragon form", 7, 2);
		if(terr->number == 8)
			judgement(victim, "being in dragon form", 8, 2);
		if(terr->number == 9)
			judgement(victim, "being in dragon form", 9, 2);
		if(terr->number == 10)
			judgement(victim, "being in dragon form", 10, 2);
   		return;
	}
 	if( !str_prefix( arg2, "undead") && IS_SET(terr->crimeflags, CRIME_UNDEAD)) 
	{
		argument = "undead";
       		act("$N has arrested $n for onsorting with the undead.", victim, NULL, ch, TO_ALL);
		if(terr->number == 1)
			judgement(victim, "undead acts", 1, 1);
		if(terr->number == 2)
			judgement(victim, "undead acts", 2, 1);
		if(terr->number == 3)
			judgement(victim, "undead acts", 3, 1);
		if(terr->number == 4)
			judgement(victim, "undead acts", 4, 1);
		if(terr->number == 5)
			judgement(victim, "undead acts", 5, 1);
		if(terr->number == 6)
			judgement(victim, "undead acts", 6, 1);
		if(terr->number == 7)
			judgement(victim, "undead acts", 7, 1);
		if(terr->number == 8)
			judgement(victim, "undead acts", 8, 1);
		if(terr->number == 9)
			judgement(victim, "undead acts", 9, 1);
		if(terr->number == 10)
			judgement(victim, "undead acts", 10, 1);
   		return;
	}

	ch->println("That is not a crime worthy of punishment.");
	ch->println("Syntax: arrest <name> <crime>");
	ch->printlnf("`#`SCrimes Available: `W[`Y%s`W]", flag_string(crime_flags, terr->crimeflags));
	return;
}
/**************************************************************************/
void judgement(char_data *ch, char *argument, int number, int mod)
{
	char_data *judge;
   	int punishment;
	char judge1[MSL], judge2[MSL];
	char lawyer1[MSL], lawyer2[MSL];
	char lawyer3[MSL], lawyer4[MSL];
	char crime[MSL], buf[MSL];
	TERRITORY_DATA *terr;

	terr = territory_number(number);

	if(IS_NULLSTR(argument))
		argument = "breaking established laws";

   info_broadcast(ch, "%s is on trial for the crime of %s commited in %s.`^", ch->name, argument, terr->name);
   char_from_room(ch);
   char_to_room(ch, get_room_index(3094));
   do_look(ch, "auto");

   for ( judge = ch->in_room->people; judge; judge = judge->next_in_room )
   {
   	if ( IS_NPC(judge) && IS_SET(judge->act2, ACT2_COURTJUDGE) )
      	break;
   }
	switch( dice( 1, 8 ) )
   {
		case 1:
			mod -= 4; // Judge Angry
			sprintf(judge1, "$N look to be pretty angry.");
    		sprintf(judge2, "$N glares at $n in anger.");
			break;
		case 2:
			mod -= 3; // Judge Distracted
			sprintf(judge1, "$N appears to be distracted.");
    		sprintf(judge2, "$N appears to be distracted.");
			break;
		case 3:
			mod -= 2; // Judge Irritated
			sprintf(judge1, "$N looks irritated to see you.");
    		sprintf(judge2, "$N looks very irrated to see $n.");
			break;
		case 4:
			mod -= 1; // Judge Hungover
			sprintf(judge1, "$N appears to have a hangover of all things.");
    			sprintf(judge2, "$N appears to have a hangover of all things.");
			break;
		case 5:
			mod += 0; // Judge Bored
			sprintf(judge1, "$N looks pretty bored and almost nods off to sleep.");
    		sprintf(judge2, "$N looks at $n in total boredom.");
			break;
		case 6:
			mod += 1; // Judge Interested
			sprintf(judge1, "$N perks up as you approach him.");
    		sprintf(judge2, "$N looks interested in $n's case.");
			break;
		case 7:
			mod += 2; // Judge Sympathetic
			sprintf(judge1, "The judge looks like a fairly nice fellow.");
    		sprintf(judge2, "The judge looks to be in a good mood.");
			break;
		case 8:
			mod += 3; // Judge Amiable
			sprintf(judge1, "$N smiles at you.");
    		sprintf(judge2, "$N smiles at $n.");
  			break;
	}

	switch( dice( 1, 8 ) )
   {
		case 1:
			mod -= 3; // Lawyer Didn't Show Up
			sprintf(lawyer1, "It appears your lawyer didn't show up!");
    		sprintf(lawyer2, "$n doesn't have a lawyer, this should be a short trial.");
			sprintf(lawyer3, "You mutter an excuse to the judge for your actions.");
			sprintf(lawyer4, "$n doesn't have a lawyer, and mutters a lame excuse to the judge.");
			break;
		case 2:
			mod -= 4; // Lawyer Hates Client
			sprintf(lawyer1, "Your lawyer looks in you in disgust.");
    		sprintf(lawyer2, "The lawyer looks at $n in utter disgust.");
			sprintf(lawyer3, "Your lawyer tells the judge that you probably commited the crime.");
			sprintf(lawyer4, "The lawyer tells the judge that $n probably commited the crime.");
			break;
		case 3:
			mod -= 2; // Lawyer Dislikes Client
			sprintf(lawyer1, "The lawyer frowns at you.");
    		sprintf(lawyer2, "The lawyer frowns at $n.");
			sprintf(lawyer3, "The lawyer tells the judge you had a bad childhood.");
			sprintf(lawyer4, "The lawyer says that $n had a bad childhood.");
			break;
		case 4:
			mod += 0; // Lawyer Concerned
			sprintf(lawyer1, "The lawyer glances your way as you approach.");
    		sprintf(lawyer2, "The lawyer glances at $n and goes over his papers.");
			sprintf(lawyer3, "The lawyer ask you to explain what happened to the judge, and you do.");
			sprintf(lawyer4, "The lawyer has $n explain their actions to the judge.");
			break;
		case 5:
			mod += 1; // Lawyer Deeply Concerned
			sprintf(lawyer1, "The lawyer looks concerned about your case.");
    		sprintf(lawyer2, "The lawyer looks concerned about $n's case.");
			sprintf(lawyer3, "The lawyer tells the judge that you did not commit the crime.");
			sprintf(lawyer4, "The lawyer says that $n did not commit the crime.");
			break;
		case 6:
			mod += 2; // Lawyer Lucid
			sprintf(lawyer1, "The lawyer perks up as you approach him.");
    		sprintf(lawyer2, "The lawyer looks interested in $n's case.");
			sprintf(lawyer3, "The lawyer informs the judge you are innocent of the charges.");
			sprintf(lawyer4, "The lawyer informs the judge that $n is innocent of the charges.");
			break;
		case 7:
			mod += 3; // Lawyer Eloquent
			sprintf(lawyer1, "The lawyer looks like a fairly nice fellow.");
    		sprintf(lawyer2, "The lawyer looks to be in a good mood.");
			sprintf(lawyer3, "The lawyer declares your innocence before the judge.");
			sprintf(lawyer4, "The lawyer declares that $n is innocent.");
			break;
		case 8:
			mod += 4; // Lawyer Inspired
			sprintf(lawyer1, "The lawyer smiles at you.");
    		sprintf(lawyer2, "The lawyer smiles at $n.");
			sprintf(lawyer3, "The lawyer elegantly declares your innocence before the judge.");
			sprintf(lawyer4, "The lawyer elegantly declares that $n is innocent.");
			break;
	}

	if (ch->perm_stats[STAT_PR] < 20) {
		mod -= 2;
   } else
   if (ch->perm_stats[STAT_PR] < 40) {
		mod -= 1;
   } else
   if (ch->perm_stats[STAT_PR] < 60) {
		mod += 1;
   } else
   if (ch->perm_stats[STAT_PR] < 80) {
		mod += 2;
   } else
   if (ch->perm_stats[STAT_PR] < 120) {
		mod += 3; 
   }
    
   act("$N calls the court to order with his gavel.", ch,NULL,judge,TO_ROOM);
  	ch->set_pdelay(1);
   act("You are called to appear before the judge.", ch,NULL, ch,TO_CHAR);
   act("$n is called to appear before the judge.", ch, NULL, NULL, TO_ROOM );

	/*** Judge shows thier mood ***/
	ch->set_pdelay(3);
   act(judge1, ch, NULL, judge, TO_CHAR);
   act(judge2, ch, NULL, judge, TO_ROOM );

	/*** Lawyer shows their mood ***/
	ch->set_pdelay(5);
   act(lawyer1, ch,NULL, ch,TO_CHAR);
   act(lawyer2, ch, NULL, NULL, TO_ROOM );

	/*** Judge reads the charges against ch ***/
	ch->set_pdelay(7);
	sprintf( crime, "$N says '$n is accused of the crime of %s.'", argument );
	act(crime, ch, NULL, judge, TO_ALL );

	/*** Lawyer Speaks to Judge about case ***/
	ch->set_pdelay(9);
   act(lawyer3, ch, NULL, ch, TO_CHAR);
   act(lawyer4, ch, NULL, NULL, TO_ROOM );

   punishment = (number_range(1, 20)) + mod;
	if(ch->pcdata->murder != 0)
		punishment -= ch->pcdata->murder/10;
	if(ch->pcdata->theft != 0)
		punishment -= ch->pcdata->theft/10;

   REMOVE_BIT(ch->dyn, DYN_ONTRIAL);

   if (punishment < 1)
		punishment = 1;
   if (punishment > 20)
		punishment = 20;

	/*** Punishment Read ***/
	ch->set_pdelay(12);

   if (punishment == 20) /* Beneficial Judgement */
   {
  		act("$N says 'These charges are totally unfounded. Take this small compensation for your trouble.'", ch, NULL, judge, TO_CHAR);
  		act("$N says '$n should never have been brought in here. $n should be compensated for their trouble.'", ch, NULL, judge, TO_ROOM );
  		act("$N gives you 25 gold.'", ch, NULL, judge, TO_CHAR);
		ch->gold += 25;
		sprintf(buf,"%s was paid 25 gold by the court for their trouble, and found innocent.", ch->name);
		logf(buf, ch->name);
		wiznet(buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
		autonote(NOTE_PKNOTE, "The Courthouse","Judgement Ruling", "all", buf, true);

   }
   if (punishment == 19) /* Favorable Judgement */
   {
  		act("$N says 'I doubt you would even commit such a crime. Case dismissed.'", ch, NULL, judge, TO_CHAR);
  		act("$N says '$n would never commit such a crime. Case dismissed.'", ch, NULL, judge, TO_ROOM );
		sprintf(buf,"%s had their case dismissed by the judge.", ch->name);
		logf(buf, ch->name);
		wiznet(buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
		autonote(NOTE_PKNOTE, "The Courthouse","Judgement Ruling", "all", buf, true);

   }
   if (punishment == 18) /* Case Dismissed */
   {
  		act("$N says 'I see no basis for the charges. Case dismissed.'", ch, NULL, judge, TO_CHAR);
  		act("$N says 'I see no basis on the charges against $n. Case dismissed.'", ch, NULL, judge, TO_ROOM );
		sprintf(buf,"%s had their case dismissed by the judge.", ch->name);
		logf(buf, ch->name);
		wiznet(buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
		autonote(NOTE_PKNOTE, "The Courthouse","Judgement Ruling", "all", buf, true);

   }
   if (punishment == 17) /* Pay Court Cost */
   {
  		act("$N says 'You will be fined the court cost of 10 gold.'", ch, NULL, judge, TO_CHAR);
  		act("$N says '$n is fined the court cost of 10 gold.'", ch, NULL, judge, TO_ROOM );
    		if (1000 > (ch->gold * 100 + ch->silver))
		{
  			act("$N says 'You do not have enough on you to pay the fine.'", ch, NULL, judge, TO_CHAR);
  			act("$N says '$n is unable to pay at this time.'", ch, NULL, judge, TO_ROOM );
			punishment = 16;
		}
		else 
		{
			deduct_cost(ch, 1000);
			sprintf(buf,"%s was ordered to pay the court cost of 10 gold.", ch->name);
			logf(buf, ch->name);
			wiznet(buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
			autonote(NOTE_PKNOTE, "The Courthouse","Judgement Ruling", "all", buf, true);

		}
   }
   if (punishment == 16) /* Reduced Jail */
   {
		ch->pcdata->murder = 1;
		if(ch->pcdata->murder > 49)
		{
			punishment = 2;
		}
		else
		{
    		act("$N says 'I have reduced your prison sentence.'", ch, NULL, judge, TO_CHAR);
    		act("$N says '$n's prison sentence is reduced.'", ch, NULL, judge, TO_ROOM );
		ch->set_pdelay(13);
    		act("$N says 'You are to be imprisoned for your crimes.'", ch, NULL, judge, TO_CHAR);
    		act("$N says '$n's is to be imprisoned for their crimes.'", ch, NULL, judge, TO_ROOM );
		ch->pcdata->thief_until=0;
		if(IS_IMMORTAL(ch))
		{
			ch->pcdata->theft = 0;
			ch->pcdata->murder = 0;
			return;
		}
			do_startprison(ch, "");
			sprintf(buf,"%s was sentenced to a reduced prison term for their crimes.", ch->name);
			logf(buf, ch->name);
			wiznet(buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
			autonote(NOTE_PKNOTE, "The Courthouse","Judgement Ruling", "all", buf, true);

			return;
		}
   }
   if (punishment == 15) /* Fined */
   {
  	act("$N says 'You will be fined 100 gold for your crimes.'", ch, NULL, judge, TO_CHAR);
  	act("$N says '$n is fined 100 gold for their crimes.'", ch, NULL, judge, TO_ROOM );
    	if (10000 > (ch->gold * 100 + ch->silver))
		{
  			act("$N says 'You do not have enough on you to pay the fine.'", ch, NULL, judge, TO_CHAR);
  			act("$N says '$n is unable to pay at this time.'", ch, NULL, judge, TO_ROOM );
			punishment = 11;
		}
		else 
		{
			deduct_cost(ch, 10000);
			sprintf(buf,"%s was fined 100 gold for their crimes.", ch->name);
			logf(buf, ch->name);
			wiznet(buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
			autonote(NOTE_PKNOTE, "The Courthouse","Judgement Ruling", "all", buf, true);
		}
   }
   if (punishment == 14) /* High Fine */
   {
  		act("$N says 'You will be fined 500 gold for your crimes.'", ch, NULL, judge, TO_CHAR);
  		act("$N says '$n is fined 500 gold for their crimes.'", ch, NULL, judge, TO_ROOM );
    	if (50000 > (ch->gold * 100 + ch->silver))
		{
  			act("$N says 'You do not have enough on you to pay the fine.'", ch, NULL, judge, TO_CHAR);
  			act("$N says '$n is unable to pay at this time.'", ch, NULL, judge, TO_ROOM );
			punishment = 10;
		}
		else 
		{
			deduct_cost(ch, 50000);
			sprintf(buf,"%s was fined 500 gold for their crimes.", ch->name);
			logf(buf, ch->name);
			wiznet(buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
			autonote(NOTE_PKNOTE, "The Courthouse","Judgement Ruling", "all", buf, true);
		}
   }
   if (punishment == 13) /* Stocks */
   {
  		act("$N says 'You are sentenced to the stocks for 24 hours.'", ch, NULL, judge, TO_CHAR);
  		act("$N says '$n is sentenced to the stocks.'", ch, NULL, judge, TO_ROOM );
		ch->set_pdelay(13);
		if(IS_IMMORTAL(ch))
		{
			ch->pcdata->theft = 0;
			ch->pcdata->murder = 0;
			return;
		}
		ch->pcdata->theft = 0;
		ch->pcdata->murder = 0;
		SET_BIT(ch->affected_by2, AFF2_STOCKADE);
		sprintf(buf,"%s was sentenced to one day in the stocks.", ch->name);
		logf(buf, ch->name);
		wiznet(buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
		autonote(NOTE_PKNOTE, "The Courthouse","Judgement Ruling", "all", buf, true);
		ch->pcdata->thief_until=0;
		if(IS_WEREWOLF(ch))
		{
			char_from_room(ch);
			char_to_room(ch, get_room_index(3094));
			do_look(ch, "auto");
			ch->pcdata->jail = current_time+ (1 * ICTIME_IRLSECS_PER_WEEK);
			ch->printlnf("Your release date is %s (%.24s)", 
				get_shorticdate_from_time(ch->pcdata->jail, "", 0),
				ctime(&ch->pcdata->jail));
		return;
		}
		
		char_from_room(ch);
		char_to_room(ch, get_room_index(terr->stockade));
		do_look(ch, "auto");
		ch->pcdata->jail = current_time+ (1 * ICTIME_IRLSECS_PER_WEEK);
		ch->printlnf("Your release date is %s (%.24s)", 
			get_shorticdate_from_time(ch->pcdata->jail, "", 0),
			ctime(&ch->pcdata->jail));
		return;
   }
   if (punishment == 12) /* Stocks 2 */
   {
  		act("$N says 'You are sentenced to the stocks for 48 hours.'", ch, NULL, judge, TO_CHAR);
  		act("$N says '$n is sentenced to the stocks.'", ch, NULL, judge, TO_ROOM );
		ch->set_pdelay(13);
		if(IS_IMMORTAL(ch))
		{
			ch->pcdata->theft = 0;
			ch->pcdata->murder = 0;
			return;
		}
		ch->pcdata->theft = 0;
		ch->pcdata->murder = 0;
		SET_BIT(ch->affected_by2, AFF2_STOCKADE);
		sprintf(buf,"%s was sentenced to two days in the stocks.", ch->name);
		logf(buf, ch->name);
		wiznet(buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
		autonote(NOTE_PKNOTE, "The Courthouse","Judgement Ruling", "all", buf, true);
		ch->pcdata->thief_until=0;
		if(IS_WEREWOLF(ch))
		{
			char_from_room(ch);
			char_to_room(ch, get_room_index(3094));
			do_look(ch, "auto");
			ch->pcdata->jail = current_time+ (2 * ICTIME_IRLSECS_PER_WEEK);
			ch->printlnf("Your release date is %s (%.24s)", 
				get_shorticdate_from_time(ch->pcdata->jail, "", 0),
				ctime(&ch->pcdata->jail));
		return;
		}
		char_from_room(ch);
		char_to_room(ch, get_room_index(terr->stockade));
		do_look(ch, "auto");
		ch->pcdata->jail = current_time+ (2 * ICTIME_IRLSECS_PER_WEEK);
		ch->printlnf("Your release date is %s (%.24s)", 
			get_shorticdate_from_time(ch->pcdata->jail, "", 0),
			ctime(&ch->pcdata->jail));
		return;
   }
   if (punishment == 11) /* Jailed */
   {
		if(ch->pcdata->murder < 3)
			ch->pcdata->murder = 3;
		if(ch->pcdata->theft < 3)
			ch->pcdata->theft = 3;
		ch->pcdata->murder = ch->pcdata->murder/3;
		ch->pcdata->theft  = ch->pcdata->theft/3;
		ch->pcdata->thief_until=0;
		if(ch->pcdata->murder > 49)
		{
			punishment = 2;
		}
		else
		{
    			act("$N says 'I have reduced your prison sentence.'", ch, NULL, judge, TO_CHAR);
    			act("$N says '$n's prison sentence is reduced.'", ch, NULL, judge, TO_ROOM );
			ch->set_pdelay(13);
   	 		act("$N says 'You are to be imprisoned for your crimes.'", ch, NULL, judge, TO_CHAR);
    			act("$N says '$n's is to be imprisoned for their crimes.'", ch, NULL, judge, TO_ROOM );
			ch->pcdata->murder = 2;
			if(IS_IMMORTAL(ch))
			{
				ch->pcdata->theft = 0;
				ch->pcdata->murder = 0;
				return;
			}
			do_startprison(ch, "");
			ch->pcdata->thief_until=0;
			sprintf(buf,"%s was sentenced to a reduced prison term.", ch->name);
			logf(buf, ch->name);
			wiznet(buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
			autonote(NOTE_PKNOTE, "The Courthouse","Judgement Ruling", "all", buf, true);
			return;
		}
   }

   if (punishment == 10) /* Weapons Confiscated */
   {
	 act("$N says 'Your weapons are to be confiscated.", ch,NULL, judge, TO_CHAR);
 	 act("$N says '$n's weapons are to be confiscated.", ch, NULL, judge, TO_ROOM );
		if(IS_IMMORTAL(ch))
		{
			ch->pcdata->theft = 0;
			ch->pcdata->murder = 0;
			return;
		}
		obj_data *obj;
		obj_data *obj_next;
	  	for (obj = ch->carrying; obj != NULL; obj = obj_next)
	    	{
		    obj_next = obj->next_content;
		    if(obj->item_type == ITEM_WEAPON)
		    {
		            unequip_char( ch, obj );
		            obj_from_char( obj );
			    extract_obj(obj);
			    act( "$p is torn from $n by the guards!", ch, obj, NULL, TO_ROOM );
			    act( "$p is torn from your grasp by the guards!", ch, obj, NULL, TO_CHAR );
		    }
		}
		save_char_obj(ch);
		sprintf(buf,"%s was sentenced confiscation of all weapons.", ch->name);
		logf(buf, ch->name);
		wiznet(buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
		autonote(NOTE_PKNOTE, "The Courthouse","Judgement Ruling", "all", buf, true);
   }

   if (punishment == 9) /* Magical Iems Confiscated */
   {
    	act("$N says 'Your magical items are to be confiscated.'", ch, NULL, judge, TO_CHAR);
    	act("$N says '$n's magical items are to be confiscated.'", ch, NULL, judge, TO_ROOM );

		if(IS_IMMORTAL(ch))
		{
			ch->pcdata->theft = 0;
			ch->pcdata->murder = 0;
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
			    act( "$p is torn from $n by the guards!", ch, obj, NULL, TO_ROOM );
			    act( "$p is torn from your grasp by the guards!", ch, obj, NULL, TO_CHAR );
		    }
		}
		sprintf(buf,"%s was sentenced to confiscation of all magical items.", ch->name);
		logf(buf, ch->name);
		wiznet(buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
		autonote(NOTE_PKNOTE, "The Courthouse","Judgement Ruling", "all", buf, true);
		save_char_obj(ch);
   }

   if (punishment == 8) /* Gauntlet */
   {
    	act("$N says 'You sentenced to run the Gauntlet.'", ch, NULL, judge, TO_CHAR);
    	act("$N says '$n's is to run the Gauntlet.'", ch, NULL, judge, TO_ROOM );
	ch->pcdata->theft = 0;
	ch->pcdata->murder = 0;
	ch->pcdata->thief_until=0;
	char_from_room(ch);
	char_to_room(ch, get_room_index(4800));
	do_look(ch, "auto");
	sprintf(buf,"%s was sentenced to run the Gauntlet.", ch->name);
	logf(buf, ch->name);
	wiznet(buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
	autonote(NOTE_PKNOTE, "The Courthouse","Judgement Ruling", "all", buf, true);
	return;
   }
   if (punishment == 7) /* Prison */
   {
	if(ch->pcdata->murder > 49)
	{
		punishment = 2;
	}
	else
	{
		act("$N says 'You are to be imprisoned for your crimes.'", ch, NULL, judge, TO_CHAR);
		act("$N says '$n's is to be imprisoned for their crimes.'", ch, NULL, judge, TO_ROOM );
		if(IS_IMMORTAL(ch))
		{
			ch->pcdata->theft = 0;
			ch->pcdata->murder = 0;
			ch->pcdata->thief_until=0;
			return;
		}
		do_startprison(ch, "");
		ch->pcdata->thief_until=0;
		sprintf(buf,"%s was sentenced to be imprisoned for their crimes.", ch->name);
		logf(buf, ch->name);
		wiznet(buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
		autonote(NOTE_PKNOTE, "The Courthouse","Judgement Ruling", "all", buf, true);
		return;
	}
   }
   if (punishment == 6) /* Whipped */
   {
    	act("$N says 'You are to be fed to the dragons.'", ch, NULL, judge, TO_CHAR);
    	act("$N says '$n's is to be fed to the dragons.'", ch, NULL, judge, TO_ROOM );
	ch->pcdata->theft = 0;
	ch->pcdata->murder = 0;
	ch->pcdata->thief_until=0;
	char_from_room(ch);
	char_to_room(ch, get_room_index(2518));
	do_look(ch, "auto");
	sprintf(buf,"%s was sentenced to be fed to the dragons.", ch->name);
	logf(buf, ch->name);
	wiznet(buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
	autonote(NOTE_PKNOTE, "The Courthouse","Judgement Ruling", "all", buf, true);
	return;
   }
   if (punishment == 5) /* Sent to Hero's Keep */
   {
    	act("$N says 'You are to be fed to the dragons.'", ch, NULL, judge, TO_CHAR);
    	act("$N says '$n's is to be fed to the dragons.'", ch, NULL, judge, TO_ROOM );
	ch->pcdata->theft = 0;
	ch->pcdata->murder = 0;
	ch->pcdata->thief_until=0;
	char_from_room(ch);
	char_to_room(ch, get_room_index(2518));
	do_look(ch, "auto");
	sprintf(buf,"%s was sentenced to be fed to the dragons.", ch->name);
	logf(buf, ch->name);
	wiznet(buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
	autonote(NOTE_PKNOTE, "The Courthouse","Judgement Ruling", "all", buf, true);
	return;
   }
   if (punishment == 4) /* Flayed */
   {
    	act("$N says 'You are to be skinned and flayed alive your crimes.'", ch, NULL, judge, TO_CHAR);
    	act("$N says '$n's is to be skinned and flayed alive for their crimes.'", ch, NULL, judge, TO_ROOM );
	if(IS_IMMORTAL(ch))
	{
		ch->pcdata->theft = 0;
		ch->pcdata->murder = 0;
		return;
	}
	ch->pcdata->theft = 0;
	ch->pcdata->thief_until=0;
	ch->pcdata->murder = 0;
	ch->position = POS_DEAD;
	sprintf(buf,"%s was sentenced to skinned and flayed alive.", ch->name);
	logf(buf, ch->name);
	wiznet(buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
	autonote(NOTE_PKNOTE, "The Courthouse","Judgement Ruling", "all", buf, true);
	kill_char(ch, judge);
	return;
   }
   if (punishment == 3) /* Hanged */
   {
    	act("$N says 'You are to be hung until dead for your crimes.'", ch, NULL, judge, TO_CHAR);
    	act("$N says '$n's is to be hung until dead for their crimes.'", ch, NULL, judge, TO_ROOM );
	if(IS_IMMORTAL(ch))
	{
		ch->pcdata->theft = 0;
		ch->pcdata->murder = 0;
		return;
	}
	ch->pcdata->theft = 0;
	ch->pcdata->murder = 0;
	ch->position = POS_DEAD;
	ch->pcdata->thief_until=0;
	sprintf(buf,"%s was sentenced to be hung until dead.", ch->name);
	logf(buf, ch->name);
	wiznet(buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
	autonote(NOTE_PKNOTE, "The Courthouse","Judgement Ruling", "all", buf, true);
	kill_char(ch, judge);
	return;
   }

   if (punishment == 2) /* Beheaded */
   {
    	act("$N says 'You are to be beheaded for your crimes.'", ch, NULL, judge, TO_CHAR);
    	act("$N says '$n's is to be beheaded for their crimes.'", ch, NULL, judge, TO_ROOM );
	if(IS_IMMORTAL(ch))
	{
		ch->pcdata->theft = 0;
		ch->pcdata->murder = 0;
		return;
	}
	char_from_room(ch);
	char_to_room(ch, get_room_index(10398));
	do_look(ch, "auto");
	ch->pcdata->theft = 0;
	ch->pcdata->murder = 0;
	ch->pcdata->thief_until=0;
	sprintf(buf,"%s was sentenced to beheading for their crimes.", ch->name);
	logf(buf, ch->name);
	wiznet(buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
	autonote(NOTE_PKNOTE, "The Courthouse","Judgement Ruling", "all", buf, true);
	return;
   }

   if (punishment == 1) /* Drawn and Quartered */
   {
    	act("$N says 'You are sentenced to be drawn & quartered.'", ch, NULL, judge, TO_CHAR);
    	act("$N says '$n's is to be drawn & quartered.'", ch, NULL, judge, TO_ROOM );
	if(IS_IMMORTAL(ch))
	{
		ch->pcdata->theft = 0;
		ch->pcdata->murder = 0;
		return;
	}
	char_from_room(ch);
	char_to_room(ch, get_room_index(10398));
	do_look(ch, "auto");
	ch->pcdata->theft = 0;
	ch->pcdata->murder = 0;
	ch->pcdata->thief_until=0;
	sprintf(buf,"%s was sentenced to be drawn & quartered.", ch->name);
	logf(buf, ch->name);
	wiznet(buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
	autonote(NOTE_PKNOTE, "The Courthouse","Judgement Ruling", "all", buf, true);
	return;
   }

	ch->set_pdelay(14);
  	act("$N says 'You are free to go.'", ch, NULL, judge, TO_CHAR);
  	act("$N says '$n is free to go.'", ch, NULL, judge, TO_ROOM );
	ch->pcdata->theft = 0;
	ch->pcdata->murder = 0;
	ch->pcdata->thief_until=0;
	char_from_room(ch);
	char_to_room(ch, get_room_index(terr->stockade));
	return;
}
/**************************************************************************/
void do_stockade(char_data *ch, char *argument)
{
	char arg[MIL], buf[MSL];
	char_data *victim;
   	TERRITORY_DATA *terr;
	
	one_argument( argument, arg );
	terr = territory_number(11);
	
	if(IS_NPC(ch)) 
	{
		do_huh(ch, "");
		return;
	}

    	if(!IS_IMMORTAL(ch) && !IS_SET(ch->territory1, TERRITORY_GUARD))
    	{
		do_huh(ch, "");
		return;
    	}

 	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_1))
		terr = territory_number(1);
	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_2))
		terr = territory_number(2);
	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_3))
		terr = territory_number(3);
	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_4))
		terr = territory_number(4);
	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_5))
		terr = territory_number(5);
	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_6))
		terr = territory_number(6);
	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_7))
		terr = territory_number(7);
	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_8))
		terr = territory_number(8);
	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_9))
		terr = territory_number(9);
	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_10))
		terr = territory_number(10);

	if(!terr)
	{
		ch->printlnf("You cannot do that here.");
		return; 
	}
	
    	if ( arg[0] == '\0' )
    	{
		ch->println( "Stockade who ?" );
		return;
    	}
	
   	if((victim = get_char_room(ch,arg)) == NULL)
	{ 
		ch->printlnf("There is no player called '%s' here.", arg);
		return; 
	}
	
    	if ( IS_NPC(victim) )
   	{
		ch->println( "Not on NPC's." );
		return;
    	}

    	if(IS_IMMORTAL(victim)){
		ch->println( "You wish." );
		return;
	}

	if ( victim==ch){
		ch->println( "Yeah, right." );
		return;
	}
	
	if(IS_AFFECTED2(victim, AFF2_STOCKADE))
	{
		ch->printlnf("%s is released from the stocks.", victim->name);
		REMOVE_BIT(victim->affected_by2, AFF2_STOCKADE);
		victim->printlnf("You have been released from the stocks by %s.", ch->name);
		victim->pcdata->jail = current_time;
		return;
	}	
	info_broadcast(ch, "%s has been locked in the stocks of %s by a Justicar.`^", victim->name, terr->name);
	victim->printlnf("You have been locked in the stocks of %s by a Justicar.", terr->name);
	sprintf(buf,"%s sent %s to the stocks of %s.", ch->name, victim->name, terr->name);
	logf("%s sent %s to the stocks of %s.", ch->name, victim->name, terr->name);
	wiznet(buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
	autonote(NOTE_SNOTE, "Stockade","Stockade Punishment", "imm", buf, true);
	SET_BIT(victim->affected_by2, AFF2_STOCKADE);
   	char_from_room(victim);
	char_to_room(victim, get_room_index(terr->stockade));
	do_look(victim, "auto");
	victim->pcdata->jail = current_time+ (1 * ICTIME_IRLSECS_PER_WEEK);
	victim->printlnf("Your release date is %s (%.24s)", 
		get_shorticdate_from_time(victim->pcdata->jail, "", 0),
		ctime(&victim->pcdata->jail));
	return;
}
/**************************************************************************/
void do_guard( char_data *ch, char *argument )
{
	char arg[MIL],buf[MSL];
	char_data *victim;
	TERRITORY_DATA *terr;
	
	one_argument( argument, arg );
	terr = territory_number(11);

    	if(!IS_IMMORTAL(ch))
    	{
		do_huh(ch, "");
		return;
    	}

    	if (IS_NULLSTR(arg))
    	{
		ch->println( "Syntax: justicar <victim>" );
		return;
    	}
	
    	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    	{
		ch->println( "They aren't here." );
		return;
    	}
	
    	if ( IS_NPC(victim) )
    	{
		ch->println( "Not on NPC's." );
		return;
    	}
	
    	if( IS_SET(victim->territory1, TERRITORY_GUARD) )
    	{
		victim->println("You are no longer a Justicar of the realm.");
		ch->printlnf( "%s is no longer a Justicar of the realm.", victim->name );
		judge_broadcast(ch, "%s is no longer a Justicar of the realm.`^", victim->name);
		wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
		REMOVE_BIT(victim->territory1, TERRITORY_GUARD);
	    	save_char_obj( victim );
    		return;
	}

	victim->println("You are now a Justicar of the realm.");
	ch->printlnf( "%s is now a Justicar of the realm.", victim->name );
	judge_broadcast(ch, "%s is now a Justicar of the realm.`^", victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
	SET_BIT(victim->territory1, TERRITORY_GUARD);
    	save_char_obj( victim );
    	return;
}
/**************************************************************************/
void do_laws(char_data *ch, char *argument)
{
   	TERRITORY_DATA *terr;
    
	if(IS_NPC(ch)) 
	{
		do_huh(ch, "");
		return;
	}

	terr = territory_number(11);

  	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_1))
  	{
		terr = territory_number(1); }
  	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_2))
  	{
		terr = territory_number(2); }
  	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_3)) 
  	{
		terr = territory_number(3); }
  	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_4)) 
  	{
		terr = territory_number(4); }
  	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_5)) 
  	{
		terr = territory_number(5); }
  	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_6)) 
  	{
		terr = territory_number(6); }
  	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_7)) 
  	{
		terr = territory_number(7); }
  	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_8)) 
  	{
		terr = territory_number(8); }
  	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_9))  
  	{
		terr = territory_number(9); }
  	if(IS_SET(ch->in_room->room2_flags, ROOM2_TERR_10))
  	{
		terr = territory_number(10); 
	}

	if(!terr)
	{
		ch->printlnf("This area is not goverened by laws.");
		return; 
	}

	ch->printlnf("`Y-=-=-=-=-=-=-=-=-=-=-=-= LAWS OF THE CITY =-=-=-=-=-=-=-=-=-=-=-=-`c");

   	if(IS_SET(terr->crimeflags, CRIME_MURDER))
	{
		ch->printlnf("It is against the law to commit murder.");
		ch->printlnf(" ");
	}
   	if(IS_SET(terr->crimeflags, CRIME_THEFT))
	{
		ch->printlnf("It is against the law to steal or commit theft.");
		ch->printlnf(" ");
	}
   	if(IS_SET(terr->crimeflags, CRIME_WEAPONS))
	{	
		ch->printlnf("It is against the law to wield open weapons.");
		ch->printlnf(" ");
	}
   	if(IS_SET(terr->crimeflags, CRIME_ASSAULT)) 
	{
		ch->printlnf("It is against the law to assault another person.");
		ch->printlnf(" ");
	}
   	if(IS_SET(terr->crimeflags, CRIME_EXTORTION))
	{
		ch->printlnf("It is against the law to commit extortion.");
		ch->printlnf(" ");
	}
   	if(IS_SET(terr->crimeflags, CRIME_TREASON))
	{
		ch->printlnf("It is against the law to commit treason against the Mayor.");
		ch->printlnf(" ");
	}
   	if(IS_SET(terr->crimeflags, CRIME_BADMAGIC))
	{
		ch->printlnf("It is against the law to use magic to harm another.");
		ch->printlnf(" ");
	}
   	if(IS_SET(terr->crimeflags, CRIME_DRUNK))
	{
		ch->printlnf("It is against the law to be publicly intoxicated.");
		ch->printlnf(" ");
	}
   	if(IS_SET(terr->crimeflags, CRIME_BRIBE))
	{
		ch->printlnf("It is against the law to bribe any guard, Council Member or the Mayor.");
		ch->printlnf(" ");
	}
  	if(IS_SET(terr->crimeflags, CRIME_IMPERSONATE))
	{
		ch->printlnf("It is against the law to be in a polymorphed form.");
		ch->printlnf(" ");
	}
   	if(IS_SET(terr->crimeflags, CRIME_WEREWOLF))
	{
		ch->printlnf("Werewolves or other lycanthropes are not allowed within the jurisdiction of the city.");
		ch->printlnf(" ");
	}
   	if(IS_SET(terr->crimeflags, CRIME_VAMPIRE)) 
	{
		ch->printlnf("Vampires are not allowed within the jurisdiction of the city.");
		ch->printlnf(" ");
	}
   	if(IS_SET(terr->crimeflags, CRIME_DISEASE)) 
	{
		ch->printlnf("It is against the law to spread fatal diseases or plagues.");
		ch->printlnf(" ");
	}
   	if(IS_SET(terr->crimeflags, CRIME_RELIGION)) 
	{
		ch->printlnf("We do not allow members of outlawed religions in the city.");
		ch->printlnf(" ");
	}
   	if(IS_SET(terr->crimeflags, CRIME_THREATS)) 
	{
		ch->printlnf("It is against the law to threaten another person with harm.");
		ch->printlnf(" ");
	}
   	if(IS_SET(terr->crimeflags, CRIME_DISTURB_PEACE)) 
	{
		ch->printlnf("It is against the law to disturb the peace.");
		ch->printlnf(" ");
	}
   	if(IS_SET(terr->crimeflags, CRIME_BLACKMAIL)) 
	{
		ch->printlnf("It is against the law to blackmail another.");
		ch->printlnf(" ");
	}
   	if(IS_SET(terr->crimeflags, CRIME_EVADING_LAW)) 
	{
		ch->printlnf("It is against the law to evade capture and attempt escape from custody.");
		ch->printlnf(" ");
	}
   	if(IS_SET(terr->crimeflags, CRIME_NECROMANCY)) 
	{
		ch->printlnf("It is against the law to practice necromancy.");
		ch->printlnf(" ");
	}
   	if(IS_SET(terr->crimeflags, CRIME_DRAGON)) 
	{
		ch->printlnf("It is against the law to be in full dragon form.");
		ch->printlnf(" ");
	}
   	if(IS_SET(terr->crimeflags, CRIME_UNDEAD)) 
	{
		ch->printlnf("It is against the law to be undead or possess undead minions.");
		ch->printlnf(" ");
	}
	if(terr->description)
		ch->printlnf(terr->description);
	ch->printlnf("`Y-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-`x");

	return;

}
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/





