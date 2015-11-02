ATHENA_ROOT = ../
include $(ATHENA_ROOT)/Makefile.inc

MODE ?= debug

Base_Name=PathFinder

ifeq ($(MODE),debug)
Out_Dir = ./Debug
CFLAGS += $(C_DEBUG_OPT)
DEFINES += $(DEBUG_DEFINES)
Target_Dir = $(DEBUG_LIB_PATH)
else
Out_Dir=./Release
Target_Dir = $(RELEASE_LIB_PATH)
CFLAGS += $(C_RELEASE_OPT)
DEFINES += $(RELEASE_DEFINES)
LFLAGS += -O2
endif

DEFINES += -DSERVER -D_SERVER -D_AISERVER -D_LIB -D_USE_TCP -D_EDITOR

INCLUDES += 		-I"$(SYSTEM_INC)"	\
			-I"$(INC_ROOT)"				\
			-I"./"		\
			$(NULL)

ifeq ($(MODE),debug)			
LIBPATHS += -L"$(LIB_ROOT)/Debug" -L"$(LIB_SERVER_ROOT)/Debug"
else
LIBPATHS += -L"$(LIB_ROOT)/Release" -L"$(LIB_SERVER_ROOT)/Release"
endif

LIBS = 
LFLAGS += $(LIBPATHS)

Target_Name=lib$(Base_Name).a
Target=$(Target_Dir)/$(Target_Name)

Source=.

Copy_Target_Dir=$(ATHENA_ROOT)/Inc/PathFinder
Copy_Target_Headers = $(patsubst %.h,$(Copy_Target_Dir)/%.h,$(shell find -P . -name "*.h"))

Objects =  $(patsubst %.cpp,$(Out_Dir)/%.o,$(shell find -P . -name "*.cpp"))
Objects += $(patsubst %.c,$(Out_Dir)/%.o,$(shell find -P . -name "*.c"))

all : $(Out_Dir) $(Target_Dir) $(Target) $(Copy_Target_Headers)
rebuild : clean all

$(Target) : $(Objects)
	$(AR) -rv $@ $^
	
$(Out_Dir) :
	$(MKDIR) -p $@
	
$(Target_Dir) :
	$(MKDIR) -p $@

$(Out_Dir)/%.o : %.cpp
	@if [ -d $(shell dirname $@) ] ; \
	then \
		echo ">>>>>>>> $< ......"; \
	else \
		echo ">>>>>>>> $< ......"; \
		$(MKDIR) -p $(shell dirname $@); \
	fi
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

$(Out_Dir)/%.o : %.c
	@if [ -d $(shell dirname $@) ] ; \
	then \
		echo ">>>>>>>> $< ......"; \
	else \
		echo ">>>>>>>> $< ......"; \
		$(MKDIR) -p $(shell dirname $@); \
	fi
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<
	
$(Copy_Target_Dir)/%.h : %.h
	@if [ -d $(shell dirname $@) ] ; \
	then \
		echo "copy $< to $@"; \
	else \
		echo "copy $< to $@"; \
		$(MKDIR) -p $(shell dirname $@); \
	fi
	@$(CP) -f $< $@

clean :
	$(RM) -f $(Target)
	$(RM) -f $(Objects)
	$(RM) -f $(Copy_Target_Dir)/*.h

