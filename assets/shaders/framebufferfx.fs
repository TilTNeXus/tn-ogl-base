#version 330
precision mediump float;

in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D tex;
uniform float caAmountX;
uniform float caAmountY;
uniform vec3 filterColor;
uniform float filterAmount;
uniform float time;

const float pi = 3.14;
const float offset = 1.0/100.0;
const float oscAmp = 0.00;
const float oscPeriod = 0.5;
const float oscSpeed = 0.5;

void main() {
    vec4 result;
    float oscOffset = oscAmp * sin(2.0*pi/oscPeriod * (oscSpeed*time + texCoord.y));

    float red = texture(tex, texCoord + vec2(caAmountX + oscOffset, caAmountY)).rgb.r;
    float green = texture(tex, texCoord + vec2(oscOffset, caAmountY)).rgb.g;
    float blue = texture(tex, texCoord - vec2(caAmountX - oscOffset, caAmountY)).rgb.b;
    float alpha = texture(tex, texCoord).a;
    result = vec4(red, green, blue, alpha);



    vec3 multiply = vec3(result.x * filterColor.x, result.y * filterColor.y, result.z * filterColor.z);
    result = vec4(result.xyz * vec3(1.0 - filterAmount) + multiply.xyz * vec3(filterAmount), 1.0);
    if (alpha < 0.1) discard;
    fragColor = result;
}