#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/vec3.hpp" // glm::vec3
#include "glm/trigonometric.hpp" // sin, cos

#include "mesh.hpp"
#include "shader.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "grid.hpp"
#include "flat_lighter.hpp"
#include "mesh.hpp"

//Trying for ASN10
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

Camera cam;
bool scene_changed = true;
GLFWwindow *window;
Grid grid;
Flat_Lighter lighter;

std::vector<Mesh*> letter_shapes;

int add_vertex(float x, float y, float z,
               float *coords, GLuint *indexes, int vert_index) {
    int i = vert_index / 4;
    indexes[i] = i;
    coords[vert_index++] = x;
    coords[vert_index++] = y;
    coords[vert_index++] = z;
    coords[vert_index++] = 1;
    return vert_index;
}

void make_scene() {
    int max_vertices = 100;
    float *coords = new float[max_vertices * 4];
    GLuint *indexes = new GLuint[max_vertices];
    GLuint program_handle = lighter.get_program_handle();


    int i = 0;
    i = add_vertex(0.1, 0.75, 0, coords, indexes, i);
    i = add_vertex(0.1, 0.99, 0, coords, indexes, i);
    i = add_vertex(0.9, 0.99, 0, coords, indexes, i);
    i = add_vertex(0.9, 0.75, 0, coords, indexes, i);
    letter_shapes.push_back(new Mesh(coords, 4, 4,
                                     indexes, 4,
                                     4,
                                     GL_TRIANGLE_FAN, program_handle,
                                     "Top J"));
    i = 0;
    i = add_vertex(0.45, 0.01, 0, coords, indexes, i);
    i = add_vertex(0.45, 0.99, 0, coords, indexes, i);
    i = add_vertex(0.6, 0.99, 0, coords, indexes, i);
    i = add_vertex(0.6, 0.01, 0, coords, indexes, i);
    letter_shapes.push_back(new Mesh(coords, 4, 4,
                                     indexes, 4,
                                     4,
                                     GL_TRIANGLE_FAN, program_handle,
                                     "Horizontal of J"));
    i = 0;
    i = add_vertex(0.05, 0.1, 0, coords, indexes, i);
    i = add_vertex(0.6, 0.1, 0, coords, indexes, i);
    i = add_vertex(0.6, 0.01, 0, coords, indexes, i);
    i = add_vertex(0.05, 0.01, 0, coords, indexes, i);
    letter_shapes.push_back(new Mesh(coords, 4, 4,
                                     indexes, 4,
                                     4,
                                     GL_TRIANGLE_FAN, program_handle,
                                     "Bottom of J"));

    // My last name
    // letter "f"
    i = 0;
    i = add_vertex(1.2, 0.01, 0, coords, indexes, i);
    i = add_vertex(1.2, 0.9, 0, coords, indexes, i);
    i = add_vertex(1.9, 0.9, 0, coords, indexes, i);
    i = add_vertex(1.2, 0.9, 0, coords, indexes, i);
    i = add_vertex(1.2, 0.5, 0, coords, indexes, i);
    i = add_vertex(1.9, 0.5, 0, coords, indexes, i);
    letter_shapes.push_back(new Mesh(coords, 6, 4,
                                     indexes, 6,
                                     6,
                                     GL_LINE_STRIP, program_handle,
                                     "Letter f"));


    // letter "e"
    i = 0;
    i = add_vertex(2.8, 0.01, 0, coords, indexes, i);
    i = add_vertex(2.3, 0.01, 0, coords, indexes, i);
    i = add_vertex(2.2, 0.2, 0, coords, indexes, i);
    i = add_vertex(2.2, 0.4, 0, coords, indexes, i);
    i = add_vertex(2.3, 0.5, 0, coords, indexes, i);
    i = add_vertex(2.7, 0.5, 0, coords, indexes, i);
    i = add_vertex(2.8, 0.4, 0, coords, indexes, i);
    i = add_vertex(2.8, 0.25, 0, coords, indexes, i);
    i = add_vertex(2.2, 0.25, 0, coords, indexes, i);
    letter_shapes.push_back(new Mesh(coords, 9, 4,
                                     indexes, 9,
                                     9,
                                     GL_LINE_STRIP, program_handle,
                                     "Letter e"));

    // letter "r"
    i = 0;
    i = add_vertex(3.2, 0.01, 0, coords, indexes, i);
    i = add_vertex(3.2, 0.5, 0, coords, indexes, i);
    i = add_vertex(3.2, 0.4, 0, coords, indexes, i);
    i = add_vertex(3.3, 0.5, 0, coords, indexes, i);
    i = add_vertex(3.8, 0.5, 0, coords, indexes, i);
    letter_shapes.push_back(new Mesh(coords, 5, 4,
                                     indexes, 5,
                                     5,
                                     GL_LINE_STRIP, program_handle,
                                     "Letter r"));

    // letter "n"
    i = 0;
    i = add_vertex(4.2, 0.01, 0, coords, indexes, i);
    i = add_vertex(4.2, 0.5, 0, coords, indexes, i);
    i = add_vertex(4.2, 0.4, 0, coords, indexes, i);
    i = add_vertex(4.3, 0.5, 0, coords, indexes, i);
    i = add_vertex(4.7, 0.5, 0, coords, indexes, i);
    i = add_vertex(4.8, 0.4, 0, coords, indexes, i);
    i = add_vertex(4.8, 0.01, 0, coords, indexes, i);
    letter_shapes.push_back(new Mesh(coords, 7, 4,
                                     indexes, 7,
                                     7,
                                     GL_LINE_STRIP, program_handle,
                                     "Letter n"));


    // letter "a"
    i = 0;
    i = add_vertex(5.2, 0.2, 0, coords, indexes, i);
    i = add_vertex(5.2, 0.4, 0, coords, indexes, i);
    i = add_vertex(5.3, 0.5, 0, coords, indexes, i);
    i = add_vertex(5.7, 0.5, 0, coords, indexes, i);
    i = add_vertex(5.8, 0.4, 0, coords, indexes, i);
    i = add_vertex(5.8, 0.5, 0, coords, indexes, i);
    i = add_vertex(5.8, 0.01, 0, coords, indexes, i);
    i = add_vertex(5.8, 0.2, 0, coords, indexes, i);
    i = add_vertex(5.7, 0.01, 0, coords, indexes, i);
    i = add_vertex(5.3, 0.01, 0, coords, indexes, i);
    i = add_vertex(5.2, 0.2, 0, coords, indexes, i);
    letter_shapes.push_back(new Mesh(coords, 11, 4,
                                     indexes, 11,
                                     11,
                                     GL_LINE_STRIP, program_handle,
                                     "Letter a"));

    // letter "n"
    i = 0;
    i = add_vertex(6.2, 0.01, 0, coords, indexes, i);
    i = add_vertex(6.2, 0.5, 0, coords, indexes, i);
    i = add_vertex(6.2, 0.4, 0, coords, indexes, i);
    i = add_vertex(6.3, 0.5, 0, coords, indexes, i);
    i = add_vertex(6.7, 0.5, 0, coords, indexes, i);
    i = add_vertex(6.8, 0.4, 0, coords, indexes, i);
    i = add_vertex(6.8, 0.01, 0, coords, indexes, i);
    letter_shapes.push_back(new Mesh(coords, 7, 4,
                                     indexes, 7,
                                     7,
                                     GL_LINE_STRIP, program_handle,
                                     "Letter n"));

    /// letter "d"
    i = 0;
    i = add_vertex(7.2, 0.2, 0, coords, indexes, i);
    i = add_vertex(7.2, 0.4, 0, coords, indexes, i);
    i = add_vertex(7.3, 0.5, 0, coords, indexes, i);
    i = add_vertex(7.7, 0.5, 0, coords, indexes, i);
    i = add_vertex(7.8, 0.4, 0, coords, indexes, i);
    i = add_vertex(7.8, 1, 0, coords, indexes, i);
    i = add_vertex(7.8, 0.01, 0, coords, indexes, i);
    i = add_vertex(7.8, 0.2, 0, coords, indexes, i);
    i = add_vertex(7.7, 0.01, 0, coords, indexes, i);
    i = add_vertex(7.3, 0.01, 0, coords, indexes, i);
    i = add_vertex(7.2, 0.2, 0, coords, indexes, i);
    letter_shapes.push_back(new Mesh(coords, 11, 4,
                                     indexes, 11,
                                     11,
                                     GL_LINE_STRIP, program_handle,
                                     "Letter d"));

    // letter "e"
    i = 0;
    i = add_vertex(8.8, 0.01, 0, coords, indexes, i);
    i = add_vertex(8.3, 0.01, 0, coords, indexes, i);
    i = add_vertex(8.2, 0.2, 0, coords, indexes, i);
    i = add_vertex(8.2, 0.4, 0, coords, indexes, i);
    i = add_vertex(8.3, 0.5, 0, coords, indexes, i);
    i = add_vertex(8.7, 0.5, 0, coords, indexes, i);
    i = add_vertex(8.8, 0.4, 0, coords, indexes, i);
    i = add_vertex(8.8, 0.25, 0, coords, indexes, i);
    i = add_vertex(8.2, 0.25, 0, coords, indexes, i);
    letter_shapes.push_back(new Mesh(coords, 9, 4,
                                     indexes, 9,
                                     9,
                                     GL_LINE_STRIP, program_handle,
                                     "Letter e"));
    
    // letter "s"
    i = 0;
    i = add_vertex(9.2, 0.01, 0, coords, indexes, i);
    i = add_vertex(9.7, 0.01, 0, coords, indexes, i);
    i = add_vertex(9.8, 0.15, 0, coords, indexes, i);
    i = add_vertex(9.8, 0.2, 0, coords, indexes, i);
    i = add_vertex(9.7, 0.25, 0, coords, indexes, i);
    i = add_vertex(9.3, 0.25, 0, coords, indexes, i);
    i = add_vertex(9.2, 0.3, 0, coords, indexes, i);
    i = add_vertex(9.2, 0.35, 0, coords, indexes, i);
    i = add_vertex(9.3, 0.5, 0, coords, indexes, i);
    i = add_vertex(9.8, 0.5, 0, coords, indexes, i);
    letter_shapes.push_back(new Mesh(coords, 10, 4,
                                     indexes, 10,
                                     10,
                                     GL_LINE_STRIP, program_handle,
                                     "Letter s"));
                                     
}

void draw_scene() {
    // render
    // ------
    glClearColor(0.4f, 0.2f, 0.1f, 1.0f); // dark red/brown
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    grid.draw();

    lighter.set_camera(cam);
    lighter.set_color(glm::vec4(1, 0, 0, 1));

    letter_shapes[0]->draw();
    letter_shapes[1]->draw();
    letter_shapes[2]->draw();

    lighter.set_color(glm::vec4(1, 0.5, 0, 1));
    letter_shapes[3]->draw();

    lighter.set_color(glm::vec4(0, 1, 0, 1));
    letter_shapes[4]->draw();

    lighter.set_color(glm::vec4(0.2, 1, 0.5, 1));
    letter_shapes[5]->draw();

    lighter.set_color(glm::vec4(0, 1, 1, 1));
    letter_shapes[6]->draw();

    lighter.set_color(glm::vec4(0, .5, 1, 1));
    letter_shapes[7]->draw();

    lighter.set_color(glm::vec4(.5, 0, 1, 1));
    letter_shapes[8]->draw();

    lighter.set_color(glm::vec4(.6, 1, 0.5, 1));
    letter_shapes[9]->draw();

    lighter.set_color(glm::vec4(0, 1, 0.5, 1));
    letter_shapes[10]->draw();

    lighter.set_color(glm::vec4(0, 0, 1, 1));
    letter_shapes[11]->draw();

    glfwSwapBuffers(window);
    scene_changed = false;
}

int main(int argc, char **argv)
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();

    int display_width = 1000;
    int display_height = 200;

    GLFW_OpenGL_Window glfw_window(3, 3,
                                   display_width,
                                   display_height,
                                   "My Name",
                                   &framebuffer_size_callback);
    window = glfw_window.get_window();

    lighter.init();

    grid = Grid(0, 0, 10, 1,
                1, 0.1, lighter);

    cam = Camera(display_width, display_height);
    cam.init_2D(-0.5, -0.5, 11, 2);

    glfwGetFramebufferSize(glfw_window.get_window(),
                           &display_width,
                           &display_height);
    cam.resize_display(display_width, display_height);

    lighter.set_camera(cam);

    make_scene();

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
    return glfwGetKey(window, key_ID) == GLFW_PRESS;
}

// process all input: query GLFW whether relevant keys are pressed/released
// this frame, and react accordingly
// --------------------------------------------------------------
void processInput(GLFWwindow *w)
{
    if (key_pressed(GLFW_KEY_ESCAPE, w) ||
        key_pressed(GLFW_KEY_Q, w))
        glfwSetWindowShouldClose(w, true);
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
