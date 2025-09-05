#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include "shader.h"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <queue>

#include <map>
#include <array>
#include <mutex>
#include "Block.h"
#include "Chunk.h"
#include "ChunkManager.h"
#include "Perlin.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float lastX = 400, lastY = 300;

glm::vec3* cameraPos = new glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float yaw = -90.0f;
bool firstMouse = true;
float pitch = 0.0f;

struct Vertex {
public:
    Vertex(glm::vec3 position, glm::vec2 textCoords) : m_position(position), m_textCoords(textCoords) {};
    glm::vec3 m_position;
    glm::vec2 m_textCoords;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
unsigned int LoadTextureAtlas();


int main()
{
    Perlin::Init();

    //Init glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(0);

    //Load OpenGl function pointers with glad
    if (!gladLoaderLoadGL())
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("3.3.shader.vs", "3.3.shader.fs");

    //perpective projection matrix
    //-------------------------------------
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

    //model matrix
    //-------------------------------------
    glm::mat4 model = glm::mat4(1.0f);
    //view matrix
    //--------------------------------------
    glm::mat4 view;
    view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    //camera position
    //--------------------------------------

    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraDirection = glm::normalize(*cameraPos - cameraTarget);

    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraUp, cameraDirection));

    glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

    

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = { //vert data for 0 == verts[0 - 4] == one vert, 1 == verts[5-9] == one vert
        // back face
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // bottom-left
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right    
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, // top-right              
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, // top-right
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, // top-left
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // bottom-left     
        // front face
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, // bottom-right        
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, // bottom-left
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // top-left 
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // top-left 
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, // bottom-right        
        // left face
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, // top-left       
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, // bottom-right
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, // bottom-right
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, // top-left       
        // right face
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, // top-right      
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right          
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, // bottom-left
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
         // bottom face          
         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
          0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
          0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left        
          0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
         // top face
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right                 
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
         -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // bottom-left  
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f  // top-left      
    };

    size_t count = sizeof(vertices) / sizeof(vertices[0]);


    int digCount = 0;


    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);




    unsigned int g_TextureAtlasID = 0;
    g_TextureAtlasID = LoadTextureAtlas();


    ChunkManager chunkManager{vertices, g_TextureAtlasID, 20, cameraPos};



    ourShader.use();
    int frameCount = 0;
    double previousTime = glfwGetTime();


    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //processInput(window, chunkManager);
        double currentTime = glfwGetTime();
        frameCount++;
        if (currentTime - previousTime >= 1.0) {
            std::cout << "FPS: " << frameCount << std::endl;
            frameCount = 0;
            previousTime = currentTime;
        }


        ourShader.use();

        glm::mat4 view = glm::lookAt(*cameraPos, *cameraPos + cameraFront, cameraUp);
        ourShader.setMat4("view", view);
        ourShader.setMat4("model", model);
        ourShader.setMat4("projection", projection);

        chunkManager.Update();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    const float cameraSpeed = 30.0f * deltaTime; // camera speed based on frame rate
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        *cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        *cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        *cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        *cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        *cameraPos += cameraSpeed * glm::vec3(0, 1, 0);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        *cameraPos += cameraSpeed * glm::vec3(0, -1, 0);
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);

}

/*
Retrieves the texture atlas and asigns the texture
*/
unsigned int LoadTextureAtlas() {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("texture_atlas_2.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture!" << std::endl;
        std::cout << stbi_failure_reason() << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}