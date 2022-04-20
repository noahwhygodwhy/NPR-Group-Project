#include <iostream>
#include <vector>
#include <unordered_map>

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Shader.hpp"
#include "Screen.hpp"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION  
#include "stb_image_write.h"



using namespace std;
using namespace glm;



//bunch of initialized values
enum Stages {
    COLOR,
    GAUSSIAN,
    KUWAHARA,
    SOBEL,
    COMPOSITE,
    MAX_STAGE
};

Stages stageUpTo = COLOR;

double deltaTime = 0.0f; // Time between current frame and last frame
double lastFrame = 0.0f; // Time of last frame

int screenX = 500;
int screenY = 500;

int kuwaharaX = 5;
int kuwaharaY = 5;

bool outputFrame = false;


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
DFT kuwaharaDFT;


//generates a framebuffer/texture/rbo combo for a new screen size
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

//debug function
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

//helper function to regen all of the intermediate frame buffers
void genFrameBuffers() {
    regenFrameBuffer(colorDFT);
    regenFrameBuffer(gaussianDFT);
    regenFrameBuffer(labplacianDFT);
    regenFrameBuffer(sobelDFT);
    regenFrameBuffer(kuwaharaDFT);
}




//saves the glfw window to a file. copied this from some stack overflow post
void saveImage(string filepath, GLFWwindow* w) {

    int width, height;
    glfwGetFramebufferSize(w, &width, &height);
    GLsizei nrChannels = 4;
    GLsizei stride = nrChannels * width;
    stride += (stride % 4) ? (4 - stride % 4) : 0;
    GLsizei bufferSize = stride * height;
    std::vector<float> buffer(bufferSize);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());

    stbi_flip_vertically_on_write(true);
    stbi_write_png(("out/"+filepath).c_str(), width, height, nrChannels, buffer.data(), stride);
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



//on screen size change
void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    screenX = width;
    screenY = height;
    glViewport(0, 0, width, height);
    genFrameBuffers();
}


//every frame key processing
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

}
//any time key proecssing
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
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        kuwaharaX = glm::max(kuwaharaX - 1, 0);
        printf("kuwaharaX: %i\n", kuwaharaX);
    }

    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        kuwaharaX = glm::min(kuwaharaX + 1, 50);
        printf("kuwaharaX: %i\n", kuwaharaX);
    }
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        kuwaharaY = glm::min(kuwaharaY + 1, 50);
        printf("kuwaharaX: %i\n", kuwaharaY);
    }

    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        kuwaharaY = glm::max(kuwaharaY - 1, 0);
        printf("kuwaharaX: %i\n", kuwaharaY);
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        outputFrame = true;
    }


}


//duh
string getTimeString() {
    time_t now;
    time(&now);
    char buf[sizeof "####-##-##-##-##-##"];
    strftime(buf, sizeof buf, "%F-%H-%M-%S", gmtime(&now));
    return string(buf);
}





//get a opengl texture from a file. path is the path, width and height give you the ints, directory if it's not a local file
unsigned int textureFromFile(const char* path, int& widthOut, int& heightOut, const string& directory = "./")
{
    stbi_set_flip_vertically_on_load(true);
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;


    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    heightOut = height;
    widthOut = width;

    return textureID;
}

void clear()
{
    #if defined _WIN32
        system("cls");
        //clrscr(); // including header file : conio.h
    #elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
        system("clear");
        //std::cout<< u8"\033[2J\033[1;1H"; //Using ANSI Escape Sequences 
    #elif defined (__APPLE__)
        system("clear");
    #endif
}

void printInfo(float fps) {
    clear();

    string stage;

    switch (stageUpTo) {
        case COLOR:stage = "color"; break;
        case GAUSSIAN:stage = "gaussian"; break;
        case KUWAHARA:stage = "kuwahara"; break;
        case SOBEL:stage = "sobel"; break;
        case COMPOSITE:stage = "composite"; break;
        default:stage = "broken"; break;
    }

    printf("Stage: %s\n", stage.c_str());
    printf("FPS: %f\n", fps);
}

unordered_map<string, Shader*> shaders;
Shader* shader;
int main()
{

//***********Initialize opengl stuff
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 16);
    GLFWwindow* window = glfwCreateWindow(screenX, screenY, "", NULL, NULL);

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

  


//initialize the flatscreen triangle to draw stuff on
    Screen flatscreen;



 //set up shaders
    shaders["color"] = new Shader("GeneralVertShader.glsl", "BasicColorFragShader.glsl");
    shaders["kuwahara"] = new Shader("GeneralVertShader.glsl", "KuwaharaFragShader.glsl");
    shaders["gaussian"] = new Shader("GeneralVertShader.glsl", "GaussianBlurFragShader.glsl");
    shaders["sobel"] = new Shader("GeneralVertShader.glsl", "SobelFragShader.glsl");
    shaders["composite"] = new Shader("GeneralVertShader.glsl", "CompositeFragShader.glsl");


//set up gaussian blur kernel
    int gaussianSize = 7;
    float* gaussianKernel = new float[gaussianSize * gaussianSize];
    create2DGaussianKernel(1.0f, gaussianSize, gaussianKernel);

//load images, including input
    int width, height;
    GLuint canvasTextureID = textureFromFile("canvas.jpg", width, height);
    GLuint frameTextureID = textureFromFile("rit.jpg", width, height);
    glfwSetWindowSize(window, width, height);
    frameBufferSizeCallback(window, width, height);

//fps stuff
    int frameCounter = -1;
    float frameTimes[30](0);
    int lastSecondFrameCount = -1;

    mat4 orth = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);

    while (!glfwWindowShouldClose(window)) {

        //more fps stuff
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


        //clear buffers
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


//All of these stages are pretty similar, so i'm just going to comment one of them
        if (stageUpTo >= COLOR) {//if this is the last stage, output to normal fb, otherwise output to special fb
            glBindFramebuffer(GL_FRAMEBUFFER, stageUpTo > COLOR ? colorDFT.framebuffer : 0);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //load the right shader + uniforms
            shader = shaders["color"];
            shader->use();
            shader->setMatFour("projection", orth);

            //load textures
            uint32_t colorLocation = glGetUniformLocation(shader->program, "colorTexture"); 
            glUniform1i(colorLocation, 0);
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, frameTextureID);

            //draw it
            flatscreen.draw(shader);
        }
//gaussian stage
        if (stageUpTo >= GAUSSIAN) {
            glBindFramebuffer(GL_FRAMEBUFFER, stageUpTo > GAUSSIAN ? gaussianDFT.framebuffer : 0);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader = shaders["gaussian"];
            shader->use();
            shader->setMatFour("projection", orth);
            glUniform1fv(glGetUniformLocation(shader->program, "gaussianKernel"), gaussianSize * gaussianSize, gaussianKernel);

            uint32_t colorLocation = glGetUniformLocation(shader->program, "colorTexture");
            glUniform1i(colorLocation, 0);
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, colorDFT.texture);
            flatscreen.draw(shader);
        }
//kuwahara stage
        if (stageUpTo >= KUWAHARA) {
            glBindFramebuffer(GL_FRAMEBUFFER, stageUpTo > KUWAHARA ? kuwaharaDFT.framebuffer : 0);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader = shaders["kuwahara"];
            shader->use();
            shader->setMatFour("projection", orth);

            shader->setInt("kernalWidth", kuwaharaX);
            shader->setInt("kernalHeight", kuwaharaY);

            uint32_t colorLocation = glGetUniformLocation(shader->program, "colorTexture");
            glUniform1i(colorLocation, 0);
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, gaussianDFT.texture);

            flatscreen.draw(shader);
        }
//sobel stage
        if (stageUpTo >= SOBEL) {
            glBindFramebuffer(GL_FRAMEBUFFER, stageUpTo > SOBEL ? sobelDFT.framebuffer : 0);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader = shaders["sobel"];
            shader->use();
            shader->setMatFour("projection", orth);

            uint32_t colorLocation = glGetUniformLocation(shader->program, "colorTexture");
            glUniform1i(colorLocation, 0);
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, kuwaharaDFT.texture);
            flatscreen.draw(shader);
        }
//put it all together stage
        if (stageUpTo >= COMPOSITE) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader = shaders["composite"];
            shader->use();
            shader->setMatFour("projection", glm::ortho(0.0f, 1.0f, 0.0f, 1.0f));
            glUniform1fv(glGetUniformLocation(shader->program, "gaussianKernel"), gaussianSize * gaussianSize, gaussianKernel);

            uint32_t lapLocation = glGetUniformLocation(shader->program, "sobelTexture");
            glUniform1i(lapLocation, 0);
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, sobelDFT.texture);

            uint32_t colorLocation = glGetUniformLocation(shader->program, "kuwaharaTexture");
            glUniform1i(colorLocation, 1);
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, kuwaharaDFT.texture);

            uint32_t canvasloc = glGetUniformLocation(shader->program, "canvasTexture");
            glUniform1i(canvasloc, 2);
            glActiveTexture(GL_TEXTURE0 + 2);
            glBindTexture(GL_TEXTURE_2D, canvasTextureID);
            flatscreen.draw(shader);
        }


        processInput(window);
        glfwSwapBuffers(window);

        if (outputFrame) {
            saveImage(getTimeString() + string(".png"), window);
            printf("outputing frame\n");
            outputFrame = false;
        }

        glfwPollEvents();


        //cin.get();
    }
}