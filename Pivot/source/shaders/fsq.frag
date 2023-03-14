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
//uniform sampler2D depthTexture;

bool checkNeighboring(sampler2D tx, vec2 oTex) {
	// remove this function to remove denoising
	float bound = 2;
	float offX = bound/1024;
	float offY = bound/576;
	vec2 temp = oTex;
	temp.x = temp.x - offX;
	if (texture(tx, temp).xyz == vec3(1, 0, 1)) {
		return true;
	}
	temp = oTex;
	temp.x = temp.x + offX;
	if (texture(tx, temp).xyz == vec3(1, 0, 1)) {
		return true;
	}
	temp = oTex;
	temp.y = temp.y - offY;
	if (texture(tx, temp).xyz == vec3(1, 0, 1)) {
		return true;
	}
	temp = oTex;
	temp.y = temp.y + offY;
	if (texture(tx, temp).xyz == vec3(1, 0, 1)) {
		return true;
	}
	return false;
}

void main(void) {
	//float d = -(Mv * pos).z;
	//float div = 100;
	//frag_color = vec4(0, 0, 0, 1);// - vec4(d / div, d / div, d / div, 0);
	//frag_color = vec4(0, 0, 0, 1);
	frag_color = texture(fsqTexture, outTexCoord);
	if (frag_color.xyz == vec3(1, 0, 1) || checkNeighboring(fsqTexture, outTexCoord)) {
        // TEMP: layer tex coords
		vec2 transTexCoord;
        float numTexX = (16 / 10) * 2;
        float numTexY = (9 / 6) * .5;
        transTexCoord.x = mod(outTexCoord.x * numTexX, 1.0);
        transTexCoord.y = mod(outTexCoord.y * numTexY, 1.0);
		// End temp
		frag_color = texture(outsideTexture, transTexCoord);
	}
}

/////////// SHADER END //////////)"
