#version 330 core

in vec3 vNormal;
in vec3 vPos;
in vec2 vUv;
in float vDistort;
in vec3 vViewPosition;

uniform float uC1r;
uniform float uC1g;
uniform float uC1b;
uniform float uC2r;
uniform float uC2g;
uniform float uC2b;
uniform float uC3r;
uniform float uC3g;
uniform float uC3b;

out vec4 fragColor;

void main() {
    vec3 color1 = vec3(uC1r, uC1g, uC1b);
    vec3 color2 = vec3(uC2r, uC2g, uC2b);
    vec3 color3 = vec3(uC3r, uC3g, uC3b);

    vec3 finalColor = mix(mix(color1, color2, smoothstep(-3.0, 3.0, vPos.x)), color3, vPos.z);

    // Apply some brightness to match the URL (brightness=1.1)
    finalColor *= 1.1;
    finalColor = clamp(finalColor, 0.0, 1.0);

    fragColor = vec4(finalColor, 1.0);
}
