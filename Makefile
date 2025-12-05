.SUFFIXES:
.INTERMEDIATE:
.SECONDARY:
.PHONY: all clean

MAKEFILE_PATH := $(patsubst %/,%,$(dir $(abspath $(firstword $(MAKEFILE_LIST)))))

DEBUG = -g

CSTD += -std=gnu23
CXXSTD += -std=gnu++23

CFLAGS += -Wall -Werror -Wfatal-errors
CFLAGS += -Wno-error=unused-function
CFLAGS += -Wno-error=unused-const-variable
CFLAGS += -Wno-error=unused-but-set-variable
CFLAGS += -Wno-error=unused-variable
CFLAGS += -I$(MAKEFILE_PATH)/include
LDFLAGS += -lm

ifndef GLFW
$(error GLFW undefined)
endif

ifeq ($(shell uname),Linux)
OBJARCH = -O elf64-x86-64
LDFLAGS += -z noexecstack
else
OBJARCH = -O pe-x86-64 -B i386:x86-64
endif

DEPFLAGS = -MMD -MP

OPT = -Og

all: main

define BUILD_BINARY_O
	objcopy \
		-I binary $(OBJARCH) \
		--rename-section .data=.data.$(basename $@) \
		$< $@
endef

makefile_path := $(dir $(abspath $(firstword $(MAKEFILE_LIST))))
makefile_relative = $(shell realpath --relative-to $(makefile_path) $(1))
as_obj_binary = $(subst -,_,$(subst .,_,$(subst /,_,$(subst .h,,$(call makefile_relative,$(1))))))
as_obj_binary_p = _binary_$(call as_obj_binary,$(1))

define BUILD_BINARY_H
	@echo gen $(call makefile_relative,$@)
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
	@echo 'extern uint32_t $(call as_obj_binary_p,$<)_size __asm("$(call as_obj_binary_p,$<)_size");' >> $@
	@echo '' >> $@
	@echo '#define $(call as_obj_binary,$<)_start ((const char *)&$(call as_obj_binary_p,$<)_start)' >> $@
	@echo '#define $(call as_obj_binary,$<)_end ((const char *)&$(call as_obj_binary_p,$<)_end)' >> $@
	@echo '#define $(call as_obj_binary,$<)_size ($(call as_obj_binary,$<)_end - $(call as_obj_binary,$<)_start)' >> $@
	@echo '' >> $@
	@echo '#ifdef __cplusplus' >> $@
	@echo '}' >> $@
	@echo '#endif' >> $@
endef

%.glsl.o: %.glsl
	$(BUILD_BINARY_O)

include/%.glsl.h: src/%.glsl
	$(BUILD_BINARY_H)

%.data.o: %.data
	$(BUILD_BINARY_O)

include/%.data.h: src/%.data
	$(BUILD_BINARY_H)

%.data.pal.o: %.data.pal
	$(BUILD_BINARY_O)

include/%.data.pal.h: src/%.data.pal
	$(BUILD_BINARY_H)

clean:
	rm -f *.o *.d *.gch
	rm -f main

%.o: %.cpp
	$(CXX) $(CXXSTD) $(CFLAGS) $(OPT) $(DEBUG) $(DEPFLAGS) -MF ${<}.d -c $< -o $@

%.o: %.c
	$(CC) $(CSTD) $(CFLAGS) $(OPT) $(DEBUG) $(DEPFLAGS) -MF ${<}.d -c $< -o $@

MAIN_OBJS = \
	src/main.o \
	src/glad.o \
	src/opengl.o \
	src/render.o \
	src/collision.o \
	src/update.o \
	src/unparse.o \
	$(patsubst %.glsl,%.glsl.o,$(wildcard src/shader/*.glsl)) \
	$(patsubst %.data,%.data.o,$(wildcard src/level/*.data)) \
	$(patsubst %.data,%.data.o,$(wildcard src/font/*.data)) \
	$(patsubst %.data.pal,%.data.pal.o,$(wildcard src/level/*.data.pal)) \
	$(GLFW)

main: $(MAIN_OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

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
