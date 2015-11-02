AURORALIB_DIR = ..
include $(AURORALIB_DIR)/Makefile.inc

Base_Name=System$(LIBNAME_DEC)
Out_Dir = $(TMP_PATH)
Target_Dir = $(AURORA_LIB_PATH)

Target_Name=lib$(Base_Name).a
Target=$(Target_Dir)/$(Target_Name)

Source=.
INCLUDES += -I"../3rd_lib/openssl 1.0.0/include"

Objects =  $(patsubst %.cpp,$(Out_Dir)/%.o,$(shell find -P . -name "*.cpp"))
Objects += $(patsubst %.c,$(Out_Dir)/%.o,$(shell find -P . -name "*.c"))
Copy_Target_Dir=../Inc/System
Copy_Target_Headers = $(patsubst %.h,$(Copy_Target_Dir)/%.h,$(shell find -P . -name "*.h"))

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

$(Copy_Target_Dir)/%.h : %.h
	@if [ -d $(shell dirname $@) ] ; \
	then \
		echo "copy $< to $@"; \
	else \
		echo "copy $< to $@"; \
		$(MKDIR) -p $(shell dirname $@); \
	fi
	@$(CP) -f $< $@

$(Copy_Target_Dir)/%.hh : %.hh
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
