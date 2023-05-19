R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec2 outTexCoord;

out vec4 frag_color;

uniform sampler2D albedoTexture;
uniform sampler2D replaceTexture;
uniform sampler2D depthTexture;
uniform sampler2D outsideTexture;
uniform vec2 transOffset;
uniform vec2 screenSize;
uniform sampler2D background;
uniform float interpStartPosLeft;
uniform float amtOfScreens;
uniform int drawOutline;
uniform vec4 ambientLight;
uniform vec3 cutColor;
uniform vec3 outlineColor;

// Editable parameter for how thick the cut outline is
const float thickness = 1.0;
// Editable parameter for strength of ambient lighting. 0 = no ambient light, 1 = all ambient light
//const float ambient = .4;
const float squashFactor = 2.0;

bool checkNeighboring(sampler2D tx, vec2 oTex) {
	float offX = thickness/screenSize.x;
	float offY = thickness/screenSize.y;
	for (float xAdd = -offX; xAdd <= offX; xAdd += offX) {
		for (float yAdd = -offY; yAdd <= offY; yAdd += offY) {
			vec2 newCoord = oTex + vec2(xAdd, yAdd);
			if (texture(tx, newCoord).r != 0.0) {
				return true;
			}
		}
	}
	return false;
}

void main(void) {
    // Only overwrite if this area is cut
	if (texture(replaceTexture, outTexCoord).r == 0.0) {

        // Cut texture translation coordinates
		vec2 midTexCoord = outTexCoord + transOffset;
		vec2 transTexCoord;
        transTexCoord.x = mod(midTexCoord.x / squashFactor, 1.0);
        transTexCoord.y = 1.0 - mod(midTexCoord.y, 1.0);

		// Set cut texture
		frag_color.xyz = texture(outsideTexture, transTexCoord).xyz * cutColor;
		frag_color.a = 1.0;

		// Edge detection, set to green if cut is near non-cut
		if (checkNeighboring(replaceTexture, outTexCoord) && drawOutline == 1) {
			frag_color = vec4(outlineColor, 1.0);
		}
	} else if (texture(depthTexture, outTexCoord).xyz == vec3(1.0)) {
		// Background color
		//frag_color = vec4(31.0, 34.0, 69.0, 255.0) / 255.0;
		vec2 rotTexCoord = outTexCoord;
		//rotTexCoord *= .5;
		rotTexCoord.x = mod((outTexCoord.x / amtOfScreens) + interpStartPosLeft, 1.0);
		rotTexCoord.y = 1.0 - rotTexCoord.y;
		frag_color = texture(background, rotTexCoord);
		frag_color.a = 1.0;
	} else {
		// Ambient lighting
		frag_color.xyz = texture(albedoTexture, outTexCoord).xyz * ambientLight.xyz;
		frag_color.a = ambientLight.a;
	}
}

/////////// SHADER END //////////)"
