#include "main.h"

#include <iostream>
using namespace std;

void setNewDatas(int sample);

float i;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
int processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


GLint uniform_offset_x;
GLint uniform_scale_x;

const float scale_x = 5.;

bool add_is_press = false;
bool subtract_is_press = false;

typedef struct
{
    GLfloat x, y, z;
} Data;

unsigned int VBO,VBO_colors, VAO;


const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"

"uniform float offset_x;\n"
"uniform float scale_x;\n"
"out vec3 color;\n"

"void main()\n"
"{\n"
"   gl_Position = vec4((aPos.x + 0.0f) / scale_x, (aPos.y + 0.0f) / scale_x, aPos.z, 1.0);\n"
"   color = aColor;\n"

"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 color;\n"
"out vec4 FragColor;\n"

"void main()\n"
"{\n"
"   FragColor = vec4((color.r+ 0.0f),(color.g+ 0.0f),(color.b+ 0.0f),1.0);\n"

"}\n\0";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // build and compile our shader program
    // ------------------------------------
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

    int id_scale_x = glGetUniformLocation(shaderProgram, "scale_x");
    glUseProgram(shaderProgram);
    glUniform1f(id_scale_x, scale_x);

    // Nombre d'echantillons
    int sample = 100;

    // Ajuster le nombre d'echantillons
    int input_ch;
    int step_sample = 10;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
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

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        // draw our first triangle
        glUseProgram(shaderProgram);
        

        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, 1000000);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}




// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
int processInput(GLFWwindow* window) { 
    
    // Fermer la fenetre
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
       
        return -1;
    }

    // 'Debloquer' les touches '+' et '-'
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_RELEASE && add_is_press) {
        std::cout << "Ca release add" << std::endl;
        add_is_press = false;

    }

    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_RELEASE && subtract_is_press) {
        std::cout << "Ca release subtract" << std::endl;
        subtract_is_press = false;

    }

    // 'Bloquer' les touches '+' et '-' & Envoie du signal
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS && !add_is_press) {
        // Cas du '+'
        std::cout << "C'est plus" << std::endl;
        add_is_press = true;

        return 1;
    }

    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS && !subtract_is_press) {
        // Cas du '-'
        std::cout << "C'est moins" << std::endl;
        subtract_is_press = true;

        return 2;
    }

    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

Data_color rainbowColorMap(float f){ 
    const  float dx = 0.8f;
    f=(f<0)? 0  :  ( f>1)? 1  :  f ;//clamp f in[0,1]
    float g=(6-2*dx)*f+dx;//scale f to[dx,6−dx]
    float R=max(0.0f,(3-fabs(g-4)-fabs(g-5))/2);
    //int R = f/255.0;
    float G=max(0.0f,(4-std::fabs(g-2)-std::fabs(g-4))/2);
    float B=max(0.0f,(3-std::fabs(g-1)-std::fabs(g-2))/2);
    Data_color color;
    color.r = R;
    color.g = G;
    color.b = B;
    return color;
}

float function_calc(float x, float y){
    return 2* exp(-(pow(x,2)+pow(y,2)))+ exp(-(pow(x-3,2)+pow(y-3,2)));
}

void setNewDatas(int sample) {

    int num_points = sample * sample;
    Data data[sample][sample];
    Data_color data_color[sample][sample];

    for (int x = 0; x < sample ; x += 1) {
        for (int y = 0; y < sample; y += 1) {
            float x_data = (x - 0.5 * (float)sample) / (0.1 * (float)sample);
            float y_data = (y - 0.5 * (float)sample) / (0.1 * (float)sample);
            float z_data = 2 * exp(-(x_data * x_data + y_data * y_data) + exp(-((x_data - 3) * (x_data - 3) + (y_data - 3) * (y_data - 3))));

            Data_color color_x_y = rainbowColorMap(z_data);

            //std::cout << "x_data : " << z_data << " | x_data_bis : " << z_data_bis << std::endl;
            data[x][y].x = x_data;
            data[x][y].y = y_data;
            data[x][y].z = 0.0f;

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
 
