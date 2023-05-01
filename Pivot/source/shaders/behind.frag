R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec2 outTexCoord;

layout (location = 0) out vec4 frag_color_r;
layout (location = 1) out vec4 frag_color_g;
layout (location = 2) out vec4 frag_color_b;

uniform sampler2D billTex;
uniform sampler2D replaceTexture;
uniform float alpha;

vec4 EncodeFloatRGBA(float v) {
    vec4 enc = vec4(1.0, 255.0, 65025.0, 16581375.0) * v;
    enc = fract(enc);
    enc -= enc.yzww * vec4(1.0/255.0, 1.0/255.0, 1.0/255.0, 0.0);
    return enc;
}

void main(void) {
	// Draw
	vec4 frag_color = texture(billTex, outTexCoord);
	if (frag_color.a < 0.5) {
		discard;
	}
	frag_color.xyz = frag_color.xyz - vec3(.2); //TEMPORARY: artificial darkening
	frag_color.a = alpha;
	
    frag_color_r = EncodeFloatRGBA(frag_color.r);
    frag_color_g = EncodeFloatRGBA(frag_color.g);
    frag_color_b = EncodeFloatRGBA(frag_color.b);
}

/////////// SHADER END //////////)"
