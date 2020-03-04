#Set this to @ to keep the makefile quiet
SILENCE = @

#CCPUTEST Hardcode
CPPUTEST_HOME=/home/marko/Software/cpputest
#---- Outputs ----#
COMPONENT_NAME = Robot_CppUTest
TARGET_LIB = \
	lib/lib$(COMPONENT_NAME).a
	
TEST_TARGET = \
	$(COMPONENT_NAME)_tests

#--- Inputs ----#
PROJECT_HOME_DIR = .
CPP_PLATFORM = Gcc

SRC_DIRS = \
	$(PROJECT_HOME_DIR)/Src\
	$(PROJECT_HOME_DIR)/Drivers/STM32F4xx_HAL_Driver/Src\
	$(PROJECT_HOME_DIR)/VL53L0X_1.0.2/Api/core/src\
 	$(PROJECT_HOME_DIR)/VL53L0X_1.0.2/Api/platform/src\
	
	


TEST_SRC_DIRS = \
	tests\
	tests/*\

INCLUDE_DIRS =\
  .\
  $(CPPUTEST_HOME)/include\
  $(PROJECT_HOME_DIR)/Inc\
  $(PROJECT_HOME_DIR)/Drivers/CMSIS/Include\
  $(PROJECT_HOME_DIR)/Drivers/STM32F4xx_HAL_Driver/Inc\
  $(PROJECT_HOME_DIR)/Drivers/CMSIS/Device/ST/STM32F4xx/Include\
  $(PROJECT_HOME_DIR)/VL53L0X_1.0.2/Api/core/inc\
  $(PROJECT_HOME_DIR)/VL53L0X_1.0.2/Api/platform/inc\
  
  

  
#CPPUTEST_WARNINGFLAGS += -pedantic-errors -Wconversion -Wshadow  -Wextra
CPPUTEST_WARNINGFLAGS += -Wall -Wswitch-default -Wswitch-enum 


include $(CPPUTEST_HOME)/build/MakefileWorker.mk
