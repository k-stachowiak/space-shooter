EXENAME := shoot

CXX = g++
LIBS = -lallegro_monolith               # GNU/Linux
# LIBS = -lallegro-5.0.8-monolith-md    # Win
CXXFLAGS := -Wall -Werror --std=gnu++11 -g

OBJS := src/resources/resman.o \
	src/efwk/comm.o \
	src/efwk/game_ent_fac.o \
	src/efwk/sys/collision.o \
	src/efwk/sys/arms.o \
	src/efwk/sys/score.o \
	src/efwk/sys/wellness.o \
	src/efwk/sys/pain.o \
	src/efwk/sys/movement.o \
	src/efwk/sys/drawing.o \
	src/efwk/sys/sound.o \
	src/efwk/sys/fx.o \
	src/efwk/sys/pickup.o \
	src/efwk/sys/input.o \
	src/efwk/sys/hud.o \
	src/efwk/cmp/dynamics.o \
	src/efwk/cmp/appr.o \
	src/efwk/cmp/col_profile.o \
	src/efwk/cmp/pickup_profile.o \
	src/efwk/cmp/shape.o \
	src/efwk/cmp/timer.o \
	src/efwk/cmp/fx.o \
	src/efwk/cmp/weapon_beh.o \
	src/efwk/cmp/reaction.o \
	src/efwk/cmp/misc.o \
	src/main.o \
	src/misc/config.o \
	src/misc/rand.o \
	src/misc/logger.o \
	src/misc/high_score.o \
	src/script/dom.o \
	src/script/expect.o \
	src/allegro.o \
	src/geometry/bezier.o \
	src/gameplay/wave.o \
	src/states/game_state.o \
	src/states/menu_state.o \
	src/states/hs_enter_state.o \
	src/states/hs_display_state.o

# Link.
# -----
exe : $(OBJS)
	$(CXX) $(OBJS) $(LIBS) -o $(EXENAME)

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

# compile and generate dependency info;
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $*.cpp -o $*.o
	$(CXX) -MM $(CXXFLAGS) $*.cpp > $*.d
	@cp -f $*.d $*.d.tmp
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

# remove compilation products
clean:
	rm -f $(EXENAME)
	find src -name "*.o" -o -name "*.d" | xargs rm -f

.PHONY: clean

