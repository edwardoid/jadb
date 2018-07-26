SRC_DIR := $(PWD)/src/
OBJ_DIR := $(PWD)/obj/
BIN_DIR := $(PWD)/bin/
DEP_DIR := $(PWD)/dep/
LIB_DIR := $(PWD)/lib/
INC_DIR := -I$(PWD)/src -I/usr/include -I./3dp -I$(PWD)/3dp/nlohmann/include 
CC:= g++-7 -std=c++17 -g -w
SRC_FILES:=$(wildcard $(SRC_DIR)*.cpp)
OBJ_FILES:=$(patsubst $(SRC_DIR), $(OBJ_DIR),$(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)%.o,$(SRC_FILES)))
DEFILES:=$(patsubst $(SRC_DIR), $(DEP_DIR),$(patsubst $(SRC_DIR)%.cpp,$(DEP_DIR)%.o,$(SRC_FILES)))
LINK_LIBS:= -lboost_system -lboost_filesystem -lboost_program_options -lboost_date_time -lpthread
BINARY_TARGET := jadb
.PHONY: all
all: binary

.PHONY: binary
binary: $(OBJ_FILES)
	@ echo "Building $(BINARY_TARGET)"
	@ $(CC) $(OBJ_FILES) $(LINK_LIBS) -o $(BIN_DIR)$(BINARY_TARGET)

.PHONY: run
run: binary
	$(BIN_DIR)$(BINARY_TARGET) --config=$(PWD)/config.json
$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@ echo "Compiling $@"
	@ $(CC) -c $< -I$(INC_DIR) -o $@
$(DEP_DIR)%.dep: $(SRC_DIR)%.cpp
	@ $(CC) -MM $< -MT "$@ $(patsubst $(DEP_DIR)%.dep,$(OBJ_DIR)%o,$@)" $(INC_DIR) -o $@

-include $(DEP_FILES)

.PHONY: prepare
prepare:
	@ echo "Creating $(DEP_DIR)"
	@ mkdir -p $(DEP_DIR)
	@ echo "Creating $(OBJ_DIR)"
	@ mkdir -p $(OBJ_DIR)
	@ echo "Creating $(BIN_DIR)"
	@ mkdir -p $(BIN_DIR)
	@ echo "Creating $(LIB_DIR)"
	@ mkdir -p $(LIB_DIR)

.PHONY: clean
clean:
	@ echo "Cleaning..."
	@ rm -rf $(BIN_DIR)*
	@ rm -rf $(LIB_DIR)*
	@ rm -rf $(OBJ_DIR)*
	@ rm -rf $(DEP_DIR)*
