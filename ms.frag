#version 450

layout(set = 0, binding = 0) uniform sampler2D tex;

layout(location = 0) out vec4 frag_colour;

layout(location = 0) in vec2 frag_uv;
layout(location = 1) in vec4 frag_bg;

void main() {
  vec4 rgba = texelFetch(tex, ivec2(frag_uv * vec2(8,4)), 0);
  frag_colour = mix(frag_bg, rgba, rgba.a);
}
