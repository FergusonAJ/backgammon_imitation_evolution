#ifndef BACKGAMMON_UI_TOKEN_H
#define BACKGAMMON_UI_TOKEN_H

#include "web/web.h"

#include "./ui_constants.h"

class Token{
private:
    double cur_x, cur_y, radius;
    size_t player_id;
    size_t triangle_id;
    std::string color_fill;
    std::string color_outline;
    bool is_selected = false;
    bool is_active = false;
public:
    Token(double x, double y, double rad, size_t player_num, size_t tri_id, std::string fill, 
            std::string outline = "#000000"):
        cur_x(x), 
        cur_y(y),
        radius(rad), 
        player_id(player_num),
        triangle_id(tri_id), 
        color_fill(fill), 
        color_outline(outline),
        is_active(true){ 
    }
    void Render(emp::web::Canvas& canvas){
        if(is_active){
            if(!is_selected)
                canvas.Circle(cur_x, cur_y, radius, color_fill, color_outline);
            else
                canvas.Circle(cur_x, cur_y, radius, SELECTED_COLOR, color_outline);
        }   
            
    }
    bool CheckClick(double x, double y){
        if((cur_x - x) * (cur_x - x) + (cur_y - y) * (cur_y - y) <= radius * radius)
            return true;
        return false;
    }
    void SetSelect(bool b){
        is_selected = b;
    }
    size_t GetTriangleId(){
        return triangle_id;
    }
    void SetTriangleId(size_t id){
        triangle_id = id;
    }
    void MoveTo(double x, double y){
        cur_x = x;
        cur_y = y;
    }
    size_t GetPlayerId(){
        return player_id;
    }       
    void SetActive(bool b){
        is_active = b;
    }
    double GetX(){
        return cur_x;
    }
    double GetY(){
        return cur_y;
    }
};

#endif
