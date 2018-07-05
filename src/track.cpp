/**************************************************************************/
// track.cpp - Incomplete track system written by Kal
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/
#include "include.h"
#include "track.h"

void resort_tracks();

DECLARE_DO_FUN(do_open);

#define ch2int(ch) (*((int *)ch))

int rooms_with_tracks=0;
unsigned short tracktime=1500;
C_track_table *track_table=NULL;

#define	MTPR	MAX_TRACKS_PER_ROOM
/**************************************************************************/
void init_track_table()
{
	log_string("init_track_table():: Initialising track_table...");
	track_table=new C_track_table;

	if(MAX_RACE>32000){
		bugf( __FILE__":init_track_table() - MAX_RACE is set to %d, which is higher than 32000...\n"
			"Track wont function correctly in this environment without modifications.  Aborting startup.", MAX_RACE);
		exit_error( 1 , "init_track_table", "MAX_RACE too high");
	}
	log_string("init_track_table():: track_table initialised.");
}
/**************************************************************************/
// constructor 
C_track_table::C_track_table()
{
	// NULL out all the tracks to start with 
	for(int i=0;i<MTC; i++){
		character[i]=NULL;
		race_oldchar[i]=0;
	}
	total_tracked_characters=0;
	next_free_track=0;
}
/**************************************************************************/
// returns true if the character field is pointing to a valid char_data ch
// - this will only be true if that ch is in the game currently
bool C_track_table::is_active(int index)
{		
	if(IS_SET(race_oldchar[index], 0x8000)){
		return false;
	}else{
		if(character[index]==NULL){
			return false;
		}
		return true;
	}
}
/**************************************************************************/
int C_track_table::add_char(char_data *ch)
{	
	int trackindex=next_free_track;
	character[trackindex]=ch; 
	// MSB (most significant bit) of race_oldchar is only set to 1 after
	// the character has been freed, then the character pointer no longer
	// points to something of char_data but becomes a flags field recording
	// info about what the character was e.g. npc/pc, immortal etc.
	race_oldchar[trackindex]=ch->race & 0x7FFF; // ensure MSB is off for now

	total_tracked_characters++;

	// find where our next free track index is 
	int count=0;
	do{	++next_free_track%=MTC;
		count++;
		if(count>MTC){
			bugf( __FILE__":C_track_table::add_char() - MTC is set to %d which is less than", MTC);
			bug("the number of players + mobs in the game!  Track can't run unless MTC is increased");
			bug("(MTC is short for MAX_TRACKABLE_CHARACTERS_IN_GAME which is set in params.h)");
			bug("Increase it by say 2500, do a clean recompile then restart the mud.)");
			do_abort();
		}
	}while(is_active(next_free_track));

	return (trackindex);
}
/**************************************************************************/
void C_track_table::del_char(int index)
{
	assert(index>=0);
	assert(index<MTC);

	if(is_active(index)){
		SET_BIT(race_oldchar[index], 0x8000); // mark the character field
												 // as unused
		// record what we want to record from the character
		// before it is freed
		if(IS_NPC(character[index])){
			ch2int(character[index])=0x01; // first bit records is_npc status
		}else{
			ch2int(character)=0x00;
			if(IS_IMMORTAL(character[index])){
				ch2int(character[index])|=0x02; // second bit records imm status
			}
		}
		total_tracked_characters--;
	}else{
		bugf(__FILE__":C_track_table::del_char(int) - index %d was previously "
			"deleted!", index);
	}
}
/**************************************************************************/
void C_track_table::del_char(char_data *ch)
{
	assert(ch->track_index>=0);
	assert(ch->track_index<MTC);
	
	if(character[ch->track_index]!=ch){
		bugf(__FILE__":C_track_table::del_char(CD*) - ch != character[ch->track_index]");
	}else{
		del_char(ch->track_index);
	}
}
/**************************************************************************/
int C_track_table::get_race_value(int index)
{
	assert(index>=0);
	assert(index<MTC);

	if(is_active(index)){ // temp hack till cleaned up
		race_oldchar[index]=character[index]->race & 0x7FFF; // ensure MSB is off for now
	}
	return (int)(race_oldchar[index] & 0x7fff); // result less the MSB
}
/**************************************************************************/
char *C_track_table::get_race(int index)
{
	assert(index>=0);
	assert(index<MTC);

	return (race_table[get_race_value(index)]->name); 
}
/**************************************************************************/
int C_track_table::get_total_tracked_characters()
{
	return total_tracked_characters;	
}

/**************************************************************************/
bool C_track_table::is_npc(int index)
{
	assert(index>=0);
	assert(index<MTC);

	if(is_active(index)){
		return (IS_NPC(character[index]));
	}else{ 
		// check bit 0 of the character field
		return(IS_SET(ch2int(character[index]),0x80)!=false);
	}
}
/**************************************************************************/
char_data * C_track_table::get_char(int index)
{
	assert(index>=0);
	assert(index<MTC);

	if(is_active(index)){
		return character[index];
	}else{
		return NULL;
	}
};
/**************************************************************************/
char * C_track_table::get_pers(int index, char_data *looker)
{
	assert(index>=0);
	assert(index<MTC);

	if(is_active(index)){
		return PERS(character[index], looker);
	}else{		
		return "(logged out or dead)";
	}
};
/**************************************************************************/


/**************************************************************************/
void tracktime_update()
{
	tracktime++;
	if(tracktime==65535){
		// this happens once every 4 days or so if PULSE_MINUTE is
		// once every 6 seconds.
		resort_tracks();
		tracktime=1500; // start as if there are 2.5 hours worth of tracks 
	}
}
/**************************************************************************/
void resort_tracks()
{
	bug("resort_tracks() called - not yet implemented");
};
/**************************************************************************/
void init_room_tracks()
{
	ROOM_INDEX_DATA *pRI;
    int iHash;

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
		for ( pRI= room_index_hash[iHash]; pRI; pRI=pRI->next )
		{			
			pRI->tracks=NULL; // add tracks as they are needed
		}
	}
}
/**************************************************************************/
// tracks for room constructor
C_track_data::C_track_data()
{
	memset(trackindex, 0, sizeof(trackindex));
	memset(time_of_track, 0, sizeof(time_of_track));
	memset(direction_type, 0, sizeof(direction_type));
	nexttrack=0;
}
/**************************************************************************/
tracktype C_track_data::get_tracktype(int index)
{
	return (tracktype)(direction_type[index]>>4);
};
/**************************************************************************/
void C_track_data::set_tracktype(int index, tracktype type)
{
	direction_type[index]= (direction_type[index] & 0x0F) + (type << 4);
};
/**************************************************************************/
int C_track_data::get_direction(int index)
{
	return (direction_type[index]&0x0F);
};
/**************************************************************************/
void C_track_data::set_direction(int index, int direction)
{
	direction_type[index]= (direction_type[index] & 0xF0) + (direction& 0x0F);
};
/**************************************************************************/
void C_track_data::add_track(char_data *ch, int direction, tracktype type)
{	
	assertp(ch->in_room); 
	if(this==NULL){
		// allocate memory for tracks when they are needed, not before
		ch->in_room->tracks=new C_track_data();	
		rooms_with_tracks++;
//		logf("allocating track memory for room %d", ch->in_room->vnum);		
		ch->in_room->tracks->add_track(ch, direction, type);
		return;
	}
	// ensure ch->in_room is the room we (this) belong to
	assert(ch->in_room->tracks==this); 

	// record the details of the track
	time_of_track[nexttrack]=tracktime;
	trackindex[nexttrack]=ch->track_index;

	// record the type and direction
	// type is converted if the general move into a more specific type
	if(type==TRACKTYPE_MOVE){
		if( (INVIS_LEVEL(ch)>=LEVEL_IMMORTAL) 
			|| IS_SET(TRUE_CH(ch)->act, PLR_HOLYWALK))
		{
			type=TRACKTYPE_WIZIIMM;
		}else if(IS_AFFECTED(ch, AFF_FLYING)){
			type=TRACKTYPE_FLY;
		}
		else if(IS_AFFECTED2(ch, AFF2_PASSWOTRACE)){
			if ( IS_OUTSIDE(ch)){
				type=TRACKTYPE_PASSWOTRACE;
			}else{
				type=TRACKTYPE_WALK;
			}
		}else if(IS_AFFECTED(ch, AFF_SNEAK)){
			type=TRACKTYPE_SNEAK;
		}else{
			type=TRACKTYPE_WALK;
		}
	}
	set_direction(nexttrack, direction);
	set_tracktype(nexttrack, type);

	++nexttrack%=MAX_TRACKS_PER_ROOM;
}
/**************************************************************************/
char *tracktype_name(tracktype type)
{
	switch(type){
	default: return "unknown type!!! - report the bug";
	case(TRACKTYPE_NONE			):	return "none";
	case(TRACKTYPE_MOVE			):	return "move";
	case(TRACKTYPE_FLY			):	return "fly";
	case(TRACKTYPE_SNEAK		):	return "sneak";
	case(TRACKTYPE_WALK			):	return "walk";
	case(TRACKTYPE_BLOODTRAIL	):	return "bloodtrail";
	case(TRACKTYPE_BLOODPOOL	):	return "bloodpool";
	case(TRACKTYPE_PASSWOTRACE	):	return "pass without trace";
	case(TRACKTYPE_WIZIIMM		):	return "wiziimm/holywalk";
	}
}
/**************************************************************************/
char *tracktype_age(int time)
{
	// time is between 0 and 600

	int sw; //=URANGE(6, time, 600)/50;

	if(time<2) sw=0;		// 12 seconds IRL	(2mins IC)
	else if(time<5) sw=1;   // 30 seconds IRL	(5mins IC)
	else if(time<10) sw=2;  // 1 minute IRL		(10minsIC)
	else if(time<20) sw=3;  // 2 minutes IRL	 (10		
	else if(time<40)  sw=4; // 4 minutes IRL	 (40mins IC)
	else if(time<70) sw=5;  // 7 minutes IRL	 (1.1hoursIC)
	else if(time<100) sw=6;  // 10 minutes IRL	 (1.6hoursIC)
	else if(time<150) sw=7;  // 15 minutes IRL   (2.5hoursIC)
	else if(time<225) sw=8;  // 22.5 minutes IRL (3,75hours IC)
	else if(time<300) sw=9;	 // 30 minutes IRL (5hours IC)
	else if(time<400) sw=10; // 40 minutes IRL (6.6hours IC)
	else if(time<500) sw=11; // 50 minutes IRL (8.3hours IC)
	else sw=12;

	switch(sw){
		default: return "";
		case(0): return "extremely fresh";
		case(1): return "very fresh";
		case(2): return "fresh";
		case(3): return "rather fresh";
		case(4): return "moderately fresh";
		case(5): return "fairly recent";
		case(6): return "recent";
		case(7): return "moderately recent";
		case(8): return "old";
		case(9): return "very old";
		case(10): return "extremely old";
		case(11): return "faintly visible";					  
		case(12): return "barely visible";
	}
}
/**************************************************************************/
void C_track_data::show_tracks(char_data *ch)
{
	assertp(ch->in_room); 
	int seen=0;
	int sect=ch->in_room->sector_type;

	int sk;
	int main_sn;
	switch(sect){
		case(SECT_INSIDE):
		case(SECT_CITY):
			sk=get_skill(ch, gsn_citytrack) + get_skill(ch, gsn_fieldtrack)/5;
			main_sn=gsn_citytrack;
			break;
		default:
			sk=get_skill(ch, gsn_fieldtrack) + get_skill(ch, gsn_citytrack)/8;
			main_sn=gsn_fieldtrack;
			break;
	}
	sk++; // everyone gets it basically
	if(sk<1){
		ch->println("What would you know about tracking in this terrain?");
		return;
	}
	if(this==NULL){
		if(IS_ICIMMORTAL(ch)){
			ch->println("No tracks in the room yet.");
		}else{
			ch->println("You failed to see any tracks here.");
			WAIT_STATE( ch, skill_table[main_sn].beats );	
		}
		return;
	}
	// ensure ch->in_room is the room we (this) belong to
	assert(ch->in_room->tracks==this); 

	if(!IS_IMMORTAL(ch)){
		if(IS_WATER_SECTOR(sect)){
			ch->println("You can't see tracks in the water.");
			return;
		}
		if(sect==SECT_AIR){
			ch->println("Tracks arent left in the air.");
			return;
		}
	}
	WAIT_STATE( ch, skill_table[main_sn].beats );	
	
	// loop thru displaying the track info in newest to oldest order
	for (int tempindex=nexttrack+MTPR-1; tempindex>=nexttrack; tempindex--)
	{
		int i= tempindex%MTPR;
		if(!time_of_track[i]){
			continue;
		}
		if(IS_ICIMMORTAL(ch)){
			ch->printlnf("%2d> age=%5d, %s tracks of %s to the %s.", 
				i,
				time_of_track[i],
				tracktype_name( (tracktype)(direction_type[i]>>4) ),
				track_table->get_pers(trackindex[i],ch),
				dir_name[(direction_type[i]&0x0F)]);
			seen++;
		}else{
			int timediff;
			if(tracktime<time_of_track[i]){
				// approx hack for now to support looping
				timediff= tracktime+ 65535 - 1500 -time_of_track[i];
			}else{
				timediff=tracktime-time_of_track[i];
			}		
			
			if(timediff< (6*number_range(1,sk))) // allow a range of 6->600
			{
				tracktype type=get_tracktype(i);
				char *race=lowercase(track_table->get_race(trackindex[i]));
				if(!race){
					bugf( __FILE__":C_track_data::show_tracks() - get_race() return NULL\n");
					continue;
				}
				char *racea_an;
				if(*race=='a' || *race=='e' || *race=='i' || *race=='o' || *race=='u'){
					racea_an="an";
				}else{
					racea_an="a";
				}

				int dir=get_direction(i);
				switch(type){
					default: 
						ch->printlnf("unknown track type %d for index %d!!! - please report the bug to code", 
								 (int)type, i);
						break;
					case(TRACKTYPE_NONE			):	break;
					
					case(TRACKTYPE_MOVE			):	
					case(TRACKTYPE_SNEAK		):	
					case(TRACKTYPE_WALK			):
						if(ch->track_index==trackindex[i]){
							ch->printlnf("`SSome %s tracks of %s %s (possibly your own) lead %s%s.`x", 
								tracktype_age(timediff),
								racea_an,
								race,
								(dir==DIR_UP || dir==DIR_DOWN)?"":"to the ",
								dir_name[dir]);
						}else{
							ch->printlnf("Some %s tracks of %s %s lead %s%s.", 
								tracktype_age(timediff),
								racea_an,
								race,
								(dir==DIR_UP || dir==DIR_DOWN)?"":"to the ",
								dir_name[dir]);
						}
						seen++;
						break;

					case(TRACKTYPE_FLY			):	break;
					case(TRACKTYPE_BLOODTRAIL	):	break;
					case(TRACKTYPE_BLOODPOOL	):	break;
					case(TRACKTYPE_WIZIIMM		):	break;
					case(TRACKTYPE_PASSWOTRACE	):	break;
				}
			}
		}	
	}
	if(seen==0){
		check_improve(ch,main_sn, false, 10);
		ch->println("You failed to see any tracks here.");
	}else{
		check_improve(ch,main_sn, true, 10);
	}

}
/**************************************************************************/
void do_tracks( char_data *ch, char *)
{
	ch->in_room->tracks->show_tracks(ch);
}
/**************************************************************************/
void do_autotrack(char_data *ch, char *)
{
    if (HAS_CONFIG(ch, CONFIG_AUTOTRACK))
    {
		ch->println("Autotrack disabled.");
		REMOVE_CONFIG(ch, CONFIG_AUTOTRACK);
    }
    else
    {
		ch->wraplnf("Autotrack enabled, you will automatically "
			"look for tracks when you move and are not following others (assuming "
			"you have any tracking skill greater than 1%% and you arent speedwalking).");
		SET_CONFIG(ch, CONFIG_AUTOTRACK);
    }
}
/**************************************************************************/
struct hash_link
{
	int key;
	struct hash_link *next;
	void *data;
};

struct hash_header
{
	int rec_size;
	int table_size;
	int *keylist, klistsize, klistlen;

	struct hash_link **buckets;
};

#define	HASH_KEY(ht,key)((((unsigned int)(key))*17)%(ht)->table_size)

struct hunting_data
{
	char *name;
	char_data **victim;
};

struct room_q
{
	int room_nr;
	struct room_q *next_q;
};

struct nodes
{
	int visited;
	int ancestor;
};

#define	IS_DIR          (get_room_index(q_head->room_nr)->exit[i])
#define	GO_OK           (!IS_SET( IS_DIR->exit_info, EX_CLOSED ))
#define	GO_OK_SMARTER   1

void init_hash_table
args ((struct hash_header * ht, int rec_size, int table_size));
void init_world args ((ROOM_INDEX_DATA * room_db[]));
char_data *get_char_area_restrict args ((char_data * ch, char *argument));
void destroy_hash_table args ((struct hash_header * ht));
void _hash_enter args ((struct hash_header * ht, int key, void *data));
ROOM_INDEX_DATA *room_find args ((ROOM_INDEX_DATA * room_db[], int key));
void *hash_find args ((struct hash_header * ht, int key));
int room_enter args ((ROOM_INDEX_DATA * rb[], int key, ROOM_INDEX_DATA * rm));
int hash_enter args ((struct hash_header * ht, int key, void *data));
ROOM_INDEX_DATA *room_find_or_create args ((ROOM_INDEX_DATA * rb[], int key));
void *hash_find_or_create args ((struct hash_header * ht, int key));
int room_remove args ((ROOM_INDEX_DATA * rb[], int key));
void *hash_remove args ((struct hash_header * ht, int key));
int exit_ok args ((EXIT_DATA * pexit));
int find_path
args (
	  (int in_room_vnum, int out_room_vnum, char_data * ch, int depth,
	   int in_zone));

void init_hash_table (struct hash_header *ht, int rec_size, int table_size)
{
	ht->rec_size = rec_size;
	ht->table_size = table_size;

	ht->buckets =
		(struct hash_link **) calloc (sizeof (*ht->buckets), table_size);

	ht->keylist =
		(int *) calloc (sizeof (*ht->keylist), (ht->klistsize = 128));
	ht->klistlen = 0;
}

void init_world (ROOM_INDEX_DATA * room_db[])
{
	memset (room_db, 0, sizeof (ROOM_INDEX_DATA *) * top_room);
}

char_data *get_char_area (char_data * ch, char *argument)
{
	char arg[MIL];
	char_data *ach;
	int number;
	int count;

	if (IS_NULLSTR (argument))
		return NULL;

	if ((ach = get_char_room (ch, argument)) != NULL)
		return ach;

	number = number_argument (argument, arg);
	count = 0;
	for (ach = char_list; ach != NULL; ach = ach->next)
	{
		if (ach->in_room == NULL
			|| ach->in_room->area != ch->in_room->area
			|| !can_see (ch, ach) || !is_name (arg, ach->name))
			continue;
		if (++count == number)
			return ach;
	}

	return NULL;
}

void destroy_hash_table (struct hash_header *ht)
{
	int i;
	struct hash_link *scan, *temp;

	for (i = 0; i < ht->table_size; i++)
		for (scan = ht->buckets[i]; scan;)
		{
			temp = scan->next;
			free (scan);
			scan = temp;
		}
	free (ht->buckets);
	free (ht->keylist);
}

void _hash_enter (struct hash_header *ht, int key, void *data)
{

	struct hash_link *temp;
	int i;

	temp = (struct hash_link *) calloc (sizeof (*temp), 1);

	temp->key = key;
	temp->next = ht->buckets[HASH_KEY (ht, key)];
	temp->data = data;
	ht->buckets[HASH_KEY (ht, key)] = temp;
	if (ht->klistlen >= ht->klistsize)
	{
		ht->keylist =
			(int *) realloc (ht->keylist,
							 sizeof (*ht->keylist) * (ht->klistsize *= 2));
	}
	for (i = ht->klistlen; i >= 0; i--)
	{
		if (ht->keylist[i - 1] < key)
		{
			ht->keylist[i] = key;
			break;
		}
		ht->keylist[i] = ht->keylist[i - 1];
	}
	ht->klistlen++;
}

ROOM_INDEX_DATA *room_find (ROOM_INDEX_DATA * room_db[], int key)
{
	return ((key < top_room && key > -1) ? room_db[key] : 0);
}

void *hash_find (struct hash_header *ht, int key)
{
	struct hash_link *scan;

	scan = ht->buckets[HASH_KEY (ht, key)];

	while (scan && scan->key != key)
		scan = scan->next;

	return scan ? scan->data : NULL;
}

int room_enter (ROOM_INDEX_DATA * rb[], int key, ROOM_INDEX_DATA * rm)
{
	ROOM_INDEX_DATA *temp;

	temp = room_find (rb, key);
	if (temp)
		return (0);

	rb[key] = rm;
	return (1);
}

int hash_enter (struct hash_header *ht, int key, void *data)
{
	void *temp;

	temp = hash_find (ht, key);
	if (temp)
		return 0;

	_hash_enter (ht, key, data);
	return 1;
}

ROOM_INDEX_DATA *room_find_or_create (ROOM_INDEX_DATA * rb[], int key)
{
	ROOM_INDEX_DATA *rv;

	rv = room_find (rb, key);
	if (rv)
		return rv;

	rv = (ROOM_INDEX_DATA *) calloc (sizeof (*rv), 1);
	rb[key] = rv;

	return rv;
}

void *hash_find_or_create (struct hash_header *ht, int key)
{
	void *rval;

	rval = hash_find (ht, key);
	if (rval)
		return rval;

	rval = (void *) malloc (ht->rec_size);
	_hash_enter (ht, key, rval);

	return rval;
}

int room_remove (ROOM_INDEX_DATA * rb[], int key)
{
	ROOM_INDEX_DATA *tmp;

	tmp = room_find (rb, key);
	if (tmp)
	{
		rb[key] = 0;
		free (tmp);
	}
	return (0);
}

void *hash_remove (struct hash_header *ht, int key)
{
	struct hash_link **scan;

	scan = ht->buckets + HASH_KEY (ht, key);

	while (*scan && (*scan)->key != key)
		scan = &(*scan)->next;

	if (*scan)
	{
		int i;
		struct hash_link *temp, *aux;

		temp = (struct hash_link *) (*scan)->data;
		aux = *scan;
		*scan = aux->next;
		free (aux);

		for (i = 0; i < ht->klistlen; i++)
			if (ht->keylist[i] == key)
				break;

		if (i < ht->klistlen)
		{
			memcpy ((char *) ht->keylist + i + 1,
					(char *) ht->keylist + i,
					(ht->klistlen - i) * sizeof (*ht->keylist));
			ht->klistlen--;
		}

		return temp;
	}

	return NULL;
}

int exit_ok (EXIT_DATA * pexit)
{
	ROOM_INDEX_DATA *to_room;

	if ((pexit == NULL) || (to_room = pexit->u1.to_room) == NULL)
		return 0;

	return 1;
}

int find_path (int in_room_vnum, int out_room_vnum, char_data * ch, int depth,
			   int in_zone)
{
	struct room_q *tmp_q, *q_head, *q_tail;
	struct hash_header x_room;
	int i, tmp_room, count = 0, thru_doors;
	ROOM_INDEX_DATA *herep;
	ROOM_INDEX_DATA *startp;
	EXIT_DATA *exitp;

	if (depth < 0)
	{
		thru_doors = true;
		depth = -depth;
	}
	else
	{
		thru_doors = false;
	}

	startp = get_room_index (in_room_vnum);

	init_hash_table (&x_room, sizeof (int), 2048);

	hash_enter (&x_room, in_room_vnum, (void *) -1);

	q_head = (struct room_q *) calloc (sizeof (*q_head), 1);

	q_tail = q_head;
	q_tail->room_nr = in_room_vnum;
	q_tail->next_q = 0;

	while (q_head)
	{
		herep = get_room_index (q_head->room_nr);

		if (herep->area == startp->area || !in_zone)
		{
			for (i = 0; i < MAX_DIR; i++)
			{
				exitp = herep->exit[i];
				if (exit_ok (exitp) && (thru_doors ? GO_OK_SMARTER : GO_OK))
				{

					tmp_room = herep->exit[i]->u1.to_room->vnum;
					if (tmp_room != out_room_vnum)
					{
						if (!hash_find (&x_room, tmp_room) && (count < depth))
						{
							count++;

							tmp_q =
								(struct room_q *) calloc (sizeof (*tmp_q), 1);
							tmp_q->room_nr = tmp_room;
							tmp_q->next_q = 0;
							q_tail->next_q = tmp_q;
							q_tail = tmp_q;

							hash_enter (&x_room,
										tmp_room,
										((int)
										 hash_find
										 (&x_room,
										  q_head->room_nr) ==
										 -1) ? (void
												*) (i
													+
													1)
										:
										hash_find (&x_room, q_head->room_nr));
						}
					}
					else
					{

						tmp_room = q_head->room_nr;
						for (; q_head; q_head = tmp_q)
						{
							tmp_q = q_head->next_q;
							free (q_head);
						}

						if ((int) hash_find (&x_room, tmp_room) == -1)
						{
							if (x_room.buckets)
							{
								destroy_hash_table (&x_room);
							}
							return (i);
						}
						else
						{
							i = (int) hash_find (&x_room, tmp_room);
							if (x_room.buckets)
							{
								destroy_hash_table (&x_room);
							}
							return (-1 + i);
						}
					}
				}
			}
		}

		tmp_q = q_head->next_q;
		free (q_head);
		q_head = tmp_q;
	}

	if (x_room.buckets)
	{

		destroy_hash_table (&x_room);
	}
	return -1;
}

void do_hunt (char_data * ch, char *argument)
{
	char buf[MSL];
	char arg[MSL];
	char_data *victim;
	int direction;
	bool fArea;

	one_argument (argument, arg);

    if ( !IS_NPC(ch)
		&&   (ch->level < skill_table[gsn_hunt].skill_level[ch->clss] 
		|| get_skill(ch,gsn_hunt)==0) )
    {
		ch->println("You do not know how to hunt.");
		return;
    }

	if (arg[0] == '\0')
	{
		ch->println ("Whom are you trying to hunt?");
		return;
	}

	fArea = (get_trust (ch) < MAX_LEVEL);

	if (IS_NPC (ch))
		victim = get_char_world (ch, arg);
	else if (fArea)
		victim = get_char_area (ch, arg);
	else
		victim = get_char_world (ch, arg);

	if (victim == NULL)
	{
		ch->println ("No-one around by that name.");
		return;
	}

	if (ch->in_room == victim->in_room)
	{
		act ("$N is here!", ch, NULL, victim, TO_CHAR);
		return;
	}

	if (ch->move > 2)
		ch->move -= 3;
	else
	{
		ch->println ("You're too exhausted to hunt anyone!");
		return;
	}

	act ("$n carefully sniffs the air.", ch, NULL, NULL, TO_ROOM);
	WAIT_STATE (ch, skill_table[gsn_hunt].beats);
	direction =
		find_path (ch->in_room->vnum, victim->in_room->vnum, ch, -40000,
				   fArea);

	if (direction == -1)
	{
		act ("You couldn't find a path to $N from here.",
			 ch, NULL, victim, TO_CHAR);
		return;
	}

	if (direction < 0 || direction > 5)
	{
		ch->println ("Hmm... Something seems to be wrong.");
		return;
	}

	if ((IS_NPC (ch) && number_percent () > 50) ||
		(!IS_NPC (ch) && number_percent () > get_skill (ch, gsn_hunt)))
	{
		do
		{
			direction = number_door ();
		}
		while ((ch->in_room->exit[direction] == NULL) ||
			   (ch->in_room->exit[direction]->u1.to_room == NULL));
	}

	sprintf (buf, "$N is %s from here.", dir_name[direction]);
	act (buf, ch, NULL, victim, TO_CHAR);
	check_improve (ch, gsn_hunt, true, 1);
	return;
}

void hunt_victim (char_data * ch)
{
	int dir;
	bool found;
	char_data *tmp;

	if (ch == NULL || ch->mobmemory == NULL || !IS_NPC (ch))
		return;

	if (  IS_SET(ch->act, ACT_DONT_WANDER) 
           || IS_SET(ch->act, ACT_PET) 
           || IS_SET(ch->act, ACT_DOCILE)
  	   || IS_SET(ch->act2, ACT2_NOHUNT))
		return;

	for (found = 0, tmp = char_list; tmp && !found; tmp = tmp->next)
		if (ch->mobmemory == tmp)
			found = 1;

	if (!found || !can_see (ch, ch->mobmemory))
	{
		ch->mobmemory = NULL;
		return;
	}

	if(ch->mobmemory == ch) // Mobs won't attack themselves
	{
		ch->mobmemory = NULL;
		return;
	}

	if (ch->in_room == ch->mobmemory->in_room)
	{

	   if ( !is_affected(ch, AFF_POISON) && !IS_AFFECTED(ch, AFF_PLAGUE)) // Keep mobs from attacking themselves
	   {
		if (ch->race == race_lookup("avian")      ||  ch->race == race_lookup("dragon") ||
		    ch->race == race_lookup("drow")       ||  ch->race == race_lookup("duergar") ||
		    ch->race == race_lookup("dwarf")      ||  ch->race == race_lookup("elf") ||
		    ch->race == race_lookup("faerie")     ||  ch->race == race_lookup("gnome") ||
		    ch->race == race_lookup("half-elf")   ||  ch->race == race_lookup("half-orc") ||
		    ch->race == race_lookup("centaur")    ||  ch->race == race_lookup("demon") ||
		    ch->race == race_lookup("angel")      ||  ch->race == race_lookup("feylar") ||
		    ch->race == race_lookup("thri-kreen") ||  ch->race == race_lookup("human") ||
		    ch->race == race_lookup("lich")       ||  ch->race == race_lookup("human") ||
		    ch->race == race_lookup("dryad")      ||  ch->race == race_lookup("human") ||
		    ch->race == race_lookup("cambion"))
		{
			act ("$n glares at $N and says 'How dare you attack me!'", ch, NULL, ch->mobmemory, TO_NOTVICT);
			act ("$n glares at you and says 'How dare you attack me!'", ch, NULL, ch->mobmemory, TO_VICT);
			act ("You glare at $N and say 'How dare you attack me!'", ch, NULL, ch->mobmemory, TO_CHAR);
		}else {

		if (ch->race == race_lookup("lizardman") ||  ch->race == race_lookup("orc") ||
		    ch->race == race_lookup("troll") ||  ch->race == race_lookup("minotaur") ||
		    ch->race == race_lookup("golem") ||  ch->race == race_lookup("cyclops") ||
		    ch->race == race_lookup("gargoyle"))
		{
			act ("$n glares at $N and says 'Me kill you now!'", ch, NULL, ch->mobmemory, TO_NOTVICT);
			act ("$n glares at you and says 'Me kill you now!'", ch, NULL, ch->mobmemory, TO_VICT);
			act ("You glare at $N and say 'Me kill you now!'", ch, NULL, ch->mobmemory, TO_CHAR);
		}else{

		if (number_percent () < 60)
		{
			act ("$n glares at $N in anger!", ch, NULL, ch->mobmemory, TO_NOTVICT);
			act ("$n glares at you in anger!", ch, NULL, ch->mobmemory, TO_VICT);
			act ("You glare at $N in anger!", ch, NULL, ch->mobmemory, TO_CHAR);
		}
		else
		{
			act (" looks at $N suspiciously.", ch, NULL, ch->mobmemory, TO_NOTVICT);
			act ("$n looks at you suspiciously.", ch, NULL, ch->mobmemory, TO_VICT);
			act ("You look at $N suspiciously.", ch, NULL, ch->mobmemory, TO_CHAR);
		} } } }
		multi_hit (ch, ch->mobmemory, TYPE_UNDEFINED);
		ch->mobmemory = NULL;
		return;
	}

	WAIT_STATE (ch, skill_table[gsn_hunt].beats);
	dir =
		find_path (ch->in_room->vnum, ch->mobmemory->in_room->vnum, ch, -40000,
				   true);

	if (dir < 0 || dir > 10)
	{
		act ("$n says 'Damn! Lost $M!'", ch, NULL, ch->mobmemory, TO_ROOM);
		ch->mobmemory = NULL;
		return;
	}

	if (number_percent () > 50)
	{

		do
		{
			dir = number_door ();
		}
		while ((ch->in_room->exit[dir] == NULL) ||
			   (ch->in_room->exit[dir]->u1.to_room == NULL));
	}

	if (IS_SET (ch->in_room->exit[dir]->exit_info, EX_CLOSED))
	{
		do_open (ch, (char *) dir_name[dir]);
		return;
	}

	move_char (ch, dir, false, NULL);
	return;
}


