#version 330 core

in vec3 vNormal;
in vec3 vPos;
in vec2 vUv;
in float vDistort;

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

  // Normalize by sphere radius (5.0) to match Three.js default radius (1.0)
  float normalizedY = vPos.y / 5.0;
  float distanceToCenter = distance(vPos, vec3(0.0, 0.0, 0.0)) / 5.0;

  // Mix colors based on Y position and distance from center
  vec3 diffuseColor = mix(color3, 
                          mix(color2, color1, smoothstep(-1.0, 1.0, normalizedY)),
                          distanceToCenter);

  // Apply basic lighting/ambient to give it some depth like the reference
  vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
  float diff = max(dot(normalize(vNormal), lightDir), 0.0);
  vec3 lighting = vec3(0.8) + vec3(0.2) * diff; // 80% ambient, 20% diffuse to keep it bright

  vec3 finalColor = diffuseColor * lighting;

  // Apply brightness matching URL settings (brightness=1.5)
  finalColor *= 1.5;
  finalColor = clamp(finalColor, 0.0, 1.0);

  fragColor = vec4(finalColor, 1.0);
}
