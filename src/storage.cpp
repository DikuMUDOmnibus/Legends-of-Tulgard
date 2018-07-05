/**************************************************************************/
// storage.cpp - Storage Room Code
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/
#include "include.h"

/**************************************************************************/
void fwrite_obj( obj_data *obj, FILE *fp, int iNest, char* heading );
/**************************************************************************/
void save_storage_db(char_data *ch, char *)
{
    	obj_data *obj=NULL;
	FILE *fp;

	ch->println("Saving objects in storage rooms");
	char write_filename[MIL];
	char storagename[MIL];

	ROOM_INDEX_DATA *r;
	// loop thru every room, if there are corpses in a room, we save the room
	for(int i=0; i<MAX_KEY_HASH; i++){
		for( r=room_index_hash[i % MAX_KEY_HASH]; r; r=r->next ){
		    if(IS_SET(r->room2_flags, ROOM2_STORAGE))
		    {
		// 	ch->printlnf("Saving Room %d.", r->vnum);

			sprintf(storagename, "%s%d.txt", STORAGE_DIR, r->vnum);
			sprintf(write_filename, "%s.write", storagename);
			if(file_exists(write_filename)){
			for(int i=0; i<20; i++){
				sprintf(write_filename,"%s.write%d", storagename, i);
				if(!file_exists(write_filename)){
					break;
				}
			}
		    }

		    	unlink(write_filename);

		    	fclose( fpReserve );
    		    	if ( ( fp = fopen( write_filename, "w" ) ) == NULL ){
				bugf("save_storage_db(): fopen '%s' failed for write - error %d (%s)",
				write_filename, errno, strerror( errno));
	    			fpReserve = fopen( NULL_FILE, "r" );
				return;
    		    	}

			bool stay_in_room=true; // set to false if we find a corpse in the room
			obj = r->contents;
			if(obj){
				fprintf(fp, "OBJECTS_IN_ROOM %d\n", r->vnum);
				fwrite_obj(obj, fp, 0, "O");
				fprintf(fp, "\n");
			}
			stay_in_room=false;

			int bytes_written=fprintf(fp, "EOF\n");
			fclose( fp );
			if(   bytes_written != str_len("EOF\n") ){
				bugf("Incomplete write of %s, write aborted - check diskspace! - error %d (%s)", 
				write_filename, errno, strerror( errno));
	    		fpReserve = fopen( NULL_FILE, "r" );
			}else{
				logf("Renaming new %s to %s", write_filename, storagename);
				unlink(storagename);
				rename(write_filename, storagename);
			}

    			fpReserve = fopen( NULL_FILE, "r" );

		    }
		}
	}

	ch->println("Finished saving objects in storage rooms.");
}

/**************************************************************************/
obj_data * fread_obj    ( FILE *fp, const char *filename );
/**************************************************************************/
void old_load_storage_db(char_data *ch, char *)
{
    obj_data *obj;
	FILE *fp;

	ch->println("Loading objects in storage rooms...");
	logf("Loading objects in storage rooms...");

	if(!file_exists(STORAGE_FILE)){
		ch->printlnf("No storage file (%s) exists..",STORAGE_FILE);
		logf("No storage file (%s) exists..",STORAGE_FILE);
		return;
	}

	fclose( fpReserve );
    if ( ( fp = fopen( STORAGE_FILE, "r" ) ) == NULL ){
		bugf("do_load_corpses(): fopen '%s' failed for read - error %d (%s)",
			STORAGE_FILE, errno, strerror( errno));
	    fpReserve = fopen( NULL_FILE, "r" );
		return;
    }

	int count=0;
	int room_number=0;
	room_index_data *room=NULL;
			
	for( ; ; ){
		char *word= feof( fp ) ? (char*)"EOF" : fread_word( fp );
		
		if(!str_cmp(word,"EOF")){
			break;
		}

		if(!str_cmp(word,"OBJECTS_IN_ROOM")){
			room_number=fread_number(fp);
			room=get_room_index(room_number);
			
			/* reset the read in nest
			int iNest;	
			for ( iNest = 0; iNest < MAX_NEST; iNest++ ){
				rgObjNest[iNest] = NULL;
			} */
		}else if(!str_cmp(word,"#O")){
			obj=fread_obj(fp, STORAGE_FILE);
			if(obj){
				if(room){
					obj_to_room(obj, room);
					count++;
				}else{
					logf("room %d not found to load object into!", room_number);
				}				
			}
		}else{
			logf("unrecognised word in %s file '%s'", STORAGE_FILE, word);
		}
	}

	fclose(fp);
    fpReserve = fopen( NULL_FILE, "r" );

	log_string ("old_load_storage_db(): finished");
	ch->printlnf("old_load_storage_db(): finished");
}
/**************************************************************************/
void load_storage_db(char_data *ch, char *)
{
    	obj_data *obj;
	FILE *fp;
	char storagename[MIL];

	ch->println("Loading objects in storage rooms...");
	logf("Loading objects in storage rooms...");


	ROOM_INDEX_DATA *r;
	// loop thru every room, if there are corpses in a room, we save the room
	for(int i=0; i<MAX_KEY_HASH; i++){
		for( r=room_index_hash[i % MAX_KEY_HASH]; r; r=r->next ){
		    if(IS_SET(r->room2_flags, ROOM2_STORAGE))
		    {

			sprintf(storagename, "%s%d.txt", STORAGE_DIR, r->vnum);

		//	ch->printlnf("Loading Room %d.", r->vnum);
			bool stay_in_room=true; // set to false if we find a corpse in the room


	if(!file_exists(storagename)){
		ch->printlnf("No storage file (%s) exists..",storagename);
		logf("No storage file (%s) exists..",storagename);
		return;
	}

	fclose( fpReserve );
    	if ( ( fp = fopen( storagename, "r" ) ) == NULL ){
		bugf("do_load_corpses(): fopen '%s' failed for read - error %d (%s)",
			storagename, errno, strerror( errno));
	    fpReserve = fopen( NULL_FILE, "r" );
		return;
    	}

	int count=0;
	int room_number=0;
	room_index_data *room=NULL;
			
	for( ; ; ){
		char *word= feof( fp ) ? (char*)"EOF" : fread_word( fp );
		
		if(!str_cmp(word,"EOF")){
			break;
		}

		if(!str_cmp(word,"OBJECTS_IN_ROOM")){
			room_number=fread_number(fp);
			room=get_room_index(room_number);
			
			/* reset the read in nest
			int iNest;	
			for ( iNest = 0; iNest < MAX_NEST; iNest++ ){
				rgObjNest[iNest] = NULL;
			} */
		}else if(!str_cmp(word,"#O")){
			obj=fread_obj(fp, storagename);
			if(obj){
				if(room){
					obj_to_room(obj, room);
					count++;
				}else{
					logf("room %d not found to load object into!", room_number);
				}				
			}
		}else{
			logf("unrecognised word in %s file '%s'", storagename, word);
		}
	}

	fclose(fp);
    	fpReserve = fopen( NULL_FILE, "r" );


			stay_in_room=false;
		    }
		}
	}


	log_string ("new_load_storage_db(): finished");
	ch->printlnf("new_load_storage_db(): finished");

}
/**************************************************************************/
/**************************************************************************/

