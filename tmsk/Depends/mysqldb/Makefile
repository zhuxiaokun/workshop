AURORALIB_DIR = ..
include $(AURORALIB_DIR)/Makefile.inc

MODE ?= debug

Base_Name=mysqldb$(LIBNAME_DEC)
Out_Dir = $(TMP_PATH)
Target_Dir = $(AURORA_LIB_PATH)

INCLUDES += -I".." -I"../Inc"

LIBS = -lpthread -l$(SYSTEM_LIB)
LFLAGS += $(LIBPATHS)

Target_Name=$(Base_Name)
Target=$(Target_Dir)/lib$(Target_Name).a

Source=.

Copy_Target_Dir=../Inc/mysqldb
Copy_Target_Headers = $(patsubst %.h,$(Copy_Target_Dir)/%.h,$(shell ls *.h))

Objects =  $(patsubst %.cpp,$(Out_Dir)/%.o,$(shell find -P . -name "*.cpp"))
Objects += $(patsubst %.c,$(Out_Dir)/%.o,$(shell find -P . -name "*.c"))

all : $(Out_Dir) $(Target_Dir) $(Copy_Target_Headers) $(Target)
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
