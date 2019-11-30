// This is a lightly modified version of pubeval.c, a community
//  standard for testing digital backgammon agents. 
//  This is NOT my code, I have adapted it from here:
//  https://www.bkgm.com/rgb/rgb.cgi?view+610
// Original author: Gerry Tesauro

#ifndef BACKGAMMON_AGENT_PUBEVAL_H
#define BACKGAMMON_AGENT_PUBEVAL_H

//Local 
#include "./agent_base.h"
//Empirical
#include "tools/Random.h"
#include "base/array.h"
//Standard
#include <string>
#include <iostream>
#include <fstream>

class BackgammonAgent_PubEval : public BackgammonAgent_Base{
private: 
    emp::array<float, 122> wr = {0.00000,-0.17160, 0.27010, 0.29906,-0.08471, 0.00000,-1.40375,
        -1.05121, 0.07217,-0.01351, 0.00000,-1.29506,-2.16183, 0.13246,-1.03508, 0.00000,-2.29847,
        -2.34631, 0.17253, 0.08302, 0.00000,-1.27266,-2.87401,-0.07456,-0.34240, 0.00000,-1.34640,
        -2.46556,-0.13022,-0.01591, 0.00000, 0.27448, 0.60015, 0.48302, 0.25236, 0.00000, 0.39521, 
        0.68178, 0.05281, 0.09266, 0.00000, 0.24855,-0.06844,-0.37646, 0.05685, 0.00000, 0.17405, 
        0.00430, 0.74427, 0.00576, 0.00000, 0.12392, 0.31202,-0.91035,-0.16270, 0.00000, 0.01418,
        -0.10839,-0.02781,-0.88035, 0.00000, 1.07274, 2.00366, 1.16242, 0.22520, 0.00000, 0.85631, 
        1.06349, 1.49549, 0.18966, 0.00000, 0.37183,-0.50352,-0.14818, 0.12039, 0.00000, 0.13681, 
        0.13978, 1.11245,-0.12707, 0.00000,-0.22082, 0.20178,-0.06285,-0.52728, 0.00000,-0.13597,
        -0.19412,-0.09308,-1.26062, 0.00000, 3.05454, 5.16874, 1.50680, 5.35000, 0.00000, 2.19605, 
        3.85390, 0.88296, 2.30052, 0.00000, 0.92321, 1.08744,-0.11696,-0.78560, 0.00000,-0.09795,
        -0.83050,-1.09167,-4.94251, 0.00000,-1.00316,-3.66465,-2.56906,-9.67677, 0.00000,-2.77982,
        -7.26713,-3.40177,-12.32252, 0.00000,3.42040
    };
    emp::array<float, 122> wc = { 0.25696, -0.66937,-1.66135, -2.02487, -2.53398, -0.16092, -1.11725,
        -1.06654, -0.92830, -1.99558, -1.10388, -0.80802, 0.09856, -0.62086, -1.27999, -0.59220,
        -0.73667, 0.89032, -0.38933, -1.59847, -1.50197, -0.60966, 1.56166, -0.47389, -1.80390,
        -0.83425, -0.97741, -1.41371, 0.24500, 0.10970, -1.36476, -1.05572, 1.15420, 0.11069,
        -0.38319, -0.74816, -0.59244, 0.81116, -0.39511, 0.11424, -0.73169, -0.56074, 1.09792, 
        0.15977, 0.13786, -1.18435, -0.43363, 1.06169, -0.21329, 0.04798, -0.94373, -0.22982, 
        1.22737, -0.13099, -0.06295, -0.75882, -0.13658, 1.78389, 0.30416, 0.36797, -0.69851, 
        0.13003, 1.23070, 0.40868, -0.21081, -0.64073, 0.31061, 1.59554, 0.65718, 0.25429, 
        -0.80789, 0.08240, 1.78964, 0.54304, 0.41174, -1.06161, 0.07851, 2.01451, 0.49786, 
        0.91936, -0.90750, 0.05941, 1.83120, 0.58722, 1.28777, -0.83711, -0.33248, 2.64983, 
        0.52698, 0.82132, -0.58897, -1.18223, 3.35809, 0.62017, 0.57353, -0.07276, -0.36214, 
        4.37655, 0.45481, 0.21746, 0.10504, -0.61977, 3.54001, 0.04612, -0.18108, 0.63211, 
        -0.87046, 2.47673, -0.48016, -1.27157, 0.86505, -1.11342, 1.24612, -0.82385, -2.77082, 
        1.23606, -1.59529, 0.10438, -1.30206, -4.11520, 5.62596, -2.75800
    };
    emp::array<float, 122> x;
    emp::array<int, 28> pos;
    virtual void PromptTurn(
            const BackgammonState& state, 
            std::function<void(BackgammonMove)> callback) override{
        callback(state.possible_moves[ScoreMoves(state)]);
    }
    size_t ScoreMoves(const BackgammonState& state){
        double cur_score = 0;
        double max_score = 0;
        int max_idx = -1; 
        for(size_t move_idx = 0; move_idx < state.possible_moves.size(); ++move_idx){
            BackgammonState new_state = test_move_func(state, state.possible_moves[move_idx]); 
            FillPos(new_state);
            cur_score = Evaluate((state.in_bearoff_agent_1 && state.in_bearoff_agent_2) ? 1 : 0);
            if(cur_score > max_score || max_idx < 0){
                max_score = cur_score;
                max_idx = move_idx;
            }
        }
        return max_idx;
    }
    virtual void Notify(
            const emp::vector<BackgammonMove>& moves, const emp::vector<int>& dice_vals) override{
    }
    void FillPos(const BackgammonState& state){
       /*
       pos[]  is an integer array of dimension 28 which
       should represent a legal final board state after
       the move. Elements 1-24 correspond to board locations
       1-24 from computer's point of view, i.e. computer's
       men move in the negative direction from 24 to 1, and
       opponent's men move in the positive direction from
       1 to 24. Computer's men are represented by positive
       integers, and opponent's men are represented by negative
       integers. Element 25 represents computer's men on the
       bar (positive integer), and element 0 represents opponent's
       men on the bar (negative integer). Element 26 represents
       computer's men off the board (positive integer), and
       element 27 represents opponent's men off the board
       (negative integer).                                  
        */
        // First, clear out pos, just in case
        for(size_t i = 0; i < 28; ++i){
            pos[i] = 0;
        }
        if(agent_id == 1){
            for(size_t idx = 1; idx < 25; ++idx){
               pos[25 - idx] = state.tokens_per_point[idx]; 
            }
            pos[25] = state.tokens_off_agent_1;
            pos[0] = state.tokens_off_agent_2 * -1;
            pos[26] = state.tokens_finished_agent_1;
            pos[27] = state.tokens_finished_agent_2 * -1;
        }
        else{
            for(size_t idx = 1; idx < 25; ++idx){
               pos[idx] = state.tokens_per_point[idx] * -1; 
            }
            pos[25] = state.tokens_off_agent_2;
            pos[0] = state.tokens_off_agent_1 * -1;
            pos[26] = state.tokens_finished_agent_2;
            pos[27] = state.tokens_finished_agent_1 * -1;
        }
    }
    /*
    void ReadWeights(){ //Originally rdwts
            // read weight files into arrays wr[], wc[] 
            int i = 0;
            std::ifstream fp("./src/WT.race");
            if(!fp.is_open()){
                std::cerr << "ERROR: Unable to open WT.race for pubeval agent!" << std::endl;
                exit(-1);  
            }
            for(std::string line; std::getline(fp, line); )   //read stream line by line
            {
                std::istringstream in(line);      //make a stream for the line itself
                in >> wr[i];
                ++i;
            }
            std::ifstream fq("WT.cntc");
            if(!fq.is_open()){
                std::cerr << "ERROR: Unable to open WT.cntc for pubeval agent!" << std::endl;
                exit(-1);  
            }
            for(std::string line; std::getline(fq, line); )   //read stream line by line
            {
                std::istringstream in(line);      //make a stream for the line itself
                in >> wc[i];
                ++i;
            }
            std::cout << "Race:" << std::endl;
            for(size_t i = 0; i < 10; i++){
                std::cout << "\t"  << wr[i] << std::endl;
            }
            std::cout  << "\t" << "..." << std::endl;
            std::cout << "\t"  << wr[121] << std::endl;
            std::cout << "cntc:" << std::endl;
            for(size_t i = 0; i < 10; i++){
                std::cout << "\t"  << wc[i] << std::endl;
            }
            std::cout  << "\t" << "..." << std::endl;
            std::cout << "\t"  << wc[121] << std::endl;
            //FILE *fp, *fq;
            //fp = fopen("WT.race", "r");
            //fq = fopen("WT.cntc", "r");
            //for(i=0;i<122;++i) {
            //        fscanf(fp,"%f",wr+i);
            //        fscanf(fq,"%f",wc+i);
            //}
            //fclose(fp);
            //fclose(fq);
    }
    */
    void SetX(){
            /* sets input vector x[] given board position pos[] */
            int j, jm1, n;
            /* initialize */
            for(j=0;j<122;++j) x[j] = 0.0;

            /* first encode board locations 24-1 */
            for(j=1;j<=24;++j) {
                jm1 = j - 1;
                n = pos[25-j];
                if(n!=0) {
                    if(n==-1) x[5*jm1+0] = 1.0;
                    if(n==1) x[5*jm1+1] = 1.0;
                    if(n>=2) x[5*jm1+2] = 1.0;
                    if(n==3) x[5*jm1+3] = 1.0;
                    if(n>=4) x[5*jm1+4] = (float)(n-3)/2.0;
                }
            }
            /* encode opponent barmen */
            x[120] = -(float)(pos[0])/2.0;
            /* encode computer's menoff */
            x[121] = (float)(pos[26])/15.0;
    }
    float Evaluate(int race){        //Originally pubeval
            /* Backgammon move-selection evaluation function
               for benchmark comparisons.  Computes a linear
               evaluation function:  Score = W * X, where X is
               an input vector encoding the board state (using
               a raw encoding of the number of men at each location),
               and W is a weight vector.  Separate weight vectors
               are used for racing positions and contact positions.
               Makes lots of obvious mistakes, but provides a
               decent level of play for benchmarking purposes. */

            /* Provided as a public service to the backgammon
               programming community by Gerry Tesauro, IBM Research.
               (e-mail: tesauro@watson.ibm.com)                     */

            /* The following inputs are needed for this routine:

               race   is an integer variable which should be set
               based on the INITIAL position BEFORE the move.
               Set race=1 if the position is a race (i.e. no contact)
               and 0 if the position is a contact position.

               pos[]  is an integer array of dimension 28 which
               should represent a legal final board state after
               the move. Elements 1-24 correspond to board locations
               1-24 from computer's point of view, i.e. computer's
               men move in the negative direction from 24 to 1, and
               opponent's men move in the positive direction from
               1 to 24. Computer's men are represented by positive
               integers, and opponent's men are represented by negative
               integers. Element 25 represents computer's men on the
               bar (positive integer), and element 0 represents opponent's
               men on the bar (negative integer). Element 26 represents
               computer's men off the board (positive integer), and
               element 27 represents opponent's men off the board
               (negative integer).                                  */

            /* Also, be sure to call rdwts() at the start of your
               program to read in the weight values. Happy hacking] */

            int i;
            float score;

            if(pos[26]==15) return(99999999.);
            /* all men off, best possible move */
            SetX(); /* sets input array x[] */
            score = 0.0;
            if(race) {  /* use race weights */
                for(i=0;i<122;++i){ 
                    score += wr[i]*x[i];
                }
            }
            else {  /* use contact weights */
                for(i=0;i<122;++i) {
                    score += wc[i]*x[i];
                }   
            }
            return(score);
    }
public :
    BackgammonAgent_PubEval(){
    }
};


#endif

