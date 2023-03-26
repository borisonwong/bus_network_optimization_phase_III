#ifndef TRAFFIC_ROUTE_H
#define TRAFFIC_ROUTE_H

class Traffic_Route{
    public:
    int i, e, x, y, m, n, r_1, r_2;
    float in_veh_time, waiting_time, walking_time, cost;
    bool no_interchange;
    bool no_interchange_walk;
    vector<int> route_a_int_vector;
    vector<int> route_b_int_vector;
    Traffic_Route(int i_in, int e_in, int x_in, int y_in, int m_in, int n_in, Route* route_a_in, Route* route_b_in, bool is_inertia, int route_index_1, int route_index_2){
        i = i_in;
        e = e_in;
        x = x_in;
        y = y_in;
        m = m_in;
        n = n_in;
        r_1 = route_index_1;
        r_2 = route_index_2;
        route_a_int_vector = route_a_in -> stop_node_int_vector;
        route_b_int_vector = route_b_in -> stop_node_int_vector;
        if(m == -1 || n == -1){
            no_interchange = true;
        }else{
            no_interchange = false;
        }
        if(m == n){
            no_interchange_walk = true;
        }else{
            no_interchange_walk = false;
        }
        in_veh_time = 0;
        waiting_time = 0;
        walking_time = 0;
        cost = 0;
        if(no_interchange){
            if(is_inertia){
                in_veh_time = route_a_in -> get_time_ij(x, y);
                waiting_time += 60 / route_a_in -> f_CB;
            }else{
                float f_tot_sum = route_a_in -> f_AB + route_a_in -> f_CB;
                float numerator = (route_a_in -> f_AB * alpha + route_a_in -> f_CB) * route_a_in -> get_time_ij(x,y);
                in_veh_time = numerator / f_tot_sum;
                waiting_time += 60 / (route_a_in -> f_CB + route_a_in -> f_AB);
            }
        }else{
            if(is_inertia){
                in_veh_time = route_a_in -> get_time_ij(x, m) + route_b_in -> get_time_ij(n, y);
                waiting_time += 60 / route_a_in -> f_CB + 60 / route_b_in -> f_CB;
            }else{
                float f_tot_sum_a = route_a_in -> f_AB + route_a_in -> f_CB;
                float numerator_a = (route_a_in -> f_AB * alpha + route_a_in -> f_CB) * route_a_in -> get_time_ij(x,m);
                float f_tot_sum_b = route_b_in -> f_AB + route_b_in -> f_CB;
                float numerator_b = (route_b_in -> f_AB * alpha + route_b_in -> f_CB) * route_b_in -> get_time_ij(n,y);
                in_veh_time = numerator_a / f_tot_sum_a + numerator_b / f_tot_sum_b;
                waiting_time += 60 / (route_a_in -> f_CB + route_a_in -> f_AB) + 60 / (route_b_in -> f_CB + route_b_in -> f_AB);
            }
        }
        if(no_interchange_walk == false){
            walking_time += get_c_ij(m,n,false) * beta;
        }
        if(i != x){
            walking_time += get_c_ij(i,x,false) * beta;
        }
        if(e != y){
            walking_time += get_c_ij(e,y,false) * beta;
        }
        cost = in_veh_time * gamma_veh + waiting_time * gamma_wt + walking_time * gamma_wa;
    }
    void print(){
        cout << "============ TRAFFIC ROUTE ============" << endl;
        cout << "i: " << i << endl;
        cout << "x: " << x << endl;
        cout << "m: " << m << endl;
        cout << "n: " << n << endl;
        cout << "y: " << y << endl;
        cout << "e: " << e << endl;
        cout << "Route a: ";
        print_int_vector(route_a_int_vector);
        cout << "Route b: ";
        print_int_vector(route_b_int_vector);
        cout << "in_veh_time = " << in_veh_time << endl;
        cout << "walking_time = " << walking_time << endl;
        cout << "waiting_time = " << waiting_time << endl;
        cout << "cost = " << cost << endl;
        cout << "=======================================" << endl;
    }
};
vector<float> get_logit_prob(vector<Traffic_Route*> traffic_vector_in){
    vector<float> prob_vec;
    float sum = 0;
    for(Traffic_Route* t : traffic_vector_in){
        sum += exp(1 / t -> cost);
    }
    for(Traffic_Route* t : traffic_vector_in){
        prob_vec.push_back(exp(1 / t -> cost)/sum);
    }
    return prob_vec;
}
float get_expected_cost(vector<Traffic_Route*> traffic_vector_in){
    float exp_cost = 0;
    vector<float> prob_vector = get_logit_prob(traffic_vector_in);
    for(int i = 0; i < prob_vector.size(); i++){
        exp_cost += prob_vector[i] * traffic_vector_in[i] -> cost;
    }
    return exp_cost;
}
#endif