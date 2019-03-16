#version 110

#ifdef GL_ES
precision highp float;
#endif

attribute vec3 a_position;
attribute vec2 a_texcoord0;
attribute vec4 a_color;
attribute float a_gamma;

uniform mat4 u_modelViewProjectionMatrix;

varying vec4 v_color;
varying vec2 v_texcoord0;
varying float v_gamma;

void main() {
    v_color = a_color;
    v_gamma = a_gamma;
    v_texcoord0 = a_texcoord0;
    gl_Position = u_modelViewProjectionMatrix * vec4(a_position.xyz, 1.0);
}
