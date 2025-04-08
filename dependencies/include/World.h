#include <FastNoiseLite.h>
#include <chrono>
#include <iostream>

class World {
private:
    int sizeX = 20, sizeZ = 20;
    int seed = 1337;
    FastNoiseLite noise;

    std::vector<float> vertices = {};
    unsigned int VBO, VAO;

public:
    World() {

        srand(time(NULL));
        int seed = rand();

        noise.SetSeed(seed);
        noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
        noise.SetFractalType(FastNoiseLite::FractalType_FBm);
        noise.SetFrequency(0.036);
        noise.SetFractalOctaves(8.0);

        vertices.reserve(sizeX * sizeZ * 3);

        for(int x = 0; x < sizeX - 1; x++){
            for(int z = 0; z < sizeZ - 1; z++){
                float y1 = (noise.GetNoise((float)x, (float)z) + 1.0f) * 0.5f * 10.0f;
                float y2 = (noise.GetNoise((float)x + 1, (float)z) + 1.0f) * 0.5f * 10.0f;
                float y3 = (noise.GetNoise((float)x, (float)z + 1) + 1.0f) * 0.5f * 10.0f;
                float y4 = (noise.GetNoise((float)x + 1, (float)z + 1) + 1.0f) * 0.5f * 10.0f;


                // Triangle 1
                vertices.push_back(x);
                vertices.push_back(y1);
                vertices.push_back(z);
                vertices.push_back(x + 1);
                vertices.push_back(y2);
                vertices.push_back(z);
                vertices.push_back(x);
                vertices.push_back(y3);
                vertices.push_back(z + 1);

                // Triangle 2
                vertices.push_back(x + 1);
                vertices.push_back(y2);
                vertices.push_back(z);
                vertices.push_back(x + 1);
                vertices.push_back(y4);
                vertices.push_back(z + 1);
                vertices.push_back(x);
                vertices.push_back(y3);
                vertices.push_back(z + 1);
            }
        }
        
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
    }

    void draw(){
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
    }

    std::vector<float> getVertices(){
        return vertices;
    }
};

class Grass {
private:
    unsigned int VBO, VAO, instanceVBO;
    float grassBlade[9] = {
        0, 0, 0,
        0.05f, 0, 0,
        0.015f, 1.0f, 0
    };
    int sizeX = 10, sizeZ = 10;
    int bladeCount;
public:
    glm::mat4 *modelMatrices;
    glm::vec3 *colors;

    //Blades are the density
    Grass(int blades)
    {
        bladeCount = blades * sizeX * sizeZ;

        //  Generate original blade

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), &grassBlade[0], GL_STATIC_DRAW);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);


        //  Generate transformations for instanced blades
        srand(time(NULL));
        modelMatrices = new glm::mat4[bladeCount];
        colors = new glm::vec3[bladeCount];

        int matrixIndex = 0;

        for(int x = 0; x < sizeX; x++){
            for (int z = 0; z < sizeZ; z++)
            {
                glm::vec3 worldOffset = glm::vec3((float)x, 0, (float)z);

                for (int i = 0; i < blades; i++)
                {
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, worldOffset);

                    //  Scale
                    float scale = (rand() % 10 + 4) / 10.0f;
                    glm::vec3 scalar = glm::vec3(scale);
                    scalar.x = 1.0f;
                    scalar.z = 1.0f;

                    model = glm::scale(model, scalar);

                    //  Offset
                    float offsetX, offsetY;
                    offsetX = (rand() % 50 + 10) / 40.0f;
                    offsetY = (rand() % 50 + 10) / 40.0f;

                    model = glm::translate(model, glm::vec3(offsetX, 0, offsetY));
                    modelMatrices[matrixIndex] = model;

                    //  Color
                    float greenOffset = (rand() % 15) / 100.0f;
                    glm::vec3 color = glm::vec3(0 ,greenOffset, 0);
                    colors[matrixIndex] = color;

                    matrixIndex++;
                }
            }
        }

        //Logic for instancing the models
        unsigned int modelsBuffer;
        glGenBuffers(1, &modelsBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, modelsBuffer);
        glBufferData(GL_ARRAY_BUFFER, bladeCount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

        for(int i = 0; i < bladeCount; i++){
            glBindVertexArray(VAO);

            std::size_t vec4Size = sizeof(glm::vec4);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)0);
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(1 * vec4Size));
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(2 * vec4Size));
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(3 * vec4Size));

            glVertexAttribDivisor(1, 1);
            glVertexAttribDivisor(2, 1);
            glVertexAttribDivisor(3, 1);
            glVertexAttribDivisor(4, 1);

            glBindVertexArray(0);
        }

        unsigned int colorBuffer;
        glGenBuffers(1, &colorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, bladeCount * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);

        glBindVertexArray(VAO);

        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
        glVertexAttribDivisor(5, 1);

        glBindVertexArray(0);

    }

    void draw()
    {
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 3, bladeCount);
    }

};