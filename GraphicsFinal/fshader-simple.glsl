#version 430

in vec4 fPosEye;
in vec4 fAmbientDiffuseColor;

out vec4 fColor;

void main()
{
	fColor = fAmbientDiffuseColor;
	//fColor = vec4(1, 1, 1, 1);
}