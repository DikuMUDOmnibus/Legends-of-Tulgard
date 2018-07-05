/**************************************************************************/
// territory.cpp - Territory Option for WOTL
/***************************************************************************
 * Whispers of Times Lost (c)1998-2004 Brad Wilson (wotlmud@bellsouth.net) *
 * >> If you use this code you must give credit in your help file as well  *
 *    as leaving this header intact.                                       *
 * >> To use this source code, you must fully comply with all the licenses *
 *    in licenses below. In particular, you may not remove this copyright  *
 *    notice.                                                              *
 ***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/

#include "include.h"
#include "tables.h"
#include "lookup.h"
#include "olc.h"
#include "gio.h"

void save_area( AREA_DATA *pArea );
RESET_DATA *new_reset_data args ( ( void ) );

// Prototypes
void do_worship(char_data *ch, char *argument);

// GIO 
GIO_START( TERRITORY_DATA )
GIO_STRH(  name,	"Name         " )
GIO_STRH(  description,	"Description  " )
GIO_STRH( mayor,	"Mayor        " )
GIO_SHINTH(number,	"Number       " )
GIO_SHINTH(taxrate,	"TaxRate      " )
GIO_SHINTH(taxes,	"Taxes        " )
GIO_SHINTH(jailvnum,	"Jail         " )
GIO_SHINTH(stockade,	"Stockade     " )
GIO_WFLAGH(crimeflags,	"Crimeflags   ", crime_flags )
GIO_FINISH

territory_type *territory_list;

/**************************************************************************/
// load em up
void load_territory_db( void )
{
	logf("===Loading territory database from %s...", TERRITORY_FILE );
	GIOLOAD_LIST( territory_list, TERRITORY_DATA, TERRITORY_FILE );
	log_string( "load_territory_db(): finished" );
}
/**************************************************************************/
// save the territory list
void save_territory_db( void )
{
	logf( "save_territory_db(): saving territory database to %s...", TERRITORY_FILE );
	GIOSAVE_LIST( territory_list, TERRITORY_DATA, TERRITORY_FILE, true );
}
/**************************************************************************/
// do func so it can be used as a command
void do_saveterritory( char_data *ch, char * )
{
	save_territory_db();
	ch->println("Territories saved...");
	logf( "do_saveterritory(): manual save of territories..." );
}
/**************************************************************************/
TERRITORY_DATA *territory_lookup( char *name )
{
    TERRITORY_DATA *pTerritory;

	// first try an exact match
	for(pTerritory=territory_list;pTerritory;pTerritory=pTerritory->next){
		if(!str_cmp(name,pTerritory->name)){
			return pTerritory;
		}
	}

	// now try a prefix match
	for(pTerritory=territory_list;pTerritory;pTerritory=pTerritory->next){
		if(!str_prefix(name,pTerritory->name)){
			return pTerritory;
		}
	}

	// not found
    return NULL;
}
/**************************************************************************/
TERRITORY_DATA *territory_number( int number )
{
    TERRITORY_DATA *pTerritory;

	// first try an exact match
	for(pTerritory=territory_list;pTerritory;pTerritory=pTerritory->next){
		if(number == pTerritory->number){
			return pTerritory;
		}
	}
	// not found
    return NULL;
}
/**************************************************************************/
// OLC section
bool territory_create( char_data *ch, char *newName )
{
	TERRITORY_DATA	*node;
	static TERRITORY_DATA zero_node;

	node	   = new TERRITORY_DATA;
	*node	   = zero_node;
	node->next = territory_list;
	territory_list = node;

	territory_list->name		= str_dup( newName );
	territory_list->description	= str_dup( "" );
	ch->desc->pEdit			= (void *)territory_list;
	ch->desc->editor		= ED_TERRITORY;
	ch->println("Territory Created.");
	return false;
}
/**************************************************************************/
bool tedit_description(char_data *ch, char *argument)
{
	TERRITORY_DATA *pD;
    	EDIT_TERRITORY( ch, pD );
	
	if(!IS_NULLSTR(argument)){
		ch->println("Syntax:  description (string editor used)");
		ch->println("uses the string editor to edit/add the description.");
		return false;
	}
	string_append( ch, &pD->description);
	return true;
}
/**************************************************************************/
void do_tedit( char_data *ch, char *argument )
{
	TERRITORY_DATA	*pTerritory;
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
		ch->println("syntax: tedit <territory>");
		ch->println("        tedit `=Ccreate`x to make a new territory.");
		ch->println("        tedit `=Clist`x lists existing territories.");
		return;
	}
	
	argument = one_argument( argument, arg );

	if ( !str_cmp( arg, "list" ))
	{
		TERRITORY_DATA	*pD;
		int			col = 0;

		ch->titlebar("Existing Territories");

		for( pD=territory_list;pD;pD=pD->next)
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
		territory_create( ch, arg );
		return;
	}

	pTerritory = territory_lookup( arg );
	
    if ( !pTerritory )
    {
        ch->printlnf("There is no territory named '`Y%s`x'.", arg);
        return;
    }

	ch->desc->pEdit = (void *)pTerritory;
	ch->desc->editor = ED_TERRITORY;
	ch->printlnf("Editing properties '%s'", pTerritory->name );
	return;
}
/**************************************************************************/
bool tedit_name(char_data *ch, char *argument)
{
    TERRITORY_DATA		*pTerritory;
	
    EDIT_TERRITORY( ch, pTerritory );
	
    if ( argument[0] == '\0' )
    {
		ch->println("Syntax:   name [name]");
		return false;
    }
    free_string( pTerritory->name );
    pTerritory->name = str_dup( argument );
    ch->println("Territory name set.");
    return true;
}
/**************************************************************************/
bool tedit_mayor(char_data *ch, char *argument)
{
    TERRITORY_DATA		*pTerritory;
	
    EDIT_TERRITORY( ch, pTerritory );
	
    if ( argument[0] == '\0' )
    {
		ch->println("Syntax:   mayor [name]");
		return false;
    }
    free_string( pTerritory->mayor );
    pTerritory->mayor = str_dup( argument );
    ch->println("Territory Mayor set.");
    return true;
}
/**************************************************************************/
bool tedit_show(char_data *ch, char *)
{
	TERRITORY_DATA *pD;
	
    	EDIT_TERRITORY(ch, pD);

	if(IS_NULLSTR( pD->name )) {ch->println("NULL TERRITORY?!?");return false;}

	ch->printlnf("`=rName:  `x%-20s   `=r", pD->name );
	ch->printlnf("`=rNumber: `x%-2d", pD->number);
	ch->wraplnf("`=rDescription: `x%s",	pD->description );
	ch->printlnf("`=rMayor:  `x%-20s   `=r", pD->mayor );
	ch->printlnf("`=rJail Vnum: `x%-5d",
		pD->jailvnum);
	ch->printlnf("`=rStockade Vnum: `x%-5d",
		pD->stockade);
	ch->printlnf("`=rCrimes Available: [`x%s`=r]", 
		flag_string(crime_flags, pD->crimeflags) );
	ch->printlnf("`=rTaxes: `x%-10d",
		pD->taxes);
	ch->printlnf("`=rTax Rate: `x%-2d",
		pD->taxrate);
	return false;
}
/**************************************************************************/
bool tedit_stockade( char_data *ch, char *argument )
{
	TERRITORY_DATA	*pD;
		
    if ( IS_NULLSTR(argument))
    {
		ch->println("Syntax: stockade [vnum]");
		return false;
    }

	if ( !is_number( argument ))
	{
		ch->println("The vnum value must be numeric.");
		tedit_stockade( ch, "" );		// redisplay the help
		return false;
	}

	if ( get_room_index( atoi( argument )) == '\0' )
	{
		ch->println("That room vnum doesn't exist.");
		return false;
	}

    EDIT_TERRITORY( ch, pD );

	pD->stockade = atoi( argument );
	ch->printf("The stockade vnum is now set to '%d'.", pD->stockade );
    return true;
}
/**************************************************************************/
bool tedit_jailvnum( char_data *ch, char *argument )
{
	TERRITORY_DATA	*pD;
		
    if ( IS_NULLSTR(argument))
    {
		ch->println("Syntax: jailvnum [vnum]");
		return false;
    }

	if ( !is_number( argument ))
	{
		ch->println("The vnum value must be numeric.");
		tedit_jailvnum( ch, "" );		// redisplay the help
		return false;
	}

	if ( get_room_index( atoi( argument )) == '\0' )
	{
		ch->println("That room vnum doesn't exist.");
		return false;
	}

    EDIT_TERRITORY( ch, pD );

	pD->jailvnum = atoi( argument );
	ch->printf("The jail vnum is now set to '%d'.", pD->jailvnum );
    return true;
}
/**************************************************************************/
bool tedit_taxes( char_data *ch, char *argument )
{
    TERRITORY_DATA *pD;
    EDIT_TERRITORY( ch, pD );
    int value;
	
    if ( argument[0] == '\0' || !is_number( argument ) )
    {
        ch->println("Syntax:  taxes [number]");
        return false;
    }
	
    value = atoi( argument );
	
    if (value>100000 || value<0)
    {
        ch->println("Value must lie between 0 and 100000.");
        return false;
    }
    pD->taxes = value;
    ch->printlnf("Taxes in bank set to %d.", pD->taxes );
    return true;
}
/**************************************************************************/
bool tedit_number( char_data *ch, char *argument )
{
    TERRITORY_DATA *pD;
    EDIT_TERRITORY( ch, pD );
    int value;
	
    if ( argument[0] == '\0' || !is_number( argument ) )
    {
        ch->println("Syntax:  number [number]");
        return false;
    }
	
    value = atoi( argument );
	
    if (value>10 || value<1)
    {
        ch->println("Value must lie between 0 and 10.");
        return false;
    }
    pD->number = value;
    ch->printlnf("Territory Number set to %d.", pD->number );
    return true;
}
/**************************************************************************/
bool tedit_taxrate( char_data *ch, char *argument )
{
    TERRITORY_DATA *pD;
    EDIT_TERRITORY( ch, pD );
    int value;
	
    if ( argument[0] == '\0' || !is_number( argument ) )
    {
        ch->println("Syntax:  taxrate [number]");
        return false;
    }
	
    value = atoi( argument );
	
    if (value>50 || value<0)
    {
        ch->println("Value must lie between 0 and 50.");
        return false;
    }
    pD->taxrate = value;
    ch->printlnf("Tax Rate set to %d.", pD->taxrate );
    return true;
}
/**************************************************************************/
bool tedit_crimeflags( char_data *ch, char *argument )
{
	TERRITORY_DATA *pD;
	int value;

	if ( IS_NULLSTR( argument ))
	{
		ch->println("Syntax:  crimeflag [flag].");
		ch->println("  Type '? crimeflag' for a list.");
		return false;
	}

	if ( (value = flag_value(crime_flags, argument) ) == NO_FLAG )
	{
		ch->printlnf( "Invalid flag '%s'", argument );
		tedit_crimeflags( ch, "" ); // redisplay the help
		return false;
	}

	EDIT_TERRITORY ( ch, pD );

	TOGGLE_BIT( pD->crimeflags, value );
	ch->println( "Flag value toggled." );
	return true;
}
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/


