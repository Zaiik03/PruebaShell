.PHONY: clean

TARGET_EXEC = run

SOURCE_DIR = .
OUTPUT_DIR = ../exec

C++ = g++ -std=c++20

COMPILER = $(C++)

RMDIR = rm --recursive
MKDIR = mkdir --parents

$(OUTPUT_DIR)/$(TARGET_EXEC): $(SOURCE_DIR)/shell.cpp $(SOURCE_DIR)/favs.cpp
	$(MKDIR) $(dir $@)
	
	$(COMPILER) -o $@ $(SOURCE_DIR)/shell.cpp
	$(COMPILER) -o $(OUTPUT_DIR)/favs $(SOURCE_DIR)/favs.cpp

clean:
	$(RMDIR) $(OUTPUT_DIR)