#version 330
precision mediump float;

out vec4 fragColor;

uniform vec3 objectColor;

void main() {

    vec3 result = objectColor;
    fragColor = vec4(result, 1.0f);
}