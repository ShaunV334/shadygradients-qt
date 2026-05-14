#version 330 core

uniform float uTime;
uniform float uSpeed;

uniform float uC1r;
uniform float uC1g;
uniform float uC1b;
uniform float uC2r;
uniform float uC2g;
uniform float uC2b;
uniform float uC3r;
uniform float uC3g;
uniform float uC3b;

in vec3 vPos;
in vec3 vNormal;
in vec2 vUv;
in float vHolographicIntensity;
in float vCosmicWave;

out vec4 FragColor;

// Holographic helper functions
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

float noise2D(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(
        mix(hash(i + vec2(0.0, 0.0)), hash(i + vec2(1.0, 0.0)), u.x),
        mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), u.x),
        u.y
    );
}

void main() {
    vec3 color1 = vec3(uC1r, uC1g, uC1b);
    vec3 color2 = vec3(uC2r, uC2g, uC2b);
    vec3 color3 = vec3(uC3r, uC3g, uC3b);

    float t = uTime * uSpeed;

    // Smooth gradient driven by the global sine wave
    float wave = 0.5 + 0.5 * vCosmicWave;
    float drift = 0.5 + 0.5 * sin(vPos.y * 0.35 + t * 0.4);

    vec3 baseGradient = mix(color1, color2, smoothstep(0.15, 0.85, wave));
    baseGradient = mix(baseGradient, color3, smoothstep(0.25, 0.95, drift));

    // Keep the shimmer very subtle so it doesn't turn into visible banding
    float shimmer = noise2D(vUv * 3.0 + t * 0.2) * 0.04;
    float glow = 0.08 * vHolographicIntensity;

    vec3 finalColor = baseGradient + vec3(shimmer + glow);

    // Add subtle dithering to reduce color banding (slightly stronger)
    float dither = (hash(gl_FragCoord.xy) - 0.5) / 255.0;
    finalColor = clamp(finalColor + vec3(dither), 0.0, 1.0);

    // Do not perform manual gamma correction here if the GL framebuffer
    // is sRGB-capable (Qt/GL will handle linear->sRGB conversion). Manual
    // gamma correction can make banding worse if the framebuffer is already
    // performing sRGB conversion.

    FragColor = vec4(finalColor, 1.0);
}
