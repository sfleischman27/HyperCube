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
uniform sampler2D outsideTexture;

void main(void) {
	//float d = -(Mv * pos).z;
	//float div = 100;
	//frag_color = vec4(0, 0, 0, 1);// - vec4(d / div, d / div, d / div, 0);
	//frag_color = vec4(0, 0, 0, 1);
	frag_color = texture(fsqTexture, outTexCoord);
	if (frag_color.xyz == vec3(0, 0, 0)) {
        // TEMP: layer tex coords
		vec2 transTexCoord;
        int numTexX = 16;
        int numTexY = 9;
        transTexCoord.x = mod(outTexCoord.x * numTexX, 1.0);
        transTexCoord.y = mod(outTexCoord.y * numTexY, 1.0);
		// End temp
		frag_color = texture(outsideTexture, transTexCoord);
	}
	//frag_color = vec4(outTexCoord, 0, 1);
}

/////////// SHADER END //////////)"
