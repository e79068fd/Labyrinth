#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 mv_matrix;
uniform mat4 mvp_matrix;
uniform mat4 normal_matrix;

attribute vec4 a_position;
attribute vec4 a_normal;
attribute vec2 a_texcoord;

varying vec3 normal;
varying vec3 pos;

void main() {
    pos = vec3(mv_matrix * a_position);
    normal = vec3(normal_matrix * a_normal);//normalize(vec3(mv_matrix * a_normal));

    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * a_position;
}
