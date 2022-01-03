#version 330 core
in vec3 nearPoint;
in vec3 farPoint;

out vec4 FragColor;

float gridScale = 1;
vec3 gridColor = vec3(0.2);


float gridStrength(vec3 fragPos3D) {
    vec2 coord = fragPos3D.xy * gridScale;
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);
    return 1 - min(grid.x, grid.y);
}

void main() {
    // Parametric line equation to only render where the distance to z = 0 > 0
    // Not really needed for orthographic, but worth keeping for reference
    float t = -nearPoint.z / (farPoint.z - nearPoint.z);
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);
    float strength = gridStrength(fragPos3D);
    FragColor = vec4(gridColor * strength, strength * float(t > 0));
}
