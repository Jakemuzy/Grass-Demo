#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4 instanceMatrix;
layout (location = 5) in vec3 colorsMatrix;

uniform mat4 model; 
uniform mat4 view;  
uniform mat4 projection; 

//Facing player
uniform vec3 cameraPos;       //ADD WIND DIRECTION
uniform float time;
uniform vec3 grassColor;
uniform vec3 grassTipColor;

out vec3 aCol;

void main()
{
    
    //Rotate towards Camera
    vec3 instancePos = vec3(instanceMatrix[3]);  // extract position from matrix
    vec3 rotationDir = normalize(cameraPos - instancePos);
    vec3 forward = normalize(rotationDir); 

    vec3 right = normalize(cross(vec3(0.0, 1.0, 0.0), forward));  // Right vector (X+)
    vec3 up = normalize(cross(forward, right));  // Up vector (Y+)

    mat3 rotation = mat3(right, up, forward);
    vec3 rotatedPos = rotation * aPos;

    vec4 worldPosition = instanceMatrix * vec4(rotatedPos, 1.0);

    gl_Position = projection * view * worldPosition;
    aCol = colorsMatrix + grassColor;
    
    if(rotatedPos.y > 0.5f){
        vec3 newCol = colorsMatrix;
        newCol[1] -= 0.5f;
        aCol = newCol + grassTipColor;

        //  Movement on top of the blades, based on time plus scale
        mat4 movement = mat4(1.0f);
        movement[3][0] = sin(time + rotatedPos.y * 5.0) * 0.1;
        movement[3][2] = cos(time + rotatedPos.y * 5.0) * 0.1;
        gl_Position = projection * view * instanceMatrix  *  movement * vec4(rotatedPos, 1.0);
    }
        
}  