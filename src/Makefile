CXX = g++

CXXFLAGS = -O3 -g -DNDEBUG -Wno-deprecated -I./ 
#CXXFLAGS = -O3 -DNDEBUG -W -Wall -Wno-deprecated -I./ -L./
#CXXFLAGS = -g -DNDEBUG -Wno-deprecated -pedantic -ansi -finline-functions -foptimize-sibling-calls -Wcast-qual -Wwrite-strings -Wsign-promo -Wcast-align -Wno-long-long -fexpensive-optimizations -funroll-all-loops -ffast-math -fomit-frame-pointer -pipe -I./ -L./
#CXXFLAGS = -O3 -DNDEBUG -Wno-deprecated -pedantic -ansi -finline-functions -foptimize-sibling-calls -Wcast-qual -Wwrite-strings -Wsign-promo -Wcast-align -Wno-long-long -fexpensive-optimizations -funroll-all-loops -ffast-math -fomit-frame-pointer -pipe -I./ -L./

LINKFLAGS = -lm

SRCS = \
	Graph.cpp \
	WLKernel.cpp \
	gWM.cpp \
	rank9sel.cpp \

SRCS1 = \
	Build.cpp 

SRCS2 = \
	Search.cpp

OBJS  = $(SRCS:%.cpp=%.o)
OBJS1 = $(SRCS1:%.cpp=%.o) 
OBJS2 = $(SRCS2:%.cpp=%.o)

all: gwt-build gwt-search

gwt-build: $(OBJS) $(OBJS1)
	$(CXX) $(CXXFLAGS) $(OTHERFLAGS) $(OBJS) $(OBJS1) $(LINKFLAGS) -o gwt-build
gwt-search: $(OBJS) $(OBJS2)
	$(CXX) $(CXXFLAGS) $(OTHERFLAGS) $(OBJS) $(OBJS2) $(LINKFLAGS) -o gwt-search
debug:
	make all CXXFLAGS="-ggdb -W -Wall -pedantic"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(OTHERFLAGS) -c $<

clean:
	rm -f gwt-build gwt-search *.o *~
