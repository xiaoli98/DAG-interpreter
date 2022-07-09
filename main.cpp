#include "Graph.hpp"
#include "Node.hpp"
#include "Utils.hpp"

int main(int argc, char *argv[]) {
    auto* a = new Node<int>(1, 1, 3, IN);
    auto* b = new Node<int>(2, 1, 1, MIDDLE);
    auto* c = new Node<int>(3, 1, 2, MIDDLE);
    auto* d = new Node<int>(4, 2, 1, MIDDLE);
    auto* e = new Node<int>(5, 3, 1, OUT);

//    auto* a = new Node<int>(1, 1, 1, IN);
//    auto* b = new Node<int>(2, 1, 1, MIDDLE);
//    auto* c = new Node<int>(3, 1, 1, MIDDLE);
//    auto* d = new Node<int>(4, 1, 1, MIDDLE);
//    auto* e = new Node<int>(5, 1, 1, OUT);

    Graph<int> g;

    a->addDep(b);
    a->addDep(c);
    a->addDep(e);
    b->addDep(d);
    c->addDep(d);
    c->addDep(e);
    d->addDep(e);

//    a->addDep(b);
//    b->addDep(c);
//    c->addDep(d);
//    d->addDep(e);


    a->addCompute([&a]() {
        int in;
        int id = a->binds_inputs({&in});
        int out1 = in + 1;
        int out2 = in + 2;
        int out3 = in + 3;

        a->send_out(out1, 0, id);
        a->send_out(out2, 1, id);
        a->send_out(out3, 2, id);
//        int in;
//        int id = a->binds_inputs({&in});
//        int out1 = in + 1;
//        a->send_out(out1, 0, id);
    });

    b->addCompute([&b]() {
        int in;
        int id = b->binds_inputs({&in});
        int out1 = in + 10;
        b->send_out(out1, 0, id);
    });

    d->addCompute([&d]() {
        int in1, in2;
        int id = d->binds_inputs({&in1, &in2});
        int out1 = in1 + in2;
        d->send_out(out1, 0, id);
//        int in1;
//        int id = d->binds_inputs({&in1});
//        int out1 = in1 +1;
//        d->send_out(out1, 0, id);
    });

    e->addCompute([&e]() {
        int in1;
        int in2;
        int in3;
        int id = e->binds_inputs({&in1, &in2, &in3});
        int out1 = in1 * in2 + in3;
        e->send_out(out1, 0, id);
//        int in1;
//        int id = e->binds_inputs({&in1});
//        int out1 = in1 * 2;
//        e->send_out(out1, 0, id);
    });

    c->addCompute([&c]() {
        int in1;
        int id = c->binds_inputs({&in1});
        int out1 = in1 * 3;
        int out2 = in1 + 50;
        c->send_out(out1, 0, id);
        c->send_out(out2, 1, id);
//        int in1;
//        int id = c->binds_inputs({&in1});
//        int out1 = in1 * 3;
//        c->send_out(out1, 0, id);
    });

    g.addNode(a);
    g.addNode(b);
    g.addNode(c);
    g.addNode(d);
    g.addNode(e);

    vector<int> inputs;
    for (int i = 1; i < 1000; i++) {
//        cout<<"pushing values"<<endl;
        inputs.push_back(i);
    }
    g.compute(inputs);
//    cout<<"finished"<<endl;

    g.terminate();
    return 0;
}

//int main(int argc, char *argv[]) {
//    vector<int> input;
//    for(int i= 0; i< 10; i++){
//        input.push_back(i);
//    }
//    input.push_back(NULL);
//
//    for(int i=0; i < input.size(); i++){
//        if(input[i] == NULL) cout <<"null"<<endl;
//        cout << input[i]<<endl;
//    }
//}