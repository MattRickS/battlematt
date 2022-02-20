#version 330 core
uniform vec4 color;
uniform vec4 coords;

in vec3 outPos;

out vec4 FragColor;


void main() {
    if (coords.x <= gl_FragCoord.x && gl_FragCoord.x <= coords.z && coords.y <= gl_FragCoord.y && gl_FragCoord.y <= coords.w)
        FragColor = color;
    else
        FragColor = vec4(0);
}
