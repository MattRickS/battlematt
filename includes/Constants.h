#include <glm/glm.hpp>

const float DEFAULT_PIXELS_PER_UNIT = 50.0f;
const glm::vec4 BLACK_RGBA = glm::vec4(0.0f);
const glm::vec3 SELECTION_COLOR = glm::vec3(0.0f, 1.0f, 1.0f);
const glm::vec3 HIGHLIGHT_COLOR = SELECTION_COLOR * 0.5f;
const glm::vec4 SELECTION_COLOR_ALPHA = glm::vec4(SELECTION_COLOR, 1.0f);
const glm::vec4 HIGHLIGHT_COLOR_ALPHA = glm::vec4(HIGHLIGHT_COLOR, 1.0f);
const float OVERLAY_OPACITY = 0.3f;

