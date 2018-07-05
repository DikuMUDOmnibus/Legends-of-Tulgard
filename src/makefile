MAKE	= "make"
CPP     = g++
PROF	= -g -O
OBJDIR  = obj
VPATH   = .:obj
NOCRYPT	=
C_FLAGS = -Wall $(PROF) $(NOCRYPT)
L_FLAGS = $(PROF)
LIBS	= -lz

O_FILES = act_comm.o act_ente.o act_info.o act_move.o act_obj.o altform.o\
	act_wiz2.o act_wiz.o aedit.o affects.o afteraff.o alias.o areas.o autostat.o \
	banedit.o ban.o bard.o base64.o bit.o cedit.o channels.o chardata.o \
	clanedit.o clan.o class.o colour.o comedit.o comm.o const.o \
	connect.o corpse.o craft.o cust_col.o d2magsys.o dawnlib.o dawnlog.o dawnstat.o db2.o db.o \
	debug.o deity.o dream.o duel.o dynamics.o dyntable.o effects.o \
	exitlist.o ey_crypt.o fight.o flags.o ftp.o gameedit.o games.o \
	gio.o global.o grpedit.o handler.o healer.o hedit.o help.o \
	herbedit.o highlander.o homes.o hreboot.o ictime.o immquest.o interp.o intro_ex.o \
	intro.o ispell.o jail.o justice.o langedit.o language.o laston.o letters.o lockers.o \
    	lookup.o magic_ce.o magic_da.o magic_ja.o magic_ke.o magic.o magic_ob.o \
	magic_ne.o magic_qu.o magic_ra.o magic_re.o magic_sb.o magic_ti.o map.o match.o \
    	medit.o mem.o mixedit.o mix.o mob_cmds.o mob_prog.o mob_q.o mount.o \
    	msp.o mud2web.o mxp.o namegen.o nanny.o netio.o network.o noble.o notenet.o \
	note.o obdb.o obskill.o oedit.o offmoot.o olc_act.o olc_ex.o \
	olc_mpcd.o olc.o olc_save.o o_lookup.o password.o pets.o pigeon.o pushdrag.o pipe.o \
    	pload.o qedit.o quest.o raceedit.o races.o random_obj.o recycle.o redit.o remort.o resolve.o \
	roles.o room-mobs.o rp.o save.o saymote.o scan.o score.o scripts.o sedit.o shop.o \
	skill_ke.o skills.o skill_ti.o sk_type.o smoking.o socedit.o socials.o \
	special.o statset.o storage.o string.o support.o tables.o territory.o textsrch.o \
	tokens.o track.o trap.o update.o vampire.o vehicle.o websrv.o whofmt.o who.o wizlist.o

all: dawn resolver

# GNU related make 
dawn: $(sort $(O_FILES))
	$(CPP) -c $(C_FLAGS) com_time.cpp -o $(OBJDIR)/com_time.o
	-rm -f ../dawn
	$(CPP) $(L_FLAGS) $(LIBS) -o ../dawn \
		$(addprefix $(OBJDIR)/,$(O_FILES)) $(OBJDIR)/com_time.o

# BSD related make 
#dawn: $(O_FILES)
#	$(CPP) -c $(C_FLAGS) $(.CURDIR)/com_time.cpp -o com_time.o
#	-rm -f $(.CURDIR)/../dawn
#	$(CPP) $(L_FLAGS) $(LIBS) -o $(.CURDIR)/../dawn \
#		$(O_FILES) $(.OBJDIR)/com_time.o

# STD related make 
#dawn: $(O_FILES)
#	$(CPP) -c $(C_FLAGS) $(.CURDIR)/com_time.cpp -o $(.OBJDIR)/com_time.o
#	-rm -f ../dawn
#	$(CPP) $(L_FLAGS) $(LIBS) -o ../dawn \
#		$(addprefix $(OBJDIR)/,$(O_FILES)) $(OBJDIR)/com_time.o

# GNU resolver make
../resolver: extras/resolver.cpp
	$(CPP) $(L_FLAGS) -o ../resolver extras/resolver.cpp

# BSD resolver make
#../resolver: ../../resolver
#../../resolver: ../extras/resolver.cpp
#	$(CPP) $(L_FLAGS) -o ../../resolver ../extras/resolver.cpp

# STD resolver make
#../resolver: extras/resolver.cpp
#	$(CPP) $(L_FLAGS) -o ../resolver extras/resolver.cpp

resolver: ../resolver
	
clean:
	rm -f $(OBJDIR)/*.o
#	rm -f *.o
#	rm -f *.o

cleanbin:
	rm -f ../dawn
	rm -f ../resolver
#	rm -f ../../dawn
#	rm -f ../../resolver
#	rm -f ../dawn
#	rm -f ../resolver

resetconfig:
	rm -f $(OBJDIR)/*.o
	rm -f configure/config.cache
	rm -f configure/config.log
	rm -f configure/config.status
	rm -f ./config.h
	rm -f ./makefile
	cp -f configure/makefile.original ./makefile

rebuild: clean dawn

rebuildall: cleanbin clean all

.cpp.o: dawn.h
	$(CPP) -c $(C_FLAGS) $< -o $(OBJDIR)/$(basename $<).o
#	$(CPP) -c $(C_FLAGS) $< -o $*.o
#	$(CPP) -c $(C_FLAGS) $< -o $*.o

%.o : %.cpp
	$(CPP) -c $(C_FLAGS) $< -o $(OBJDIR)/$(basename $<).o
#	$(CPP) -c $(C_FLAGS) $< -o $*.o
#	$(CPP) -c $(C_FLAGS) $< -o $*.o

beginhelp:
	@echo =============================================================================
	@echo Change into the configure directory and run ./configure to create a makefile
	@echo 
	@echo Note: If the makefile has problems compiling the resolver, manually compile
	@echo "      extras/resolver.cpp as resolver in the directory above the source and"
	@echo "      remove the resolver reference from the 'all' line in the makefile."
	@echo =============================================================================

