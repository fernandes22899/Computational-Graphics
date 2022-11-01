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
#include "grid.hpp"
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
Grid grid;
Flat_Lighter lighter;

const float vertex_size = 0.10;
int active_vertex = -1;
Mesh *vertex_shape;
Mesh *edge_crossing_shape;
Mesh *line_segment_shape;

glm::vec3 mouse_pos;

vector<Mesh*> clipping_window;
vector<Mesh*> vertex_collection;
vector<glm::vec2> vertices; //vertex points
vector< pair<int, int> > edges; //edge indexes
vector<glm::vec2> clipped_vertices; //clipped vertex points
vector< pair<int, int> > clipped_edges; //clipped edge indexes

void make_meshes(GLuint program_handle) {
    float *coords = new float[4 * 4];
    GLuint *indexes = new GLuint[4];

    int i = 0;

    //Make Clipping Window
    i = add_vertex(-1, -2, 0, coords, indexes, i);
    i = add_vertex(-1, 2, 0, coords, indexes, i);
    clipping_window.push_back(new Mesh(coords, 2, 4,
                                indexes, 2,
                                2,
                                GL_LINE_STRIP, program_handle,
                                "Edge")); // Left Bound
    i = 0;
    i = add_vertex(1, -2, 0, coords, indexes, i);
    i = add_vertex(1, 2, 0, coords, indexes, i);
    clipping_window.push_back(new Mesh(coords, 2, 4,
                                indexes, 2,
                                2,
                                GL_LINE_STRIP, program_handle,
                                "Edge")); // Right Bound
    i = 0;
    i = add_vertex(-2, 0.5, 0, coords, indexes, i);
    i = add_vertex(2, 0.5, 0, coords, indexes, i);
    clipping_window.push_back(new Mesh(coords, 2, 4,
                                indexes, 2,
                                2,
                                GL_LINE_STRIP, program_handle,
                                "Edge")); // Top Bound
    i = 0;
    i = add_vertex(-2, -0.5, 0, coords, indexes, i);
    i = add_vertex(2, -0.5, 0, coords, indexes, i);
    clipping_window.push_back(new Mesh(coords, 2, 4,
                                indexes, 2,
                                2,
                                GL_LINE_STRIP, program_handle,
                                "Edge")); // Bottom Bound                                                                                                
    
    //Vertex Shape
    i = 0;
    i = add_vertex(-vertex_size / 2, -vertex_size / 2, 0, coords, indexes, i);
    i = add_vertex(-vertex_size / 2, +vertex_size / 2, 0, coords, indexes, i);
    i = add_vertex(+vertex_size / 2, +vertex_size / 2, 0, coords, indexes, i);
    i = add_vertex(+vertex_size / 2, -vertex_size / 2, 0, coords, indexes, i);

    vertex_shape = new Mesh(coords, 4, 4,
                            indexes, 4,
                            4,
                            GL_TRIANGLE_FAN, program_handle,
                            "Vertex");

    //Clipped Vertex Shape
    i = 0;
    float cross_size = vertex_size * 0.7;
    i = add_vertex(-cross_size / 2, -cross_size / 2, 0, coords, indexes, i);
    i = add_vertex(-cross_size / 2, +cross_size / 2, 0, coords, indexes, i);
    i = add_vertex(+cross_size / 2, +cross_size / 2, 0, coords, indexes, i);
    i = add_vertex(+cross_size / 2, -cross_size / 2, 0, coords, indexes, i);

    edge_crossing_shape = new Mesh(coords, 4, 4,
                                   indexes, 4,
                                   4,
                                   GL_TRIANGLE_FAN, program_handle,
                                   "Crossing");
    //Edge Shape                                   
    i = 0;
    i = add_vertex(0, 0, 0, coords, indexes, i);
    i = add_vertex(0, 1, 0, coords, indexes, i);

    line_segment_shape = new Mesh(coords, 2, 4,
                                   indexes, 2,
                                   2,
                                   GL_LINE_STRIP, program_handle,
                                   "Edge");
                                      
    delete[] coords;
    delete[] indexes;
}

void draw_scene() {
    //Draw the current scene

    //Erase the screen and depth buffer.
    glClearColor(0.4f, 0.2f, 0.1f, 1.0f); // dark red/brown
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Draw the background grid.
    grid.draw();

    lighter.set_camera(cam);

    //Draw yellow clipping window
    lighter.set_color(glm::vec4(1, 1, 0, 1));
    for (int i = 0; i < clipping_window.size(); i++){
        clipping_window[i]->draw();
    }

    //Draw Edges
    lighter.set_color(glm::vec4(1, 0, 0, 1));
    if (edges.size() > 0){
        float *coords = new float[4];
        coords[0] = 0;
        coords[1] = 0;
        coords[2] = 0;
        coords[3] = 1;
        for (int i = 0; i < edges.size(); i++){
            coords[0] = vertices[edges[i].first].x;
            coords[1] = vertices[edges[i].first].y;
            line_segment_shape->update_vertex(coords, 0);
            coords[0] = vertices[edges[i].second].x;
            coords[1] = vertices[edges[i].second].y;
            line_segment_shape->update_vertex(coords, 1);
            line_segment_shape->draw();
        }
        delete[] coords;
    }

    //Draw clipped edges
    lighter.set_color(glm::vec4(0, 1, 0, 1));
    if (clipped_edges.size() > 0){
        float *coords = new float[4];
        coords[0] = 0;
        coords[1] = 0;
        coords[2] = 0;
        coords[3] = 1;
        for (int i = 0; i < clipped_edges.size(); i++){
            coords[0] = clipped_vertices[clipped_edges[i].first].x;
            coords[1] = clipped_vertices[clipped_edges[i].first].y;
            line_segment_shape->update_vertex(coords, 0);
            coords[0] = clipped_vertices[clipped_edges[i].second].x;
            coords[1] = clipped_vertices[clipped_edges[i].second].y;
            line_segment_shape->update_vertex(coords, 1);
            line_segment_shape->draw();
        }
        delete[] coords;
    }
    
    //Draw vertices
    lighter.set_color(glm::vec4(1, 0, 0, 1));    
    for (int i = 0; i < vertices.size(); i++){
        glm::vec3 position(vertices[i].x,vertices[i].y,0);
        glm::mat4 move = glm::translate(glm::mat4(1.0f), position);
        lighter.set_model_transform(move);
        vertex_shape->draw();
    }

    //Draw clipped vertices
    lighter.set_color(glm::vec4(0, 1, 0, 1));
    for (int i = 0; i < clipped_vertices.size(); i++){
        glm::vec3 position(clipped_vertices[i].x,clipped_vertices[i].y,0);
        glm::mat4 move = glm::translate(glm::mat4(1.0f), position);
        lighter.set_model_transform(move);
        edge_crossing_shape->draw();
    }    

    //Bring the pixels to the front.
    glfwSwapBuffers(window);
    scene_changed = false;
}

int main(int argc, char **argv)
{
    if (argc != 1) {
        std::cerr << "Usage:\n";
        std::cerr << "  clipper\n";
        std::cerr << "Press ENTER to exit program:";
        char reply[10];
        std::cin.getline(reply, 9);
        exit(1);
    }
    //glfw: initialize and configure
    glfwInit();

    int display_width = 800;
    int display_height = 800;


    std::string title(argv[0]);
    for (int i = 1; i < argc; i++) {
        title += std::string(" ") + argv[i];
    }

    GLFW_OpenGL_Window glfw_window(3, 3,
                                   display_width,
                                   display_height,
                                   title.c_str(),
                                   &framebuffer_size_callback);
    window = glfw_window.get_window();

    lighter.init();

    mouse_pos = glm::vec3(0, 0, 0);

    grid = Grid(-1, -1, 4, 4,
                1, 0.1, lighter);

    cam = Camera(display_width, display_height);
    cam.init_2D(-1.5, -1.5, 3, 3);

    glfwGetFramebufferSize(glfw_window.get_window(),
                           &display_width,
                           &display_height);
    cam.resize_display(display_width, display_height);

    lighter.set_camera(cam);

    make_meshes(lighter.get_program_handle());

    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_motion_callback);

    //render loop
    scene_changed = true;
    while (!glfwWindowShouldClose(window))
    {
        //input
        processInput(window);
        if (scene_changed) {

            draw_scene();

        }
        glfwWaitEvents();
    }

    //glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

bool key_pressed(unsigned int key_ID, GLFWwindow *window) {
    return glfwGetKey(window, key_ID) == GLFW_PRESS;
}

//process all input: query GLFW whether relevant keys are pressed/released
void processInput(GLFWwindow *w)
{
    if (key_pressed(GLFW_KEY_ESCAPE, w) ||
        key_pressed(GLFW_KEY_Q, w))
        glfwSetWindowShouldClose(w, true);

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    cam.resize_display(width, height);
    lighter.set_camera(cam);
}

//Helper function to find "N"
glm::vec2 get_N (glm::vec2 A, glm::vec2 B){
    float x = A.x - B.x;
    float y = A.y - B.y;
    glm::vec2 N(-y, x);
    return N;
}

glm::vec2 sub_vertices(glm::vec2 A, glm::vec2 B){
    float x = A.x - B.x;
    float y = A.y - B.y;
    glm::vec2 C(x, y);
    return C;
}

glm::vec2 get_intersect(glm::vec2 A, glm::vec2 B, float t){
    glm::vec2 v(A.x + (t*(B.x-A.x)), A.y + (t*(B.y-A.y)));
    return v;
}

//Helper function to calculate "T"
float calculate_T(glm::vec2 N, glm::vec2 A, glm::vec2 B, glm::vec2 Q){
    float denom = glm::dot(N, sub_vertices(A, B));
    if (denom != 0){ // make sure not dividing by 0 (lines are parallel)
        return glm::dot(N, sub_vertices(A, Q)) / denom;
    }
    else {
        return -1.0; // CHECK HOW THIS IS HANDLED IN uncross.cpp !!!!!!
    }    
}

vector<glm::vec2> clip_bound(glm::vec2 N, glm::vec2 Q, vector<glm::vec2> v){
    vector<glm::vec2> ret;
    float cur;
    float next;

    int cur_index = 0;
    int next_index = 1;
    float t;

    bool clip = true; //flag

    while (clip){

        // calculate N dot (P - Q) 
        if (next_index == v.size()){ // set next index to 0 to complete polygon at end
            next_index = 0;
            clip = false;
        }
        cur = glm::dot(N, sub_vertices(v[cur_index], Q));
        next = glm::dot(N, sub_vertices(v[next_index], Q));

        // IN IN         
        if ((cur >= 0) && (next >= 0)){            
            ret.push_back(v[cur_index]); // keep current vertex
        }
        // IN OUT
        else if ((cur >= 0) && (next < 0)){            
            ret.push_back(v[cur_index]); // keep current vertex
            
            // find intersection
            t = calculate_T(N, v[cur_index], v[next_index], Q);
            ret.push_back(get_intersect(v[cur_index], v[next_index], t)); // keep intersection
        }
        // OUT IN
        else if ((cur < 0) && (next >= 0)){            
            // find intersection
            t = calculate_T(N, v[cur_index], v[next_index], Q);
            ret.push_back(get_intersect(v[cur_index], v[next_index], t)); // keep intersection
        }
        // OUT OUT
        else{
            ;
        }
        cur_index++;
        next_index++;
    } 
    return ret;
}

vector< pair<int, int> > make_edges(vector<glm::vec2> v){
    vector< pair<int, int> > ret;
    for (int i = 0; i < v.size()-1; i++){                
        ret.push_back(make_pair(i, i+1));
    }
    if (v.size() > 2){
        ret.push_back(make_pair(v.size()-1, 0));                      
    }
    return ret;
}

void clip_polygon(){
    clipped_vertices = vertices;

    //Left Bound    
    glm::vec2 AL(-1, -2);
    glm::vec2 BL(-1, 2);
    if (clipped_vertices.size() > 0){        
        clipped_vertices = clip_bound(get_N(AL, BL), AL, clipped_vertices);
    }

    //Right Bound      
    glm::vec2 AR(1, 2);
    glm::vec2 BR(1, -2);
    if (clipped_vertices.size() > 0){        
        clipped_vertices = clip_bound(get_N(AR, BR), AR, clipped_vertices);
    }

    //Top Bound    
    glm::vec2 AT(-2, 0.5);
    glm::vec2 BT(2, 0.5);
    if (clipped_vertices.size() > 0){        
        clipped_vertices = clip_bound(get_N(AT, BT), AT, clipped_vertices);
    }

    //Bottom Bound    
    glm::vec2 AB(2, -0.5);
    glm::vec2 BB(-2, -0.5);
    if (clipped_vertices.size() > 0){
        clipped_vertices = clip_bound(get_N(AB, BB), AB, clipped_vertices);
    }

    //Connect Edges    
    clipped_edges.clear(); //clear old clipped edges
    if (clipped_vertices.size() > 1){ //create new clipped edges if needed
        clipped_edges = make_edges(clipped_vertices);
    } 
}

void mouse_motion_callback(GLFWwindow* window, double xpos, double ypos) {
    mouse_pos = cam.mouse_to_world(xpos, ypos);

    if (active_vertex != -1){
        //Edit Unclipped Polygon            
        float new_x = mouse_pos.x;
        float new_y = mouse_pos.y;        
        vertices[active_vertex].x = new_x;
        vertices[active_vertex].y = new_y;

        // Edit Clipped Polygon
        clip_polygon();

        scene_changed = true;              
    }
}

void mouse_button_callback( GLFWwindow* window, int button,
                            int action, int mods ) {
    if (button != GLFW_MOUSE_BUTTON_LEFT)
        return;

    if (action == GLFW_PRESS)
    {
        //Check if mouse is on a polygon vertex.
        //If so, make the vertex active.
        //If not, add a vertex to the
        
        if (vertices.size() != 0){            
            float left_bound;
            float right_bound;
            float top_bound;
            float bottom_bound;        
            //go through vertices and check if mouse is within one of their bounds
            for (int i = 0; i < vertices.size(); i++){
                // calculate bounds of vertex box
                left_bound = vertices[i].x-(vertex_size/2);
                right_bound = vertices[i].x+(vertex_size/2);
                top_bound = vertices[i].y+(vertex_size/2);
                bottom_bound = vertices[i].y-(vertex_size/2);
                    
                if ((mouse_pos.x > left_bound) && (mouse_pos.x < right_bound) && (mouse_pos.y < top_bound) && (mouse_pos.y > bottom_bound)){                
                    active_vertex = i; //set active vertex
                }                
            }
        }
        if (active_vertex == -1){

            //Edit Unclipped Polygon            
            //add vertex
            glm::vec2 v(mouse_pos.x, mouse_pos.y);
            vertices.push_back(v);

            //Make edges
            edges = make_edges(vertices);
            clip_polygon(); // clip
            scene_changed = true; // draw_scene()
        }        
    }
    else {
        // mouse released.
        active_vertex = -1;
    }
}
