attribute float a_point;
attribute float a_face;
attribute vec4 a_color;

varying vec4 v_color;

uniform mat4 u_modelViewProjectionMatrix;

uniform float u_pointSize;
uniform float u_texWidth;
uniform sampler2D u_positionTex;
uniform vec4 u_minBound;
uniform vec4 u_maxBound;
uniform vec4 u_viewBound;
uniform vec4 u_viewOffset;

void main()
{
    vec4 pos = texture2D(u_positionTex, vec2(a_point / u_texWidth, 0));
    vec4 proj = (pos - u_minBound) / (u_maxBound - u_minBound) * u_viewBound + u_viewOffset;
    vec4 finalpos = vec4(proj.x, proj.y, 0.0, 1.0);

    float h = u_pointSize;
    
    // front
    if (a_face ==  1.0) finalpos += vec4(-h, -h, -h, 0);
    if (a_face ==  2.0) finalpos += vec4( h, -h, -h, 0);
    if (a_face ==  3.0) finalpos += vec4( h,  h, -h, 0);
    if (a_face ==  4.0) finalpos += vec4(-h,  h, -h, 0);

    // back
    if (a_face ==  5.0) finalpos += vec4(-h, -h,  h, 0);
    if (a_face ==  6.0) finalpos += vec4( h, -h,  h, 0);
    if (a_face ==  7.0) finalpos += vec4( h,  h,  h, 0);
    if (a_face ==  8.0) finalpos += vec4(-h,  h,  h, 0);

    // top
    if (a_face ==  9.0) finalpos += vec4(-h,  h, -h, 0);
    if (a_face == 10.0) finalpos += vec4( h,  h, -h, 0);
    if (a_face == 11.0) finalpos += vec4( h,  h,  h, 0);
    if (a_face == 12.0) finalpos += vec4(-h,  h,  h, 0);

    // bottom
    if (a_face == 13.0) finalpos += vec4(-h, -h, -h, 0);
    if (a_face == 14.0) finalpos += vec4( h, -h, -h, 0);
    if (a_face == 15.0) finalpos += vec4( h, -h,  h, 0);
    if (a_face == 16.0) finalpos += vec4(-h, -h,  h, 0);

    // left
    if (a_face == 17.0) finalpos += vec4(-h, -h,  h, 0);
    if (a_face == 18.0) finalpos += vec4(-h, -h, -h, 0);
    if (a_face == 19.0) finalpos += vec4(-h,  h, -h, 0);
    if (a_face == 20.0) finalpos += vec4(-h,  h,  h, 0);

    // right
    if (a_face == 21.0) finalpos += vec4( h, -h, -h, 0);
    if (a_face == 22.0) finalpos += vec4( h, -h,  h, 0);
    if (a_face == 23.0) finalpos += vec4( h,  h,  h, 0);
    if (a_face == 24.0) finalpos += vec4( h,  h, -h, 0);

    gl_Position = u_modelViewProjectionMatrix * finalpos;
    v_color = a_color;
}
