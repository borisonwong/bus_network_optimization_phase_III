#include<iostream>
#include<fstream>
#include<cstdlib>
#include<string>
#include<iomanip>
#include<cmath>
#include <chrono>
#include <vector>

using namespace std;
using namespace std::chrono;

#include "network_topology.h"
#include "toolbox.h"
#include "node_class.h"
#include "route_class.h"
#include "traffic_route.h"
#include "toolbox_advanced.h"
#include "solution_class.h"
#include <math.h>       /* pow */
#include "GA_operation.h"

vector<float> get_solution_fitness(vector<Solution*> solution_vector_in, int objective){
    vector<float> return_float_vec;
    for(Solution* solution_iter : solution_vector_in){
        if(objective == 0){
            return_float_vec.push_back(1 / solution_iter -> Q_1);
        }
        if(objective == 1){
            return_float_vec.push_back(1 / solution_iter -> Q_2);
        }
        if(objective == 2){
            return_float_vec.push_back(1 / solution_iter -> Q_3);
        }        
    }
    return return_float_vec;
}
bool is_sol_equal(Solution* sol_1,Solution* sol_2){
    for(int r = 0; r < R_max; r++){
        if(sol_1 -> route_vector[r] -> is_express != sol_2 -> route_vector[r] -> is_express){
            return false;
        }
        if(sol_1 -> route_vector[r] -> is_express){
            if(sol_1 -> route_vector[r] -> stop_node_express_vector != sol_1 -> route_vector[r] -> stop_node_express_vector){
                return false;
            }
        }else{
            if(sol_1 -> route_vector[r] -> stop_node_int_vector != sol_2 -> route_vector[r] -> stop_node_int_vector){
                return false;
            }
        }
    }
    return true;
}


int main(){
    srand(15);
    ofstream totFileQ1,totFileQ2,totFileQ3,totFileViolate,totFileTime;
    totFileQ1.open("totFileQ1.csv");
    totFileQ2.open("totFileQ2.csv");
    totFileQ3.open("totFileQ3.csv");
    totFileViolate.open("totFileViolate.csv");
    totFileTime.open("totFileTime.csv");
    for(int run_count = 1; run_count <= 3; run_count++){
        cout << "######################## RUN COUNT " << run_count << " ########################" << endl;
        auto start_all = high_resolution_clock::now();
        ofstream runFile;
        runFile.open("output_" + to_string(run_count) + ".csv");
        runFile << "gamma_wt," << gamma_wt << "\n";
        runFile << "gamma_wa," << gamma_wa << "\n";
        runFile << "gamma_veh," << gamma_veh << "\n";
        runFile << "gamma_oper_AB," << gamma_oper_AB << "\n";

        runFile << "gamma_oper_AB," << gamma_oper_AB << "\n";
        runFile << "gamma_oper_CB," << gamma_oper_CB << "\n";
        runFile << "gamma_capital_AB," << gamma_capital_AB << "\n";
        runFile << "gamma_capital_CB," << gamma_capital_CB << "\n";
        runFile << "AB_portion," << AB_portion << "\n";

        vector<Solution*> sol_vector_main;
        vector<float> Q_1_history_vec,Q_2_history_vec,Q_3_history_vec;
        vector<vector<float>> global_Q_1_history,global_Q_2_history,global_Q_3_history;
        vector<vector<int>> global_violate_history;
        
        auto start = high_resolution_clock::now();
        for(int sol_count = 0; sol_count < population_size; sol_count++){
            Solution* tmp_solution = new Solution();
            tmp_solution -> init();
            tmp_solution -> high_f_opt_express_CB_AB();
            sol_vector_main.push_back(tmp_solution);
        }
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "INIT Time elapsed = " << setprecision(3) << duration.count() /  static_cast<float> (1000000) << " s" << setprecision(6);
        for(Solution* sol : sol_vector_main){
            sol -> Q_1 = sol -> get_user_cost();
            sol -> Q_2 = sol -> get_oper_cost();
            sol -> Q_3 = sol -> get_infra_cost();
        }
        sol_vector_main = get_sorted_solution(sol_vector_main);
        //  Begin of crossover and mutation operations
        for(int iteration_count = 0; iteration_count < 4; iteration_count++){
            auto start = high_resolution_clock::now();
            cout << "=========== ITERATION " << iteration_count << " ===========" << endl;
            vector<Solution*> sol_vector_main_copy = sol_vector_main;
            for(int operation_count = 0; operation_count < offspring_size / 2; operation_count++){
                int decision_choice = rand() % 2;   //  Choose between route and stop crossover operations, 0 for route crossover, 1 for stop crossover
                vector<float> fitness_vector = get_index_fitness(population_size);
                vector<int> parent_index = rand_from_float_vector(fitness_vector,2);
                if(decision_choice == 0){
                    vector<Solution*> route_crossover_output = route_crossover(*sol_vector_main_copy[parent_index[0]],*sol_vector_main_copy[parent_index[1]]);
                    sol_vector_main.push_back(route_crossover_output[0]);
                    sol_vector_main.push_back(route_crossover_output[1]);
                }else{
                    vector<Solution*> stop_crossover_output = stop_crossover(sol_vector_main_copy[parent_index[0]],sol_vector_main_copy[parent_index[1]]);
                    sol_vector_main.push_back(stop_crossover_output[0]);
                    sol_vector_main.push_back(stop_crossover_output[1]);
                }
            }
            vector<Solution*> sol_vector_new;
            for(Solution* s_old : sol_vector_main){
                bool is_equal = false;
                for(Solution* s_new : sol_vector_new){
                    if(is_sol_equal(s_old,s_new)){
                        //  Remove duplicate solutions
                        is_equal = true;
                        delete_solution(s_old);
                        break;
                    }
                }
                if(is_equal == false){
                    sol_vector_new.push_back(s_old);
                }
            }
            sol_vector_main = sol_vector_new;
            sol_vector_main = get_sorted_solution(sol_vector_main);
            int delete_count_max = sol_vector_main.size() - population_size;
            for(int i = 0; i < delete_count_max; i++){
                Solution* del_sol_pointer = sol_vector_main[sol_vector_main.size() - 1];
                sol_vector_main.erase(sol_vector_main.begin() + sol_vector_main.size() - 1);
                delete_solution(del_sol_pointer);
            }
            int deficient_sol = population_size - sol_vector_main.size(); 
            for(int i = 0; i < deficient_sol; i++){
                //  Handle cases which the number of solutions is less than the population size
                Solution* new_solution = new Solution();
                new_solution -> init();
                new_solution -> high_f_opt_express_CB_AB();
                new_solution -> remove_violate();
                sol_vector_main.push_back(new_solution);
            }
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            cout << "Time elapsed = " << setprecision(3) << duration.count() /  static_cast<float> (1000000) << " s" << endl << setprecision(6);
            //  Data logging
            Q_1_history_vec.push_back(sol_vector_main[0] -> Q_1);
            Q_2_history_vec.push_back(sol_vector_main[0] -> Q_2);
            Q_3_history_vec.push_back(sol_vector_main[0] -> Q_3);
            vector<float> local_Q_1_history,local_Q_2_history,local_Q_3_history;
            vector<int> local_violate_history;
            for(Solution* sol : sol_vector_main){
                local_Q_1_history.push_back(sol -> Q_1);
                local_Q_2_history.push_back(sol -> Q_2);
                local_Q_3_history.push_back(sol -> Q_3);
                local_violate_history.push_back(sol -> count_f_violate());
            }
            cout << "Violate count = " << local_violate_history[0] << endl;
            global_Q_1_history.push_back(local_Q_1_history);
            global_Q_2_history.push_back(local_Q_2_history);
            global_Q_3_history.push_back(local_Q_3_history);
            global_violate_history.push_back(local_violate_history);
        }
        for(vector<float> local_Q1 : global_Q_1_history){
            totFileQ1 << local_Q1[0] << ",";
        }
        for(vector<float> local_Q2 : global_Q_2_history){
            totFileQ2 << local_Q2[0] << ",";
        }
        for(vector<float> local_Q3 : global_Q_3_history){
            totFileQ3 << local_Q3[0] << ",";
        }
        for(vector<int> local_violate : global_violate_history){
            totFileViolate << local_violate[0] << ",";
        }
        //  Data logging of the best solution
        sol_vector_main[0] -> solution_final_fix();
        sol_vector_main[0] -> Q_1 = sol_vector_main[0] -> get_user_cost();
        sol_vector_main[0] -> Q_2 = sol_vector_main[0] -> get_oper_cost();
        sol_vector_main[0] -> Q_3 = sol_vector_main[0] -> get_infra_cost();
        runFile << "Q1," << to_string(sol_vector_main[0] -> Q_1) << "\n";
        runFile << "Q2," << to_string(sol_vector_main[0] -> Q_2) << "\n";
        runFile << "Q3," << to_string(sol_vector_main[0] -> Q_3) << "\n";
        totFileQ1 << "Final," << to_string(sol_vector_main[0] -> Q_1);
        totFileQ2 << "Final," << to_string(sol_vector_main[0] -> Q_2);
        totFileQ3 << "Final," << to_string(sol_vector_main[0] -> Q_3);
        runFile << sol_vector_main[0] -> output();
        for(int i = 0; i < population_size; i++){
            //  Delete all solutions in the vector
            Solution* del_sol_pointer = sol_vector_main[sol_vector_main.size() - 1];
            sol_vector_main.erase(sol_vector_main.begin() + sol_vector_main.size() - 1);
            delete_solution(del_sol_pointer);
        }
        auto stop_all = high_resolution_clock::now();
        auto duration_all = duration_cast<microseconds>(stop_all - start_all);
        totFileTime << "Time elapsed," << duration_all.count() /  static_cast<float> (1000000) << "\n";
        runFile.close();
        totFileQ1 << "\n";
        totFileQ2 << "\n";
        totFileQ3 << "\n";
        totFileViolate << "\n";
        totFileTime << "\n";
    }
    totFileQ1.close();
    totFileQ2.close();
    totFileQ3.close();
    totFileViolate.close();
    totFileTime.close();
}