#version 450

layout(push_constant) uniform upc {
  float grid_size;
} pc;

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 i_pos;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec4 colour;

layout(location = 0) out vec2 frag_coord;

void main() {
  vec2 p = position * 2.0 - 1.0;
  gl_Position = vec4(p, 0, 1);
  frag_coord = p;
}