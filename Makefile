# encoding: UTF-8

MCLIENT_DIR := mclient
VCLIENT_DIR := vclient
GATEWAY_DIR := gateway
DESTSTATION_DIR := deststation
BIN_DIR := bin

RM := rm -rf

CC := gcc
FLAGS := -O2 -Wall

all: mclient vclient gateway deststation

-include $(MCLIENT_DIR)/mclient.mk
-include $(VCLIENT_DIR)/vclient.mk
-include $(GATEWAY_DIR)/gateway.mk
-include $(DESTSTATION_DIR)/deststation.mk

mclient: $(MCLIENT_OBJS)
	$(CC) $(MCLIENT_OBJS) -o $(MCLIENT_BIN)

vclient: $(VCLIENT_OBJS)
	$(CC) $(VCLIENT_OBJS) -o $(VCLIENT_BIN)

gateway: $(GATEWAY_OBJS)
	$(CC) $(GATEWAY_OBJS) -o $(GATEWAY_BIN)

deststation: $(DESTSTATION_OBJS)
	$(CC) $(DESTSTATION_OBJS) -o $(DESTSTATION_BIN)

.PHONY: clean clean_gateway clean_vclient clean_gateway clean_deststation clean_all

clean: clean_all
clean_all: clean_mclient clean_vclient clean_gateway clean_deststation

clean_mclient:
	-$(RM) $(MCLIENT_OBJS) $(MCLIENT_DEPS) $(MCLIENT_BIN)
clean_vclient:
	-$(RM) $(VCLIENT_OBJS) $(VCLIENT_DEPS) $(VCLIENT_BIN)
clean_gateway:
	-$(RM) $(GATEWAY_OBJS) $(GATEWAY_DEPS) $(GATEWAY_BIN)
clean_deststation:
	-$(RM) $(DESTSTATION_OBJS) $(DESTSTATION_DEPS) $(DESTSTATION_BIN)