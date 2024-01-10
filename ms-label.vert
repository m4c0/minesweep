#version 450

layout(push_constant) uniform upc {
  vec4 client_area;
  vec2 hover;
} pc;

layout(location = 0) in vec2 position;

layout(location = 0) out vec2 frag_uv;

void main() {
  vec2 area = pc.client_area.xy / pc.client_area.zw;
  area = 2.0 * area - 1.0;

  vec2 p = vec2(0, -6) / pc.client_area.zw;
  p += mix(area, -area, position);

  gl_Position = vec4(p, 0, 1);
  frag_uv = position;
}

