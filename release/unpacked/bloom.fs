#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;

// Input uniform values
uniform sampler2D texture0;

// Output fragment color
out vec4 fragColor;

// NOTE: Add here your custom variables

const float blurSize = 1.0/512.0;
void main()
{
   vec4 sum = vec4(0);
   vec2 texcoord = fragTexCoord;
   int j;
   int i;

   //thank you! http://www.gamerendering.com/2008/10/11/gaussian-blur-filter-shader/ for the 
   //blur tutorial
   // blur in y (vertical)
   // take nine samples, with the distance blurSize between them
   sum += texture(texture0, vec2(texcoord.x - 4.0*blurSize, texcoord.y)) * 0.05;
   sum += texture(texture0, vec2(texcoord.x - 3.0*blurSize, texcoord.y)) * 0.09;
   sum += texture(texture0, vec2(texcoord.x - 2.0*blurSize, texcoord.y)) * 0.12;
   sum += texture(texture0, vec2(texcoord.x - blurSize, texcoord.y)) * 0.15;
   sum += texture(texture0, vec2(texcoord.x, texcoord.y)) * 0.16;
   sum += texture(texture0, vec2(texcoord.x + blurSize, texcoord.y)) * 0.15;
   sum += texture(texture0, vec2(texcoord.x + 2.0*blurSize, texcoord.y)) * 0.12;
   sum += texture(texture0, vec2(texcoord.x + 3.0*blurSize, texcoord.y)) * 0.09;
   sum += texture(texture0, vec2(texcoord.x + 4.0*blurSize, texcoord.y)) * 0.05;
	
	// blur in y (vertical)
   // take nine samples, with the distance blurSize between them
   sum += texture(texture0, vec2(texcoord.x, texcoord.y - 4.0*blurSize)) * 0.05;
   sum += texture(texture0, vec2(texcoord.x, texcoord.y - 3.0*blurSize)) * 0.09;
   sum += texture(texture0, vec2(texcoord.x, texcoord.y - 2.0*blurSize)) * 0.12;
   sum += texture(texture0, vec2(texcoord.x, texcoord.y - blurSize)) * 0.15;
   sum += texture(texture0, vec2(texcoord.x, texcoord.y)) * 0.16;
   sum += texture(texture0, vec2(texcoord.x, texcoord.y + blurSize)) * 0.15;
   sum += texture(texture0, vec2(texcoord.x, texcoord.y + 2.0*blurSize)) * 0.12;
   sum += texture(texture0, vec2(texcoord.x, texcoord.y + 3.0*blurSize)) * 0.09;
   sum += texture(texture0, vec2(texcoord.x, texcoord.y + 4.0*blurSize)) * 0.05;

   //increase blur with intensity!
   //fragColor = sum*intensity + texture(texture0, texcoord); 
	   fragColor = sum * 0.4 + texture(texture0, texcoord) * 0.5;
}