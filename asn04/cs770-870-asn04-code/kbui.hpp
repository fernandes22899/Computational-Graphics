#ifndef KBUI_HPP
#define KBUI_HPP

#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <GLFW/glfw3.h>

using namespace std;

class Variable {
public:
    string _name;
    float *_value;
    float _min;
    float _max;
    float _increment;
    void (*_callback)(float);

    Variable(const string& name,
             float *value,
             float min,
             float max,
             float increment,
             void (*callback)(float));
};

class KBUI {
private:
    vector<Variable> variables;
    map<string, Variable*> var_from_name;
    size_t cur_variable;
    size_t max_name_len;

    void prev_variable();
    void next_variable();
    void increment();
    void decrement();
    void show_variable(const Variable& var);

public:
    KBUI();
    bool handle_key(int key);
    float get_float(const string& name);
    float get_bool(const string& name);
    void add_variable(const string& name,
                      float *value,
                      float min,
                      float max,
                      float increment,
                      void (*callback)(float) = NULL);
    void done_init();
};

#endif
