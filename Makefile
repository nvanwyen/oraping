#
#
#
WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = -I$(ORACLE_HOME)/include
CFLAGS = -std=c++11 -Wall -fexceptions -Wwrite-strings
RESINC = 
LIBDIR = -L$(ORACLE_HOME)/lib
LIB = -lclntsh
LDFLAGS = 

INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = obj/debug
DEP_DEBUG = 
OUT_DEBUG = bin/debug/oraping

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O2
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = obj/release
DEP_RELEASE = 
OUT_RELEASE = bin/release/oraping

OBJ_DEBUG = $(OBJDIR_DEBUG)/main.o $(OBJDIR_DEBUG)/ora.o $(OBJDIR_DEBUG)/tns.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/main.o $(OBJDIR_RELEASE)/ora.o $(OBJDIR_RELEASE)/tns.o

all: debug release

clean: clean_debug clean_release

before_debug: 
	test -d $(ORACLE_HOME)/
	test -d bin/debug || mkdir -p bin/debug
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)

after_debug: 

debug: oracle_env before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/main.o: main.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c main.cpp -o $(OBJDIR_DEBUG)/main.o

$(OBJDIR_DEBUG)/ora.o: ora.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ora.cpp -o $(OBJDIR_DEBUG)/ora.o

$(OBJDIR_DEBUG)/tns.o: tns.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c tns.cpp -o $(OBJDIR_DEBUG)/tns.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf bin/debug
	rm -rf $(OBJDIR_DEBUG)

before_release: 
	test -d bin/release || mkdir -p bin/release
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)

after_release: 

release: oracle_env before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/main.o: main.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c main.cpp -o $(OBJDIR_RELEASE)/main.o

$(OBJDIR_RELEASE)/ora.o: ora.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ora.cpp -o $(OBJDIR_RELEASE)/ora.o

$(OBJDIR_RELEASE)/tns.o: tns.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c tns.cpp -o $(OBJDIR_RELEASE)/tns.o

oracle_env:
ifndef ORACLE_HOME
	$(error ORACLE_HOME not defined)
endif

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf bin/release
	rm -rf $(OBJDIR_RELEASE)

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release

