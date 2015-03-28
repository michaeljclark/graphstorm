#ifdef GL_ES
precision highp float;
#endif

varying vec2 v_texcoord0;
varying vec4 v_color;

uniform sampler2D u_texture0;
uniform sampler2D u_texture1;

void main() {
    gl_FragColor = v_color * texture2D(u_texture0, v_texcoord0);
}
