#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include "ModelLoader.h"

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

struct Component {};

struct RenderComponent : public Component{
    unsigned int m_Vbo, m_Vao;
    std::vector<float>* m_model{};
    bool m_modelUpdated = false;
};

struct System {
public:
    virtual void Update() = 0;
};

struct RenderSystem : public System{
private:
    std::vector<RenderComponent*> m_renderables{};
public:
    void AddComponent(RenderComponent* renderable) {
        glGenVertexArrays(1, &renderable->m_Vao);
        glBindVertexArray(renderable->m_Vao);

        glGenBuffers(1, &renderable->m_Vbo);
        glBindBuffer(GL_ARRAY_BUFFER, renderable->m_Vbo);

        // assume layout = position(3), normal(3), texcoord(2)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        renderable->m_modelUpdated = true;
        m_renderables.push_back(renderable);
    }

    void Update() override {
        for (RenderComponent* renderComponent : m_renderables) {
            unsigned int vbo = renderComponent->m_Vbo;
            unsigned int vao = renderComponent->m_Vao;
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            std::vector<float>* model = renderComponent->m_model;


            if (renderComponent->m_modelUpdated) {
                glBufferData(GL_ARRAY_BUFFER, model->size() * sizeof(float), model->data(), GL_STATIC_DRAW);
                renderComponent->m_modelUpdated = false;
            }
            glDrawArrays(GL_TRIANGLES, 0, model->size() / 8);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            //delete model;
        }
    }
};

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

    //Load OpenGl function pointers with glad
    if (!gladLoaderLoadGL())
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

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

    ModelLoader modelLoader("Models/");
    std::vector<float> boxModel = modelLoader.GetModel(Model::Box);

    RenderSystem renderSystem{};

    RenderComponent renderComponent{};
    renderComponent.m_model = &boxModel;


    /*
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    */


    renderSystem.AddComponent(&renderComponent);

    ourShader.use();
    int frameCount = 0;
    double previousTime = glfwGetTime();


    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        double currentTime = glfwGetTime();
        frameCount++;
        if (currentTime - previousTime >= 1.0) {
            std::cout << "FPS: " << frameCount << std::endl;
            frameCount = 0;
            previousTime = currentTime;
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        const float radius = 10.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        view = glm::lookAt(*cameraPos, *cameraPos + cameraFront, cameraUp);


        ourShader.use();

        glm::mat4 view = glm::lookAt(*cameraPos, *cameraPos + cameraFront, cameraUp);
        ourShader.setMat4("view", view);


        int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        //glBindVertexArray(VAO);

        renderSystem.Update();
        /*
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, boxModel.size() * sizeof(float), boxModel.data(), GL_STATIC_DRAW);

        glDrawArrays(GL_TRIANGLES, 0, boxModel.size() / 8);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        */

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //glDeleteVertexArrays(1, &VAO);
    //glDeleteBuffers(1, &VBO);

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