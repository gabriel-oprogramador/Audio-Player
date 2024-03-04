CC = clang
INCLUDES = -ISource/Engine/Include -ISource/Engine/Platform/ThirdParty
CDEFINES = -D_CRT_SECURE_NO_WARNINGS -D"GAME_NAME=$(notdir $(CURDIR))"
CFLAGS =
LIBS =

# Generating clang files
ifeq ($(CC), clang)
CL_CCJ   = @sed -e '1s/^/[\n''/' -e '$$s/,$$/\n'']/' $(OBJ_JSON) > compile_commands.json
CL_JSON = -MJ $@.json
endif

# Set Platform
PLATFORM = $(shell uname)
ifeq ($(PLATFORM), Windows_NT)
EXTENSION =  .exe
CDEFINES += -DPLATFORM_WINDOWS
CFLAGS += -std=c99
LIBS +=

else ifeq ($(PLATFORM), Linux)
EXTENSION =
CDEFINES += -DPLATFORM_LINUX
CFLAGS += -std=gnu99
LIBS += -lm -pthread
endif

# Set Compile Mode
config = debug
ifeq ($(config), debug)
COMPILE_MODE = Debug/
CFLAGS += -g -Wall
CDEFINES += -DDEBUG_MODE
CDEFINES += -D"CONTENT_PATH=$(abspath Content/)/"
CDEFINES += -D"CONFIG_PATH=$(abspath Config/)/"
else
COMPILE_MODE = Release/
CFLAGS += -O3
CDEFINES += -DRELEASE_MODE
CDEFINES += -D"CONTENT_PATH=Data/Content/"
CDEFINES += -D"CONFIG_PATH=Data/Config/"

# Copy files in release mode
CREATE_DATA = @mkdir -p $(BUILD_DIR)/$(COMPILE_MODE)/Data
COPY_FILES  = @cp -f -r "$(abspath Content/)" "$(BUILD_DIR)/$(COMPILE_MODE)/Data"
COPY_CONFIG = @cp -f -r "$(abspath Config/)" "$(BUILD_DIR)/$(COMPILE_MODE)/Data"
endif

# Set Auto Config
TARGET = $(notdir $(CURDIR))
BUILD_DIR = Build/
OBJ_DIR = $(BUILD_DIR)Obj/$(COMPILE_MODE)
SRC_DIR = Source/
INCLUDE_DIR = $(SRC_DIR)/Engine/Include
INCLUDE_TP_DIR = $(SRC_DIR)/Engine/Platform/ThirdParty
CONTENT_DIR = Content/
CONFIG_DIR = Config/
SRC = $(wildcard $(SRC_DIR)*.c) $(wildcard $(SRC_DIR)*/*.c) $(wildcard $(SRC_DIR)*/*/*.c) $(wildcard $(SRC_DIR)*/*/*/*.c)
OBJ = $(addprefix $(OBJ_DIR), $(notdir $(SRC:.c=.o)))
OBJ_JSON = $(addprefix $(OBJ_DIR), $(notdir $(OBJ:.o=.o.json)))

all: $(TARGET)

install:
	@echo "Stating Compiling => Platform:$(PLATFORM), Compiler:$(CC), Mode:$(config)"
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)$(COMPILE_MODE)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(SRC_DIR)
	@mkdir -p $(INCLUDE_DIR)
	@mkdir -p $(CONTENT_DIR)
	@mkdir -p $(CONFIG_DIR)

clean:
	@echo "Cleaning Everything..."
	@rm -f -r $(BUILD_DIR)
	@rm -f -r .cache
	@rm -f -r .vscode
	@rm -f compile_commands.json

run: $(TARGET)
	@echo "Running => $(BUILD_DIR)$(COMPILE_MODE)$(TARGET)$(EXTENSION)"
	@$(BUILD_DIR)$(COMPILE_MODE)$(TARGET)$(EXTENSION)

rebuild: clean run

$(TARGET): install $(OBJ)
	@echo "Linking => $(BUILD_DIR)$(COMPILE_MODE)$(TARGET)$(EXTENSION)"
	@$(CC) $(OBJ) $(INCLUDES) $(CFLAGS) $(CDEFINES) $(LIBS) -o $(BUILD_DIR)$(COMPILE_MODE)$(TARGET)$(EXTENSION)
	@$(CL_CCJ)
	@$(CREATE_DATA)
	@$(COPY_CONFIG)
	@$(COPY_FILES)

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	@echo "Compiling:$<..."
	@$(CC) $(CL_JSON) $< $(INCLUDES) $(CFLAGS) $(CDEFINES) -c -o $@

$(OBJ_DIR)%.o: $(SRC_DIR)*/%.c
	@echo "Compiling:$<..."
	@$(CC) $(CL_JSON) $< $(INCLUDES) $(CFLAGS) $(CDEFINES) -c -o $@

$(OBJ_DIR)%.o: $(SRC_DIR)*/*/%.c
	@echo "Compiling:$<..."
	@$(CC) $(CL_JSON) $< $(INCLUDES) $(CFLAGS) $(CDEFINES) -c -o $@

$(OBJ_DIR)%.o: $(SRC_DIR)*/*/*/%.c
	@echo "Compiling:$<..."
	@$(CC) $(CL_JSON) $< $(INCLUDES) $(CFLAGS) $(CDEFINES) -c -o $@
