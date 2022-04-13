#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <array>
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
#include "vec2Hash.hpp"
#include "delaunator.hpp"

#include "Video.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

enum Stages {
    COLOR,
    GAUSSIAN,
    LAPLACIAN,
    SOBEL,
    DERIVATIVE,
    ETF,
    MAX_STAGE


};

using namespace std;
using namespace glm;

int shaderMode = 1;
Stages stageUpTo = COLOR;
//constexpr int maxStage = 9;

double deltaTime = 0.0f;	// Time between current frame and last frame
double lastFrame = 0.0f; // Time of last frame

int screenX = 1000;
int screenY = 1000;

int numberOfPoints = 5000;


//drawn framebuffer texture
struct DFT {

    uint32_t framebuffer;
    uint32_t texture;
    uint32_t RBO;
};


DFT colorDFT;
DFT gaussianDFT;
DFT labplacianDFT;
DFT sobelDFT;
DFT derivativeDFT;
DFT ETFDTF;


void regenFrameBuffer(DFT& dft, int width = screenX, int height = screenY) {

    glDeleteRenderbuffers(1, &dft.RBO);
    glDeleteFramebuffers(1, &dft.framebuffer);

    glGenFramebuffers(1, &dft.framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, dft.framebuffer);
    glGenTextures(1, &dft.texture);
    glBindTexture(GL_TEXTURE_2D, dft.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_LINEAR
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dft.texture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &dft.RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, dft.RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dft.RBO);
}

void GLAPIENTRY MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
}


void genFrameBuffers() {
    regenFrameBuffer(colorDFT);
    regenFrameBuffer(gaussianDFT);
    regenFrameBuffer(labplacianDFT);
    regenFrameBuffer(sobelDFT);
    regenFrameBuffer(derivativeDFT);
    regenFrameBuffer(ETFDTF);
}




//prereq output is an allocated size array of floats
void create1DGaussianKernel(float stdDev, int size, float* output) {

    float a = 2.0f * stdDev * stdDev;
    float lower = 1.0f / sqrt(glm::pi<float>() * a);


    int halfSize = size / 2;

    for (int x = -halfSize; x <= halfSize; x++) {
        float upper =  -((x * x) / a);
        output[x+ halfSize] = lower * exp(upper);
    }
}


//prereq output is an allocated size*size array of floats
void create2DGaussianKernel(float stdDev, int size, float* output) {

    float a = 2.0f * stdDev * stdDev;
    float lower = 1.0f / (glm::pi<float>() * a);

    int halfSize = size / 2;
    for (int x = -halfSize; x <= halfSize; x++) {
        for (int y = -halfSize; y <= halfSize; y++) {
            int idx = (x+ halfSize) + ((y+ halfSize) * size);
            float upper = -(((x * x) + (y * y)) / a);
            output[idx] = lower * exp(upper);
        }
    }

}

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {


    screenX = width;
    screenY = height;
    glViewport(0, 0, width, height);
    genFrameBuffers();
}
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {

        stageUpTo = Stages((stageUpTo + 1) % (MAX_STAGE));
        if (stageUpTo < 0) {
            stageUpTo = Stages(MAX_STAGE-1);
        }
        printf("stageupto: %i\n", stageUpTo);
    }

    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        stageUpTo = Stages((stageUpTo - 1) % (MAX_STAGE));
        if (stageUpTo < 0) {
            stageUpTo = Stages(MAX_STAGE-1);
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
   /* int size = 7;

    float* x = new float[size*size];
    
    create2DGaussianKernel(1.0f, size, x);


    for (int i = 0; i < size; i++) {
        for (int k = 0; k < size; k++) {
            printf("%f, ", x[(i*size)+k]);
        }
        printf("\n");
    }
    exit(-1);*/


    //Video x("C:/Users/noahm/OneDrive/Desktop/sample.mp4");
    //printf("opened\n");
    //x.getFrame();
    //printf("got frame\n");
    //exit(0);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
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

    //glFrontFace(GL_CCW);
    //glEnable(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);


    //glEnable(GL_DEBUG_OUTPUT);
    //glDebugMessageCallback(MessageCallback, 0);

    Screen flatscreen;

    vector<Model*> models;
    models.push_back(new Model("backpack"));
    //models.push_back(new Model("dedust"));

    shaders["object"] = new Shader("object.vert", "object.frag");

    shaders["gaussian"] = new Shader("KuwaharaVertShader.glsl", "GaussianBlurFragShader.glsl");
    shaders["derivative"] = new Shader("KuwaharaVertShader.glsl", "DerivativeFragShader.glsl");
    shaders["laplacian"] = new Shader("KuwaharaVertShader.glsl", "LaplacianFragShader.glsl");
    shaders["sobel"] = new Shader("KuwaharaVertShader.glsl", "sobelFragShader.glsl");
    shaders["derivative"] = new Shader("KuwaharaVertShader.glsl", "DerivativeFragShader.glsl");
    shaders["etf"] = new Shader("KuwaharaVertShader.glsl", "etfFragShader.glsl");




   // shaders["vPoints"] = new Shader("veronPointVertShader.glsl", "veronPointFragShader.glsl");





    mat4 ortho = glm::ortho(0.0f, 1.0f, 0.0f, float(screenX)/float(screenY));


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


    unsigned int triangleVAO, triangleVBO, triangleEBO;



    int gaussianSize = 7;

    float* gaussianKernel = new float[gaussianSize * gaussianSize];

    create2DGaussianKernel(1.0f, gaussianSize, gaussianKernel);



    GLfloat* pointTexturePixels = new GLfloat[numberOfPoints *2];
    //vec4* polarPoints = new vec4[numberOfPoints];

    //vec2* pointsArray = new vec2[numberOfPoints];

    int frameCounter = -1;
    float frameTimes[30](0);
    int lastSecondFrameCount = -1;

    printf("begining of draw loop\n");

    //int sampleTexture = textureFromFile("sample.png");

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


        float sinit = (sin(currentFrame / 5.0f));
        float cosit = (cos(currentFrame / 5.0f));
        float ratio = (float)screenX / (float)screenY;

        //vec3 eye = vec3(sinit * 5.0f, 5.0f, cosit * 5.0f);
        //vec3 eye = vec3(5.0f, 200.0f, 5.0f);//for dedust
        vec3 eye = vec3(5.0, 5.0, 5.0);
        mat4 view = glm::lookAt(eye, vec3(0, 0, 0), vec3(0, 1, 0));



        if (stageUpTo >= COLOR) {
            glBindFramebuffer(GL_FRAMEBUFFER, stageUpTo>COLOR?colorDFT.framebuffer:0);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader = shaders["object"];
            shader->use();
            shader->setVecThree("eyePos", eye);
            shader->setMatFour("view", view);
            for (Model* m : models) {
                shader->setMatFour("projection", glm::perspective(glm::radians(20.0f), ratio, 0.01f, 100.0f));
                m->draw(shader);
            }
        
        }

        if (stageUpTo >= GAUSSIAN) {
            glBindFramebuffer(GL_FRAMEBUFFER, stageUpTo > GAUSSIAN ? gaussianDFT.framebuffer : 0);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader = shaders["gaussian"];
            shader->use();
            shader->setMatFour("projection", ortho);
            glUniform1fv(glGetUniformLocation(shader->program, "gaussianKernel"), gaussianSize * gaussianSize, gaussianKernel);

            uint32_t colorLocation = glGetUniformLocation(shader->program, "colorTexture");
            glUniform1i(colorLocation, 0);
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, colorDFT.texture);
            flatscreen.draw(shader);
        }
        if (stageUpTo >= LAPLACIAN) {
            glBindFramebuffer(GL_FRAMEBUFFER, stageUpTo > LAPLACIAN ? labplacianDFT.framebuffer : 0);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader = shaders["laplacian"];
            shader->use();
            shader->setMatFour("projection", ortho);

            uint32_t colorLocation = glGetUniformLocation(shader->program, "colorTexture");
            glUniform1i(colorLocation, 0);
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, gaussianDFT.texture);
            flatscreen.draw(shader);
        }
        if (stageUpTo >= SOBEL) {
            glBindFramebuffer(GL_FRAMEBUFFER, stageUpTo > SOBEL ? sobelDFT.framebuffer : 0);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader = shaders["sobel"];
            shader->use();
            shader->setMatFour("projection", ortho);

            uint32_t colorLocation = glGetUniformLocation(shader->program, "colorTexture");
            glUniform1i(colorLocation, 0);
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, gaussianDFT.texture);
            flatscreen.draw(shader);
        }
        if (stageUpTo >= DERIVATIVE) {
            glBindFramebuffer(GL_FRAMEBUFFER, stageUpTo > DERIVATIVE ? derivativeDFT.framebuffer : 0);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader = shaders["derivative"];
            shader->use();
            shader->setMatFour("projection", ortho);

            uint32_t colorLocation = glGetUniformLocation(shader->program, "colorTexture");
            glUniform1i(colorLocation, 0);
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, gaussianDFT.texture);
            flatscreen.draw(shader);
        }
        if (stageUpTo >= ETF) {
            glBindFramebuffer(GL_FRAMEBUFFER, stageUpTo > ETF ? ETFDTF.framebuffer : 0);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader = shaders["etf"];
            shader->use();
            shader->setMatFour("projection", ortho);

            uint32_t colorLocation = glGetUniformLocation(shader->program, "colorTexture");
            glUniform1i(colorLocation, 0);
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, gaussianDFT.texture);

            uint32_t derivativeLocation = glGetUniformLocation(shader->program, "derivativeTexture");
            glUniform1i(derivativeLocation, 1);
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, derivativeDFT.texture);
            flatscreen.draw(shader);
        }



        processInput(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
        //cin.get();
    }
}