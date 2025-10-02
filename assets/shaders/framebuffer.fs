#version 330
precision mediump float;

in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D tex;
uniform float darkness;

void main() {
    vec4 result;
    
    vec4 color = texture(tex, texCoord);
    result = vec4(color.rgb * (1.0-darkness), 1.0);
    if (color.a < 0.01) discard;
    fragColor = result;
}