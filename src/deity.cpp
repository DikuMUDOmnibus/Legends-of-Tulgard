/**************************************************************************/
// deity.cpp - Kereno's uncompleted deity code
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/

#include "include.h"
#include "magic.h"
#include "tables.h"
#include "lookup.h"
#include "olc.h"
#include "gio.h"

void save_area( AREA_DATA *pArea );

RESET_DATA *new_reset_data args ( ( void ) );

// protos
void do_worship(char_data *ch, char *argument);
void dedit_read_race(gio_type *, int, void *data, FILE *fp);
void dedit_write_race(gio_type *gio_table, int tableIndex, void *data, FILE *fp);
void dedit_read_rival(gio_type *, int, void *data, FILE *fp);
void dedit_write_rival(gio_type *gio_table, int tableIndex, void *data, FILE *fp);

GIO_CUSTOM_FUNCTION_PROTOTYPE(racetype_write_generic_races_set_for_n_array);
GIO_CUSTOM_FUNCTION_PROTOTYPE(racetype_read_generic_races_set_for_n_array);

// do da GIO thang
GIO_START( DEITY_DATA )
GIO_STRH(  name,		"Name         " )
GIO_STRH(  description,		"Description  " )
GIO_SHINTH(essence,		"Essence      " )
GIO_SHINTH(shrinevnum,		"Shrine       " )
GIO_SHINTH(symbol[0],		"Symbol1	  " )
GIO_SHINTH(symbol[1],		"Symbol2	  " )
GIO_SHINTH(symbol[2],		"Symbol3      " )
GIO_SHINTH(symbol[3],		"Symbol4	  " )
GIO_SHINTH(tendency,		"Tendency     " )
GIO_SHINTH(alliance,		"Alliance     " )
GIO_SHINTH(followers,		"Followers    " )
GIO_SHINTH(max_followers,	"MaxFollow    " )
GIO_WFLAGH(alignflags,		"Alignflags	  ", align_flags )
GIO_WFLAGH(tendflags,		"Tendflags	  ", tendency_flags )
GIO_CUSTOM_WRITEH(race_allow_n, "RaceAllowances ", racetype_write_generic_races_set_for_n_array)
GIO_CUSTOM_READH(race_allow_n,  "RaceAllowances ", racetype_read_generic_races_set_for_n_array)
GIO_WFLAGH(sex,			"Sex          ", sex_types )
GIO_CUSTOM_WRITEH(rival,	"Rival		  ", dedit_write_rival )
GIO_CUSTOM_READH( rival,	"Rival		  ", dedit_read_rival )
GIO_CUSTOM_WRITEH(race,		"Race         ", dedit_write_race )
GIO_CUSTOM_READH( race,		"Race         ", dedit_read_race )
GIO_FINISH

deity_type *deity_list;

/**************************************************************************/
// load em up
void load_deity_db( void )
{
	DEITY_DATA* pD = NULL;
	DEITY_DATA* pRival = NULL;

	logf("===Loading deity database from %s...", DEITY_FILE );
	GIOLOAD_LIST( deity_list, DEITY_DATA, DEITY_FILE );

	// Replace temporary rivals with proper ones from the deity_list;
	for ( pD=deity_list; pD; pD=pD->next )
	{
		if ( pD->rival && pD->rival->alignflags == -1 
					   && pD->rival->tendflags  == -1 )
		{
			if ( (pRival = deity_lookup(pD->rival->name) ) == NULL )
			{
				bugf("Dedit found unrecognised rival deity '%s' for '%s'", pD->rival->name, pD->name);
				free(pD->rival);
				pD->rival = NULL;
			} else {
				free(pD->rival);
				pD->rival = pRival;
			}
		}
	}

	log_string( "load_deity_db(): finished" );
}

/**************************************************************************/
// save the deity list
void save_deity_db( void )
{
	logf( "save_deity_db(): saving deity database to %s...", DEITY_FILE );
	GIOSAVE_LIST( deity_list, DEITY_DATA, DEITY_FILE, true );
}
/**************************************************************************/
// save the deity for essence update
void save_essence( void )
{
	GIOSAVE_LIST( deity_list, DEITY_DATA, DEITY_FILE, true );
}

/**************************************************************************/
// do func so it can be used as a command
void do_savedeities( char_data *ch, char * )
{
	save_deity_db();
	ch->println("Deities saved...");
	logf( "do_savedeities(): manual save of deities..." );
}

/**************************************************************************/
DEITY_DATA *deity_lookup( char *name )
{
    DEITY_DATA *pDeity;

	// first try an exact match
	for(pDeity=deity_list;pDeity;pDeity=pDeity->next){
		if(!str_cmp(name,pDeity->name)){
			return pDeity;
		}
	}

	// now try a prefix match
	for(pDeity=deity_list;pDeity;pDeity=pDeity->next){
		if(!str_prefix(name,pDeity->name)){
			return pDeity;
		}
	}

	// not found
    return NULL;
}

/**************************************************************************/
// OLC section
bool deity_create( char_data *ch, char *newName )
{
	DEITY_DATA	*node;
	static DEITY_DATA zero_node;

	node	   = new DEITY_DATA;
	*node	   = zero_node;
	node->next = deity_list;
	deity_list = node;

	deity_list->name			= str_dup( newName );
	deity_list->description		= str_dup( "" );
	deity_list->essence		= 100;
	deity_list->rival			= NULL;
	deity_list->symbol[0]		= 0;
	deity_list->symbol[1]		= 0;
	deity_list->symbol[2]		= 0;
	deity_list->symbol[3]		= 0;
	deity_list->max_followers	= 1;
	deity_list->race			= race_lookup( "human" );
	deity_list->sex				= sex_lookup( "male" );

	ch->desc->pEdit		= (void *)deity_list;
	ch->desc->editor	= ED_DEITY;
	ch->println("Deity Created.");
	return false;
}

/**************************************************************************/
void do_dedit( char_data *ch, char *argument )
{
	DEITY_DATA	*pDeity;
	char		arg[MIL];

	if ( IS_NPC( ch ))
	{
		ch->println("Players only.");
		return;
	}

	// do security checks
	if ( !HAS_SECURITY( ch, 8 ))
	{
		ch->println("You must have an olc security 8 or higher to use this command.");
		return;
	}


	if (IS_NULLSTR(argument)){
		ch->titlebar("DEDIT: SYNTAX");
		ch->println("syntax: dedit <deity>");
		ch->println("        dedit `=Ccreate`x to make a new deity.");
		ch->println("        dedit `=Clist`x lists existing deities.");
		return;
	}
	
	argument = one_argument( argument, arg );

	if ( !str_cmp( arg, "list" ))
	{
		DEITY_DATA	*pD;
		int			col = 0;

		ch->titlebar("Existing Deities");

		for( pD=deity_list;pD;pD=pD->next)
		{
            ch->printf(" %-40s", pD->name );
			if ( ++col % 2 == 0 )
				ch->println("");
		}
		ch->println("`x");
		return;
	}

	if ( !str_cmp( arg, "create" ))
	{
		one_argument( argument, arg );
		deity_create( ch, arg );
		return;
	}

	pDeity = deity_lookup( arg );
	
    if ( !pDeity )
    {
        ch->printlnf("There is no deity named '`Y%s`x'.", arg);
        return;
    }

	ch->desc->pEdit = (void *)pDeity;
	ch->desc->editor = ED_DEITY;
	ch->printlnf("Editing properties '%s'", pDeity->name );
	return;
}
/**************************************************************************/
bool dedit_name(char_data *ch, char *argument)
{
    DEITY_DATA		*pDeity;
	
    EDIT_DEITY( ch, pDeity );
	
    if ( argument[0] == '\0' )
    {
		ch->println("Syntax:   name [name]");
		return false;
    }
	
    free_string( pDeity->name );
    pDeity->name = str_dup( argument );
	
    ch->println("Deity name set.");
	
    return true;
}
/**************************************************************************/
bool dedit_show(char_data *ch, char *)
{
	DEITY_DATA *pD;
	
    EDIT_DEITY(ch, pD);

	if(IS_NULLSTR( pD->name )) {ch->println("NULL DEITY?!?");return false;}

	ch->printlnf("`=rName:  `x%-20s   `=r", pD->name );
	ch->wraplnf("`=rDescription: `x%s",	pD->description );
	ch->printlnf("`=rRace: `x%s     `=rSex:   `x%s",
		race_table[pD->race]->name,
		pD->sex == SEX_MALE ? "male" :
		pD->sex == SEX_FEMALE ? "female" : 
		pD->sex == 3 ? "random" : "neutral" );

	ch->printlnf("`=rRival Deity: `x%-20s    `=r",
		pD->rival == NULL ? "none" : pD->rival->name );
	ch->printlnf("`=rAlliance:    `x%-5d   `=rTendency:    `x%-5d",
		pD->alliance, pD->tendency );
	ch->printlnf("`=rShrine Vnum: `x%-5d",
		pD->shrinevnum);
	ch->printlnf("`=rSymbol1 Vnum: `x%-5d   `=rSymbol2 Vnum: `x%-5d",
		pD->symbol[0], pD->symbol[1] );
	ch->printlnf("`=rSymbol3 Vnum: `x%-5d   `=rSymbol4 Vnum: `x%-5d",
		pD->symbol[2], pD->symbol[3] );
	ch->printlnf("`=rAlignment Restrictions: [`x%s`=r]", 
		flag_string(align_flags, pD->alignflags) );
	ch->printlnf("`=rTendency Restrictions : [`x%s`=r]",
		flag_string(tendency_flags, pD->tendflags) );
	ch->printlnf("`=rRaces allowed: [`x%s`=r]", 
		race_get_races_set_for_n_array(pD->race_allow_n));

	return false;
}

/**************************************************************************/
bool dedit_shrinevnum( char_data *ch, char *argument )
{
	DEITY_DATA	*pD;
		
    if ( IS_NULLSTR(argument))
    {
		ch->println("Syntax: shrinevnum [vnum]");
		ch->println("        The shrinevnum denotes the room vnum where a player");
		ch->println("        is able to devote him/herself to the deity.  Should");
		ch->println("        be a remote place, so that the players have to try");
		ch->println("        and find it.");
		return false;
    }

	if ( !is_number( argument ))
	{
		ch->println("The vnum value must be numeric.");
		dedit_shrinevnum( ch, "" );		// redisplay the help
		return false;
	}

	if ( get_room_index( atoi( argument )) == '\0' )
	{
		ch->println("That room vnum doesn't exist.");
		return false;
	}

    EDIT_DEITY( ch, pD );

	pD->shrinevnum = atoi( argument );
	ch->printf("The shrine vnum is now set to '%d'.", pD->shrinevnum );
    return true;
}

bool dedit_rival( char_data *ch, char *argument )
{
	char arg[MIL];
	DEITY_DATA *pDeity;
	DEITY_DATA *pD;
	argument = one_argument(argument, arg);

	if ( IS_NULLSTR(arg))
	{
		ch->println( "Syntax: rival [deity name]." );
		return false;
	}

	EDIT_DEITY( ch, pD );

	if ( ( strstr(arg, "none") != '\0') )
	{
		pD->rival = NULL;
		ch->println( "Rival deity is now set to 'none'") ;
		return true;
	}

	if ( !(pDeity = deity_lookup( arg )) )
	{
		ch->printlnf( "`#`Y%s`^ is not a valid deity.", arg );
		return false;
	}


	if ( pDeity == pD )
	{
		ch->println("Rival deity cannot be the deity himself.");
		return false;
	}

	pD->rival = pDeity;
	ch->printlnf( "Rival deity is now set to '`#`Y%s`^'.", pD->rival->name );
	return true;
}

/**************************************************************************/
bool dedit_symbol( char_data *ch, char *argument )
{
	char arg[MIL];
	DEITY_DATA *pD;
	argument = one_argument(argument,arg);
		
    if ( IS_NULLSTR(argument))
    {
		ch->println("Syntax: symbol [number 1-4] [vnum]");
		ch->println("        The symbol denotes the symbol object that will be");
		ch->println("        used for when the player has stacked up enough favour");
		ch->println("        points.  They will then be able to summon the symbol to");
		ch->println("        show that they are loyal followers.");
		return false;
    }

	if ( !is_number( argument ) || !is_number(arg) )
	{
		ch->println("Both symbol number and  vnum value must be numeric.");
		dedit_symbol( ch, "" );		// redisplay the help
		return false;
	}

	if ( atoi(arg) < 1 || atoi(arg) > 4 )
	{
		ch->println( "The valid range for symbol numbers is between `#`W1`^ and `W4`^." );
		return false;
	}

	if ( get_obj_index( atoi( argument )) == '\0' )
	{
		ch->println("That object doesn't exist, create it first then set this value.");
		return false;
	}

    EDIT_DEITY( ch, pD );

	pD->symbol[atoi(arg)-1] = atoi( argument );
	ch->printlnf ( "The symbol%d vnum is now set as '%d'.", atoi(arg), atoi(argument) );
    return true;
}
/**************************************************************************/
bool dedit_alliance (char_data *ch, char *argument)
{
    DEITY_DATA *pD;
    int value;
	
    EDIT_DEITY( ch, pD );
	
    if ( argument[0] == '\0' || !is_number( argument ) )
    {
		ch->println("Syntax:  alliance [number]");
		return false;
    }
	
    value = atoi( argument );
	
    if (value>3 || value<-3)
    {
        ch->println("Alliance must be in the range -3 thru 3.");
        return false;
    }
    pD->alliance = value;
    ch->printlnf("Alliance set to %d.", pD->alliance);
    return true;
}

/**************************************************************************/
bool dedit_tendency(char_data *ch, char *argument)
{
    DEITY_DATA *pD;
    int value;
	
    EDIT_DEITY( ch, pD );
	
    if ( argument[0] == '\0' || !is_number( argument ) )
    {
        ch->println("Syntax:  tendency [number]");
        return false;
    }
	
    value = atoi( argument );
	
    if (value>3 || value<-3)
    {
        ch->println("Tendency must be in the range -3 thru 3.");
        return false;
    }
    pD->tendency = value;
    ch->printlnf("Tendency set to %d.", pD->tendency);
    return true;
}
/**************************************************************************/
bool dedit_sex(char_data *ch, char *argument)
{
    DEITY_DATA *pD;
	EDIT_DEITY( ch, pD );
    int value;
	
    if ( argument[0] != '\0' )
    {
		
		if (( value = flag_value( sex_types, argument ) ) != NO_FLAG )
		{
			pD->sex = value;
			ch->println("Sex set.");
			return true;
		}
    }
	
	show_olc_options(ch, sex_types, "sex", "sex", pD->sex);
    return false;
}
/**************************************************************************/
bool dedit_race(char_data *ch, char *argument)
{
    DEITY_DATA *pD;
    int race;
	
    if ( argument[0] != '\0'
		&& ( race = race_lookup( argument )) >= 0 )
    {
		EDIT_DEITY( ch, pD );
		
		pD->race = race;
        ch->printlnf("Race set to '%s'.", race_table[race]->name);
		return true;
    }
	
    if ( argument[0] == '?' )
    {
		ch->print("Available races are:");
		
		for ( race = 1; race_table[race]; race++ )
		{
			if (( race % 3 ) == 0 )
				ch->println("");
			ch->printf(" %-15s", race_table[race]->name );
		}
		
		ch->println("");
		return false;
    }
	
    ch->printf("Syntax:  race [race]\r\n"
				 "Type 'race ?' for a list of races.\r\n" );
    return false;
}

/**************************************************************************/
bool dedit_maxfollowers(char_data *ch, char *argument)
{
    DEITY_DATA *pD;
    EDIT_DEITY( ch, pD );
    int value;
	
    if ( argument[0] == '\0' || !is_number( argument ) )
    {
        ch->println("Syntax:  maxfollowers [number]");
        return false;
    }
	
    value = atoi( argument );
	
    if (value>1000 || value<0)
    {
        ch->println("Value must lie between 1 and 1000.");
        return false;
    }
    pD->max_followers = value;
    ch->printlnf("Max Followers set to %d.", pD->max_followers );
    return true;
}

/**************************************************************************/
bool dedit_tendflags( char_data *ch, char *argument )
{
	DEITY_DATA *pD;
	int value;

	if ( IS_NULLSTR( argument ))
	{
		ch->println("Syntax:  tendflag [flag].");
		ch->println("  Type '? tendflag' for a list.");
		return false;
	}

	value = flag_value( tendency_flags, argument );

	if ( value == NO_FLAG ) {
		ch->printlnf( "Invalid flag '%s'", argument );
		dedit_tendflags( ch, "" ); // redisplay the help
		return false;
	}

	EDIT_DEITY ( ch, pD );

	TOGGLE_BIT( pD->tendflags, value );
	ch->println( "Flag value toggled." );
	return true;
}
/**************************************************************************/
bool dedit_alignflags( char_data *ch, char *argument )
{
	DEITY_DATA *pD;
	int value;

	if ( IS_NULLSTR( argument ))
	{
		ch->println("Syntax:  alignflag [flag].");
		ch->println("  Type '? alignflag' for a list.");
		return false;
	}

	if ( (value = flag_value(align_flags, argument) ) == NO_FLAG )
	{
		ch->printlnf( "Invalid flag '%s'", argument );
		dedit_alignflags( ch, "" ); // redisplay the help
		return false;
	}

	EDIT_DEITY ( ch, pD );

	TOGGLE_BIT( pD->alignflags, value );
	ch->println( "Flag value toggled." );
	return true;
}

/**************************************************************************/
bool dedit_raceallow( char_data *ch, char *argument )
{
	DEITY_DATA *pD;
	EDIT_DEITY( ch, pD );
	int value;

	if ( IS_NULLSTR( argument )){
		ch->println("Syntax:  raceallow [race].");
		return false;
	}

	value = race_lookup(argument);
	if (value>=0)
	{
		TOGGLE_BITn(pD->race_allow_n, value);
		ch->printlnf( "Race '%s' toggled.", race_table[value]->name );
		return true;
	}

	ch->printlnf("Invalid race '%s'", argument );
	dedit_raceallow( ch, "" );	// redisplay the help
	return false;
}

/**************************************************************************/
void dedit_write_race(gio_type *gio_table, int tableIndex, void *data, FILE *fp)
{
	DEITY_DATA *pD;
	pD = (DEITY_DATA * ) data;
	fprintf(fp, "%s %s~\n",		
			gio_table[tableIndex].heading,	race_table[pD->race]->name);
}
/**************************************************************************/
void dedit_read_race(gio_type *, int, void *data, FILE *fp)
{
	DEITY_DATA *pD;
	char *pstr;

	pD = (DEITY_DATA * ) data;

	pstr=fread_string(fp);
	pD->race=race_lookup(pstr);
	if(pD->race<0){
		bugf("Dedit found unrecognised race '%s' for '%s'", pstr, pD->name);
		pD->race=0;
	}
	free_string(pstr);
}

/**************************************************************************/
void dedit_write_rival(gio_type *gio_table, int tableIndex, void *data, FILE *fp)
{
	DEITY_DATA *pD;

	pD = (DEITY_DATA * ) data;
	if ( !pD->rival )
		fprintf(fp, "%s none~\n", gio_table[tableIndex].heading);
	else
		fprintf(fp,"%s %s~\n", gio_table[tableIndex].heading , pD->rival->name);
}

/**************************************************************************/
void dedit_read_rival(gio_type *, int, void *data, FILE *fp)
{
	DEITY_DATA *pD, *pRival;
	char *pStr;

	pD = (DEITY_DATA *) data;

	pStr = fread_string(fp);
	if ( strstr(pStr, "none") != '\0' ) {
		pD->rival = NULL;
		return;
	}

	if ( (pRival = deity_lookup(pStr)) == NULL )
	{
		pRival = new DEITY_DATA;
		pRival->name = pStr;
		pRival->alignflags = -1;
		pRival->tendflags = -1;
		pD->rival = pRival;
	} else {
		pD->rival = pRival;
	}
	free_string(pStr);
}

/**************************************************************************/
bool dedit_description(char_data *ch, char *argument)
{
	DEITY_DATA *pD;
    EDIT_DEITY( ch, pD );
	
	if(!IS_NULLSTR(argument)){
		ch->println("Syntax:  description (string editor used)");
		ch->println("uses the string editor to edit/add the deity description.");
		return false;
	}
	string_append( ch, &pD->description);
	return true;
}
/**************************************************************************/
void do_godpower(char_data *ch, char *argument)
{
    char arg[MIL];


    if(IS_NPC(ch))
	return;

    if(ch->deity == NULL)
    {
	do_huh(ch, "");
	return;
    }

    argument=one_argument(argument,arg);

    DEITY_DATA	*pDeity;

	if ( (pDeity = deity_lookup(ch->name)) == NULL ) 
	{
		ch->println( "Your not a deity.");
		return;
	}

    if (arg[0] == '\0')
    {
	ch->println("Syntax:");
	ch->println("Godpower <power>");
	return; 
    }

    if (!str_prefix(arg,"amulet"))
    {
	obj_data *o;
	o=create_object(get_obj_index(pDeity->symbol[0]));
	obj_to_char( o, ch );
	ch->println("You have created an amulet.");
	act( "$n has created a divine amulet.", ch, NULL, NULL, TO_ROOM );
	return;
    }

    if (!str_prefix(arg,"heal"))
    {
     
	if (pDeity->essence < 100)
	{
		ch->println("You need at least 100 essence before you can heal.");
		return;
	}
     	char_data *rch;
	pDeity->essence -= 100;
	save_essence();

     	connection_data *d;
	for (d = connection_list; d != NULL; d = d->next)
	{
		rch = d->character;
		if (rch == NULL || IS_NPC(rch))
				continue;
		if ( rch->deity == NULL) 
			continue;
		if ( rch->deity->name == ch->deity->name )
		{
		   
		   	rch->printlnf( "`#`Y%s, %s has healed you.`^", ch->name, ch->short_descr );
		   	rch->subdued = false;
			rch->subdued_timer=0;
			rch->hit = rch->max_hit;
			rch->bleeding = 0;
			update_pos(rch);
		}
      }
      return;
    }


    if (!str_prefix(arg,"mana"))
    {
     
	if (pDeity->essence < 75)
	{
		ch->println("You need at least 75 essence before you can restore mana.");
		return;
	}
     	char_data *rch;
	pDeity->essence -= 75;
	save_essence();

     	connection_data *d;
	for (d = connection_list; d != NULL; d = d->next)
	{
		rch = d->character;
		if (rch == NULL || IS_NPC(rch))
				continue;
		if ( rch->deity == NULL) 
			continue;
		if ( rch->deity->name == ch->deity->name )
		{
		   
		   	rch->printlnf( "`#`Y%s, %s has restored your mana.`^", ch->name, ch->short_descr );
			rch->mana = rch->max_mana;
			update_pos(rch);
		}
      }
      return;
    }

    if (!str_prefix(arg,"see"))
    {
        char_data *rch;
     	connection_data *d;
	ch->printlnf("`YEssence: %d", pDeity->essence);
	for (d = connection_list; d != NULL; d = d->next)
	{
		rch = d->character;
		if (rch == NULL || IS_NPC(rch))
				continue;
		if ( rch->deity == NULL) 
			continue;
		if ( !rch->pcdata->deityrank)
			rch->pcdata->deityrank = 0;
		if ( rch->deity->name == ch->deity->name )
		{
		   	ch->printlnf( "`#`c%s `Y(`^%s`Y) `cRank: %d`^", rch->short_descr, rch->name, rch->pcdata->deityrank );
		}
      }
      return;
    }

    if (!str_prefix(arg,"refresh"))
    {
     
	if (pDeity->essence < 50)
	{
		ch->println("You need at least 50 essence before you can refresh.");
		return;
	}
     	char_data *rch;
	pDeity->essence -= 50;
	save_essence();

     	connection_data *d;
	for (d = connection_list; d != NULL; d = d->next)
	{
		rch = d->character;
		if (rch == NULL || IS_NPC(rch))
				continue;
		if ( rch->deity == NULL) 
			continue;
		if ( rch->deity->name == ch->deity->name )
		{
		   
		   	rch->printlnf( "`#`Y%s, %s has refreshed you.`^", ch->name, ch->short_descr );
			rch->pcdata->tired=UMIN(0, rch->pcdata->tired);
			rch->pcdata->condition[COND_THIRST]=
				UMAX(30, rch->pcdata->condition[COND_THIRST]);
			rch->pcdata->condition[COND_HUNGER]=
				UMAX(30,rch->pcdata->condition[COND_HUNGER]);
			rch->pcdata->condition[COND_FULL]=
				UMIN(0,rch->pcdata->condition[COND_FULL]);
			rch->pcdata->condition[COND_DRUNK]=
				UMIN(0,rch->pcdata->condition[COND_DRUNK]);
			rch->move = rch->max_move;
			update_pos(rch);
		}
      }
      return;
    }

    if (!str_prefix(arg,"bless"))
    {
     
	if (pDeity->essence < 500)
	{
		ch->println("You need at least 500 essence before you can power bless.");
		return;
	}
     	char_data *rch;
	pDeity->essence -= 500;
	save_essence();

     	connection_data *d;
	for (d = connection_list; d != NULL; d = d->next)
	{
		rch = d->character;
		if (rch == NULL || IS_NPC(rch))
				continue;
		if ( rch->deity == NULL) 
			continue;
		if ( rch->deity->name == ch->deity->name )
		{
			affect_parentspellfunc_strip( rch, gsn_bless);
			affect_parentspellfunc_strip( rch, gsn_armor);
			affect_parentspellfunc_strip( rch, gsn_giant_strength);
			affect_parentspellfunc_strip( rch, gsn_illusions_grandeur);
			affect_parentspellfunc_strip( rch, gsn_shield);
			affect_parentspellfunc_strip( rch, gsn_sanctuary);	
			spell_bless(gsn_bless, 92, ch, rch, TARGET_CHAR);
			spell_armor(gsn_armor, 92, ch, rch, TARGET_CHAR);
			spell_giant_strength(gsn_giant_strength, 92, ch, rch, TARGET_CHAR);	
			spell_illusions_grandeur(gsn_illusions_grandeur, 92, ch, rch, TARGET_CHAR);
			spell_shield(gsn_shield, 92, ch, rch, TARGET_CHAR);
			spell_sanctuary(gsn_sanctuary, 92, ch, rch, TARGET_CHAR);
		   	rch->printlnf( "`#`Y%s, %s has given you a powerful blessing.`x", ch->name, ch->short_descr );
			update_pos(rch);
		}
      }
      return;
    }	



    if (!str_prefix(arg,"cure"))
    {

	if (pDeity->essence < 75)
	{
		ch->println("You need at least 75 essence before you can cure.");
		return;
	}
     	char_data *rch;
	pDeity->essence  -= 75;
	save_essence();
     	connection_data *d;
	for (d = connection_list; d != NULL; d = d->next)
	{
		rch = d->character;
		if (rch == NULL || IS_NPC(rch))
			continue;
		if ( rch->deity == NULL) 
			continue;
		if ( rch->deity->name == ch->deity->name )
		{
		   	rch->printlnf( "`#`Y%s, %s has cured you.`^", ch->name, ch->short_descr );
			affect_parentspellfunc_strip( rch, gsn_plague	);
			affect_parentspellfunc_strip( rch, gsn_poison	);
			affect_parentspellfunc_strip( rch, gsn_blindness	);
			affect_parentspellfunc_strip( rch, gsn_sleep		);
			affect_parentspellfunc_strip( rch, gsn_curse		);
			affect_parentspellfunc_strip( rch, gsn_cause_fear	);
			affect_parentspellfunc_strip( rch, gsn_fear_magic    );
			affect_parentspellfunc_strip( rch, gsn_thorny_feet	);
			affect_parentspellfunc_strip( rch, gsn_chaotic_poison);
			affect_parentspellfunc_strip( rch, gsn_cause_headache);
			affect_parentspellfunc_strip( rch, gsn_despair	);
		        affect_strip(rch, gsn_neck_thrust);
		        update_pos(rch);
   	       }
      	}
      	return;
    }


    
    do_huh(ch, "");
    return;
}

void do_heretic(char_data *ch, char *argument)
{
	char arg[MIL];
	DEITY_DATA *pDeity;

    if(IS_NPC(ch))
	return;

	argument = one_argument(argument, arg);

	// Check input and classflags.
	if ( arg[0] == '\0' )
	{
		ch->println( "`#`WSyntax:`^ Heretic <follower>" );
		return;
	}

	if ( (pDeity = deity_lookup(ch->name)) == NULL ) 
	{
		ch->println( "Your not a deity.");
		return;
	}

	char_data *victim;

	if (( victim = get_char_world( ch, arg ) ) == NULL )
	{
		ch->println( "They aren't here." );
		return;
	}

	if (IS_NPC(victim))
	{
		ch->println( "You cannot cast them out.");
		return;
	}


	// Check deity allowances
	if ( victim->deity != ch->deity) 
	{
		ch->println( "They do not worship you." );
		return;
	}


	ch->printlnf( "`#`Y%s`^ no longer worships `W%s`^.", victim->name, victim->deity->name );
	victim->printlnf( "%s no longer cares for you and casts you out!", victim->deity->name) ;
	victim->pcdata->worship_time = current_time;
	victim->pcdata->deityrank = 0;
	victim->deity = NULL;
	OBJ_DATA *obj, *obj_next;

    	for ( obj = victim->carrying; obj; obj = obj_next )
    	{
		obj_next = obj->next_content;
		if ( obj->pIndexData->vnum == pDeity->symbol[0] ) 
		{
			unequip_char( victim, obj );
			obj_from_char( obj );
			extract_obj( obj );
			return;
		}
    	}
	return;
}


void do_patron(char_data *ch, char *argument)
{
	char arg[MIL];
	DEITY_DATA *pDeity;
	OBJ_INDEX_DATA *pObjIndex;
	OBJ_DATA *obj;

	argument = one_argument(argument, arg);

	// Check input and classflags.
	if ( arg[0] == '\0' )
	{
		ch->println( "`#`WSyntax:`^ Induct <follower>" );
		return;
	}

	if ( (pDeity = deity_lookup(ch->name)) == NULL ) 
	{
		ch->println( "Your not a deity.");
		return;
	}

	char_data *victim;

	if (( victim = get_char_world( ch, arg ) ) == NULL )
	{
		ch->println( "They aren't here." );
		return;
	}

	if (IS_NPC(victim))
	{
		ch->println( "You cannot accept them as a follower.");
		return;
	}


	// Check deity allowances
	if ( (victim->deity != NULL) && (victim->deity->name != NULL) ) {
		ch->printlnf( "They already worship the deity %s.", ch->deity->name );
		return;
	}

        if ( pDeity->shrinevnum == victim->in_room->vnum)
	{
		victim->temple = pDeity->shrinevnum;
		victim->printlnf( "%s accepts you as a follower.", pDeity->name);
		victim->printlnf( "You now worship `#`W%s`^.", pDeity->name);
		ch->printlnf( "You accept %s as a follower.", victim->name);
		ch->println("You have created an amulet for them.");
		act( "$n suddenly creates an amulet.", ch, NULL, NULL, TO_ROOM );
		pObjIndex = get_obj_index ( pDeity->symbol[0]);
		obj = create_object ( pObjIndex);
		obj_to_char( obj, ch);
		victim->deity = pDeity;
		victim->pcdata->worship_time = current_time;
		victim->pcdata->deityrank = 1;
		victim->temple = pDeity->shrinevnum;
	}
	else if ( pDeity->shrinevnum != victim->in_room->vnum)
	{
		ch->println( "They must be in your temple in order to make them a follower.");
	}
	return;
}

/**************************************************************************/
void do_worship(char_data *ch, char *argument)
{
	char arg[MIL];
	DEITY_DATA *pDeity;

	argument = one_argument(argument, arg);

	// Check input and classflags.
	if ( arg[0] == '\0' )
	{
		ch->println( "`#`WSyntax:`^ worship <deity>" );
		ch->println( "`#`WSyntax:`^ worship none - leave your deity" );
		return;
	}
	if ( IS_NPC(ch) || !HAS_CLASSFLAG( ch, CLASSFLAG_DEITIES ) )
	{
		ch->println( "The deity you have chosen to worship ignores you." );
		ch->println( "Perhaps you should have chosen a holier path to walk" );
		return;
	}

	if (!str_prefix(arg,"none"))
	{

	   if ( ch->deity && ch->deity->name )
	   {
	        pDeity = deity_lookup(ch->deity->name);

		ch->println( "`#`YYou now follow no deity`^.");
		ch->println( "`#`YYou lose 7500 xp as punishment!`^");
		ch->pcdata->worship_time = current_time;
		gain_exp(ch, -7500);
		ch->deity = NULL;
		ch->pcdata->deityrank = 0;

		OBJ_DATA *obj, *obj_next = NULL;
		obj = ch->carrying;
    		for ( obj = ch->carrying; obj; obj = obj_next )
    		{
			if ( obj->pIndexData->vnum == pDeity->symbol[0] ) 
			{
				ch->printlnf( "`#`YYour amulet of %s suddenly vanishes`^.", pDeity->name);
				unequip_char( ch, obj );
				obj_from_char( obj );
				extract_obj( obj );
				return;
			}
    		}
		return;
	   }
	   else
           {
		ch->println( "You already follow no deity.");
		ch->pcdata->worship_time = current_time;
		ch->deity = NULL;
		ch->pcdata->deityrank = 0;
		ch->temple = 1;
		return;
	   }
	}

	if ( (pDeity = deity_lookup(arg)) == NULL ) 
	{
		ch->printlnf( "`#`Y%s`^ is not a valid deity.", arg);
		return;
	}

	// Check deity allowances
	if ( (ch->deity != NULL) && (ch->deity->name != NULL) ) {
		ch->printlnf( "You already worship %s.", ch->deity->name );
		return;
	}

	// Check whether the player choose this himself and whether he typed confirm.
	if ( IS_CONTROLLED(ch) )
	{
		ch->printlnf( "The gods sense this worship is not completely voluntary." );
		return;
	}
	  
        if ( pDeity->shrinevnum == ch->in_room->vnum)
	{
		ch->printlnf( "You bow down before the altar and worship %s.", pDeity->name);
		ch->printlnf( "You now worship %s.", pDeity->name);
		ch->deity = pDeity;
		ch->pcdata->worship_time = current_time;
		ch->pcdata->deityrank = 0;
		ch->temple = pDeity->shrinevnum;
		obj_data *o;
		o=create_object(get_obj_index(pDeity->symbol[0]));
		obj_to_char( o, ch );
		ch->printlnf( "An amulet of %s appears in your inventory.", pDeity->name);
		return;
	}
	else if ( pDeity->shrinevnum != ch->in_room->vnum)
	{
		ch->printlnf( "%s requests that you seek out their temple in order to dedicate yourself.", pDeity->name);
		return;
	}
}

/**************************************************************************/
void do_offering(char_data *ch, char *argument)
{
	char arg[MIL];
    	OBJ_DATA *obj=NULL;

	argument = one_argument(argument, arg);

	// Check input and classflags.
	if ( arg[0] == '\0' )
	{
		ch->println( "`#`WSyntax:`^ offering <food item or corpse>" );
		return;
	}

	if (IS_NPC(ch))
	{
		ch->println( "Huh ?");
		return;
	}

	obj = get_obj_list(ch, "altar", ch->in_room->contents);
    
    	if (obj == NULL)
    	{
	    ch->println( "You can't an altar here find it." );
	    return;
    	}

   	DEITY_DATA	*pDeity;

	if (obj->pIndexData->vnum == 7500)
	{
	    	pDeity = deity_lookup( "Harpan" );
	}else
	if (obj->pIndexData->vnum == 7510)
	{
	    	pDeity = deity_lookup( "Dorenne" );
	}else
	if (obj->pIndexData->vnum == 7522)
	{
	    	pDeity = deity_lookup( "Jivanor" );
	}else
	if (obj->pIndexData->vnum == 7530)
	{
	    	pDeity = deity_lookup( "Kreav" );
	}else
	if (obj->pIndexData->vnum == 7545)
	{
	    	pDeity = deity_lookup( "Graal" );
	}else
	if (obj->pIndexData->vnum == 7550)
	{
	    	pDeity = deity_lookup( "Venalena" );
	}else
	if (obj->pIndexData->vnum == 7560)
	{
	    	pDeity = deity_lookup( "Seandac" );
	}else
	if (obj->pIndexData->vnum == 7570)
	{
	    	pDeity = deity_lookup( "Anacor" );
	}else
	{
		ch->println( "You must make your offering at an altar.");
		return;
	}

    if (!str_prefix(arg,"corpse"))
    {
	    obj = get_obj_list(ch, arg, ch->carrying);
    
	    if (obj == NULL)
	    {
		    ch->println( "You not carrying any corpse." );
		    return;
	    }

 	    if ( obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC)
   	    {
	  	 ch->println( "That is not a worthy offering." );
	   	 return;
	    }
		
	    pDeity->essence += 5;
	    save_essence();

	    // questing wiznet
	    {
		char qbuf[MSL];
		sprintf (qbuf, "`cGODS> %s offers up %s to %s.`x", ch->name, obj->short_descr, pDeity->name);
		wiznet(qbuf,ch,NULL,WIZ_QUESTING,0,LEVEL_IMMORTAL);
	    }

            ch->printlnf( "`RYou offer up %s in sacrifice to %s.`x", obj->short_descr, pDeity->name );
            act( "`R$n offers up $p in sacrifice.`x", ch, obj, NULL, TO_ROOM );
	    extract_obj(obj);
	    return;
    }

	    obj = get_obj_list(ch, arg, ch->carrying);
    
	    if (obj == NULL)
	    {
		    ch->println( "You not carrying any food offerings." );
		    return;
	    }

 	    if ( obj->item_type != ITEM_FOOD)
   	    {
	  	 ch->println( "That is not a worthy offering." );
	   	 return;
	    }
		
	    pDeity->essence += 5;
	    save_essence();

	    // questing wiznet
	    {
		char qbuf[MSL];
		sprintf (qbuf, "`cGODS> %s offers up %s to %s.`x", ch->name, obj->short_descr, pDeity->name);
		wiznet(qbuf,ch,NULL,WIZ_QUESTING,0,LEVEL_IMMORTAL);
	    }

            ch->printlnf( "`RYou offer up %s in sacrifice to %s.`x", obj->short_descr, pDeity->name );
            act( "`R$n offers up $p in sacrifice.`x", ch, obj, NULL, TO_ROOM );
	    extract_obj(obj);
	    return;
 
}
/**************************************************************************/
void do_shrine( char_data *ch, char *argument )
{
    char arg[MIL];

    if(IS_NPC(ch))
	return;

    if(ch->deity == NULL)
    {
	do_huh(ch, "");
	return;
    }

    argument=one_argument(argument,arg);

    DEITY_DATA	*pDeity;

	if ( (pDeity = deity_lookup(ch->deity->name)) == NULL ) 
	{
		ch->println( "You do not follow any deity.");
		return;
	}

    if (arg[0] == '\0')
    {
	ch->println("Syntax: Shrine <option>");
	return; 
    }

    pDeity = deity_lookup(ch->deity->name);

    if (!str_prefix(arg,"build"))
    {
	OBJ_DATA *altar;
	altar = get_obj_list(ch, "altar", ch->in_room->contents);
    
    	if (altar != NULL)
    	{
	    ch->println( "There is already a shrine here." );
	    return;
    	}

	switch ( ch->in_room->sector_type ) 
	{
		case (SECT_INSIDE):
		case (SECT_WATER_SWIM):
		case (SECT_UNDERWATER):
		case (SECT_CITY):
		case (SECT_WATER_NOSWIM):
		case (SECT_SWAMP):
		case (SECT_AIR):
		case (SECT_GATEWAY):
		case (SECT_MAX):
	    ch->println( "This is not a suitable location to construct a shrine." );
	    return;
    	}

    	if(ch->gold < 10000)
	{
		ch->println( "It costs 10000 gold to build a shrine." );
		return;
	}
	ch->gold -= 10000;
	pDeity->essence += 400;
	save_essence();
 
	OBJ_DATA *obj;
    	AREA_DATA *loc_area;
    	ROOM_INDEX_DATA *loc = NULL;
    	RESET_DATA *loc_reset;

	loc = ch->in_room;
	loc_area = ch->in_room->area;
	obj = create_object(get_obj_index(pDeity->symbol[1]));
	loc_reset               = new_reset_data();
	loc_reset->command      = 'O';
	loc_reset->arg1         = obj->pIndexData->vnum;
	loc_reset->arg2         = 0;
	loc_reset->arg3         = loc->vnum;
	loc_reset->arg4         = 0;
	add_reset( loc, loc_reset, 0 );
	obj_to_room (obj, ch->in_room);
	SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
	save_area( loc_area );
	REMOVE_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
	ch->printlnf("You build an altar to %s.", pDeity->name);
	WAIT_STATE( ch, 10*PULSE_VIOLENCE );
        act( "`R$n has built an altar with to thier deity.`x", ch, obj, NULL, TO_ROOM );
	return;
    }
	

    if (!str_prefix(arg,"desecrate"))
    {
	OBJ_DATA *obj;
	obj = get_obj_list(ch, "altar", ch->in_room->contents);
    
    	if (obj == NULL)
    	{
	    ch->println( "You can't an altar to desecrate." );
	    return;
    	}

   	DEITY_DATA	*pDeity = NULL;

	if (obj->pIndexData->vnum == 7500)
	{
	    	pDeity = deity_lookup( "Harpan" );
	}else
	if (obj->pIndexData->vnum == 7510)
	{
	    	pDeity = deity_lookup( "Dorenne" );
	}else
	if (obj->pIndexData->vnum == 7522)
	{
	    	pDeity = deity_lookup( "Jivanor" );
	}else
	if (obj->pIndexData->vnum == 7530)
	{
	    	pDeity = deity_lookup( "Kreav" );
	}else
	if (obj->pIndexData->vnum == 7545)
	{
	    	pDeity = deity_lookup( "Graal" );
	}else
	if (obj->pIndexData->vnum == 7550)
	{
	    	pDeity = deity_lookup( "Venalena" );
	}else
	if (obj->pIndexData->vnum == 7560)
	{
	    	pDeity = deity_lookup( "Seandac" );
	}else
	if (obj->pIndexData->vnum == 7570)
	{
	    	pDeity = deity_lookup( "Anacor" );
	}

    	obj = get_obj_list(ch, "corpse", ch->carrying);
    
	if (obj == NULL)
	{
	    ch->println( "You need a corpse to desecrate the altar with." );
	    return;
	}

 	if ( obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC)
   	{
	   	ch->println( "You need a real corpse to desecrate the altar with." );
 		return;
	}
		
	pDeity->essence -= 10;
	save_essence();

	// questing wiznet
	{
		char qbuf[MSL];
		sprintf (qbuf, "`cGODS> %s desecrates an altar to %s with %s.`x", ch->name, pDeity->name, obj->short_descr);
		wiznet(qbuf,ch,NULL,WIZ_QUESTING,0,LEVEL_IMMORTAL);
	 }

         ch->printlnf( "`RYou desecrate the altar of %s with the blood of %s.`x", pDeity->name, obj->short_descr );
         act( "`R$n desecrates the altar with $p.`x", ch, obj, NULL, TO_ROOM );
	 WAIT_STATE( ch, 10*PULSE_VIOLENCE );
	 extract_obj(obj);
	 return;
    }

    ch->println("Syntax: Shrine <option>");
    ch->println("        options: build | desecrate");
    return; 

}


