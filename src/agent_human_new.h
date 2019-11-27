////    Human controlled Backgammon agent that renders to a web browser
////    Part of a term project for MSU's CSE 841 - Artificial Intelligence
////    Austin Ferguson - 2019

#ifndef BACKGAMMON_AGENT_HUMAN_NEW_H
#define BACKGAMMON_AGENT_HUMAN_NEW_H


//Local 
#include "./agent_base.h"
#include "./ui/token.h"
#include "./ui/triangle.h"
#include "./ui/die.h"
#include "./ui/ui_constants.h"
//Empirical
#include "web/web.h"
#include "tools/Random.h"
//Standard
#include <cmath>
#include <string>
#include <queue>

//TODO: Pull out some vars as constants
class BackgammonAgent_Human : public BackgammonAgent_Base, public emp::web::Animate{
private:    
    emp::web::Document doc;
    emp::web::Canvas canvas;
    emp::web::Div button_div;
    size_t canvas_width = 800;
    size_t canvas_height = 600;
    double token_radius = canvas_width / 40;
    emp::web::CanvasPolygon triangle_upper_shape, triangle_lower_shape;
    std::string color_tri_1 = "#995555";
    std::string color_tri_2 = "#999999";
    std::string color_board = "#e1cf9f";
    std::string color_player_1 = "#aa5522";
    std::string color_player_2 = "#555555";

    emp::vector<Token> token_vec;
    emp::vector<Triangle> triangle_vec;
    emp::vector<Die> dice_vec;
    emp::vector<emp::web::Button> button_vec;

    std::ostringstream oss;
    
    BackgammonState cur_state;
    std::function<void(BackgammonMove)> cur_callback;
    std::queue<BackgammonMove> pending_moves;
        
    bool draw_arrow = false;
    double arrow_start_x, arrow_start_y, arrow_end_x, arrow_end_y;
    double arrow_wing_1_x, arrow_wing_1_y, arrow_wing_2_x, arrow_wing_2_y;
    
    virtual void PromptTurn(
            const BackgammonState& state, 
            std::function<void(BackgammonMove)> callback) override{
        cur_state = BackgammonState(state);
        cur_callback = callback;
        DisplayMoves();
        UpdateFromState();
    }

    virtual void Notify(
            const emp::vector<BackgammonMove>& moves, 
            const emp::vector<int>& dice_vals) override{
        UpdateDice(dice_vals);
        for(BackgammonMove move : moves){
            pending_moves.push(move); 
        }
    }

    void SendMove(size_t move_idx){
        cur_callback(cur_state.possible_moves[move_idx]);
    }
   
    void UpdateDice(const emp::vector<int>& dice_vals){
        dice_vec[0].SetValue(dice_vals[0] > 0 ? dice_vals[0] : -1 * dice_vals[0]);
        dice_vec[0].SetActive(dice_vals[0] > 0);
        dice_vec[0].SetVisible(true);
        dice_vec[1].SetValue(dice_vals[1] > 0 ? dice_vals[1] : -1 * dice_vals[1]);
        dice_vec[1].SetActive(dice_vals[1] > 0);
        dice_vec[1].SetVisible(true);
        if(dice_vals.size() == 4 && dice_vals[2] != 0 && dice_vals[3] != 0){
            dice_vec[2].SetValue(dice_vals[2] > 0 ? dice_vals[2] : -1 * dice_vals[2]);
            dice_vec[2].SetActive(dice_vals[2] > 0);
            dice_vec[2].SetVisible(true);
            dice_vec[3].SetValue(dice_vals[3] > 0 ? dice_vals[3] : -1 * dice_vals[3]);
            dice_vec[3].SetActive(dice_vals[3] > 0);
            dice_vec[3].SetVisible(true);
        }
        else{
            dice_vec[2].SetActive(false);
            dice_vec[2].SetVisible(false);
            dice_vec[3].SetActive(false);
            dice_vec[3].SetVisible(false);
        }
    }   
   
    void UpdateFromState(){
        UpdateDice(cur_state.dice_vals);
        token_vec.clear();
        for(size_t triangle_idx = 0; triangle_idx < triangle_vec.size(); ++triangle_idx){
            triangle_vec[triangle_idx].ClearAllTokens();
            size_t num_points = cur_state.tokens_per_point[triangle_idx + 1] >= 0 ?
                    cur_state.tokens_per_point[triangle_idx + 1] : 
                    cur_state.tokens_per_point[triangle_idx + 1] * -1;
            bool is_player_1 = cur_state.tokens_per_point[triangle_idx + 1] > 0;
            for(size_t i = 0; i < num_points; ++i){
                token_vec.emplace_back(
                        triangle_vec[triangle_idx].GetBaseX(),
                        triangle_vec[triangle_idx].GetNextTokenY(token_radius),
                        token_radius,
                        (is_player_1) ? 1 : 2,
                        triangle_idx,
                        is_player_1 ? color_player_1 : color_player_2 
                );
                triangle_vec[triangle_idx].AddToken(token_vec.size() - 1, (is_player_1 ? 1 : 2));
            }
        }    
    }

    void PlaceArrow(size_t move_idx){
        draw_arrow = true;
        const BackgammonMove& move = cur_state.possible_moves[move_idx];
        if(move.start > 0){
            arrow_start_x = triangle_vec[move.start - 1].GetBaseX(); 
            arrow_start_y = triangle_vec[move.start - 1].GetNextTokenY(token_radius) +
                (move.start > 12 ? -1 * token_radius * 2 : token_radius * 2);
        }
        else{
            arrow_start_x = canvas_width / 2; 
            arrow_start_y = canvas_height / 2;
        }
        if(move.end > 0){
            arrow_end_x = triangle_vec[move.end - 1].GetBaseX(); 
            arrow_end_y = triangle_vec[move.end - 1].GetNextTokenY(token_radius);
        }
        else{
            arrow_end_x = canvas_width - canvas_width / 30.0;
            arrow_end_y = canvas_height / 2;
        }
        double diff_x = arrow_end_x - arrow_start_x;
        double diff_y = arrow_end_y - arrow_start_y;
        double magnitude = sqrt(diff_x * diff_x + diff_y * diff_y);
        diff_x /= magnitude;
        diff_y /= magnitude;
        arrow_wing_1_x = arrow_end_x - diff_x * 10 + diff_y * 10;
        arrow_wing_1_y = arrow_end_y - diff_y * 10 - diff_x * 10;
        arrow_wing_2_x = arrow_end_x - diff_x * 10 - diff_y * 10;
        arrow_wing_2_y = arrow_end_y - diff_y * 10 + diff_x * 10;
    }

    void DisplayMoves(){
        button_vec.clear();
        button_div.ClearChildren();
        button_div.Clear();
        for(size_t move_idx = 0; move_idx < cur_state.possible_moves.size(); ++move_idx){
            const BackgammonMove& move = cur_state.possible_moves[move_idx];
            oss.str("");
            oss << move.start << " -> " << move.end; 
            std::string btn_text = oss.str();
            oss.str("");
            oss << "move_btn_" << move_idx;
            emp::web::Button btn([this, move_idx](){
                    this->SendMove(move_idx);
                },
                btn_text,
                oss.str()
            );
            button_div << btn << "  ";
            btn.On("mouseover", [this, move_idx](){
                this->PlaceArrow(move_idx);
            });
            btn.On("mouseout", [this](){
                this->draw_arrow = false;
            });
            button_vec.push_back(btn);     
        }
        doc.Redraw();
    }
         
    // Setup all necessary variables for the triangles on the game board
    void InitTriangles(double width, double height){
        // Prepare triangle canvas shapes
        triangle_upper_shape.AddPoint(-width / 2, 0);
        triangle_upper_shape.AddPoint(0, height);
        triangle_upper_shape.AddPoint(width / 2, 0);
        triangle_lower_shape.AddPoint(-width / 2, 0);
        triangle_lower_shape.AddPoint(0, -height);
        triangle_lower_shape.AddPoint(width / 2, 0);
        // Create all necessary triangles
        std::string cur_tri_color;
        for(size_t tri_id = 0; tri_id < 24; tri_id++){
            cur_tri_color = (tri_id & 1 ? color_tri_1 : color_tri_2);
            if(tri_id < 12){ // First 12 = lower half (right to left)
                triangle_vec.emplace_back(
                    (13 - tri_id + (tri_id >= 6 ? -1 : 0)) * canvas_width / 15 + canvas_width / 30, 
                    canvas_height, 
                    width / 2,
                    height, 
                    false,
                    triangle_lower_shape,
                    cur_tri_color);
            }
            else{ // Second half (upper half) (left to right)
                triangle_vec.emplace_back(
                    (tri_id - 11 + (tri_id >= 18 ? 1 : 0)) * canvas_width / 15 + canvas_width / 30, 
                    0,
                    width / 2,
                    height, 
                    true,
                    triangle_upper_shape, 
                    cur_tri_color);
            }
        }
    }
    // Place the dice
    void InitDice(){
        double margin = ((canvas_width / 15) - (canvas_width / 16)) / 2;
        for(size_t i = 0; i < 4; ++i){
            dice_vec.emplace_back(
                    margin,
                    i * canvas_width / 15 + margin,
                    canvas_width / 16);  
        }   
    }
    // Render the game board
    void Render(){
        canvas.Font(oss.str());
        canvas.Rect(0, 0, canvas_width, canvas_height, color_board);
        for(Triangle & triangle : triangle_vec){
            triangle.Render(canvas);
        }
        canvas.Rect(0, 0, 
                canvas_width / 15, canvas_height, "#000000");
        canvas.Rect(14 * canvas_width / 15, 0, 
                canvas_width / 15, canvas_height, "#000000");
        canvas.Rect(7 * canvas_width / 15, 0, 
                canvas_width / 15, canvas_height, "#000000");
        for(Token & token : token_vec)
            token.Render(canvas);
  
        // Tokens in goal
        if(cur_state.tokens_finished_agent_1 > 0){
            canvas.Circle(
                    canvas_width - canvas_width / 30.0, 
                    canvas_height / 2 - token_radius * 2, 
                    token_radius, 
                    color_player_1);
            canvas.CenterText( 
                    canvas_width - canvas_width / 30.0, 
                    canvas_height / 2 - token_radius * 2, 
                    std::to_string(cur_state.tokens_finished_agent_1), 
                    "#ffffff");
        }
        if(cur_state.tokens_finished_agent_2 > 0){
            canvas.Circle(
                    canvas_width - canvas_width / 30.0, 
                    canvas_height / 2 + token_radius * 2, 
                    token_radius, 
                    color_player_2);
            canvas.CenterText(
                    canvas_width - canvas_width / 30.0, 
                    canvas_height / 2 + token_radius * 2, 
                    std::to_string(cur_state.tokens_finished_agent_2), 
                    "#ffffff");
        }
        // Tokens off the board
        if(cur_state.tokens_off_agent_1 > 0){
            canvas.Circle(
                    canvas_width / 2, 
                    (canvas_height / 2) - (2 * token_radius), 
                    token_radius, 
                    color_player_1);
            canvas.CenterText(
                    canvas_width / 2, 
                    (canvas_height / 2) - (2 * token_radius), 
                    std::to_string(cur_state.tokens_off_agent_1), 
                    "#ffffff");
        }
        if(cur_state.tokens_off_agent_2 > 0){
            canvas.Circle(
                    canvas_width / 2, 
                    (canvas_height / 2) + (2 * token_radius), 
                    token_radius, 
                    color_player_2);
            canvas.CenterText(
                    canvas_width / 2, 
                    (canvas_height / 2) + (2 * token_radius), 
                    std::to_string(cur_state.tokens_off_agent_2), 
                    "#ffffff");
        }
        // Draw the dice!
        DrawDice();
        // Draw the arrow, if applicable
        if(draw_arrow){
            canvas.Line(arrow_start_x, arrow_start_y, arrow_end_x, arrow_end_y, "#ff0000", 5);
            canvas.Line(arrow_end_x, arrow_end_y, arrow_wing_1_x, arrow_wing_1_y, "#ff0000", 5);
            canvas.Line(arrow_end_x, arrow_end_y, arrow_wing_2_x, arrow_wing_2_y, "#ff0000", 5);
            canvas.Line(arrow_wing_1_x, arrow_wing_1_y, arrow_wing_2_x, arrow_wing_2_y,"#ff0000", 5);
        }
    } 
    // Draw the dice appropriately
    void DrawDice(){
        /*
        if(dice_1 == dice_2){
            dice_vec[0].SetActive(dice_left > 0);
            dice_vec[1].SetActive(dice_left > 1);
            dice_vec[2].SetActive(dice_left > 2);
            dice_vec[3].SetActive(dice_left > 3);
        }
        else{
            dice_vec[0].SetActive(!dice_1_used);
            dice_vec[1].SetActive(!dice_2_used);
        }
        */
        for(Die & die : dice_vec){
            die.Render(canvas);
        }
    }   
    // Callback function that determines what we do when the canvas updates
    void Update(double delta_time){
        canvas.Clear();
        Render();
    }
    // Resets all triangles to their original colors
    void ClearTriangleColors(){
        for(Triangle & triangle : triangle_vec){
            triangle.SetSelect(false);
            triangle.SetIsOption(false);
        }
    }
    // Resets all tokens to their original colors
    void ClearTokenColors(){
        for(Token & token : token_vec){
            token.SetSelect(false);
        }
    }

public :
    BackgammonAgent_Human() : 
            doc("emp_base"),
            triangle_upper_shape(0,0),
            triangle_lower_shape(0,0){
        EM_ASM(console.log('Test!'););
        // Initialize the document and canvas
        canvas = doc.AddCanvas(canvas_width, canvas_height, "test_canvas");
        doc << "<br/>";
        button_div = doc.AddDiv("button_div");
        targets.push_back(canvas);
        // Set callbacks
        SetCallback( [this](double delta_time) mutable{
            this->Update(delta_time);
        });
        oss << (int)token_radius << "px foo";
        // Set up the game board
        InitTriangles(canvas_width / 15.0, canvas_height / 3.0);
        InitDice();
        // Draw the screen!
        Render();
        // Enable automatic canvas redrawing
        Start();
    }
};

#endif
