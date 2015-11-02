ATHENA_ROOT = ../../Athena
include $(ATHENA_ROOT)/Makefile.inc

MODE ?= debug

Base_Name=CommonLogic

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

DEFINES += -DSERVER -D_SERVER -D_LIB

INCLUDES += -I"$(SYSTEM_INC)/System"	\
			-I"$(INC_ROOT)"				\
			-I"$(INC_SERVER_ROOT)"		\
			-I"$(INC_PACKETGATE)"		\
			-I"$(INC_MYSQL)"			\
			-I"$(ATHENA_ROOT)/Base"		\
			-I"$(CVSROOT)/SockServer"	\
			-I"$(INC_COMMONLOGIC)"		\
			-I"$(INC_OPENSSL)"		\
			$(NULL)

ifeq ($(MODE),debug)			
LIBPATHS += -L"$(DEBUG_LIB_PATH)" -L"$(SERVER_DEBUG_LIB_PATH)" -L"$(OPENSSL_LIB_PATH)"
else
LIBPATHS += -L"$(RELEASE_LIB_PATH)" -L"$(SERVER_RELEASE_LIB_PATH)" -L"$(OPENSSL_LIB_PATH)"
endif

LIBS = -lDBAgentLib -lSockServer -lSystem -lpthread -lmysqldb -lmysqlclient
LFLAGS += $(LIBPATHS)

Target_Name=lib$(Base_Name).a
Target=$(Target_Dir)/$(Target_Name)

Source=.

Copy_Target_Dir=$(INC_SERVER_ROOT)/CommonLogic
Copy_Target_Headers = $(patsubst %.h,$(Copy_Target_Dir)/%.h,$(shell find -P . -name "*.h"))

Objects =  $(patsubst %.cpp,$(Out_Dir)/%.o,$(shell find -P . -name "*.cpp"))
Objects += $(patsubst %.c,$(Out_Dir)/%.o,$(shell find -P . -name "*.c"))

all : $(Out_Dir) $(Target_Dir) $(Target)
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
	
$(Copy_Target_Dir)/%.h : $(Source)/%.h
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

