R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec2 outTexCoord;

out vec4 frag_color;

uniform sampler2D billTex;
uniform sampler2D replaceTexture;
uniform float alpha;

void main(void) {
	// Draw
	frag_color = texture(billTex, outTexCoord);
	if (frag_color.a < 0.5) {
		discard;
	}
	frag_color.a = alpha;
}

/////////// SHADER END //////////)"
