# encoding: UTF-8
# shell is 'bash' or 'sh'
CCOMP ?= gcc
CCOMP_FLAGS ?= -std=c11 -O2 -Wall
RM ?= rm -rf

# directories
OBJ_DIR := .obj
DEP_DIR := .dep
SRC_DIR := src
BIN_DIR := bin

# bin path and name
BIN_NAME := deststation
BIN_PATH := $(BIN_DIR)/$(BIN_NAME)

# includes
# INCS :=

# definitions
# DEFS := $(addprefix -D, $(DEFS))

# source files
SRCS := $(wildcard $(SRC_DIR)/*.c)

# # object files
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# dependencies files
DEPS := $(patsubst $(SRC_DIR)/%.c, $(DEP_DIR)/%.d, $(SRCS))

$(BIN_NAME): $(BIN_PATH)

$(BIN_PATH): $(OBJS)
	$(CCOMP) $(OBJS) -o $@

clean:
	$(RM) $(OBJS) $(DEPS) $(BIN_PATH)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CCOMP) $(CCOMP_FLAGS) -c $< $(INCS) -MMD -MF $(DEP_DIR)/$*.d -o $@

.PHONY: $(BIN_NAME) $(BIN_PATH) clean

-include $(DEPS)
