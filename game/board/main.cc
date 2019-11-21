//Local 
#include "./token.h"
#include "./triangle.h"
#include "./die.h"
#include "./constants.h"
//Empirical
#include "web/web.h"
#include "tools/Random.h"
//Standard
#include <cmath>
#include <string>

//TODO: Pull out some vars as constants
class AnimationController : public emp::web::Animate{
private:
    emp::web::Document doc;
    emp::web::Canvas canvas;
    emp::Random rand;
    size_t canvas_width = 800;
    size_t canvas_height = 600;
    emp::web::CanvasPolygon triangle_upper_shape, triangle_lower_shape;
    std::string color_tri_1 = "#995555";
    std::string color_tri_2 = "#999999";
    std::string color_board = "#e1cf9f";
    std::string color_player_1 = "#aa5522";
    std::string color_player_2 = "#555555";
    emp::vector<Token> token_vec_player_1;
    emp::vector<Token> token_vec_player_2;
    emp::vector<Triangle> triangle_vec;
    emp::vector<Die> dice_vec;
    int selected_token_idx = -1;
    int selected_triangle_idx = -1;
    double token_radius = canvas_width / 40;
    size_t dice_1, dice_2;
    bool dice_1_used, dice_2_used;
    size_t dice_left;
    size_t tokens_finished_player_1 = 0;
    size_t tokens_finished_player_2 = 0;
    size_t tokens_off_player_1 = 0;
    size_t tokens_off_player_2 = 0;
    std::ostringstream oss;
    bool do_draw_goal = false;
    bool game_over = false;
    // Tweening
    size_t tween_token_idx;
    double tween_start_x, tween_start_y, tween_end_x, tween_end_y; 
    bool is_tweening = false;
    double tween_time_max = 500; //in ms
    double tween_time_cur = 0;
    double tween_time_start = 0;

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
        for(Token & token : token_vec_player_1){
            token.Render(canvas);
        }
        for(Token & token : token_vec_player_2){
            token.Render(canvas);
        }
  
        // Tokens in goal
        if(tokens_finished_player_1 > 0){
            canvas.Circle(
                    canvas_width - canvas_width / 30.0, 
                    canvas_height / 2 - token_radius * 2, 
                    token_radius, 
                    color_player_1);
            canvas.CenterText( 
                    canvas_width - canvas_width / 30.0, 
                    canvas_height / 2 - token_radius * 2, 
                    std::to_string(tokens_finished_player_1), 
                    "#ffffff");
        }
        if(tokens_finished_player_2 > 0){
            canvas.Circle(
                    canvas_width - canvas_width / 30.0, 
                    canvas_height / 2 + token_radius * 2, 
                    token_radius, 
                    color_player_2);
            canvas.CenterText(
                    canvas_width - canvas_width / 30.0, 
                    canvas_height / 2 + token_radius * 2, 
                    std::to_string(tokens_finished_player_2), 
                    "#ffffff");
        }
        // Tokens off the board
        if(tokens_off_player_1 > 0){
            canvas.Circle(
                    canvas_width / 2, 
                    (canvas_height / 2) - (2 * token_radius), 
                    token_radius, 
                    color_player_1);
            canvas.CenterText(
                    canvas_width / 2, 
                    (canvas_height / 2) - (2 * token_radius), 
                    std::to_string(tokens_off_player_1), 
                    "#ffffff");
        }
        if(tokens_off_player_2 > 0){
            canvas.Circle(
                    canvas_width / 2, 
                    (canvas_height / 2) + (2 * token_radius), 
                    token_radius, 
                    color_player_2);
            canvas.CenterText(
                    canvas_width / 2, 
                    13 * token_radius, 
                    std::to_string(tokens_off_player_2), 
                    "#ffffff");
        }
        // Draw goal if its reachable
        if(do_draw_goal){
            canvas.Rect(14 * canvas_width / 15, 0, canvas_width / 15, token_radius * 10, 
                    SELECTED_COLOR);
        }
        // Draw the dice!
        DrawDice();
    } 
    // Draw the dice appropriately
    void DrawDice(){
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
        for(Die & die : dice_vec){
            die.Render(canvas);
        }
    }   
    // Places a token at the given triangle (for start only)
    void PlaceToken(size_t triangle_idx, bool first_player, size_t count = 1){
        for(size_t i = 0; i < count; ++i){
            if(first_player){
                token_vec_player_1.emplace_back(
                    triangle_vec[triangle_idx].GetBaseX(),
                    triangle_vec[triangle_idx].GetNextTokenY(token_radius),
                    token_radius, 1, triangle_idx, color_player_1);
                triangle_vec[triangle_idx].AddToken(token_vec_player_1.size() - 1, 1);
            }
            else{
                token_vec_player_2.emplace_back(
                    triangle_vec[triangle_idx].GetBaseX(),
                    triangle_vec[triangle_idx].GetNextTokenY(token_radius),
                    token_radius, 2, triangle_idx, color_player_2);
                triangle_vec[triangle_idx].AddToken(token_vec_player_2.size() - 1, 2);
            }
        }
    }    
    // Place the tokens as defined at the start of a Backgammon game
    void PlaceInitialTokens(){
        // Player
        PlaceToken(11, true,    5);
        PlaceToken(18, true,    5);
        PlaceToken(16, true,    3);
        PlaceToken(0,  true,    2);
        // AI    
        PlaceToken(12, false,   5);
        PlaceToken(5,  false,   5);
        PlaceToken(7,  false,   3);
        PlaceToken(23, false,   2);
    }    
    // Callback function that determines what we do when the canvas updates
    void Update(double delta_time){
        if(dice_1_used && dice_2_used && dice_left == 0)
            RollDice();
        if(is_tweening)
            UpdateTween();
        if(tokens_finished_player_1 >= 15 || tokens_finished_player_2)
            game_over = true;
        canvas.Clear();
        Render();
    }
    // Update moving tokens
    void UpdateTween(){
        tween_time_cur = emp::GetTime() - tween_time_start;
        double pct = tween_time_cur / tween_time_max;
        if(pct > 1){
            pct = 1;
            is_tweening = false;
        }
        token_vec_player_1[tween_token_idx].MoveTo(
                tween_start_x + pct * (tween_end_x - tween_start_x), 
                tween_start_y + pct * (tween_end_y - tween_start_y)); 
    }
    //Handle mouse click events
    void OnClick(emp::web::MouseEvent evt){
        if(is_tweening){
            return;
        }
        double mouse_x = evt.clientX - canvas.GetXPos();
        double mouse_y = evt.clientY - canvas.GetYPos();
        // Check to see if goal was clicked
        if(do_draw_goal && selected_token_idx != -1 && 
                mouse_x >= 14 * canvas_width / 15 && mouse_y <= token_radius * 10){
            FinishToken();
        }
        do_draw_goal = false;
        // Check to see if a triangle was clicked
        selected_triangle_idx = -1;
        for(size_t tri_idx = 0; tri_idx < triangle_vec.size(); ++tri_idx){
            triangle_vec[tri_idx].SetSelect(false);
            if(triangle_vec[tri_idx].CheckClick(mouse_x,mouse_y)){
                selected_triangle_idx = tri_idx;
            }
        }
        // If a valid triangle was selected, move! 
        if(selected_triangle_idx != -1){
            if(selected_token_idx != -1 && triangle_vec[selected_triangle_idx].GetIsOption()) 
                MoveToken(selected_token_idx, selected_triangle_idx);
            else{
                int top_token_id = triangle_vec[selected_triangle_idx].GetTopTokenId();
                if (top_token_id >= 0 && 
                    triangle_vec[selected_triangle_idx].GetControllingPlayerId() == 1)
                    SelectToken(top_token_id);
            }
        }
        else{
            ClearTriangleColors();
            ClearTokenColors();
            selected_token_idx = -1;
        }
    }
    // All logic that occurs when the user clicks on a token
    void SelectToken(size_t token_idx){
        for(Token& token : token_vec_player_1){
            token.SetSelect(false);
        }
        selected_token_idx = token_idx;
        token_vec_player_1[token_idx].SetSelect(true);
        ClearTriangleColors();
        MarkOptions();
    }
    // Marks the triangles that are valid moves
    void MarkOptions(){
        size_t cur_tri_idx = token_vec_player_1[selected_token_idx].GetTriangleId();
        if(!dice_1_used || (dice_1 == dice_2 && dice_left > 0))
            TryMarkTriangle(cur_tri_idx + dice_1);
        if(!dice_2_used)
            TryMarkTriangle(cur_tri_idx + dice_2);
        do_draw_goal = CheckGoalReachable();
        // TODO: Decide if we're keeping this
        //          If we don't allow double moves (must do them separately, we have less to code)
        //if(!dice_1_used && !dice_2_used){
        //    TryMarkTriangle(cur_tri_idx + dice_1 + dice_2);
        //}
        //if(dice_1 == dice_2){ // If you roll doubles you get 4 moves
        //    for(size_t i = 1; i < dice_left + 1; ++i)
        //        TryMarkTriangle(cur_tri_idx + (i * dice_1));
        //}
    }
    // Attempts to mark a triangle, checking to see if it's in range and if the AI controls it
    void TryMarkTriangle(size_t triangle_id){
        if(triangle_id < triangle_vec.size()){
            if(triangle_vec[triangle_id].GetControllingPlayerId() == 2){
                if(triangle_vec[triangle_id].GetNumTokens() == 1)
                    triangle_vec[triangle_id].SetIsOption(true);
            }
            else{
                triangle_vec[triangle_id].SetIsOption(true);
            }
        }
    }
    // Checks to see if the current token can reach the goal (and game is to that point)
    bool CheckGoalReachable(){
        if(selected_token_idx == -1)
            return false;
        if(!CheckPlayerRunoff())
            return false;
        size_t cur_tri_idx = token_vec_player_1[selected_token_idx].GetTriangleId();
        if(cur_tri_idx + dice_1  > 23 && !dice_1_used)
            return true;
        if(cur_tri_idx + dice_2  > 23 && !dice_2_used)
            return true;
        if(cur_tri_idx + dice_1  > 23 && (dice_1 == dice_2 && dice_left > 0))
            return true;
        return false;
    }
    // Checks to see if all of the player's tokens are in the last quadrant
    bool CheckPlayerRunoff(){
        for(size_t tri_idx = 0; tri_idx < 18; ++tri_idx){
            if(triangle_vec[tri_idx].GetControllingPlayerId() == 1)
                return false;
        }
        return true;
    }   
    // Roll two d6's.
    void RollDice(){
        dice_1 =  rand.GetUInt(1, 7);
        dice_2 =  rand.GetUInt(1, 7);
        dice_1_used = false;
        dice_2_used = false;
        dice_left = 2;
        dice_vec[0].SetValue(dice_1);
        dice_vec[1].SetValue(dice_2);
        //doc << "(" << dice_1 << ", " << dice_2 << ") "; 
        if(dice_1 == dice_2){
            dice_left = 4;
            dice_1_used = true;
            dice_2_used = true;
            dice_vec[2].SetValue(dice_1);
            dice_vec[3].SetValue(dice_1);
            dice_vec[2].SetVisible(true);
            dice_vec[3].SetVisible(true);
        }
        else{
            dice_vec[2].SetVisible(false);
            dice_vec[3].SetVisible(false);
        }      
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
        for(Token & token : token_vec_player_1){
            token.SetSelect(false);
        }
    }
    // Move the specified token to the specified triangle
    void MoveToken(int token_idx, int triangle_idx){
        if(token_idx != -1 && triangle_idx != -1){
            if(triangle_vec[triangle_idx].GetIsOption()){
                bool valid_move = false;
                size_t cur_triangle_idx = token_vec_player_1[token_idx].GetTriangleId();
                if(dice_1 == dice_2){
                    int diff = triangle_idx - cur_triangle_idx;
                    if(diff % dice_1 == 0 && diff / dice_1 <= dice_left){
                        valid_move = true;
                        dice_left -= diff / dice_1;
                    }
                }
                else if(!dice_1_used && triangle_idx == cur_triangle_idx + dice_1){
                    valid_move = true;
                    dice_1_used = true;
                    dice_left--; 
                }
                else if(!dice_2_used && triangle_idx == cur_triangle_idx + dice_2){
                    valid_move = true;
                    dice_2_used = true; 
                    dice_left--; 
                }
                else if(!dice_1_used && !dice_2_used && 
                        triangle_idx == cur_triangle_idx + dice_1 + dice_2){
                    valid_move = true; 
                    dice_1_used = true;
                    dice_2_used = true;
                    dice_left -= 2; 
                }
                if(valid_move){
                    if(triangle_vec[triangle_idx].GetControllingPlayerId() == 2){
                        token_vec_player_2[triangle_vec[triangle_idx].GetTopTokenId()]
                                .SetActive(false);
                        triangle_vec[triangle_idx].RemoveToken();
                        tokens_off_player_2++;
                    }
                    triangle_vec[token_vec_player_1[token_idx].GetTriangleId()].RemoveToken(); 
                    /*
                    token_vec_player_1[token_idx].MoveTo(
                        triangle_vec[triangle_idx].GetBaseX(), 
                       - triangle_vec[triangle_idx].GetNextTokenY(token_radius));
                    */
                    StartTween(token_idx, triangle_idx);
                    token_vec_player_1[token_idx].SetTriangleId(triangle_idx);
                    triangle_vec[triangle_idx].AddToken(token_idx, 1); 
                    selected_token_idx = -1;
                    selected_triangle_idx = -1;
                    ClearTriangleColors();
                    ClearTokenColors();
                }
            }
        }
    }
    // Starts a token moving
    void StartTween(size_t token_idx, size_t triangle_idx){
        tween_time_cur = 0.0;
        tween_time_start = emp::GetTime();
        is_tweening = true;
        tween_token_idx = token_idx;
        tween_start_x = token_vec_player_1[token_idx].GetX();
        tween_start_y = token_vec_player_1[token_idx].GetY();
        tween_end_x = triangle_vec[triangle_idx].GetBaseX(), 
        tween_end_y = triangle_vec[triangle_idx].GetNextTokenY(token_radius);
    }
    // Moves a token out of play and increments bookkeeping vars
    void FinishToken(){
        size_t cur_tri_idx = token_vec_player_1[selected_token_idx].GetTriangleId();
        // Update dice
        if(dice_1 != dice_2){
            if(!dice_1_used && cur_tri_idx + dice_1 > 23){
                if(!dice_2_used && cur_tri_idx + dice_2 > 23 && dice_2 < dice_1)
                    dice_2_used = true;
                else
                    dice_1_used = true;
            }
            else if(!dice_2_used && cur_tri_idx + dice_2 > 23)
                dice_2_used = true;
        }
        dice_left--;
        // Record keeping
        triangle_vec[cur_tri_idx].RemoveToken(); 
        token_vec_player_1[selected_token_idx].SetActive(false);
        tokens_finished_player_1++;
        selected_token_idx = -1;
        selected_triangle_idx = -1;
        ClearTriangleColors();
    }   

    // ******** DEBUGGING FUNCTIONS ********
    // Just for testing - Draw a map of which pixels register a hit
    void DrawCollisionMap(){
        emp::web::CanvasPolygon pixel;
        pixel.AddPoint(0,0);
        pixel.AddPoint(0,1);
        pixel.AddPoint(1,1);
        pixel.AddPoint(1,0);
        pixel.SetFillColor("#FF0000");
        pixel.SetLineColor("#FF0000");
        for(size_t x = 0; x < canvas_width; ++x){
            for(size_t y = 0; y < canvas_height; ++y){
                bool hit = false;   
                for(Triangle & triangle : triangle_vec){
                    if(triangle.CheckClick(x,y)){
                        hit = true;
                        break;
                    }       
                }
                if(hit){
                    pixel.MoveTo(x, y);
                    canvas.Draw(pixel);
                }
            }
        }
        pixel.SetFillColor("#00FF00");
        pixel.SetLineColor("#00FF00");
        for(size_t x = 0; x < canvas_width; ++x){
            for(size_t y = 0; y < canvas_height; ++y){
                bool hit = false;   
                for(Token & token : token_vec_player_1){
                    if(token.CheckClick(x,y)){
                        hit = true;
                        break;
                    }       
                }
                if(hit){
                    pixel.MoveTo(x, y);
                    canvas.Draw(pixel);
                }
            }
        }
    
    }
    // Used to test runoff conditions
    void PlaceInitialTokens_Runoff(){
        // Player
        PlaceToken(18, true,    5);
        PlaceToken(19, true,    5);
        PlaceToken(20, true,    3);
        PlaceToken(21,  true,    1);
        PlaceToken(17,  true,    1);
        // AI    
        PlaceToken(12, false,   5);
        PlaceToken(5,  false,   5);
        PlaceToken(7,  false,   3);
        PlaceToken(23, false,   2);
    }    

public :
    AnimationController() : 
            doc("emp_base"),
            rand(), 
            triangle_upper_shape(0,0),
            triangle_lower_shape(0,0){
        // Initialize the document and canvas
        canvas = doc.AddCanvas(canvas_width, canvas_height, "test_canvas");
        doc << "<br/>";
        targets.push_back(canvas);
        // Set callbacks
        canvas.On("mousedown", [this](emp::web::MouseEvent evt) mutable{
            this->OnClick(evt);
        });
        SetCallback( [this](double delta_time) mutable{
            this->Update(delta_time);
        });
        oss << (int)token_radius << "px foo";
        // Set up the game board
        InitTriangles(canvas_width / 15.0, canvas_height / 3.0);
        PlaceInitialTokens_Runoff();
        InitDice();
        // Get the player's initial dice roll!
        RollDice();
        // Draw the screen!
        Render();
        // Enable automatic canvas redrawing
        Start();
    }
};

AnimationController anim;
int main(){

}
