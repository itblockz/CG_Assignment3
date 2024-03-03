#undef GLFW_DLL
#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <cmath>

#include "Libs/Shader.h"
#include "Libs/Window.h"
#include "Libs/Mesh.h"
#include "Libs/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const GLint WIDTH = 800, HEIGHT = 600;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;

float yaw = -90.0f, pitch = 0.0f;

glm::vec3 lightColour = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 lightPos = glm::vec3(5.0f, 5.0f, 0.0f);

//Vertex Shader
static const char* vShader = "Shaders/shader.vert";

//Fragment Shader
static const char* fShader = "Shaders/shader.frag";

//Vertex Shader
static const char* lightVShader = "Shaders/lightShader.vert";

//Fragment Shader
static const char* lightFShader = "Shaders/lightShader.frag";

void CreateTriangle()
{
    GLfloat vertices[] =
    {
        // pos                  //aTexCoord
        -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, -1.0f, 1.0f,      0.5f, 0.0f,
        1.0f, -1.0f, 0.0f,      1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,       0.5f, 1.0f
    };

    unsigned int indices[] = 
    {
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2,
    };

    Mesh *obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 20, 12);
        
    for (int i = 0; i < 10; i++)
    {
        meshList.push_back(obj1);
    }
    
}

void CreateOBJ()
{
    Mesh *obj1 = new Mesh();
    bool loaded = obj1->CreateMeshFromOBJ("Models/suzanne.obj");
    if (loaded) 
    {
        for (int i = 0; i < 10; i++)
        {
            meshList.push_back(obj1);
        }
    }
    else std::cout << "Failed to load model" << std::endl;

    Mesh *obj2 = new Mesh();
    loaded = obj2->CreateMeshFromOBJ("Models/cube.obj");
    if (loaded) 
    {
        meshList.push_back(obj2);
    }
    else std::cout << "Failed to load model" << std::endl;
}

void CreateShaders()
{
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(shader1);
    
    Shader* shader2 = new Shader();
    shader2->CreateFromFiles(lightVShader, lightFShader);
    shaderList.push_back(shader2);
}

void checkMouse() {
    double xpos, ypos;

    glfwGetCursorPos(mainWindow.getWindow(), &xpos, &ypos);

    static float lastX = xpos;
    static float lastY = ypos;

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    pitch += yoffset;
    yaw += xoffset;

    pitch = glm::clamp(pitch, -89.0f, 89.0f);
}

int main()
{
    mainWindow = Window(WIDTH, HEIGHT, 3, 3);
    mainWindow.initialise();

    CreateOBJ();
    CreateShaders();

    GLuint uniformModel = 0, uniformProjection = 0, uniformView = 0;

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPos);
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraDirection, up));
    glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));

    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth()/(GLfloat)mainWindow.getBufferHeight(),0.1f, 100.0f);
    // glm::mat4 projection = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.1f, 100.0f);


    float deltaTime, lastFrame;

    //texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // glBindTexture(GL_TEXTURE_2D, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("Textures/uvmap.png", &width, &height, &nrChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);

    //Loop until window closed
    while (!mainWindow.getShouldClose())
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //Get + Handle user input events
        glfwPollEvents();
        
        checkMouse();

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        cameraDirection = glm::normalize(direction);
        cameraRight = glm::normalize(glm::cross(cameraDirection, up));
        cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));

        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_W) == GLFW_PRESS)
        {
            cameraPos += cameraDirection * deltaTime * 5.0f;
        }
        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_S) == GLFW_PRESS)
        {
            cameraPos += -cameraDirection * deltaTime * 5.0f;
        }
        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_A) == GLFW_PRESS)
        {
            cameraPos += -cameraRight * deltaTime * 5.0f;
        }
        if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_D) == GLFW_PRESS)
        {
            cameraPos += cameraRight * deltaTime * 5.0f;
        }

        //Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // glDisable(GL_DEPTH_TEST);

        glm::vec3 pyramidPositions[] =
        {
            glm::vec3(0.0f, 0.0f, -2.5f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
        };

        glm::mat4 view (1.0f);

        glm::mat4 cameraRotateMat (1.0f);

        cameraRotateMat[0] = glm::vec4(cameraRight.x, cameraUp.x, -cameraDirection.x, 0);
        cameraRotateMat[1] = glm::vec4(cameraRight.y, cameraUp.y, -cameraDirection.y, 0);
        cameraRotateMat[2] = glm::vec4(cameraRight.z, cameraUp.z, -cameraDirection.z, 0);

        glm::mat4 cameraPosMat (1.0f);
        cameraPosMat[3][0] = -cameraPos.x;
        cameraPosMat[3][1] = -cameraPos.y;
        cameraPosMat[3][2] = -cameraPos.z;

        view = cameraRotateMat * cameraPosMat;

        // view = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);

        //draw here
        shaderList[0]->UseShader();
        uniformModel = shaderList[0]->GetUniformLocation("model");
        uniformProjection = shaderList[0]->GetUniformLocation("projection");
        uniformView = shaderList[0]->GetUniformLocation("view");

        //light
        glUniform3fv(shaderList[0]->GetUniformLocation("lightColour"), 1, (GLfloat*) &lightColour);
        glUniform3fv(shaderList[0]->GetUniformLocation("lightPos"), 1, (GLfloat*) &lightPos);
        glUniform3fv(shaderList[0]->GetUniformLocation("viewPos"), 1, (GLfloat*) &cameraPos);
        
        for (int i = 0; i < 10; i++)
        {
            glm::mat4 model (1.0f);

            model = glm::translate(model, pyramidPositions[i]);
            model = glm::scale(model, glm::vec3(0.8f, 0.8f, 1.0f));
            
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));

            //Object
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            meshList[i]->RenderMesh();
        }

        shaderList[1]->UseShader();
        uniformModel = shaderList[1]->GetUniformLocation("model");
        uniformProjection = shaderList[1]->GetUniformLocation("projection");
        uniformView = shaderList[1]->GetUniformLocation("view");

        glUniform3fv(shaderList[1]->GetUniformLocation("lightColour"), 1, (GLfloat*) &lightColour);

        glm::mat4 model (1.0f);
        model = glm::translate(model, lightPos);
        
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));

        meshList[10]->RenderMesh();

        glUseProgram(0);
        //end draw

        mainWindow.swapBuffers();
    }

    return 0;
}
