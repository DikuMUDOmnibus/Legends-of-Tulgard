/**************************************************************************/
// pigeon.cpp - Pigeon Letter Delivery & Archery Code by Ixliam
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/

#include "include.h"
#include "pigeon.h"
#include "ictime.h"

DECLARE_DO_FUN( do_look		);
bool laston_find(char *argument);

/**************************************************************************/
// semilocalized globals
PIGEON_DATA	*pigeon_list;
/**************************************************************************/
// create pigeon GIO lookup table 

GIO_START(PIGEON_DATA)
GIO_STRH(pcname,	"PCName  ")
GIO_STRH(name,		"Name    ")
GIO_STRH(short_descr,	"Shrt    ")
GIO_STRH(scribed_by,	"ScrBy   ")
GIO_STRH(scribed_time,	"ScrTm   ")
GIO_STRH(original_txt,	"OrigT   ")
GIO_STRH(text,		"Text    ")
GIO_INTH(lv0,		"Lv0 ")
GIO_INTH(lv1,		"Lv1 ")
GIO_INTH(lv2,		"Lv2 ")
GIO_INTH(lv3,		"Lv3 ")
GIO_INTH(lv4,		"Lv4 ")

GIO_FINISH_STRDUP_EMPTY
/**************************************************************************/
// loads in the pigeon database
void load_pigeon_db(void)
{
	logf("===Loading pigeon database from %s...", PIGEON_FILE);
	GIOLOAD_LIST(pigeon_list, PIGEON_DATA, PIGEON_FILE); 	
	log_string ("load_pigeon_db(): finished");
}
/**************************************************************************/
// saves the pigeon database
void save_pigeon_db( void)
{
	logf("===save_pigeon_db(): saving pigeon database to %s", PIGEON_FILE);
	GIOSAVE_LIST(pigeon_list, PIGEON_DATA, PIGEON_FILE, true);
}
/**************************************************************************/
// lists pigeons
void do_pigeonlist( char_data *ch, char *)
{
	PIGEON_DATA *node;
	int count;

	ch->titlebar("-=PENDING PIGEON LETTERS=-");

	count=0;
	for (node = pigeon_list; node; node= node->next)
	{
	 	ch->printlnf("`s%d> `MPlayer: `m%s  `MDate: `m%s", ++count, node->pcname, node->scribed_time);
		ch->printlnf("   `CText: `c%s", node->text);
	}
	return;
}
/**************************************************************************/
// lists pigeons
void do_pigeonremove( char_data *ch, char *argument)
{
	PIGEON_DATA *node;
	PIGEON_DATA *prevnode=NULL;
	char arg1[MIL];
	int selected;
	int count;
	
	argument = one_argument(argument, arg1);

	if(is_number(arg1))
		selected = atoi(arg1);
	else{
		ch->println("You need to specify a number.");
		return;
	}

	ch->titlebar("-=PENDING PIGEON LETTERS=-");

	count=0;
	for (node = pigeon_list; node; node= node->next)
	{
		count++;
		if(count == selected)
		{
	 		ch->printlnf("`s%d> `MPlayer: `m%s  `MDate: `m%s", count, node->pcname, node->scribed_time);
			ch->printlnf("   `CText: `c%s", node->text);
			ch->println("DELETED.");
			if(count > 1)
				prevnode->next = node->next;
			else
				pigeon_list = pigeon_list->next;
			delete node;
			save_pigeon_db();
			return;
		}
		prevnode = node;
	}
	ch->println("Letter not found.");
	return;
}
/**************************************************************************************/
void queue_pigeon(char * pcname, char * name, char * short_descr, char * scribed_by, char * scribed_time, char * original_txt, char * text,int lv0, int lv1, int lv2, int lv3, int lv4)
{
	PIGEON_DATA *node = new PIGEON_DATA;
	node->pcname = str_dup(pcname);
	node->name = str_dup(name);
	node->short_descr = str_dup(short_descr);
	node->scribed_by = str_dup(scribed_by);
	node->scribed_time = str_dup(scribed_time);
	node->original_txt = str_dup(original_txt);
	node->text = str_dup(text);
	node->lv0 = lv0;
	node->lv1 = lv1;
	node->lv2 = lv2;
	node->lv3 = lv3;
	node->lv4 = lv4;
	node->next = pigeon_list;
	pigeon_list = node;
	save_pigeon_db();
}
/**************************************************************************/
EXTRA_DESCR_DATA *ed_prefix(EXTRA_DESCR_DATA *ed, char *keyword, char *description);
/**************************************************************************/
void check_pending_pigeon(char_data *ch)
{
	PIGEON_DATA *node=pigeon_list, *prevnode=NULL;
	bool matched=false;
	
	while(node){		
		if(!str_cmp(node->pcname, ch->name) && 
		!IS_SET (ch->in_room->room_flags, ROOM_OOC) &&
		!IS_SET (ch->in_room->room_flags, ROOM_ARENA) &&
		!IS_SET (ch->in_room->room_flags, ROOM_NOWHERE) &&
		(ch->in_room->sector_type != SECT_CAVE) &&
		(ch->in_room->sector_type != SECT_WATER_SWIM) &&
		(ch->in_room->sector_type != SECT_WATER_NOSWIM) &&
		(ch->in_room->sector_type != SECT_UNDERWATER)){

		if(ch->fighting)
			return;

		obj_data *letter;
		letter=create_object(get_obj_index(1486)); // Parchment
		letter->name = node->name;
		letter->short_descr = node->short_descr;
		letter->extra_descr=NULL;
		letter->extra_descr=ed_prefix(letter->extra_descr, "_SCRIBED_BY", node->scribed_by);
		letter->extra_descr=ed_prefix(letter->extra_descr, "_SCRIBED_TIME", node->scribed_time);
		letter->extra_descr=ed_prefix(letter->extra_descr, "_ORIGINAL_TEXT", node->original_txt);
		letter->extra_descr=ed_prefix(letter->extra_descr, "_LANGUAGE_NAME", ch->language->name);
		letter->extra_descr=ed_prefix(letter->extra_descr, "_LANGUAGE_TEXT", node->text);

		letter->value[0] = node->lv0;
		letter->value[1] = node->lv1;
		letter->value[2] = node->lv2;
		letter->value[3] = node->lv3;
		letter->value[4] = node->lv4;

	        obj_to_char( letter, ch );
		act("A large pigeon flies into the room and delivers a message to $n.",ch,NULL,ch,TO_NOTVICT);
    		ch->printlnf("`#`YA large pigeon flies into the room and delivers a message to you.`^");

			free_string(node->pcname);
			if(!prevnode){ // delete the head
				pigeon_list = pigeon_list->next;
			}else{
				prevnode->next=node->next;
			}	
			delete node;
			matched=true;
			
			if(!prevnode){
				node=pigeon_list;
			}else{
				node=prevnode;
			}			
			continue;			
		}		
		prevnode=node;
		node=node->next;
	}

	if(matched){
		save_pigeon_db();
	}
	return;
}
/**************************************************************************/
void do_sendletter(char_data *ch, char *argument)
{
    char_data *mob;
	
    if (ch->position == POS_SLEEPING)
    {
	ch->println("You can't do that while sleeping.");
        return;
    }
	
    // check for letter courier
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
        if ( IS_NPC(mob) && IS_SET(mob->act2, ACT2_LETTER_COURIER) )
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

    if (IS_NULLSTR(argument))
    {
	act("$N says 'I can send a letter to someone by my courier pigeons for 5 gold.'",ch,NULL,mob,TO_CHAR);
	return;
    }

    obj_data *parchment;

    parchment = get_eq_char( ch, WEAR_HOLD );

    if ( !parchment || parchment->item_type != ITEM_PARCHMENT)
    {
	ch->println( "You need to be holding the letter first." );
	return;
    }

    if (parchment->value[2] == 0)
    {
	ch->println( "You need to seal the letter first." );
	return;
    }

    int cost = 500;

    if (cost > (ch->gold * 100 + ch->silver))
    {
	act("$N says 'You don't have 5 gold to send one.'", ch,ch,mob,TO_CHAR);
	return;	
    }

    char_data *victim;
    	
    if (!str_prefix(argument,"tulgard"))
    {
    	deduct_cost(ch,cost);
    	mob->gold += cost;
	limit_mobile_wealth(mob);
    	obj_from_char(parchment);
    	save_char_obj(ch);
    	act("$N says 'I will have this delivered to the town square.'", ch,ch,mob,TO_CHAR);
	obj_to_room( parchment, get_room_index(3000));
	connection_data *d;
   	for ( d = connection_list; d; d = d->next )
    	{
		if ( d->connected_state == CON_PLAYING
		&&   d->character->in_room == get_room_index(3000) )
		{
			if (get_trust(d->character) >= get_trust(ch))
				d->character->print( "local> " );
			
			d->character->printlnf( "`#`YA large pigeon flies into the room and drops a letter.`^" );
		}
    	}
	return;
    }

    if (!str_prefix(argument,"throtyl"))
    {
    	deduct_cost(ch,cost);
    	mob->gold += cost;
	limit_mobile_wealth(mob);
    	obj_from_char(parchment);
    	save_char_obj(ch);
    	act("$N says 'I will have this delivered to the town square.'", ch,ch,mob,TO_CHAR);
	obj_to_room( parchment, get_room_index(27003));
	connection_data *d;
   	for ( d = connection_list; d; d = d->next )
    	{
		if ( d->connected_state == CON_PLAYING
		&&   d->character->in_room == get_room_index(27003) )
		{
			if (get_trust(d->character) >= get_trust(ch))
				d->character->print( "local> " );
			
			d->character->printlnf( "`#`YA large pigeon flies into the room and drops a letter.`^" );
		}
    	}
	return;
    }

    if (!str_prefix(argument,"calontir"))
    {
    	deduct_cost(ch,cost);
    	mob->gold += cost;
	limit_mobile_wealth(mob);
    	obj_from_char(parchment);
    	save_char_obj(ch);
    	act("$N says 'I will have this delivered to the town square.'", ch,ch,mob,TO_CHAR);
	obj_to_room( parchment, get_room_index(800));
	connection_data *d;
   	for ( d = connection_list; d; d = d->next )
    	{
		if ( d->connected_state == CON_PLAYING
		&&   d->character->in_room == get_room_index(800) )
		{
			if (get_trust(d->character) >= get_trust(ch))
				d->character->print( "local> " );
			
			d->character->printlnf( "`#`YA large pigeon flies into the room and drops a letter.`^" );
		}
    	}
	return;
    }

    if ( ( victim = get_char_world( ch, argument ) ) == NULL )
    {
	if (laston_find(argument) == false)
	{
		act("$N says 'I have never heard of that person before.'", ch,ch,mob,TO_CHAR);
		return;
	}
    	deduct_cost(ch,cost);
    	mob->gold += cost;
	limit_mobile_wealth(mob);

	char *scribed_by, *scribed_time, *original_txt, *text;
	scribed_by=get_extra_descr("_SCRIBED_BY", parchment->extra_descr);
	scribed_time=get_extra_descr("_SCRIBED_TIME", parchment->extra_descr);
	original_txt=get_extra_descr("_ORIGINAL_TEXT", parchment->extra_descr);
	text=get_extra_descr("_LANGUAGE_TEXT", parchment->extra_descr);

	queue_pigeon(argument, parchment->name, parchment->short_descr, scribed_by, scribed_time, original_txt, text, parchment->value[0],  parchment->value[1], parchment->value[2], parchment->value[3], parchment->value[4]);
    	obj_from_char(parchment);
	extract_obj(parchment);
    	save_char_obj(ch);
    	act("$N says 'It may be a bit before they receive your letter.'", ch,ch,mob,TO_CHAR);
	return;
    }

    if ( victim == ch )
    {
    	act("$N says 'Why not just open it yourself ? Don't be stupid.'", ch,ch,mob,TO_CHAR);
	return;
    }

    if (IS_SET (victim->in_room->room_flags, ROOM_OOC) || IS_SET (victim->in_room->room_flags, ROOM_ARENA) ||
	IS_SET (victim->in_room->room_flags, ROOM_NOWHERE) || (victim->in_room->sector_type == SECT_CAVE) ||
	(victim->in_room->sector_type == SECT_WATER_SWIM) || (victim->in_room->sector_type == SECT_WATER_NOSWIM) ||
	(victim->in_room->sector_type == SECT_UNDERWATER))
    {
    	deduct_cost(ch,cost);
    	mob->gold += cost;
	limit_mobile_wealth(mob);

	char *scribed_by, *scribed_time, *original_txt, *text;
	scribed_by=get_extra_descr("_SCRIBED_BY", parchment->extra_descr);
	scribed_time=get_extra_descr("_SCRIBED_TIME", parchment->extra_descr);
	original_txt=get_extra_descr("_ORIGINAL_TEXT", parchment->extra_descr);
	text=get_extra_descr("_LANGUAGE_TEXT", parchment->extra_descr);

	queue_pigeon(argument, parchment->name, parchment->short_descr, scribed_by, scribed_time, original_txt, text, parchment->value[0],  parchment->value[1], parchment->value[2], parchment->value[3], parchment->value[4]);
    	obj_from_char(parchment);
	extract_obj(parchment);
    	save_char_obj(ch);
    	act("$N says 'It may be a bit before they receive your letter.'", ch,ch,mob,TO_CHAR);
	return;
    }


    if (IS_IMMORTAL(victim))
    {
    	act("$N says 'Don't bother the immortals with your nonsense.'", ch,ch,mob,TO_CHAR);
	return;
    }	

    if (IS_NPC(victim))
    {
    	act("$N says 'They don't want to talk to you.'", ch,ch,mob,TO_CHAR);
	return;
    }	

    deduct_cost(ch,cost);
    mob->gold += cost;
    limit_mobile_wealth(mob);
    obj_from_char(parchment);
    obj_to_char(parchment, victim);
    save_char_obj(ch);
    save_char_obj(victim);
    act("$N says 'I will send that right out. Nice doing business with you.'", ch,ch,mob,TO_CHAR);
    act("A large pigeon flies off to deliver your messaage.",ch,NULL,victim,TO_CHAR);
    act("A large pigeon flies into the room and delivers a message to $n.",victim,NULL,victim,TO_NOTVICT);
    act("A large pigeon flies into the room and delivers a message to you.",ch,NULL,victim,TO_VICT);
    return;
}
/**************************************************************************************/
void do_checkletter(char_data *ch, char *argument)
{
	
	PIGEON_DATA *node=pigeon_list, *prevnode=NULL;
	bool matched=false;
	
	while(node){		
		if(!str_cmp(node->pcname, ch->name) && 
		!IS_SET (ch->in_room->room_flags, ROOM_OOC) &&
		!IS_SET (ch->in_room->room_flags, ROOM_ARENA) &&
		!IS_SET (ch->in_room->room_flags, ROOM_NOWHERE) &&
		(ch->in_room->sector_type != SECT_CAVE) &&
		(ch->in_room->sector_type != SECT_WATER_SWIM) &&
		(ch->in_room->sector_type != SECT_WATER_NOSWIM) &&
		(ch->in_room->sector_type != SECT_UNDERWATER)){

		obj_data *letter;
		letter=create_object(get_obj_index(1486)); // Parchment
		letter->name = node->name;
		letter->short_descr = node->short_descr;
		letter->extra_descr=NULL;
		letter->extra_descr=ed_prefix(letter->extra_descr, "_SCRIBED_BY", node->scribed_by);
		letter->extra_descr=ed_prefix(letter->extra_descr, "_SCRIBED_TIME", node->scribed_time);
		letter->extra_descr=ed_prefix(letter->extra_descr, "_ORIGINAL_TEXT", node->original_txt);
		letter->extra_descr=ed_prefix(letter->extra_descr, "_LANGUAGE_NAME", ch->language->name);
		letter->extra_descr=ed_prefix(letter->extra_descr, "_LANGUAGE_TEXT", node->text);

		letter->value[0] = node->lv0;
		letter->value[1] = node->lv1;
		letter->value[2] = node->lv2;
		letter->value[3] = node->lv3;
		letter->value[4] = node->lv4;

	        obj_to_char( letter, ch );
		act("A large pigeon flies into the room and delivers a message to $n.",ch,NULL,ch,TO_NOTVICT);
    		ch->printlnf("`#`YA large pigeon flies into the room and delivers a message to you.`^");

			free_string(node->pcname);
			if(!prevnode){ // delete the head
				pigeon_list = pigeon_list->next;
			}else{
				prevnode->next=node->next;
			}	
			delete node;
			matched=true;
			
			if(!prevnode){
				node=pigeon_list;
			}else{
				node=prevnode;
			}			
			continue;			
		}		
		prevnode=node;
		node=node->next;
	}

	if(matched)
	{
		save_pigeon_db();
		return;
	}
	else
	{
		ch->println("There appears to be no deliveries for you at this time.");
		return;
	}
}
/**************************************************************************/
OBJ_DATA *find_quiver( char_data *ch )
{
    OBJ_DATA *obj;

    for( obj = ch->carrying; obj; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj ) )
	{
	   if ( obj->item_type == ITEM_QUIVER && !IS_SET(obj->value[1], CONT_CLOSED) )
		return obj;
	}
    }
    return NULL;
}

OBJ_DATA *find_projectile( char_data *ch, OBJ_DATA *quiver )
{
    OBJ_DATA *obj;

    for( obj = quiver->contains; obj; obj = obj->next_content )
    {
	if( can_see_obj( ch, obj ) )
	{
	   if( obj->item_type == ITEM_PROJECTILE )
	   	return obj;
	}
    }
    return NULL;
}
/**************************************************************************/
void do_qdraw( char_data *ch, char *argument )
{  
    OBJ_DATA *bow;
    OBJ_DATA *arrow;
    OBJ_DATA *quiver;
	
	bow = get_eq_char( ch, WEAR_WIELD );

    if (IS_OOC(ch))
    {
	do_huh(ch,"");
	return;
    }

	if ( !bow )
	{
		ch->println("You must be wielding a missile weapon.");
		return;
	}

	if ( bow->value[0]!=WEAPON_BOW && 
	     bow->value[0]!=WEAPON_BLOWGUN && 
	     bow->value[0]!=WEAPON_SLING)
	{
		ch->println("You must be wielding a missile weapon.");
		return;
	}

    if( ( quiver = find_quiver( ch ) ) == NULL )
    {
	ch->println( "You aren't wearing a quiver where you can get to it!");
	return;
    }

    if( get_eq_char( ch, WEAR_HOLD ) != NULL )
    {
	 ch->println( "Your hand is not empty!");
	 return;
    }

    if( ( arrow = find_projectile( ch, quiver ) ) == NULL )
    {
	ch->println( "Your quiver is empty!!");
	return;
    }

    char buf[MSL];

    WAIT_STATE( ch, PULSE_VIOLENCE );
    sprintf(buf, "$n draws %s from $p.", arrow->short_descr);
    act(buf, ch, quiver, NULL, TO_ROOM );
    ch->printlnf ("You draw %s from %s.", arrow->short_descr, quiver->short_descr );

    obj_from_obj( arrow );
    obj_to_char( arrow, ch );
    wear_obj( ch, arrow, true, false );
    return;
}

/**************************************************************************/
void do_fire( char_data *ch, char *argument )
{
	char		arg[MIL];
	char_data	*victim = NULL;
    	OBJ_DATA 	*arrow = NULL;
    	OBJ_DATA 	*bow = NULL;
	int		dam, door, chance;
    	ROOM_INDEX_DATA *was_in_room;
    	EXIT_DATA 	*pexit;

	bow = get_eq_char( ch, WEAR_WIELD );

	if ( !bow )
	{
		ch->println("You must be wielding a missile weapon.");
		return;
	}

    if (IS_OOC(ch))
    {
	do_huh(ch,"");
	return;
    }

	if ( bow->value[0]!=WEAPON_BOW && 
	     bow->value[0]!=WEAPON_CROSSBOW &&
	     bow->value[0]!=WEAPON_BLOWGUN && 
	     bow->value[0]!=WEAPON_SLING)
	{
		ch->println("You must be wielding a missile weapon.");
		return;
	}

	if ( bow->value[0] == WEAPON_BOW)
	{
		chance = get_skill( ch, gsn_bow );
		if ( chance < 1)
		{
			do_huh(ch,"");
			return;
		}
	}

    if ( bow->value[0] == WEAPON_BLOWGUN)
    {
	chance = get_skill( ch, gsn_blowgun );
	if ( chance < 1)
	{
		do_huh(ch,"");
		return;
	}
    }

    if ( bow->value[0] == WEAPON_SLING)
    {
	chance = get_skill( ch, gsn_sling );
	if ( chance < 1)
	{
		do_huh(ch,"");
		return;
	}
    }

    if( ( arrow = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
    {
        ch->println( "You are not holding a projectile!");
        return;
    }

    if( arrow->item_type != ITEM_PROJECTILE )
    {
        ch->println( "You are not holding a projectile!");
        return;
    }

    if ( !can_drop_obj( ch, arrow ))
    {
		ch->println("You can't let go of that.");
		return;
    }

	one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        if ( ch->fighting )
        {
            victim = ch->fighting;
        }
        else
        {
            ch->println( "Fire at whom or what?");
            return;
        }
    }
    else
    {
	/* See if who you are trying to shoot at is nearby... */
        if ( ( victim = get_char_room ( ch, arg ) ) == NULL)
        {
            was_in_room = ch->in_room;

            for( door=0 ; door < MAX_DIR && victim==NULL ; door++ )
             { 
                if ( (  pexit = was_in_room->exit[door] ) != NULL
                   &&   pexit->u1.to_room != NULL
                   &&   pexit->u1.to_room != was_in_room 
                   &&   !strcmp( pexit->u1.to_room->area->name ,
                                was_in_room->area->name ) )
                   { 
                     ch->in_room = pexit->u1.to_room;
                     victim = get_char_room ( ch, arg ); 
                    }
               
              }

            ch->in_room = was_in_room;

            if(victim==NULL)
              {
               ch->println( "You can't seem to locate your target.");
               return;
              }
            else
              {  if(IS_SET(pexit->exit_info,EX_CLOSED))
                    { ch->println("You can't fire through a door.");
                      return;
                     } 
             }
        }
    	if((ch->in_room) == (victim->in_room))
    	{
		ch->println("Don't you think that standing a bit further away would be wise?");
		return;
    	}

 	if(ch == victim)
 	{
     	   ch->println("Yeah, right.");
      	   return;
    	}

	if ( is_safe( ch, victim ))
	{
     	   	ch->println("You cannot seem to target them for some reason.");
		return;
	}

	if(IS_NPC(victim) && (IS_SET(victim->act, ACT_NO_HUNT) ||
	   IS_SET(victim->act, ACT_DONT_WANDER)) )
	{
     	   	ch->println("You cannot seem to target them for some reason.");
		return;
	}

    	if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim ) 
	{
		act("But $N is your friend!",ch,NULL,victim,TO_CHAR);
		return;
    	}
    }

      switch( arrow->value[0] )
      {
        case PROJ_BOLT:
		if(bow->value[0] != WEAPON_CROSSBOW)
		{
			ch->println("You need to be holding a bolt to use with that.");
			return;
		}
		break;
        case PROJ_ARROW:
		if(bow->value[0] != WEAPON_BOW)
		{
			ch->println("You need to be holding an arrow to use with that.");
			return;
		}
		break;
        case PROJ_DART:
		if(bow->value[0] != WEAPON_BLOWGUN)
		{
			ch->println("You need to be holding a dart to use with that.");
			return;
		}
		break;
        case PROJ_STONE:
		if(bow->value[0] != WEAPON_SLING)
		{
			ch->println("You need to be holding a stone to use with that.");
			return;
		}
		break;
      }


    /* Lag the bowman... */
    WAIT_STATE( ch, 2 * PULSE_VIOLENCE ); 

     /* Fire the weapon */
    if(arrow->value[0] == PROJ_ARROW )
    {
	act( "$n fire $p at $N.",  ch, arrow, victim, TO_NOTVICT );
	act( "You fire $p at $N.",  ch, arrow, victim, TO_CHAR );
	act( "$n fires $p at you.", ch, arrow, victim, TO_VICT );

	if (( ch->level + 5 ) < arrow->level
	||  number_percent() >= 10 + get_skill( ch, gsn_bow ) * 4/5 )
	{
		act( "$p misses $N and lands on the ground.", ch, arrow, victim, TO_CHAR );
		act( "$n fires $p but it misses and lands on the ground.", ch, arrow, NULL, TO_ROOM );
		act( "$p flies into the room and lands on the ground.", victim, arrow, NULL, TO_ROOM );
		obj_from_char( arrow );
		obj_to_room( arrow, ch->in_room );
		check_improve( ch, gsn_bow, false, 1 );
	    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
	    {
			act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
			extract_obj( arrow );
		}
	}
	else
	{
		if(arrow->value[3] != -1)
		{
			obj_cast_spell( arrow->value[3], arrow->level, ch, victim, arrow );
			extract_obj(arrow);
			return;
		}		
		chance = number_range(1,10);
		switch (chance)
		{
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				obj_from_char( arrow );
				dam = dice( arrow->value[1], arrow->value[2] );
				if(IS_WEREBEAST(victim) && !str_cmp( arrow->pIndexData->material, "silver" ))
				{
					dam += dam/3;
				}
				if(IS_VAMPIRE(victim) && !str_cmp( arrow->pIndexData->material, "wood" ))
				{
					dam += dam/3;
				}
				
				if (damage( ch, victim, dam, gsn_bow, DAM_PIERCE, true ))
				{
					act( "$p flies into the room and hits $n.", victim, arrow, NULL, TO_ROOM );
	
					if ( get_eq_char( victim, WEAR_LODGED_ARM ) != NULL ) {
						obj_to_room( arrow, ch->in_room );
						act( "$p doesn't lodge into your arm.", ch, arrow, victim, TO_VICT );
						act( "$p hurts $N but doesn't succesfully lodge.", ch, arrow, victim, TO_CHAR );
					    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
						{
							act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
							extract_obj( arrow );
						}
					}
					else
					{
						obj_to_char( arrow, victim );
						SET_BIT( arrow->extra_flags, OBJEXTRA_LODGED );
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_LODGED_ARM;
						wear_obj( victim, arrow, true, false);
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_WIELD;
					}
				}
				else
				{
					obj_to_room( arrow, ch->in_room );
				    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
					{
						act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
						extract_obj( arrow );
					}
				}
				check_improve( ch, gsn_bow, true, 1 );
				break;
			case 6:
			case 7:
			case 8:
				obj_from_char( arrow );
				dam = 3 * ( dice( arrow->value[1], arrow->value[2] )/2);
				if(IS_WEREBEAST(victim) && !str_cmp( arrow->pIndexData->material, "silver" ))
				{
					dam += dam/3;
				}
				if(IS_VAMPIRE(victim) && !str_cmp( arrow->pIndexData->material, "wood" ))
				{
					dam += dam/3;
				}
				if ( damage( ch, victim, dam, gsn_bow, DAM_PIERCE, true ))
				{
					act( "$p flies into the room and hits $n.", victim, arrow, NULL, TO_ROOM );

					if ( get_eq_char( victim, WEAR_LODGED_LEG ) != NULL ) {
						obj_to_room( arrow, ch->in_room );
						act( "$p doesn't lodge into your leg.", ch, arrow, victim, TO_VICT );
						act( "$p hurts $N but doesn't succesfully lodge.", ch, arrow, victim, TO_CHAR );
					    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
						{
							act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
							extract_obj( arrow );
						}
					}
					else
					{
						obj_to_char( arrow, victim );
						SET_BIT( arrow->extra_flags, OBJEXTRA_LODGED );
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_LODGED_LEG;
						wear_obj( victim, arrow, true, false);
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_WIELD;
					}
				}
				else
				{
					obj_to_room( arrow, ch->in_room );
				    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
					{
						act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
						extract_obj( arrow );
					}
				}
				check_improve( ch, gsn_bow, true, 1 );
				break;
			case 9:
			case 10:
				obj_from_char( arrow );
				dam = 2 * ( dice( arrow->value[1], arrow->value[2] ));
				if(IS_WEREWOLF(victim) && !str_cmp( arrow->pIndexData->material, "silver" ))
				{
					dam += dam/3;
				}
				if(IS_VAMPIRE(victim) && !str_cmp( arrow->pIndexData->material, "wood" ))
				{
					dam += dam/3;
				}
				if ( damage( ch, victim, dam, gsn_bow, DAM_PIERCE, true ))
				{
					act( "$p flies into the room and hits $n.", victim, arrow, NULL, TO_ROOM );

					if ( get_eq_char( victim, WEAR_LODGED_RIB ) != NULL ) {
						obj_to_room( arrow, ch->in_room );
						act( "$p doesn't lodge into your rib.", ch, arrow, victim, TO_VICT );
						act( "$p hurts $N but doesn't succesfully lodge.", ch, arrow, victim, TO_CHAR );
					    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
						{
							act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
							extract_obj( arrow );
						}
					}
					else
					{
						obj_to_char( arrow, victim );
						SET_BIT( arrow->extra_flags, OBJEXTRA_LODGED );
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_LODGED_RIB;
						wear_obj( victim, arrow, true, false);
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_WIELD;
					}
				}
				else
				{
					obj_to_room( arrow, ch->in_room );
				    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
					{
						act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
						extract_obj( arrow );
					}
				}
				check_improve( ch, gsn_bow, true, 1 );
				break;
		}
	}

	return;
    }



    if(arrow->value[0] == PROJ_BOLT )
    {
	act( "$n fire $p at $N.",  ch, arrow, victim, TO_NOTVICT );
	act( "You fire $p at $N.",  ch, arrow, victim, TO_CHAR );
	act( "$n fires $p at you.", ch, arrow, victim, TO_VICT );

	if (( ch->level + 5 ) < arrow->level
	||  number_percent() >= 10 + get_skill( ch, gsn_bow ) * 4/5 )
	{
		act( "$p misses $N and lands on the ground.", ch, arrow, victim, TO_CHAR );
		act( "$n fires $p but it misses and lands on the ground.", ch, arrow, NULL, TO_ROOM );
		act( "$p flies into the room and lands on the ground.", victim, arrow, NULL, TO_ROOM );
		obj_from_char( arrow );
		obj_to_room( arrow, ch->in_room );
		check_improve( ch, gsn_bow, false, 1 );
	    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
	    {
			act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
			extract_obj( arrow );
		}
	}
	else
	{

		if(arrow->value[3] != -1)
		{
			obj_cast_spell( arrow->value[3], arrow->level, ch, victim, arrow );
			extract_obj(arrow);
			return;
		}
		chance = number_range(1,10);
		switch (chance)
		{
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				obj_from_char( arrow );
				dam = dice( arrow->value[1], arrow->value[2] );
				if(IS_WEREBEAST(victim) && !str_cmp( arrow->pIndexData->material, "silver" ))
				{
					dam += dam/3;
				}
				if (damage( ch, victim, dam, gsn_bow, DAM_PIERCE, true ))
				{
					act( "$p flies into the room and hits $n.", victim, arrow, NULL, TO_ROOM );
	
					if ( get_eq_char( victim, WEAR_LODGED_ARM ) != NULL ) {
						obj_to_room( arrow, ch->in_room );
						act( "$p doesn't lodge into your arm.", ch, arrow, victim, TO_VICT );
						act( "$p hurts $N but doesn't succesfully lodge.", ch, arrow, victim, TO_CHAR );
					    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
						{
							act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
							extract_obj( arrow );
						}
					}
					else
					{
						obj_to_char( arrow, victim );
						SET_BIT( arrow->extra_flags, OBJEXTRA_LODGED );
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_LODGED_ARM;
						wear_obj( victim, arrow, true, false);
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_WIELD;
					}
				}
				else
				{
					obj_to_room( arrow, ch->in_room );
				    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
					{
						act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
						extract_obj( arrow );
					}
				}
				check_improve( ch, gsn_bow, true, 1 );
				break;
			case 6:
			case 7:
			case 8:
				obj_from_char( arrow );
				dam = 3 * ( dice( arrow->value[1], arrow->value[2] )/2);
				if(IS_WEREBEAST(victim) && !str_cmp( arrow->pIndexData->material, "silver" ))
				{
					dam += dam/3;
				}
				if ( damage( ch, victim, dam, gsn_bow, DAM_PIERCE, true ))
				{
					act( "$p flies into the room and hits $n.", victim, arrow, NULL, TO_ROOM );

					if ( get_eq_char( victim, WEAR_LODGED_LEG ) != NULL ) {
						obj_to_room( arrow, ch->in_room );
						act( "$p doesn't lodge into your leg.", ch, arrow, victim, TO_VICT );
						act( "$p hurts $N but doesn't succesfully lodge.", ch, arrow, victim, TO_CHAR );
					    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
						{
							act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
							extract_obj( arrow );
						}
					}
					else
					{
						obj_to_char( arrow, victim );
						SET_BIT( arrow->extra_flags, OBJEXTRA_LODGED );
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_LODGED_LEG;
						wear_obj( victim, arrow, true, false);
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_WIELD;
					}
				}
				else
				{
					obj_to_room( arrow, ch->in_room );
				    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
					{
						act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
						extract_obj( arrow );
					}
				}
				check_improve( ch, gsn_bow, true, 1 );
				break;
			case 9:
			case 10:
				obj_from_char( arrow );
				dam = 2 * ( dice( arrow->value[1], arrow->value[2] ));
				if(IS_WEREBEAST(victim) && !str_cmp( arrow->pIndexData->material, "silver" ))
				{
					dam += dam/3;
				}
				if(IS_VAMPIRE(victim) && !str_cmp( arrow->pIndexData->material, "wood" ))
				{
					dam += dam/3;
				}
				if ( damage( ch, victim, dam, gsn_bow, DAM_PIERCE, true ))
				{
					act( "$p flies into the room and hits $n.", victim, arrow, NULL, TO_ROOM );

					if ( get_eq_char( victim, WEAR_LODGED_RIB ) != NULL ) {
						obj_to_room( arrow, ch->in_room );
						act( "$p doesn't lodge into your rib.", ch, arrow, victim, TO_VICT );
						act( "$p hurts $N but doesn't succesfully lodge.", ch, arrow, victim, TO_CHAR );
					    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
						{
							act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
							extract_obj( arrow );
						}
					}
					else
					{
						obj_to_char( arrow, victim );
						SET_BIT( arrow->extra_flags, OBJEXTRA_LODGED );
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_LODGED_RIB;
						wear_obj( victim, arrow, true, false);
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_WIELD;
					}
				}
				else
				{
					obj_to_room( arrow, ch->in_room );
				    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
					{
						act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
						extract_obj( arrow );
					}
				}
				check_improve( ch, gsn_bow, true, 1 );
				break;
		}
	}

	return;
    }


    if(arrow->value[0] == PROJ_DART )
    {
	act( "$n fire $p at $N.",  ch, arrow, victim, TO_NOTVICT );
	act( "You fire $p at $N.",  ch, arrow, victim, TO_CHAR );
	act( "$n fires $p at you.", ch, arrow, victim, TO_VICT );

	if (( ch->level + 5 ) < arrow->level
	||  number_percent() >= 10 + get_skill( ch, gsn_blowgun ) * 4/5 )
	{
		act( "$p misses $N and lands on the ground.", ch, arrow, victim, TO_CHAR );
		act( "$n fires $p but it misses and lands on the ground.", ch, arrow, NULL, TO_ROOM );
		act( "$p flies into the room and lands on the ground.", victim, arrow, NULL, TO_ROOM );
		obj_from_char( arrow );
		obj_to_room( arrow, ch->in_room );
		check_improve( ch, gsn_blowgun, false, 1 );
	    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
	    {
			act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
			extract_obj( arrow );
		}
	}
	else
	{

		if(arrow->value[3] != -1)
		{
			obj_cast_spell( arrow->value[3], arrow->level, ch, victim, arrow );
			extract_obj(arrow);
			return;
		}
		chance = number_range(1,10);
		switch (chance)
		{
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				obj_from_char( arrow );
				dam = dice( arrow->value[1], arrow->value[2] );
				if(IS_WEREBEAST(victim) && !str_cmp( arrow->pIndexData->material, "silver" ))
				{
					dam += dam/3;
				}
				if (damage( ch, victim, dam, gsn_blowgun, DAM_PIERCE, true ))
				{
					act( "$p flies into the room and hits $n.", victim, arrow, NULL, TO_ROOM );
	
					if ( get_eq_char( victim, WEAR_LODGED_ARM ) != NULL ) {
						obj_to_room( arrow, ch->in_room );
						act( "$p doesn't lodge into your arm.", ch, arrow, victim, TO_VICT );
						act( "$p hurts $N but doesn't succesfully lodge.", ch, arrow, victim, TO_CHAR );
					    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
						{
							act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
							extract_obj( arrow );
						}
					}
					else
					{
						obj_to_char( arrow, victim );
						SET_BIT( arrow->extra_flags, OBJEXTRA_LODGED );
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_LODGED_ARM;
						wear_obj( victim, arrow, true, false);
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_WIELD;
					}
				}
				else
				{
					obj_to_room( arrow, ch->in_room );
				    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
					{
						act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
						extract_obj( arrow );
					}
				}
				check_improve( ch, gsn_blowgun, true, 1 );
				break;
			case 6:
			case 7:
			case 8:
				obj_from_char( arrow );
				dam = 3 * ( dice( arrow->value[1], arrow->value[2] )/2);
				if(IS_WEREBEAST(victim) && !str_cmp( arrow->pIndexData->material, "silver" ))
				{
					dam += dam/3;
				}
				if ( damage( ch, victim, dam, gsn_blowgun, DAM_PIERCE, true ))
				{
					act( "$p flies into the room and hits $n.", victim, arrow, NULL, TO_ROOM );

					if ( get_eq_char( victim, WEAR_LODGED_LEG ) != NULL ) {
						obj_to_room( arrow, ch->in_room );
						act( "$p doesn't lodge into your leg.", ch, arrow, victim, TO_VICT );
						act( "$p hurts $N but doesn't succesfully lodge.", ch, arrow, victim, TO_CHAR );
					    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
						{
							act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
							extract_obj( arrow );
						}
					}
					else
					{
						obj_to_char( arrow, victim );
						SET_BIT( arrow->extra_flags, OBJEXTRA_LODGED );
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_LODGED_LEG;
						wear_obj( victim, arrow, true, false);
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_WIELD;
					}
				}
				else
				{
					obj_to_room( arrow, ch->in_room );
				    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
					{
						act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
						extract_obj( arrow );
					}
				}
				check_improve( ch, gsn_blowgun, true, 1 );
				break;
			case 9:
			case 10:
				obj_from_char( arrow );
				dam = 2 * ( dice( arrow->value[1], arrow->value[2] ));
				if(IS_WEREBEAST(victim) && !str_cmp( arrow->pIndexData->material, "silver" ))
				{
					dam += dam/3;
				}
				if(IS_VAMPIRE(victim) && !str_cmp( arrow->pIndexData->material, "wood" ))
				{
					dam += dam/3;
				}
				if ( damage( ch, victim, dam, gsn_blowgun, DAM_PIERCE, true ))
				{
					act( "$p flies into the room and hits $n.", victim, arrow, NULL, TO_ROOM );

					if ( get_eq_char( victim, WEAR_LODGED_RIB ) != NULL ) {
						obj_to_room( arrow, ch->in_room );
						act( "$p doesn't lodge into your rib.", ch, arrow, victim, TO_VICT );
						act( "$p hurts $N but doesn't succesfully lodge.", ch, arrow, victim, TO_CHAR );
					    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
						{
							act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
							extract_obj( arrow );
						}
					}
					else
					{
						obj_to_char( arrow, victim );
						SET_BIT( arrow->extra_flags, OBJEXTRA_LODGED );
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_LODGED_RIB;
						wear_obj( victim, arrow, true, false);
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_WIELD;
					}
				}
				else
				{
					obj_to_room( arrow, ch->in_room );
				    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
					{
						act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
						extract_obj( arrow );
					}
				}
				check_improve( ch, gsn_blowgun, true, 1 );
				break;
		}
	}

	return;
    }


    if(arrow->value[0] == PROJ_STONE )
    {
	act( "$n flings $p at $N.",  ch, arrow, victim, TO_NOTVICT );
	act( "You fling $p at $N.",  ch, arrow, victim, TO_CHAR );
	act( "$n flings $p at you.", ch, arrow, victim, TO_VICT );

	if (( ch->level + 5 ) < arrow->level
	||  number_percent() >= 10 + get_skill( ch, gsn_sling ) * 4/5 )
	{
		act( "$p misses $N and lands on the ground.", ch, arrow, victim, TO_CHAR );
		act( "$n flings $p but it misses and lands on the ground.", ch, arrow, NULL, TO_ROOM );
		act( "$p flies into the room and lands on the ground.", victim, arrow, NULL, TO_ROOM );
		obj_from_char( arrow );
		obj_to_room( arrow, ch->in_room );
		check_improve( ch, gsn_sling, false, 1 );
	    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
	    {
			act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
			extract_obj( arrow );
		}
	}
	else
	{
		if(arrow->value[3] != -1)
		{
			obj_cast_spell( arrow->value[3], arrow->level, ch, victim, arrow );
			extract_obj(arrow);
			return;
		}
		chance = number_range(1,10);
		switch (chance)
		{
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				obj_from_char( arrow );
				dam = dice( arrow->value[1], arrow->value[2] );
				if (damage( ch, victim, dam, gsn_sling, DAM_PIERCE, true ))
				{
					act( "$p flies into the room and hits $n.", victim, arrow, NULL, TO_ROOM );
	
					if ( get_eq_char( victim, WEAR_LODGED_ARM ) != NULL ) {
						obj_to_room( arrow, ch->in_room );
						act( "$p doesn't lodge into your arm.", ch, arrow, victim, TO_VICT );
						act( "$p hurts $N but doesn't succesfully lodge.", ch, arrow, victim, TO_CHAR );
					    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
						{
							act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
							extract_obj( arrow );
						}
					}
					else
					{
						obj_to_char( arrow, victim );
						SET_BIT( arrow->extra_flags, OBJEXTRA_LODGED );
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_LODGED_ARM;
						wear_obj( victim, arrow, true, false);
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_WIELD;
					}
				}
				else
				{
					obj_to_room( arrow, ch->in_room );
				    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
					{
						act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
						extract_obj( arrow );
					}
				}
				check_improve( ch, gsn_sling, true, 1 );
				break;
			case 6:
			case 7:
			case 8:
				obj_from_char( arrow );
				dam = 3 * ( dice( arrow->value[1], arrow->value[2] )/2);
				if ( damage( ch, victim, dam, gsn_sling, DAM_PIERCE, true ))
				{
					act( "$p flies into the room and hits $n.", victim, arrow, NULL, TO_ROOM );

					if ( get_eq_char( victim, WEAR_LODGED_LEG ) != NULL ) {
						obj_to_room( arrow, ch->in_room );
						act( "$p doesn't lodge into your leg.", ch, arrow, victim, TO_VICT );
						act( "$p hurts $N but doesn't succesfully lodge.", ch, arrow, victim, TO_CHAR );
					    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
						{
							act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
							extract_obj( arrow );
						}
					}
					else
					{
						obj_to_char( arrow, victim );
						SET_BIT( arrow->extra_flags, OBJEXTRA_LODGED );
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_LODGED_LEG;
						wear_obj( victim, arrow, true, false);
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_WIELD;
					}
				}
				else
				{
					obj_to_room( arrow, ch->in_room );
				    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
					{
						act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
						extract_obj( arrow );
					}
				}
				check_improve( ch, gsn_sling, true, 1 );
				break;
			case 9:
			case 10:
				obj_from_char( arrow );
				dam = 2 * ( dice( arrow->value[1], arrow->value[2] ));
				if ( damage( ch, victim, dam, gsn_sling, DAM_PIERCE, true ))
				{
					act( "$p flies into the room and hits $n.", victim, arrow, NULL, TO_ROOM );

					if ( get_eq_char( victim, WEAR_LODGED_RIB ) != NULL ) {
						obj_to_room( arrow, ch->in_room );
						act( "$p doesn't lodge into your rib.", ch, arrow, victim, TO_VICT );
						act( "$p hurts $N but doesn't succesfully lodge.", ch, arrow, victim, TO_CHAR );
					    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
						{
							act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
							extract_obj( arrow );
						}
					}
					else
					{
						obj_to_char( arrow, victim );
						SET_BIT( arrow->extra_flags, OBJEXTRA_LODGED );
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_LODGED_RIB;
						wear_obj( victim, arrow, true, false);
						arrow->wear_flags = OBJWEAR_TAKE | OBJWEAR_WIELD;
					}
				}
				else
				{
					obj_to_room( arrow, ch->in_room );
				    if ( IS_OBJ_STAT( arrow, OBJEXTRA_MELT_DROP ))
					{
						act( "$p dissolves into smoke.", ch, arrow, NULL, TO_ROOM );
						extract_obj( arrow );
					}
				}

				check_improve( ch, gsn_sling, true, 1 );
				break;
	
		}
	}

	return;
    }

}
/**************************************************************************/
void do_pregnant(char_data *ch, char *argument)
{
	char arg[MIL], buf[MSL];
	char_data *victim;
	
	one_argument( argument, arg );
	
	if(!IS_IMMORTAL(ch)) 
	{
		do_huh(ch, "");
		return;
	}
	
    if ( arg[0] == '\0' )
    {
		ch->println( "Pregnant who ?" );
		return;
    }
	
   	if((victim = get_char_world(ch,arg)) == NULL)
	{ 
		ch->printlnf("There is no player called '%s' here.", arg);
		return; 
	}
	
    	if ( IS_NPC(victim) )
    	{
		ch->println( "Not on NPC's." );
		return;
    	}

	if ( victim==ch){
		ch->println( "Yeah, right." );
		return;
	}

	if ( victim->sex != SEX_FEMALE)
	{
		ch->println( "They are not female." );
		return;
	}
	
	if(IS_AFFECTED2(victim, AFF2_PREGNANT))
	{
		ch->printlnf("%s is no longer pregnant.", victim->name);
		REMOVE_BIT(victim->affected_by2, AFF2_PREGNANT);
		victim->println("You are no longer pregnant.");
		victim->pcdata->pregnant = current_time;
		return;
	}	

	victim->println("`#`CYou suddenly realize you are pregnant. `YCongratulations!!`^");
	sprintf(buf,"%s had flagged %s with pregnancy.", ch->name, victim->name);
	logf("%s had flagged %s with pregnancy.", ch->name, victim->name);
	wiznet(buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
	autonote(NOTE_SNOTE, "Pregnant","Pregnancy", "imm", buf, true);
	SET_BIT(victim->affected_by2, AFF2_PREGNANT);
	victim->pcdata->pregnant = current_time+ (9 * ICTIME_IRLSECS_PER_MONTH);
	victim->printlnf("You will give birth on %s (%.24s)", 
		get_shorticdate_from_time(victim->pcdata->pregnant, "", 0),
		ctime(&victim->pcdata->pregnant));
	return;
}
/**************************************************************************/
void do_connect( char_data *ch, char *argument )
{
  OBJ_DATA *first_ob, *second_ob, *new_ob;
  char arg1[MSL], arg2[MSL];

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( arg1[0] == '\0' || arg2[0] == '\0' )
  {
    ch->println( "`#`BSyntax: `wconnect <firstobj> <secondobj>." );
    return;
  }

  if ( ( first_ob = get_obj_carry( ch, arg1 ) ) == NULL )
  {
    ch->println( "You must be holding both parts!!");
    return;
  }

  if ( ( second_ob = get_obj_carry( ch, arg2 ) ) == NULL )
  {
    ch->println( "You must be holding both parts!!" );
    return;
  }

  if ( first_ob->item_type != ITEM_PIECE || second_ob->item_type !=ITEM_PIECE )
  {
    ch->println( "Both items must be pieces of another item!" );
    return;
  }

  /* check to see if the pieces connect */

  if ( ( first_ob->value[0] == second_ob->pIndexData->vnum )
  && ( second_ob->value[0] == first_ob->pIndexData->vnum )
  && ( first_ob->value[1] == second_ob->value[1]  )
  && ( first_ob->pIndexData->vnum != second_ob->pIndexData->vnum ) )
  /* good connection  */
  {
    new_ob = create_object( get_obj_index( first_ob->value[2]));
    extract_obj( first_ob );
    extract_obj( second_ob );
    obj_to_char( new_ob, ch );

    act( "$n jiggles some pieces together...\r\n ...suddenly they snap in place, creating $p!", ch, new_ob, NULL, TO_ROOM );
    act( "You jiggle the pieces together...\r\n ...suddenly they snap into place, creating $p!", ch, new_ob, NULL, TO_CHAR );
  }
  else
  {
    act( "$n jiggles some pieces together, but can't seem to make them connect.", ch, NULL, NULL, TO_ROOM );
    act( "You try to fit them together every which way, but they just don't want to fit together.", ch, NULL, NULL, TO_CHAR );
    return;
  }
  return;
}
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/


