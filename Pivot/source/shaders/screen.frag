R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec2 outTexCoord;

out vec4 frag_color;

uniform sampler2D screenTexture;

void main(void) {
	frag_color = texture(screenTexture, outTexCoord);
	frag_color.a = 1.0;
}

/////////// SHADER END //////////)"
