#include "main.h"

#include <iostream>
using namespace std;


float i;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


GLint uniform_offset_x;
GLint uniform_scale_x;

float offset_x = 0.0;
float scale_x = 1.0;

typedef struct
{
    GLfloat x, y, z;
} Data;





const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"uniform float offset_x;\n"
"uniform float scale_x;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4((aPos.x + 0.0f) / 5,(aPos.y + 0.0f) / 5, aPos.z, 1.0);\n"
"   color = aColor;\n"
//"   gl_FrontColor = vec4((fragmentColor.r+ 0.0f),(fragmentColor.g+ 0.0f),(fragmentColor.b+ 0.0f),1.0);\n"
"}\0"; 

const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 color;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4((color.r+ 0.0f),(color.g+ 0.0f),(color.b+ 0.0f),1.0);\n"
//"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
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

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
   
    const int grid_x = 100;
    const int grid_y = 100;
    const int num_points = grid_x * grid_y;

    Data data[grid_x][grid_y];
    Data_color data_color[grid_x][grid_y];



    for (int x = 0; x < grid_x ; x += 1) {
        for (int y = 0; y < grid_y; y += 1) {
            float x_data = (x - 50.0) / 10.0;
            float y_data = (y - 50.0) / 10.0;
            float z_data = 2 * exp(-(x_data * x_data + y_data * y_data) + exp(-((x_data - 3) * (x_data - 3) + (y_data - 3) * (y_data - 3))));
            z_data  =function_calc(x,y);
            Data_color color_x_y = rainbowColorMap(z_data);


            data[x][y].x = x_data;
            data[x][y].y = y_data;
            data[x][y].z = 0.0f;

            data_color[x][y] = color_x_y;  

        }
    }
 

    unsigned int VBO,VBO_colors, VAO;
    GLint attribute_coord2d, attribute_v_color;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);



        
    //On fait la meme pour les couleurs
    glGenBuffers(1, &VBO_colors);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_colors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data_color), data_color, GL_STATIC_DRAW);
    

    




    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_colors);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);



        
        glDrawArrays(GL_POINTS, 0, 1000000);
        // glBindVertexArray(0); // no need to unbind it every time 

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
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

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
    float f_orig =f;
    const  float dx = 0.8;
    f=(f<0)? 0  :  ( f>1)? 1  :  f ;//clamp f in[0,1]
    int g=(6-2*dx)*f+dx;//scale f to[dx,6−dx]
    int R=max(0.0,(3-fabs(g-4)-fabs(g-5))/2);
    //int R = f/255.0;
    int G=max(0.0,(4-std::fabs(g-2)-std::fabs(g-4))/2);
    int B=max(0.0,(3-std::fabs(g-1)-std::fabs(g-2))/2);
    Data_color color;
    color.r = R;
    color.g = G;
    color.b = B;
    if(f_orig!=0)
    std::cout << f_orig << " => " << color.r << " "<< color.g << " "<< color.b << std::endl;
    return color;
}

float function_calc(float x, float y){
    return 2* exp(-(pow(x,2)+pow(y,2)))+ exp(-(pow(x-3,2)+pow(y-3,2)));
}


 
