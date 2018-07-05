/**************************************************************************/
// quest.cpp - Quest Code by Brad Wilson - Adapted from many other quest codes
/***************************************************************************
 * Whispers of Times Lost (c)1998-2003 Brad Wilson (wotlmud@bellsouth.net) *
 * >> If you use this code you must give credit in your help file as well  *
 *    as leaving this header intact.                                       *
 * >> To use this source code, you must fully comply with all the licenses *
 *    in licenses below. In particular, you may not remove this copyright  *
 *    notice.                                                              *
 ***************************************************************************
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

#include "include.h"
#include "tables.h"

/* Object vnums for Quest Rewards */

#define QUEST_ITEM1 5099
#define QUEST_ITEM2 5007
#define QUEST_ITEM3 5008

#define QUEST_OBJQUEST1 5002 // Sceptre
#define QUEST_OBJQUEST2 5003 // Amulet
#define QUEST_OBJQUEST3 5004 // Cloak
#define QUEST_OBJQUEST4 5005 // Shield
#define QUEST_OBJQUEST5 5006 // Signet

/* Local functions */

void generate_quest     args(( char_data *ch, char_data *questman ));
void quest_update       args(( void ));
bool quest_level_diff   args(( int clevel, int mlevel));
bool chance             args(( int num ));
ROOM_INDEX_DATA         *find_location( char_data *ch, char *arg );

bool chance(int num)
{
    if (number_range(1,100) <= num) return true;
    else return false;
}

/**************************************************************************/
void do_quest(char_data *ch, char *argument)
{
    char_data *questman;
    OBJ_DATA *obj=NULL, *obj_next;
    OBJ_INDEX_DATA *questinfoobj;
    MOB_INDEX_DATA *questinfo;
    char arg1 [MIL];
    char arg2 [MIL];
    char buf [MSL];

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (IS_NULLSTR(arg1))
    {
        ch->println("JOBS commands: QUIT POINTS INFO TIME REQUEST COMPLETE LIST BUY.");
        ch->println("For more information, type 'HELP JOBS'.");
        return;
    }
    if (!strcmp(arg1, "info"))
    {
        if (IS_SET(ch->dyn, DYN_QUESTING))
        {
            if (ch->questmob == -1 && ch->questgiver->short_descr != NULL)
            {
    		ch->printlnf("Your quest is ALMOST complete!\n\rGet back to %s before your time runs out!\n\r",ch->questgiver->short_descr);
            }
            else if (ch->questobj > 0)
            {
                questinfoobj = get_obj_index(ch->questobj);
                if (questinfoobj != NULL)
                {
                    ch->printlnf("You are on a quest to recover the fabled %s!",questinfoobj->name);
                }
                else ch->println("You aren't currently on a quest.");
                return;
            }
            else if (ch->questmob > 0)
            {
                questinfo = get_mob_index(ch->questmob);
                if (questinfo != NULL)
                {
                    ch->printlnf("You are on a quest to slay the dreaded %s!",questinfo->short_descr);
                }
                else ch->println("You aren't currently on a quest.");
                return;
            }
        }
        else ch->println("You aren't currently on a quest.");
        return;
    }
    if (!strcmp(arg1, "points"))
    {
        ch->printlnf("You have %d quest points.",ch->pcdata->qpoints);
        return;
    }
    else if (!strcmp(arg1, "time"))
    {
        if (!IS_SET(ch->dyn, DYN_QUESTING))
        {
            ch->println("You aren't currently on a quest.");
            if (ch->nextquest > 1)
            {
                ch->printlnf("There are %d minutes remaining until you can go on another quest.",ch->nextquest);
            }
            else if (ch->nextquest == 1)
            {
                ch->printlnf("There is less than a minute remaining until you can go on another quest.");
            }
        }
        else if (ch->questtimer > 0)
        {
            ch->printlnf("Time left for current quest: %d",ch->questtimer);
        }
        return;
    }

    for ( questman = ch->in_room->people; questman != NULL; questman = questman->next_in_room )
    {
        if (!IS_NPC(questman)) continue;
        if (questman->spec_fun == spec_lookup( "spec_questmaster" )) break;
    }

    if (questman == NULL || questman->spec_fun != spec_lookup( "spec_questmaster" ))
    {
        ch->println("You can't do that here.");
        return;
    }

    if ( questman->fighting != NULL)
    {
        ch->println("Wait until the fighting stops.");
        return;
    }

    ch->questgiver = questman;

    if (!strcmp(arg1, "list"))
    {
        act( "$n asks $N for a list of quest items.", ch, NULL, questman, TO_ROOM);
        act ("You ask $N for a list of quest items.",ch, NULL, questman, TO_CHAR);
        ch->println("Current Quest Items available for Purchase:");
        ch->println("1000qp         Horn of Blasting");
        ch->println("750qp          Decanter of Endless Water");
        ch->println("500qp          1000 gold pieces");
        ch->println("250qp          10 Practices");
        ch->println("100qp          Restring Coupon");
        ch->println("To buy an item, type 'JOBS BUY <item>'.");
        return;
    }

    else if (!strcmp(arg1, "buy"))
    {
        if (IS_NULLSTR(arg2))
        {
            ch->println("To buy an item, type 'JOBS BUY <item>'.");
            return;
        }
        if (is_name(arg2, "horn"))
        {
            if (ch->pcdata->qpoints >= 1000)
            {
                ch->pcdata->qpoints -= 1000;
                obj = create_object(get_obj_index(QUEST_ITEM1));
            }
            else
            {
		act( "$N says 'Sorry, but you don't have enough quest points for that.'",   ch, NULL, questman, TO_CHAR );
                return;
            }
        }

        else if (is_name(arg2, "decanter"))
        {
            if (ch->pcdata->qpoints >= 750)
            {
                ch->pcdata->qpoints -= 750;
                obj = create_object(get_obj_index(QUEST_ITEM2));
            }
            else
            {
		act( "$N says 'Sorry, but you don't have enough quest points for that.'",   ch, NULL, questman, TO_CHAR );
                return;
            }
        }

       else if (is_name(arg2, "restring"))
        {
            if (ch->pcdata->qpoints >= 100)
            {
                ch->pcdata->qpoints -= 100;
                obj = create_object(get_obj_index(QUEST_ITEM3));
            }
            else
            {
		act( "$N says 'Sorry, but you don't have enough quest points for that.'",   ch, NULL, questman, TO_CHAR );
                return;
            }
        }

     else if (is_name(arg2, "practices pracs prac practice"))
        {
            if (ch->pcdata->qpoints >= 250)
            {
                ch->pcdata->qpoints -= 250;
                ch->practice += 10;
                act( "$N gives 10 practices to $n.", ch, NULL, questman, TO_ROOM );
                act( "$N gives you 10 practices.",   ch, NULL, questman, TO_CHAR );
                return;
            }
            else
            {
		act( "$N says 'Sorry, but you don't have enough quest points for that.'",   ch, NULL, questman, TO_CHAR );
                return;
            }
        }
        else if (is_name(arg2, "gold gp"))
        {
            if (ch->pcdata->qpoints >= 500)
            {
                ch->pcdata->qpoints -= 500;
                ch->gold += 1000;
                act( "$N gives 1,000 gold to $n.", ch, NULL, questman, TO_ROOM );
                act( "$N gives you 1,000 gold.",   ch, NULL, questman, TO_CHAR );
                return;
            }
            else
            {
		act( "$N says 'Sorry, but you don't have enough quest points for that.'",   ch, NULL, questman, TO_CHAR );
                return;
            }
        }
        else
        {
		act( "$N says 'Sorry, but I do not have that item.'",   ch, NULL, questman, TO_CHAR );
                return;
        }
        if (obj != NULL)
        {
            act( "$N gives $p to $n.", ch, obj, questman, TO_ROOM );
            act( "$N gives you $p.",   ch, obj, questman, TO_CHAR );
            obj_to_char(obj, ch);
        }
        return;
    }
    else if (!strcmp(arg1, "request"))
    {
        act( "$n asks $N for a quest.", ch, NULL, questman, TO_ROOM);
        act ("You ask $N for a quest.",ch, NULL, questman, TO_CHAR);
        if (IS_SET(ch->dyn, DYN_QUESTING))
        {
  	    act( "$N says 'But your already on a quest.'",   ch, NULL, questman, TO_CHAR );
            return;
        }
        if (ch->nextquest > 0)
        {
	    act( "$N says 'You're very brave, but let someone else have a chance.'",   ch, NULL, questman, TO_CHAR );
	    act( "$N says 'Come back later.'",   ch, NULL, questman, TO_CHAR );
            return;
        }

        sprintf(buf, "Thank you, brave %s!",ch->name);
	act( buf, ch, NULL, questman, TO_CHAR );
        ch->questmob = 0;
        ch->questobj = 0;

        generate_quest(ch, questman);

        if (ch->questmob > 0 || ch->questobj > 0)
        {
            ch->questtimer = number_range(10,30);
            SET_BIT(ch->dyn, DYN_QUESTING);
            sprintf(buf, "You have %d minutes to complete this quest.",ch->questtimer);
	    act( buf, ch, NULL, questman, TO_CHAR );
            sprintf(buf, "May the gods go with you!");
            act( buf, ch, NULL, questman, TO_CHAR );
        }
        return;
    }
    else if (!strcmp(arg1, "quit"))
    {
        act( "$n informs $N $e cannot complete the quest.", ch, NULL, questman, TO_ROOM);
        act ("You inform $N you cannot complete the quest.",ch, NULL, questman, TO_CHAR);
        REMOVE_BIT(ch->dyn, DYN_QUESTING);
        ch->questgiver = NULL;
        ch->questtimer = 0;
        ch->questmob = 0;
        ch->questobj = 0;
        ch->nextquest = 10;
	if (ch->questgiver != questman)
        {
            ch->println("I never sent you on a quest! Perhaps you're thinking of someone else.");
            return;
        }
    }
    else if (!strcmp(arg1, "complete"))
    {
        act( "$n informs $N $e has completed $s quest.", ch, NULL, questman, TO_ROOM);
        act ("You inform $N you have completed $s quest.",ch, NULL, questman, TO_CHAR);
        if (ch->questgiver != questman)
        {
            ch->println("I never sent you on a quest! Perhaps you're thinking of someone else.");
            return;
            return;
        }

        if (IS_SET(ch->dyn, DYN_QUESTING))
        {
            if (ch->questmob == -1 && ch->questtimer > 0)
            {
                int reward, pointreward, pracreward;

                reward = number_range(1,25);
                pointreward = number_range(10,25);

                sprintf(buf, "Congratulations on completing your quest!");
                act( buf, ch, NULL, questman, TO_CHAR );
                sprintf(buf,"As a reward, I am giving you %d quest points, and %d gold.",pointreward,reward);
                act( buf, ch, NULL, questman, TO_CHAR );
                if (chance(15))
                {
                    pracreward = number_range(1,6);
                    sprintf(buf, "You gain %d practices!",pracreward);
                    act( buf, ch, NULL, questman, TO_CHAR );
                    ch->practice += pracreward;
                }

                REMOVE_BIT(ch->dyn, DYN_QUESTING);
                ch->questgiver = NULL;
                ch->questtimer = 0;
                ch->questmob = 0;
                ch->questobj = 0;
                ch->nextquest = 10;
                ch->gold += reward;
                ch->pcdata->qpoints += pointreward;
                return;
            }
            else if (ch->questobj > 0 && ch->questtimer > 0)
            {
                bool obj_found = false;

                for (obj = ch->carrying; obj != NULL; obj= obj_next)
                {
                    obj_next = obj->next_content;

                    if (obj != NULL && obj->pIndexData->vnum == ch->questobj)
                    {
                        obj_found = true;
                        break;
                    }
                }
                if (obj_found == true)
                {
                    int reward, pointreward, pracreward;

                    reward = number_range(1,25);
                    pointreward = number_range(5,15);

                    act("You hand $p to $N.",ch, obj, questman, TO_CHAR);
                    act("$n hands $p to $N.",ch, obj, questman, TO_ROOM);

                    sprintf(buf, "Congratulations on completing your quest!");
                    act( buf, ch, NULL, questman, TO_CHAR );
                    sprintf(buf,"As a reward, I am giving you %d quest points, and %d gold.",pointreward,reward);
                    act( buf, ch, NULL, questman, TO_CHAR );
                    if (chance(15))
                    {
                        pracreward = number_range(1,6);
                        sprintf(buf, "You gain %d practices!",pracreward);
                        act( buf, ch, NULL, questman, TO_CHAR );
                        ch->practice += pracreward;
                    }

                    REMOVE_BIT(ch->dyn, DYN_QUESTING);
                    ch->questgiver = NULL;
                    ch->questtimer = 0;
                    ch->questmob = 0;
                    ch->questobj = 0;
                    ch->nextquest = 15;
                    ch->gold += reward;
                    ch->pcdata->qpoints += pointreward;
                    extract_obj(obj);
                    return;
                }
                else
                {
                    ch->println("You haven't completed the quest yet, but there is still time!");     
                    return;
                }
                return;
            }
            else if ((ch->questmob > 0 || ch->questobj > 0) && ch->questtimer >0)
            {
                ch->println("You haven't completed the quest yet, but there is still time!");
                return;
            }
        }
        if (ch->nextquest > 0)
        { 
	    sprintf(buf,"But you didn't complete your quest in time!");
	    REMOVE_BIT(ch->dyn, DYN_QUESTING);
	    ch->questgiver = NULL;
            ch->questtimer = 0;
            ch->questmob = 0;
            ch->questobj = 0;
	}
        else sprintf(buf, "You have to REQUEST a quest first, %s.",ch->name);
        act( buf, ch, NULL, questman, TO_CHAR );
        return;
    }

    ch->println("JOBS commands: POINTS INFO TIME REQUEST COMPLETE LIST BUY.");
    ch->println("For more information, type 'HELP JOBS'.");
    return;
}

void generate_quest(char_data *ch, char_data *questman)
{
    char_data *victim;
    ROOM_INDEX_DATA *room;
    OBJ_DATA *questitem;
    char buf [MSL];

    for (victim = char_list; victim != NULL; victim = victim->next)
    {
        if (!IS_NPC(victim)) continue;

        if (quest_level_diff(ch->level, victim->level) == true
            && !IS_SET(victim->imm_flags, IMM_SUMMON)
            && victim->pIndexData != NULL
            && victim->pIndexData->pShop == NULL
            && !IS_SET(victim->act, ACT_PET)
            && !IS_SET(victim->affected_by, AFF_CHARM)
            && chance(15)) break;
    }

    if ( victim == NULL  )
    {
	act( "I'm sorry, but I don't have any quests for you at this time.", ch, NULL, questman, TO_CHAR );
	act( "Try again later.", ch, NULL, questman, TO_CHAR );
        ch->nextquest = 1;
        ch->nextquest = 1;
        return;
    }


//    if ( ( room = find_location( ch, victim->name ) ) == NULL ) 

    if ( (room = get_random_room(ch) ) == NULL ) 
    {
	act( "I'm sorry, but I don't have any quests for you at this time.", ch, NULL, questman, TO_CHAR );
	act( "Try again later.", ch, NULL, questman, TO_CHAR );
        ch->nextquest = 1;
        return;
    }

    if (IS_SET(room->room_flags, ROOM_OOC) || IS_SET(room->area->area_flags, AREA_OLCONLY))
    {
	act( "I'm sorry, but I don't have any quests for you at this time.", ch, NULL, questman, TO_CHAR );
	act( "Try again later.", ch, NULL, questman, TO_CHAR );
        ch->nextquest = 0;
	if(IS_IMMORTAL(ch))
	{
		ch->printlnf("Attempted Area %s & Room %s!",room->area->name, room->name);
	}
        return;
    }

        int objvnum = 0;

        switch(number_range(0,4))
        {
            case 0:
            objvnum = QUEST_OBJQUEST1;
            break;

            case 1:
            objvnum = QUEST_OBJQUEST2;
            break;

            case 2:
            objvnum = QUEST_OBJQUEST3;
            break;

            case 3:
            objvnum = QUEST_OBJQUEST4;
            break;

            case 4:
            objvnum = QUEST_OBJQUEST5;
            break;
        }

        questitem = create_object( get_obj_index(objvnum));
        obj_to_room(questitem, room);
        ch->questobj = questitem->pIndexData->vnum;

        sprintf(buf, "Vile pilferers have stolen %s from the royal treasury!",questitem->short_descr);
	act( buf, ch, NULL, questman, TO_CHAR );
	act( "My court wizardess, with her magic mirror, has pinpointed its location.", ch, NULL, questman, TO_CHAR );
        sprintf(buf, "Look in the general area of %s for %s!",room->area->name, room->name);
	act( buf, ch, NULL, questman, TO_CHAR );
        return;
}

bool quest_level_diff(int clevel, int mlevel)
{
    if (clevel < 6 && mlevel < 8) return true;
    else if (clevel > 6 && clevel < 10 && mlevel < 11) return true;
    else if (clevel > 9 && clevel < 20 && mlevel > 11 && mlevel < 25) return true;
    else if (clevel > 19 && clevel < 30 && mlevel > 19 && mlevel < 35) return true;
    else if (clevel > 29 && clevel  < 40 && mlevel > 29 && mlevel < 45) return true;
    else if (clevel > 39 && clevel  < 50 && mlevel > 39 && mlevel < 55) return true;
    else if (clevel > 49 && clevel  < 60 && mlevel > 49 && mlevel < 65) return true;
    else if (clevel > 59 && clevel  < 70 && mlevel > 59 && mlevel < 75) return true;
    else if (clevel > 69 && clevel  < 80 && mlevel > 69 && mlevel < 85) return true;
    else if (clevel > 79 && clevel  < 93 && mlevel > 79 && mlevel < 100) return true;
    else if (clevel > 50 && mlevel > 50) return true;
    else return false;
}

/* Called from update_handler() by pulse_area */

void quest_update(void)
{
    connection_data *d;
    char_data *ch;

    for ( d = connection_list; d != NULL; d = d->next )
    {
      if (d->character != NULL && d->connected_state == CON_PLAYING)
      {

        ch = d->character;

        if (ch->nextquest > 0)
        {
            ch->nextquest--;
            if (ch->nextquest == 0)
            {
                ch->println("You may now quest again.");
		REMOVE_BIT(ch->dyn, DYN_QUESTING);
                return;
            }
        }
        else if (IS_SET(ch->dyn, DYN_QUESTING))
        {
            if (--ch->questtimer <= 0)
            {
                ch->nextquest = 10;
                ch->printlnf("You have run out of time for your quest!\n\rYou may quest again in %d minutes.\n\r",ch->nextquest);
                REMOVE_BIT(ch->dyn, DYN_QUESTING);
		ch->questgiver = NULL;
                ch->questtimer = 0;
                ch->questmob = 0;
                ch->questobj = 0;
            }
            if (ch->questtimer > 0 && ch->questtimer < 6)
            {
                ch->println("Better hurry, you're almost out of time for your quest!");
                return;
            }
        }
        }
    }
    return;
}

