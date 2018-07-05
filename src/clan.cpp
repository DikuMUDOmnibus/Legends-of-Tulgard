/**************************************************************************/
// clan.cpp - Clan Class and clan related functions - Tibault & Kal
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/
#include "include.h"
#include "olc.h"
#include "clan.h"

#define ROOM_CLAN_DEFAULT	100

DECLARE_DO_FUN( do_look );
void save_clan_db( void );

CClanType *clan_list=NULL;
/**************************************************************************/    
CClanType::CClanType()
{
	m_pSaveName		= str_dup("");
	m_pName			= str_dup("");
	m_pFounder		= str_dup("");
	m_pNoteName		= str_dup("");
	m_pWhoName		= str_dup("");
	m_pDescription	= str_dup("");
	m_pWhoCat		= str_dup("");
	m_pColorStr		= str_dup("");
	m_RecallRoom	= 0;
	m_BankRoom		= 0;
	m_Hidden		= false;

	m_pClanRankTitle[0]	= str_dup( "toprank" );
	m_CanAdd[0]			= true;
	m_CanPromote[0]		= true;
	m_CanRemove[0]		= true;
	m_CanWithdraw[0]	= true;

	for ( int i=1; i< MAX_RANK; i++ )
	{
		m_pClanRankTitle[i]	= str_dup( "" );
		m_CanAdd[i]			= false;
		m_CanPromote[i]		= false;
		m_CanRemove[i]		= false;
		m_CanWithdraw[i]	= false;
	}

	next=NULL;
}

/**************************************************************************/    
CClanType::~CClanType()
{
	free_string(m_pSaveName);
	free_string(m_pName);
	free_string(m_pFounder);
	free_string(m_pNoteName);
	free_string(m_pWhoName);	
	free_string(m_pDescription);	
	free_string(m_pWhoCat);
	free_string(m_pColorStr);
	for ( int i=0; i< MAX_RANK; i++ )
	{
		free_string(m_pClanRankTitle[i]);
	}
}

/**************************************************************************/    
void CClanType::printDetails(char_data *ch)
{
	if(IS_ADMIN(ch)){
		ch->printlnf( " `#%s%-40s`& %-15s %6d %8d",
			m_pColorStr, m_pName, m_pNoteName, m_RecallRoom, (int)m_BankFunds);
	}else if(IS_IMMORTAL(ch)){
		ch->printlnf( " `#%s%-40s`& %-15s %6d",
			m_pColorStr, m_pName, m_pNoteName, m_RecallRoom );
	}else{
		ch->printlnf( " `#%s%-40s`& %-15s ???",
			m_pColorStr, m_pName, m_pNoteName);
	}
}

/**************************************************************************/    
void CClanType::printRanks( char_data *ch )
{
	int i;
	int count=0;

	for ( i=0; i< MAX_RANK; i++ ) 
	{
		if ( !IS_NULLSTR(m_pClanRankTitle[i]) ) {
			ch->printlnf( "`sRank %d `s- Title %s%s",
				i,
				m_pColorStr,
				m_pClanRankTitle[i] );
			count++;
		}
	}
	ch->printlnf("There %s a total of %d rank%s in %s",
		count==1?"is":"are", 
		count, 		
		count==1?"":"s", 
		cname());

}

/**************************************************************************/
int CClanType::minRank()
{
	int lIdx;

	for ( lIdx=MAX_RANK-1; lIdx>=0; lIdx-- ) {
		if( !IS_NULLSTR(m_pClanRankTitle[lIdx])){
			return lIdx;
		}
	}

	return 0;
}

/**************************************************************************/
/**************************************************************************/
//	GENERIC CLAN FUNCTIONS												  
/**************************************************************************/
/**************************************************************************/

/**************************************************************************/
//	do_addclan											  Tibault:Sep 00
//	Function for admin or headclan for setting the clan on players.
//	You can also set the rank with this command. If ommited, the top rank
//	will be set.
/**************************************************************************/
void do_addclan( char_data *ch, char *argument )
{
	char arg1[MIL],arg2[MIL],arg3[MIL];
	char_data *victim;
	int rank = 0;

	CClanType* pClan;

    if (!IS_ADMIN(ch)
        && !IS_SET(TRUE_CH(ch)->pcdata->council, COUNCIL_HEADCLAN))	
    {
		ch->println( "Addclan can only be used by the head of the clan council or admin." );
        return;
    }

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	
	if(IS_IMMORTAL(ch))
	{
		argument = one_argument( argument, arg3 );
	}
	
	if ( arg1[0] == '\0' || arg2[0] == '\0'  )
	{
		if(IS_IMMORTAL(ch))
		{
			ch->println( "Syntax: addclan <char> <clan name> <clan rank>" );
		} else {
			ch->println( "Syntax: addclan <char> <clan name>" );
		}
		ch->println( "Syntax: addclan <char> none (to remove them from a clan)" );
		return;
	}
	if ( ( victim = get_whovis_player_world( ch, arg1 ) ) == NULL )
	{
		ch->println( "They aren't playing." );
		return;
	}
	
	if (!str_prefix(arg2,"none"))
	{
        ch->println( "They are now clanless." );
        victim->println( "You are now a member of no clan!" );
		victim->clan	= NULL;
		victim->clanrank = 0;
        return;
	}
	
	if (!IS_LETGAINED(victim)){
        ch->printlnf( "%s hasn't been letgained yet.", PERS(victim, ch) );
        return;
	}
		
	if ((pClan = clan_lookup(arg2)) == NULL)
	{
        ch->printlnf( "No such clan '%s' exists.", arg2 );
        return;
	}

	if(!IS_IMMORTAL(ch))
	{
		rank=pClan->minRank();
	}
	
	if (IS_NULLSTR(arg3))
	{
		rank=pClan->minRank();
	}
	else
	{
		if ((rank = pClan->rank_lookup(arg3) ) == -1)
		{
			ch->printlnf( "No such rank '%s' exists in the clan '%s'.", 
				arg3, pClan->cname());
			return;
		}
	}

	{
		ch->printlnf( "They are now a %s (%d) of clan %s.",
			pClan->clan_rank_title(rank), rank, pClan->cname() );
		victim->printlnf("You are now a %s of clan %s.",
			pClan->clan_rank_title(rank), pClan->cname() );
	}
	
	victim->clan  = pClan;
	victim->clanrank = rank;
}

/**************************************************************************
	do_clanlist											  Tibault:Sep 00
	Prints the details for every clan in the global variable clan_list.
	If you want to change the way these details are shown, change 
	the CClanType function printdetails().
**************************************************************************/
void do_clanlist(char_data *ch, char *)
{
    CClanType *pClan;

	if(!clan_list){
		ch->println("There are currently no clans, clans can be added using clanedit create.");
		return;
	}

    	ch->print( "`#`S-`YClanname`S===================================`YNoteName`S========`YRecall`S");

	if(IS_ADMIN(ch)){
		ch->println("====`YFunds`S-`^");
	}else{
		ch->println("-`^");
	}

    for (pClan = clan_list; pClan; pClan = pClan->next ){
		pClan->printDetails(ch);
	}
}
/**************************************************************************/
// Kal, Feb 02
void clan_show_clanrank_syntax(char_data *ch)
{
	if(IS_IMMORTAL(ch)){
		ch->println("syntax: clanrank <clanname> - to see the ranks of a given clan.");
		ch->println("syntax: clanrank <player> - to see the rank of a given player.");
	}else{
		ch->println("syntax: clanrank <clanmember> - to see the rank of another clan member in the same room.");
	}
}

/**************************************************************************/
// Kal, Feb 02
void clan_show_ranks(char_data *ch, char_data *victim, CClanType *pClan)
{
	if(pClan){
		pClan->printRanks(ch);

		if(victim){
			ch->printlnf( "%s's rank in this clan is: `#%s%s`^",
				PERS(victim, ch),
				pClan->color_str(), 
				pClan->clan_rank_title(victim->clanrank) );   
		}
		
		if ( ch->clan == pClan){
			ch->printlnf( "Your rank in this clan is: `#%s%s`^",
				pClan->color_str(), 
				pClan->clan_rank_title(ch->clanrank) );   
		}
	}

}

/**************************************************************************/
// Kal, Feb 02
void do_clanranks(char_data *ch, char *argument)
{	
	char_data *victim;

	// non clanned players can not use the command
	if(!IS_IMMORTAL(ch) && !ch->clan){
		ch->println("You are clanless, you have no use for this command.");
		return;
	}

	// if no argument is specified, show a player/immortal 
	// the clan they are in
	if(IS_NULLSTR(argument)){		
		clan_show_ranks(ch, NULL, ch->clan);
		clan_show_clanrank_syntax(ch);
		return;
	}

	// attempt to find a specified player
	victim = get_whovis_player_world(ch, argument);
	if(!victim){
		// character not found
		if(IS_IMMORTAL(ch)){ // imms can also specify the clan name
			// picking a clan
			CClanType* pClan= clan_lookup(argument);
			if(!pClan){
				ch->printlnf( "No such clan or player '%s' is currently in the game.", argument );
				clan_show_clanrank_syntax(ch);
				return;
			}

			// clan found, show the ranks
			clan_show_ranks(ch, NULL, pClan);
			return;
		}

		ch->printlnf( "No such clanmember '%s' is currently in the game.", argument );	
		return;
	}

	// victim was found, do some checks
	if(IS_IMMORTAL(ch)){
		if(!victim->clan){
			ch->printlnf("%s is not in a clan.", PERS(victim, ch));
			clan_show_clanrank_syntax(ch);
			return;
		}
	}else{
		if(!victim->clan || victim->clan!=ch->clan){
			ch->printlnf("%s is not in the same clan as you.", PERS(victim, ch));
			clan_show_clanrank_syntax(ch);
			return;
		}

		if(ch->in_room!=victim->in_room){
			ch->printlnf("%s is not in the same room as you.", PERS(victim, ch));
			clan_show_clanrank_syntax(ch);
			return;
		}
	}

	
	// victim found, show the ranks
	clan_show_ranks(ch, victim, victim->clan);
}

/**************************************************************************
	do_add												Tibault:Sep 00
	Function for mortals to add members to their clan.
**************************************************************************/
void do_add( char_data *ch, char *argument )
{
    char arg1[MIL];
	char_data *victim;

	if ( IS_OOC(ch) ) {
		ch->println( "Add is an IC command, you can't use it in OOC rooms." );
		return;
    }

    argument = one_argument( argument, arg1 );
    if( ch->clan  == NULL ) {
        ch->println( "You are not in a clan yourself!" );
        return;
    }

    if( ch->clan->m_CanAdd[ch->clanrank] == false ) {
        ch->println( "You don't have the authority to do that." );
        return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL ) {
		ch->println( "They aren't playing." );
		return;
    }

    if ( victim->in_room != ch->in_room ) {
        ch->println( "They aren't here." );
        return;
    }

    if ( !IS_LETGAINED(victim) ) {
        ch->printlnf( "%s hasn't been letgained yet.", PERS(victim, ch) );
        return;
    }

    if( victim->clan  ) {
        ch->printlnf( "%s is already in a clan.", PERS(victim, ch) );
        return;
    }

    if( victim->seeks != ch->clan ) {
        ch->wraplnf("They are not seeking to join your clan."
			"They have been told you invited them to join, and to seek your clan if interested"
			"- try again shortly.");

        victim->wraplnf("%s invited you to become a member of a "
            "clan which goes by the name '%s'.  You weren't seeking that "
            "clan, type 'seek %s' if you want to join, and when they invite "
            "you again... you will automatically join.",
            PERS(ch, victim),
            ch->clan->name(),
            ch->clan->notename());
        return;
    }

    victim->clan 		= ch->clan ;
    victim->clanrank	= ch->clan->minRank();
	victim->seeks		= NULL;

    ch->printlnf( "They are now a %s of your clan.",
		victim->clan->clan_rank_title(victim->clanrank));
	victim->printlnf( "You are now a %s of clan %s.",
		victim->clan->clan_rank_title(victim->clanrank),
		victim->clan->name());

    return;
}

/**************************************************************************
	do_demote											  Tibault:Sep 00
	Function for mortals to demote members to their clan.
**************************************************************************/
void do_demote( char_data *ch, char *argument )
{
    char arg1[MIL];
	char_data *victim;

	if ( IS_OOC(ch) ) {
		ch->println( "Demote is an IC command, you can't use it in OOC rooms." );
		return;
    }

    if( ch->clan  == NULL ) {
        ch->println( "You are not in a clan yourself!" );
        return;
    }

    if( ch->clan->m_CanPromote[ch->clanrank] == false ) {
        ch->println( "You don't have the authority to do that." );
        return;
    }

	argument = one_argument( argument, arg1 );
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL ) {
		ch->println( "They aren't playing." );
		return;
    }

    if ( victim->in_room != ch->in_room ) {
        ch->println( "They aren't here." );
        return;
    }

    if( victim->clan  != ch->clan  ) {
        ch->println( "They are not in your clan." );
        return;
    }

    if( victim->clanrank <= ch->clanrank  ) {
        ch->println( "They are too high in rank for your authority." );
		ch->println( "You Must ask an Immortal to remove another leader." );
        return;
    }

    if( victim->clanrank >= victim->clan->minRank() ) {
		ch->println( "They are already of the lowest rank." );
		return;
	}

    victim->clanrank++;

    ch->printlnf( "They are now a %s of your clan.",
		 ch->clan->clan_rank_title(victim->clanrank) );

	victim->printlnf( "You are now a %s of clan %s.",
         victim->clan->clan_rank_title(victim->clanrank),
         victim->clan->name());

    return;
}

/**************************************************************************
	do_outcast											  Tibault:Sep 00
	Function for mortals to outcast members to their clan.
**************************************************************************/
void do_outcast( char_data *ch, char *argument )
{
    char arg1[MIL];
	char_data *victim;

	if ( IS_OOC(ch) ) {
		ch->println( "Outcast is an IC command, you can't use it in OOC rooms." );
		return;
    }

    if( ch->clan  == NULL ) {
        ch->println( "You are not in a clan yourself!" );
        return;
    }

    if( ch->clan->m_CanRemove[ch->clanrank] == false ) {
        ch->println( "You don't have the authority to do that." );
        return;
    }

    argument = one_argument( argument, arg1 );
    if ( (victim=get_char_world(ch, arg1)) == NULL ) {
		ch->println( "They aren't playing." );
		return;
	}

    if ( victim->in_room != ch->in_room ) {
        ch->println( "They aren't here." );
        return;
    }

    if( victim->clan  != ch->clan ){
        ch->println( "They are not in your clan." );
        return;
    }

    if(ch->clanrank >= victim->clanrank ) {
        ch->println( "You don't have the authority to do that." );
        return;
    }

    ch->printlnf( "They are no longer a %s of your clan.",
		victim->clan->clan_rank_title(victim->clanrank) );
 	victim->printlnf( "You are no longer a %s of clan %s.",
		victim->clan->clan_rank_title(victim->clanrank),
		victim->clan->name());
 	victim->println("You have been outcast!");

    victim->clan  = NULL;
    victim->clanrank = 0;

    return;
}

/**************************************************************************
	do_promote											  Tibault:Sep 00
	Function for mortals to outcast members to their clan.
**************************************************************************/
void do_promote( char_data *ch, char *argument )
{
    char arg1[MIL];
	char_data *victim;

	if ( IS_OOC(ch) ) {
		ch->println( "Promote is an IC command, you can't use it in OOC rooms." );
		return;
    }

    if( ch->clan  == NULL ) {
        ch->println("You are not in a clan yourself!" );
        return;
    }

    if( ch->clan->m_CanPromote[ch->clanrank] == false ) {
        ch->println( "You don't have the authority to do that." );
        return;
    }

    argument = one_argument( argument, arg1 );
    if ( (victim = get_char_world(ch, arg1)) == NULL ) {
		  ch->println( "They aren't playing." );
		  return;
    }

    if ( victim->in_room != ch->in_room ) {
        ch->println( "They aren't here." );
        return;
    }

    if( victim->clan  != ch->clan  ) {
        ch->println( "They are not in your clan." );
        return;
    }

    if( victim->clanrank<=ch->clanrank ){
        ch->println( "They are already too high in rank for your authority." );
        return;
    }

	if(victim->clanrank==1){
        ch->println( "They are already the highest rank in your clan." );
        return;
	}

    victim->clanrank--;

    ch->printlnf( "They are now a %s of your clan.",
		ch->clan->clan_rank_title(victim->clanrank) );
	victim->printlnf( "Congratulations, you are now a %s of clan %s.",
		ch->clan->clan_rank_title(victim->clanrank),
		ch->clan->name());

    return;
}

/**************************************************************************
	do_seek												  Tibault:Sep 00
	Function for mortals to seek the clan they wish to join.
**************************************************************************/
void do_seek( char_data *ch, char *argument )
{
    char arg1[MIL];
    CClanType* pClan;

    argument = one_argument(argument, arg1);

    if ( !str_cmp(arg1,"none") ) {
		ch->seeks = NULL;
        ch->println( "You are seeking no clan." );
        return;
    }

    if ( ch->clan ) {
        ch->println( "You are already in a clan." );
        return;
    }

    if ( (pClan = clan_lookup(arg1)) == NULL ) {
		ch->println( "No such clan exists." );
        return;
    }

    ch->seeks = pClan;

	ch->printlnf( "You are seeking to join %s.",
		pClan->name() );
	ch->println("(type 'seek none' to stop seeking a clan.)");

    return;
}
/**************************************************************************/
const char * CClanType::name()
{
	if(!this){
		return "";		
	}
	return m_pName;
}
/**************************************************************************/
const char * CClanType::cname()
{
	if(!this){
		return "";		
	}
	static char result[MIL];
	sprintf(result, "`#%s%s`&",	m_pColorStr, m_pName);

	return result;
}
/**************************************************************************/
// name used to save to disk, never changes
const char * CClanType::savename()
{
	if(!this){
		return "";		
	}
	return m_pSaveName;
}
/**************************************************************************/
const char * CClanType::notename()
{
	if(!this){
		return "";		
	}
	return m_pNoteName;
}
/**************************************************************************/
const char * CClanType::color_str()
{
	if(!this){
		return "";		
	}
	return m_pColorStr;
}
/**************************************************************************/
const char * CClanType::who_name()
{
	if(!this){
		return "";		
	}
	return m_pWhoName;
}
/**************************************************************************/
const char * CClanType::cwho_name()
{
	if(!this){
		return "";		
	}
	static char result[MIL];
	sprintf(result, "`#%s%s`&",	m_pColorStr, m_pWhoName);

	return result;
}
/**************************************************************************/
const char * CClanType::who_cat()
{
	if(!this){
		return "";		
	}
	return m_pWhoCat;
}
/**************************************************************************/
const char * CClanType::clan_rank_title(int rank)
{
	return m_pClanRankTitle[rank];
}
/**************************************************************************/
int CClanType::recall_room()
{
	return m_RecallRoom;
}
/**************************************************************************/
bool CClanType::hidden()
{
	return m_Hidden;
}
/**************************************************************************/
// Kal - Sept 2000
int CClanType::rank_lookup(const char *ranktitle)
{
	int rnk;
	if(this==NULL){ // clanless
		return 0;		
	}

	for (rnk = 0; rnk < MAX_RANK; rnk++)
	{
		if (!str_prefix(ranktitle,m_pClanRankTitle[rnk])){
			return rnk;
		}
	}
    return -1;
}
/**************************************************************************/
CClanType* clan_lookup( const char *name )
{
    CClanType *pClan;

	if(IS_NULLSTR(name)){
		return NULL;
	}

	// first try an exact match
	for(pClan=clan_list;pClan;pClan=pClan->next){
		if(!str_cmp(name,pClan->name() )){
			return pClan;
		}
	}

	// now try a prefix match
	for(pClan=clan_list;pClan;pClan=pClan->next){
		if(!str_prefix(name,pClan->name())){
			return pClan;
		}
	}

	// not found
    return NULL;
}
/**************************************************************************/
// lookup a clan by their note name - Kalahn oct97
CClanType* clan_nlookup( const char *name )
{
    CClanType *pClan;

	// first try an exact match
	for(pClan=clan_list;pClan;pClan=pClan->next){
		if(!str_cmp(name,pClan->notename())){
			return pClan;
		}
	}

	// now try a prefix match
	for(pClan=clan_list;pClan;pClan=pClan->next){
		if(!str_prefix(name,pClan->notename())){
			return pClan;
		}
	}

	// not found
    return NULL;
}
/**************************************************************************/
// lookup a clan by their save name - Kalahn Sept2000
CClanType* clan_slookup( const char *savename )
{
    CClanType *pClan;

	// only do exact matching
	for(pClan=clan_list;pClan;pClan=pClan->next){
		if(!str_cmp(savename,pClan->savename())){
			return pClan;
		}
	}

	// not found
    return NULL;
}
/**************************************************************************/    
void clan_bank( char_data *ch, char *task, char *amount)
{
	int number;
	if(IS_NPC(ch)){
		ch->println("clan banking by players only sorry.");
		return;
	}
	
	if(!str_prefix(task,"balance") )
	{
		ch->printlnf( "Your clan has %ld gold in its coffers.", ch->clan->m_BankFunds );

		// the clanbank transaction trail
		append_datetimestring_to_file( CLANBANKING_FILE, 
			FORMATF("%s[%d] got a clanbank balance of %s - balance is %d`x",
			TRUE_CH(ch)->name,
			ch->clanrank,
			ch->clan->who_name(),
			(int)ch->clan->m_BankFunds));

		return;
	}     
	
	if(!is_number(amount))
	{
		ch->printlnf( "clanbank: The second argument must be a number, '%s' is not.", amount );
		return;
	}

	number=atoi(amount);
	if(number<=0)
	{
		ch->println( "clanbank: The second argument must be a number greater than 0." );
		return;
	}
	if(number>1250000){
		ch->println( "clanbank: Sorry, we don't deal with such large amounts!" );
		return;
	}
	
	if(!str_prefix(task,"deposit"))
	{
		if( number>ch->gold )
		{
			
			if( number>ch->gold ){
				ch->println( "You do not have that much money." );
			}
			return;
		}
		ch->gold -= number;
		ch->clan->m_BankFunds += number;
		ch->printlnf( "Deposit made.\r\nYour clan now has %ld gold pieces in its coffers.",
			ch->clan->m_BankFunds );

		logf("[clanbank] %s deposited %d gold into clan bank account %s - balance now %d",
			ch->name,
			number,
			ch->clan->who_name(),
			(int)ch->clan->m_BankFunds);

		// the clanbank transaction trail
		append_datetimestring_to_file( CLANBANKING_FILE, 
			FORMATF("%s[%d] deposited %d gold into clan bank account %s - balance now %d`x",
			TRUE_CH(ch)->name,
			ch->clanrank,
			number,
			ch->clan->who_name(),
			(int)ch->clan->m_BankFunds));

		// resave the clan database
		save_clan_db();		
		save_char_obj(ch); // resave them
		WAIT_STATE(ch, PULSE_PER_SECOND*3);
		return;
	}
	
    if(!str_prefix(task,"withdraw"))
	{
		if ( !ch->clan->m_CanWithdraw[ch->clanrank])
		{
			// the clanbank transaction trail
			append_datetimestring_to_file( CLANBANKING_FILE, 
				FORMATF("%s[%d] attempted to withdraw %d gold from clan bank account %s but didn't have authority - balance is %d`x",
				TRUE_CH(ch)->name,
				ch->clanrank,
				number,
				ch->clan->who_name(),
				(int)ch->clan->m_BankFunds));
			ch->println( "You do not have the authority to withdraw money from your clan." );
			return;
		}
		
		if( number > ch->clan->m_BankFunds )
		{
			ch->println("You do not have that much in the bank.");
			return;
		}
		ch->gold += number;
		ch->clan->m_BankFunds -= number;
		ch->printlnf( "Withdrawal made.  Your clan now has %ld gold pieces in its coffers.",
			ch->clan->m_BankFunds );
		
		logf("[clanbank] %s withdrew %d gold from clan bank account %s - balance now %d",
			ch->name,
			number,
			ch->clan->who_name(),
			(int)ch->clan->m_BankFunds);

		// the clanbank transaction trail
		append_datetimestring_to_file( CLANBANKING_FILE, 
			FORMATF("%s[%d] withdrew %d gold from clan bank account %s - balance now %d`x",
			TRUE_CH(ch)->name,
			ch->clanrank,
			number,
			ch->clan->who_name(),
			(int)ch->clan->m_BankFunds));

		// resave the clan database
		save_clan_db();
		save_char_obj(ch); // resave them
		WAIT_STATE(ch, PULSE_PER_SECOND*3);
		return;
	}
	
	ch->printlnf( "'%s' is not a valid clanbank transaction.", task );
	
	return;
}
/**************************************************************************/
void do_disbandclan( char_data *ch, char *argument )
{
	char_data	*vch;

	CClanType 	*pClan;
	CClanType 	*pC;

	if ( ch->clan == NULL )
	{
		ch->println("You are not in a clan.");
		return;
	}

	if (!is_name( ch->name, ch->clan->m_pFounder))
	{
		ch->println("You are not the clan leader.");
		return;
	}

	if(str_cmp("confirm", argument)){
		ch->println("Type `=Cdisbandclan confirm`x to disband your clan.");
		return;
	}

	pClan = ch->clan;
	
	ch->desc->pEdit = (void *)pClan;
	ch->desc->editor = ED_CLAN;

	for(vch=player_list; vch; vch=vch->next_player)
	{
		if(IS_NPC(vch))
			continue;

		if ( vch->clan == pClan )
		{
			vch->clan	= NULL;
			vch->clanrank = 0;
			vch->println("Your clan has been disbanded");
			if ( vch->in_room->vnum == vch->clan->recall_room() ) 
			{
				act("$n vanishes suddenly!",vch,NULL,NULL,TO_ROOM);
				char_from_room( vch );
				char_to_room( vch, get_room_index( 3000 ) );
				act("$n materializes before you.",vch, NULL, NULL, TO_ROOM);
				do_look(vch, "auto");
			}
		}
	}

	EDIT_CLAN( ch, pClan);

	if ( clan_list == pClan )
	{
		clan_list = pClan->next;
		free(pClan);
		edit_done ( ch );
		return;
	}

	for ( pC = clan_list; pC; pC = pC->next )
	{
		if ( pC->next == pClan )
		{
			pC->next = pClan->next;
			free(pClan);
			edit_done( ch );
			return;
		}
	}

	save_clan_db();
	return;
}
/**************************************************************************/
void do_rankclan( char_data *ch, char *argument )
{
	char 		arg[MIL];
	CClanType 	*pC;

	if ( ch->clan == NULL )
	{
		ch->println("You are not in a clan.");
		return;
	}

	if (!is_name( ch->name, ch->clan->m_pFounder))
	{
		ch->println("You are not the clan leader.");
		return;
	}

	pC = ch->clan;

    	if ( IS_NULLSTR(argument))
    	{
		ch->printlnf("Syntax: rankclan [number 2-%d] [title]", MAX_RANK);
		// Ranks
		ch->println("`b/---------------------------------------------------------\\`=r");
		ch->println("`b|   `=rTitle                `b|`=rAdd    `b|`=rPromote`b|`=rOutcast`b|`=rWithdraw`b|`=r");
		ch->println("`b|---------------------------------------------------------|`=r");
	
		int lIdx = 0;

		for ( lIdx = 0; lIdx < MAX_RANK; lIdx++ )
		{
			ch->printlnf("`b|`x%d. %-20s `b|`x%-1s      `b|`x%-1s      `b|`x%-1s      `b|`x%-1s       `b|`=r",
			lIdx+1,
			pC->m_pClanRankTitle[lIdx], 
			pC->m_CanAdd[lIdx] ? "`GY`=r" : "`RN`=r",
			pC->m_CanPromote[lIdx] ? "`GY`=r" : "`RN`=r",
			pC->m_CanRemove[lIdx] ? "`GY`=r" : "`RN`=r",
			pC->m_CanWithdraw[lIdx] ? "`GY`=r" : "`RN`=r");
		}
		ch->println("`b\\---------------------------------------------------------/`=r");
		ch->println("(`xRank 1 is the highest rank, promote includes demote.`=r)");
		return;
    	}

	if ( IS_NULLSTR(argument) || !is_number(arg) )
	{
		ch->println("First argument must be a rank number, the second the title.");
		return;
	}

	if ( atoi(arg) < 2 || atoi(arg) > MAX_RANK)
	{
		ch->printlnf( "The valid range for ranks is between `#`W2`^ and `W%d`^.", MAX_RANK);
		return;
	}
	
    	EDIT_CLAN( ch, pC );

    	free_string( pC->m_pClanRankTitle[atoi(arg)-1] );
	pC->m_pClanRankTitle[atoi(arg)-1] = str_dup(argument);
	ch->printlnf ( "Rank %d is now known as %s.", atoi(arg), argument );

	edit_done( ch );
	save_clan_db();
	return;
}
/**************************************************************************/
void do_whoclan( char_data *ch, char *argument )
{
	char		*buf=NULL;
	CClanType 	*pClan;

	if ( ch->clan == NULL )
	{
		ch->println("You are not in a clan.");
		return;
	}

	if (!is_name( ch->name, ch->clan->m_pFounder))
	{
		ch->println("You are not the clan leader.");
		return;
	}

	pClan = ch->clan;


    	EDIT_CLAN( ch, pClan );
	
    	if ( argument[0] == '\0' )
    	{
		ch->println( "Syntax: whoclan [name]." );
		return;
    	}
	
    	free_string( pClan->m_pWhoName );

	sprintf(buf, "`#`W[`c%s`W]`^", argument);
    	pClan->m_pWhoName = str_dup( buf );
	
    	ch->printlnf("Clan who name set to %s.", buf);

	edit_done( ch );
	save_clan_db();
	return;
}
/**************************************************************************/ 
void do_foundclan( char_data *ch, char *argument )
{   
	int 		votes=0;
	char 		*arg="";
	char_data 	*mob=NULL;

	one_argument( argument, arg );

    	if ( IS_NULLSTR(arg))
    	{
        	ch->println("Syntax: foundclan <one word name>");
		return;
	}

	if(ch->clan)
	{
        	ch->println("You are already in a clan.");
		return;
	}

    	for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    	{
        	if ( IS_NPC(mob) && IS_SET(mob->act2, ACT2_CLANMAKER) )
            		break;
    	}

    	if ( mob == NULL )
    	{
        	ch->println("You can't do that here.");
        	return;
    	}

	char *p = arg;

	while( *p )
	{
		if ( !isalpha(*p))
		{
			act("$N tells you 'Clan names may only use alphabet characters.'", ch, NULL, mob, TO_CHAR );
			return;
		}
		++p;
	}

	if ( (clan_lookup( arg )) != NULL )
	{
		act("$N tells you 'I already have such a clan registered.'", ch, NULL, mob, TO_CHAR);
		return;
	}

	if ( IS_NPC(ch) )
	{
		ch->println("NPCs cannot make clans.");
		return;
   	}

    	int cost = 1500000;

    	if (cost > (ch->gold * 100 + ch->silver))
    	{
		act("$N says 'You don't have 15000 gold to start a clan.'", ch,ch,mob,TO_CHAR);
		return;	
    	}

	char_data *gch;

	int vote = 0;

	for ( gch = char_list; gch; gch = gch->next )
	{
		if ( is_same_group( gch, ch ) && ch->in_room == gch->in_room)
		{
			if(HAS_CONFIG2(gch, CONFIG2_VOTEYES))
			{
				votes ++;
			}
			else if(!HAS_CONFIG2(gch, CONFIG2_VOTEYES) && !HAS_CONFIG2(gch, CONFIG2_VOTENO))
			{
				gch->printlnf("%s is wanting to found a clan. Please `#`cVOTECLAN YES `^or `cVOTECLAN NO`^.", ch->name);
			}
		}
	}

	if(vote <= 5)
	{
		act("$N tells you 'You do not have enough votes in your group to start a clan.'", ch, NULL, mob, TO_CHAR);
		return;
	}

	deduct_cost(ch,cost);

	CClanType*		node;
	static CClanType	zero_node;

	node		= new CClanType;
	*node		= zero_node;
	node->next	= clan_list;
	clan_list	= node;

	replace_string(clan_list->m_pSaveName, arg ); // set it once, doesn't change
	replace_string(clan_list->m_pName, arg );	
	replace_string(clan_list->m_pFounder, ch->name );
	replace_string(clan_list->m_pNoteName, arg );
	replace_string(clan_list->m_pWhoName, arg );
	replace_string(clan_list->m_pWhoCat, "" );
	replace_string(clan_list->m_pDescription, "" );
	replace_string(clan_list->m_pColorStr, "`W" );
	clan_list->m_RecallRoom			= ROOM_VNUM_LIMBO;
	clan_list->m_BankRoom			= 0;
	clan_list->m_BankFunds			= 0;	// No starting money for em! :)

    	free_string( clan_list->m_pClanRankTitle[0] );
	clan_list->m_pClanRankTitle[0] = str_dup("`#`YFounder`^");
	save_clan_db();

	act("$N tells you 'I have registered your new clan.'", ch, NULL, mob, TO_CHAR);
	act("$N says 'Congratulations! I have registered the new clan.'", ch, NULL, mob, TO_CHAR);

	for ( gch = char_list; gch; gch = gch->next )
	{
		if(gch == ch)
		{
			gch->printlnf("You are now a member of clan %s.", node->cname() );
			gch->clan  = node;
			gch->clanrank = 0;
		}

		if ( is_same_group( gch, ch ) && ch->in_room == gch->in_room)
		{
			gch->printlnf("You are now a member of clan %s.", node->cname() );
			gch->clan  = node;
			gch->clanrank = node->minRank();
		}
	}
	return;
}
/**************************************************************************/
void do_voteclan( char_data *ch, char *argument )
{   
	if(IS_NPC(ch))
	{
		return;
	}

    	if ( IS_NULLSTR(argument))
    	{
        	ch->println("Syntax: voteclan <yes/no>");
		return;
	}

	if(ch->clan)
	{
        	ch->println("You are already in a clan.");
		return;
	}
	
	if(!str_cmp("yes", argument))
	{
		SET_CONFIG2(ch, CONFIG2_VOTEYES);
		REMOVE_CONFIG2(ch, CONFIG2_VOTENO);
		ch->println("Your vote for a new clan is YES.");
		return;
	}

	if(!str_cmp("no", argument))
	{
		SET_CONFIG2(ch, CONFIG2_VOTENO);
		REMOVE_CONFIG2(ch, CONFIG2_VOTEYES);;
		ch->println("Your vote for a new clan is NO.");
		return;
	}

       	ch->println("Syntax: voteclan <yes/no>");
	return;
}
/**************************************************************************/
int get_direction( char *arg );
EXTRA_DESCR_DATA * dup_extdescr_list(EXTRA_DESCR_DATA * descript);
void save_area( AREA_DATA *pArea );
/**************************************************************************/
bool clan_room_create( char_data *ch, int value)
{
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoom;
    int iHash;
    
    pArea = get_vnum_area( value );
    if ( !pArea )
    {
        ch->println( "MINE ERROR:  That vnum is not assigned an area." );
	return false;
    }
	
    if ( get_room_index( value ) )
    {
        ch->println( "MINE ERROR:  Room vnum already exists." );
	return false;
    }
	
    pRoom		= new_room_index();
    pRoom->area		= pArea;
    pRoom->vnum		= value;
	
    if ( value > top_vnum_room )
        top_vnum_room = value;

    iHash			= value % MAX_KEY_HASH;
    pRoom->next			= room_index_hash[iHash];
    room_index_hash[iHash]	= pRoom;
    ch->desc->pEdit		= (void *)pRoom;
    return true;
}
/*****************************************************************************/
static bool clan_room_exit( char_data *ch, char *argument, int door )
{
	ROOM_INDEX_DATA *pRoom=NULL;
	ROOM_INDEX_DATA *newRoom=NULL;
	ROOM_INDEX_DATA *pToRoom=NULL;
	ROOM_INDEX_DATA *pSrc=NULL;
 	AREA_DATA *pArea;
    	char command[MIL];
   	char arg[MIL];
	int rev;
    	int value, foundroom;
    	int newvnum;
    	newvnum = 0;
    	foundroom = 0;
    	pArea = ch->in_room->area;

	
	// record the full argument incase exit flags are being specified
    char fullarg[MIL];
	strcpy(fullarg, argument);
	
	// get the first arg, check if it is a command
    argument = one_argument( argument, command );
    one_argument( argument, arg );
	
	// no arguments - normal move 
    if ( IS_NULLSTR(command))	
    {
		move_char( ch, door, true, NULL );                    
		return false;
    }
	
	// help
    if ( command[0] == '?' )
    {
		do_help( ch, "OLC-EXIT" );
		return false;
    }
	
	// editing commands
	ch->desc->editor = ED_ROOM;
	EDIT_ROOM(ch, pRoom);
    if ( !str_cmp( command, "remove" ) || !str_cmp( command, "delete" ))
    {
		ROOM_INDEX_DATA *pToRoom;
		sh_int rev;
		
		if ( !pRoom->exit[door] )
		{
			return false;
		}
		
		/*
		* Remove ToRoom Exit if it leads to current room.
		*/
		rev = rev_dir[door];
		pToRoom = pRoom->exit[door]->u1.to_room;
		
		if ( pToRoom && pToRoom->exit[rev] && pToRoom->exit[rev]->u1.to_room==pRoom)
		{
			free_exit( pToRoom->exit[rev] );
			pToRoom->exit[rev] = NULL;
		}
		
		/*
		* Remove this exit.
		*/
		free_exit( pRoom->exit[door] );
		pRoom->exit[door] = NULL;
		
		return true;
	}
	
    if ( !str_cmp( command, "link" ) )
    {
		EXIT_DATA *pExit;
		
		if ( arg[0] == '\0' || !is_number( arg ) )
		{
			return false;
		}
		
		value = atoi( arg );
		
		if ( !get_room_index( value ) )
		{
			return false;
		}
		
		if ( get_room_index( value )->exit[rev_dir[door]] )
		{
			return false;
		}
		
		if ( !pRoom->exit[door] )
		{
			pRoom->exit[door] = new_exit();
		}
		
		pRoom->exit[door]->u1.to_room = get_room_index( value );

		
		pRoom                   = get_room_index( value );
		door                    = rev_dir[door];
		pExit                   = new_exit();
		pExit->u1.to_room       = ch->in_room;
		pRoom->exit[door]       = pExit;

		// Mark the area the reverse door went on as changed - Kal Feb 01
		if(pRoom->area){ 
			SET_BIT( pRoom->area->olc_flags, OLCAREA_CHANGED );
		}
		return true;
	}

	if (!str_cmp (command, "dig")) 
	{
		if(IS_NULLSTR(arg))
		{
			// find the first unused room vnum in the area
			int newvnum =  ch->in_room->area->min_vnum-1; 
			pToRoom=ch->in_room; // make sure the while loop actually starts
			while(pToRoom){
				newvnum++;
				if (newvnum >  ch->in_room->area->max_vnum){ 
					ch->printlnf("Dig Error: No more free room vnums in area range %d to %d.",
						ch->in_room->area->min_vnum,
						ch->in_room->area->max_vnum); 
					return false; 
				}
				pToRoom = get_room_index(newvnum);
			}

			// create a room with that vnum, and link it in 
			clan_room_create(ch, newvnum);         

			pSrc = get_room_index(ROOM_CLAN_DEFAULT);
			newRoom = get_room_index(newvnum);

			// copy the object details
			newRoom->name		= str_dup(pSrc->name);
			newRoom->description	= str_dup(pSrc->description);
			newRoom->owner		= str_dup(pSrc->owner);

			newRoom->room_flags	= pSrc->room_flags;
			newRoom->room2_flags	= pSrc->room2_flags;
			newRoom->sector_type	= pSrc->sector_type;
			newRoom->heal_rate	= pSrc->heal_rate;
			newRoom->mana_rate	= pSrc->mana_rate;
			newRoom->clan		= pSrc->clan;

			// COPY THE EXTENDED DESCRIPTIONS 
			newRoom->extra_descr	= dup_extdescr_list(pSrc->extra_descr);
			clan_room_exit(ch, FORMATF("link %d", newvnum), door); 
			return true;        
		} 
		else {
		ch->println("ERROR ROOM NOT DUG"); }
	}
	
    if ( !str_cmp( command, "name" ) )
    {
		if ( arg[0] == '\0' )
		{
            ch->println( "Syntax:  <direction> name <string>" );
            ch->println( "         <direction> name none" );
			return false;
		}
		
		if ( !pRoom->exit[door] )
		{
            ch->println( "That exit doesn't exist." );
			return false;
		}
		
		if (str_cmp(arg,"none")){
	        ch->printlnf( "Exit name changed from '%s' to '%s'.",
				pRoom->exit[door]->keyword, arg);
			replace_string(pRoom->exit[door]->keyword, arg );
		}else{
	        ch->printlnf( "Exit name cleared (was '%s').",
				pRoom->exit[door]->keyword);
			replace_string(pRoom->exit[door]->keyword, "" );
		}
		return true;
    }
	
    if ( !str_prefix( command, "description" ) )
    {
		if ( arg[0] == '\0' )
		{
			if ( !pRoom->exit[door] )
			{
				ch->println( "That exit doesn't exist." );
				return false;
			}
			
			if(ch->desc && pRoom->area){ 
				// if the string as changed, this is used to flag the area file saving
				ch->desc->changed_flag=&pRoom->area->olc_flags;
			}
			string_append( ch, &pRoom->exit[door]->description );
			return true;
		}
		
        ch->println( "Syntax:  [direction] desc" );
		return false;
    }
	
	
    // Set the exit flags, needs full argument.
    // ----------------------------------------    
    if ( ( value = flag_value( exit_flags, fullarg) ) != NO_FLAG )
    {
		
		if ( !pRoom->exit[door] )
		{
            ch->println( "Exit doesn't exit." );
			return false;
		}
		
		// This room.
		TOGGLE_BIT(pRoom->exit[door]->rs_flags, value);
		
		// Don't toggle exit_info because it can be changed by players.
		pRoom->exit[door]->exit_info = pRoom->exit[door]->rs_flags;
		
		if(value==EX_ONEWAY){
			if(IS_SET(pRoom->exit[door]->rs_flags, EX_ONEWAY)){
				ch->println( "Oneway exit flag set" );
			}else{
				ch->println( "Oneway exit flag removed" );
			}
		}else{
			if (pRoom->exit[door]->rs_flags && 
				!IS_SET(pRoom->exit[door]->rs_flags , EX_ISDOOR) )
			{
				SET_BIT(pRoom->exit[door]->rs_flags , EX_ISDOOR);
				ch->println( "Door flag added on this side of door." );
				if (IS_SET(value, EX_ISDOOR))
					ch->println( "note: to remove door exit flag, remove all other exit flags first." );
			}
			
			/*
			* Connected room.
			*/
			pToRoom = pRoom->exit[door]->u1.to_room;
			rev = rev_dir[door];
			
			if(pToRoom->exit[rev] && pToRoom->exit[rev]->u1.to_room==pRoom){
				if (IS_SET(pRoom->exit[door]->rs_flags,  value)){
					SET_BIT(pToRoom->exit[rev]->rs_flags,  value);
				}else{
					REMOVE_BIT(pToRoom->exit[rev]->rs_flags,  value);
				}
				pToRoom->exit[rev]->exit_info = pToRoom->exit[rev]->rs_flags;

				if (pToRoom->exit[rev]->rs_flags && 
					!IS_SET(pToRoom->exit[rev]->rs_flags , EX_ISDOOR) )
				{
					SET_BIT(pToRoom->exit[rev]->rs_flags , EX_ISDOOR);
					ch->println( "Door flag added on OTHER side of door." );
					if (IS_SET(value, EX_ISDOOR))
						ch->println( "note: to remove door exit flag, remove all other exit flags first." );
				}
				// Mark the area the reverse door went on as changed - Kal Feb 01
				if(pToRoom->area){ 
					SET_BIT( pToRoom->area->olc_flags, OLCAREA_CHANGED );
				}
				ch->println( "Exit flag toggled this side, set/removed on otherside." );
			}else{
				ch->printf("`RWarning:`x The exit going %s from %d (here) to %d doesn't link 2 ways.\r\n"
					"i.e. The exit going %s from %d doesn't go to %d...\r\n"
					"Therefore the exit flags have not been set on that side.\r\n",
					dir_name[door], pRoom->vnum, pToRoom->vnum, 
					dir_name[rev], pToRoom->vnum, pRoom->vnum);
			}			
		}
		return true;
    }
    ch->wrapln("Unrecognised exit command/exit flags, read help `=_OLC-EXIT for "
		"more help on olc exits and olc exit flags.");
	ch->wraplnf("Exit flags include: %s", flag_string(exit_flags, -1));
    return false;
}
/**************************************************************************/
void do_buildclan(char_data *ch, char *argument)
{
    char arg[MIL];

    argument = one_argument( argument, arg );

    if ( !is_name( ch->in_room->area->file_name, "clanhalls.are" ) )
    {
	ch->printlnf("You cannot build a building here.");
	return;
    }

    int door = get_direction(arg);

    if (door==-1 || door==4 || door==6 || door==7|| door==8|| door==9)
    {
	ch->printlnf("Unknown direction to build. Mine N,S,E,W,D.");
	return;
    }

    if(ch->in_room->exit[door] != NULL)
    {
	ch->println("You cannot build there!");
	return;
    }

    ch->println("You begin to dig....");
 
	clan_room_exit(ch, FORMATF("dig"), door);
	edit_done(ch);
	save_area( ch->in_room->area);
	return;
}
/**************************************************************************/

