#ifndef GA_OPERATION_H
#define GA_OPERATION_H


void delete_solution(Solution* sol_pointer){
    sol_pointer -> delete_all_route();
    sol_pointer -> delete_all_node();
    delete sol_pointer;
}

class Sort_unit{
    public:
    Solution* sol_pointer;
    int n;
    vector<Sort_unit*> s;
    int rank;
    float Q_1;
    float Q_2;
    float Q_3;
    float crowding_distance;
    Sort_unit(Solution* sol_pointer_in){
        sol_pointer = sol_pointer_in;
        n = 0;
        Q_1 = sol_pointer -> Q_1;
        Q_2 = sol_pointer -> Q_2;
        Q_3 = sol_pointer -> Q_3;
    }
    float get_Q_by_m(int m){
        if(m == 1){
            return Q_1;
        }else if(m == 2){
            return Q_2;
        }else if(m == 3){
            return Q_3;
        }else{
            return Q_1;
        }
    }
};

int A_dom_B(Solution* sol_1, Solution* sol_2){
    //  1 for A dominates B
    //  0 for infominate
    //  -1 for B dominates A
    bool Q_1_leq = sol_1 -> Q_1 <= sol_2 -> Q_1;
    bool Q_2_leq = sol_1 -> Q_2 <= sol_2 -> Q_2;
    bool Q_3_leq = sol_1 -> Q_3 <= sol_2 -> Q_3;
    bool Q_1_geq = sol_1 -> Q_1 >= sol_2 -> Q_1;
    bool Q_2_geq = sol_1 -> Q_2 >= sol_2 -> Q_2;
    bool Q_3_geq = sol_1 -> Q_3 >= sol_2 -> Q_3;
    bool Q_123_eq = (sol_1 -> Q_1 == sol_2 -> Q_1) && (sol_1 -> Q_2 == sol_2 -> Q_2) && (sol_1 -> Q_3 == sol_2 -> Q_3);
    if(Q_1_leq && Q_2_leq && Q_3_leq && not(Q_123_eq)){
        return 1;
    }else if(Q_1_geq & Q_2_geq && Q_3_geq && not(Q_123_eq)){
        return -1;
    }else{
        return 0;
    }
}
int A_dom_B(Sort_unit* sort_1, Sort_unit* sort_2){
    return A_dom_B(sort_1 -> sol_pointer,sort_2 -> sol_pointer);
}
vector<vector<Sort_unit*>> fast_sort(vector<Solution*> solution_vec_in){
    vector<Sort_unit*> first_front_vec;
    vector<vector<Sort_unit*>> sort_unit_front_tot_vec = {first_front_vec};
    vector<Sort_unit*> sort_unit_vec;
    for(Solution* p : solution_vec_in){
        Sort_unit* sort_unit_p = new Sort_unit(p);
        sort_unit_vec.push_back(sort_unit_p);
    }
    for(Sort_unit* p : sort_unit_vec){
        for(Sort_unit* q : sort_unit_vec){
            int dom_index = A_dom_B(p,q);
            if(dom_index == 1){
                //  p dominates q
                p -> s.push_back(q);
            }else if(dom_index == -1){
                //  q dominates p
                p -> n += 1;
            }
        }
        if(p -> n == 0){
            //  p belongs to first front
            p -> rank = 0;
            sort_unit_front_tot_vec[0].push_back(p);
        }
    }
    int i = 0;
    while(i < sort_unit_front_tot_vec.size()){
        if(sort_unit_front_tot_vec[i].size() == 0){
            break;
        }
        vector<Sort_unit*> F_i = sort_unit_front_tot_vec[i];
        vector<Sort_unit*> Q;
        for(Sort_unit* p : F_i){
            for(Sort_unit* q : p -> s){
                q -> n -= 1;
                if(q -> n == 0){
                    //  q belongs to the next front
                    q -> rank = i + 1;
                    Q.push_back(q);
                }
            }
        }
        i += 1;
        if(Q.size() > 0){
            if(i < sort_unit_front_tot_vec.size()){
                sort_unit_front_tot_vec[i] = Q;
            }else{
                sort_unit_front_tot_vec.push_back(Q);
            }
        }else{
            break;
        }
    }
    return sort_unit_front_tot_vec;
}
void unit_sort(vector<Sort_unit*> &unit_vector_in, int objective){
    for(int i = 0; i < unit_vector_in.size() - 1; i++){
        for(int j = 0; j < unit_vector_in.size() - i - 1; j++){
            if(objective == 1 && unit_vector_in[j] -> Q_1 > unit_vector_in[j + 1] -> Q_1){
                swap(unit_vector_in[j],unit_vector_in[j + 1]);
            }
            if(objective == 2 && unit_vector_in[j] -> Q_2 > unit_vector_in[j + 1] -> Q_2){
                swap(unit_vector_in[j],unit_vector_in[j + 1]);
            }
            if(objective == 3 && unit_vector_in[j] -> Q_3 > unit_vector_in[j + 1] -> Q_3){
                swap(unit_vector_in[j],unit_vector_in[j + 1]);
            }
        }
    }
}
void indom_sort(vector<Sort_unit*> &front_in){
    int l = front_in.size();
    if(l < 3){
        return;
    }
    for(int i = 0; i < l; i++){
        //  Initialize distance
        front_in[i] -> crowding_distance = 0;
    }
    for(int m = 1; m <= 3; m++){
        unit_sort(front_in,m);
        front_in[0] -> crowding_distance = INF_USED;
        front_in[l - 1] -> crowding_distance = INF_USED;
        for(int i = 1; i < l - 1; i++){
            if(front_in[l - 1] -> get_Q_by_m(m) == front_in[0] -> get_Q_by_m(m)){
                front_in[i] -> crowding_distance += INF_USED;
            }else{
                front_in[i] -> crowding_distance += (front_in[i + 1] -> get_Q_by_m(m) - front_in[i - 1] -> get_Q_by_m(m))/(front_in[l - 1] -> get_Q_by_m(m) - front_in[0] -> get_Q_by_m(m));
            }
        }
    }
    for(int i = 0; i < l; i++){
        if(front_in[i] -> crowding_distance > INF_USED){
            front_in[i] -> crowding_distance = INF_USED;
        }
    }
    for(int i = 0; i < l - 1; i++){
        for(int j = 0; j < l - i - 1; j++){
            if(front_in[i] -> crowding_distance < front_in[i + 1] -> crowding_distance){
                swap(front_in[i],front_in[i+1]);
            }
        }
    }
}
vector<Solution*> get_sorted_solution(vector<Solution*> solution_vec_in){
    //  Impose linear penalty function
    vector<float> Q_1_backup,Q_2_backup,Q_3_backup;
    for(Solution* s : solution_vec_in){
        Q_1_backup.push_back(s -> Q_1);
        Q_2_backup.push_back(s -> Q_2);
        Q_3_backup.push_back(s -> Q_3);
        int violate_count = s -> count_f_violate();
        float penalty = 0.01 * violate_count;
        if(violate_count > 0){
            s -> Q_1 *= (penalty + 1);
            s -> Q_2 *= (penalty + 1);
            s -> Q_3 *= (penalty + 1);
        }
    }
    //  Generate frontiers
    vector<vector<Sort_unit*>> front_tot_vec = fast_sort(solution_vec_in);
    vector<Solution*> sol_pointer_return;  //  Initialize solution pointer
    for(vector<Sort_unit*> front : front_tot_vec){
        //  Sort using crowding distance in each front
        indom_sort(front);
        for(Sort_unit* item : front){
            sol_pointer_return.push_back(item -> sol_pointer);
        }
        for(Sort_unit* su : front){
            delete su;
        }
    }
    //  Back to actual Q1,Q2,Q3
    for(int s = 0; s < solution_vec_in.size(); s++){
        solution_vec_in[s] -> Q_1 = Q_1_backup[s];
        solution_vec_in[s] -> Q_2 = Q_2_backup[s];
        solution_vec_in[s] -> Q_3 = Q_3_backup[s];
    }
    return sol_pointer_return;
}
void stop_sequence_improve(Solution* solution_in){
    int route_iter_count = 0;
    for(Route* route_iter : solution_in -> route_vector){
        int i = 1;
        while(i < route_iter -> stop_node_int_vector.size() - 3){
            int j = i;
            bool reset_i = false;
            while(j < route_iter -> stop_node_int_vector.size() - 2){
                float old_time = get_T_req_ED(route_iter -> stop_node_int_vector,false);
                swap(route_iter -> stop_node_int_vector[i],route_iter -> stop_node_int_vector[j]);
                float new_time = get_T_req_ED(route_iter -> stop_node_int_vector,false);
                if(new_time < old_time){
                    reset_i = true;
                    break;
                }else{
                    swap(route_iter -> stop_node_int_vector[i],route_iter -> stop_node_int_vector[j]);
                }
                j += 1;
            }
            if(reset_i){
                i = 1;
            }else{
                i += 1;
            }
        }
    }
    
}
void delete_sequence_repeat(Solution* solution_in){
    for(Route* r : solution_in -> route_vector){
        delete_duplicates_int_vector(r -> stop_node_int_vector);
    }
}
void insert_mutation(Solution* sol_pointer){
    int mutate_index = rand() % R_max;
    int length = sol_pointer -> route_vector[mutate_index] -> stop_node_int_vector.size();
    int mutate_pos = mutate_pos = rand() % (length - 2) + 1;
    int mutate_stop = U[rand() % U.size()];
    vector_int_insert(sol_pointer -> route_vector[mutate_index] -> stop_node_int_vector,mutate_pos,mutate_stop);
}
void delete_mutation(Solution* sol_pointer){
    int mutate_index = rand() % R_max;
    int length = sol_pointer -> route_vector[mutate_index] -> stop_node_int_vector.size();
    while(length == 3){
        mutate_index = rand() % R_max;
        length = sol_pointer -> route_vector[mutate_index] -> stop_node_int_vector.size();
    }
    int mutate_pos = rand() % (length - 3) + 1;
    index_del_int_element(sol_pointer -> route_vector[mutate_index] -> stop_node_int_vector,mutate_pos);
}
void swap_mutation(Solution* sol_pointer){
    vector<int> mutate_index_vec = rand_from_int_vector(gen_range(0,R_max,1),2);
    int stop_type = rand() % 3;
    int length_0 = sol_pointer -> route_vector[mutate_index_vec[0]] -> stop_node_int_vector.size();
    int length_1 = sol_pointer -> route_vector[mutate_index_vec[1]] -> stop_node_int_vector.size();
    if(length_0 == 3 || length_1 == 3){
        //  If any route has only 3 stops, only consider origin or destination swap
        stop_type = rand() % 2;
    }
    if(stop_type == 0){
        //  Swap terminal
        swap(sol_pointer -> route_vector[mutate_index_vec[0]] -> stop_node_int_vector[0],sol_pointer -> route_vector[mutate_index_vec[1]] -> stop_node_int_vector[0]);
    }else if(stop_type == 1){
        //  Swap destination
        int length_0 = sol_pointer -> route_vector[mutate_index_vec[0]] -> stop_node_int_vector.size();
        int length_1 = sol_pointer -> route_vector[mutate_index_vec[1]] -> stop_node_int_vector.size();
        swap(sol_pointer -> route_vector[mutate_index_vec[0]] -> stop_node_int_vector[length_0 - 1],sol_pointer -> route_vector[mutate_index_vec[1]] -> stop_node_int_vector[length_1 - 1]);
    }else{
        //  Swap intermediate
        int mutate_pos_0 = rand() % (length_0 - 3) + 1;
        int mutate_pos_1 = rand() % (length_1 - 3) + 1;
        swap(sol_pointer -> route_vector[mutate_index_vec[0]] -> stop_node_int_vector[mutate_pos_0],sol_pointer -> route_vector[mutate_index_vec[1]] -> stop_node_int_vector[mutate_pos_1]);
    }
}
void transfer_mutation(Solution* sol_pointer){
    vector<int> mutate_index_vec = rand_from_int_vector(gen_range(0,R_max,1),2);
    int length_0 = sol_pointer -> route_vector[mutate_index_vec[0]] -> stop_node_int_vector.size();
    int length_1 = sol_pointer -> route_vector[mutate_index_vec[1]] -> stop_node_int_vector.size();
    while(length_0 == 3 && length_1 == 3){
        mutate_index_vec = rand_from_int_vector(gen_range(0,R_max,1),2);
        length_0 = sol_pointer -> route_vector[mutate_index_vec[0]] -> stop_node_int_vector.size();
        length_1 = sol_pointer -> route_vector[mutate_index_vec[1]] -> stop_node_int_vector.size();
    }
    if(length_0 == 3){
        swap(mutate_index_vec[0],mutate_index_vec[1]);
        swap(length_0,length_1);
    }
    int del_index = rand() % (length_0 - 3) + 1;
    int insert_index = rand() % (length_1 - 2) + 1;
    int insert_node = sol_pointer -> route_vector[mutate_index_vec[0]] -> stop_node_int_vector[del_index];
    index_del_int_element(sol_pointer -> route_vector[mutate_index_vec[0]] -> stop_node_int_vector,del_index);
    vector_int_insert(sol_pointer -> route_vector[mutate_index_vec[1]] -> stop_node_int_vector,insert_index,insert_node);
}
void general_mutation(Solution* sol_pointer){
    int decision = rand() % 10;
    if(decision < 4){
        insert_mutation(sol_pointer);
    }else if(decision < 8){
        delete_mutation(sol_pointer);
    }else if(decision < 9){
        swap_mutation(sol_pointer);
    }else{
        transfer_mutation(sol_pointer);
    }
}
vector<Solution*> route_crossover(Solution parent_1,Solution parent_2){
    vector<Solution*> sol_output_vec;
    int iter_count = 1;
    while(true){
        Solution* child_1 = new Solution();
        Solution* child_2 = new Solution();
        child_1 -> self_AB_portion = 0.5 * (parent_1.self_AB_portion + parent_2.self_AB_portion);
        child_2 -> self_AB_portion = 0.5 * (parent_1.self_AB_portion + parent_2.self_AB_portion);
        vector<int> cut_pos_choice;
        for(int r = 0; r < R_max; r++){
            cut_pos_choice.push_back(r);
        }
        vector<int> cut_pos_vector = rand_from_int_vector(cut_pos_choice,2);
        int cut_pos_1 = cut_pos_vector[0];
        int cut_pos_2 = cut_pos_vector[1];
        if(cut_pos_1 > cut_pos_2){
            swap(cut_pos_1,cut_pos_2);
        }
        for(int i = 0; i < cut_pos_1; i++){
            Route* parent_1_route_copy = new Route(false,child_1 -> sol_all_node_vector);
            Route* parent_2_route_copy = new Route(false,child_2 -> sol_all_node_vector);
            parent_1_route_copy -> stop_node_int_vector = parent_1.route_vector[i] -> stop_node_int_vector;
            parent_2_route_copy -> stop_node_int_vector = parent_2.route_vector[i] -> stop_node_int_vector;
            parent_1_route_copy -> update_node_vector();
            parent_2_route_copy -> update_node_vector();
            child_1 -> route_vector.push_back(parent_1_route_copy);
            child_2 -> route_vector.push_back(parent_2_route_copy);
            
        }
        for(int i = cut_pos_1; i < cut_pos_2 + 1; i++){
            Route* parent_1_route_copy = new Route(false,child_1 -> sol_all_node_vector);
            Route* parent_2_route_copy = new Route(false,child_2 -> sol_all_node_vector);
            parent_1_route_copy -> stop_node_int_vector = parent_1.route_vector[i] -> stop_node_int_vector;
            parent_2_route_copy -> stop_node_int_vector = parent_2.route_vector[i] -> stop_node_int_vector;
            parent_1_route_copy -> update_node_vector();
            parent_2_route_copy -> update_node_vector();
            child_1 -> route_vector.push_back(parent_2_route_copy);
            child_2 -> route_vector.push_back(parent_1_route_copy);
        }
        for(int i = cut_pos_2 + 1; i < R_max; i++){
            Route* parent_1_route_copy = new Route(false,child_1 -> sol_all_node_vector);
            Route* parent_2_route_copy = new Route(false,child_2 -> sol_all_node_vector);
            parent_1_route_copy -> stop_node_int_vector = parent_1.route_vector[i] -> stop_node_int_vector;
            parent_2_route_copy -> stop_node_int_vector = parent_2.route_vector[i] -> stop_node_int_vector;
            parent_1_route_copy -> update_node_vector();
            parent_2_route_copy -> update_node_vector();
            child_1 -> route_vector.push_back(parent_1_route_copy);
            child_2 -> route_vector.push_back(parent_2_route_copy);
        }
        general_mutation(child_1);
        general_mutation(child_2);

        delete_sequence_repeat(child_1);
        delete_sequence_repeat(child_2);
        
        stop_sequence_improve(child_1);
        stop_sequence_improve(child_2);
        for(int r = 0; r < R_max; r++){
            child_1 -> route_vector[r] -> update_node_vector();
            child_2 -> route_vector[r] -> update_node_vector();
        }
        child_1 -> update_used_node();
        child_2 -> update_used_node();
        if(child_1 -> is_valid() && child_2 -> is_valid()){
            child_1 -> high_f_opt_express_CB_AB();
            child_2 -> high_f_opt_express_CB_AB();
            child_1 -> remove_violate();
            child_2 -> remove_violate();
            child_1 -> high_f_opt_express_CB_AB();
            child_2 -> high_f_opt_express_CB_AB();
            sol_output_vec.push_back(child_1);
            sol_output_vec.push_back(child_2);
            break;
        }else{
            delete_solution(child_1);
            delete_solution(child_2);
        }
        iter_count++;
    }
    for(Solution* sol : sol_output_vec){
        sol -> Q_1 = sol -> get_user_cost();
        sol -> Q_2 = sol -> get_oper_cost();
        sol -> Q_3 = sol -> get_infra_cost();
    }
    return sol_output_vec;
}
vector<Solution*> stop_crossover(Solution* parent_1,Solution* parent_2){
    vector<Solution*> sol_output_vec;
    int iter_count = 1;
    while(true){
        Solution* child_1 = new Solution();
        Solution* child_2 = new Solution();
        child_1 -> self_AB_portion = 0.5 * (parent_1 -> self_AB_portion + parent_2 ->self_AB_portion);
        child_2 -> self_AB_portion = 0.5 * (parent_1 -> self_AB_portion + parent_2 -> self_AB_portion);
        int route_index_select_1 = rand() % R_max;
        int dest = parent_1 -> route_vector[route_index_select_1] -> stop_node_int_vector[parent_1 -> route_vector[route_index_select_1] -> stop_node_int_vector.size() - 1];
        vector<int> parent_2_same_dest_vec;
        for(int r = 0; r < R_max; r++){
            vector<int> int_vec_copy = parent_2 -> route_vector[r] -> stop_node_int_vector;
            if(int_vec_copy[int_vec_copy.size() - 1] == dest){
                parent_2_same_dest_vec.push_back(r);
            }
        }
        int route_index_select_2 = rand_from_int_vector(parent_2_same_dest_vec,1)[0];
        if(route_index_select_1 > route_index_select_2){
            swap(route_index_select_1,route_index_select_2);
        }
        vector<int> route_1_cut_pos;
        vector<int> route_2_cut_pos;
        if(parent_1 -> route_vector[route_index_select_1] -> stop_node_int_vector.size() == 4){
            route_1_cut_pos = {1,1};
        }else if(parent_1 -> route_vector[route_index_select_1] -> stop_node_int_vector.size() == 3){
            route_1_cut_pos = {1,0};
        }else{
            route_1_cut_pos = rand_from_int_vector(gen_range(1,parent_1 -> route_vector[route_index_select_1] -> stop_node_int_vector.size() - 2,1),2);
        }
        if(parent_2 -> route_vector[route_index_select_2] -> stop_node_int_vector.size() == 4){
            route_2_cut_pos = {1,1};
        }else if(parent_2 -> route_vector[route_index_select_2] -> stop_node_int_vector.size() == 3){
            route_2_cut_pos = {1,0};
        }else{
            route_2_cut_pos = rand_from_int_vector(gen_range(1,parent_2 -> route_vector[route_index_select_2] -> stop_node_int_vector.size() - 2,1),2);
        }
        if(route_1_cut_pos[0] > route_1_cut_pos[1] && route_1_cut_pos[1] != 0){
            swap(route_1_cut_pos[0],route_1_cut_pos[1]);
        }
        if(route_2_cut_pos[0] > route_2_cut_pos[1] && route_2_cut_pos[1] != 0){
            swap(route_2_cut_pos[0],route_2_cut_pos[1]);
        }
        for(int i = 0; i < route_index_select_1; i++){
            Route* tmp_route_1 = new Route(false,child_1 -> sol_all_node_vector);
            tmp_route_1 -> stop_node_int_vector = parent_1 -> route_vector[i] -> stop_node_int_vector;
            tmp_route_1->update_node_vector();
            child_1 -> route_vector.push_back(tmp_route_1);
        }
        for(int i = 0; i < route_index_select_2; i++){
            Route* tmp_route_2 = new Route(false,child_2 -> sol_all_node_vector);
            tmp_route_2 -> stop_node_int_vector = parent_2 -> route_vector[i] -> stop_node_int_vector;
            tmp_route_2->update_node_vector();
            child_2 -> route_vector.push_back(tmp_route_2);
        }
        vector<int> tmp_stop_int_1,tmp_stop_int_2;
        Route* child_route_1 = new Route(false,child_1 -> sol_all_node_vector);
        Route* child_route_2 = new Route(false,child_2 -> sol_all_node_vector);
        for(int i = 0; i < route_1_cut_pos[0]; i++){
            tmp_stop_int_1.push_back(parent_1 -> route_vector[route_index_select_1] -> stop_node_int_vector[i]);
        }
        for(int i = route_2_cut_pos[0]; i <= route_2_cut_pos[1]; i++){
            tmp_stop_int_1.push_back(parent_2 -> route_vector[route_index_select_2] -> stop_node_int_vector[i]);
        }
        for(int i = route_1_cut_pos[1] + 1; i < parent_1 -> route_vector[route_index_select_1] -> stop_node_int_vector.size(); i++){
            tmp_stop_int_1.push_back(parent_1 -> route_vector[route_index_select_1] -> stop_node_int_vector[i]);
        }
        child_route_1 -> stop_node_int_vector = tmp_stop_int_1;
        child_route_1 -> update_node_vector();
        child_1 -> route_vector.push_back(child_route_1);
        for(int i = 0; i < route_2_cut_pos[0]; i++){
            tmp_stop_int_2.push_back(parent_2 -> route_vector[route_index_select_2] -> stop_node_int_vector[i]);
        }
        for(int i = route_1_cut_pos[0]; i <= route_1_cut_pos[1]; i++){
            tmp_stop_int_2.push_back(parent_1 -> route_vector[route_index_select_1] -> stop_node_int_vector[i]);
        }
        for(int i = route_2_cut_pos[1] + 1; i < parent_2 -> route_vector[route_index_select_2] -> stop_node_int_vector.size(); i++){
            tmp_stop_int_2.push_back(parent_2 -> route_vector[route_index_select_2] -> stop_node_int_vector[i]);
        }
        child_route_2 -> stop_node_int_vector = tmp_stop_int_2;
        child_route_2 -> update_node_vector();
        child_2 -> route_vector.push_back(child_route_2);
        for(int i = route_index_select_1 + 1; i < R_max; i++){
            Route* tmp_route_1 = new Route(false,child_1 -> sol_all_node_vector);
            tmp_route_1 -> stop_node_int_vector = parent_1 -> route_vector[i] -> stop_node_int_vector;
            tmp_route_1 -> update_node_vector();
            child_1 -> route_vector.push_back(tmp_route_1);
        }
        for(int i = route_index_select_2 + 1; i < R_max; i++){
            Route* tmp_route_2 = new Route(false,child_2 -> sol_all_node_vector);
            tmp_route_2 -> stop_node_int_vector = parent_2 -> route_vector[i] -> stop_node_int_vector;
            tmp_route_2 -> update_node_vector();
            child_2 -> route_vector.push_back(tmp_route_2);
        }
        general_mutation(child_1);
        general_mutation(child_2);
        delete_sequence_repeat(child_1);
        delete_sequence_repeat(child_2);
        stop_sequence_improve(child_1);
        stop_sequence_improve(child_2);
        child_1 -> update_used_node();
        child_2 -> update_used_node();
        if(child_1 -> is_valid() && child_2 -> is_valid()){
            child_1 -> high_f_opt_express_CB_AB();
            child_2 -> high_f_opt_express_CB_AB();
            child_1 -> remove_violate();
            child_2 -> remove_violate();
            child_1 -> high_f_opt_express_CB_AB();
            child_2 -> high_f_opt_express_CB_AB();
            sol_output_vec.push_back(child_1);
            sol_output_vec.push_back(child_2);
            break;
        }else{
            delete_solution(child_1);
            delete_solution(child_2);
        }
        iter_count++;
    }
    for(Solution* sol : sol_output_vec){
        sol -> Q_1 = sol -> get_user_cost();
        sol -> Q_2 = sol -> get_oper_cost();
        sol -> Q_3 = sol -> get_infra_cost();
    }
    return sol_output_vec;
}

#endif