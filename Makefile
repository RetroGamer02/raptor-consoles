XBE_TITLE = nxdk\ sample\ -\ Raptor-XBOX
GEN_XISO = $(XBE_TITLE).iso
SRCS = $(wildcard $(CURDIR)/src/*.cpp) $(wildcard $(CURDIR)/src/*.c) $(wildcard $(CURDIR)/include/textscreen/include/*.cpp) $(wildcard $(CURDIR)/include/textscreen/include/*.c) $(wildcard $(CURDIR)/include/TinySoundFont/*.cpp) $(wildcard $(CURDIR)/include/TinySoundFont/*.c)
NXDK_DIR ?= $(CURDIR)/../..
NXDK_SDL = y

include $(NXDK_DIR)/Makefile