#include "glad.h" // A modifier en fonction de votre cas - normalement pour le reste rien ne change
#include <GLFW/glfw3.h>

#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

float getFunction(float x, float y);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
int processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 720;

bool add_is_press = false;
bool subtract_is_press = false;

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

    // Nombre d'echantillons
    float sample = 1000;

    // Ajuster le nombre d'echantillons
    int input_ch;
    float step_sample = 10;

    float step_x;
    float step_y;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) { 
        // Input
        input_ch = processInput(window);

        if (input_ch == 1) {
            sample += step_sample;
        }

        if (input_ch == 2) {
            sample -= step_sample;
        }

        // Pas pour l'intervale [-5,5]
        //step_x = -5 + (x / sample) * 10;
        //step_y = -5 + (y / sample) * 10;

        std::cout << "Nombre d'echantillons : " << sample << std::endl;

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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

float getFunction(float x, float y) {
    // Fonction a echantillonner
    return 2 * exp(-(pow(x, 2) + pow(y, 2))) + exp(-(pow(x - 3, 2) + pow(y - 3, 2)));
}

