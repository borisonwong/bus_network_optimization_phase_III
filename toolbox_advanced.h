#ifndef TOOLBOX_ADVANCED_H
#define TOOLBOX_ADVANCED_H

void init_all_node_vector(vector<Node*> &all_node_vector){
    for(int i : Z){
        Node* tmp_node = new Node(i);
        all_node_vector.push_back(tmp_node);
    }
    for(int i : Y){
        all_node_vector[i - 1] -> is_terminal = true;
    }
    for(int i : V){
        all_node_vector[i - 1] -> is_destination = true;
    }
    for(int i : C){
        all_node_vector[i - 1] -> is_exchange = true;
    }
}


#endif