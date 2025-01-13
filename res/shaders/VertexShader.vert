#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in mat4 instanceMatrix;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;  

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
void main()
{
	if (model[0][0] == 0.0f)
	{
		gl_Position = projection * view *  instanceMatrix * vec4(aPos, 1.0);
		FragPos = vec3(instanceMatrix * vec4(aPos, 1.0));
	}
	else
	{
		gl_Position = projection * view *  model * vec4(aPos, 1.0);
		FragPos = vec3(model * vec4(aPos, 1.0));
	}
	
   
   
   TexCoord = aTexCoord;
   if (model[0][0] == 0.0f)
   {
   Normal = mat3(transpose(inverse(instanceMatrix))) * aNormal; 
   }
   else
   {
   Normal = mat3(transpose(inverse(model))) * aNormal; 
   }
	 
}