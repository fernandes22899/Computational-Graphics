#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtx/string_cast.hpp>
#include <cmath>
#include <iostream>

using namespace std;

pair <float, float> find_t(float a, float b, float d){
    std::cout << "a: " << a << endl;
    std::cout << "b: " << b << endl;    
    std::cout << "d: " << d << endl;
    float t1 = -b - sqrt(d) / 2*a;
    float t2 = -b + sqrt(d) / 2*a;
    return make_pair(t1, t2);
}

bool obeys_triangle_constraints(float t, float u, float v){
    return (t>=0) && (u>=0) && (u<=1) && (v>=0) && (v<=1) && (u+v>=0) && (u+v<=1);
}

int main() {

    //Sphere
    glm::vec3 center(1, 0, 1);
    float radius = 2;

    //Variables for the triangle
    glm::vec3 A(3, 0, -1);
    glm::vec3 B(3, 0, 1);
    glm::vec3 C(1, 3, 2);    
    
    //Variables for the camera specifications
    glm::vec3 eye(6, 1, 0);
    glm::vec3 lookat(0, 1, 0);
    glm::vec3 vup(0, 1, 0);
    
    float clipLeft = -2; 
    float clipRight = 2;
    float clipBottom = -1;
    float clipTop = 1;
    float clipNear = 2;

    int W = 301;
    int H = 201;

    glm::vec3 P_dcs(150, 100, 0);

    //Q1
    float delta_x = (clipRight - clipLeft) / W;
    float delta_y = (clipTop - clipBottom) / H;

    float x_vcs = clipLeft + ((P_dcs.x + 0.5) * delta_x);
    float y_vcs = clipBottom + ((P_dcs.y + 0.5) * delta_y);
    float z_vcs = -clipNear;

    std::cout << x_vcs << " " << y_vcs << " " << z_vcs << endl;

    glm::vec3 P_vcs(x_vcs, y_vcs, z_vcs);

    //Q2
    glm::vec3 n = glm::normalize(eye-lookat); //Z_vcs
    glm::vec3 r = glm::normalize(glm::cross(vup,n)); //X_vcs
    glm::vec3 u = glm::cross(n, r); //Y_vcs

    //Q3
    glm::mat4 vcs_to_wcs(r.x, r.y, r.z, 0,
                        u.x, u.y, u.z, 0,
                        n.x, n.y, n.z, 0,
                        eye.x, eye.y, eye.z, 1);
                 

    //Q4
    glm::vec3 P_wcs = vcs_to_wcs * glm::vec4(P_vcs, 1);

    //Q5
    glm::vec3 V_wcs = glm::normalize(P_wcs - eye); //V

    //Q6
    float a = glm::dot(V_wcs, V_wcs);
    float b = 2 * glm::dot((V_wcs), (P_wcs-center));
    float c = (glm::dot((P_wcs-center), (P_wcs-center))) - radius*radius;    

    //Find d    
    float d = (b*b) - (4*a*c);
    float t1;
    float t2;
    float t_sphere = 0.0;
    bool hit_sphere = true;
    if (d < 0){
        hit_sphere = false;
    }
    else{
        pair<float, float> t_pair = find_t(a,b,d);        
        t1 = t_pair.first;
        t2 = t_pair.second;
        std::cout << "t1: " << t1 << endl;
        std::cout << "t2: " << t2 << endl;
        if (t1 >= 0){
            t_sphere = t1;
        }
        else if (t2 > 0){
            t_sphere = t2;            
        }
        else{
            hit_sphere = false;
        }
    }

    glm::vec3 p_sphere = P_wcs + t_sphere*V_wcs;

    //Q7
    glm::vec3 N_p_sphere = glm::normalize(p_sphere - center); 

    //Q8
    glm::mat3 D(V_wcs.x, V_wcs.y, V_wcs.z,
                (A.x-B.x),(A.y-B.y),(A.z-B.z),
                (A.x-C.x),(A.y-C.y),(A.z-C.z));

    glm::mat3 D_t((A.x-P_wcs.x), (A.y-P_wcs.y), (A.z-P_wcs.z),
                (A.x-B.x), (A.y-B.y), (A.z-B.z),
                (A.x-C.x), (A.y-C.y), (A.z-C.z));
              
    glm::mat3 D_u(V_wcs.x, V_wcs.y, V_wcs.z,
                (A.x-P_wcs.x),(A.y-P_wcs.y),(A.z-P_wcs.z),
                (A.x-C.x),(A.y-C.y),(A.z-C.z));

    glm::mat3 D_v(V_wcs.x, V_wcs.y, V_wcs.z,
                (A.x-B.x),(A.y-B.y),(A.z-B.z),
                (A.x-P_wcs.x),(A.y-P_wcs.y),(A.z-P_wcs.z));

    float t_triangle = glm::determinant(D_t) / glm::determinant(D);
    float u_triangle = glm::determinant(D_u) / glm::determinant(D);
    float v_triangle = glm::determinant(D_v) / glm::determinant(D);

    glm::vec3 left_side_matrix_eq = (t_triangle*v_triangle) + (u_triangle*(A-B)) + (v_triangle*(A-C));
    glm::vec3 right_side_matrix_eq = A-P_wcs;

    bool hit_triangle = true;
    if (!obeys_triangle_constraints(t_triangle,u_triangle,v_triangle)){
        hit_triangle = false;
    }

    glm::vec3 p_triangle = P_wcs + t_triangle*V_wcs;
    
    //Q9
    glm::vec3 N_p_triangle = glm::normalize(glm::cross((B-A),(C-A)));                                                                

    //Q10
    string first_hit;
    if (!hit_sphere && !hit_triangle){
        first_hit = "neither (constraints violated)";
    }
    else{
        if (t_triangle < t_sphere){
        first_hit = "triangle first";
        }    
        else{
            first_hit = "sphere first";
        }
    }
    

    //Suggested output format:
    std::cout << "\nQuestion 1:\n";
    std::cout << "Pixel in VCS:  " << glm::to_string(P_vcs) << "\n";

    std::cout << "\nQuestion 2:\n";
    std::cout << "3 Basis Vectors:" << endl;    
    std::cout << "X_vcs: " << glm::to_string(r) << endl;
    std::cout << "Y_vcs: " << glm::to_string(u) << endl;
    std::cout << "Z_vcs: " << glm::to_string(n) << endl;

    std::cout << "\nQuestion 3:\n";
    std::cout << "M_VCS_to_WCS: " << glm::to_string(vcs_to_wcs) << endl;

    std::cout << "\nQuestion 4:\n";
    std::cout << "P_WCS: " << glm::to_string(P_wcs) << endl;

    std::cout << "\nQuestion 5:\n";
    std::cout << "V: " << glm::to_string(V_wcs) << endl;

    std::cout << "\nQuestion 6:\n";
    std::cout << "a: " << a << endl;
    std::cout << "b: " << b << endl;
    std::cout << "c: " << c << endl;
    std::cout << "d: " << d << endl;
    std::cout << "t: " << t_sphere << endl;
    std::cout << "hit point: " << glm::to_string(p_sphere) << endl;
    
    std::cout << "\nQuestion 7:\n";
    std::cout << "Surface normal at hit point: " << glm::to_string(N_p_sphere) << endl;

    std::cout << "\nQuestion 8:\n";
    std::cout << "Left-hand side of matrix equation: (tV + u(A−B) + v(A−C)) = " << glm::to_string(left_side_matrix_eq) << endl;
    std::cout << "Right-hand side of matrix equation: (A-S) = " << glm::to_string(right_side_matrix_eq) << endl;
    std::cout << "t: " << t_triangle << endl;
    std::cout << "u: " << u_triangle << endl;
    std::cout << "v: " << v_triangle << endl;
    std::cout << "Hit point: " << glm::to_string(p_triangle) << endl;

    std::cout << "\nQuestion 9:\n";
    std::cout << "Surface normal at hit point: " << glm::to_string(N_p_triangle) << endl;

    std::cout << "\nQuestion 10:\n";
    std::cout << "Ray hits " << first_hit << endl;
}
