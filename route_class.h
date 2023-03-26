#ifndef ROUTE_CLASS_H
#define ROUTE_CLASS_H

#include "node_class.h"

class Route{
    public:
    bool is_express;
    vector<Node*> stop_node_vector;
    vector<int> stop_node_int_vector;
    vector<Node*> stop_node_express_vector;
    vector<int> stop_node_express_int_vector;
    int start_node_int;
    int end_node_int;
    Node* start_node;
    Node* end_node;
    int CB_allocated;
    int AB_allocated;
    float time_req_ED_CB;
    float time_req_ED_AB;
    float time_tot_CB;
    float time_tot_AB;
    float f_CB;
    float f_AB;
    float demand;
    vector<Node*> all_node_vector;
    vector<Node*> all_node_ordinary_vector;
    vector<Node*> all_node_express_vector;
    Route(int is_express_input, vector<Node*> all_node_vector_input){
        is_express = is_express_input;
        all_node_vector = all_node_vector_input;
    }
    void update_node_vector(){
        stop_node_vector.clear();
        stop_node_vector.shrink_to_fit();
        for(int i : stop_node_int_vector){
            stop_node_vector.push_back(all_node_vector[i - 1]);
            all_node_vector[i - 1] -> use();
        }
        start_node = stop_node_vector[0];
        end_node = stop_node_vector[stop_node_vector.size() - 1];
    }
    void update_express_node_vector(){
        stop_node_express_vector.clear();
        stop_node_express_vector.shrink_to_fit();
        for(int i : stop_node_express_int_vector){
            stop_node_express_vector.push_back(all_node_vector[i - 1]);
        }
    }
    bool node_in_route(int check_node){
        if(is_express){
            for (int i : stop_node_express_int_vector){
                if (i == check_node){
                    return true;
                }
            }
        }else{
            for(int i : stop_node_int_vector){
                if(i == check_node){
                    return true;
                }
            }
        }
        return false;
    }
    int get_node_index(int check_node){
        if(is_express){
            for(int i = 0; i < stop_node_express_int_vector.size(); i++){
                if(stop_node_express_int_vector[i] == check_node){
                    return i;
                }
            }
        }else{
            for(int i = 0; i < stop_node_int_vector.size(); i++){
                if(stop_node_int_vector[i] == check_node){
                    return i;
                }
            }
        }
        return -1;
    }
    void init(){
        is_express = false;
        stop_node_int_vector.push_back(Y[rand_between(0,Y.size() - 1)]);
        stop_node_int_vector.push_back(EHC_node);
        stop_node_int_vector.push_back(V[rand_between(0,V.size() - 1)]);
        vector<int> usable_node_int_vector = U;
        value_del_all_int_element(usable_node_int_vector,stop_node_int_vector[0]);
        bool time_exceed = false;
        while (time_exceed == false && stop_node_int_vector.size() < S_max){
            int chosen_node = rand_from_int_vector(usable_node_int_vector,1)[0];
            value_del_all_int_element(usable_node_int_vector,chosen_node);
            float min_time = -1;
            int min_pos = -1;
            for(int i = 1; i < stop_node_int_vector.size() - 1; i++){
                vector_int_insert(stop_node_int_vector,i,chosen_node);
                float tmp_time = get_T_req_ED(stop_node_int_vector,is_express);
                if(min_time == -1){
                    min_time = tmp_time;
                    min_pos = i;
                }else{
                    if(tmp_time < min_time){
                        min_time = tmp_time;
                        min_pos = i;
                    }
                }
                index_del_int_element(stop_node_int_vector,i);
            }
            if((is_express && min_time < T_max_express)||(is_express == false && min_time < T_max_ordinary)){
                vector_int_insert(stop_node_int_vector,min_pos,chosen_node);
            }else{
                time_exceed = true;
            }
        }
        time_req_ED_CB = get_T_req_ED(stop_node_int_vector,is_express);
        time_req_ED_AB = time_req_ED_CB * alpha;
        update_node_vector();
    }
    void update_time_f(bool penalty_on){
        if(is_express){
            time_tot_CB = get_T_req(stop_node_express_int_vector,is_express);
            time_req_ED_CB = get_T_req_ED(stop_node_express_int_vector,is_express);
        }else{
            time_tot_CB = get_T_req(stop_node_int_vector,is_express);
            time_req_ED_CB = get_T_req_ED(stop_node_int_vector,is_express);
        }
        time_tot_AB = time_tot_CB * alpha;
        time_req_ED_AB = time_req_ED_CB * alpha;
        f_CB = 30 * CB_allocated / time_tot_CB;
        f_AB = 30 * AB_allocated / time_tot_AB;
    }
    float get_time_ij(int i, int j){
        int i_pos = get_node_index(i);
        int j_pos = get_node_index(j);
        float tmp_time = 0;
        for(int i_iter = i_pos; i_iter < j_pos; i_iter++){
            if(is_express){
                tmp_time += get_c_ij(stop_node_express_int_vector[i_iter],stop_node_express_int_vector[i_iter + 1],is_express) + s;
            }else{
                tmp_time += get_c_ij(stop_node_int_vector[i_iter],stop_node_int_vector[i_iter + 1],is_express) + s;
            }
        }
        return tmp_time - s;
    }
    void print(){
        cout << "Stop sequence: ";
        if(is_express){
            print_int_vector(stop_node_express_int_vector);
        }else{
            print_int_vector(stop_node_int_vector);
        }
        cout << "CB = " << setw(5) << CB_allocated << "   f_CB = " << setw(5) << setprecision(2) << f_CB << endl;
        cout << "AB = " << setw(5) << AB_allocated << "   f_AB = " << setw(5) << setprecision(2) << f_AB << endl << setprecision(6);
        cout << "Demand = " << demand << endl;
        cout << time_req_ED_CB << endl;
    }
    string output(){
        string output_str;
        output_str += "CB," + to_string(CB_allocated) + ",f_CB," + to_string(f_CB) + ",";
        output_str += "AB," + to_string(AB_allocated) + ",f_AB," + to_string(f_AB) + ",";
        output_str += "Demand," + to_string(demand) + ",";
        output_str += "Time_req_ED_CB," + to_string(time_req_ED_CB) + ",";
        output_str += "time_tot_CB," + to_string(time_tot_CB) + ",";
        output_str += "Stop node,";
        if(is_express){
            for(int node_int : stop_node_int_vector){
                output_str += to_string(node_int) + ",";
            }
        }else{
            for(int node_int : stop_node_express_int_vector){
                output_str += to_string(node_int) + ",";
            }
        }
        output_str += "\n";
        return output_str;
    }
    void generate_express(){
        if(stop_node_int_vector.size() <= 3){
            stop_node_express_int_vector = stop_node_int_vector;
            stop_node_express_vector = stop_node_vector;
            return;
        }
        vector<int> tmp_express = stop_node_int_vector;
        vector<int> tmp_express_copy = tmp_express;
        while(true){
            float min_time_obtained = get_T_req_ED(tmp_express,true);
            if(min_time_obtained < T_max_express || tmp_express.size() <= 3){
                stop_node_express_int_vector = tmp_express;
                update_express_node_vector();
                break;
            }
            int tmp_delete_pos;
            float tmp_min_time;
            for(int delete_pos = 1; delete_pos < tmp_express.size() - 2; delete_pos++){
                vector<int> tmp_deleted_vector = tmp_express;
                index_del_int_element(tmp_deleted_vector,delete_pos);
                float current_time = get_T_req_ED(tmp_deleted_vector,true);
                if(delete_pos == 1){                    
                    tmp_min_time = current_time;
                    tmp_delete_pos = delete_pos;
                }else{
                    if(current_time < tmp_min_time){
                        tmp_min_time = current_time;
                        tmp_delete_pos = delete_pos;
                    }
                }
            }
            index_del_int_element(tmp_express,tmp_delete_pos);
        }
        stop_node_express_int_vector = tmp_express;
        update_express_node_vector();
    }
    void invert_express(){
        is_express = not(is_express);
        time_req_ED_CB = get_T_req_ED(get_using_int_vector(),is_express);
        time_req_ED_AB = time_req_ED_CB * alpha;
    }
    vector<int> get_using_int_vector(){
        if(is_express){
            return stop_node_express_int_vector;
        }else{
            return stop_node_int_vector;
        }
    }
    bool is_route_valid(vector<int> using_vec,bool is_express_in){
        float time_req_ED_CB_in = get_T_req_ED(using_vec,is_express_in);
        bool correct_start = false;
        for(int node : Y){
            if(using_vec[0] == node){
                correct_start = true;
                break;
            }
        }
        if(correct_start == false){
            return false;
        }
        if(using_vec[using_vec.size() - 2] != 21){
            return false;
        }
        bool correct_end = false;
        for(int node : V){
            if(using_vec[using_vec.size() - 1] == node){
                correct_end = true;
            }
        }
        if(correct_end == false){
            return false;
        }
        return true;
    }
};

#endif