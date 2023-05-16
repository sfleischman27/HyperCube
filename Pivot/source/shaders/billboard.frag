R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec2 outTexCoord;

layout (location = 0) out vec4 frag_color;
layout (location = 2) out vec4 frag_normal;
layout (location = 3) out vec4 frag_depth; // stored in r

uniform sampler2D billTex;
uniform sampler2D normTex;
uniform int useNormTex;
uniform int flipXfrag;
uniform int id;
uniform vec3 uDirection;
uniform float farPlaneDist;
uniform vec3 campos;

vec4 EncodeFloatRGBA(float v) {
  vec4 enc = vec4(1.0, 255.0, 65025.0, 16581375.0) * v;
  enc = fract(enc);
  enc -= enc.yzww * vec4(1.0/255.0, 1.0/255.0, 1.0/255.0, 0.0);
  return enc;
}

void main(void) {

	frag_color = texture(billTex, outTexCoord);
	if (frag_color.a == 0.0) {
		discard;
	}
	frag_color.a = 1.0;
	frag_depth = EncodeFloatRGBA(gl_FragCoord.z);
	// Set normal, if it exists
	frag_normal = vec4(0.0, 0.0, 0.0, 1.0);
	if (id == 1) { // if glowstick
		frag_normal.xyz = texture(normTex, outTexCoord).xyz;
	} else if (useNormTex == 1) {
		vec3 pre_norm = texture(normTex, outTexCoord).xzy;
		if (flipXfrag != 1) {
			pre_norm.x = 1.0 - pre_norm.x;
		}
		vec3 unencoded_normal = (pre_norm * 2.0) - 1.0;
		mat2 R = mat2(uDirection.y, uDirection.x, -uDirection.x, uDirection.y);
		unencoded_normal.xy = unencoded_normal.xy * R;
		frag_normal.xyz = (unencoded_normal + 1.0) / 2.0;
	}
}

/////////// SHADER END //////////)"
