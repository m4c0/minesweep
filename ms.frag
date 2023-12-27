#version 450

layout(set = 0, binding = 0) uniform sampler2D tex;

layout(location = 0) out vec4 frag_colour;

layout(location = 0) in vec2 frag_uv;

void main() {
  vec4 rgba = texture(tex, frag_uv);
  frag_colour = vec4(1, rgba.gba);
}
