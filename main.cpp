#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>    
#include <iostream>
using namespace std;

#pragma region parameters 



float i;
bool threeD = true;
//prototypes
void setNewDatas(int sample);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
int processInput(GLFWwindow* window);
// Data_color twoHueColorMap(float f);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 720;

//camera settings
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//field of view
float fov = 45.0f;

int currentColor = 0;

const float scale_x = 5.;

//variable pour le sampling
bool add_is_press = false;
bool subtract_is_press = false;
bool c_is_press = false;
bool e_is_press = false;

//structure pour contenir les données de la fonction
typedef struct
{
    GLfloat x, y, z;
} Data;

//srtucture pour les données liées aux couleurs 
typedef struct
{
    GLfloat r, g, b;
} Data_color;

//buffers
unsigned int VBO, VBO_colors, VAO;

#pragma endregion



#pragma region Shader



//vertex shader
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"

"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform float scale_x;\n"
"out vec3 color;\n"

"void main()\n"
"{\n"
"   gl_Position =projection*view*model * vec4((aPos.x + 0.0f) /scale_x, (aPos.y + 0.0f) / scale_x, aPos.z, 1.0);\n"
"   color = aColor;\n"

"}\0";

//fragment shader
const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 color;\n"
"out vec4 FragColor;\n"

"void main()\n"
"{\n"
"   FragColor = vec4((color.r+ 0.0f),(color.g+ 0.0f),(color.b+ 0.0f),1.0);\n"
"}\n\0";

#pragma endregion

#pragma region colormaps



Data_color rainbowColorMap(float f) {
    const  float dx = 0.8f;
    f = (f < 0) ? 0 : (f > 1) ? 1 : f;//clamp f in[0,1]
    float g = (6 - 2 * dx) * f + dx;//scale f to[dx,6−dx]
    float R = max(0.0f, (3 - fabs(g - 4) - fabs(g - 5)) / 2);
    //int R = f/255.0;
    float G = max(0.0f, (4 - std::fabs(g - 2) - std::fabs(g - 4)) / 2);
    float B = max(0.0f, (3 - std::fabs(g - 1) - std::fabs(g - 2)) / 2);
    Data_color color;
    color.r = R;
    color.g = G;
    color.b = B;
    return color;
}

Data_color grayscaleColorMap(float f) {
    const  float dx = 0.8f;
    f = (f < 0) ? 0 : (f > 1) ? 1 : f;
    Data_color color;
    color.r = f * 255;
    color.g = f * 255;
    color.b = f * 255;
    return color;
}

Data_color heatColorMap(float f) {
    const float mid = 0.3f;
    f = (f < 0) ? 0 : (f > 1) ? 1 : f;//clamp f in[0,1]
    float R, G, B;
    if (f <= mid) {
        R = f * 255 / mid;
        G = 0.0f;
        B = 0.0f;
    }

    else {
        R = 255.0f;
        G = (f - mid) * 255 / (1 - mid);
        B = 0.0f;

    }


    Data_color color;
    color.r = R;
    color.g = G;
    color.b = B;
    return color;
}


Data_color divergingColorMap(float f) {
    const  float dx = 0.8f;
    const float mid = 0.5f;
    f = (f < 0) ? 0 : (f > 1) ? 1 : f;//clamp f in[0,1]
    float R, G, B;
    if (f <= 0.25f) {
        R = 0.0f;
        G = f * 255 / 0.25f;
        B = 255.0f;
    }
    else if (f <= mid) {
        R = (f - 0.25f) * 255 / 0.25f;
        G = 255.0f;
        B = 255.0f;
    }
    else if (f <= 0.75f) {
        R = 255.0f;
        G = 255.0f;
        B = 255 - ((f - 0.5f) * 255 / 0.25f);

    }
    else {
        R = 255.0f;
        G = 255 - ((f - 0.5f) * 255 / 0.25f);
        B = 0.0f;
    }

    Data_color color;
    color.r = R;
    color.g = G;
    color.b = B;
    return color;
}



Data_color twoHueColorMap(float f) {
    float percentage_z = f / 0.5;
    float R;
    float B;

    if (percentage_z < 0.5) {
        R = percentage_z;
        B = 1 - percentage_z;

    }
    else {
        R = 1 - percentage_z;
        B = percentage_z;

    }

    Data_color color;
    color.r = R;
    color.g = B;
    color.b = 0.;
    return color;
}


// fonction intermédiaire pour le choix des colormaps
Data_color getColorMap(float f) {
    if (currentColor == 0)
        return rainbowColorMap(f);
    if (currentColor == 1)
        return grayscaleColorMap(f);
    if (currentColor == 2)
        return twoHueColorMap(f);
    if (currentColor == 3)
        return heatColorMap(f);
    if (currentColor == 4)
        return divergingColorMap(f);

}
#pragma endregion

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "TP2 IMN430 USherbrooke", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

#pragma region ShaderInit




    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

#pragma endregion

#pragma region VertexBuffer

    //taille
    const int grid_x = 200;
    const int grid_y = 200;

    //data pour les points
    Data data[grid_x][grid_y];
    //data pour les couleurs
    Data_color data_color[grid_x][grid_y];

    //remplit les points nécessaires 
    for (int x = 0; x < grid_x; x += 1) {
        for (int y = 0; y < grid_y; y += 1) {
            float x_data = (x - 100.0) / 5.0;
            float y_data = (y - 100.0) / 5.0;
            float z_data = 2 * exp(-(x_data * x_data + y_data * y_data)) + exp(-((x_data - 3) * (x_data - 3) + (y_data - 3) * (y_data - 3)));
            Data_color color_x_y = getColorMap(z_data);
            data[x][y].x = x_data;
            data[x][y].y = y_data;
            data[x][y].z = z_data;

            data_color[x][y] = color_x_y;


        }
    }


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);


#pragma endregion




    glPointSize(2.0f);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // pour que l'écran se cale sur [-5;5]
    int id_scale_x = glGetUniformLocation(shaderProgram, "scale_x");
    glUseProgram(shaderProgram);
    glUniform1f(id_scale_x, scale_x);

    // // Nombre d'echantillons
    int sample = 100;

    // // Ajuster le nombre d'echantillons
    int input_ch;
    int step_sample = 10;




    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        //actualisation du temps
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        input_ch = processInput(window);

        if (input_ch == 1) {
            sample += step_sample;
        }
        if (input_ch == 2) {
            sample -= step_sample;
        }

        std::cout << "Nombre d'echantillons : " << sample << std::endl;

        // Actualiser les donnees
        setNewDatas(sample);

       
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        float uniform_scale_x = 8.0f;
        int scale_loc = glGetUniformLocation(shaderProgram, "scale_x");
        glUniform1f(scale_loc, uniform_scale_x);


        //matrice pour la projection
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        if (threeD){
            model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
            projection = glm::perspective(glm::radians(fov), 1200.0f / 720.0f, 0.1f, 100.0f);
        }

        //on passe les matrices aux shaders
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


        glBindVertexArray(VAO);


        glDrawArrays(GL_POINTS, 0, 1000000);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VBO_colors);


    glfwTerminate();
    return 0;
}




int processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE && c_is_press) {
        std::cout << "Ca release c" << std::endl;
        c_is_press = false;
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !c_is_press) {
        // Cas du '+'

        c_is_press = true;
        currentColor = (currentColor + 1) % 5;

    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE && e_is_press) {
        std::cout << "Ca release e" << std::endl;
        e_is_press = false;
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !e_is_press) {
        // Cas du '+'

        e_is_press = true;
        threeD = !threeD;

    }


    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    // 'Debloquer' les touches '+' et '-'
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_RELEASE && add_is_press) {
       
        add_is_press = false;
    }

    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_RELEASE && subtract_is_press) {
        subtract_is_press = false;
    }

    // 'Bloquer' les touches '+' et '-' & Envoie du signal
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS && !add_is_press) {
        // Cas du '+'
        add_is_press = true;
        return 1;
    }

    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS && !subtract_is_press) {
        // Cas du '-'
        subtract_is_press = true;
        return 2;
    }

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

    glViewport(0, 0, width, height);
}




void setNewDatas(int sample) {

    int num_points = sample * sample;
    Data data[sample][sample];
    Data_color data_color[sample][sample];

    for (int x = 0; x < sample; x += 1) {
        for (int y = 0; y < sample; y += 1) {
            float x_data = (x - 0.5 * (float)sample) / (0.1 * (float)sample);
            float y_data = (y - 0.5 * (float)sample) / (0.1 * (float)sample);
            float z_data = 2 * exp(-(x_data * x_data + y_data * y_data)) + exp(-((x_data - 3) * (x_data - 3) + (y_data - 3) * (y_data - 3)));

            //Data_color color_x_y = rainbowColorMap(z_data);
            Data_color color_x_y = getColorMap(z_data);

            //std::cout << "x_data : " << z_data << " | x_data_bis : " << z_data_bis << std::endl;
            data[x][y].x = x_data;
            data[x][y].y = y_data;
            data[x][y].z = z_data;

            data_color[x][y] = color_x_y;

        }
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VBO_colors);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_colors);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data_color), data_color, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

}


//zoom
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (fov >= 1.0f && fov <= 100.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 100.0f)
        fov = 100.0f;
}