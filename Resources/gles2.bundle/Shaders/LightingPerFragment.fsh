#ifdef GL_ES
precision highp float;
#endif

struct Light
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 position;
};

varying vec2 v_texcoord0;
varying vec4 v_color;

varying vec3 v_normal;
varying vec3 v_light0Vector;
varying vec3 v_light0HalfVector;

uniform sampler2D u_texture0;
uniform sampler2D u_texture1;

uniform vec4 u_globalAmbientColor;
uniform float u_materialShininess;
uniform vec4 u_materialAmbient;
uniform vec4 u_materialDiffuse;
uniform vec4 u_materialSpecular;
uniform vec4 u_materialEmission;
uniform Light u_light0;

void main() {
    vec4 texture0color = texture2D(u_texture0, v_texcoord0);
    
    vec3 normal = normalize(v_normal);
    
    /* light0 - simple point light with per fragment shading */
    
    float light0cosL = max(dot(normal, normalize(v_light0Vector)), 0.0);
    float light0cosH = max(dot(normal, normalize(v_light0HalfVector)), 0.0);
    vec4 light0color =
        u_light0.ambient * u_materialAmbient +
        u_light0.diffuse * u_materialDiffuse * light0cosL +
        u_light0.specular * u_materialSpecular * pow(light0cosH, u_materialShininess + 1.0);
    
    vec4 lightcolor = u_globalAmbientColor * u_materialAmbient + u_materialEmission + light0color;
    
    gl_FragColor = min(v_color * lightcolor, 1.0) * texture0color;
}
