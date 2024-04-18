NAME=Scop
SRC_DIR=Source
SRC_FILES=main.cpp math.cpp obj_file.cpp
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

LIB_DIRS=Third_Party/glfw-3.4/lib-macos-universal
LIBS=glfw3
OPENGL_FRAMEWORKS=CoreVideo OpenGL IOKit Cocoa Carbon
VULKAN_FRAMEWORKS=CoreVideo IOKit Cocoa Carbon

CC=clang
C_FLAGS=$(addprefix -I, $(INCLUDE_DIRS))

CPP=c++
CPP_FLAGS=$(addprefix -I, $(INCLUDE_DIRS)) -std=c++11

all: $(NAME)GL

$(OPENGL_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CPP) $(addprefix -D, $(OPENGL_DEFINES)) $(CPP_FLAGS) -c $< -o $@

$(VULKAN_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CPP) $(addprefix -D, $(VULKAN_DEFINES)) $(CPP_FLAGS) -c $< -o $@

$(OPENGL_OBJ_DIR)/glad.o: Third_Party/glad/src/glad.c
	$(CC) $(C_FLAGS) -c $< -o $@

$(NAME)GL: $(addprefix $(OPENGL_OBJ_DIR)/, $(OBJ_FILES)) $(addprefix $(OPENGL_OBJ_DIR)/, $(OPENGL_OBJ_FILES))
	$(CPP) $(CPP_FLAGS) $(addprefix $(OPENGL_OBJ_DIR)/, $(OBJ_FILES)) $(addprefix $(OPENGL_OBJ_DIR)/, $(OPENGL_OBJ_FILES)) $(addprefix -L, $(LIB_DIRS)) $(addprefix -l, $(LIBS)) $(addprefix -framework , $(OPENGL_FRAMEWORKS)) -o $(NAME)GL

$(NAME)Vk: $(addprefix $(VULKAN_OBJ_DIR)/, $(OBJ_FILES)) $(addprefix $(VULKAN_OBJ_DIR)/, $(VULKAN_OBJ_FILES))
	$(CPP) $(CPP_FLAGS) $(addprefix $(VULKAN_OBJ_DIR)/, $(OBJ_FILES)) $(addprefix $(VULKAN_OBJ_DIR)/, $(VULKAN_OBJ_FILES)) $(addprefix -L, $(LIB_DIRS)) $(addprefix -l, $(LIBS)) $(addprefix -framework , $(VULKAN_FRAMEWORKS)) -o $(NAME)Vk

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)GL
	rm -f $(NAME)Vk

re: fclean all

.PHONY: all clean fclean re
