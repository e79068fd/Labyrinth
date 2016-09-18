#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform sampler2D texture;

varying vec2 v_texcoord;

uniform vec4 color;

uniform int rendTexture;

void main()
{
    if(rendTexture == 1) {
        // Set fragment color from texture
        gl_FragColor = texture2D(texture, v_texcoord);
    } else {
        gl_FragColor = color;
    }
}
