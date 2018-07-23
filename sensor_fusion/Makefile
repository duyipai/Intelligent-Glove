# Makefile for the paho-mqttpp (C++) sample applications

ifdef DEVELOP
  PAHO_C_DIR ?= $(abspath ../../../paho.mqtt.c)
  PAHO_C_LIB_DIR ?= $(PAHO_C_DIR)/build/output
  PAHO_C_INC_DIR ?= $(PAHO_C_DIR)/src
else
  PAHO_C_LIB_DIR ?= /usr/local/lib
  PAHO_C_INC_DIR ?= /usr/local/include
endif

TGTS  = subscribe

all: $(TGTS)

ifneq ($(CROSS_COMPILE),)
  CC  = $(CROSS_COMPILE)gcc
  CXX = $(CROSS_COMPILE)g++
  AR  = $(CROSS_COMPILE)ar
  LD  = $(CROSS_COMPILE)ld
endif

CXXFLAGS += -Wall -std=c++11
CPPFLAGS += -I.. -I$(PAHO_C_INC_DIR)

ifdef DEBUG
  CPPFLAGS += -DDEBUG
  CXXFLAGS += -g -O0
else
  CPPFLAGS += -D_NDEBUG
  CXXFLAGS += -O2
endif

LDLIBS += -L../../lib -L$(PAHO_C_LIB_DIR) -lpaho-mqttpp3 -lpaho-mqtt3a

subscribe: subscribe.cpp attitude_estimator.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ $(LDLIBS)
# Cleanup

.PHONY: clean distclean

clean:
	rm -f $(TGTS)

distclean: clean

