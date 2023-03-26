#ifndef TOOLBOX_H
#define TOOLBOX_H

int rand_between(int i, int j){
    if(i > j){
        std::swap(i,j);
    }
    return rand() % (j - i + 1) + i;
}
float get_c_ij(int i, int j,bool is_express){
    if(is_express){
        return all_time_matrix_express[i - 1][j - 1];
    }else{
        return all_time_matrix_ordinary[i - 1][j - 1];
    }
}
float get_demand_ij(int i, int e){
    return demand_matrix[i - 1][e - 16];
}
float get_T_req(vector<int> stop_sequence, bool is_express){
    float tmp_time = 0;
    for(int i = 0; i < stop_sequence.size() - 1; i++){
        tmp_time += get_c_ij(stop_sequence[i],stop_sequence[i + 1],is_express) + s;
    }
    return tmp_time - s;
}
float get_T_req_ED(vector<int> stop_sequence, bool is_express){
    float tmp_time = 0;
    int EHT_index = 0;
    for(int i = 0; i < stop_sequence.size(); i++){
        if(stop_sequence[i] == 21){
            EHT_index = i;
            break;
        }
    }
    for(int i = 0; i < EHT_index; i++){
        tmp_time += get_c_ij(stop_sequence[i],stop_sequence[i + 1],is_express) + s;
    }
    return tmp_time - s;
}
void index_del_int_element(vector<int> &input_vec, int index){
    input_vec.erase(input_vec.begin() + index);
}
void print_int_vector(vector<int> &input_vec){
    for(int i : input_vec){
        cout << setw(5) << i;
    }
    cout << endl;
}
void print_float_vector(vector<float> &input_vec,int width){
    for(float i : input_vec){
        cout << setw(width) << i;
    }
    cout << endl;
}
bool in_int_vector(vector<int> input_vec, int target){
    for(int i : input_vec){
        if(target == i){
            return true;
        }
    }
    return false;
}
void value_del_all_int_element(vector<int> &input_vec, int target){
    while(true){
        int target_index = -1;
        for(int i = 0; i < input_vec.size(); i++){
            if(target == input_vec[i]){
                target_index = i;
                break;
            }
        }
        if(target_index == -1){
            break;
        }else{
            index_del_int_element(input_vec,target_index);
        }
    }
}
vector<int> rand_from_int_vector(vector<int> input_vec,int size){
    vector<int> tmp_vector;
    vector<int> vec_copy = input_vec;
    for(int i = 0; i < size; i++){
        int choose_index = rand_between(0,vec_copy.size() - 1);
        tmp_vector.push_back(vec_copy[choose_index]);
        index_del_int_element(vec_copy,choose_index);
    }
    return tmp_vector;
}
void vector_int_insert(vector<int> &input_vec, int position, int value){
    input_vec.insert(input_vec.begin() + position, value);
}
bool check_zero_in_vector(vector<int> vector_in){
    for(int i : vector_in){
        if (i == 0){
            return true;
        }
    }
    return false;
}
vector<int> rand_with_sum(int size, int sum, bool zero_check = true){
    if(zero_check){
        sum -= size;
    }
    vector<float> tmp_float_vector;
    vector<int> output_vector;
    float tmp_sum_float = 0;
    float tmp_sum_int = 0;
    for(int i = 0; i < size; i++){
        float tmp_rand = rand() % 10000;
        tmp_float_vector.push_back(tmp_rand);
        tmp_sum_float += tmp_rand;
    }
    for(float i : tmp_float_vector){
        output_vector.push_back(static_cast<int>((i / tmp_sum_float) * sum));
        tmp_sum_int += static_cast<int>((i / tmp_sum_float) * sum);
    }
    int delta = sum - tmp_sum_int;
    output_vector[0] += delta;
    if(zero_check){
        for(int i = 0; i < size; i++){
            output_vector[i] += 1;
        }
    }
    return output_vector;
}
void pause(){
    int tmp;
    cin >> tmp;
}
bool is_int_vector_identical(vector<int> vec_1,vector<int> vec_2){
    if(vec_1.size() == vec_2.size()){
        for(int i = 0; i < vec_1.size(); i++){
            if(vec_1[i] != vec_2[i]){
                return false;
            }
        }
        return true;
    }else{
        return false;
    }
}
vector<int> gen_range(int i, int j, int step){
    vector<int> tmp_vec;
    //  j is not included
    for(int iter = i; iter < j; iter += step){
        tmp_vec.push_back(iter);
    }
    return tmp_vec;
}
vector<int> rand_from_float_vector(vector<float> input_vec,int size){
    vector<int> tmp_vector;
    vector<float> vec_copy = input_vec;
    // cout << "rand_from_float_vector input = " << endl;
    // for(float f : input_vec){
    //     cout << setw(10) << setprecision(3) << f << setprecision(6);
    // }
    // cout << endl;
    for(int i = 0; i < size; i++){
        vector<int> range_vector;
        float sum = 0;
        for(float float_iter : vec_copy){
            sum += float_iter;
        }
        int new_sum = 0;
        for(int iter = 0; iter < vec_copy.size(); iter++){
            vec_copy[iter] /= sum;
            vec_copy[iter] *= 1000;
        }
        for(int iter = 0; iter < vec_copy.size(); iter++){
            if(iter == 0){
                new_sum += round(vec_copy[iter]);
                range_vector.push_back(round(vec_copy[iter]));
            }else{
                new_sum += round(vec_copy[iter]);
                range_vector.push_back(round(vec_copy[iter]) + range_vector[iter - 1]);
            }
        }
        int choose_range = rand() % new_sum;
        int chosen_index = -1;
        // print_int_vector(range_vector);
        // cout << choose_range << endl;
        for(int iter = 0; iter < range_vector.size(); iter++){
            if(choose_range < range_vector[iter]){
                chosen_index = iter;
                break;
            }
        }
        if(chosen_index == -1){
            cout << "CHOOSE RANDOM ERROR" << endl;
        }
        tmp_vector.push_back(chosen_index);
        vec_copy.erase(vec_copy.begin() + chosen_index);
    }
    return tmp_vector;
}
void delete_duplicates_int_vector(vector<int> &input_vec){
    vector<int> tmp_store_vector;
    for(int i : input_vec){
        bool in_list = false;
        for(int j : tmp_store_vector){
            if(i == j){
                in_list = true;
                break;
            }
        }
        if(in_list == false){
            tmp_store_vector.push_back(i);
        }
    }
    input_vec = tmp_store_vector;
}
void repair_CB_vector(vector<int> &input_vec){
    vector<int> index_order;
    for(int iter = 0; iter < input_vec.size(); iter++){
        index_order.push_back(iter);
    }
    for(int i = 0; i < index_order.size(); i++){
        for(int j = i; j < index_order.size(); j++){
            int index_1 = index_order[i];
            int index_2 = index_order[j];
            if(input_vec[index_1] < input_vec[index_2]){
                swap(index_order[i],index_order[j]);
            }
        }
    }
    int zero_count = 0;
    for(int r = 0; r < input_vec.size(); r++){
        if(input_vec[r] == 0){
            input_vec[index_order[zero_count]] -= 1;
            input_vec[r] += 1;
            zero_count += 1;
        }
    }
}
void fix_sum(vector<int> &CB_vec, vector<int> &AB_vec){
    int CB_count = 0;
    int AB_count = 0;
    for(int r = 0; r < R_max; r++){
        CB_count += CB_vec[r];
        AB_count += AB_vec[r];
    }
    int tot_count = CB_count + AB_count;
    while(tot_count != W){
        if(tot_count > W){
            //  Handle too many buses
            if(AB_count > W * AB_portion){
                //  Too many AB
                int del_index;
                int max_AB;
                for(int i = 0; i < AB_vec.size(); i++){
                    if(i == 0){
                        del_index = 0;
                        max_AB = AB_vec[i];
                    }else{
                        if(AB_vec[i] > max_AB){
                            del_index = i;
                            max_AB = AB_vec[i];
                        }
                    }
                }
                AB_vec[del_index] -= 1;
            }else{
                //  Too many CB
                int del_index;
                int max_CB;
                for(int i = 0; i < CB_vec.size(); i++){
                    if(i == 0){
                        del_index = 0;
                        max_CB = CB_vec[i];
                    }else{
                        if(CB_vec[i] > max_CB){
                            del_index = i;
                            max_CB = CB_vec[i];
                        }
                    }
                }
                CB_vec[del_index] -= 1;
            }
            tot_count -= 1;
        }
        if(tot_count < W){
            //  Handle too few buses
            if(AB_count < W * AB_portion){
                //  Too few AB
                int add_index;
                int max_AB;
                for(int i = 0; i < AB_vec.size(); i++){
                    if(i == 0){
                        add_index = 0;
                        max_AB = AB_vec[i];
                    }else{
                        if(AB_vec[i] > max_AB){
                            add_index = i;
                            max_AB = AB_vec[i];
                        }
                    }
                }
                AB_vec[add_index] += 1;
            }else{
                //  Too few CB
                int add_index;
                int max_CB;
                for(int i = 0; i < CB_vec.size(); i++){
                    if(i == 0){
                        add_index = 0;
                        max_CB = CB_vec[i];
                    }else{
                        if(CB_vec[i] > max_CB){
                            add_index = i;
                            max_CB = CB_vec[i];
                        }
                    }
                }
                CB_vec[add_index] += 1;
            }
            tot_count += 1;
        }
    }
}
vector<float> get_index_fitness(int size){
    vector<float> return_vector;
    for(int i = size; i > 0; i--){
        return_vector.push_back(i);
    }
    return return_vector;
}
#endif