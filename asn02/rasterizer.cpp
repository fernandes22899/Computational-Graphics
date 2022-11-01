#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/vec3.hpp" // glm::vec3
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, rotate
#include "glm/gtx/string_cast.hpp" // glm::to_string()

#include "mesh.hpp"
#include "shader.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "flat_lighter.hpp"
#include "mesh.hpp"

#define EPSILON 0.0001
using namespace std;

extern int add_vertex(float x, float y, float z,
                      float *coords, GLuint *indexes, int vert_index);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback( GLFWwindow* window, int button,
                            int action, int mods );
void mouse_motion_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);

Camera cam;
bool scene_changed = true;
GLFWwindow *window;
Flat_Lighter lighter;

float world_BL_x = 0;
float world_BL_y = 0;
float world_width = 1;
float world_height = 1;
int raster_width = 10;
int raster_height = 10;
float vertex_size;
bool active_line = false;

Mesh *pixel_shape;
Mesh *line_shape;

vector < Mesh* > pixels;
vector < pair<int, int> > rast_pair;

glm::vec2 line_begins;
glm::vec2 line_ends;
glm::vec2 mouse_pos;

void make_meshes(GLuint program_handle) {
    float *coords = new float[4 * 4];
    GLuint *indexes = new GLuint[4];

    float vertex_size = world_width / raster_width * 0.9;

    int i = 0;
    int row = 0;
    int col = 0;
    float x, y;

    for( int p = 0; p < 100; p++ ){
        x = col * (world_width / raster_width);
        y = row * (world_height / raster_height);
        i = add_vertex(x-(vertex_size / 2), y-(vertex_size / 2), 0, coords, indexes, i);
        i = add_vertex(x-(vertex_size / 2), y+(vertex_size / 2), 0, coords, indexes, i);
        i = add_vertex(x+(vertex_size / 2), y+(vertex_size / 2), 0, coords, indexes, i);
        i = add_vertex(x+(vertex_size / 2), y-(vertex_size / 2), 0, coords, indexes, i);
        pixels.push_back( new Mesh(coords, 4, 4,
                            indexes, 4,
                            4,
                            GL_TRIANGLE_FAN, program_handle,
                            "Pixel"));
        col++;
        if(col % 10 == 0){
            col = 0;
            row++;
        }
        i = 0;
    }

    i = 0;
    i = add_vertex(0, 0, 0, coords, indexes, i);
    i = add_vertex(0, 1, 0, coords, indexes, i);
    line_shape = new Mesh(coords, 2, 4,
                          indexes, 2,
                          2,
                          GL_LINE_STRIP, program_handle,
                          "Line Segment");

    delete[] coords;
    delete[] indexes;
}

void draw_scene() {
    // Draw the current scene.

    // Clear the image, and the depth buffer.
    glClearColor(0.4f, 0.2f, 0.1f, 1.0f); // dark red/brown
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    lighter.set_camera(cam);

    // Draw one pixel, at the bottom left, gray.
    lighter.set_color(glm::vec4(0.5, 0.5, 0.5, 1));
    float x = world_BL_x + world_width / raster_width / 2;
    float y = world_BL_y + world_height / raster_height / 2;
    glm::vec2 pixel_center = glm::vec2(x, y);
    glm::vec3 position(pixel_center, 0);
    glm::mat4 move = glm::translate(glm::mat4(1.0f), position);
    lighter.set_model_transform(move);

    pair<int, int> pix;
    int row_draw = 0;
    int col_draw = 0;
    
    for( int i = 0; i < pixels.size(); i++ ){
        lighter.set_color(glm::vec4(.5,.5,.5,1));

        pix = make_pair(col_draw, row_draw);
        for(int l = 0; l < rast_pair.size(); l++){
            if(rast_pair[l] == pix){
                lighter.set_color(glm::vec4(1,1,1,1));
                break;
            }
        }
        pixels[i]->draw();
        col_draw++;
        if(col_draw % 10 == 0){
            row_draw++;
            col_draw = 0;
        }
    }

    // And draw a black line
    lighter.set_color(glm::vec4(0,0,0,1));

    if(active_line){
        float *coords = new float[4];
        coords[0] = line_begins.x-.05;
        coords[1] = line_begins.y-.05;
        coords[2] = 0;
        coords[3] = 1;
        line_shape->update_vertex(coords, 0);
        coords[0] = line_ends.x-.05;
        coords[1] = line_ends.y-.05;
        coords[2] = 0;
        coords[3] = 1;
        line_shape->update_vertex(coords, 1);
    }
    line_shape->draw();

    // Bring the pixels to the front
    glfwSwapBuffers(window);
    scene_changed = false;
}

int main(int argc, char **argv)
{
    if (argc != 1) {
        std::cerr << "Usage:\n";
        std::cerr << "  rasterizer\n";
        std::cerr << "Press ENTER to exit program:";
        char reply[10];
        std::cin.getline(reply, 9);
        exit(1);
    }

    //test_pixel_math();

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();

    int display_width = 800;
    int display_height = 800;

    GLFW_OpenGL_Window glfw_window(3, 3,
                                   display_width,
                                   display_height,
                                   "Rasterizer",
                                   &framebuffer_size_callback);
    window = glfw_window.get_window();

    lighter.init();
    cam = Camera(display_width, display_height);
    cam.init_2D(world_BL_x, world_BL_y, world_width, world_height);

    glfwGetFramebufferSize(glfw_window.get_window(),
                           &display_width,
                           &display_height);
    cam.resize_display(display_width, display_height);

    lighter.set_camera(cam);

    make_meshes(lighter.get_program_handle());

    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_motion_callback);

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

void rast(glm::vec2 s, glm::vec2 e){
    rast_pair.clear();

    float x, y;
    float x1 = floor(s.x * 10);
    float x2 = floor(e.x * 10);
    float y1 = floor(s.y * 10);
    float y2 = floor(e.y * 10);
    float m = (y2 - y1) / (x2 - x1);
    float b = y1 - (m * x1);
    

    if(abs(m) > 1){
        if(y2 < y1){
            y2 = s.y * 10;
            y1 = e.y * 10;
        }
        for(int i = floor(y1); i <= floor(y2); i++){
            if(x1 == x2)
                x = x1;
            else
                x = (i - b) / m;
            rast_pair.push_back(make_pair(round(x), i));
        }
    }
    else{
        for(int i = floor(x1); i <= floor(x2); i++){
            y = (m * i) + b;
            rast_pair.push_back(make_pair(i, round(y)));
        }
    }
}


// glfw calls this every time the mouse moves
void mouse_motion_callback(GLFWwindow* window, double xpos, double ypos) {
    mouse_pos = cam.mouse_to_world(xpos, ypos);

    float xl = floor(mouse_pos.x * 10);
    float yd = floor(mouse_pos.y * 10);
    float x = (xl + 0.5) / 10;
    float y = (yd + 0.5) / 10;
    glm::vec2 v(x, y);

    // Rasterize the line, and set scene_changed to true
    line_ends = v;

    if(active_line){
        if(line_ends.x < line_begins.x){
            rast(line_ends,line_begins);
        }
        else{
            rast(line_begins, line_ends);
        }
        scene_changed = true;
    }

}


// glfw calls this every time the user presses/releases a mouse button
void mouse_button_callback( GLFWwindow* window, int button,
                            int action, int mods ) {
                                
    if (button != GLFW_MOUSE_BUTTON_LEFT)
        return;

    if (action == GLFW_PRESS)
    {
        active_line = true;
        // Mouse pressed. Save the start of the line.
        // Set scene_changed to true
        float xl = floor(mouse_pos.x * 10);
        float yd = floor(mouse_pos.y * 10);
        float x = (xl + 0.5) / 10;
        float y = (yd + 0.5) / 10;

        glm::vec2 v(x, y);
        
        line_begins = v;

        xl = floor(mouse_pos.x * 10);
        yd = floor(mouse_pos.y * 10);
        x = (xl + 0.5) / 10;
        y = (yd + 0.5) / 10;

        glm::vec2 w(x, y);
        line_ends = w;

        rast_pair.clear();
        scene_changed = true;

    }
    else {
        // Mouse released.  Stop making pixels.
        // Set scene_changed to true
        active_line = false;
        scene_changed = true;
    }
}


