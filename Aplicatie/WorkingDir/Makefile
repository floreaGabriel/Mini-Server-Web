# Compiler
CXX = g++
CXXFLAGS = -Iinclude -fPIC
LDFLAGS = -Llib -lserver -pthread

# Directories
SRC_DIR = src
BIN_DIR = bin
LIB_DIR = lib

# Source files
SERVER_SOURCES = $(SRC_DIR)/GetHandler.cpp $(SRC_DIR)/PostHandler.cpp $(SRC_DIR)/PutHandler.cpp
MAIN_SOURCE = main.cpp
THREADPOOL_SOURCE = $(SRC_DIR)/threadpool.cpp
SERVER_SOURCE = $(SRC_DIR)/Server.cpp

# Output
LIB_OUTPUT = $(LIB_DIR)/libserver.so
EXECUTABLE = $(BIN_DIR)/mini_server

# Default target
all: $(LIB_OUTPUT) $(EXECUTABLE)

# Build shared library directly from source files
$(LIB_OUTPUT): $(SERVER_SOURCES)
	$(CXX) -shared $^ -o $@ $(CXXFLAGS)

# Build the executable directly from source files
$(EXECUTABLE): $(MAIN_SOURCE) $(THREADPOOL_SOURCE) $(SERVER_SOURCE) $(LIB_OUTPUT)
	$(CXX) $(MAIN_SOURCE) $(THREADPOOL_SOURCE) $(SERVER_SOURCE) $(CXXFLAGS) $(LDFLAGS) -o $@

# Clean up executables and libraries
clean:
	rm -f $(EXECUTABLE) $(LIB_OUTPUT)

# Export LD_LIBRARY_PATH
run: all
	export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$(shell pwd)/$(LIB_DIR) && ./$(EXECUTABLE)

.PHONY: all clean run
