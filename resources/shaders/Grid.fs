#version 330 core
uniform float gridScale;
uniform vec3 gridColour;

in vec3 outPos;

out vec4 FragColor;


float gridProximity(vec3 fragPos3D) {
    // Anti-aliased value for the proximity to a grid line
    vec2 coord = fragPos3D.xy / gridScale;
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);
    return 1 - min(grid.x, grid.y);
}

void main() {
    float proximity = gridProximity(outPos);
    FragColor = vec4(gridColour * proximity, proximity);
}
