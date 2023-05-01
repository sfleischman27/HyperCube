R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec2 outTexCoord;

layout (location = 0) out vec4 frag_color_r;
layout (location = 1) out vec4 frag_color_g;
layout (location = 2) out vec4 frag_color_b;

uniform sampler2D albedoTexture;
uniform sampler2D replaceTexture;
uniform sampler2D depthTexture;
uniform sampler2D outsideTexture;
uniform vec2 transOffset;
uniform vec2 screenSize;

// Editable parameter for how thick the cut outline is
const float thickness = 1.0;
// Editable parameter for strength of ambient lighting. 0 = no ambient light, 1 = all ambient light
const float ambient = .4;

vec4 EncodeFloatRGBA(float v) {
    vec4 enc = vec4(1.0, 255.0, 65025.0, 16581375.0) * v;
    enc = fract(enc);
    enc -= enc.yzww * vec4(1.0/255.0, 1.0/255.0, 1.0/255.0, 0.0);
    return enc;
}

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
	vec4 frag_color;
	if (texture(replaceTexture, outTexCoord).r == 0.0) {

        // Cut texture translation coordinates
		vec2 midTexCoord = outTexCoord + transOffset;
		vec2 transTexCoord;
        transTexCoord.x = mod(midTexCoord.x, 1.0);
        transTexCoord.y = 1.0 - mod(midTexCoord.y, 1.0);

		// Set cut texture
		frag_color = texture(outsideTexture, transTexCoord);
		frag_color.a = 1.0;

		// Edge detection, set to green if cut is near non-cut
		if (checkNeighboring(replaceTexture, outTexCoord)) {
			frag_color = vec4(0.0, 1.0, 0.0, 1.0);
		}
	} else if (texture(depthTexture, outTexCoord).xyz == vec3(1.0)) {
		// Background color
		frag_color = vec4(31.0, 34.0, 69.0, 255.0) / 255.0;
	} else {
		// Ambient lighting
		frag_color = texture(albedoTexture, outTexCoord);
		frag_color.a = ambient;
	}
    frag_color_r = EncodeFloatRGBA(frag_color.r);
    frag_color_g = EncodeFloatRGBA(frag_color.g);
    frag_color_b = EncodeFloatRGBA(frag_color.b);
}

/////////// SHADER END //////////)"
