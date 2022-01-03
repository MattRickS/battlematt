#version 420 core
#extension GL_KHR_vulkan_glsl : enable
layout (location = 0) in vec3 aPos;
layout(std140, binding=0) uniform Camera
{
    mat4 projection;
    mat4 projectionInv;
    mat4 view;
    mat4 viewInv;
} camera;

out vec3 outPos;


vec3 UnprojectPoint(float x, float y, float z) {
    vec4 unprojectedPoint =  camera.viewInv * camera.projectionInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
    outPos = UnprojectPoint(aPos.x, aPos.y, 0.0).xyz;
    gl_Position = vec4(aPos, 1.0); // using directly the clipped coordinates
}