ATHENA_ROOT = ..
include $(ATHENA_ROOT)/Makefile.inc

Base_Name=TestChatClient
Out_Dir = $(TMP_PATH)
Target_Dir = $(Out_Dir)

DEFINES+=

INCLUDES += -I"$(INC_COMMONLOGIC)"	\
	-I"$(INC_ROOT)"			\
	-I"$(INC_SERVER_ROOT)"		\
	-I"../Inc"	\
	$(NULL)

LIBS = \
	-lCommonLogic \
	-llua \
	-lSockServer \
	-lSystem \
	$(NULL)

LFLAGS += $(LIBPATHS)

Target_Name=$(Base_Name)
Target=$(Target_Dir)/$(Target_Name)

Source=.

Objects =  $(patsubst %.cpp,$(Out_Dir)/%.o,$(shell find -P . -name "*.cpp"))
Objects += $(patsubst %.c,$(Out_Dir)/%.o,$(shell find -P . -name "*.c"))

all : $(Out_Dir) $(Target_Dir) $(Target)
rebuild : clean all

$(Target) : $(Objects)
	$(LD) $(LFLAGS) -o $@ $^ $(LIBS)
	
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
	
clean :
	$(RM) -f $(Target)
	$(RM) -f $(Objects)

