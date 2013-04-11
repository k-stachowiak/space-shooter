EXENAME := shoot

CXX = g++
LIBS = -lallegro_monolith		# GNU/Linux
# LIBS = -lallegro-5.0.8-monolith-md	# Win
CXXFLAGS := -Wall --std=gnu++11 -g

OBJS := src/resources/resman.o \
	src/states/test_state.o \
	src/geometry/bezier.o \
	src/gameplay/wave.o \
	src/efwk/sys_arms.o \
	src/efwk/sys_wellness.o \
	src/efwk/cmp_col_profile.o \
	src/efwk/cmp_timer.o \
	src/efwk/sys_sound.o \
	src/efwk/cmp_weapon_beh.o \
	src/efwk/sys_hud.o \
	src/efwk/cmp_misc.o \
	src/efwk/sys_input.o \
	src/efwk/sys_movement.o \
	src/efwk/sys_collision.o \
	src/efwk/comm.o \
	src/efwk/entity_factory.o \
	src/efwk/sys_score.o \
	src/efwk/sys_fx.o \
	src/efwk/sys_pain.o \
	src/efwk/sys_pickup.o \
	src/efwk/cmp_dynamics.o \
	src/efwk/cmp_fx.o \
	src/efwk/sys_drawing.o \
	src/efwk/cmp_shape.o \
	src/efwk/cmp_reaction.o \
	src/efwk/cmp_appearance.o \
	src/script/dom.o \
	src/misc/rand.o \
	src/misc/config.o \
	src/main.o \
	src/allegro.o

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

