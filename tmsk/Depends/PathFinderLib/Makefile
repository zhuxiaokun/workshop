AURORALIB_DIR = ..
include $(AURORALIB_DIR)/Makefile.inc

Base_Name=PathFinderLib
Out_Dir = $(TMP_PATH)
Target_Dir = $(AURORA_DEBUG_LIB_PATH)

DEFINES += -DSERVER -D_SERVER -D_LIB

INCLUDES += -I".." \
	    -I"../Inc" \
	    $(NULL)

LIBS = -lSystem
LFLAGS += $(LIBPATHS)

Target_Name=lib$(Base_Name).a
Target=$(Target_Dir)/$(Target_Name)

Source=.

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

#$(Copy_Target_Dir)/%.h : %.h
#	@if [ -d $(shell dirname $@) ] ; \
#	then \
#		echo "copy $< to $@"; \
#	else \
#		echo "copy $< to $@"; \
#		$(MKDIR) -p $(shell dirname $@); \
#	fi
#	@$(CP) -f $< $@

#$(Copy_Target_Dir)/%.hh : %.hh
#	@if [ -d $(shell dirname $@) ] ; \
#	then \
#		echo "copy $< to $@"; \
#	else \
#		echo "copy $< to $@"; \
#		$(MKDIR) -p $(shell dirname $@); \
#	fi
#	@$(CP) -f $< $@

clean :
	$(RM) -f $(Target)
	$(RM) -f $(Objects)
