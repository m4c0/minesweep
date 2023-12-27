#version 450

layout(set = 0, binding = 0) uniform sampler2D tex;

layout(location = 0) out vec4 frag_colour;

layout(location = 0) in vec2 frag_coord;

void main() {
  vec2 uv = frag_coord * 0.5 + 0.5;
  frag_colour = texture(tex, uv);
}
