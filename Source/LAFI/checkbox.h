#ifndef LAFI_CHECKBOX_INCLUDED
#define LAFI_CHECKBOX_INCLUDED

#include "widget.h"

class lafi_checkbox : public lafi_widget {
public:
    bool checked;
    string text;
    
    void check();
    void uncheck();
    void set(bool value);
    
    void init();
    
    lafi_checkbox(int x1 = 0, int y1 = 0, int x2 = 1, int y2 = 1, string text = "", bool checked = false, lafi_style* style = NULL, unsigned char flags = 0);
    ~lafi_checkbox();
    void widget_on_left_mouse_click(int x, int y);
    
    void draw_self();
};

class lafi_checkbox_box : public lafi_widget {
private:

public:
    bool checked;
    
    lafi_checkbox_box(int x1 = 0, int y1 = 0, bool checked = false, lafi_style* style = NULL, unsigned char flags = 0);
    ~lafi_checkbox_box();
    
    void draw_self();
};

#endif //ifndef LAFI_CHECKBOX_INCLUDED