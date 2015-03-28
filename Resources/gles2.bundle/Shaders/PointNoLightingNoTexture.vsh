attribute vec4 a_position;
attribute float a_pointsize;
attribute vec4 a_color;

varying vec4 v_color;

uniform mat4 u_modelViewProjectionMatrix;

void main() {
    gl_Position = u_modelViewProjectionMatrix * a_position;
    gl_PointSize = a_pointsize;
    v_color = a_color;
}
