R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec4 outColor;
in vec4 pos;
in vec2 outTexCoord;

out vec4 frag_color;

uniform mat4 Mv;
uniform sampler2D fsqTexture;

void main(void) {
	//float d = -(Mv * pos).z;
	//float div = 100;
	//frag_color = vec4(0, 0, 0, 1);// - vec4(d / div, d / div, d / div, 0);
	//frag_color = vec4(0, 0, 0, 1);
	frag_color = texture(fsqTexture, outTexCoord);
	if (frag_color.xyz == vec3(0, 0, 0)) {
		frag_color = vec4(1, 1, 1, 1);
	}
	//frag_color = vec4(outTexCoord, 0, 1);
}

/////////// SHADER END //////////)"
