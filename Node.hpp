//
// Created by boy on 02/07/22.
//

#ifndef DAG_LIB_NODE_HPP
#define DAG_LIB_NODE_HPP

#include <assert.h>
#include <cstdlib>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include <set>
#include <thread>
#include <vector>

#include "Utils.hpp"

#define IN 0
#define OUT 1
#define MIDDLE 2

#define DEBUG 0

#define EOS -1000000

using namespace std;

template<class T>
struct token {
    vector<T> value;
    int id;
    int counter;

    token(int id, int c) : id(id), counter(c) {
        value.resize(c);
#if(DEBUG)
        cout<<"constructor:";
        print();
#endif
    };

    /**
     * input is received in channel denoted by pos, then decrement the counter
     * once the counter is 0 means that the node has all input, ready to be processed
     * @param value input value
     * @param pos aka channel
     */
    void set_value(T val, int pos) {
        value[pos] = val;
        counter--;
    }

    bool isReady() {
        return counter == 0;
    }

    T getValue(int pos) {
        return value[pos];
    }

    void print() {
        cout << "\tsequence ID: " << id;
        cout << " [" << counter << ",";

        for (auto v: value) {
            cout << v << " ";
        }
        cout << "]" << endl;
    }
};

template<class T>
class Node {
private:
    int id;
    int n_in;
    int n_out;
    int channel_count;
    int nodeType;
    int fireable_token;

    map<int, int> parentToChannel;
    vector<Node<T> *> dep_node_list;
    function<void(void)> f;

    map<int, token<T>> input_map;
    queue<token<T>> readyTokens;
    vector<pair<int,T>> output;

    mutex mtx_input_map;
    mutex mtx_ready_token;

    mutex mtx_fireable;
    condition_variable cv_token_queue;

public:
    Node(int id, int n_in, int n_out, int type) {
        Node::id = id;
        Node::n_in = n_in;
        Node::n_out = n_out;
        Node::nodeType = type;
        channel_count = 0;
        fireable_token = 0;
    }

    void addDep(Node *child) {
//        cout << "Node "<< getId() <<"'s output channel "<<dep_node_list.size() << " associated to nodeID " << child->getId() << endl;
        dep_node_list.push_back(child);
        child->associate_parent_channel(getId());
    }

    void associate_parent_channel(int parent_id) {
        parentToChannel[parent_id] = channel_count++;
        assert(channel_count <= n_in);
    }

    void addCompute(function<void(void)> task) {
        f = task;
    }

    void doTask(int id_seq) {
        f();
    }

    /**
     * send the output value to the indicated channel (which correspond to the child),
     * must be called in the user defined function for each output
     * @param value the output to be send
     * @param channel the relative channel [0 - n_out)
     */
    void send_out(T value, int channel, int id_seq) {
        if (nodeType == OUT) {
//            output.emplace_back(id_seq, value);
//            cout << value <<endl;
            return;
        }
#if (DEBUG)
        cout << "sending " << value << " to " << channel << " ";
#endif
        dep_node_list[channel]->push_value(value, id_seq, this);
    }

    /**
     * maps the channel with parent node
     * @param value
     * @param parent
     */
    void push_value(T value, int id_sequence, Node *parent) {
        push_value(value, id_sequence, parentToChannel[parent->getId()]);
    }

    /**
     * push input to the relative input queue
     * @param value aka input for the node
     * @param id_channel aka n-th input queue
     */
    void push_value(T value, int id_sequence, int id_channel) {
        unique_lock<mutex> lock(mtx_input_map);
        auto it = input_map.find(id_sequence);
        if (it == input_map.end()) {
#if (DEBUG)
            cout <<"generate new token <"<< id_sequence<<","<< n_in<<">"<<endl;
#endif
            auto newToken = token<T>(id_sequence, n_in);
            newToken.set_value(value, id_channel);
            auto temp = input_map.insert(pair<int, token<T>>(id_sequence, newToken));
            it = temp.first;
        } else {
            it->second.set_value(value, id_channel);
        }
        unique_lock ready_lock(mtx_ready_token);
        if (it->second.isReady()) {
            auto tok = it->second;
            input_map.erase(it);
            readyTokens.push(tok);
            ready_to_fire();
            cv_token_queue.notify_one();
        }

#if (DEBUG)
        cout << "pushed to node ID " << getId() << " channel " << id_channel << endl;
        cout << "Node " <<getId()<<": ";
        printMap(input_map);
        printQueue(readyTokens);
#endif
    }

    /**
     * user must call this function inside the task function to bind each input with the queue
     * @param input_list a list of input to be binded
     */
    int binds_inputs(initializer_list<T *> input_list) {
        unique_lock<mutex> lock_token(mtx_ready_token);
        auto newToken = readyTokens.front();
        readyTokens.pop();
        int i = 0;
        for (auto in: input_list) {
            *in = newToken.getValue(i++);
        }
        return newToken.id;
    }

    /**
     * checks if the instruction is fire-able, if true it is sends to the thread-pool and executed
     * @return
     */
    bool isReady() {
        unique_lock l(mtx_fireable);
        if (fireable_token > 0) {
            fireable_token--;
            assert(fireable_token >= 0);
            return true;
        }
        return false;
    }

    void ready_to_fire() {
        unique_lock l(mtx_fireable);
        fireable_token++;
    }

    void printQueue(queue<token<T>> q) {
        cout << "readyTokens of Node " << getId() << ":" << " size = "<<q.size()<<endl;
        for (int j = 0; j < q.size(); j++) {
            auto v = q.front();
            v.print();
            q.push(q.front());
            q.pop();
        }
        cout << endl;
    }

    void printMap(map<int, token<T>> m) {
        cout << "map of Node " << getId() << ", "<<"size = "<<m.size();
        for (auto temp: m) {
            temp.second.print();
        }
    }
#if (DEBUG)
    void debug() {
        cout << "id " << getId() << endl;
        cout << "n_in " << getNIn() << endl;
        cout << "n_out " << getNOut() << endl;
        cout << "nodeType " << isOutFlag() << endl;
        cout << "queue size " << int(readyTokens.size()) << endl;
    }
#endif

    int getId() const {
        return id;
    }

    int getNIn() const {
        return n_in;
    }

    int getNOut() const {
        return n_out;
    }

    bool isOutFlag() const {
        return nodeType;
    }

    int getNodeType() const {
        return nodeType;
    }

    const vector<Node<T> *> &getDepNodeList() const {
        return dep_node_list;
    }

    const map<int, token<T>> &getInputMap() const {
        return input_map;
    }

    const queue<token<T>> &getReadyTokens() const {
        return readyTokens;
    }

    const vector<pair<int, T>> &getOutput() const {
        return output;
    }

};

#endif //DAG_LIB_NODE_HPP
