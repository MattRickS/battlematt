# TODO: Separate make files for each lib
APP=mapmaker
SRC_DIR = src
IMGUI_DIR = lib/imgui
GLUTIL_DIR = lib/glutil
BUILD_DIR = build
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/Token.cpp $(SRC_DIR)/BGImage.cpp $(SRC_DIR)/stb_image.cpp $(SRC_DIR)/glad.c
SOURCES += $(GLUTIL_DIR)/lib/Camera.cpp $(GLUTIL_DIR)/lib/Mesh.cpp $(GLUTIL_DIR)/lib/Primitives.cpp $(GLUTIL_DIR)/lib/Shader.cpp $(GLUTIL_DIR)/lib/Texture.cpp $(GLUTIL_DIR)/lib/TextureCache.cpp
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/imgui_impl_glfw.cpp $(IMGUI_DIR)/imgui_impl_opengl3.cpp
OBJS = $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))

LIBS = -lGL -pthread
LIBS += `pkg-config --static --libs glfw3`
CXXFLAGS = --std=c++17 -I$(IMGUI_DIR) -I$(GLUTIL_DIR)/includes -Iincludes
CXXFLAGS += -g -Wall -Wformat
CXXFLAGS += `pkg-config --cflags glfw3`

$(APP): $(OBJS)
	$(CXX) -o $(BUILD_DIR)/$@ $^ $(CXXFLAGS) $(LIBS)

$(BUILD_DIR)/%.o:$(SRC_DIR)/%.c
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o:$(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o:$(GLUTIL_DIR)/lib/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm $(BUILD_DIR)/$(APP) $(OBJS)
