R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec4 pos;
in vec2 outTexCoord;

layout (location = 0) out vec4 frag_pos;

uniform int removeA;
uniform sampler2D billTex;

void main(void) {

	vec4 frag_color = texture(billTex, outTexCoord);

	if (removeA == 1) {
		if (frag_color.a < 0.5) {
			discard;
		}
	}

	frag_pos = pos;
	frag_pos.a = 1.0;
}

/////////// SHADER END //////////)"
