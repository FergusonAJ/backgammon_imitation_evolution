#include "web/web.h"
#include <cmath>


class AnimationController : public emp::web::Animate{
private:
    emp::web::Document doc;
    emp::web::Canvas canvas;
    size_t canvas_width = 800;
    size_t canvas_height = 600;
    void DrawLineSegment(double x, double y, double theta, double length){
        if(length > 4){
            float x2 = x + cos(theta) * length;
            float y2 = y - sin(theta) * length;
            canvas.Line(x, y, x2, y2);
            DrawLineSegment(x2, y2, theta + emp::PI / 4, length * 0.7); 
            DrawLineSegment(x2, y2, theta - emp::PI / 4, length * 0.7); 
        }
    }
public :
    AnimationController() : doc("emp_base"){
        canvas = doc.AddCanvas(canvas_width, canvas_height, "test_canvas");
        targets.push_back(canvas);
        DrawLineSegment(canvas_width / 2, canvas_height, emp::PI/2, canvas_height / 4);
    }
};

AnimationController anim;
int main(){

}
