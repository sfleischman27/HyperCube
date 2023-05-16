R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec2 outTexCoord;

out vec4 frag_color;

uniform sampler2D billTex;
uniform float alpha;

void main(void) {
	// Draw
	frag_color = texture(billTex, outTexCoord);
	if (frag_color.a == 0.0) {
		discard;
	}
	frag_color.xyz = frag_color.xyz - vec3(.2); //TEMPORARY: artificial darkening
	frag_color.a = alpha * frag_color.a;
}

/////////// SHADER END //////////)"
