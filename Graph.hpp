//
// Created by boy on 02/07/22.
//

#ifndef DAG_LIB_GRAPH_HPP
#define DAG_LIB_GRAPH_HPP

#include <cstdlib>
#include <vector>
#include "Node.hpp"
#include "ThreadPool.hpp"
#include "Utils.hpp"

#define NWORKER 1
using namespace std;

template<class T>
class Graph {
private:
    vector<Node<T> *> input_nodes;
    vector<Node<T> *> output_nodes;
    vector<Node<T> *> middle_nodes;

    vector<Node<T> *> fireAble;

    mutex mtx_graph;
    Thread_pool *tp{};

//    size_t sum;

//    int count;
public:
    Graph() : Graph(NWORKER, true){};

    Graph(int nw, bool flag){
        tp = new Thread_pool(nw);
        if(flag) tp->start();
//        sum = 0;
    }

    explicit Graph(Thread_pool *threadPool, bool flag){
        tp = threadPool;
//        count = 0;
        if (flag) tp->start();
    }

    void addNode(Node<T> *newNode) {
        switch (newNode->getNodeType()) {
            case IN:
                input_nodes.push_back(newNode);
                break;
            case OUT:
                output_nodes.push_back(newNode);
                break;
            case MIDDLE:
                middle_nodes.push_back(newNode);
                break;
            default:
                break;
        }
    };

    void compute(vector<T> value) {
        int i =0;

        for (auto it: value) {
            for (Node<T> *n: input_nodes) {
                n->push_value(it, i, 0);
                auto wrapper = bind(&Graph<T>::fire, this, n, i);
                tp->addTask(wrapper);
            }
            i++;
        }

    };

    void fire(Node<T> *node, int i) {

        node->doTask(i);

//        unique_lock<mutex> lock_graph(mtx_graph);
        for (Node<T> *n: node->getDepNodeList()) {
            if (n->isReady()) {
                tp->addTask(bind(&Graph<T>::fire, this, n, i));
            }
        }
    }

    void compute_sequential(vector<T> value) {
        int i =0;
        for (auto it: value) {
            for (Node<T> *n: input_nodes) {
//                count++;
                n->push_value(it, i, 0);
                fire_sequential(n, i);
            }
            i++;
        }
//        cout << "sum = "<< sum <<" count = "<<count<<endl;
//        cout << "mean comp time = "<<double(sum)/double(count)<<endl;
    };

    void fire_sequential(Node<T> *node, int i){
//        START(time);
        node->doTask(i);
//        cout << elapsed<<endl;
        for (Node<T> *n: node->getDepNodeList()) {
            if (n->isReady()) {
//                count++;
                fire_sequential(n, i);
            }
        }
//        STOP(time, elapsed);
//        sum += elapsed;
    }

    void startExecutor(){
        tp->start();
    }

    void terminate() {
        tp->shutdown();
//        cout<< "total task done: "<<tp->getN_done()<<endl;
//        cout << "total task added:" << tp->getN_added()<<endl;
    }

//    int getCount(){
//        return count;
//    }
};

#endif //DAG_LIB_GRAPH_HPP
