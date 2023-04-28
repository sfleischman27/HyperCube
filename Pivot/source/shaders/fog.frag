R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec4 pos;
in vec2 outTexCoord;

out vec4 frag_color;

uniform sampler2D cutTexture;
uniform sampler2D depthTexture;
uniform sampler2D replaceTexture; // can skip lighting calculation on "toCut" to save time
uniform sampler2D normalTexture;

void main(void) {
    if (texture(replaceTexture, outTexCoord).r == 0.0) {
        discard;
    }
	vec3 diff = texture(cutTexture, outTexCoord).xyz;
	float d = texture(depthTexture, outTexCoord).r * 10000.0;
	d *= 50.0; // higher multiplier here = depth fades out sooner
	vec3 fadeColor = vec3(1.0, 1.0, 1.0);
	frag_color.xyz = fadeColor * d;
	//frag_color.xyz *= .25;
	frag_color.a = 1.0;
}

/////////// SHADER END //////////)"
