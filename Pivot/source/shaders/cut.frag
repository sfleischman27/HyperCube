R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec4 pos;
in vec2 outTexCoord;

out vec4 frag_color;

uniform sampler2D cutTexture;
uniform sampler2D outsideTexture;
uniform sampler2D replaceTexture;
uniform sampler2D depthTexture;
uniform vec2 transOffset;
uniform vec2 screenSize;

bool checkNeighboring(sampler2D tx, vec2 oTex) {
	float bound = 1.0;
	float offX = bound/screenSize.x;
	float offY = bound/screenSize.y;

	// sides
	vec2 temp = oTex;
	temp.x = temp.x - offX;
	if (texture(tx, temp).r != 0.0) {
		return true;
	}
	temp = oTex;
	temp.x = temp.x + offX;
	if (texture(tx, temp).r != 0.0) {
		return true;
	}
	temp = oTex;
	temp.y = temp.y - offY;
	if (texture(tx, temp).r != 0.0) {
		return true;
	}
	temp = oTex;
	temp.y = temp.y + offY;
	if (texture(tx, temp).r != 0.0) {
		return true;
	}

	// corners
	temp = oTex;
	temp.x = temp.x - offX;
	temp.y = temp.y - offY;
	if (texture(tx, temp).r != 0.0) {
		return true;
	}
	temp = oTex;
	temp.y = temp.y - offY;
	temp.x = temp.x + offX;
	if (texture(tx, temp).r != 0.0) {
		return true;
	}
	temp = oTex;
	temp.y = temp.y + offY;
	temp.x = temp.x - offX;
	if (texture(tx, temp).r != 0.0) {
		return true;
	}
	temp = oTex;
	temp.y = temp.y + offY;
	temp.x = temp.x + offX;
	if (texture(tx, temp).r != 0.0) {
		return true;
	}
	return false;
}

void main(void) {
	if (texture(replaceTexture, outTexCoord).r == 0.0) {

        // TEMP: layer tex coords
		vec2 transTexCoord;
        
        float numTexX = 1.0;
        float numTexY = 1.0;
		vec2 midTexCoord = outTexCoord + transOffset;
        
        transTexCoord.x = mod(midTexCoord.x * numTexX, 1.0);
        transTexCoord.y = 1.0 - mod(midTexCoord.y * numTexY, 1.0);
		// End temp

		frag_color = texture(outsideTexture, transTexCoord);
		frag_color.a = 1.0;
		// Check neighboring and set to green
		if (checkNeighboring(replaceTexture, outTexCoord)) {
			frag_color = vec4(0.0, 1.0, 0.0, 1.0);
		}
	} else if (texture(depthTexture, outTexCoord).xyz == vec3(1.0)) {
		frag_color = vec4(31.0, 34.0, 69.0, 255.0) / 255.0;
	} else {
		discard;
	}
}

/////////// SHADER END //////////)"
