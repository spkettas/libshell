#mk

CC            = gcc
CXX           = g++
DEFINES       =
CFLAGS        = -pipe -g -fPIC -Wall -W $(DEFINES)
CXXFLAGS      = -pipe -g -fPIC -Wall -W $(DEFINES)
AR            = ar cqs
LINK          = g++
LFLAGS        = -shared #-Wl,-soname,libhead2.so.1
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

### target
ROOT	  = ./
LIBS 		  = -lpthread -L/usr/local/lib -lreadline -lncurses
INCPATH       = -I. -I./include
SRC_DIR 	  = ./
OBJECTS_DIR	  = $(ROOT)/objs
BIN_DIR 	  = $(ROOT)/lib
MYTARGET	  = libshell
TARGET        = $(MYTARGET).so.1.0.0
TARGETA       = $(MYTARGET).a
TARGETD       = $(MYTARGET).so.1.0.0
TARGET0       = $(MYTARGET).so
TARGET1       = $(MYTARGET).so.1
TARGET2       = $(MYTARGET).so.1.0

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
OBJS 			= 	$(OBJECTS_DIR)/cli_config.o \
					$(OBJECTS_DIR)/cli_server.o \
					$(OBJECTS_DIR)/cli_shell.o \
					$(OBJECTS_DIR)/tinyxml2.o


### Task list
all: dylib

### Build rules
dylib: $(TARGET)

$(TARGET): $(OBJECTS_DIR) $(OBJS)
	@$(CHK_DIR_EXISTS) $(BIN_DIR)/ || $(MKDIR) $(BIN_DIR)/
	-$(DEL_FILE) $(TARGET) $(TARGET0) $(TARGET1) $(TARGET2)
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJS) $(LIBS)
	-ln -s $(TARGET) $(TARGET0)
	-ln -s $(TARGET) $(TARGET1)
	-ln -s $(TARGET) $(TARGET2)
	-$(DEL_FILE) $(BIN_DIR)/$(TARGET)
	-$(DEL_FILE) $(BIN_DIR)/$(TARGET0)
	-$(DEL_FILE) $(BIN_DIR)/$(TARGET1)
	-$(DEL_FILE) $(BIN_DIR)/$(TARGET2)
	-$(MOVE) $(TARGET) $(TARGET0) $(TARGET1) $(TARGET2) $(BIN_DIR)/

staticlib: $(TARGETA)

$(TARGETA): $(OBJECTS_DIR) $(OBJS)
	@$(CHK_DIR_EXISTS) $(BIN_DIR)/ || $(MKDIR) $(BIN_DIR)/
	-$(DEL_FILE) $(TARGETA)
	$(AR) $(TARGETA) $(OBJS)
	-$(MOVE) $(TARGETA) $(BIN_DIR)/


$(OBJECTS_DIR):
	$(CHK_DIR_EXISTS) $@ || mkdir -p $@

$(OBJECTS_DIR)/%.o: $(SRC_DIR)/src/%.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"


.PHONY: clean

clean:
	-$(DEL_FILE) $(OBJS)
	-$(DEL_FILE) *~ core *.core


