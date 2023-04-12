R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec4 pos;
in vec2 outTexCoord;

out vec4 frag_color;

void main(void) {
	frag_color = vec4(outTexCoord, 0.0, 1.0);
}

/////////// SHADER END //////////)"
