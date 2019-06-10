CC := gcc
CFLAGS := -std=c99
LFLAGS := -lpthread

ALL_SRC := $(shell find -name "*.c")
ALL_OBJ := $(patsubst %.c, %.o, $(notdir $(ALL_SRC)))
SERVER_OBJ_FILTER := client_utils.o client.o
CLIENT_OBJ_FILTER := server_utils.o server.o
OBJ_SERVER := $(filter-out $(SERVER_OBJ_FILTER), $(ALL_OBJ))
OBJ_CLIENT := $(filter-out $(CLIENT_OBJ_FILTER), $(ALL_OBJ))

all: dropbox_server dropbox_client

all_objs: $(ALL_SRC)
	$(CC) -c $(ALL_SRC) $(LFLAGS)

dropbox_server: all_objs
	$(CC) $(OBJ_SERVER) -o $@ $(LFLAGS)

dropbox_client: all_objs
	$(CC) $(OBJ_CLIENT) -o $@ $(LFLAGS)

.PHONY: clean

clean:
	rm -rf $(ALL_OBJ) dropbox_server dropbox_client

#$(VERBOSE).SILENT:
