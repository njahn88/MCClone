#include "Game.h"

/*

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
*/

int main()
{
    
    GameSpecifications gameSpecifications;
    gameSpecifications.m_windowHeight = 800;
    gameSpecifications.m_windowWidth = 800;
    gameSpecifications.m_frameRateLimit = 60;
    gameSpecifications.m_gameName = "New Game";


    Game game{ gameSpecifications };
    game.Run();


    /*
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
        */
}