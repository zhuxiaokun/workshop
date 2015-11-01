######################################
#PROJ_DIR ?= $(HOME)/tkv300
PROJ_DIR ?= $(shell pwd |cut -d / -f -4)

######################################
CVSROOT ?= $(PROJ_DIR)
AURORALIB_DIR ?= $(PROJ_DIR)/auroralib
include $(AURORALIB_DIR)/Makefile.inc

######################################
ATHENA_ROOT=$(CVSROOT)

######################################
INC_ROOT=$(ATHENA_ROOT)/Inc
INC_SERVER_ROOT=$(ATHENA_ROOT)/Server/Inc
INC_DBAGENTLIB=$(INC_SERVER_ROOT)/DBAgentLib
INC_LUA=$(INC_ROOT)/lua
INC_MYSQL=$(AURORALIB_DIR)/Inc/mysqldb
INC_PACKETGATE=$(INC_ROOT)/PacketGate
INC_SQUISH=$(INC_ROOT)/squish-1.10
INC_SHAREFILES=$(ATHENA_ROOT)/ShareFiles
INC_COMMONLOGIC=$(ATHENA_ROOT)/Server/CommonLogic
INC_AILOGIC=$(ATHENA_ROOT)/Server/AILogic
INC_GAMELOGIC=$(ATHENA_ROOT)/Server/GameLogic
INC_BILLINGDBAGENT=$(INC_SERVER_ROOT)/BillingDBAgent
######################################
LIB_ROOT=$(ATHENA_ROOT)/Lib$(PlatformNum)
LIB_SERVER_ROOT=$(ATHENA_ROOT)/Server/Lib$(PlatformNum)
DEBUG_LIB_PATH=$(LIB_ROOT)/Debug
RELEASE_LIB_PATH=$(LIB_ROOT)/Release
SERVER_DEBUG_LIB_PATH=$(LIB_SERVER_ROOT)/Debug
SERVER_RELEASE_LIB_PATH=$(LIB_SERVER_ROOT)/Release

#######################################
DBAGENT_LIB_NAME=DBAgent
LUA_LIB_NAME=Lua
MYSQLCLIENT_LIB_NAME=mysqlclient

#####################################################
SERVER_TMP_PATH=./$(ConfigurationName)$(PlatformName)
SERVER_DISTRIBUTE_PATH=$(ATHENA_ROOT)/Server/$(ConfigurationName)$(PlatformName)

#####################################################
TMP_PATH=./$(ConfigurationName)$(PlatformNum)
DISTRIBUTE_PATH=$(ATHENA_ROOT)/Server/Bin/$(ConfigurationName)$(PlatformName)

#######################################
ATHENA_LIB_PATH=$(ATHENA_ROOT)/Lib$(PlatformNum)/$(ConfigurationName)
SERVER_LIB_PATH=$(ATHENA_ROOT)/Server/Lib$(PlatformNum)/$(ConfigurationName)
SECURITY_LIB_PATH=$(CVSROOT)/Security/Lib$(PlatformNum)/$(ConfigurationName)

#####################################################
DEFINES += -DLUA_USE_MKSTEMP 
INCLUDES += -I"$(AURORALIB_DIR)" -I"$(AURORALIB_DIR)/Inc"
LIBPATHS += -L"$(AURORA_LIB_PATH)" -L"$(ATHENA_LIB_PATH)" -L"$(SERVER_LIB_PATH)"
