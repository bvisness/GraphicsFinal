#version 150

#define MAX_LIGHTS 10

#define LIGHT_DISABLED 0
#define LIGHT_POINT 1
#define LIGHT_SPOT 2
#define LIGHT_DIRECTIONAL 3

#define HAS_TEX_DIFFUSE 1
#define HAS_TEX_SPEC 2
#define HAS_TEX_NORM 4

#define MIN_SPEC_EXP 1

in vec4 fPosEye;
in vec3 fNormal;
in vec3 fTangent;
flat in float fBitangentSign;
in vec2 fTexCoord;
in vec4 fAmbientDiffuseColor;
in vec4 fSpecularColor;
in float fDiffuseAmount;
in float fSpecularAmount;
in float fSpecularExponent;

out vec4 fColor;

uniform int uLightType[MAX_LIGHTS];
uniform vec4 uLightPosition[MAX_LIGHTS];
uniform vec4 uLightDirection[MAX_LIGHTS];
uniform float uLightSpotAngleCos[MAX_LIGHTS];
uniform vec4 uLightColor[MAX_LIGHTS];
uniform vec4 uAmbientLight;
uniform int uTextureMode;
uniform sampler2D uDiffuseTexture;
uniform sampler2D uSpecularTexture;
uniform sampler2D uNormalMap;

bool hasDiffuseTexture() {
	return (uTextureMode & HAS_TEX_DIFFUSE) != 0;
}

bool hasSpecularTexture() {
	return (uTextureMode & HAS_TEX_SPEC) != 0;
}

bool hasNormalMap() {
	return (uTextureMode & HAS_TEX_NORM) != 0;
}

vec4 getAmbientDiffuseColor() {
	vec4 result = fAmbientDiffuseColor;
	if (hasDiffuseTexture()) {
		result *= texture2D(uDiffuseTexture, fTexCoord);
	}
	return result;
}

vec4 getSpecularColor() {
	vec4 result;
	if (hasSpecularTexture()) {
		result = vec4(texture2D(uSpecularTexture, fTexCoord).rgb, 1);
	} else {
		result = fSpecularColor;
	}
	return result * getAmbientDiffuseColor().a;
}

float getSpecularExponent() {
	if (hasSpecularTexture()) {
		return (texture2D(uSpecularTexture, fTexCoord).a * (fSpecularExponent - MIN_SPEC_EXP)) + MIN_SPEC_EXP;
	} else {
		return fSpecularExponent;
	}
}

vec3 getNormalVector() {
	vec3 fN = normalize(fNormal);
	if (!hasNormalMap()) {
		return fN;
	}

	vec3 fT = normalize(fTangent);
	vec3 fB = fBitangentSign * cross(fN, fT);
	mat4 toEyeSpace = mat4(
		vec4(fT, 0),
		vec4(fB, 0),
		vec4(fN, 0),
		vec4(0, 0, 0, 1)
	);

	vec3 fNOffset = (texture2D(uNormalMap, fTexCoord) * 2).xyz - vec3(1, 1, 1);
	fNOffset = (toEyeSpace * vec4(fNOffset, 0)).xyz;

	return fNOffset;
}

void main()
{
    vec4 amb = getAmbientDiffuseColor() * uAmbientLight;
    vec4 diff = vec4(0, 0, 0, 0);
    vec4 spec = vec4(0, 0, 0, 0);

	vec3 fV = normalize(-fPosEye.xyz);
	vec3 fN = getNormalVector();
    
    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (uLightType[i] == 0) {
            continue;
        }
        
		vec3 fL;
		if (uLightType[i] == LIGHT_DIRECTIONAL) {
			fL = -normalize(uLightDirection[i].xyz);
		} else {
			fL = normalize(uLightPosition[i].xyz - fPosEye.xyz);
		}
        vec3 fH = normalize(fL + normalize(-fPosEye.xyz));

		bool isLit = true;
		if (uLightType[i] == LIGHT_SPOT) {
			isLit = dot(normalize(uLightDirection[i].xyz), -fL) >= uLightSpotAngleCos[i];
		}

		if (isLit) {
			diff += max(0, dot(fN, fL)) * fDiffuseAmount * getAmbientDiffuseColor() * uLightColor[i];
			if (dot(fL, fN) >= 0) {
				spec += pow(max(0, dot(fN, fH)), getSpecularExponent()) * fSpecularAmount * getSpecularColor() * uLightColor[i];
			}
		}
	}
	
    fColor = amb + diff + spec;
    //fColor = amb;
    //fColor = diff;
    //fColor = spec;
	//fColor = texture2D(uTexture, fTexCoord);
    //fColor = vec4(fN, 1);
	//fColor = vec4(normalize(fTangent), 1);
    //fColor = vec4(diffuseAmount, diffuseAmount, diffuseAmount, 1);
    //fColor = vec4(specularAmount, specularAmount, specularAmount, 1);
	//fColor = getSpecularColor();
	//fColor = vec4(dot(N[0], H[0]), dot(N[0], H[0]), dot(N[0], H[0]), 1);
    //fColor = vec4(getSpecularExponent() / fSpecularExponent, getSpecularExponent() / fSpecularExponent, getSpecularExponent() / fSpecularExponent, 1);
	//fColor = uLightColor[0];
	//fColor = vec4(uLightSpotAngleCos[0], uLightSpotAngleCos[0], uLightSpotAngleCos[0], 1);
	//fColor = vec4(dot(uLightDirection[0].xyz, -L[0]), dot(uLightDirection[0].xyz, -L[0]), dot(uLightDirection[0].xyz, -L[0]), 1);
	//fColor = vec4((L[0].r + 1) / 2, (L[0].g + 1) / 2, (L[0].b + 1) / 2, 1);
	//fColor = vec4(max(0, dot(getNormalVector(), normalize(normalize(uLightPosition[0].xyz - fPosEye.xyz) + normalize(-fPosEye.xyz)))), max(0, dot(getNormalVector(), normalize(normalize(uLightPosition[0].xyz - fPosEye.xyz) + normalize(-fPosEye.xyz)))), max(0, dot(getNormalVector(), normalize(normalize(uLightPosition[0].xyz - fPosEye.xyz) + normalize(-fPosEye.xyz)))), 1);
	//fColor = vec4(pow(max(0, dot(getNormalVector(), normalize(normalize(uLightPosition[0].xyz - fPosEye.xyz) + normalize(-fPosEye.xyz)))), getSpecularExponent()), pow(max(0, dot(getNormalVector(), normalize(normalize(uLightPosition[0].xyz - fPosEye.xyz) + normalize(-fPosEye.xyz)))), getSpecularExponent()), pow(max(0, dot(getNormalVector(), normalize(normalize(uLightPosition[0].xyz - fPosEye.xyz) + normalize(-fPosEye.xyz)))), getSpecularExponent()), 1);
}
