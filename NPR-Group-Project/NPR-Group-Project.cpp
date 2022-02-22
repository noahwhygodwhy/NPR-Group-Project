#include <iostream>
#include <vector>

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>`
#include "Shader.hpp"
#include "Model.hpp"

using namespace std;
using namespace glm;


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, 1);
    glfwWindowHint(GLFW_SAMPLES, 16);
    GLFWwindow* window = glfwCreateWindow(1000, 1000, "Title Goes here", NULL, NULL);

    if (window == NULL)
    {
        cout << "Window creation failed" << endl;
        exit(-1);
    }

    glfwMakeContextCurrent(window);




    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "GLAD init failed" << endl;
        exit(-1);
    }


    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);


    Model m("backpack");
    Shader objectShader("object.vert", "object.frag");



    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();


        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        mat4 projection = glm::perspective(radians(70.0), 1.0, 0.1, 100.0);
        vec3 eye = vec3(sin(currentTime) * 5.0, 0.0, cos(currentTime) * 5.0);
        mat4 view = glm::lookAt(eye, vec3(0.0), vec3(0.0, 1.0, 0.0));

        objectShader.use();
        objectShader.setMatFour("projection", projection);
        objectShader.setMatFour("view", view);

        m.draw(objectShader);



        //processInput(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}