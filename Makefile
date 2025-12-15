.SUFFIXES:
.INTERMEDIATE:
.SECONDARY:
.PHONY: all clean

MAKEFILE_PATH := $(patsubst %/,%,$(dir $(abspath $(firstword $(MAKEFILE_LIST)))))

DEBUG = -g

CSTD += -std=gnu11
CXXSTD += -std=gnu++14

CFLAGS += -Wall -Werror -Wfatal-errors
CFLAGS += -Wno-error=unused-function
CFLAGS += -Wno-error=unused-const-variable
#CFLAGS += -Wno-error=unused-but-set-variable
CFLAGS += -Wno-error=unused-variable
CFLAGS += -I$(MAKEFILE_PATH)/include
CFLAGS += -I$(dir $(GLFW))../include
CXXFLAGS += -fno-exceptions
#CFLAGS += -DDEBUG_BUTTONS
#CFLAGS += -DDEBUG_AXES
LDFLAGS += -nostdlib++ -lm
ifeq ($(shell uname),Linux)
LDFLAGS += -static-libgcc
endif
ifeq ($(OS),Windows_NT)
LDFLAGS += -Wl,--subsystem,windows -mwindows
endif
ifeq ($(shell uname),Darwin)
LDFLAGS += -framework Foundation -framework IOKit -framework AppKit
endif

OBJCOPY ?= objcopy

ifndef GLFW
$(error GLFW undefined)
endif

ifdef I386
ARCH='-m32'
AFLAGS='--32'
endif

ifeq ($(shell uname),Linux)

LDFLAGS += -z noexecstack
ifdef I386
OBJARCH = -O elf32-i386 -B i386
else
OBJARCH = -O elf64-x86-64 -B i386:x86-64
endif

else ifeq ($(shell uname),Darwin)

OBJARCH += -O mach-o-x86-64

else # assume Windows

ifdef I386
OBJARCH += -O pe-i386 -B i386
else
OBJARCH += -O pe-x86-64 -B i386:x86-64
endif

endif # ifeq ($(shell uname),Linux)

DEPFLAGS = -MMD -MP

OPT = -Og

all: main

#makefile_relative = $(shell realpath --relative-to $(makefile_path) $(1))
makefile_path := $(dir $(abspath $(firstword $(MAKEFILE_LIST))))
as_obj_binary = $(subst -,_,$(subst .,_,$(subst /,_,$(subst .h,,$(1)))))
as_obj_binary_p = _binary_$(call as_obj_binary,$(1))

define BUILD_BINARY_S
	@echo build_binary_h $@
	@echo '.global $(call as_obj_binary_p,$<)_start' > $@
	@echo '.global $(call as_obj_binary_p,$<)_end' >> $@
	@echo '$(call as_obj_binary_p,$<)_start:' >> $@
	@printf '\t.incbin "$<"\n' >> $@
	@echo '$(call as_obj_binary_p,$<)_end:' >> $@
endef

define BUILD_BINARY_H
	@echo build_binary_h $@
	@echo '#pragma once' > $@
	@echo '' >> $@
	@echo '#include <stdint.h>' >> $@
	@echo '' >> $@
	@echo '#ifdef __cplusplus' >> $@
	@echo 'extern "C" {' >> $@
	@echo '#endif' >> $@
	@echo '' >> $@
	@echo 'extern uint32_t $(call as_obj_binary_p,$<)_start __asm("$(call as_obj_binary_p,$<)_start");' >> $@
	@echo 'extern uint32_t $(call as_obj_binary_p,$<)_end __asm("$(call as_obj_binary_p,$<)_end");' >> $@
	@echo '' >> $@
	@echo '#define $(call as_obj_binary,$<)_start ((const char *)&$(call as_obj_binary_p,$<)_start)' >> $@
	@echo '#define $(call as_obj_binary,$<)_end ((const char *)&$(call as_obj_binary_p,$<)_end)' >> $@
	@echo '#define $(call as_obj_binary,$<)_size ($(call as_obj_binary,$<)_end - $(call as_obj_binary,$<)_start)' >> $@
	@echo '' >> $@
	@echo '#ifdef __cplusplus' >> $@
	@echo '}' >> $@
	@echo '#endif' >> $@
endef

%.glsl.s: %.glsl
	$(BUILD_BINARY_S)

%.data.s: %.data
	$(BUILD_BINARY_S)

%.data.pal.s: %.data.pal
	$(BUILD_BINARY_S)

include/%.glsl.h: src/%.glsl
	$(BUILD_BINARY_H)

include/%.data.h: src/%.data
	$(BUILD_BINARY_H)

include/%.data.pal.h: src/%.data.pal
	$(BUILD_BINARY_H)

clean:
	rm -f *.o *.d *.gch
	rm -f main

%.o: %.s
	$(AS) $(AFLAGS) $< -o $@ $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXSTD) $(ARCH) $(CFLAGS) $(OPT) $(DEBUG) $(DEPFLAGS) -MF ${<}.d -c $< -o $@ $(TARGET)

%.o: %.c
	$(CC) $(CSTD) $(ARCH) $(CFLAGS) $(CXXFLAGS) $(OPT) $(DEBUG) $(DEPFLAGS) -MF ${<}.d -c $< -o $@ $(TARGET)

MAIN_OBJS = \
	src/main.o \
	src/glad.o \
	src/opengl.o \
	src/render.o \
	src/collision2.o \
	src/update.o \
	src/unparse.o \
	$(patsubst %.glsl,%.glsl.o,$(wildcard src/shader/*.glsl)) \
	$(patsubst %.data,%.data.o,$(wildcard src/level/*.data)) \
	$(patsubst %.data,%.data.o,$(wildcard src/font/*.data)) \
	$(patsubst %.data,%.data.o,$(wildcard src/texture/*.data)) \
	$(patsubst %.data.pal,%.data.pal.o,$(wildcard src/level/*.data.pal)) \
	$(GLFW)

main: $(MAIN_OBJS)
	$(CXX) $^ $(CXXFLAGS) $(ARCH) -o $@ $(LDFLAGS) $(TARGET)

#-include $(shell find -type f -name 'src/*.d')

.SUFFIXES:
.INTERMEDIATE:
.SECONDARY:
.PHONY: all clean

%: RCS/%,v
%: RCS/%
%: %,v
%: s.%
%: SCCS/s.%
