#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.hpp"
#include "Model.hpp"
#include "Screen.hpp"
#include "DTriangle.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;
using namespace glm;


int stageUpTo = 7;
constexpr int maxStage = 9;

double deltaTime = 0.0f;	// Time between current frame and last frame
double lastFrame = 0.0f; // Time of last frame

int screenX = 1000;
int screenY = 1000;

int numberOfPoints = 200;


//drawn framebuffer texture
struct DFT {

    uint32_t framebuffer;
    uint32_t texture;
    uint32_t RBO;
};


DFT depthDFT;
DFT normalDFT;
DFT colorDFT;
DFT lineDFT;
DFT pointDFT;
DFT coordDFT;
DFT veronoiDFT;

DFT stage1;
DFT stage2;
DFT stage3;


uint32_t stage4Framebuffer;
uint32_t stage4Texture1;
uint32_t stage4Texture2;
uint32_t stage4Texture3;
uint32_t stage4RBO;



void regenFrameBuffer(DFT& dft, int width = screenX, int height = screenY) {

    glDeleteRenderbuffers(1, &dft.RBO);
    glDeleteFramebuffers(1, &dft.framebuffer);

    glGenFramebuffers(1, &dft.framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, dft.framebuffer);
    glGenTextures(1, &dft.texture);
    glBindTexture(GL_TEXTURE_2D, dft.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_LINEAR
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dft.texture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &dft.RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, dft.RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dft.RBO);
}



void genFrameBuffers() {
    regenFrameBuffer(depthDFT);
    regenFrameBuffer(normalDFT);
    regenFrameBuffer(colorDFT);
    regenFrameBuffer(lineDFT);
    regenFrameBuffer(pointDFT, numberOfPoints, 1);
    regenFrameBuffer(veronoiDFT);

    regenFrameBuffer(stage1);
    regenFrameBuffer(stage2);
    regenFrameBuffer(stage3);

    glDeleteRenderbuffers(1, &stage4RBO);
    glDeleteFramebuffers(1, &stage4Framebuffer);

    glGenFramebuffers(1, &stage4Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, stage4Framebuffer);

    glGenTextures(1, &stage4Texture1);
    glBindTexture(GL_TEXTURE_2D, stage4Texture1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenX, screenY, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_LINEAR
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, stage4Texture1, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &stage4Texture2);
    glBindTexture(GL_TEXTURE_2D, stage4Texture2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenX, screenY, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_LINEAR
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, stage4Texture2, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &stage4Texture3);
    glBindTexture(GL_TEXTURE_2D, stage4Texture3);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenX, screenY, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_LINEAR
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, stage4Texture3, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &stage4RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, stage4RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, screenX, screenY);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, stage4RBO);

    
}

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {


    screenX = width;
    screenY = height;
    glViewport(0, 0, width, height);
    genFrameBuffers();
}void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {

        stageUpTo = (stageUpTo + 1) % (maxStage+1);
        if (stageUpTo < 0) {
            stageUpTo = maxStage;
        }
        printf("stageupto: %i\n", stageUpTo);
    }

    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        stageUpTo = (stageUpTo - 1) % (maxStage+1);
        if (stageUpTo < 0) {
            stageUpTo = maxStage;
        }
        printf("stageupto: %i\n", stageUpTo);
    }

}

vec4 calculatePolarPoints(const vec3& point, const vec2& polarCenter) {
    vec2 transPoint = vec2(point.x - 0.5, point.y - 0.5);
    double theta = atan2(transPoint.x, transPoint.y);
    if (point.y < 0.0) {
        theta += (2.0 * glm::pi<double>());
    }
    double r = length(transPoint);
     
    return vec4(point.x, point.y, r, theta);
}

//TODO: use texelfetch not texture()
unordered_map<string, Shader*> shaders;
Shader* shader;
int main()
{


    /*printf("%f\n", glm::dot(vec2(0.0, 1.0), glm::normalize(vec2(1.0, 1.0))));
    printf("%f\n", glm::dot(vec2(0.0, 1.0), glm::normalize(vec2(1.0, 0.0))));
    printf("%f\n", glm::dot(vec2(0.0, 1.0), glm::normalize(vec2(1.0, -1.0))));
    printf("%f\n", glm::dot(vec2(0.0, 1.0), glm::normalize(vec2(0.0, -1.0))));
    printf("%f\n", glm::dot(vec2(0.0, 1.0), glm::normalize(vec2(-1.0, -1.0))));
    printf("%f\n", glm::dot(vec2(0.0, 1.0), glm::normalize(vec2(-1.0, 0.0))));
    printf("%f\n", glm::dot(vec2(0.0, 1.0), glm::normalize(vec2(-1.0, 1.0))));
    printf("%f\n", glm::dot(vec2(0.0, 1.0), glm::normalize(vec2(0.0, 1.0))));

    exit(-1);*/
    /*double x = 1;
    double y = -1;

    double z = atan2(y, x);

    if (y < 0.0) {
        z += (2.0 * glm::pi<double>());
    }

    printf("%f\n", glm::degrees(z));

    exit(-1);

    */
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
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
    frameBufferSizeCallback(window, screenX, screenY);

    glfwSetKeyCallback(window, keyCallback);

    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);

    Screen flatscreen;

    vector<Model*> models;
    models.push_back(new Model("backpack"));

    shaders["object"] = new Shader("object.vert", "object.frag");
    shaders["regular"] = new Shader("vertShader.glsl", "fragShader.glsl");
    shaders["normal"] = new Shader("vertShader.glsl", "normalFragShader.glsl");
    shaders["depth"] = new Shader("vertShader.glsl", "depthFragShader.glsl");
    shaders["simpleShader"] = new Shader("simpleVertShader.glsl", "simpleFragShader.glsl");
    shaders["line"] = new Shader("lineVertShader.glsl", "lineFragShader.glsl");
    shaders["point"] = new Shader("pointVertShader.glsl", "pointFragShader.glsl");
    shaders["coord"] = new Shader("tempPointDisplayVertShader.glsl", "tempPointDisplayFragShader.glsl");
    shaders["veronoi"] = new Shader("veronoiVertShader.glsl", "veronoiFragShader.glsl");
    shaders["vPoints"] = new Shader("veronPointVertShader.glsl", "veronPointFragShader.glsl");
    shaders["stage1"] = new Shader("Stage1VertShader.glsl", "Stage1FragShader.glsl");
    shaders["stage2"] = new Shader("Stage2VertShader.glsl", "Stage2FragShader.glsl");
    shaders["stage3"] = new Shader("Stage3VertShader.glsl", "Stage3FragShader.glsl");
    shaders["stage4"] = new Shader("Stage4VertShader.glsl", "Stage4FragShader.glsl");
   // shaders["vPoints"] = new Shader("veronPointVertShader.glsl", "veronPointFragShader.glsl");





    mat4 ortho = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);


    unsigned int fakeVAO, fakeEBO, fakeVBO;
    glGenVertexArrays(1, &fakeVAO);
    glGenBuffers(1, &fakeVBO);
    //glGenBuffers(1, &fakeEBO);
    glBindVertexArray(fakeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, fakeVBO);
    glBufferData(GL_ARRAY_BUFFER, screenX*screenY, NULL, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 1, GL_BYTE, GL_FALSE, sizeof(char), (void*)0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fakeEBO);
    glBindVertexArray(0);


    //const size_t numberOfPoints = (numberOfPoints) + 4;//+4 corners 

    GLfloat* pointTexturePixels = new GLfloat[numberOfPoints *3];
    vec4* polarPoints = new vec4[numberOfPoints];


    int frameCounter = -1;
    float frameTimes[30](0);
    int lastSecondFrameCount = -1;

    while (!glfwWindowShouldClose(window)) {
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        frameCounter++;
        if (int(currentFrame) > lastSecondFrameCount) {
            lastSecondFrameCount = int(currentFrame);
            float sum = 0;
            for (float f : frameTimes) {
                sum += f;
            }

            printf("fps: %f\n", sum / 30.0f);
        }
        frameTimes[frameCounter % 30] = 1.0f / float(deltaTime);


        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        /*shader = shaders["object"];
        shader.use();
        shader.setMatFour("projection", projection);
        shader.setMatFour("view", view);
        m.draw(shader);*/

        float sinit = (sin(currentFrame / 5.0f));
        float cosit = (cos(currentFrame / 5.0f));
        float ratio = (float)screenX / (float)screenY;

        //vec3 eye = vec3(sinit * 5.0f, 2.0f, cosit * 5.0f);
        vec3 eye = vec3(5.0, 5.0, 5.0);
        mat4 view = glm::lookAt(eye, vec3(0, 0, 0), vec3(0, 1, 0));

        //Draw the depth of the object
        if (stageUpTo > -1) {
            if (stageUpTo > 0) glBindFramebuffer(GL_FRAMEBUFFER, depthDFT.framebuffer);
            if (stageUpTo == 0) glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader = shaders["depth"];
            shader->use();
            shader->setVecThree("eyePos", eye);
            shader->setMatFour("view", view);
            for (Model* m : models) {
                float newNear = m->getNear(eye);
                float newFar = m->getFar(eye);
                shader->setMatFour("projection", glm::perspective(glm::radians(70.0f), ratio, glm::max(newNear, 0.01f), newFar));
                shader->setFloat("near", newNear);
                shader->setFloat("far", newFar);
                m->draw(shader);
            }
        }
        
        //draw the normal (really the direction) of the object

        if (stageUpTo > 0) {
            if (stageUpTo > 1) glBindFramebuffer(GL_FRAMEBUFFER, normalDFT.framebuffer);
            if (stageUpTo == 1) glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader = shaders["normal"];
            shader->use();
            shader->setVecThree("eyePos", eye);
            shader->setMatFour("view", view);
            for (Model* m : models) {
                float newNear = m->getNear(eye);
                float newFar = m->getFar(eye);
                shader->setMatFour("projection", glm::perspective(glm::radians(70.0f), ratio, glm::max(newNear, 0.01f), newFar));
                shader->setFloat("near", newNear);
                shader->setFloat("far", newFar);
                m->draw(shader);
            } 
        }

        //draw the object regularly,like it would be drawn normally, but i can't use the word normal

        if (stageUpTo > 1) {
            if (stageUpTo > 2) glBindFramebuffer(GL_FRAMEBUFFER, colorDFT.framebuffer);
            if (stageUpTo == 2) glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader = shaders["object"];
            shader->use();
            shader->setVecThree("eyePos", eye);
            shader->setMatFour("view", view);
            for (Model* m : models) {
                float newNear = m->getNear(eye);
                float newFar = m->getFar(eye);
                shader->setMatFour("projection", glm::perspective(glm::radians(70.0f), ratio, glm::max(newNear, 0.01f), newFar));
                shader->setFloat("near", newNear);
                shader->setFloat("far", newFar);

                //TODO: you have to give the model or mesh it's texture, and indicate to the draw function that it needs to do this only this time
                //take a look at collision test bed

                m->draw(shader);
            }
        }


        //draw a combinatino of the previous framebuffers

//draws edges, white 
        if (stageUpTo > 2) {
            if (stageUpTo > 3) glBindFramebuffer(GL_FRAMEBUFFER, lineDFT.framebuffer);
            if (stageUpTo == 3) glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.05f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader = shaders["line"];
            shader->use();
            shader->setMatFour("projection", ortho);
            shader->setVecTwo("screenResolution", vec2(screenX, screenY));
            //shader.setMatFour("view", view);

            uint32_t normalLocation = glGetUniformLocation(shader->program, "normalTexture");
            uint32_t depthLocation = glGetUniformLocation(shader->program, "depthTexture");

            glUniform1i(normalLocation, 0);
            glUniform1i(depthLocation, 1);

            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, normalDFT.texture);

            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, depthDFT.texture);
            flatscreen.draw(shader);
        }
        
 //use line data to create a texture of coordinates distributed weighted twoards the edges of objects
        if (stageUpTo > 3) {
            if (stageUpTo > 4) glBindFramebuffer(GL_FRAMEBUFFER, pointDFT.framebuffer);
            if (stageUpTo == 4) glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader = shaders["point"];
            shader->use();
            shader->setVecThree("eyePos", eye);
            shader->setMatFour("projection", ortho);
            //shader.setMatFour("projection", glm::ortho(0.0f, 1.0f, 0.0f, 1.0f));
            //shader.setVecTwo("screenResolution", vec2(screenX, screenY));
            //shader.setMatFour("view", view);

            uint32_t lineLocation = glGetUniformLocation(shader->program, "lineTexture");

            glUniform1i(lineLocation, 0);

            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, lineDFT.texture);

            flatscreen.draw(shader);
        }

//TEMPORARY --- draws the theoretical points to th screen to confirm that they're in correct locations

        if (stageUpTo == 5) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader = shaders["coord"];
            shader->use();
            shader->setMatFour("projection", ortho);
            shader->setInt("width", screenX);
            shader->setInt("height", screenY);

            uint32_t pointLocation = glGetUniformLocation(shader->program, "pointTexture");
            glUniform1i(pointLocation, 0);
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, pointDFT.texture);

            glBindVertexArray(fakeVAO);
            glDrawArrays(GL_POINTS, 0, numberOfPoints);
            glBindVertexArray(0);
        }


//draw veronoi diagram as part of stage 1 //TODO: do this better/faster/harder/stronger
        if (stageUpTo > 5) {
            if (stageUpTo > 6) glBindFramebuffer(GL_FRAMEBUFFER, stage1.framebuffer);
            if (stageUpTo == 6) glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader = shaders["stage1"];
            shader->use();
            shader->setMatFour("projection", ortho);
            shader->setInt("width", screenX);

            uint32_t pointLocation = glGetUniformLocation(shader->program, "pointTexture");
            glUniform1i(pointLocation, 0);
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, pointDFT.texture);

            flatscreen.drawInstances(shader, numberOfPoints);
        }

//stage 2 of the triangulation

        if (stageUpTo > 6) {
            if (stageUpTo > 7) glBindFramebuffer(GL_FRAMEBUFFER, stage2.framebuffer);
            if (stageUpTo == 7) glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader = shaders["stage2"];
            shader->use();
            shader->setMatFour("projection", ortho);


            uint32_t stage2PointLocation = glGetUniformLocation(shader->program, "pointTexture");
            glUniform1i(stage2PointLocation, 0);
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, pointDFT.texture);

            uint32_t stage1Location = glGetUniformLocation(shader->program, "stage1Texture");
            glUniform1i(stage1Location, 1);
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, stage1.texture);

            flatscreen.drawInstances(shader, numberOfPoints);
        }

//stage 3 of the triangulation
        if (stageUpTo > 7) {
            if (stageUpTo > 8) glBindFramebuffer(GL_FRAMEBUFFER, stage3.framebuffer);
            if (stageUpTo == 8) glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.00f, 0.00f, 0.00f, 1.0f);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader = shaders["stage3"];
            shader->use();
            shader->setMatFour("projection", ortho);

            uint32_t stage1Location2 = glGetUniformLocation(shader->program, "stage1Texture");
            glUniform1i(stage1Location2, 0);
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, stage1.texture);

            uint32_t stage2Location = glGetUniformLocation(shader->program, "stage2Texture");
            glUniform1i(stage2Location, 1);
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, stage2.texture);

            uint32_t pointLocation3 = glGetUniformLocation(shader->program, "pointTexture");
            glUniform1i(pointLocation3, 2);
            glActiveTexture(GL_TEXTURE0 + 2);
            glBindTexture(GL_TEXTURE_2D, pointDFT.texture);

            flatscreen.drawInstances(shader, numberOfPoints);
        }
//stage 4 draw triangles from the 3 points   
        if (stageUpTo >8)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.00f, 0.00f, 0.00f, 1.0f);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader = shaders["stage4"];
            shader->use();
            shader->setMatFour("projection", ortho);

            uint32_t stage1Location4 = glGetUniformLocation(shader->program, "stage1Texture");
            glUniform1i(stage1Location4, 0);
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, stage1.texture);

            uint32_t stage2Location4 = glGetUniformLocation(shader->program, "stage2Texture");
            glUniform1i(stage2Location4, 1);
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, stage2.texture);

            uint32_t stage3Location4 = glGetUniformLocation(shader->program, "stage3Texture");
            glUniform1i(stage3Location4, 2);
            glActiveTexture(GL_TEXTURE0 + 2);
            glBindTexture(GL_TEXTURE_2D, stage3.texture);

            uint32_t colorLocation4 = glGetUniformLocation(shader->program, "colorTexture");
            glUniform1i(colorLocation4, 3);
            glActiveTexture(GL_TEXTURE0 + 3);
            glBindTexture(GL_TEXTURE_2D, colorDFT.texture);

            flatscreen.draw(shader);

        }

        processInput(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}