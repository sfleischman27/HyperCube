R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec4 outColor;
in vec4 pos;

out vec4 frag_color;

uniform mat4 Mv;

void main(void) {
	frag_color = outColor;// + vec4(.00001 * outTexCoord, 0, 0);
	//frag_color = vec4(outTexCoord, 0, 1);
}

/////////// SHADER END //////////)"
