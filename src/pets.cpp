/**************************************************************************/
// pets.cpp - Pet Code by Ixliam
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/

#include "include.h"
#include "areas.h"
#include "olc.h"
#include "security.h"
/**************************************************************************/
#define ROOM_MINE_DEFAULT     	65001
#define VNUM_ORE	     	65000
//#define ORE_GOLD	     	65001
//#define ORE_COPPER	     	65002
//#define ORE_SILVER	     	65003
//#define ORE_IRON	     	65004
//#define ORE_MITHRIL	     	65005
//#define ORE_ADAMANTIUM	     	65006
#define GEM_1	     		65007
#define GEM_2	     		65008
#define GEM_3	     		65009
#define GEM_4	     		65010
#define GEM_5	     		65011
#define GEM_6	     		65012
#define GEM_7	     		65013
#define GEM_8	     		65014
#define GEM_9	     		65015
#define GEM_10	     		65016
//#define ORE_VAMPIRIC   		65017

#define CAVE_MOB_1		65000
#define CAVE_MOB_2		65001
#define CAVE_MOB_3		65002
#define CAVE_MOB_4		65003
#define CAVE_MOB_5		65004

/**************************************************************************/
void fwrite_obj  args( ( OBJ_DATA  *obj, FILE *fp, int iNest, char *heading) );
int get_direction( char *arg );
EXTRA_DESCR_DATA * dup_extdescr_list(EXTRA_DESCR_DATA * descript);
void save_area( AREA_DATA *pArea );
/**************************************************************************/
bool room_create( char_data *ch, int value)
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
static bool room_exit( char_data *ch, char *argument, int door )
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
			room_create(ch, newvnum);         

			pSrc = get_room_index(ROOM_MINE_DEFAULT);
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
			room_exit(ch, FORMATF("link %d", newvnum), door); 
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
void fread_newpet( char_data *ch, FILE *fp )
{
    	char *word;
    	char_data *pet;
    	bool fMatch;
    	time_t lastlogoff = current_time;
    	int percent;
	bool non_existant_pet=false;
	int pet_vnum=0;
	
    // first entry had BETTER be the vnum or we barf 
	word = feof(fp) ? (char*)"END" : fread_word(fp);
    if (!str_cmp(word,"Vnum"))
    {				
		pet_vnum= fread_number(fp);
		if (get_mob_index(pet_vnum) == NULL)
		{
			bugf("Fread_pet: bad vnum %d.",pet_vnum);
			non_existant_pet=true;
			pet = create_mobile(limbo_mob_index_data, 0);
		}else{
			pet = create_mobile(get_mob_index(pet_vnum), 0);
		}
    }
    else
    {
		bug("Fread_pet: no vnum in file.");
		pet = create_mobile(limbo_mob_index_data, 0);
    }
    
    for ( ; ; )
    {
		word    = feof(fp) ? (char*)"END" : fread_word(fp);
		fMatch = false;
		
		switch (UPPER(word[0]))
		{
		case '*':
			fMatch = true;
			fread_to_eol(fp);
			break;
			
		case 'A':
			KEY( "Act",         pet->act,               fread_flag(fp));
			KEY( "AfBy",        pet->affected_by,       fread_flag(fp));
			KEY( "AffBy",       pet->affected_by,       fread_wordflag(affect_flags, fp));
			KEY( "AffBy2",      pet->affected_by2,      fread_wordflag(affect2_flags,fp));			
			KEY( "AffBy3",      pet->affected_by3,      fread_wordflag(affect3_flags,fp));			
			KEY( "Alig",        pet->alliance,          3*fread_number(fp)/1000);
			
			if (!str_cmp(word,"Algn"))
			{
				pet->tendency=fread_number(fp);
				pet->alliance=fread_number(fp);
				fMatch = true;
				break;
			}   
			
			if (!str_cmp(word,"ACs"))
			{
				int i;
				
				for (i = 0; i < 4; i++)
					pet->armor[i] = fread_number(fp);
				fMatch = true;
				break;
			}
				
			if (!str_cmp(word,"Affc"))
			{
				AFFECT_DATA *paf;
				int sn;
				
				paf = new_affect();
				
				sn = skill_lookup(fread_word(fp));
				if (sn < 0)
					bug("Fread_char: unknown skill.");
				else
					paf->type = sn;
				
				paf->where      = fread_number(fp);
				paf->level      = fread_number(fp);
				paf->duration   = fread_number(fp);
				paf->modifier   = fread_number(fp);
				paf->location   = translate_old_apply_number(fread_number(fp));
				paf->bitvector  = fread_number(fp);
				paf->next       = pet->affected;
				pet->affected   = paf;
				fMatch          = true;
				break;
			}
			
			if (!str_cmp(word,"AMod"))
			{
				int stat;
				if(ch->version>8)
				{
					for (stat = 0; stat < MAX_STATS; stat++)
						pet->modifiers[stat] = fread_number(fp);
				}
				fMatch = true;
				break;
			}
			
			if (!str_cmp(word,"Attr"))
			{
				int stat;
				if(ch->version>8)
				{
					for (stat = 0; stat < MAX_STATS; stat++)
						pet->perm_stats[stat] = fread_number(fp);
				}
				fMatch = true;
				break;
			}
			break;
			
		case 'C':
			KEY( "Clan",       pet->clan,       clan_slookup(fread_string(fp)));
			KEY( "Color1",     pet->petcolor1,       fread_string(fp));
			KEY( "Color2",     pet->petcolor2,       fread_string(fp));
			KEY( "Color3",     pet->petcolor3,       fread_string(fp));
			KEY( "Color4",     pet->petcolor4,       fread_string(fp));
			KEY( "Comm",       pet->comm,              fread_flag(fp));			
			KEY( "Com",        pet->comm,              fread_wordflag(comm_flags, fp));						
			break;
			
		case 'D':
			KEY( "Dam",        pet->damroll,           fread_number(fp));
			KEY( "Desc",       pet->description,       fread_string(fp));
			break;
			
		case 'E':
			if (!str_cmp(word,"End"))
			{
				if(non_existant_pet){
					logf("Removing pet from %s, because the vnum of it couldn't be located.", PERS(ch, NULL));
					ch->println("`R********************************************************************************");
					ch->printlnf("`R*`W Your pet vnum %5d couldn't be located for loading, contact the admin. `R*", pet_vnum);
					ch->println("`R********************************************************************************`x");
				}else{
					pet->leader = ch;
					pet->master = ch;
					ch->pet = pet;
					/* adjust hp mana move up  -- here for speed's sake */
					percent = (int)(current_time - lastlogoff) * 25 / ( 2 * 60 * 60);
					
					if (percent > 0 && !IS_AFFECTED(ch,AFF_POISON)
						&&  !IS_AFFECTED(ch,AFF_PLAGUE))
					{
						percent = UMIN(percent,100);
						pet->hit    += (pet->max_hit - pet->hit) * percent / 100;
						pet->mana   += (pet->max_mana - pet->mana) * percent / 100;
						pet->move   += (pet->max_move - pet->move)* percent / 100;
					}
				}
				return;
			}
			KEY( "Exp",        pet->exp,               fread_number(fp));
			break;
			
		case 'G':
			KEY( "Gold",       pet->gold,              fread_number(fp));
			break;
			
		case 'H':
			KEY( "Hit",        pet->hitroll,           fread_number(fp));
			
			if (!str_cmp(word,"HMV"))
			{
				pet->hit        = fread_number(fp);
				pet->max_hit    = fread_number(fp);
				pet->mana       = fread_number(fp);
				pet->max_mana   = fread_number(fp);
				pet->move       = fread_number(fp);
				pet->max_move   = fread_number(fp);
				fMatch = true;
				break;
			}
			break;
			
		case 'L':
			KEY( "Levl",       pet->level,             fread_number(fp));
			KEY( "LnD",        pet->long_descr,        fread_string(fp));
			KEY( "LogO",       lastlogoff,             fread_number(fp));
			break;
		
		case 'M':
			KEY( "MAct",       pet->act,              fread_wordflag(act_flags,fp));
			KEY( "MTknown",    pet->magic_trick,     fread_number(fp));
			KEY( "Mtricks",    pet->pet_magic_flags, fread_wordflag(pet_magic_flags,fp));
			break;

		case 'N':
			KEY( "Name",       pet->name,              fread_string(fp));
			KEY( "NTknown",    pet->norm_trick,   fread_number(fp));
			KEY( "Ntricks",    pet->pet_norm_flags, fread_wordflag(pet_norm_flags,fp));

			break;
			
		case 'P':
			KEY( "Pos",        pet->position,          fread_number(fp));
			KEY( "Petbirth",   pet->petbirth,             fread_number(fp));
			
		case 'R':
			//	    KEY( "Race",        pet->race, race_lookup(fread_string(fp)));
			if ( !str_cmp( word, "Race" ) )
			{
				ch->race = race_lookup(fread_string( fp ));
				if (ch->race == -1)
				{
					ch->race = race_lookup("human");
					bugf("fread_newpet: pet '%s' has an invalid race!!!",
						ch->name);
				}
				fMatch = true;
			}
			
			
			if ( !str_cmp( word, "Room" ) )
			{
				pet->in_room = get_room_index(ROOM_VNUM_LIMBO);
				fMatch = true;
			}
			
			break;
			
		case 'S' :
			KEY( "Save",        pet->saving_throw,      fread_number(fp));
			KEY( "Sex",         pet->sex,               fread_number(fp));
			KEY( "ShD",         pet->short_descr,       fread_string(fp));
			KEY( "Silv",        pet->silver,            fread_number( fp ) );
			break;
			
			if ( !fMatch )
			{
				bug("Fread_pet: no match.");
				fread_to_eol(fp);
			}
			
		}
    }

    char_to_room( pet, ch->in_room );

    pet->comm = COMM_NOTELL|COMM_NOSHOUT|COMM_NOCHANNELS;
    SET_BIT(pet->act, ACT_PET);
    SET_BIT(pet->affected_by, AFF_CHARM);
    SET_BIT(pet->dyn, DYN_IS_BEING_ORDERED);
    add_follower( pet, ch );
    pet->leader = ch;
    ch->pet = pet;

    return;
}
/**************************************************************************/
// write a pet 
void fwrite_newpet( char_data *pet, FILE *fp)
{
	AFFECT_DATA *paf;
    
	fprintf(fp,"Vnum %d\n",pet->pIndexData->vnum);
	
	fprintf(fp,"Name %s~\n", pet->name);
    fprintf(fp,"LogO %ld\n", (long) current_time);
    if (pet->short_descr != pet->pIndexData->short_descr)
		fprintf(fp,"ShD  %s~\n", fix_string(pet->short_descr));
    if (pet->long_descr != pet->pIndexData->long_descr)
		fprintf(fp,"LnD  %s~\n", fix_string(pet->long_descr));
	if (pet->description != pet->pIndexData->description)
		fprintf(fp,"Desc %s~\n", fix_string(pet->description));
    if (pet->race != pet->pIndexData->race)
		fprintf(fp,"Race %s~\n", race_table[pet->race]->name);
	if (pet->clan){
		fprintf( fp, "Clan %s~\n",pet->clan->savename());
	}
	fprintf(fp,"Sex  %d\n", pet->sex);
	if (pet->level != pet->pIndexData->level)
		fprintf(fp,"Levl %d\n", pet->level);
	fprintf(fp, "HMV  %d %d %d %d %d %d\n",
		pet->hit, pet->max_hit, pet->mana, pet->max_mana, pet->move, pet->max_move);
	if (pet->gold > 0)
		fprintf(fp,"Gold %ld\n",pet->gold);
	if (pet->silver > 0)
		fprintf(fp,"Silv %ld\n",pet->silver);
	
	fprintf( fp, "Room %d\n",
		(  pet->in_room == get_room_index( ROOM_VNUM_LIMBO )
		&& pet->was_in_room != NULL )
		? pet->was_in_room->vnum
		: pet->in_room == NULL ? ROOM_VNUM_LIMBO : pet->in_room->vnum );
	
	if (pet->exp > 0)
		fprintf(fp, "Exp  %d\n", pet->exp);
    if (pet->act != pet->pIndexData->act){
		fwrite_wordflag( act_flags, pet->act, "MAct ", fp); 
	}
    if (pet->affected_by != pet->pIndexData->affected_by){
		fwrite_wordflag( affect_flags, pet->affected_by, "AffBy ", fp); 
	}
    if (pet->affected_by2 != pet->pIndexData->affected_by2){
		fwrite_wordflag( affect2_flags, pet->affected_by2, "AffBy2 ", fp); 
	}
	if (pet->affected_by3 != pet->pIndexData->affected_by3){
		fwrite_wordflag( affect3_flags, pet->affected_by3, "AffBy3 ", fp); 
	}
	if (pet->comm != 0){
		fwrite_wordflag( comm_flags, pet->comm, "Com ", fp); 
	}

	fprintf(fp,"Pos  %d\n", pet->position = POS_FIGHTING ? POS_STANDING : pet->position);

  	if(pet->petcolor1 != NULL){
	    fprintf(fp,"Color1 %s\n", pet->petcolor1);
	}
  	if(pet->petcolor2 != NULL){
	    fprintf(fp,"Color2 %s\n", pet->petcolor2);
	}
  	if(pet->petcolor3 != NULL){
	    fprintf(fp,"Color3 %s\n", pet->petcolor3);
	}
  	if(pet->petcolor4 != NULL){
	    fprintf(fp,"Color4 %s\n", pet->petcolor4);
	}

	fprintf(fp,"Petbirth %ld\n", pet->petbirth);
	
	if(pet->norm_trick){
		fprintf(fp,"NTknown  %d\n", pet->norm_trick);
	}
	if(pet->magic_trick){
		fprintf(fp,"MTknown  %d\n", pet->magic_trick);
	}

    	if (pet->pet_norm_flags != 0){
		fwrite_wordflag( pet_norm_flags, pet->pet_norm_flags, "Ntricks ", fp); 
	}
    	if (pet->pet_magic_flags != 0){
		fwrite_wordflag( pet_magic_flags, pet->pet_magic_flags, "Mtricks ", fp); 
	}
    
    if (pet->saving_throw != 0)
		fprintf(fp, "Save %d\n", pet->saving_throw);
    if (pet->alliance != pet->pIndexData->alliance ||
		pet->tendency != pet->pIndexData->tendency)
		fprintf(fp, "Algn %d %d\n", pet->tendency, pet->alliance);
    if (pet->hitroll != pet->pIndexData->hitroll)
		fprintf(fp, "Hit  %d\n", pet->hitroll);
	if (pet->damroll != pet->pIndexData->damage[DICE_BONUS])
		fprintf(fp, "Dam  %d\n", pet->damroll);
    fprintf(fp, "ACs  %d %d %d %d\n",
		pet->armor[0],pet->armor[1],pet->armor[2],pet->armor[3]);
	fprintf(fp, "Attr %d %d %d %d %d %d %d %d %d %d\n",
		pet->perm_stats[STAT_ST], pet->perm_stats[STAT_QU],
		pet->perm_stats[STAT_PR], pet->perm_stats[STAT_EM],
		pet->perm_stats[STAT_IN],pet->perm_stats[STAT_CO], 
		pet->perm_stats[STAT_AG],
		pet->perm_stats[STAT_SD], pet->perm_stats[STAT_ME],
		pet->perm_stats[STAT_RE]);
	fprintf(fp, "AMod %d %d %d %d %d %d %d %d %d %d\n",
		pet->modifiers[STAT_ST], pet->modifiers[STAT_QU],
		pet->modifiers[STAT_PR], pet->modifiers[STAT_EM],
		pet->modifiers[STAT_IN], 
		pet->modifiers[STAT_CO], pet->modifiers[STAT_AG],
		pet->modifiers[STAT_SD], pet->modifiers[STAT_ME],
		pet->modifiers[STAT_RE] );
	
	
	for ( paf = pet->affected; paf != NULL; paf = paf->next )
    {
		if (paf->type < 0 || paf->type >= MAX_SKILL)
			continue;
		
		fprintf(fp, "Affc '%s' %3d %3d %3d %3d %3d %10d\n",
			skill_table[paf->type].name,
			paf->where, paf->level, paf->duration, paf->modifier,paf->location,
			paf->bitvector);
    }
    
	fprintf(fp,"End\n");
	return;
}
/**************************************************************************/
bool save_pet(char_data *ch)
{
	FILE *fp;
	char write_filename[MIL];
	char pet_filename[MIL];
	char_data *pet;
	int x=1;	

	for (x=1; x; x++)
	{
		sprintf(write_filename, "%s%s%d.write", PET_DIR, ch->name, x);
		sprintf(pet_filename, "%s%s%d.txt", PET_DIR, ch->name, x);
		if(!file_exists(pet_filename))
		{
			break;
		}
		if(x==4)
		{
			return false;
		}
	}
	
	unlink(write_filename);

	fclose( fpReserve );
    	fp = fopen( write_filename, "w" );

	pet = ch->pet;

	fwrite_newpet(pet, fp);
	if ( pet->carrying != NULL )
	{
		fwrite_obj( ch->pet->carrying, fp, 0, "PO" ); // pet objects
	}
	fprintf(fp, "\n");

	int bytes_written=fprintf(fp, "EOF\n");
	fclose( fp );
	if(   bytes_written != str_len("EOF\n") ){
		ch->printlnf("Incomplete write of %s, write aborted - check diskspace! - error %d (%s)", 
			write_filename, errno, strerror( errno));
	    fpReserve = fopen( NULL_FILE, "r" );
	}else{
		unlink(pet_filename);
		rename(write_filename, pet_filename);
	}

	if(x==1)
		ch->pet1 = pet->short_descr;
	if(x==2)
		ch->pet2 = pet->short_descr;
	if(x==3)
		ch->pet3 = pet->short_descr;
	if(x==4)
		ch->pet4 = pet->short_descr;

    	fpReserve = fopen( NULL_FILE, "r" );
	extract_char(pet,true);
	ch->pet = NULL;
	return true;
}
/**************************************************************************/
void list_pet(char_data *ch)
{
	char pet_file1[MIL];
	char pet_file2[MIL];
	char pet_file3[MIL];
	char pet_file4[MIL];
	bool found = false;

	sprintf(pet_file1, "%s%s1.txt", PET_DIR, ch->name);
	sprintf(pet_file2, "%s%s2.txt", PET_DIR, ch->name);
	sprintf(pet_file3, "%s%s3.txt", PET_DIR, ch->name);
	sprintf(pet_file4, "%s%s4.txt", PET_DIR, ch->name);

	ch->titlebar("-= PETS IN KENNEL =-");

	if(file_exists(pet_file1))
	{	
		found = true;
	 	ch->printlnf("`s1> `CPet: `c%s", ch->pet1);
	}
	if(file_exists(pet_file2))
	{	
		found = true;
	 	ch->printlnf("`s2> `CPet: `c%s", ch->pet2);
	}
	if(file_exists(pet_file3))
	{	
		found = true;
	 	ch->printlnf("`s3> `CPet: `c%s", ch->pet3);
	}
	if(file_exists(pet_file4))
	{	
		found = true;
	 	ch->printlnf("`s4> `CPet: `c%s", ch->pet4);
	}


	if (found == false)
		ch->println("None.");
	return;
}
/**************************************************************************/
bool load_pet(char_data *ch, char *num)
{
	FILE *fp;
	char write_filename[MIL];
	
	sprintf(write_filename, "%s%s%s.txt", PET_DIR, ch->name, num);

	if(!file_exists(write_filename))
	{
		ch->println("Cannot find file.");
		return false;
	}

	fclose( fpReserve );
    	if ( ( fp = fopen( write_filename, "r" ) ) == NULL ){
		bugf("load_pet(): fopen '%s' failed for read - error %d (%s)",
			write_filename, errno, strerror( errno));
	    fpReserve = fopen( NULL_FILE, "r" );
		ch->println("Error in load_pet.");
		return false;
    	}

	fread_newpet( ch, fp );

	for ( ; ; )
	{
		char *word;
		word = fread_word( fp );
		if ( !str_cmp( word, "PO") )
		{
			OBJ_DATA *o=fread_obj( fp, write_filename);
			if(o && ch->pet)
			{
				obj_to_char(o, ch->pet);
			}			
		}else if ( !str_cmp( word, "EOF"    ) )
		{
			break;
		}
	}

	fclose(fp);
	fpReserve = fopen( NULL_FILE, "r" );
	unlink(write_filename);
	remove(write_filename);
	char_to_room( ch->pet, ch->in_room );
	return true;
}
/**************************************************************************/
void do_kennel(char_data *ch, char *argument)
{
    char_data *mob;
    char arg1[MIL];
    char arg2[MIL];
	
    if (ch->position == POS_SLEEPING)
    {
	ch->println("You can't do that while sleeping.");
        return;
    }
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
        if ( IS_NPC(mob) && IS_SET(mob->act2, ACT2_KENNEL) )
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

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_NULLSTR(arg1))
    {
	act("$N says 'I can LIST, STORE, or RELEASE your pets.'",ch,NULL,mob,TO_CHAR);
	act("$N says 'It will cost 20 gold to store your pet.'",ch,NULL,mob,TO_CHAR);
	return;
    }

    if (!str_prefix(arg1,"list"))
    {
	act("$N says 'Here is what you have in our kennels.'",ch,NULL,mob,TO_CHAR);
	list_pet(ch);
	return;
    }

    if (!str_prefix(arg1,"store"))
    {
	int cost  = 2000;
	if (cost > (ch->gold * 100 + ch->silver))
	{
		act("$N says 'You do not have enough gold to store your pet.'",ch,NULL,mob,TO_CHAR);
		return;
	}

	if(!ch->pet)
	{
		act("$N says 'You don't even have a pet!'",ch,NULL,mob,TO_CHAR);
		return;
	}

	if(ch->pet->in_room != ch->in_room)
	{
		act("$N says 'Your pet is not here.'",ch,NULL,mob,TO_CHAR);
		return;
	}

	if(!IS_SET(ch->pet->act, ACT_PET) || IS_SET(ch->pet->act, ACT_AGGRESSIVE))
	{
		act("$N says 'You pet cannot be kept here!'",ch,NULL,mob,TO_CHAR);
		return;
	}

	if (save_pet(ch) == true)
	{
		act("$N says 'I will keep you pet safe here.'",ch,NULL,mob,TO_CHAR);
		deduct_cost(ch,cost);
		mob->gold += cost;
		limit_mobile_wealth(mob);
	}
	else
		act("$N says 'You will need to remove on of your other pets first. No more room.'",ch,NULL,mob,TO_CHAR);
	return;
    }

    if (!str_prefix(arg1,"release"))
    {
	if (IS_NULLSTR(arg1))
    	{
		act("$N says 'You need to give me the number of the pet you want to release.'",ch,NULL,mob,TO_CHAR);
		return;
    	}

	if(ch->pet)
    	{
		act("$N says 'You already have a pet!'",ch,NULL,mob,TO_CHAR);
		return;
    	}

	if (load_pet(ch, arg2) == true) 
	{
		act("$N says 'Enjoy your pet.'",ch,NULL,mob,TO_CHAR);
	    	act( "$n now follows you.", ch->pet, NULL, ch, TO_VICT );
	}
	else
		act("$N says 'You do not have a pet with that number.'",ch,NULL,mob,TO_CHAR);
	return;	
    }

    act("$N says 'I can LIST, STORE, or REMOVE your pets.'",ch,NULL,mob,TO_CHAR);
    return;
}
/**************************************************************************/
bool pet_train (char_data * ch, int failure)
{
	if ( failure > 0 )
	{
		act( "You cannot seem to make your pet understand you.", ch, NULL, NULL, TO_CHAR );
		if ( !is_affected( ch, gsn_cause_headache ))
		{
			AFFECT_DATA af;

			ch->println( "Your head seems to explode with a sudden wave of indescribable pain!" );
			af.where		= WHERE_MODIFIER;
			af.type			= gsn_cause_headache;
			af.level		= ch->level/2;
			af.duration		= 5;
			af.location		= APPLY_SD;
			af.modifier		= - ch->level/10;
			af.bitvector	= 0;
			affect_to_char( ch, &af );
		}
		return false;
	}
	return true;
}
/**************************************************************************/
#define PET_BALL        	3155



/**************************************************************************/
void do_pet(char_data *ch, char *argument)
{
    char_data *pet;
    char arg1[MIL];
    char arg2[MIL];
    OBJ_DATA *obj=NULL;
    int modifier = 0;
    int failure = 0;
    int percent  = number_percent();
	
    if (ch->position == POS_SLEEPING)
    {
	ch->println("You can't do that while sleeping.");
        return;
    }
    if (!ch->pet)
    {
        ch->println("You do not have a pet right now.");
        return;
    }
    if(!IS_SET(ch->pet->act, ACT_PET))
   {
        ch->println("You do not have a pet right now.");
        return;
    }

    if ( ch->fighting ) {
	ch->println("You seem to be too busy right now.");
	return;
    }

    pet = ch->pet;

    obj = get_eq_char(ch, WEAR_HOLD);

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (!str_prefix(arg1,"train"))
    {
    	if (ch->pet->in_room != ch->in_room)
    	{
        	ch->println("Your pet is not here.");
	        return;
    	}

	modifier = ( ch->level - pet->level );
	if ( modifier > 0 ) modifier *= 3;
	else modifier *=5;
	modifier += 50;	
	modifier += (( ch->modifiers[STAT_EM] +  ch->modifiers[STAT_EM] +ch->modifiers[STAT_PR] )/3);
	modifier += ((( get_skill( ch, gsn_animal_training) - 50 ) / 5) + (get_skill(ch, gsn_animal_training) > 0) ? 5 : 0);
	failure = percent - modifier;

	if (!str_prefix(arg2,"fetch") && obj != NULL && obj->pIndexData->vnum == PET_BALL)
        {
	
		if(IS_SET(pet->pet_norm_flags, PET_NORMTRICK_1))
		{
			ch->printlnf("%s already knows how to fetch.", capitalize(ch->pet->short_descr));
			return;
		}

		if(pet_train(ch, failure) == false)
		{
			ch->printlnf("You encourage %s to go fetch %s as you throw it.", capitalize(ch->pet->short_descr), obj->short_descr);
			act( "$n throws $p on the ground.", ch, obj, pet, TO_ROOM );
			obj_from_char(obj);
			obj_to_room(obj, ch->in_room);
			ch->printlnf("%s doesn't understand what you are trying to teach.", capitalize(ch->pet->short_descr));
			act( "$n attempts to train $M with $p.", ch, obj, pet, TO_ROOM );
			return;	
		}
		
		SET_BIT(pet->pet_norm_flags, PET_NORMTRICK_1);
		ch->printlnf("You encourage %s to go fetch %s as you throw it.", capitalize(ch->pet->short_descr), obj->short_descr);
		act( "$n throws $p on the ground.", ch, obj, pet, TO_ROOM );
		act( "$M runs and grabs $p, and happily brings it back to $n.", ch, obj, pet, TO_ROOM );
		ch->printlnf("%s runs and grabs %s, and happily brings it to you.", capitalize(ch->pet->short_descr), obj->short_descr);
		ch->println("You pet has learned the trick FETCH!!");
		return;
	} // Fetch



    }

    if (!str_prefix(arg1,"whistle"))
    {
	ch->printlnf("You whistle loudly for %s.", capitalize(ch->pet->short_descr));
	act( "$n whistles loudly for $M.", ch, obj, pet, TO_ROOM );
	char_from_room( pet );
	char_to_room( pet, ch->in_room );
	return;	
    }

    if (!str_prefix(arg1,"trick"))
    {
    	if (ch->pet->in_room != ch->in_room)
    	{
        	ch->println("Your pet is not here.");
	        return;
    	}
	if (!str_prefix(arg2,"fetch") && obj != NULL && obj->pIndexData->vnum == PET_BALL)
        {
		if(!IS_SET(pet->pet_norm_flags, PET_NORMTRICK_1))
		{
			ch->printlnf("%s doesn't know how to fetch.", capitalize(ch->pet->short_descr));
			return;
		}
		obj_from_char(obj);
		obj_to_char(obj, pet);
		ch->printlnf("You throw %s for %s to fetch.", obj->short_descr, capitalize(ch->pet->short_descr));
		act( "$n throws $p on the ground for $M to fetch.", ch, obj, pet, TO_ROOM );
		act( "$N runs and grabs $p, and happily brings it back to $n.", ch, obj, pet, TO_ROOM );
		ch->printlnf("%s runs and grabs %s, and happily brings it back to you.", capitalize(ch->pet->short_descr), obj->short_descr);
		return;
	} // Fetch

    }

    ch->printlnf("%s looks at you in confusion.", capitalize(ch->pet->short_descr));
    return;
}
/**************************************************************************/
void mine_mob (char_data *ch, int vnum)
{
    char_data *mob;
    int level, leveladj, bonus;

	leveladj = number_range(1,10);
	bonus = number_range(1,4);
	if(bonus == 1){
		level = ch->level - leveladj;
	}
	else { level = ch->level + leveladj; }
	if(bonus == 2)
		level = ch->level - 1;	

	mob = create_mobile( get_mob_index(vnum),0);
	mob->level = level;
	bonus = number_range(25,200);
	mob->max_hit = ch->max_hit + bonus;
	mob->hit = mob->max_hit;
 	reset_char(mob);
	char_to_room( mob, ch->in_room );
	act( "You have uncovered $n in your digging!", mob, NULL, NULL, TO_ROOM );
	update_pos(mob);
	act( "$n hisses at you and attacks!!", mob, NULL, NULL, TO_ROOM );
	multi_hit( ch, mob, TYPE_UNDEFINED );
	return;
}
/**************************************************************************/
void do_mine(char_data *ch, char *argument)
{
    char arg[MIL];
    OBJ_DATA *obj=NULL;
    OBJ_DATA *ore=NULL;
    int percent = 0;
    bool fshovel = false;

    obj = get_eq_char(ch, WEAR_HOLD);
    argument = one_argument( argument, arg );

    if ( !is_name( ch->in_room->area->file_name, "mines.are" ) )
    {
	ch->printlnf("You cannot mine here.");
	return;
    }

    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
        if ( !str_cmp( obj->pIndexData->material, "shovel" )
  	     && ( obj->wear_loc == WEAR_HOLD ||  obj->wear_loc == WEAR_WIELD )) 
	{
		fshovel = true;
           	break;
	}
    }

    if ( !fshovel ) 
    {
	ch->printlnf("You are not holding a shovel or a good mining pick.");
	return;
    }

    if(ch->in_room->vnum==65000 || ch->in_room->vnum==65001)
    {
	ch->println("You must go deeper into the mine first.");
	return;
    }

    int door = get_direction(arg);

    if (door==-1 || door==4 || door==6 || door==7|| door==8|| door==9)
    {
	ch->printlnf("Unknown direction to mine. Mine N,S,E,W,D.");
	return;
    }

    if(ch->in_room->exit[door] != NULL)
    {
	ch->println("You cannot mine air!");
	return;
    }

    ch->println("You begin to dig....");

    if ( !IS_IMMORTAL( ch ))
    	WAIT_STATE(ch, 30);

    gain_condition( ch, COND_FULL, ch->size > SIZE_MEDIUM ? -4 : -2 );
    gain_condition( ch, COND_THIRST, -1 );
    gain_condition( ch, COND_HUNGER, ch->size > SIZE_MEDIUM ? -2 : -1);

    percent = number_range(1, 10);
    if (percent <= 7) 
    {
    	ch->println("You dig and dig and seem to get nowhere.");
	act( "$n digs into the dirt with $p.", ch, obj, NULL, TO_ROOM );
	return;
    }
    percent = number_range(1, 15);

    if(percent == 1) // Room Cavein
    {
	int dam=0;
	dam = number_range( 20, 150 );
	if ( dam < 0 )
		dam = 0;
    	ch->println("`#`RThe ceiling caves in on you as you start to dig.`^");
	act( "$n's `#`Rdigging has caused a cave in!!`^", ch, obj, NULL, TO_ROOM );

   	for(char_data *vch = player_list; vch; vch = vch->next_player)
   	{
		if(ch->in_room == vch->in_room)
		{	
			vch->println("`#`yRocks and dirt tumble all around you!!`^");
			vch->hit -= dam;
			if (( IS_NPC(vch) || IS_IMMORTAL( vch )) &&   vch->hit < 1 )
				vch->hit = 1;
			update_pos( vch );
		}
   	}
	return;
    }

    if (percent <= 5) // Dig Room
    {
  	ch->pcdata->tired ++;
    	switch ( door )
    	{
		case 0: 
		   	ch->println("You dig the mine shaft further north.");
			act( "$n digs a tunnel north with $p.", ch, obj, NULL, TO_ROOM );
			break;
		case 1: 
		   	ch->println("You dig the mine shaft further east.");
			act( "$n digs a tunnel east with $p.", ch, obj, NULL, TO_ROOM );
			break;
		case 2:
		   	ch->println("You dig the mine shaft further south.");
			act( "$n digs a tunnel south with $p.", ch, obj, NULL, TO_ROOM );
			break;
		case 3:
		   	ch->println("You dig the mine shaft further west.");
			act( "$n digs a tunnel west with $p.", ch, obj, NULL, TO_ROOM );
			break;
		case 4:
		   	ch->println("You dig a mine shaft up above you.");
			act( "$n digs a tunnel up with $p.", ch, obj, NULL, TO_ROOM );
			break;
		case 5:
		   	ch->println("You dig a mine shaft below you.");
			act( "$n digs a tunnel down with $p.", ch, obj, NULL, TO_ROOM );
			break;
		case 6:
		   	ch->println("You dig the mine shaft further northeast.");
			act( "$n digs a tunnel northeast with $p.", ch, obj, NULL, TO_ROOM );
			break;
		case 7:
		   	ch->println("You dig the mine shaft further southeast.");
			act( "$n digs a tunnel southeast with $p.", ch, obj, NULL, TO_ROOM );
			break;
		case 8:
		   	ch->println("You dig the mine shaft further southwest.");
			act( "$n digs a tunnel southwest with $p.", ch, obj, NULL, TO_ROOM );
			break;
		case 9:
		   	ch->println("You dig the mine shaft further northwest.");
			act( "$n digs a tunnel northwest with $p.", ch, obj, NULL, TO_ROOM );
			break;
    	}
	room_exit(ch, FORMATF("dig"), door);
	edit_done(ch);
//	save_area( ch->in_room->area);
	return;
    }

    percent = number_range(1, 31);
    int cost = 0;
    int vampore = 0;
    ch->pcdata->tired ++;
    obj_data *o=NULL;
	ore = create_object(get_obj_index(VNUM_ORE));
	free_string( ore->name );
	free_string( ore->short_descr );
	free_string( ore->description );
	free_string( ore->material );
    switch ( percent )
    {
	case 1:
		ore->name = str_dup("dirt");
		ore->short_descr = str_dup("a pile of dirt");
		ore->description = str_dup("a pile of dirt lays here.");
		ore->material = str_dup("dirt");
		ore->value[0] = MATERIAL_ORE;
		ore->value[1] = 0;
		ore->value[2] = 100;
		ore->value[3] = 100;
		ch->printlnf("You dig and uncover %s.", ore->short_descr);
		act( "$n digs into the mine and uncovers $p!", ch, ore, NULL, TO_ROOM );
		cost = number_range(10, 5000);
		break;
	case 2:
		ore->name = str_dup("silver");
		ore->short_descr = str_dup("a silver ore");
		ore->description = str_dup("a silver ore lays here.");
		ore->material = str_dup("silver");
		ore->weight = number_range(50, 200);
		ore->value[0] = MATERIAL_ORE;
		ore->value[1] = ORE_SILVER;
		ore->value[2] = number_percent();
		ore->value[3] = number_percent();
		ch->printlnf("You dig and uncover %s.", ore->short_descr);
		act( "$n digs into the mine and uncovers $p!", ch, ore, NULL, TO_ROOM );
		cost = number_range(1, 5);
		break;
	case 3:
		ore->name = str_dup("gold");
		ore->short_descr = str_dup("a gold ore");
		ore->description = str_dup("a golden ore lays here.");
		ore->material = str_dup("gold");
		ore->weight = number_range(50, 200);
		ore->value[0] = MATERIAL_ORE;
		ore->value[1] = ORE_GOLD;
		ore->value[2] = number_percent();
		ore->value[3] = number_percent();
		ch->printlnf("You dig and uncover %s.", ore->short_descr);
		act( "$n digs into the mine and uncovers $p!", ch, ore, NULL, TO_ROOM );
		cost = number_range(1, 150);
		break;
	case 4:
		ore->name = str_dup("adamantium");
		ore->short_descr = str_dup("an adamantium ore");
		ore->description = str_dup("an adamantium ore lays here.");
		ore->material = str_dup("adamantium");
		ore->weight = number_range(10, 50);
		ore->value[0] = MATERIAL_ORE;
		ore->value[1] = ORE_ADAMANTIUM;
		ore->value[2] = number_percent();
		ore->value[3] = number_percent();
		ch->printlnf("You dig and uncover %s.", ore->short_descr);
		act( "$n digs into the mine and uncovers $p!", ch, ore, NULL, TO_ROOM );
		cost = number_range(1, 50);
		break;
	case 5:
		ore->name = str_dup("orchalicum");
		ore->short_descr = str_dup("an orchalicum ore");
		ore->description = str_dup("an orchalicum ore lays here.");
		ore->material = str_dup("orchalicum");
		ore->weight = number_range(30, 180);
		ore->value[0] = MATERIAL_ORE;
		ore->value[1] = ORE_ORCHALICUM;
		ch->printlnf("You dig and uncover %s.", ore->short_descr);
		act( "$n digs into the mine and uncovers $p!", ch, ore, NULL, TO_ROOM );
		cost = number_range(10, 50);
		break;
	case 6:
		ore->name = str_dup("mithril");
		ore->short_descr = str_dup("a mithril ore");
		ore->description = str_dup("a mithril ore lays here.");
		ore->material = str_dup("mithril");
		ore->weight = number_range(30, 180);
		ore->value[0] = MATERIAL_ORE;
		ore->value[1] = ORE_MITHRIL;
		ch->printlnf("You dig and uncover %s.", ore->short_descr);
		act( "$n digs into the mine and uncovers $p!", ch, ore, NULL, TO_ROOM );
		cost = number_range(10, 200);
		break;
	case 7:
		ore->name = str_dup("iron");
		ore->short_descr = str_dup("an iron ore");
		ore->description = str_dup("an iron ore lays here.");
		ore->material = str_dup("iron");
		ore->weight = number_range(20, 100);
		ore->value[0] = MATERIAL_ORE;
		ore->value[1] = ORE_IRON;
		ch->printlnf("You dig and uncover %s.", ore->short_descr);
		act( "$n digs into the mine and uncovers $p!", ch, ore, NULL, TO_ROOM );
		cost = number_range(10, 200);
		break;
	case 8:
		ore = create_object(get_obj_index(GEM_1));
		ch->printlnf("You dig and uncover %s.", ore->short_descr);
		act( "$n digs into the mine and uncovers $p!", ch, ore, NULL, TO_ROOM );
		cost = number_range(20, 150);
		break;
	case 9: 
		ore = create_object(get_obj_index(GEM_2));
		ch->printlnf("You dig and uncover %s.", ore->short_descr);
		act( "$n digs into the mine and uncovers $p!", ch, ore, NULL, TO_ROOM );
		cost = number_range(10, 45);
		break;
	case 10: 
		ore = create_object(get_obj_index(GEM_3));
		ch->printlnf("You dig and uncover %s.", ore->short_descr);
		act( "$n digs into the mine and uncovers $p!", ch, ore, NULL, TO_ROOM );
		cost = number_range(10, 50);
		break;
	case 11:
		ore = create_object(get_obj_index(GEM_4));
		ch->printlnf("You dig and uncover %s.", ore->short_descr);
		act( "$n digs into the mine and uncovers $p!", ch, ore, NULL, TO_ROOM );
		cost = number_range(10, 40);
		break;
	case 12:
		ore = create_object(get_obj_index(GEM_5));
		ch->printlnf("You dig and uncover %s.", ore->short_descr);
		act( "$n digs into the mine and uncovers $p!", ch, ore, NULL, TO_ROOM );
		cost = number_range(1, 200);
		break;
	case 13:
		ore = create_object(get_obj_index(GEM_6));
		ch->printlnf("You dig and uncover %s.", ore->short_descr);
		act( "$n digs into the mine and uncovers $p!", ch, ore, NULL, TO_ROOM );
		cost = number_range(2, 78);
		break;
	case 14:
		ore = create_object(get_obj_index(GEM_7));
		ch->printlnf("You dig and uncover %s.", ore->short_descr);
		act( "$n digs into the mine and uncovers $p!", ch, ore, NULL, TO_ROOM );
		cost = number_range(5, 20);
		break;
	case 15:
		ore = create_object(get_obj_index(GEM_8));
		ch->printlnf("You dig and uncover %s.", ore->short_descr);
		act( "$n digs into the mine and uncovers $p!", ch, ore, NULL, TO_ROOM );
		cost = number_range(10, 90);
		break;
	case 16:
		ore = create_object(get_obj_index(GEM_9));
		ch->printlnf("You dig and uncover %s.", ore->short_descr);
		act( "$n digs into the mine and uncovers $p!", ch, ore, NULL, TO_ROOM );
		cost = number_range(0, 20);
		break;
	case 17:
		ore = create_object(get_obj_index(GEM_10));
		ch->printlnf("You dig and uncover %s.", ore->short_descr);
		act( "$n digs into the mine and uncovers $p!", ch, ore, NULL, TO_ROOM );
		cost = number_range(1, 75);
		break;
	case 18:
		mine_mob(ch, CAVE_MOB_1);
		return;
		break;
	case 19:
		mine_mob(ch, CAVE_MOB_2);
		return;
		break;
	case 20:
		mine_mob(ch, CAVE_MOB_3);
		return;
		break;
	case 21:
		mine_mob(ch, CAVE_MOB_4);
		return;
		break;
	case 22:
		mine_mob(ch, CAVE_MOB_5);
		return;
		break;
	case 23:
		vampore=number_range(1,2);
		if(vampore==1)
		{
			ore = create_object(get_obj_index(VNUM_ORE));
			ch->printlnf("You dig and uncover %s.", ore->short_descr);
			act( "$n digs into the mine and uncovers $p!", ch, ore, NULL, TO_ROOM );
			cost = number_range(1, 75);
		} else {
		mine_mob(ch, CAVE_MOB_5);
		return; }
		break;
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:
		o = get_obj_list( ch, "dirt", ch->in_room->contents );
    		if ( o == NULL )
    		{
			ore = create_object(get_obj_index(VNUM_ORE));
			ch->println("You dig and knock lose a large pile of dirt.");
			act( "$n digs into the mine knocks loose a large chunk of dirt!", ch, ore, NULL, TO_ROOM );
			break;
		}
		ch->println("You dig and knock lose a large pile of dirt.");
		act( "$n digs into the mine knocks loose a large chunk of dirt!", ch, ore, NULL, TO_ROOM );
		return;
		break;
    }

    ore->cost = cost;
    obj_to_room(ore, ch->in_room);
    return;
}
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
