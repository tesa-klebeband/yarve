CPP = g++
LINKER =
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
SRC_DIR = src
EXEC_BIN = yarve

SRCS := $(wildcard $(SRC_DIR)/*.cpp)

OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CPP) -Ofast -c -o $@ $<

all: prepare $(EXEC_BIN)

prepare:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BUILD_DIR)

$(EXEC_BIN): $(OBJS)
	$(CPP) -Ofast -o $(BUILD_DIR)/$@ $^ $(LINKER)

.PHONY: linux

linux:
	@cd linux && make
	@cd ..

run-linux:
	$(BUILD_DIR)/$(EXEC_BIN) -k linux/build/Image -d linux/build/yarve.dtb

clean:
	@rm -rf $(BUILD_DIR)