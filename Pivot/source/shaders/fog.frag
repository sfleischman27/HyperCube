R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec2 outTexCoord;

layout (location = 0) out vec4 frag_color_r;
layout (location = 1) out vec4 frag_color_g;
layout (location = 2) out vec4 frag_color_b;

uniform float farPlaneDist;
uniform sampler2D depthTexture;
uniform sampler2D replaceTexture;

// Editable parameter for depth falloff. Higher = fades out sooner
const float severity = 50.0;

vec4 EncodeFloatRGBA(float v) {
    vec4 enc = vec4(1.0, 255.0, 65025.0, 16581375.0) * v;
    enc = fract(enc);
    enc -= enc.yzww * vec4(1.0/255.0, 1.0/255.0, 1.0/255.0, 0.0);
    return enc;
}

float DecodeFloatRGBA(vec4 rgba) {
  return dot( rgba, vec4(1.0, 1.0/255.0, 1.0/65025.0, 1.0/16581375.0) );
}

void main(void) {
    // Do not calculate if this area is cut
    if (texture(replaceTexture, outTexCoord).r == 0.0) {
        discard;
    }
	float d = DecodeFloatRGBA(texture(depthTexture, outTexCoord)) * farPlaneDist * severity;
    vec4 frag_color;
	frag_color.xyz = vec3(1.0) * d;
	frag_color.a = 1.0;
    
    frag_color_r = EncodeFloatRGBA(frag_color.r);
    frag_color_g = EncodeFloatRGBA(frag_color.g);
    frag_color_b = EncodeFloatRGBA(frag_color.b);
}

/////////// SHADER END //////////)"
