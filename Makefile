NAME=Scop
SRC_DIR=Source
SRC_FILES=main.cpp math.cpp obj_file.cpp
BACKEND_OPENGL_SRC_FILES=opengl_backend.cpp
BACKEND_VULKAN_SRC_FILES=vulkan_backend.cpp

OBJ_DIR=Obj
OBJ_FILES=$(SRC_FILES:.cpp=.o) $(BACKEND_OPENGL_OBJ_FILES) glad.o
BACKEND_OPENGL_OBJ_FILES=$(BACKEND_OPENGL_SRC_FILES:.cpp=.o)
BACKEND_VULKAN_OBJ_FILES=$(BACKEND_VULKAN_SRC_FILES:.cpp=.o)
INCLUDE_DIRS=Source Third_Party/glfw-3.4/include Third_Party/glad/include
DEFINES=SCOP_BACKEND_OPENGL

LIB_DIRS=Third_Party/glfw-3.4/lib-macos-universal
LIBS=glfw3
FRAMEWORKS=CoreVideo OpenGL IOKit Cocoa Carbon

CC=clang
C_FLAGS=$(addprefix -I, $(INCLUDE_DIRS))
CPP=c++
CPP_FLAGS=$(addprefix -I, $(INCLUDE_DIRS)) -std=c++11

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CPP) $(addprefix -D, $(DEFINES)) $(CPP_FLAGS) -c $< -o $@

$(OBJ_DIR)/glad.o: Third_Party/glad/src/glad.c
	$(CC) $(C_FLAGS) -c $< -o $@

$(NAME): $(addprefix $(OBJ_DIR)/, $(OBJ_FILES))
	$(CPP) $(CPP_FLAGS) $(addprefix $(OBJ_DIR)/, $(OBJ_FILES)) $(addprefix -L, $(LIB_DIRS)) $(addprefix -l, $(LIBS)) $(addprefix -framework , $(FRAMEWORKS)) -o $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
