#ifndef NODE_CLASS_H
#define NODE_CLASS_H

class Node{
    public:
    int index;
    bool is_terminal, is_upgraded, is_destination, is_exchange, is_used;
    Node(int index_input, bool is_terminal_input = false, bool is_destination_input = false, bool is_exchange_input = false, bool is_upgraded_input = false){
        index = index_input;
        is_terminal = is_terminal_input;
        is_upgraded = is_upgraded_input;
        is_destination = is_destination_input;
        is_exchange = is_exchange_input;
        is_used = false;
    }
    void upgrade(){
        is_upgraded = true;
    }
    void use(){
        is_used = true;
    }
    void unuse(){
        is_used = false;
    }
};

#endif