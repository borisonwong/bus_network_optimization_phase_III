#ifndef SOLUTION_CLASS_H
#define SOLUTION_CLASS_H

#include "route_class.h"
#include "node_class.h"

class Solution{
    public:
    vector<Route*> route_vector;
    vector<Node*> sol_all_node_vector;
    float Q_1,Q_2,Q_3;
    float self_AB_portion = AB_portion;
    Solution(){
        init_all_node_vector(sol_all_node_vector);
    }
    void init(){
        bool all_nodes_included = false;
        while(all_nodes_included == false){
            unuse_all_nodes();
            for(int r = 0; r < R_max; r++){
                Route* tmp_route = new Route(false,sol_all_node_vector);
                tmp_route -> init();
                route_vector.push_back(tmp_route);
            }
            all_nodes_included = is_valid();
            if(all_nodes_included == false){
                clear_and_del_route();
            }
        }
    }
    void print(){
        cout << "============================== SOLUTION ==============================" << endl;
        cout << "AB_portion = " << self_AB_portion << endl;
        for(int r = 0; r < route_vector.size(); r++){
            cout << "Route " << r + 1;
            if(route_vector[r] -> is_express){
                cout << " (EXPRESS)";
            }
            cout << " :" << endl;
            route_vector[r] -> print();
        }
        cout << "VIOLATE = " << count_f_violate() << endl;
        cout << "======================================================================" << endl;
    }
    string output(){
        string output_str;
        output_str += "\n";
        output_str += "AB_portion," + to_string(self_AB_portion) + "\n";
        for(int r = 0; r < route_vector.size(); r++){
            output_str += "Route "+ to_string(r + 1);
            if(route_vector[r] -> is_express){
                output_str += " (EXPRESS)";
            }
            output_str += ",";
            output_str += route_vector[r] -> output();
        }
        return output_str;
    }
    bool check_all_used(){
        for(Node* i : sol_all_node_vector){
            if(i -> is_used == false){
                //  One node unused
                bool walkable = false;
                for(Node* x : sol_all_node_vector){
                    if(x -> is_used == false){
                        continue;
                    }
                    if(get_c_ij(i -> index,x -> index,false) * beta < T_wt){
                        walkable = true;
                        break;
                    }
                }
                if(walkable == false){
                    return false;
                }
            }
        }
        return true;
    }
    void clear_and_del_route(){
        for(Route* r : route_vector){
            delete r;
        }
        route_vector.clear();
    }
    void unuse_all_nodes(){
        for(Node* i : sol_all_node_vector){
            i -> unuse();
        }
    }
    Traffic_Route* get_traffic_route(int i, int e, bool is_inertia){
        vector<int> possible_x;
        vector<int> possible_y;
        bool pointer_not_null = false;
        for(int x : Z){
            float tmp_walking_time = get_c_ij(i, x, false);
            if(tmp_walking_time < T_wt){
                possible_x.push_back(x);
            }
        }
        for(int y : Z){
            float tmp_walking_time = get_c_ij(y, e, false);
            if(tmp_walking_time < T_wt){
                possible_y.push_back(y);
            }
        }
        bool is_init = false;
        float min_cost = -1;
        Traffic_Route* best_route_pointer;
        for(int x : possible_x){
            for(int y : possible_y){
                vector<int> possible_route_1_index;
                vector<int> possible_route_2_index;
                for(int r = 0; r < route_vector.size(); r++){
                    if(route_vector[r] -> node_in_route(x)){
                        possible_route_1_index.push_back(r);
                    }
                }
                for(int r = 0; r < route_vector.size(); r++){
                    if(route_vector[r] -> node_in_route(y)){
                        possible_route_2_index.push_back(r);
                    }
                }
                for(int route_1_index : possible_route_1_index){
                    for(int route_2_index : possible_route_2_index){
                        if (route_1_index == route_2_index){
                            Traffic_Route* tmp_traffic_route = new Traffic_Route(i,e,x,y,-1,-1,route_vector[route_1_index],route_vector[route_1_index],is_inertia,route_1_index,route_2_index);
                            if(is_init){
                                if(tmp_traffic_route -> cost < min_cost){
                                    best_route_pointer = tmp_traffic_route;
                                    pointer_not_null = true;
                                    min_cost = tmp_traffic_route -> cost;
                                }else{
                                    delete tmp_traffic_route;
                                }
                            }else{
                                best_route_pointer = tmp_traffic_route;
                                pointer_not_null = true;
                                min_cost = tmp_traffic_route -> cost;
                            }
                        }else{
                            float min_mn_walk = 0;
                            bool min_mn_init = false;
                            bool possible = false;
                            int min_m, min_n;
                            vector<int> possible_m_list;
                            vector<int> possible_n_list;
                            bool x_appeared = false;
                            for(int m : route_vector[route_1_index] -> stop_node_int_vector){
                                if(m == 21){
                                    //  m == 21 means EHC exchange (hardcoded)
                                    possible_m_list.push_back(m);
                                    break;
                                }
                                if(x_appeared){
                                    possible_m_list.push_back(m);
                                }
                                if(m == x){
                                    x_appeared = true;
                                }
                            }
                            for(int n : route_vector[route_2_index] -> stop_node_int_vector){
                                if(n == 21){
                                    possible_n_list.push_back(n);
                                    break;
                                }else{
                                    possible_n_list.push_back(n);
                                }
                            }
                            for(int m : possible_m_list){
                                for(int n : possible_n_list){
                                    float tmp_walk_mn = get_c_ij(m,n,false) * beta;
                                    if(tmp_walk_mn < T_wt){
                                        if(min_mn_init){
                                            if(tmp_walk_mn < min_mn_walk){
                                                min_mn_walk = tmp_walk_mn;
                                                min_m = m;
                                                min_n = n;
                                            }
                                        }else{
                                            min_mn_walk = tmp_walk_mn;
                                            min_m = m;
                                            min_n = n;
                                            possible = true;
                                        }            
                                    }
                                }
                            }
                            if(possible){
                                Traffic_Route* tmp_traffic_route = new Traffic_Route(i,e,x,y,min_m,min_n,route_vector[route_1_index],route_vector[route_2_index],is_inertia,route_1_index,route_2_index);
                                if(is_init){
                                    if(tmp_traffic_route -> cost < min_cost){
                                        best_route_pointer = tmp_traffic_route;
                                        pointer_not_null = true;
                                        min_cost = tmp_traffic_route -> cost;
                                    }else{
                                        delete tmp_traffic_route;
                                    }
                                }else{
                                    best_route_pointer = tmp_traffic_route;
                                    pointer_not_null = true;
                                    min_cost = tmp_traffic_route -> cost;
                                }
                            }
                        }
                    }
                }
            }
        }
        return best_route_pointer;
    }
    vector<Traffic_Route*> get_traffic_route_vector(int i, int e, bool is_inertia){
        vector<int> possible_x;
        vector<int> possible_y;
        bool pointer_not_null = false;
        for(int x : Z){
            float tmp_walking_time = get_c_ij(i, x, false);
            if(tmp_walking_time < T_wt){
                possible_x.push_back(x);
            }
        }
        for(int y : Z){
            float tmp_walking_time = get_c_ij(y, e, false);
            if(tmp_walking_time < T_wt){
                possible_y.push_back(y);
            }
        }
        bool is_init = false;
        float min_cost = -1;
        vector<Traffic_Route*> traffic_route_vector;
        for(int x : possible_x){
            for(int y : possible_y){
                vector<int> possible_route_1_index;
                vector<int> possible_route_2_index;
                for(int r = 0; r < route_vector.size(); r++){
                    if(route_vector[r] -> node_in_route(x)){
                        possible_route_1_index.push_back(r);
                    }
                }
                for(int r = 0; r < route_vector.size(); r++){
                    if(route_vector[r] -> node_in_route(y)){
                        possible_route_2_index.push_back(r);
                    }
                }
                for(int route_1_index : possible_route_1_index){
                    for(int route_2_index : possible_route_2_index){
                        if (route_1_index == route_2_index){
                            Traffic_Route* tmp_traffic_route = new Traffic_Route(i,e,x,y,-1,-1,route_vector[route_1_index],route_vector[route_1_index],is_inertia,route_1_index,route_2_index);
                            pointer_not_null = true;
                            traffic_route_vector.push_back(tmp_traffic_route);
                        }else{
                            float min_mn_walk = 0;
                            bool min_mn_init = false;
                            bool possible = false;
                            int min_m, min_n;
                            vector<int> possible_m_list;
                            vector<int> possible_n_list;
                            bool x_appeared = false;
                            for(int m : route_vector[route_1_index] -> stop_node_int_vector){
                                if(m == 21){
                                    //  m == 21 means EHC exchange (hardcoded)
                                    possible_m_list.push_back(m);
                                    break;
                                }
                                if(x_appeared){
                                    possible_m_list.push_back(m);
                                }
                                if(m == x){
                                    x_appeared = true;
                                }
                            }
                            for(int n : route_vector[route_2_index] -> stop_node_int_vector){
                                if(n == 21){
                                    possible_n_list.push_back(n);
                                    break;
                                }else{
                                    possible_n_list.push_back(n);
                                }
                            }
                            for(int m : possible_m_list){
                                for(int n : possible_n_list){
                                    float tmp_walk_mn = get_c_ij(m,n,false) * beta;
                                    if(tmp_walk_mn < T_wt){
                                        if(min_mn_init){
                                            if(tmp_walk_mn < min_mn_walk){
                                                min_mn_walk = tmp_walk_mn;
                                                min_m = m;
                                                min_n = n;
                                            }
                                        }else{
                                            min_mn_walk = tmp_walk_mn;
                                            min_m = m;
                                            min_n = n;
                                            possible = true;
                                        }
                                            
                                    }
                                }
                            }
                            if(possible){
                                Traffic_Route* tmp_traffic_route = new Traffic_Route(i,e,x,y,min_m,min_n,route_vector[route_1_index],route_vector[route_2_index],is_inertia,route_1_index,route_2_index);
                                traffic_route_vector.push_back(tmp_traffic_route);
                                pointer_not_null = true;
                            }
                        }
                    }
                }
            }
        }
        return traffic_route_vector;
    }
    float get_user_cost(){
        float tot_cost = 0;
        for(Route* r : route_vector){
            r -> update_time_f(true);
        }
        for(int i : U){
            for(int e : V){
                vector<Traffic_Route*> IP_route = get_traffic_route_vector(i,e,true);
                vector<Traffic_Route*> NO_IP_route = get_traffic_route_vector(i,e,false);
                float IP_cost = get_expected_cost(IP_route);
                float NO_IP_cost = get_expected_cost(NO_IP_route);
                tot_cost += IP * IP_cost + (1 - IP) * NO_IP_cost;
                for(Traffic_Route* t : IP_route){
                    delete t;
                }
                for(Traffic_Route* t : NO_IP_route){
                    delete t;
                }
            }
        }
        return tot_cost;
    }
    float get_oper_cost(){
        float output_cost = 0;
        for(Route* r : route_vector){
            output_cost += r -> CB_allocated * (gamma_oper_CB + gamma_capital_CB);
            output_cost += r -> AB_allocated * (gamma_oper_AB + gamma_capital_AB);
        }
        return output_cost;
    }
    float get_infra_cost(){
        float output_cost = 0;
        for(Route* r : route_vector){
            if(r -> is_express){
                output_cost += stop_upgrade_cost * r -> stop_node_express_int_vector.size();
            }else{
                output_cost += stop_upgrade_cost * r -> stop_node_int_vector.size();
            }
            output_cost += r -> time_req_ED_AB * road_upgrade_cost;
        }
        return output_cost;
    }
    void allocate_CB_by_vector(vector<int> bus_allocation_vector_in,bool is_init){
        for(int r = 0; r < R_max; r++){
            route_vector[r] -> CB_allocated = bus_allocation_vector_in[r];
            if(is_init){
                route_vector[r] -> AB_allocated = 0;
            }
            route_vector[r] -> update_time_f(false);
        }
    }
    void allocate_AB_by_vector(vector<int> bus_allocation_vector_in){
        for(int r = 0; r < R_max; r++){
            route_vector[r] -> AB_allocated = bus_allocation_vector_in[r];
            route_vector[r] -> update_time_f(false);
        }
    }
    void opt_express(){
        float min_user_cost = get_user_cost();
        bool no_improvement = false;
        while(no_improvement == false){
            int change_index;
            no_improvement = true;
            for(int r = 0; r < R_max; r++){
                route_vector[r] -> invert_express();
                if(is_valid()){
                    float tmp_user_cost = get_user_cost();
                    if(tmp_user_cost < min_user_cost){
                        change_index = r;
                        min_user_cost = tmp_user_cost;
                        no_improvement = false;
                    }
                }
                route_vector[r] -> invert_express();
            }
            if(no_improvement == false){
                route_vector[change_index] -> invert_express();
            }
        }
    }
    void high_f_get_bus(){
        int original_W = W;
        for(int r = 0; r < R_max; r++){
            route_vector[r] -> f_CB = 100000;
            route_vector[r] -> f_AB = 0;
            route_vector[r] -> demand = 0;
        }
        int tot_demand = 0;
        for(int i : U){
            for(int e : V){
                vector<Traffic_Route*> route_vector_no_inertia = get_traffic_route_vector(i,e,false);
                vector<float> logit_prob = get_logit_prob(route_vector_no_inertia);
                for(int iter = 0; iter < route_vector_no_inertia.size(); iter++){
                    Traffic_Route* route_no_intertia = route_vector_no_inertia[iter];
                    float logit_prob_iter = logit_prob[iter];
                    if(is_int_vector_identical(route_no_intertia -> route_a_int_vector,route_no_intertia -> route_b_int_vector)){
                        route_vector[route_no_intertia -> r_1] -> demand += get_demand_ij(i,e) * logit_prob_iter;
                        tot_demand += logit_prob_iter * get_demand_ij(i,e);
                    }else{
                        route_vector[route_no_intertia -> r_1] -> demand += get_demand_ij(i,e) * logit_prob_iter;
                        route_vector[route_no_intertia -> r_2] -> demand += get_demand_ij(i,e) * logit_prob_iter;
                        tot_demand += 2 * get_demand_ij(i,e) * logit_prob_iter;
                    }
                }
                for(Traffic_Route* t : route_vector_no_inertia){
                    delete t;
                }
            }
        }
        vector<int> CB_vector;
        vector<int> AB_vector;
        for(int r = 0; r < route_vector.size(); r++){
            int bus_allocated = round((route_vector[r] -> demand / tot_demand) * W);
            int CB_allocated = round(bus_allocated * (1 - self_AB_portion));
            int AB_allocated = bus_allocated - CB_allocated;
            CB_vector.push_back(CB_allocated);
            AB_vector.push_back(AB_allocated);
        }
        fix_sum(CB_vector,AB_vector);
        int CB_int_count = 0;
        int AB_int_count = 0;
        for(int r = 0; r < R_max; r++){
            CB_int_count += CB_vector[r];
            AB_int_count += AB_vector[r];
        }
        repair_CB_vector(CB_vector);
        allocate_CB_by_vector(CB_vector,false);
        allocate_AB_by_vector(AB_vector);
        Q_1 = get_user_cost();
        Q_2 = get_oper_cost();
        Q_3 = get_infra_cost();
    }
    void low_f_get_bus(){
        int original_W = W;
        int tot_demand = 0;
        for(Route* r : route_vector){
            r -> demand = 0;
        }
        for(int i : U){
            for(int e : V){
                vector<Traffic_Route*> route_vector_no_inertia = get_traffic_route_vector(i,e,false);
                vector<float> logit_prob_no_inertia = get_logit_prob(route_vector_no_inertia);
                for(int iter = 0; iter < route_vector_no_inertia.size(); iter++){
                    Traffic_Route* route_no_intertia = route_vector_no_inertia[iter];
                    float logit_prob_iter = logit_prob_no_inertia[iter];
                    if(is_int_vector_identical(route_no_intertia -> route_a_int_vector,route_no_intertia -> route_b_int_vector)){
                        route_vector[route_no_intertia -> r_1] -> demand += logit_prob_iter * get_demand_ij(i,e) * (1 - IP);
                        tot_demand += logit_prob_iter * get_demand_ij(i,e) * (1 - IP);
                    }else{
                        route_vector[route_no_intertia -> r_1] -> demand += get_demand_ij(i,e) * logit_prob_iter * (1 - IP);
                        route_vector[route_no_intertia -> r_2] -> demand += get_demand_ij(i,e) * logit_prob_iter * (1 - IP);
                        tot_demand += 2 * get_demand_ij(i,e) * logit_prob_iter * (1 - IP);
                    }
                }
                for(Traffic_Route* t : route_vector_no_inertia){
                    delete t;
                }
                vector<Traffic_Route*> route_vector_inertia = get_traffic_route_vector(i,e,true);
                vector<float> logit_prob_inertia = get_logit_prob(route_vector_inertia);
                for(int iter = 0; iter < route_vector_inertia.size(); iter++){
                    Traffic_Route* route_inertia = route_vector_inertia[iter];
                    float logit_prob_iter = logit_prob_inertia[iter];
                    if(is_int_vector_identical(route_inertia -> route_a_int_vector,route_inertia -> route_b_int_vector)){
                        route_vector[route_inertia -> r_1] -> demand += logit_prob_iter * get_demand_ij(i,e) * IP;
                        tot_demand += logit_prob_iter * get_demand_ij(i,e) * IP;
                    }else{
                        route_vector[route_inertia -> r_1] -> demand += logit_prob_iter * get_demand_ij(i,e) * IP;
                        route_vector[route_inertia -> r_2] -> demand += logit_prob_iter * get_demand_ij(i,e) * IP;
                        tot_demand += 2 * get_demand_ij(i,e) * logit_prob_iter * IP;
                    }
                }
                for(Traffic_Route* t : route_vector_inertia){
                    delete t;
                }
            }
        }
        vector<int> CB_vector;
        vector<int> AB_vector;
        for(int r = 0; r < route_vector.size(); r++){
            int bus_allocated = round((route_vector[r] -> demand / tot_demand) * W);
            int CB_allocated = round(bus_allocated * (1 - self_AB_portion));
            int AB_allocated = bus_allocated - CB_allocated;
            CB_vector.push_back(CB_allocated);
            AB_vector.push_back(AB_allocated);
        }
        fix_sum(CB_vector,AB_vector);
        int CB_int_count = 0;
        int AB_int_count = 0;
        for(int r = 0; r < R_max; r++){
            CB_int_count += CB_vector[r];
            AB_int_count += AB_vector[r];
        }
        repair_CB_vector(CB_vector);
        allocate_CB_by_vector(CB_vector,false);
        allocate_AB_by_vector(AB_vector);
        Q_1 = get_user_cost();
        Q_2 = get_oper_cost();
        Q_3 = get_infra_cost();
    }
    void high_f_opt_express_CB_AB(){
        for(Route* r : route_vector){
            r -> generate_express();
        }
        high_f_get_bus();
        opt_express();
        low_f_get_bus();
    }
    bool is_valid(){
        update_used_node();
        bool all_route_ok = true;
        for(Route* r : route_vector){
            if(not(r -> is_route_valid(r -> get_using_int_vector(),r -> is_express))){
                all_route_ok = false;
            }
        }
        return check_all_used();
    }
    void use_node(int i){
        sol_all_node_vector[i] -> use();
    }
    void update_used_node(){
        unuse_all_nodes();
        for(Route* r : route_vector){
            if(r -> is_express){
                for(int stop_int : r -> stop_node_express_int_vector){
                    sol_all_node_vector[stop_int - 1] -> use();
                }
            }else{
                for(int stop_int : r -> stop_node_int_vector){
                    sol_all_node_vector[stop_int - 1] -> use();
                }
            }
        }
    }
    void delete_all_route(){
        for(Route* r : route_vector){
            delete r;
        }
        route_vector.clear();
    }
    void delete_all_node(){
        for(Node* n : sol_all_node_vector){
            delete n;
        }
        sol_all_node_vector.clear();
    }
    int count_f_violate(){
        int counter = 0;
        for(Route* r : route_vector){
            if((r -> is_express && r -> f_CB + r -> f_AB < f_min_express) || (not(r -> is_express) && r -> f_CB + r -> f_AB < f_min_ordinary)){
                counter += 1;
            }
        }
        return counter;
    }
    void remove_violate(){
        float max_delta_ratio = 0;
        int max_delta_index = -1;
        for(int r = 0; r < R_max; r++){
            Route* route_pointer = route_vector[r];
            if(route_pointer -> is_express){
                if(route_pointer -> f_CB + route_pointer -> f_AB < f_min_express){
                    float delta = (f_min_express - route_pointer -> f_CB - route_pointer -> f_AB) / f_min_express;
                    if(delta > max_delta_ratio){
                        max_delta_index = r;
                        max_delta_ratio = delta;
                    }
                }
            }else{
                if(route_pointer -> f_CB + route_pointer -> f_AB < f_min_ordinary){
                    float delta = (f_min_ordinary - route_pointer -> f_CB - route_pointer -> f_AB) / f_min_ordinary;
                    if(delta > max_delta_ratio){
                        max_delta_index = r;
                        max_delta_ratio = delta;
                    }
                }
            }
        }
        if(max_delta_ratio > 0){
            //  Delete the worst one
            Route* route_worst_pointer = route_vector[max_delta_index];
            Route* new_route = new Route(false,sol_all_node_vector);
            new_route -> init();
            route_vector[max_delta_index] = new_route;
            route_vector[max_delta_index] -> update_node_vector();
            if(is_valid()){
                delete route_worst_pointer;
            }else{
                route_vector[max_delta_index] = route_worst_pointer;
                route_vector[max_delta_index] -> update_node_vector();
                delete new_route;
            }
        }
        high_f_opt_express_CB_AB();
    }
    void solution_final_fix(){
        //  Repair final solutions with frequency violations
        for(Route* r : route_vector){
            while((r -> is_express && r -> f_CB + r -> f_AB < f_min_express) || (not(r -> is_express) && r -> f_CB + r -> f_AB < f_min_ordinary)){
                r -> AB_allocated += 1;
                r -> update_time_f(false);
            }
        }
        int tot_bus_count = 0;
        for(Route* r : route_vector){
            tot_bus_count += r -> CB_allocated + r -> AB_allocated;
        }
        int bus_difference = tot_bus_count - W;
        while(bus_difference > 0){
            int max_freq_diff_index = 0;
            float max_freq_diff;
            //  Init max freq diff
            if(route_vector[0] -> is_express){
                max_freq_diff = (route_vector[0] -> f_AB + route_vector[0] -> f_CB) -  f_min_express;
            }else{
                max_freq_diff = (route_vector[0] -> f_AB + route_vector[0] -> f_CB) -  f_min_ordinary;
            }
            for(int r = 0; r < R_max; r++){
                float current_freq_diff;
                if(route_vector[r] -> is_express){
                    current_freq_diff = (route_vector[r] -> f_AB + route_vector[r] -> f_CB) -  f_min_express;
                }else{
                    current_freq_diff = (route_vector[r] -> f_AB + route_vector[r] -> f_CB) -  f_min_ordinary;
                }
                if(current_freq_diff > max_freq_diff && route_vector[r] -> AB_allocated > 1){
                    max_freq_diff = current_freq_diff;
                    max_freq_diff_index = r;
                }
            }
            route_vector[max_freq_diff_index] -> AB_allocated -= 1;
            route_vector[max_freq_diff_index] -> update_time_f(false);
            bus_difference -= 1;
        }
    }
};

#endif