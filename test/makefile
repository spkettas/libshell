#mk

CC            = gcc
CXX           = g++
DEFINES       = -D_SERVER_MODE
CFLAGS        = -pipe -g -fPIC -Wall -W $(DEFINES)
CXXFLAGS      = -pipe -g -fPIC -Wall -W $(DEFINES)
AR            = ar cqs
LINK          = g++
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= tester -d
MKDIR         = mkdir -p

### target
ROOT	          = ..
LIBS 		  = -lpthread -L$(ROOT)/lib -lshell
INCPATH           = -I.. -I../include
SRC_DIR 	  = ./
OBJECTS_DIR	  = $(ROOT)/objs
BIN_DIR 	  = $(ROOT)/bin
TARGET 		  = $(BIN_DIR)/myshell \
		    $(BIN_DIR)/myserv \
		    $(BIN_DIR)/tester

### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"


### code
SOURCES 		= $(wildcard *.c *.cpp)
#OBJS 			= $(patsubst %.c,%.o, $(patsubst %.cpp,%.o, $(SOURCES)))

### more objs
OBJS 			= 	


### Task list
all: app

### Build rules
app: $(TARGET)

$(BIN_DIR)/myshell: $(OBJECTS_DIR)/myshell.o
	$(LINK) -o $@ $^ $(LIBS)

$(BIN_DIR)/myserv: $(OBJECTS_DIR)/myserv.o
	$(LINK) -o $@ $^ $(LIBS)

$(BIN_DIR)/tester: $(OBJECTS_DIR)/tester.o
	$(LINK) -o $@ $^ $(LIBS)


$(OBJECTS_DIR)/myshell.o: $(SRC_DIR)/myshell.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

$(OBJECTS_DIR)/myserv.o: $(SRC_DIR)/myserv.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

$(OBJECTS_DIR)/tester.o: $(SRC_DIR)/tester.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.PHONY: clean

clean:
	-$(DEL_FILE) $(OBJECTS_DIR)/* $(BIN_DIR)/*
	-$(DEL_FILE) *~ core *.core

