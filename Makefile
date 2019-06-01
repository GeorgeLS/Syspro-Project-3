CC := gcc
CFLAGS := -std=c99
LFLAGS := -lpthread

ALL_SRC := $(shell find -name "*.c")
ALL_OBJ := $(patsubst %.c, %.o, $(notdir $(ALL_SRC)))
SERVER_OBJ_FILTER := client_utils.o client.o
CLIENT_OBJ_FILTER := server_utils.o server.o
OBJ_SERVER := $(filter-out $(SERVER_OBJ_FILTER), $(ALL_OBJ))
OBJ_CLIENT := $(filter-out $(CLIENT_OBJ_FILTER), $(ALL_OBJ))

all: Server Client

all_objs: $(ALL_SRC)
	$(CC) -c $(ALL_SRC) $(LFLAGS)

Server: all_objs
	$(CC) $(OBJ_SERVER) -o Server $(LFLAGS)

Client: all_objs
	$(CC) $(OBJ_CLIENT) -o Client $(LFLAGS)

.PHONY: clean

clean:
	rm -rf $(ALL_OBJ) Server Client

#$(VERBOSE).SILENT:
