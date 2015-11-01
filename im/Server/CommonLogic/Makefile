ATHENA_ROOT = ..
include $(ATHENA_ROOT)/Makefile.inc

MODE ?= debug

Base_Name=CommonLogic
Out_Dir = $(TMP_PATH)
Target_Dir = $(ATHENA_LIB_PATH)

Target_Name=lib$(Base_Name).a
Target=$(Target_Dir)/$(Target_Name)

Source=.
INCLUDES += -I"." -I"$(ATHENA_ROOT)/Inc" -I"../Inc"

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
