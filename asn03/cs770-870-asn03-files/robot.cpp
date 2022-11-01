// Draw an articulated figure.

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, rotate
#include <glm/gtx/string_cast.hpp> // glm::to_string()

#include "mesh.hpp"
#include "shader.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "grid.hpp"
#include "flat_lighter.hpp"
#include "mesh.hpp"

#define EPSILON 0.0001

using namespace std;

extern int add_vertex(float x, float y, float z,
                      float *coords, GLuint *indexes, int vert_index);
extern void print_mat4(const glm::mat4& m);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

Camera cam;
bool scene_changed = true;
Grid grid;
GLFWwindow *window;
Flat_Lighter lighter;

float world_BL_x = -4;
float world_BL_y = -1;
float world_width = 9;
float world_height = 5;



void draw_box(const glm::mat4x4& M) {
    // Draw an axis-aligned box, with corners at (0 0) and (1 1).
    // Apply the transformation M when drawing.

    // Initial points
    glm::vec4 bl(0, 0, 0, 1);
    glm::vec4 tl(0, 1, 0, 1);
    glm::vec4 tr(1, 1, 0, 1);
    glm::vec4 br(1, 0, 0, 1);

    bl = M * bl;
    tl = M * tl;
    tr = M * tr;
    br = M * br;

    float *coords = new float[4 * 4];
    GLuint *indexes = new GLuint[4];


    // Create Box
    Mesh *box_shape;

    int i = 0;   
    i = add_vertex(bl.x, bl.y, 0, coords, indexes, i);
    i = add_vertex(tl.x, tl.y, 0, coords, indexes, i);
    i = add_vertex(tr.x, tr.y, 0, coords, indexes, i);
    i = add_vertex(br.x, br.y, 0, coords, indexes, i);
    box_shape = new Mesh(coords, 4, 4,
                            indexes, 4,
                            4,
                            GL_TRIANGLE_FAN, lighter.get_program_handle(),
                            "Box");
                         
    
    // Create Outline    
    Mesh *line_shape;

    i = 0;
    i = add_vertex(bl.x, bl.y, 0, coords, indexes, i);
    i = add_vertex(tl.x, tl.y, 0, coords, indexes, i);
    i = add_vertex(tr.x, tr.y, 0, coords, indexes, i);
    i = add_vertex(br.x, br.y, 0, coords, indexes, i);
    line_shape = new Mesh(coords, 4, 4,
                            indexes, 4,
                            4,
                            GL_LINE_LOOP, lighter.get_program_handle(),
                            "Outline");
                         

    // Draw
    lighter.set_color(glm::vec4(0.5, 0.5, 0.5, 1));      
    box_shape->draw();
    lighter.set_color(glm::vec4(1, 1, 1, 1));
    line_shape->draw();

    // This is for debugging.  Remove it before submitting.
    std::cout << "draw_box. M is:\n";
    print_mat4(M);
}

stack<glm::mat4x4> init(){
    stack<glm::mat4x4> m;
    return m;
}

//glm::mat4x4 translate(stack<glm::mat4x4>, )

void draw_robot() {

    
    // Init
    stack<glm::mat4x4> modeling_stack;                  

    //modeling_stack.push(m);
    //draw_box(m); // Draw initial box

    glm::mat4 identity_matrix = glm::mat4(2.0f);
    //glm::mat4 scale = glm::mat4(1.0f);


    modeling_stack.push(identity_matrix);
    draw_box(identity_matrix); // Draw initial box


    // Matrices
    //glm::mat4 scale = glm::scale(modeling_stack.top(), glm::vec3(0.5f, 1.0f, 1.0f)); // Scaling
    //glm::mat4 scale = glm::scale(m, glm::vec3(0.5f, 1.0f, 1.0f)); // Scaling
    //glm::mat4x4 rotate = glm::rotate(modeling_stack.top(),45, 1); // Rotation
    

    // Right arm
    modeling_stack.push(glm::translate(modeling_stack.top(), glm::vec3(1.0f, 1.0f, 1.0f))); //translate
    modeling_stack.push(glm::rotate(modeling_stack.top(), 3.93f, glm::vec3(0.0f, 0.0f, 1.0f))); //rotate
    modeling_stack.push(glm::scale(modeling_stack.top(), glm::vec3(-0.5f, 1.0f, 1.0f))); // scale
    
    draw_box(modeling_stack.top()); // draw
    modeling_stack.pop();
    
    //Right Hand
    modeling_stack.push(glm::translate(modeling_stack.top(), glm::vec3(-0.5f, 1.0f, 1.0f))); //translate
    modeling_stack.push(glm::rotate(modeling_stack.top(), 0.785f, glm::vec3(0.0f, 0.0f, 1.0f))); //rotate
    modeling_stack.push(glm::scale(modeling_stack.top(), glm::vec3(0.25f, 0.5f, 1.0f))); // scale
    

    draw_box(modeling_stack.top()); // draw
    modeling_stack.pop();
    modeling_stack.pop();
    modeling_stack.pop();
    modeling_stack.pop();
    modeling_stack.pop();


    //Left Arm
    //modeling_stack.push(glm::rotate(modeling_stack.top(), 45, glm::normalize(glm::vec3(1, 1, 0))));
    modeling_stack.push(glm::translate(modeling_stack.top(), glm::vec3(0.0f, 1.0f, 1.0f))); //translate
    modeling_stack.push(glm::rotate(modeling_stack.top(), 0.785f, glm::vec3(0.0f, 0.0f, 1.0f))); //rotate
    modeling_stack.push(glm::scale(modeling_stack.top(), glm::vec3(-0.5f, 1.0f, 1.0f))); // scale
    // Scale, draw, pop

    //modeling_stack.push(glm::rotate( modeling_stack.top() , glm::cos(45.0f), glm::vec3(0.0f, 0.0f, 1.0f))); //rotate

    draw_box(modeling_stack.top()); // draw
    modeling_stack.pop();
    //modeling_stack.pop();
    

    //Left Hand
    
    //modeling_stack.push(glm::rotate(modeling_stack.top(), 1.185f, glm::vec3(0.0f, 0.0f, 1.0f))); //rotate
    //modeling_stack.push(glm::scale(modeling_stack.top(), glm::vec3(-0.25f, 0.5f, 1.0f))); // scale
    modeling_stack.push(glm::translate(modeling_stack.top(), glm::vec3(-0.5f, 1.0f, 1.0f))); //translate
    modeling_stack.push(glm::rotate(modeling_stack.top(), 2.36f, glm::vec3(0.0f, 0.0f, 1.0f))); //rotate
    modeling_stack.push(glm::scale(modeling_stack.top(), glm::vec3(0.25f, 0.5f, 1.0f))); // scale

    draw_box(modeling_stack.top()); // draw
    modeling_stack.pop();

    
}

void draw_scene() {
    // Draw all the boxes that make up an articulated figure.
    // ------
    glClearColor(0.4f, 0.2f, 0.1f, 1.0f); // dark red/brown
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    lighter.set_camera(cam);

    grid.draw();

    draw_robot();

    glfwSwapBuffers(window);
    scene_changed = false;
}

int main(int argc, char **argv)
{
    if (argc != 1) {
        std::cerr << "Usage:\n";
        std::cerr << "  robot\n";
        std::cerr << "Press ENTER to exit program:";
        char reply[10];
        std::cin.getline(reply, 9);
        exit(1);
    }

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();

    int display_width = 800;
    int display_height = (int)(display_width
                               / world_width * world_height);

    GLFW_OpenGL_Window glfw_window(3, 3,
                                   display_width,
                                   display_height,
                                   "Robot 2D",
                                   &framebuffer_size_callback);
    window = glfw_window.get_window();

    lighter.init();

    grid = Grid(world_BL_x, world_BL_y, world_width, world_height,
                1, 0.1f, lighter);

    cam = Camera(display_width, display_height);
    cam.init_2D(world_BL_x, world_BL_y, world_width, world_height);

    glfwGetFramebufferSize(glfw_window.get_window(),
                           &display_width,
                           &display_height);
    cam.resize_display(display_width, display_height);

    lighter.set_camera(cam);

    // render loop
    // -----------
    scene_changed = true;
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);
        if (scene_changed) {

            draw_scene();

        }
        glfwWaitEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

bool key_pressed(unsigned int key_ID, GLFWwindow *window) {
    return glfwGetKey(window, (int)key_ID) == GLFW_PRESS;
}

bool shift_down(GLFWwindow *window) {
    // Is the SHIFT key pressed right now?
    return key_pressed(GLFW_KEY_LEFT_SHIFT, window) ||
           key_pressed(GLFW_KEY_RIGHT_SHIFT, window);
}

// process all input: query GLFW whether relevant keys are pressed/released
// this frame, and react accordingly
// --------------------------------------------------------------
void processInput(GLFWwindow *w)
{
    if (key_pressed(GLFW_KEY_ESCAPE, w) ||
        key_pressed(GLFW_KEY_Q, w))
        glfwSetWindowShouldClose(w, true);

    else if (key_pressed(GLFW_KEY_LEFT, w)) {
        cam.left();
        scene_changed = true;
    }

    else if (key_pressed(GLFW_KEY_RIGHT, w)) {
        cam.right();
        scene_changed = true;
    }

    else if (key_pressed(GLFW_KEY_UP, w)) {
        if (shift_down(w))
            cam.forward();
        else
            cam.up();
        scene_changed = true;
    }

    else if (key_pressed(GLFW_KEY_DOWN, w)) {
        if (shift_down(w))
            cam.back();
        else
            cam.down();
        scene_changed = true;
    }
}

// glfw: whenever the window size changed (by OS or user resize)
// this callback function executes
// ------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions;
    // note that width and height will be significantly larger
    // than specified on retina displays.
    cam.resize_display(width, height);
    lighter.set_camera(cam);
}

