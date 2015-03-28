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

    /* lighting */
    vec3 normal = normalize(u_normalMatrix * a_normal);

    /* light0 - simple point light */

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
    
    float light0cosL = max(dot(normal, light0Vector), 0.0);
    float light0cosH = max(dot(normal, light0HalfVector), 0.0);
    vec4 light0color =
        u_light0.ambient * u_materialAmbient +
        u_light0.diffuse * u_materialDiffuse * light0cosL +
        u_light0.specular * u_materialSpecular * pow(light0cosH, u_materialShininess + 1.0);
    
    vec4 lightcolor = u_globalAmbientColor * u_materialAmbient + u_materialEmission + light0color;
    
    v_color = min(a_color * lightcolor, 1.0);
}
