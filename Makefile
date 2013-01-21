EXENAME := shoot

CXX = g++
LIBS = -lallegro_monolith
CXXFLAGS := -Wall --std=gnu++0x -g

OBJS := src/cmp/comm.o \
	src/cmp/components.o \
	src/cmp/entity_factory.o \
	src/cmp/systems.o \
	src/geometry/bezier.o \
	src/misc/rand.o \
	src/resources/resman.o \
	src/states/test_state.o \
	src/allegro.o \
	src/main.o

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
