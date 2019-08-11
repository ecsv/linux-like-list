#!/usr/bin/make -f
# SPDX-License-Identifier: MIT
# -*- makefile -*-
#
# Minimal Linux-like double-linked list helper functions test
#
# SPDX-FileCopyrightText: 2012-2016, Sven Eckelmann <sven@narfation.org>

TESTS_CXX_COMPATIBLE = \
 containerof \
 list_entry \
 list_init-explicit \
 list_init-local \
 list_init-global \
 list_add \
 list_add_tail \
 list_del \
 list_del_init \
 list_first_entry \
 list_last_entry \
 list_is_singular \
 list_for_each_safe \
 list_for_each_entry_safe \
 list_for_each_entry_safe_t \
 list_for_each \
 list_for_each_entry \
 list_for_each_entry_t \
 list_move \
 list_move_tail \
 list_splice \
 list_splice_tail \
 list_splice_init \
 list_splice_tail_init \
 list_splice-sort \
 list_cut_position \
 list_cut_position-sort-topdown \
 list_cut_position-sort-bottomup \
 list_add_before \
 list_add_behind \
 list_add_before-sort \
 hlist_entry \
 hlist_entry_safe \
 hlist_init-explicit \
 hlist_init-local \
 hlist_init-global \
 hlist_add_head \
 hlist_del \
 hlist_del_init \
 hlist_for_each_safe \
 hlist_for_each_entry_safe \
 hlist_for_each_entry_safe_t \
 hlist_for_each \
 hlist_for_each_entry \
 hlist_for_each_entry_t \
 hlist_move_list \
 hlist_add_before \
 hlist_add_behind \
 hlist_add_before-sort \
 hlist-prioqueue-pairing \
 hlist-prioqueue-fibonacci \

TESTS_C_ONLY = \

TESTS_ALL = $(TESTS_CXX_COMPATIBLE) $(TESTS_C_ONLY)

# tests flags and options
CFLAGS += -g3 -pedantic -Wall -W -Werror -MD -MP
ifeq ("$(BUILD_CXX)", "1")
	CFLAGS += -std=c++98
	TESTS = $(TESTS_CXX_COMPATIBLE)
	COMPILER_NAME=$(CXX)
else
	CFLAGS += -std=c99
	TESTS += $(TESTS_ALL)
	COMPILER_NAME=$(CC)
endif

# disable verbose output
ifneq ($(findstring $(MAKEFLAGS),s),s)
ifndef V
	Q_CC = @echo '    $(COMPILER_NAME)' $@;
	Q_LD = @echo '    $(COMPILER_NAME)' $@;
	export Q_CC
	export Q_LD
endif
endif

# standard build tools
CC ?= gcc
RM ?= rm -f
ifeq ("$(BUILD_CXX)", "1")
COMPILE.c = $(Q_CC)$(CXX) -x c++ $(CFLAGS) $(CXXLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
LINK.o = $(Q_LD)$(CXX) $(CFLAGS) $(CXXLAGS) $(LDFLAGS) $(TARGET_ARCH)
else
COMPILE.c = $(Q_CC)$(CC) -x c $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
LINK.o = $(Q_LD)$(CC) $(CFLAGS) $(LDFLAGS) $(TARGET_ARCH)
endif

TESTS_OK = $(TESTS:=.ok)

# default target
all: $(TESTS_OK)

$(TESTS_OK): %.ok: %
	@echo "T:  $(COMPILER_NAME) $(TESTRUN_NAME) $<"
	@$(TESTRUN_WRAPPER) ./$<
	@touch $@

# standard build rules
.SUFFIXES: .o .c
.c.o:
	$(COMPILE.c) -o $@ $<

$(TESTS): %: %.o
	$(LINK.o) $^ $(LDLIBS) -o $@

clean:
	@$(RM) $(TESTS_ALL) $(DEP) $(TESTS_OK) $(TESTS:=.o) $(TESTS:=.d)

# load dependencies
DEP = $(TESTS:=.d)
-include $(DEP)

.PHONY: all clean
