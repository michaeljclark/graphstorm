attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord0;
attribute vec2 a_texcoord1;
attribute vec4 a_color;

varying vec2 v_texcoord0;
varying vec4 v_color;

uniform mat4 u_modelViewMatrix;
uniform mat4 u_modelViewProjectionMatrix;
uniform mat3 u_normalMatrix;

void main() {
    gl_Position = u_modelViewProjectionMatrix * a_position;
    v_texcoord0 = a_texcoord0;
    v_color = a_color;
}
