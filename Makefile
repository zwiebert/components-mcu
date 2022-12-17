.PHONY: clean test rebuild  print-help

mcus := esp32
tgts := all

default: print-help

clean : esp32-test-clean

print-help:
	@cat docs/make_help.txt

########### Unit Testing ###############
esp32_test_tgts_auto := build clean flash run all all-ocd flash-ocd flash-app-ocd

define GEN_RULE
.PHONY: esp32-$(1)
esp32-test-$(1):
	make -C test/esp32 $(1)
endef
$(foreach tgt,$(esp32_test_tgts_auto),$(eval $(call GEN_RULE,$(tgt))))


############# TCP Terminal ##############
IPADDR ?= 192.168.1.65

tcpterm:
	socat -d -d -v pty,link=/tmp/ttyVirtual0,raw,echo=0,unlink-close,waitslave tcp:$(IPADDR):7777,forever,reuseaddr&
	gtkterm -p /tmp/ttyVirtual0 -c tcpterm
