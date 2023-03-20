R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec4 pos;
in vec2 outTexCoord;

out vec4 frag_color;

uniform sampler2D fsqTexture;
uniform sampler2D outsideTexture;
uniform vec2 transOffset;
uniform vec2 screenSize;

bool checkNeighboring(sampler2D tx, vec2 oTex) {
	// remove this function to remove denoising
	vec4 cur = texture(fsqTexture, outTexCoord);
	if (cur == vec4(1.0, 0.0, 0.0, 1.0)) {
		return false;
	}
	if (cur == vec4(0.0, 1.0, 0.0, 1.0)) {
		return false;
	}
	if (cur == vec4(0.0, 0.0, 1.0, 1.0)) {
		return false;
	}
	float bound = 2.0;
	float offX = bound/screenSize.x;
	float offY = bound/screenSize.y;
	vec2 temp = oTex;
	temp.x = temp.x - offX;
	if (texture(tx, temp).xyz == vec3(1.0, 0.0, 1.0)) {
		return true;
	}
	temp = oTex;
	temp.x = temp.x + offX;
	if (texture(tx, temp).xyz == vec3(1.0, 0.0, 1.0)) {
		return true;
	}
	temp = oTex;
	temp.y = temp.y - offY;
	if (texture(tx, temp).xyz == vec3(1.0, 0.0, 1.0)) {
		return true;
	}
	temp = oTex;
	temp.y = temp.y + offY;
	if (texture(tx, temp).xyz == vec3(1.0, 0.0, 1.0)) {
		return true;
	}
	return false;
}

void main(void) {
	frag_color = texture(fsqTexture, outTexCoord);
	if (frag_color.xyz == vec3(1.0, 0.0, 1.0) || checkNeighboring(fsqTexture, outTexCoord)) {

        // TEMP: layer tex coords
		vec2 transTexCoord;
		float xStretch = 1.0; // should be 2
		float yStretch = 1.0; // should be .5
        float numTexX = (16.0 / 10.0) * xStretch;
        float numTexY = (9.0 / 6.0) * yStretch;
		vec2 midTexCoord = mod(outTexCoord + transOffset, 1.0);
        transTexCoord.x = mod(midTexCoord.x * numTexX, 1.0);
        transTexCoord.y = mod(midTexCoord.y * numTexY, 1.0);
		// End temp

		frag_color = texture(outsideTexture, transTexCoord);
	}
}

/////////// SHADER END //////////)"
