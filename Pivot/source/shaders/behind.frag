R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec2 outTexCoord;

out vec4 frag_color;

uniform sampler2D billTex;
uniform sampler2D replaceTexture;
uniform float alpha;
uniform float darken;

void main(void) {
	// Draw
	frag_color = texture(billTex, outTexCoord);
	if (frag_color.a == 0.0) {
		discard;
	}
	frag_color.xyz = frag_color.xyz - vec3(darken); //TEMPORARY: artificial darkening
	frag_color.a = alpha;
}

/////////// SHADER END //////////)"
