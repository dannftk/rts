# encoding: UTF-8
CCOMP := gcc
CCOMP_FLAGS := -std=c11 -O2 -Wall
RM := rm -rf
CP := cp

# directories
MCLIENT_DIR := mclient
VCLIENT_DIR := vclient
DESTSTATION_DIR := deststation
GATEWAY_DIR := gateway
BIN_DIR := bin

all: mclient vclient deststation gateway

mclient:
	$(MAKE) -C $(MCLIENT_DIR) CCOMP="$(CCOMP)" CCOMP_FLAGS="$(CCOMP_FLAGS)" $@
	$(CP) $@/bin/$@ $(BIN_DIR)/
vclient:
	$(MAKE) -C $(VCLIENT_DIR) CCOMP="$(CCOMP)" CCOMP_FLAGS="$(CCOMP_FLAGS)" $@
	$(CP) $@/bin/$@ $(BIN_DIR)/
deststation:
	$(MAKE) -C $(DESTSTATION_DIR) CCOMP="$(CCOMP)" CCOMP_FLAGS="$(CCOMP_FLAGS)" $@
	$(CP) $@/bin/$@ $(BIN_DIR)/
gateway:
	$(MAKE) -C $(GATEWAY_DIR) CCOMP="$(CCOMP)" CCOMP_FLAGS="$(CCOMP_FLAGS)" $@
	$(CP) $@/bin/$@ $(BIN_DIR)/
clean:
	$(MAKE) -C $(MCLIENT_DIR) $@
	$(MAKE) -C $(VCLIENT_DIR) $@
	$(MAKE) -C $(DESTSTATION_DIR) $@
	$(MAKE) -C $(GATEWAY_DIR) $@
	$(RM) $(BIN_DIR)/*

.PHONY: all mclient vclient deststation gateway clean
