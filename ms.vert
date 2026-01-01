#version 450

layout(push_constant) uniform upc {
  vec4 client_area;
  vec2 hover;
} pc;

layout(location = 0) in vec2 i_pos;
layout(location = 1) in vec4 colour;
layout(location = 2) in uint uv;

layout(location = 0) out vec2 frag_uv;
layout(location = 1) out vec4 frag_bg;

void main() {
  vec2 v_pos = vec2(gl_VertexIndex & 1, (gl_VertexIndex >> 1) & 1);

  vec2 p = (v_pos + i_pos + pc.client_area.xy) / pc.client_area.zw;
  p = 2.0 * p - 1.0;
  p.y *= -1;
  gl_Position = vec4(p, 0, 1);
  frag_uv = ivec2(uv % 8, uv / 8) + v_pos;
  frag_bg = i_pos == pc.hover ? vec4(0.4, 0.5, 0.6, 1.0) : colour;
}
