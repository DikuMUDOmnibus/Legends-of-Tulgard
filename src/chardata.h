/**************************************************************************/
// chardata.h - header for char_data class
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

#ifndef char_data_H
#define char_data_H

struct pload_data;

class intro_data;

struct altform_data
{
	int				subtype; //Subtype where applicable
	int				alternate_flags; //Flags for each alternate form
	char			*description; //Long description
	char			*short_descr; //Short description
	long			aff;
	long			aff2;
    long			immune;
    long			resist;
    long			vuln;
    long			form; //Form
	long			parts; //Parts 
    sh_int			size; //The size of the form
    char *			material; //If the form is made out of something odd
	int				type; //Type of alternate
	sh_int			modifier[MAX_STATS]; //Modifications on the base race
	int				timer; //For those forms with timers between changes
};

class char_data
{
public:
// member functions

    // output to character
    void print(const char *buf);
	void printbw(const char *buf);
    void printf(const char *fmt, ...)					__mftc_printf_1__;
    void printfbw(const char *fmt, ...)					__mftc_printf_1__;
    void println(const char *buf);
	void print_blank_lines(int blank_lines_to_print_to_char);
	void printlnbw(const char *buf);
    void printlnf(const char *fmt, ...)					__mftc_printf_1__;
    void printlnfbw(const char *fmt, ...)				__mftc_printf_1__;
    void wrap(const char *buf);
    void wrapf(const char *fmt, ...)					__mftc_printf_1__;
    void wrapln(const char *buf);
    void wraplnf(const char *fmt, ...)					__mftc_printf_1__;

    void titlebar(const char *header);
	void titlebarf(const char *fmt, ...)				__mftc_printf_1__;
    void olctitlebar(const char *header);
	void olctitlebarf(const char *fmt, ...)				__mftc_printf_1__;

    void print(int seconds, const char *buf);
    void printf(int seconds, const char *fmt, ...)		__mftc_printf_2__;
    void println(int seconds, const char *buf);
    void printlnf(int seconds, const char *fmt, ...)	__mftc_printf_2__;

    void sendpage(const char *txt);

	int pdelay(); // the amount print*() text sent to the character is delayed by
	void set_pdelay(int seconds); // the amount print*() text sent to the character is delayed by
private:
	int m_pdelay; // the amount print*() text sent to the character is delayed by
public:
    // skills
    int get_skill(int sn); // returns the percentage of skill they have in it
    int get_display_skill(int sn); // return the percentage they have out of the max for the class
    int get_skill_level(int sn); // returns the level of the skill for the players class

	void mpqueue_attached(time_t when);
//	void mpqueue_removed(int when);
	void mpqueue_dequeue_all();  // dequeue all the queued events involving us

	void hit_return_to_continue();

	vn_int vnum(); // a safe way to get the vnum from a mob/player
	vn_int in_room_vnum(); // a safe way to get the vnum of a room
private:
    int npc_skill_level(int sn);
    int pc_skill_level(int sn);

public:
	int uid; // unique id
	DEITY_DATA* deity;
	TERRITORY_DATA* territory;
    char_data * next;
    char_data * next_in_room;
    char_data * next_player;
	char_data * next_who_list;
    char_data * mounted_on;
    char_data * ridden_by;
    char_data * master;
    char_data * leader;
    char_data * fighting;
    char_data * reply;
    char_data * anon_reply;
    char_data * retell;
    char_data * anon_retell;
    char_data * controlling; // who a pc is controlling
    char_data * pet;
    char_data * mprog_target;
    vn_int      mprog_remember_room_vnum; // used by premove at this stage
    MEM_DATA    *memory;
    SPEC_FUN    *spec_fun;
    GAMBLE_FUN  *gamble_fun;
    mob_index_data  *pIndexData;
    connection_data *desc;
    AFFECT_DATA *affected;
    note_data   *pnote;
    OBJ_DATA    *carrying;
    OBJ_DATA    *on;
    ROOM_INDEX_DATA *   in_room;
    ROOM_INDEX_DATA *   was_in_room;
    area_data   *zone;
    pc_data     *pcdata;
    GEN_DATA    *gen_data;
    bool        valid;
    char        *name;
    long        id;
    char *		remote_ip_copy;  // used to detect multilogging
	sh_int		host_validated;
    sh_int      version;
    sh_int      subversion;
    char *      short_descr;
    char *      long_descr;
    char *      description;
	char *      gprompt;
    char *      prompt;
    char *      olcprompt;
    char *      prefix;
    duel_data * duels;
    sh_int      group;
	sh_int		helpgroup;  // the group number they will help
    CClanType	*clan;
    sh_int      clanrank;
	CClanType	*seeks;
    sh_int      pksafe;        /* 3 pk variables enable automatic pk enforcement */
    sh_int      pknorecall;
    sh_int      pkool;
    int         pkkills;
    int         pkdefeats;
    int         pknoquit;    
    sh_int      sex;
    sh_int      clss;
    sh_int      race;
    sh_int      level;
    sh_int      trust;
    int         played;
    int         lines;  // lines per page
    time_t      logon;
    int         timer;
    int         idle;
    int         wait;
    int         daze;
    int         hit;
    int         max_hit;
    int         mana;
    int         max_mana;
    int         move;
    int         max_move;
    long        gold;
    long        silver;
    int         exp;
    long        act;
    long        act2;
    long        dyn; // dynamic data - never saved
	long        dyn2; // dynamic data - never saved
    long        comm;   
    long        wiznet[4]; // wiznet stuff 
	char *		wiznet_colour[4]; // wiznet colour coding
    long        imm_flags;
    long        res_flags;
    long        vuln_flags;
    unsigned char invis_level;
    unsigned char iwizi;
    unsigned char olcwizi;
    unsigned char owizi;
    unsigned char incog_level;
    long        affected_by;
    long        affected_by2;
	long        affected_by3;
    long        vampire; 		// Ixliam
    long        title; 			// Ixliam
    char * 		 pet1;			// Ixliam
    char * 		 pet2;
    char * 		 pet3;
    char * 		 pet4;
    char * 		 fealty;
    long        territory1;	// Ixliam
    long        wish;		// Ixliam
    long        style;		// Ixliam
    long        config;
    long        config2;
    sh_int      position;
    int			practice;
    int			train;
    int         carry_weight;
    int         carry_number;
    int         saving_throw;
    sh_int      alliance;
    sh_int      tendency;
    int         hitroll;
    int         damroll;
    int         armor[4];
    int         wimpy;
    // stats 
    sh_int      perm_stats[MAX_STATS];
    sh_int      potential_stats[MAX_STATS];
    sh_int      modifiers[MAX_STATS];
    // altform stuff
	altform_data altform;
	bool		  altformed;
	int			 catseye_timer; //Timer for weretiger's catseye
    // parts stuff 
    long        form;
    long        parts;
    sh_int      size;
    char *      material;
    // mobile stuff 
    long        off_flags;
    sh_int      damage[3];
    sh_int      dam_type;
    sh_int      start_pos;
    sh_int      default_pos;
    sh_int      mprog_delay;
    int         temple;
    long	terr_restrict;

	// Data which only egg pets have.
	char *		petcolor1;
	char *		petcolor2;
	char *		petcolor3;
	char *		petcolor4;
	time_t		petbirth;
	int		norm_trick;
	int		magic_trick;
	long		pet_norm_flags;
	long		pet_magic_flags;

    time_t	pregnant;		// Ixliam
    char *     	orig_short_descr;	// Ixliam
    char *   	orig_description;	// Ixliam
    char *     	poly_short;		// Ixliam
    char *   	poly_description;	// Ixliam
    char *     	posing;		// Ixliam
    char *     	history;	// Ixliam
    char *     	pretitle;	// Ixliam
    int		murder;		// Ixliam
    int		theft;		// Ixliam
    time_t	jail;		// Ixliam
    int		crime;		// Ixliam
    sh_int	orig_race;	// Ixliam

    int		bloodpool;	// Ixliam - Vampires
    int		vampgen;	// Ixliam - Vampires
    int		gnosis;		// Reath  - Werewolves
    int		werecircle;	// Reath  - Werewolves
    int		quazer;		// Ixliam
    int		akills;		// Ixliam
    int     adeaths;	// Ixliam
    int		questobj;	// Ixliam
    int		questmob;	// Ixliam
    int		questtimer;	// Ixliam
    char_data  *questgiver;	// Ixliam
    int		nextquest;	// Ixliam
    sh_int	dreamp;		// Ixliam
    sh_int	dreamn;		// Ixliam
	int		death_energy; //Reath

    // Bard Song Rewrites - format is song number, line number
    char *	s1_0;
    char *	s1_1;
    char *	s1_2;
    char *	s1_3;
    char *	s1_4;
    char *	s1_5;
    char *	s1_6;
    char *	s1_7;

    char *	s2_0;
    char *	s2_1;
    char *	s2_2;
    char *	s2_3;
    char *	s2_4;
    char *	s2_5;
    char *	s2_6;
    char *	s2_7;

    char *	s3_0;
    char *	s3_1;
    char *	s3_2;
    char *	s3_3;
    char *	s3_4;
    char *	s3_5;
    char *	s3_6;
    char *	s3_7;
    char *	s3_8;
    char *	s3_9;
    char *	s3_10;
    char *	s3_11;

    char *	s4_0;
    char *	s4_1;
    char *	s4_2;
    char *	s4_3;

    char *	s5_0;
    char *	s5_1;
    char *	s5_2;
    char *	s5_3;
    char *	s5_4;

    char *	s6_0;
    char *	s6_1;
    char *	s6_2;
    char *	s6_3;
    char *	s6_4;
    char *	s6_5;

    char *	s7_0;
    char *	s7_1;
    char *	s7_2;
    char *	s7_3;
    char *	s7_4;
    char *	s7_5;

    char *	s8_0;
    char *	s8_1;
    char *	s8_2;
    char *	s8_3;
    char *	s8_4;
    char *	s8_5;
    char *	s8_6;
    char *	s8_7;
    char *	s8_8;
    char *	s8_9;
    char *	s8_10;
    char *	s8_11;
    char *	s8_12;
    char *	s8_13;
    char *	s8_14;
    char *	s8_15;
    char *	s8_16;
    char *	s8_17;
    char *	s8_18;
    char *	s8_19;
    char *	s8_20;
    char *	s8_21;

    char *	s9_0;
    char *	s9_1;
    char *	s9_2;
    char *	s9_3;
    char *	s9_4;
    char *	s9_5;
    char *	s9_6;
    char *	s9_7;
    char *	s9_8;
    char *	s9_9;
    char *	s9_10;
    char *	s9_11;
    char *	s9_12;
    char *	s9_13;
    char *	s9_14;
    char *	s9_15;
    char *	s9_16;
    char *	s9_17;
    char *	s9_18;
    char *	s9_19;

    char *	s10_0;
    char *	s10_1;
    char *	s10_2;
    char *	s10_3;

    char *	s11_0;
    char *	s11_1;
    char *	s11_2;
    char *	s11_3;
    char *	s11_4;
    char *	s11_5;
    char *	s11_6;
    char *	s11_7;
    char *	s11_8;
    char *	s11_9;
    char *	s11_10;
    char *	s11_11;

    char *	s12_0;
    char *	s12_1;
    char *	s12_2;
    char *	s12_3;

    sh_int	songp;		// Ixliam
    sh_int	songn;		// Ixliam


    // critical stuff 
    int         bleeding;
    sh_int      will_die;
    sh_int      is_stunned;
    // language stuff 
    language_data *language;

    long bank;
    ROOM_INDEX_DATA *  last_ic_room;  // NULL = currently in ic rooms 
    
    // subdue system - kalahn - june 97
    bool no_xp, subdued;
    bool tethered;
    bool bucking;
    
    bool autologout; // set in char_update
	bool is_trying_sleep;
    
    int subdued_timer; // budget way to wake them 
    sh_int will;
    sh_int wildness;
    
    sh_int  state; /* determines if they are
                   * comatose (will die),
                   * unconscience,
                   * semiconscience,
                   * subdued,
                   * yielding,
                   * free 
				   */
    char_data *    subduer; // character who has subdue this character 
    int cautious_about_backstab; // counter which is how long before a mob
    // can be backstabbed for
    
    unsigned char last_colour, saved_colour[MAX_SAVED_COLOUR_ARRAY];
    unsigned char saved_colour_index;

    long    last_force;     // the tick this char was last forced to do
    // something - used to stop wandering within
    // 5 ticks of them being forced
    // ( = tick_counter on force)

    long        notenet; // notenet stuff - kalahn

    // Dynamic Spell info
    int         mirage;
    sh_int      mirage_hours;

    // MOB REMEMBER STUFF
    char_data  * mobmemory;

	//Damage memory
	int last_damage[2]; //Stores the damage number of the last attack
	                    //0 is physical, 1 is magical, +1000 to the most recent.

    // WEAPON SPECIALIZATION
    sh_int specialization;

	// STATIC VNUMS.
    vn_int      recall_room;				// Recall room for player.
	vn_int		recall_inn_room;			// Recall room for player's inn.
	
	// CRAFT INFORMATION
	int		step_number;
	int		craft_type;
	obj_data *crafting;
	int		craft_choices[10];

	// TIMERS.
	int			expire_recall_inn;			// Time the recall inn expires.

    char        saycolour;
    char        motecolour;

    sh_int      track_index;

    int     duel_challenged;
    int     duel_decline;
    int     duel_accept;
    int     duel_ignore;
    int     duel_bypass;
    int     duel_subdues_before_karn_loss;

	sh_int	remort;	// remort the player is currently on
	sh_int	beginning_remort; // if a number, the player is midstream of remorting

	sh_int	highest_level_to_do_damage;
	
	intro_data *know; // data containing who they know info
	unsigned short know_index;	// their index in the know database
	char	colour_prefix;	// character prefixing all colour codes

	pload_data *pload;	// data about the pload
	char_data *ploaded; // character which has been ploaded by this character
private:
	int mpqueue_count; // just a basic command for now
	bool mxp_enabled;

public: // mxp functions
	void mxp_send_init(); // used to send the mxp element tags we are going to use
	void record_replayroom_event(const char *txt); // record says and emotes etc
};

#endif

