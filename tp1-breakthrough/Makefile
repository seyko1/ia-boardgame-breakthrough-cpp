CC=g++
CFLAGS=-std=c++11 -Wall -O2
BUILD_DIR=players
PROGNAME=player1

${PROGNAME}: $(BUILD_DIR) mybt.h ids_player.cpp
	$(CC) $(CFLAGS) ids_player.cpp -o $(BUILD_DIR)/${PROGNAME}

rand_player: $(BUILD_DIR) mybt.h rand_player.cpp
	$(CC) $(CFLAGS) rand_player.cpp -o $(BUILD_DIR)/rand_player

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)