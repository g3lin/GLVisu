#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <math.h>

typedef struct
{
    GLfloat r, g, b;
} Data_color;

void construct_points(float pasEchantillonage);
Data_color rainbowColorMap(float f);
float function_calc(float x, float y);
