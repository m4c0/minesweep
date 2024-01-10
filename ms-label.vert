#version 450

layout(push_constant) uniform upc {
  vec4 client_area;
  vec2 hover;
} pc;

layout(location = 0) in vec2 position;

layout(location = 0) out vec2 frag_uv;

void main() {
  vec2 p = (position + pc.client_area.xy) / pc.client_area.zw;
  p = 2.0 * p - 1.0;
  gl_Position = vec4(p, 0, 1);
  frag_uv = position;
}

