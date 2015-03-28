struct Light
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 position;
};

attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord0;
attribute vec2 a_texcoord1;
attribute vec4 a_color;

varying vec2 v_texcoord0;
varying vec4 v_color;

varying vec3 v_normal;
varying vec3 v_light0Vector;
varying vec3 v_light0HalfVector;

uniform mat4 u_modelViewMatrix;
uniform mat4 u_modelViewProjectionMatrix;
uniform mat3 u_normalMatrix;

uniform vec4 u_globalAmbientColor;
uniform float u_materialShininess;
uniform vec4 u_materialAmbient;
uniform vec4 u_materialDiffuse;
uniform vec4 u_materialSpecular;
uniform vec4 u_materialEmission;
uniform Light u_light0;

void main() {
    gl_Position = u_modelViewProjectionMatrix * a_position;
    
    v_texcoord0 = a_texcoord0;
    v_color = a_color;
    
    /* lighting */
    
    vec3 light0Vector;
    vec3 light0HalfVector;
    if (u_light0.position.w != 0.0) {
        vec4 vertexPositionInEye = u_modelViewMatrix * a_position;
        vec3 eyeVector = normalize(-vertexPositionInEye.xyz);
        light0Vector = normalize(u_light0.position.xyz - vertexPositionInEye.xyz);
        light0HalfVector = normalize(eyeVector + light0Vector);
    } else {
        vec3 eyeVector = vec3(1.0, 0.0, 0.0);
        light0Vector = normalize(u_light0.position.xyz);
        light0HalfVector = normalize(eyeVector + light0Vector);
    }
        
    v_normal = normalize(u_normalMatrix * a_normal);
        
    /* light0 */
    v_light0Vector = light0Vector;
    v_light0HalfVector = light0HalfVector;
}
