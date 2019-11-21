#ifndef BACKGAMMON_DIE_H
#define BACKGAMMON_DIE_H

#include "web/web.h"

#define COLOR_ACTIVE "#dddddd"
#define COLOR_INACTIVE "#999999"

#define X_CENTER (x + width / 2)
#define X_LEFT   (x + 1 * width / 5)
#define X_RIGHT  (x + 4 * width / 5)
#define Y_CENTER (y + width / 2)
#define Y_TOP    (y + 1 * width / 5)
#define Y_BOTTOM (y + 4 * width / 5)

class Die{
private:
    double x, y, width;
    size_t value;
    bool is_active = true;
    bool is_visible = true;
    double pip_radius;
public:
    Die(double x_, double y_, double width_):
            x(x_), 
            y(y_),
            width(width_),
            value(1),
            is_active(true){
        pip_radius = width / 10;
             
    }
    void Render(emp::web::Canvas& canvas){
        if(is_visible){
            canvas.Rect(x, y, width, width, (is_active ? COLOR_ACTIVE : COLOR_INACTIVE));
            switch(value){
                case 1:
                    canvas.Circle(X_CENTER, Y_CENTER, pip_radius, "#333333");
                    break;
                case 2:
                    canvas.Circle(X_LEFT,   Y_TOP, pip_radius, "#333333");
                    canvas.Circle(X_RIGHT,  Y_BOTTOM, pip_radius, "#333333");
                    break;
                case 3:
                    canvas.Circle(X_LEFT,   Y_TOP,      pip_radius, "#333333");
                    canvas.Circle(X_CENTER, Y_CENTER,   pip_radius, "#333333");
                    canvas.Circle(X_RIGHT,  Y_BOTTOM,   pip_radius, "#333333");
                    break;
                case 4:
                    canvas.Circle(X_LEFT,   Y_TOP,      pip_radius, "#333333");
                    canvas.Circle(X_LEFT,   Y_BOTTOM,   pip_radius, "#333333");
                    canvas.Circle(X_RIGHT,  Y_TOP,      pip_radius, "#333333");
                    canvas.Circle(X_RIGHT,  Y_BOTTOM,   pip_radius, "#333333");
                    break;
                case 5:
                    canvas.Circle(X_LEFT,   Y_TOP,      pip_radius, "#333333");
                    canvas.Circle(X_LEFT,   Y_BOTTOM,   pip_radius, "#333333");
                    canvas.Circle(X_RIGHT,  Y_TOP,      pip_radius, "#333333");
                    canvas.Circle(X_RIGHT,  Y_BOTTOM,   pip_radius, "#333333");
                    canvas.Circle(X_CENTER, Y_CENTER,   pip_radius, "#333333");
                    break;
                case 6:
                    canvas.Circle(X_LEFT,   Y_TOP,      pip_radius, "#333333");
                    canvas.Circle(X_LEFT,   Y_CENTER,   pip_radius, "#333333");
                    canvas.Circle(X_LEFT,   Y_BOTTOM,   pip_radius, "#333333");
                    canvas.Circle(X_RIGHT,  Y_TOP,      pip_radius, "#333333");
                    canvas.Circle(X_RIGHT,  Y_CENTER,   pip_radius, "#333333");
                    canvas.Circle(X_RIGHT,  Y_BOTTOM,   pip_radius, "#333333");
                    break;
                    
            }
        }
    }
    void SetActive(bool b){
        is_active = b;
    }
    void SetValue(size_t val){
        value = val;
    }
    void SetVisible(bool b){
        is_visible = b;
    }
    size_t GetValue(){
        return value;
    }
};

#endif
