R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec2 outTexCoord;

out vec4 frag_color;

uniform sampler2D billTex;
uniform sampler2D replaceTexture;
uniform sampler2D depthTexture;
uniform float alpha;
uniform float darken;
uniform int compBeforeAlpha;

float DecodeFloatRGBA(vec4 rgba) {
	return dot( rgba, vec4(1.0, 1.0/255.0, 1.0/65025.0, 1.0/16581375.0) );
}

void main(void) {
	// Get color, discard if alpha = 0
	frag_color = texture(billTex, outTexCoord);
	if (frag_color.a == 0.0) {
		discard;
	}

	// Manual depth test
	if (compBeforeAlpha == 0) {
		float depth = DecodeFloatRGBA(texture(depthTexture, gl_FragCoord.xy));
		if (depth < gl_FragCoord.z) {
			discard;
		}
	}

	// Darken and compute alpha
	frag_color.xyz = frag_color.xyz - vec3(darken); //TEMPORARY: artificial darkening
	if (compBeforeAlpha == 1) {
		frag_color.a = alpha;
	}
}

/////////// SHADER END //////////)"
