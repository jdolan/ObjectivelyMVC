precision mediump float;

uniform sampler2D tex;
uniform vec4 color;

in vec2 frag_texcoord;

out vec4 out_color;

void main(void) {
	out_color = color * texture(tex, frag_texcoord);
}
