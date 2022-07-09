//
// Created by boy on 02/07/22.
//

#ifndef DAG_LIB_GRAPH_HPP
#define DAG_LIB_GRAPH_HPP

#include <cstdlib>
#include <vector>
#include "Node.hpp"
#include "ThreadPool.hpp"

#define NWORKER 4
using namespace std;

template<class T>
class Graph {
private:
    vector<Node<T> *> input_nodes;
    vector<Node<T> *> output_nodes;
    vector<Node<T> *> middle_nodes;

    vector<Node<T> *> fireAble;

    mutex mtx_graph;
    Thread_pool *tp;
public:
    Graph() {
        tp = new Thread_pool(NWORKER);
    };

    explicit Graph(Thread_pool *threadPool){
        tp = threadPool;
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
        //IF PARALLEL STARTS HERE
        tp->start();
        int i =0;
        for (auto it: value) {
            for (Node<T> *n: input_nodes) {
                if (DEBUG)cout << "input "<< i<<":";
                n->push_value(it, i, 0);
                auto wrapper = bind(&Graph<T>::fire, this, n, i);
                tp->addTask(wrapper);
            }
            i++;
        }

//        for (Node<T> *n: input_nodes) {
//            n->push_value(EOS, i, 0);
//            auto wrapper = bind(&Graph<T>::fire, this, n, i);
//            tp->addTask(wrapper);
//        }
//        i++;
        //IF SEQUENTIAL STARTS HERE
//        tp->start();
    };

    void fire(Node<T> *node, int i) {
        node->doTask(i);
        unique_lock<mutex> lock_graph(mtx_graph);
        for (Node<T> *n: node->getDepNodeList()) {
            if (n->isReady()) {
                tp->addTask(bind(&Graph<T>::fire, this, n, i));
            }
        }
    }

    void compile() {

    };

    void terminate() {
        tp->shutdown();
        cout<< "total task done: "<<tp->getN_done()<<endl;
        cout << "total task added:" << tp->getN_added()<<endl;

//        for(auto node : input_nodes){
//            cout <<" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"<<endl;
//            node->printMap(node->getInputMap());
//            node->printQueue(node->getReadyTokens());
//        }
//
//        for(auto node : middle_nodes){
//            cout <<" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"<<endl;
//            node->printMap(node->getInputMap());
//            node->printQueue(node->getReadyTokens());
//        }
//
//        for( auto node: output_nodes){
//            cout <<" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"<<endl;
//            node->printMap(node->getInputMap());
//            node->printQueue(node->getReadyTokens());
//        }
    }
};


#endif //DAG_LIB_GRAPH_HPP
