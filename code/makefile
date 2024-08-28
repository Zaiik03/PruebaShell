.PHONY: clean

TARGET_EXEC = run

SOURCE_DIR = .
OUTPUT_DIR = ../exec

C++ = g++ -std=c++20

COMPILER = $(C++)

RMDIR = rm --recursive
MKDIR = mkdir --parents

$(OUTPUT_DIR)/$(TARGET_EXEC): $(SOURCE_DIR)/shell.cpp
	$(COMPILER) -o $@ $(SOURCE_DIR)/shell.cpp

clean:
	$(RMDIR) $(OUTPUT_DIR)