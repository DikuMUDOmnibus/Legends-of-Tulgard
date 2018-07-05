/**************************************************************************/
// plants.cpp - Plant Growth System
/***************************************************************************
 * Whispers of Times Lost (c)2001-2003 Brad Wilson                         *
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
#include "plants.h"

/***************************************************************************/
extern int environment_interval;

struct plant_class   *plant_class_list   = NULL;
struct plant_species *plant_species_list = NULL;
struct plant_class    *plant_class_free    = NULL;
struct plant_instance *plant_instance_free = NULL;
struct plant_species  *plant_species_free  = NULL;

/***************************************************************************/
static char *plant_type_table [PLANT_TYPE_MAX] =
{
  "tree",
  "shrub",
  "bush",
  "stem",  /* "flowers" */
  "cactus"
};

/***************************************************************************/
struct plant_class *new_plant_class( void )
{
  struct plant_class *fc;

  if ( plant_class_free == NULL )
  {
    fc = alloc_perm( sizeof(*fc) );
  }
  else
  {
    fc = plant_class_free;
    plant_class_free = fc->next;
  }

  fc->name = NULL;
  fc->next = NULL;
  fc->type = PLANT_TYPE_TREE;
  fc->health_flower = 500;
  fc->health_max = 1000;
  fc->water_desired = 300;

  return fc;
}
/***************************************************************************/
void free_plant_class( struct plant_class *fc )
{
  if ( fc == NULL )
    return;

/*
 * This should be freed, but with short, common words like
 * oak, cactus, etc., it might be a bad idea.  The only time
 * this will leak memory is when a sector type is created
 * then freed during the same boot.  Otherwise it would be
 * in reserved string space and the free would just return.
  free_string( fc->name, MEM_GENERAL );
 */

  fc->next = plant_class_free;
  plant_class_free = fc;

  return;
}

struct plant_species *new_plant_species( void )
{
  struct plant_species *fs;

  if ( plant_species_free == NULL )
  {
    fs = alloc_perm( sizeof(*fs) );
  }
  else
  {
    fs = plant_species_free;
    plant_species_free = fs->next;
  }

  fs->name = NULL;
  fs->next = NULL;
  fs->class = NULL;
  fs->flower_effort = 6;       /* 6 hour respawn on fruit */
  fs->flower_vnum = 0;
  fs->flower_max = 1;
  fs->fruit_vnum = 0;
  fs->fruit_max = 1;
  fs->growth_seed = -168;      /* 1 week to germinate */
  fs->growth_flowering = 720;  /* 1 month to produce fruit */
  fs->growth_max = 16800;      /* 2 years default lifespan */

  return fs;
}

/***************************************************************************/
void free_plant_species( struct plant_species *fs )
{
  if ( fs == NULL )
    return;

/*
 * This should be freed, but with short, common words like
 * oak, cactus, etc., it might be a bad idea.  The only time
 * this will leak memory is when a sector type is created
 * then freed during the same boot.  Otherwise it would be
 * in reserved string space and the free would just return.
  free_string( fs->name, MEM_GENERAL );
 */

  fs->next = plant_species_free;
  plant_species_free = fs;

  return;
}

struct plant_instance *new_plant_instance( void )
{
  struct plant_instance *fi;

  if ( plant_instance_free == NULL )
  {
    fi = alloc_perm( sizeof(*fi) );
  }
  else
  {
    fi = plant_instance_free;
    plant_instance_free = fi->next;
  }

  fi->next = NULL;
  fi->species = NULL;
  fi->health = 300;
  fi->water = 300;
  fi->fruit = 0;
  fi->flower = 0;
  fi->growth = 720;
  fi->eiv_fruit = environment_interval;

  return fi;
}

void free_plant_instance( struct plant_instance *fi )
{
  if ( fi == NULL )
    return;

  fi->next = plant_instance_free;
  plant_instance_free = fi;

  return;
}


void destroy_plant( struct plant_instance *fi, ROOM_INDEX_DATA *room )
{
  struct plant_instance *f;

  if ( fi == NULL || room == NULL )
    return;

  if ( room->plant == fi )
  {
    room->plant = fi->next;
    free_plant_instance( fi );
    return;
  }

  for ( f = room->plant; f != NULL; f = f->next )
  {
    if ( f->next == fi )
    {
      f->next = fi->next;
      free_plant_instance( fi );
      return;
    }
  }

  /* plant instance not found if control reaches here */

  return;
}


struct plant_class *plant_class_lookup( char *argument )
{
  struct plant_class *fc;

  for ( fc = plant_class_list; fc != NULL; fc = fc->next )
    if ( !str_cmp( argument, fc->name ) )
      return fc;

  return NULL;
}


struct plant_species *plant_species_lookup( char *argument )
{
  struct plant_species *fs;

  for ( fs = plant_species_list; fs != NULL; fs = fs->next )
    if ( !str_cmp( argument, fs->name ) )
      return fs;

  return NULL;
}


int plant_type_lookup( char *argument )
{
  int type;

  for ( type = 0; type < PLANT_TYPE_MAX; type++ )
    if ( !str_cmp( argument, plant_type_table[type] ) )
      return type;

  return PLANT_TYPE_TREE;
}


void write_plant_class( FILE *fp, struct plant_class *fc )
{
  fprintf( fp, "Name '%s'\n", fc->name );
  fprintf( fp, "Type '%s'\n", plant_type_table[fc->type] );
  fprintf( fp, "HealthFlower %d\n", fc->health_flower );
  fprintf( fp, "HealthMax %d\n", fc->health_max );
  fprintf( fp, "WaterDesired %d\n", fc->water_desired );
  fprintf( fp, "EndFloraClass\n" );

  return;
}

void write_plant_species( FILE *fp, struct plant_species *fs )
{
  fprintf( fp, "Name '%s'\n", fs->name );
  fprintf( fp, "Class '%s'\n", fs->class->name );
  fprintf( fp, "FlowerEffort %d\n", fs->flower_effort );
  fprintf( fp, "FlowerVnum %d\n", fs->flower_vnum );
  fprintf( fp, "FlowerMax %d\n", fs->flower_max );
  fprintf( fp, "FruitVnum %d\n", fs->fruit_vnum );
  fprintf( fp, "FruitMax %d\n", fs->fruit_max );
  fprintf( fp, "GrowthSeed %d\n", fs->growth_seed );
  fprintf( fp, "GrowthFlowering %d\n", fs->growth_flowering );
  fprintf( fp, "GrowthMax %d\n", fs->growth_max );
  fprintf( fp, "EndFloraSpecies\n" );

  return;
}

void write_plant_instance( FILE *fp, struct plant_instance *fi )
{
  fprintf( fp, "Species '%s'\n", fi->species->name );
  fprintf( fp, "Quantity %d\n", fi->quantity );
  fprintf( fp, "Health %d\n", fi->health );
  fprintf( fp, "PlantWater %d\n", fi->water );
  fprintf( fp, "Fruit %d\n", fi->fruit );
  fprintf( fp, "Flower %d\n", fi->flower );
  fprintf( fp, "Growth %d\n", fi->growth );
  fprintf( fp, "FruitUpdate %d\n", environment_interval - fi->eiv_fruit );
  fprintf( fp, "EndFloraInstance\n" );

  return;
}

int write_plant_class_file( void )
{
  FILE *fp;
  struct plant_class *f;
  char buf[MSL];

  if ( ( fp = fopen( "plant_class.tmp", "w" ) ) == NULL )
  {
    bug("Error opening plant class file.\n\r", 0 );
    return 1;
  }

  for ( f = plant_class_list; f != NULL; f = f->next )
  {
    write_plant_class( fp, f );
  }

  fprintf( fp, "Endsection\n" );

  fclose( fp );
  sprintf( buf, "cp plant_class.tmp %s", PLANT_CLASS_FILE );
  system( buf );
  system( "rm plant_class.tmp" );

  return 0;
}


int write_plant_species_file( void )
{
  FILE *fp;
  struct plant_species *fs;
  char buf[MSL];

  if ( ( fp = fopen( "plant_species.tmp", "w" ) ) == NULL )
  {
    bug("Error opening plant species file.\n\r", 0 );
    return 1;
  }

  for ( fs = plant_species_list; fs != NULL; fs = fs->next )
  {
    write_plant_species( fp, fs );
  }

  fprintf( fp, "Endsection\n" );

  fclose( fp );
  sprintf( buf, "cp plant_species.tmp %s", PLANT_SPECIES_FILE );
  system( buf );
  system( "rm plant_species.tmp" );

  return 0;
}


void read_plant_classes( void )
{
  char *word;
  struct plant_class *fc = NULL;
  FILE *fp;

  if ( ( fp = fopen( PLANT_CLASS_FILE, "r" ) ) == NULL )
  {
    bug("Error opening plant class file.\n\r", 0 );
    exit( 1 );
  }

  while ( 1 )
  {
    word = fread_word( fp );

    if ( !str_cmp( word, "Name" ) )
    {
      if ( fc != NULL )
      {
        fc->next = plant_class_list;
	plant_class_list = fc;
      }
      fc = new_plant_class( );
      fc->name = str_dup( fread_word( fp ) );
      continue;
    }

    if ( !str_cmp( word, "Type" ) )
    {
      if ( fc != NULL )
        fc->type = plant_type_lookup( fread_word( fp ) );
      else
        fread_number( fp );

      continue;
    }

    if ( !str_cmp( word, "HealthFlower" ) )
    {
      if ( fc != NULL )
        fc->health_flower = fread_number( fp );
      else
        fread_number( fp );

      continue;
    }

    if ( !str_cmp( word, "HealthMax" ) )
    {
      if ( fc != NULL )
        fc->health_max = fread_number( fp );
      else
        fread_number( fp );

      continue;
    }

    if ( !str_cmp( word, "WaterDesired" ) )
    {
      if ( fc != NULL )
        fc->water_desired = fread_number( fp );
      else
        fread_number( fp );

      continue;
    }

    if ( !str_cmp( word, "EndFloraClass" ) )
    {
      fc->next = plant_class_list;
      plant_class_list = fc;
      fc = NULL;
      continue;
    }

    if ( !str_cmp( word, "Endsection" ) )
      break;
  }

  fclose( fp );

  return;
}


void read_plant_species( void )
{
  char *word;
  struct plant_species *fs = NULL;
  FILE *fp;

  if ( ( fp = fopen( PLANT_SPECIES_FILE, "r" ) ) == NULL )
  {
    bug("Error opening plant species file.\n\r", 0 );
    exit( 1 );
  }

  while ( 1 )
  {
    word = fread_word( fp );

    if ( !str_cmp( word, "Name" ) )
    {
      if ( fs != NULL )
      {
        fs->next = plant_species_list;
	plant_species_list = fs;
      }
      fs = new_plant_species( );
      fs->name = str_dup( fread_word( fp ) );
      continue;
    }

    if ( !str_cmp( word, "Class" ) )
    {
      if ( fs != NULL )
        fs->class = plant_class_lookup( fread_word( fp ) );
      else
        fread_number( fp );

      continue;
    }

    if ( !str_cmp( word, "FlowerEffort" ) )
    {
      if ( fs != NULL )
        fs->flower_effort = fread_number( fp );
      else
        fread_number( fp );

      continue;
    }

    if ( !str_cmp( word, "FlowerVnum" ) )
    {
      if ( fs != NULL )
        fs->flower_vnum = fread_number( fp );
      else
        fread_number( fp );

      continue;
    }

    if ( !str_cmp( word, "FlowerMax" ) )
    {
      if ( fs != NULL )
        fs->flower_max = fread_number( fp );
      else
        fread_number( fp );

      continue;
    }

    if ( !str_cmp( word, "FruitVnum" ) )
    {
      if ( fs != NULL )
        fs->fruit_vnum = fread_number( fp );
      else
        fread_number( fp );

      continue;
    }

    if ( !str_cmp( word, "FruitMax" ) )
    {
      if ( fs != NULL )
        fs->fruit_max = fread_number( fp );
      else
        fread_number( fp );

      continue;
    }

    if ( !str_cmp( word, "GrowthSeed" ) )
    {
      if ( fs != NULL )
        fs->growth_seed = fread_number( fp );
      else
        fread_number( fp );

      continue;
    }

    if ( !str_cmp( word, "GrowthFlowering" ) )
    {
      if ( fs != NULL )
        fs->growth_flowering = fread_number( fp );
      else
        fread_number( fp );

      continue;
    }

    if ( !str_cmp( word, "GrowthMax" ) )
    {
      if ( fs != NULL )
        fs->growth_max = fread_number( fp );
      else
        fread_number( fp );

      continue;
    }

    if ( !str_cmp( word, "EndFloraSpecies" ) )
    {
      fs->next = plant_species_list;
      plant_species_list = fs;
      fs = NULL;
      continue;
    }

    if ( !str_cmp( word, "Endsection" ) )
      break;
  }

  fclose( fp );

  return;
}

void load_plant( void )
{
  read_plant_classes( );
  read_plant_species( );

  return;
}

char *get_plant_sdesc( struct plant_instance *fi )
{
  static char sdesc[MSL];
  char buf[MSL];

  sprintf( buf, "%s %s", fi->species->name,
    ( fi->quantity > 1 )
    ? plural( plant_type_table[fi->species->class->type] )
    : plant_type_table[fi->species->class->type] );

  if ( fi->health < 0 )
    sprintf( sdesc, "rotting %s", buf );
  else if ( fi->health < ( fi->species->class->health_flower / 8 ) )
    sprintf( sdesc, "dying %s", buf );
  else if ( fi->health < ( fi->species->class->health_flower / 2 ) )
    sprintf( sdesc, "unhealthy %s", buf );
  else if ( fi->health  <
   ( ( fi->species->class->health_max
     - fi->species->class->health_flower ) / 2 )
   + fi->species->class->health_flower )
    sprintf( sdesc, "%s", buf );
  else
    sprintf( sdesc, "flourishing %s", buf );

  return sdesc;
}


void show_plant( char_data *ch, struct plant_instance *fi )
{
  char buf[MSL];
  char *sdesc;

  if ( fi->species == NULL )
    return;

  sdesc = get_plant_sdesc( fi );

  if ( fi->quantity <= 0 )
    sprintf( buf, "BUG: %d %s seem to be here...\n\r", fi->quantity,
      sdesc );
  else if ( fi->quantity == 1 )
    sprintf( buf, "%s %s grows here.\n\r",
      is_vowel(sdesc[0]) ? "an" : "a",
      sdesc );
  else if ( fi->quantity < 4 )
    sprintf( buf, "A few %s grow here.\n\r", sdesc );
  else if ( fi->quantity < 10 )
    sprintf( buf, "%s dot the terrain.\n\r", sdesc );
  else
    sprintf( buf, "A forest of %s covers the land.\n\r", sdesc );

  ch->println( buf, ch );

  return;
}
/**************************************************************************/
// plants.cpp - plant creation
/***************************************************************************
 * Whispers of Times Lost (c)2001-2003 Brad Wilson                         *
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
#include "plants.h"


bool display_plant( char_data *ch, PLANT_DATA *plant, int idist )
{
  struct plant_instance *fi;
  bool found = false;

  for ( fi = plant; fi != NULL; fi = fi->next )
  {
    found = true;
    show_plant( ch, fi );
  }

  return found;
}


void view_plant_class( char_data *ch, struct plant_class *fc )
{
  char buf[MSL];

  sprintf( buf, "Flora Class: %s.\n", fc->name );
  ch->println( buf, ch );

  sprintf( buf, "Type: %s (%d).\n\r", plant_type_table[fc->type], fc->type );
  ch->println( buf, ch );

  sprintf( buf, "Health:  Flowering: %d, Max: %d.\n\r",
    fc->health_flower, fc->health_max );
  ch->println( buf, ch );

  sprintf( buf, "Water Desired: %d.\n\r",
    fc->water_desired );
  ch->println( buf, ch );

  return;
}


void view_plant_species( char_data *ch, struct plant_species *fs )
{
  char buf[MSL];
  OBJ_INDEX_DATA *pObjIndex;

  sprintf( buf, "Flora Species: %s.  Class: %s.\n",
    fs->name, fs->class->name );
  ch->println( buf, ch );

  sprintf( buf, "Flower Effort: %d.\n\r", fs->flower_effort );
  ch->println( buf, ch );

  pObjIndex = get_obj_index( fs->flower_vnum );
  sprintf( buf, "Flower: %s (%d), Max: %d.\n\r",
    pObjIndex ? pObjIndex->short_descr : "(none)",
    fs->flower_vnum, fs->flower_max );
  ch->println( buf, ch );

  pObjIndex = get_obj_index( fs->fruit_vnum );
  sprintf( buf, "Fruit: %s (%d), Max: %d.\n\r",
    pObjIndex ? pObjIndex->short_descr : "(none)",
    fs->fruit_vnum, fs->fruit_max );
  ch->println( buf, ch );

  sprintf( buf, "Growth: Seed: %d, Flowering: %d, Max: %d.\n\r",
    fs->growth_seed, fs->growth_flowering, fs->growth_max );
  ch->println( buf, ch );

  return;
}


void view_plant_instance( char_data *ch, struct plant_instance *fi )
{
  char buf[MSL];

  sprintf( buf, "Flora Species: %s.\n",
    fi->species->name );
  ch->println( buf, ch );

  sprintf( buf, "Health: %d.  Water: %d.  Growth: %d.  Quantity: %d.\n\r",
    fi->health, fi->water, fi->growth, fi->quantity );
  ch->println( buf, ch );

  sprintf( buf, "Flowers: %d.  Fruit: %d.\n\r",
    fi->flower, fi->fruit );
  ch->println( buf, ch );

  return;
}

struct plant_instance *get_plant_here( char_data *ch, char *arg )
{
  struct plant_instance *fi;

  for ( fi = ch->in_room->plant; fi != NULL; fi = fi->next )
  {
    if ( is_name( arg, fi->species->name ) )
      return fi;

    if ( is_name( arg, fi->species->class->name ) )
      return fi;

    if ( !str_cmp( arg, plant_type_table[fi->species->class->type] ) )
      return fi;
  }

  return NULL;
}

void update_fruit( struct plant_instance *fi )
{
  if ( fi->fruit == fi->species->fruit_max )
    fi->eiv_fruit = environment_interval;

  fi->fruit--;

  return;
}

void update_flower( struct plant_instance *fi )
{
  if ( fi->flower == fi->species->flower_max )
    fi->eiv_fruit = environment_interval;

  fi->flower--;

  return;
}


void update_plant( ROOM_INDEX_DATA *room )
{
  struct plant_instance *fi;
  int interval;

  if ( room->water_update >= environment_interval )
    return;

  interval = environment_interval - room->water_update;

  update_soil( room );

  for ( fi = room->plant; fi != NULL; fi = fi->next )
  {
    if ( fi->water < room->water )
      fi->water = room->water;
    else if ( fi->water > room->water )
      fi->water = UMAX( fi->water - interval, room->water );

    if ( fi->water >= fi->species->class->water_desired )
      fi->health += interval;
    else
      fi->health -= interval;

    if ( fi->health >= fi->species->class->health_flower / 2 )
      fi->growth += interval;

    if ( fi->health < fi->species->class->health_flower )
      fi->eiv_fruit = environment_interval;
    else
    {
      if ( ( fi->species->fruit_max != 0 || fi->species->flower_max != 0 )
      && fi->health >= fi->species->class->health_flower )
      {
        while ( ( environment_interval - fi->eiv_fruit )
              > fi->species->flower_effort )
        {
          if ( fi->fruit >= fi->species->fruit_max
	  && fi->species->fruit_max != -1 )
	    break;

          fi->fruit++;
          fi->eiv_fruit += fi->species->flower_effort;
        }

        while ( ( environment_interval - fi->eiv_fruit )
              > fi->species->flower_effort )
        {
          if ( fi->flower >= fi->species->flower_max
	  && fi->species->flower_max != -1 )
            break;

          fi->flower++;
          fi->eiv_fruit += fi->species->flower_effort;
        }
      }
    }
  }

  return;
}

void do_plant( char_data *ch, char *argument )
{
  struct plant_class *fc = NULL;
  struct plant_species *fs = NULL;
  struct plant_instance *fi = NULL;
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  char arg4[MAX_INPUT_LENGTH];
  char arg5[MAX_INPUT_LENGTH];

  argument = one_argument( argument, arg2 );

  if ( arg2[0] == '\0' )
  {
    ch->println( "Usage: plant <command> [args]\n\r", ch );
    ch->println( "Commands:\n\r", ch );
    ch->println( "  cedit <class> [..]  (edits plant class info)\n\r", ch );
    ch->println( "  destroy <plant>     (destroys plant instance)\n\r", ch );
    ch->println( "  new   <species>     (creates new tree in room)\n\r", ch );
    ch->println( "  save                (save current zone state)\n\r", ch );
    ch->println( "  sedit <species> [..]  (edits plant species info)\n\r",
      ch );
    ch->println( "  set   <plant> [..]  (edits plant instance info)\n\r", ch );
    ch->println( "  stat                (reviews plant in room)\n\r", ch );
    ch->println( "  view  <plant>   (views specified plant info)\n\r", ch );
    ch->println( "  write <class/species>  (writes global plant table)\n\r",
      ch );
    return;
  }

  if ( !str_cmp( arg2, "save" ) )
  {
    write_environment_area( ch->in_room->area );
    ch->println( "Flora state saved.\n\r", ch );
    return;
  }

  if ( !str_cmp( arg2, "stat" ) )
  {
    if ( ch->in_room->plant == NULL )
    {
      ch->println( "No plant in this room.\n\r", ch );
      return;
    }

    update_plant( ch->in_room );

    for ( fi = ch->in_room->plant; fi != NULL; fi = fi->next )
      view_plant_instance( ch, fi );

    return;
  }

  argument = one_argument( argument, arg3 );

  if ( arg3[0] == '\0' )
  {
    do_plant( ch, "" );
    return;
  }

  if ( !str_cmp( arg2, "new" ) )
  {
    fs = plant_species_lookup( arg3 );
    if ( !fs )
    {
      ch->println( "Species of plant does not exist.\n\r", ch );
      return;
    }

    fi = new_plant_instance( );
    fi->species = fs;
    fi->quantity = 1;

    fi->next = ch->in_room->plant;
    ch->in_room->plant = fi;

    ch->println( "Plant created.\n\r", ch );
    return;
  }

  if ( !str_cmp( arg2, "write" ) )
  {
    if ( get_trust( ch ) < MAX_LEVEL )
      return;

    if ( !str_cmp( arg3, "class" ) )
    {
      write_plant_class_file( );
      ch->println( "Flora class file written.\n\r", ch );
    }
    else if ( !str_cmp( arg3, "species" ) )
    {
      write_plant_species_file( );
      ch->println( "Flora species file written.\n\r", ch );
    }
    else
      ch->println( "You must specify class or species.\n\r", ch );

    return;
  }

  if ( !str_cmp( arg2, "view" ) )
  {
    fc = plant_class_lookup( arg3 );
    if ( fc == NULL )
      ch->println( "No plant class by that name.\n\r", ch );
    else
      view_plant_class( ch, fc );

    fs = plant_species_lookup( arg3 );
    if ( fs == NULL )
      ch->println( "No plant species by that name.\n\r", ch );
    else
      view_plant_species( ch, fs );

    if ( fs && fs->class != fc )
    {
      ch->println( "Displaying associated plant class.\n\r", ch );
      view_plant_class( ch, fs->class );
    }

    return;
  }

  if ( !str_cmp( arg2, "destroy" ) )
  {
    fi = get_plant_here( ch, arg3 );

    if ( fi == NULL )
    {
      ch->println( "Flora instance not found.\n\r", ch );
      return;
    }

    destroy_plant( fi, ch->in_room );

    ch->println( "Flora destroyed.\n\r", ch );
    return;
  }

  argument = one_argument( argument, arg4 );
  argument = one_argument( argument, arg5 );

  if ( arg4[0] == '\0' || arg5[0] == '\0' )
  {
    do_plant( ch, "" );
    return;
  }

  if ( !str_cmp( arg2, "set" ) )
  {
    int n = atoi( arg5 );

    fi = get_plant_here( ch, arg3 );

    if ( fi == NULL )
    {
      ch->println( "Flora instance not found.\n\r", ch );
      return;
    }

    if ( !str_prefix( arg4, "flower" ) )
      fi->flower = n;
    else if ( !str_prefix( arg4, "fruit" ) )
      fi->fruit = n;
    else if ( !str_prefix( arg4, "growth" ) )
      fi->growth = n;
    else if ( !str_prefix( arg4, "health" ) )
      fi->health = n;
    else if ( !str_prefix( arg4, "quantity" ) )
      fi->quantity = n;
    else if ( !str_prefix( arg4, "water" ) )
      fi->water = n;
    else
      ch->println( "Syntax: plant set <plant> <attribute> <number>\n\r"
     "  Attribute is:  flower, fruit, growth, health, quantity, or water.\n\r",
      ch );

    return;
  }

  do_plant( ch, "" );
  return;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/


