BUILD_DIR=build

all:
	mingw64-cmake -DBUILD_SHARED_LIBS=OFF -B $(BUILD_DIR)
	cd $(BUILD_DIR) && make

bear: $(BUILD_DIR)
	bear --output $(BUILD_DIR)/compile_commands.json -- make

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
