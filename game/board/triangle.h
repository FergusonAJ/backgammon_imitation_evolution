#ifndef BACKGAMMON_TRIANGLE_H
#define BACKGAMMON_TRIANGLE_H

// Local 
#include "./token.h"
#include "./constants.h"
// Empirical
#include "web/web.h"
// Standard library
#include <stack>


class Triangle{
public: 
    
private:
    double base_x, base_y, half_width, height;
    bool is_upper;
    bool is_selected = false;
    emp::web::CanvasPolygon& canvas_shape;
    std::string color_fill;
    std::string color_outline;
    bool is_option = false;
    std::stack<size_t> token_id_stack; 
    int controlling_player_id = -1;

public:
    Triangle(double x, double y, double half_w, double h, bool upper, 
            emp::web::CanvasPolygon& shape,
            std::string fill, std::string outline = "#000000"):
        base_x(x), 
        base_y(y), 
        half_width(half_w),
        height(h),
        is_upper(upper),
        canvas_shape(shape),
        color_fill(fill), 
        color_outline(outline){
    }

    void Render(emp::web::Canvas& canvas){
        canvas_shape.MoveTo(base_x, base_y);
        canvas_shape.SetFillColor(is_option ? "#00ff00" : (is_selected ? "#00aa00" : color_fill));
        canvas_shape.SetLineColor(color_outline);
        canvas.Draw(canvas_shape);
    }
    bool CheckClick(double x, double y){
        if((is_upper && y >= base_y && y <= base_y + height) ||
                (!is_upper && y >= base_y - height && y <= base_y)){
            if(x >= base_x - half_width && x <= base_x + half_width){
                return true;
            }
        }
        return false;
        // Code below is a much more strict (only the actual triangle)
        /*
        double ratio; // Represents how close the mouse is to the base (as a fraction of height)
        if(is_upper){
            if(y >= base_y && y <= base_y + height)
                ratio = (height - (y - base_y)) / height;
            else
                return false;
        }
        else{
            if(y <= base_y && y >= base_y - height)
                ratio = (height - (base_y - y)) / height;
            else
                return false;
        }
        double x_min = base_x - half_width * ratio;
        double x_max = base_x + half_width * ratio;
        if( x >= x_min && x <= x_max)
            return true;
        return false;
        */
    }
    void SetSelect(bool b){
        is_selected = b;
    }
    void SetIsOption(bool b){
        is_option = b;
    }
    bool GetIsOption(){
        return is_option;
    }
    double GetBaseX(){
        return base_x;
    }
    double GetBaseY(){
        return base_y;
    }
    double GetNextTokenY(double token_radius){
        if(is_upper) // Initial offset + cur diameters
            return base_y + (2 * token_id_stack.size() + 1) * token_radius;
        // Same math, just the other direction
        return base_y - (2 * token_id_stack.size() + 1) * token_radius; 
    }
    void AddToken(size_t token_id, int player_id){
        token_id_stack.push(token_id); 
        controlling_player_id = player_id;
    }
    size_t RemoveToken(){
        size_t val = token_id_stack.top();
        token_id_stack.pop();
        if(token_id_stack.size() == 0){
            controlling_player_id = -1;
        }
        return val;
    }
    bool GetIsUpper(){
        return is_upper;
    }
    int GetTopTokenId(){
        return (token_id_stack.size() > 0 ? token_id_stack.top() : -1);
    }
    int GetControllingPlayerId(){
        return controlling_player_id;
    }
    size_t GetNumTokens(){
        return token_id_stack.size();
    }
};

#endif
