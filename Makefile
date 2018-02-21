SRC_DIR := src/
OBJ_DIR := obj/
BIN_DIR := bin/
DEP_DIR := dep/
LIB_DIR := lib/
INC_DIR := ./src -I/usr/include -I./3dp 
CC:= g++-7 -std=c++14 -g
SRC_FILES:=$(wildcard $(SRC_DIR)*.cpp)
OBJ_FILES:=$(patsubst $(SRC_DIR), $(OBJ_DIR),$(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)%.o,$(SRC_FILES)))
DEFILES:=$(patsubst $(SRC_DIR), $(DEP_DIR),$(patsubst $(SRC_DIR)%.cpp,$(DEP_DIR)%.o,$(SRC_FILES)))
LINK_LIBS:= -lboost_system -lboost_filesystem -lpthread
BINARY_TARGET := jadb
.PHONY: all
all: binary

.PHONY: binary
binary: $(OBJ_FILES)
	$(CC) $(OBJ_FILES) $(LINK_LIBS) -o $(BIN_DIR)$(BINARY_TARGET)

.PHONY: run
run: binary
	./$(BIN_DIR)$(BINARY_TARGET)
$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	$(CC) -c $< -I$(INC_DIR) -o $@
$(DEP_DIR)%.dep: $(SRC_DIR)%.cpp
	$(CC) -MM $< -MT "$@ $(patsubst $(DEP_DIR)%.dep,$(OBJ_DIR)%o,$@)" -I$(INC_DIR) -o $@

-include $(DEP_FILES)

.PHONY: prepare
prepare:
	mkdir -p $(DEP_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)
	mkdir -p $(LIB_DIR)

.PHONY: clean
clean:
	rm -rf $(BIN_DIR)*
	rm -rf $(LIB_DIR)*
	rm -rf $(OBJ_DIR)*
	rm -rf $(DEP_DIR)*