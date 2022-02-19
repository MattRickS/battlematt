# TODO: Separate make files for each lib
APP=mapmaker
SRC_DIR = src
IMGUI_DIR = lib/imgui
FILEDIALOG_DIR = lib/ImGuiFileDialog
GLUTIL_DIR = ${SRC_DIR}/glutil
CONTROLLER_DIR = ${SRC_DIR}/controller
MODEL_DIR = ${SRC_DIR}/model
VIEW_DIR = ${SRC_DIR}/view
BUILD_DIR = build
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/JSONSerializer.cpp $(SRC_DIR)/Resources.cpp $(SRC_DIR)/stb_image.cpp $(SRC_DIR)/glad.c \
		  $(CONTROLLER_DIR)/Application.cpp $(CONTROLLER_DIR)/Controller.cpp \
          $(MODEL_DIR)/BGImage.cpp $(MODEL_DIR)/Bounds.cpp $(MODEL_DIR)/Grid.cpp $(MODEL_DIR)/Scene.cpp $(MODEL_DIR)/Shape2D.cpp $(MODEL_DIR)/Token.cpp \
		  $(VIEW_DIR)/Window.cpp $(VIEW_DIR)/Viewport.cpp $(VIEW_DIR)/UIControls.cpp $(VIEW_DIR)/RenderBuffer.cpp $(VIEW_DIR)/SharedRenderBuffer.cpp
SOURCES += $(FILEDIALOG_DIR)/ImGuiFileDialog.cpp
SOURCES += $(GLUTIL_DIR)/Camera.cpp $(GLUTIL_DIR)/Mesh.cpp $(GLUTIL_DIR)/Matrix2D.cpp $(GLUTIL_DIR)/Shader.cpp $(GLUTIL_DIR)/Texture.cpp
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/misc/cpp/imgui_stdlib.cpp
SOURCES += $(IMGUI_DIR)/imgui_impl_glfw.cpp $(IMGUI_DIR)/imgui_impl_opengl3.cpp
OBJS = $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))

LIBS = -lGL -pthread
LIBS += `pkg-config --static --libs glfw3`
CXXFLAGS = --std=c++17 -lstdc++fs
CXXFLAGS += -I$(IMGUI_DIR) -I$(FILEDIALOG_DIR) -I$(IMGUI_DIR)/misc/cpp -Iincludes
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

$(BUILD_DIR)/%.o:$(IMGUI_DIR)/misc/cpp/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o:$(FILEDIALOG_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o:$(GLUTIL_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o:$(CONTROLLER_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o:$(MODEL_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o:$(VIEW_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm $(BUILD_DIR)/$(APP) $(OBJS)
