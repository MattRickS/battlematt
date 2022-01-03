#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout(std140, binding=0) uniform Camera
{
    mat4 projection;
    mat4 view;
} camera;

// out vec3 FragPos;
// out vec3 Normal;
out vec2 UV;

uniform mat4 model;

void main()
{
    // FragPos = vec3(model * vec4(aPos, 1.0));
    UV = aUV;

    gl_Position = camera.projection * camera.view * model * vec4(aPos, 1.0);
}
