UNAME=$(shell uname)

OPENGL_NAME=ScopGL
VULKAN_NAME=ScopVk
SRC_DIR=Source
SRC_FILES=main.cpp math.cpp obj_file.cpp mesh.cpp
OPENGL_SRC_FILES=opengl_backend.cpp
VULKAN_SRC_FILES=vulkan_backend.cpp

OPENGL_OBJ_DIR=Obj/OpenGL
VULKAN_OBJ_DIR=Obj/Vulkan
OBJ_FILES=$(SRC_FILES:.cpp=.o)
OPENGL_OBJ_FILES=$(OPENGL_SRC_FILES:.cpp=.o) glad.o
VULKAN_OBJ_FILES=$(VULKAN_SRC_FILES:.cpp=.o)
INCLUDE_DIRS=Source Third_Party/glfw-3.4/include Third_Party/glad/include
OPENGL_DEFINES=SCOP_BACKEND_OPENGL
VULKAN_DEFINES=SCOP_BACKEND_VULKAN

ifeq ($(UNAME), Linux)

LIB_DIRS=

else ifeq ($(UNAME), Darwin)

LIB_DIRS=Third_Party/glfw-3.4/lib-macos-universal
OPENGL_FRAMEWORKS=CoreVideo OpenGL IOKit Cocoa Carbon
VULKAN_FRAMEWORKS=CoreVideo IOKit Cocoa Carbon

endif

LIBS=glfw

CC=clang
C_FLAGS=$(addprefix -I, $(INCLUDE_DIRS))

CPP=c++
CPP_FLAGS=$(addprefix -I, $(INCLUDE_DIRS)) -std=c++11 -v

all: $(OPENGL_NAME)

$(OPENGL_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CPP) $(addprefix -D, $(OPENGL_DEFINES)) $(CPP_FLAGS) -c $< -o $@

$(VULKAN_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CPP) $(addprefix -D, $(VULKAN_DEFINES)) $(CPP_FLAGS) -c $< -o $@

$(OPENGL_OBJ_DIR)/glad.o: Third_Party/glad/src/glad.c
	$(CC) $(C_FLAGS) -c $< -o $@

$(OPENGL_NAME): $(addprefix $(OPENGL_OBJ_DIR)/, $(OBJ_FILES)) $(addprefix $(OPENGL_OBJ_DIR)/, $(OPENGL_OBJ_FILES))
	$(CPP) $(CPP_FLAGS) $(addprefix $(OPENGL_OBJ_DIR)/, $(OBJ_FILES)) $(addprefix $(OPENGL_OBJ_DIR)/, $(OPENGL_OBJ_FILES)) $(addprefix -L, $(LIB_DIRS)) $(addprefix -l, $(LIBS)) $(addprefix -framework , $(OPENGL_FRAMEWORKS)) -o $(OPENGL_NAME)

$(VULKAN_NAME): $(addprefix $(VULKAN_OBJ_DIR)/, $(OBJ_FILES)) $(addprefix $(VULKAN_OBJ_DIR)/, $(VULKAN_OBJ_FILES))
	$(CPP) $(CPP_FLAGS) $(addprefix $(VULKAN_OBJ_DIR)/, $(OBJ_FILES)) $(addprefix $(VULKAN_OBJ_DIR)/, $(VULKAN_OBJ_FILES)) $(addprefix -L, $(LIB_DIRS)) $(addprefix -l, $(LIBS)) $(addprefix -framework , $(VULKAN_FRAMEWORKS)) -o $(VULKAN_NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(OPENGL_NAME)
	rm -f $(VULKAN_NAME)

re: fclean all

.PHONY: all clean fclean re
