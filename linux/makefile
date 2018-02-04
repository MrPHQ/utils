
CFLAGS += -DUSE_CFILE -D__STDC_CONSTANT_MACROS -fexceptions -Wno-write-strings -std=c++11

#LINK_FLAG += -shared -fno-exceptions -lpthread -rdynamic -ldl -g
LINK_FLAG += -shared -fno-exceptions -lpthread -g

BINPATH = ../bin
BINOBJ := $(BINPATH)/libutils.so

#XHEADER_PATH=-I../sdk/mysql++ -I../sdk/mysql -I../sdk -I../sdk/pcre
#XLIB_PATH=-L../sdk/lib -L../../07-public/libsys/bin
#XDEPEND_LIB=-Wl,-Bdynamic -lmysqlpp -lmysqlclient -lsys -Wl,-Bstatic -lboost_regex
XHEADER_PATH=
XLIB_PATH=
XDEPEND_LIB=

SRC = ./src
OBJ = ./obj

#提前所有源文件(即：*.c文件)和所有中间文件(即：*.o)  
SOURCE = $(wildcard ${SRC}/*.c ${SRC}/*.cpp)
OBJECT = $(patsubst %.cpp,${OBJ}/%.o,$(notdir ${SOURCE}))

BINOBJ: $(OBJECT)
	$(GXX) -g -o $(BINOBJ) $(OBJECT) $(LINK_FLAG) $(XLIB_PATH) $(XDEPEND_LIB)
	# $(STRIP) $(BINOBJ)

#生成各个中间文件  
${OBJ}/%.o:${SRC}/%.c
	$(GXX) $(XHEADER_PATH) $(CFLAGS) -g -o $@ -c $<
	
${OBJ}/%.o:${SRC}/%.cpp
	$(GXX) $(XHEADER_PATH) $(CFLAGS) -g -o $@ -c $<

clean:
	$(RM) $(RMFLAGS) $(OBJECT) 
	$(RM) $(RMFLAGS) $(BINOBJ)

prepare:
	$(RM) $(RMFLAGS) $(SENSORS) 
	$(RM) $(RMFLAGS) $(BINOBJ)
