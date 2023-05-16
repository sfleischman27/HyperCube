R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec4 pos;
in vec2 outTexCoord;

out vec4 frag_pos;

uniform int isBillboard;
uniform sampler2D billTex;

void main(void) {

	// Removing alpha for billboards only
	vec4 frag_color = texture(billTex, outTexCoord);
	if (isBillboard == 1) {
		if (frag_color.a < 0.5) {
			discard;
		}
	}

	// Store position
	frag_pos = pos;
	frag_pos.a = 1.0;
}

/////////// SHADER END //////////)"
