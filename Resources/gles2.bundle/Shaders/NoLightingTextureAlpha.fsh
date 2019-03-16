#ifdef GL_ES
precision highp float;
#endif

varying vec2 v_texcoord0;
varying vec4 v_color;

uniform sampler2D u_texture0;

void main() {
    gl_FragColor = vec4(v_color.rgb, texture2D(u_texture0, v_texcoord0).a);
}
