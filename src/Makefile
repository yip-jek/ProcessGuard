CPP      = GNU_G++
CPP_FLAG = -g -m64 -Wall -O2 -DLINUX
RM       = rm -f

APP_DIR  = ..
APP_INCL = $(APP_DIR)/include
BIN_DIR  = $(APP_DIR)/bin

INCLS = -I$(APP_INCL)
#LIBS = 

srcs = $(wildcard *.cpp)
objs = $(srcs:.cpp=.o)
bin  = $(BIN_DIR)/processguard

.PHONY: all clean
all: $(bin)
	@echo "All made!"

$(bin): $(objs)
	#$(CPP) $(CPP_FLAG) -o $@ $(objs) $(LIBS)
	$(CPP) $(CPP_FLAG) -o $@ $(objs)

.cpp.o:
	$(CPP) $(CPP_FLAG) $(INCLS) -c $<

clean:
	@echo "[CLEAN] Deleting $(bin)"
	@$(RM) $(bin)
	@echo "[CLEAN] Deleting $(objs)"
	@$(RM) $(objs)

