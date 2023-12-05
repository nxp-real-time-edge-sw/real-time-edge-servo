# SPDX-License-Identifier: (GPL-2.0 OR MIT)
# Copyright 2018-2020 NXP

src=$(wildcard ./*.c)
obj=$(patsubst ./%.c, ./%.o, $(src))
includedir = /usr/include/nservo
libdir = /usr/lib
execdir = /usr/bin
PKG_CONFIG ?= /usr/bin/pkg-config
HOME_DIR ?= /root

console_tool_src=$(wildcard ./tool/console_tool.c)
console_tool_obj=$(patsubst ./%.c, ./%.o, $(console_tool_src))

nservo_run_src=$(wildcard ./tool/nservo_run.c)
nservo_run_obj=$(patsubst ./%.c, ./%.o, $(nservo_run_src))

nservo_client_src=$(wildcard ./tool/nservo_client.c)
nservo_client_obj=$(patsubst ./%.c, ./%.o, $(nservo_client_src))

pv_example_src=$(wildcard ./example/Profile_Velocty_Mode_Test_2HSS458-EC.c)
pv_example_obj=$(patsubst ./%.c, ./%.o, $(pv_example_src))

pp_example_src=$(wildcard ./example/Profile_Position_Mode_Test_2HSS458-EC.c)
pp_example_obj=$(patsubst ./%.c, ./%.o, $(pp_example_src))

csp_example_src=$(wildcard ./example/Cyclic_Position_Mode_Test_2HSS458-EC.c)
csp_example_obj=$(patsubst ./%.c, ./%.o, $(csp_example_src))


ifdef $(XENO_CONFIG)
CFLAGS := $(shell DESTDIR=$(XENO_DESTDIR)   $(XENO_CONFIG) --skin=posix --cflags)
LDFLAGS := $(shell DESTDIR=${XENO_DESTDIR} $(XENO_CONFIG) --skin=posix --ldflags)
endif

CFLAGS +=  $(shell $(PKG_CONFIG) libxml-2.0 --cflags)
LDFLAGS += $(shell $(PKG_CONFIG) libxml-2.0 --libs)

CFLAGS += -I./include -Ddebug_level=debug_level_info -DEC_MASTER_IN_USERSPACE -DEC_BIND_CORE_MASK=0x02
LDFLAGS += -lpthread -lm
ECAT_LIB = ethercat_master

lib = libnservo.a

all:lib console_tool nservo_run pv_example pp_example csp_example nservo_client

lib: $(obj)
	$(AR) -cr $(lib) $^
console_tool: $(console_tool_obj) lib
	$(CC) $< -o nser_console_tool -L. -lnservo -l$(ECAT_LIB) -lpthread $(LDFLAGS)
pv_example: $(pv_example_obj) lib
	$(CC) $< -o pv_2hss458 -lnservo -L. -l$(ECAT_LIB) -lpthread $(LDFLAGS)
pp_example: $(pp_example_obj) lib
	$(CC) $< -o pp_2hss458 -lnservo -L. -l$(ECAT_LIB) -lpthread $(LDFLAGS)
csp_example: $(csp_example_obj) lib
	$(CC) $< -o csp_2hss458 -lnservo -L. -l$(ECAT_LIB) -lpthread $(LDFLAGS)
nservo_run:  $(nservo_run_obj) lib
	$(CC) $< -o nservo_run -lnservo -L. -l$(ECAT_LIB) -lpthread $(LDFLAGS)
nservo_client:  $(nservo_client_obj) lib
	$(CC) $< -o nservo_client $(LDFLAGS)  

install-libs:
	install -d -m 755 $(DESTDIR)$(includedir)
	install -m 644 include/* $(DESTDIR)$(includedir)
	install -d -m 755 $(DESTDIR)$(libdir)
	install -m 644 libnservo.a $(DESTDIR)$(libdir)

install:
	install -d -m 755 $(DESTDIR)$(execdir)
	install -d -m 755 $(DESTDIR)$(HOME_DIR)/nservo_example
	install -m 755 nservo_run $(DESTDIR)$(execdir)
	install -m 755 nservo_client $(DESTDIR)$(execdir)
	install -m 755 nser_console_tool  $(DESTDIR)$(execdir)
	install -m 664 example/hss248_ec_config_pp.xml  $(DESTDIR)$(HOME_DIR)/nservo_example
	install -m 664 example/hss248_ec_config_pv.xml  $(DESTDIR)$(HOME_DIR)/nservo_example
	install -m 664 example/x3e_config.xml  $(DESTDIR)$(HOME_DIR)/nservo_example
	install -m 664 example/Delta-ASDA-B3-two-masters.xml  $(DESTDIR)$(HOME_DIR)/nservo_example
	install -m 664 example/Delta-ASDA-B3-csp.xml  $(DESTDIR)$(HOME_DIR)/nservo_example
	install -m 664 example/Delta-ASDA-B3-pp.xml  $(DESTDIR)$(HOME_DIR)/nservo_example
	install -m 664 example/Delta-ASDA-B3-pv.xml  $(DESTDIR)$(HOME_DIR)/nservo_example
	install -m 664 example/Delta-ASDA-B3-tp_arrays  $(DESTDIR)$(HOME_DIR)/nservo_example
	install -m 664 example/x6b_sv680_delta_axis.xml $(DESTDIR)$(HOME_DIR)/nservo_example
	install -m 664 example/x6b_sv680_delta_tp_arrays $(DESTDIR)$(HOME_DIR)/nservo_example
	install -m 664 example/x3e_csp_60_config.xml $(DESTDIR)$(HOME_DIR)/nservo_example
clean:
	rm *.o $(lib)
%.o:%.c
	$(CC)	-c $(CFLAGS)  -Wall -g   $< -o $@
#-Wfatal-errors -Werror $(CC)  -shared $(LDFLAGS) -o libnservo.so $< $(CC)  -fpic  -shared $(LDFLAGS) -o libnservo.so $<
