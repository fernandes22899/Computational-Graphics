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

#include <sstream>
#include <bitset>
#define PI 3.14159265358979323846
using namespace std;

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
float vertex_size = 0.1;


vector<glm::vec2> vertices; //vertex points
vector<Mesh*> vertex_collection;
vector<Mesh*> intersection_collection;
vector<Mesh*> edge_collection;
pair<double, double> mouse_position; //position of mouse
int active_vertex = -1; //flag for activation of vertex
vector< pair<int, int> > edges; //edge indexes
glm::vec2 get_N (glm::vec2 A, glm::vec2 B); 
float calculate_T(glm::vec2 N, glm::vec2 A, glm::vec2 Q, glm::vec2 B); 
glm::vec2 sub_vertices(glm::vec2 A, glm::vec2 B); 
glm::vec2 get_intersect(glm::vec2 A, glm::vec2 B, float t); 


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

    // reset vertex_collection & edge_collection & intersection_collection
    vertex_collection.clear();
    edge_collection.clear();
    intersection_collection.clear();

    int i = 0;
    float s = vertex_size / 2;
    float s_int = s / 2;

    //add vertices
    for (int j = 0; j < vertices.size(); j++){
        i = add_vertex(vertices[j].x-s, vertices[j].y-s, 0, coords, indexes, i);
        i = add_vertex(vertices[j].x+s, vertices[j].y-s, 0, coords, indexes, i);
        i = add_vertex(vertices[j].x+s, vertices[j].y+s, 0, coords, indexes, i);
        i = add_vertex(vertices[j].x-s, vertices[j].y+s, 0, coords, indexes, i);
        vertex_collection.push_back(new Mesh(coords, 4, 4,
                                indexes, 4,
                                4,
                                GL_TRIANGLE_FAN, program_handle,
                                "VERTEX"));
        i = 0;
    }

    //add edges
    i = 0;
    for (int j = 0; j < edges.size(); j++){        
        i = add_vertex(vertices[edges[j].first].x, vertices[edges[j].first].y, 0, coords, indexes, i);
        i = add_vertex(vertices[edges[j].second].x, vertices[edges[j].second].y, 0, coords, indexes, i);
        edge_collection.push_back(new Mesh(coords, 2, 4,
                                     indexes, 2,
                                     2,
                                     GL_LINE_STRIP, program_handle,
                                     "EDGE"));
        i = 0;
    }

    // add all intersections
    i = 0;
    // go through edges and find intersection points
    for (int j = 0; j < edges.size(); j++){
        for (int l = 0; l < edges.size(); l++){ // check against every other edge
            if ((edges[j].first != edges[l].first) || (edges[j].second != edges[l].second)) { // don't compare same edge
                // turn one implicit (j)
                glm::vec2 N = get_N(vertices[edges[j].first], vertices[edges[j].second]); // N                
                glm::vec2 Q(vertices[edges[j].first].x, vertices[edges[j].first].y); // Q               
                // if 0 <= t <= 1 ... (0.001 <= t <= 1 - 0.001 ... need buffer bc vertices can't intersect)
                float t1 = calculate_T(N, vertices[edges[l].first], vertices[edges[l].second], Q);
                if ((t1 >= 0.001) && (t1 <= 1 - 0.001)){
                    // turn other implicit (l) and find t
                    glm::vec2 N = get_N(vertices[edges[l].first], vertices[edges[l].second]); // N                
                    glm::vec2 Q(vertices[edges[l].first].x, vertices[edges[l].first].y); // Q
                    float t2 = calculate_T(N, vertices[edges[j].first], vertices[edges[j].second], Q);
                    // if 0 <= t <= 1 ...
                    if ((t2 >= 0.001) && (t2 <= 1 - 0.001)){ //intersection                        
                        glm::vec2 intersect = get_intersect(vertices[edges[j].first], vertices[edges[j].second], t2);
                        i = add_vertex(intersect.x-s_int, intersect.y-s_int, 0, coords, indexes, i);
                        i = add_vertex(intersect.x+s_int, intersect.y-s_int, 0, coords, indexes, i);
                        i = add_vertex(intersect.x+s_int, intersect.y+s_int, 0, coords, indexes, i);
                        i = add_vertex(intersect.x-s_int, intersect.y+s_int, 0, coords, indexes, i);
                        intersection_collection.push_back(new Mesh(coords, 4, 4,
                                                indexes, 4,
                                                4,
                                                GL_TRIANGLE_FAN, program_handle,
                                                "INTERSECT"));
                        i = 0;
                    }                        
                }                     
            }
        }
    }
}

glm::vec2 get_N (glm::vec2 A, glm::vec2 B){
    float x = A.x - B.x;
    float y = A.y - B.y;
    glm::vec2 N(-y, x);
    return N;
}

float calculate_T(glm::vec2 N, glm::vec2 A, glm::vec2 B, glm::vec2 Q){
    float denom = glm::dot(N, sub_vertices(A, B));
    if (denom != 0){ // make sure not dividing by 0 (lines are parallel)
        return glm::dot(N, sub_vertices(A, Q)) / denom;
    }
    else {
        return -1.0;
    }    
}

glm::vec2 get_intersect(glm::vec2 A, glm::vec2 B, float t){
    glm::vec2 v(A.x + (t*(B.x-A.x)), A.y + (t*(B.y-A.y)));
    return v;
}

glm::vec2 sub_vertices(glm::vec2 A, glm::vec2 B){
    float x = A.x - B.x;
    float y = A.y - B.y;
    glm::vec2 C(x, y);
    return C;
}

void draw_scene() {
    // render
    // ------

    // check if there are any intersections
    if (intersection_collection.size() == 0){
        glClearColor(0.0f, 0.5f, 0.0f, 1.0f); // green
    }
    else {
        glClearColor(0.4f, 0.2f, 0.1f, 1.0f);  // dark red/brown
    }
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    grid.draw();

    //glm::vec3 position(0.5, 0.5, 0);
    glm::vec3 position(0, 0, 0);
    glm::mat4 move = glm::translate(glm::mat4(1.0f), position);
    lighter.set_model_transform(move);

    // Draw edges
    lighter.set_color(glm::vec4(1, 1, 0, 0.5));
    for (int l = 0; l < edge_collection.size(); l++){
        edge_collection[l]->draw();        
    }

    // Draw vertices
    for (int i = 0; i < vertex_collection.size(); i++){
        string binary = bitset<3>(i).to_string();        
        lighter.set_color(glm::vec4(binary[0] - 48, binary[1] - 48, binary[2] - 48, 1)); // spec colors
        //lighter.set_color(glm::vec4(binary[2] - 48, binary[1] - 48, binary[0] - 48, 1)); // example img colors
        vertex_collection[i]->draw();        
    }

    lighter.set_color(glm::vec4(1, 1, 1, 1));
    for (int k = 0; k < intersection_collection.size(); k++){
        intersection_collection[k]->draw();        
    }

    glfwSwapBuffers(window);
    scene_changed = false;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "Usage:\n";
        std::cerr << "  uncross <graph-file.txt>\n";
        std::cerr << "Press ENTER to exit program:";
        char reply[10];
        std::cin.getline(reply, 9);
        exit(1);
    }

    // Collect graph information from text file
    int num_vertices = 0;
    int num_edges = 0;
    
    int line_counter = 0;
    fstream newfile;
    newfile.open(argv[1],ios::in); //open a file to perform read operation using file object
    if (newfile.is_open()){   //checking whether the file is open
        string tp;
        while(getline(newfile, tp)){ //read data from file object and put it into string.            
            line_counter++;
            if (line_counter == 1) { // # vertices
                num_vertices = stoi(tp);
            }
            else if (line_counter == 2){ // # edges
                num_edges = stoi(tp);
            }        
            else { // edge info
                vector<int> vertices;
                istringstream iss(tp);
                string vertex;
                while (iss >> vertex){                    
                    vertices.push_back(stoi(vertex));
                }
                edges.push_back(make_pair(vertices[0], vertices[1]));
            }                  
        }
        newfile.close(); //close the file object.
    }

    // Create vector of vertices    
    for (int i = 0; i < num_vertices; i++){
        float theta = i*((2*PI)/num_vertices);        
        //glm::vec2 v(cos(theta)-0.5, sin(theta)-0.5); // -0.5 bias ???
        glm::vec2 v(cos(theta), sin(theta)); 
        vertices.push_back(v);
    }   

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();

    int display_width = 800;
    int display_height = 800;

    GLFW_OpenGL_Window glfw_window(3, 3,
                                   display_width,
                                   display_height,
                                   "Uncross",
                                   &framebuffer_size_callback);
    window = glfw_window.get_window();

    lighter.init();

    grid = Grid(-1, -1, 4, 4,
                1, 0.1, lighter);

    cam = Camera(display_width, display_height);
    cam.init_2D(-1.5, -1.5, 3, 3);

    glfwGetFramebufferSize(glfw_window.get_window(),
                           &display_width,
                           &display_height);
    cam.resize_display(display_width, display_height);

    lighter.set_camera(cam);

    make_scene();

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
    cam.resize_display(width, height);
    lighter.set_camera(cam);
}

// glfw calls this every time the mouse moves
void mouse_motion_callback(GLFWwindow* window, double xpos, double ypos) {
    // Save the mouse position.
    mouse_position = make_pair(xpos, ypos);
    glm::vec3 coords = cam.mouse_to_world(xpos, ypos);

    // If a vertex is active, change its position.
    if (active_vertex != -1){            
        float new_x = coords.x;
        float new_y = coords.y;        
        vertices[active_vertex].x = new_x;
        vertices[active_vertex].y = new_y;
                
        make_scene(); // remake scene        
        scene_changed = true; // trigger draw_scene()                
    }    
}

// glfw calls this every time the user presses/releases a mouse button
void mouse_button_callback( GLFWwindow* window, int button,
                            int action, int mods ) {
    if (button != GLFW_MOUSE_BUTTON_LEFT)
        return;

    if (action == GLFW_PRESS)
    {
        glm::vec3 coords = cam.mouse_to_world(mouse_position.first, mouse_position.second);
        float left_bound;
        float right_bound;
        float top_bound;
        float bottom_bound;
        
        // go through vertices and check if mouse is within one of their bounds
        for (int i = 0; i < vertices.size(); i++){
            // calculate bounds of vertex box
            left_bound = vertices[i].x-(vertex_size/2);
            right_bound = vertices[i].x+(vertex_size/2);
            top_bound = vertices[i].y+(vertex_size/2);
            bottom_bound = vertices[i].y-(vertex_size/2);
   
            //if ((coords.x-0.5 > left_bound) && (coords.x-0.5 < right_bound) && (coords.y-0.5 < top_bound) && (coords.y-0.5 > bottom_bound)){ // bias
            if ((coords.x > left_bound) && (coords.x < right_bound) && (coords.y < top_bound) && (coords.y > bottom_bound)){                
                active_vertex = i; // set active vertex
            }                
        }
    }
    else if (action == GLFW_RELEASE) {
        // deactivate vertex
        active_vertex = -1;
    }
}
