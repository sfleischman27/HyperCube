R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec2 outTexCoord;
in vec4 pos;

out vec4 frag_color;

uniform sampler2D billTex;
uniform sampler2D depthTexture;

float DecodeFloatRGBA(vec4 rgba) {
  return dot( rgba, vec4(1.0, 1.0/255.0, 1.0/65025.0, 1.0/16581375.0) );
}

void main(void) {
	// Draw
	frag_color = texture(billTex, outTexCoord);
	if (frag_color.a == 0.0) {
		discard;
	}

	vec2 screenPos = (pos.xy / pos.w) * 0.5 - 0.5;
	float depth = DecodeFloatRGBA(texture(depthTexture, pos.xy * 0.5 - 0.5));
	if (depth < gl_FragCoord.z) {
		discard;
	}
	frag_color = vec4(pos.xy, 0.0, 1.0);
}

/////////// SHADER END //////////)"
