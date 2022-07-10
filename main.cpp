#include "Graph.hpp"
#include "Node.hpp"
#include "Utils.hpp"

#include <cmath>
#include <chrono>
#include <thread>
#include <unistd.h>

#define TIME 500
#define CYCLE 100000
#define N_INPUT 10000

int main(int argc, char *argv[]) {
    auto* a = new Node<float>(1, 1, 3, IN);
    auto* b = new Node<float>(2, 1, 1, MIDDLE);
    auto* c = new Node<float>(3, 1, 2, MIDDLE);
    auto* d = new Node<float>(4, 2, 1, MIDDLE);
    auto* e = new Node<float>(5, 3, 1, OUT);

//    auto* a = new Node<float>(1, 1, 1, IN);
//    auto* b = new Node<float>(2, 1, 1, MIDDLE);
//    auto* c = new Node<float>(3, 1, 1, MIDDLE);
//    auto* d = new Node<float>(4, 1, 1, MIDDLE);
//    auto* e = new Node<float>(5, 1, 1, OUT);

    Graph<float> g;

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
        float in;
        float id = a->binds_inputs({&in});
        float out1 = sin(sin(sin(in)));
        float out2 = cos(cos(sin(in)));
        float out3 = sin(tan(sin(in))) + 3;

        for(int i = 0; i< CYCLE; i++){
            out1 = sin(sin(sin(out1)));
            out2 = cos(cos(sin(out2)));
            out3 = sin(tan(sin(out3))) + 3;
        }

//        std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
        a->send_out(out1, 0, id);
        a->send_out(out2, 1, id);
        a->send_out(out3, 2, id);
//        float in;
//        float id = a->binds_inputs({&in});
//        float out1 = in + 1;
//        a->send_out(out1, 0, id);
    });

    b->addCompute([&b]() {
        float in;
        float id = b->binds_inputs({&in});
        float out1 = cos(sin(sin(log(in))));

        for(int i= 0; i < CYCLE; i++){
            out1 = cos(sin(sin(log(out1))));
        }

//        std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
        b->send_out(out1, 0, id);
    });

    d->addCompute([&d]() {
        float in1, in2;
//        cout << "bind "<<endl;
//        START(start);
        float id = d->binds_inputs({&in1, &in2});
//        STOP(start, stop);
//        cout << "elapsed: "<<stop<<endl;

//        cout <<"comput time "<<endl;
//        START(start2);
        float temp1 = exp(exp(sin(in1)));
        float temp2 = sin(log(in2));
        float out1 = temp1 + temp2;

        for(int i = 0; i< CYCLE; i++){
            out1 = exp(exp(sin(out1)) + sin(log(temp2)));
        }
//        std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
//        STOP(start2, stop2);
//        cout << "elapsed: "<<stop2<<endl;

//        cout <<"sendout time "<<endl;
//        START(start1);
        d->send_out(out1, 0, id);
//        STOP(start1, stop1);
//        cout << "elapsed: "<<stop1<<endl;
//        float in1;
//        float id = d->binds_inputs({&in1});
//        float out1 = in1 +1;
//        d->send_out(out1, 0, id);
    });

    e->addCompute([&e]() {
        float in1;
        float in2;
        float in3;
        float id = e->binds_inputs({&in1, &in2, &in3});

        float temp1 = sin(sin(sin(in1)));
        float temp2 = exp(cos(in2));
        float temp3 = tan(sin(in3));
        float out1;

        for(int i= 0; i<CYCLE; i++){
            temp1 = sin(sin(sin(temp1)));
            temp2 = exp(cos(temp2));
            temp3 = tan(sin(temp3));
        }
        out1 = temp1 * temp2 + temp3;
//        std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
        e->send_out(out1, 0, id);
//        float in1;
//        float id = e->binds_inputs({&in1});
//        float out1 = in1 * 2;
//        e->send_out(out1, 0, id);
    });

    c->addCompute([&c]() {
        float in1;
        float id = c->binds_inputs({&in1});
        float out1 = cos(exp(log10(in1)));
        float out2 = cos(sin(pow(in1, 3)));

        for (int i = 0; i < CYCLE; i++){
            out1 = cos(exp(log10(out1)));
            out2 = cos(sin(pow(out2, 3)));
        }
//        std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
        c->send_out(out1, 0, id);
        c->send_out(out2, 1, id);
//        float in1;
//        float id = c->binds_inputs({&in1});
//        float out1 = in1 * 3;
//        c->send_out(out1, 0, id);
    });

    g.addNode(a);
    g.addNode(b);
    g.addNode(c);
    g.addNode(d);
    g.addNode(e);

    vector<float> inputs;
    for (float i = 1; i < N_INPUT; i++) {
//        cout<<"pushing values"<<endl;
        inputs.push_back(i);
    }

    cout << "\nSTARTING SEQUENTIAL COMP"<<endl;
    START(seq);
    g.compute_sequential(inputs);
//    cout << "total task = "<<g.getCount()<<endl;
    STOP(seq, seq_elapsed);
    cout <<"total elapsed time:" <<seq_elapsed<<endl;


    cout << "STARTING PARALLEL COMP 1 THREAD THREAD-POOL"<<endl;
    START(start);
    g.compute(inputs);
    g.terminate();
    STOP(start, par_elapsed);
    cout <<"total elapsed time:" <<par_elapsed<<endl;
    printf("speedup = %f\n",double(seq_elapsed)/double(par_elapsed));

    cout << "STARTING PARALLEL COMP 2 THREADS THREAD-POOL"<<endl;
    Graph<float> g2(2);
    g2.addNode(a);
    g2.addNode(b);
    g2.addNode(c);
    g2.addNode(d);
    g2.addNode(e);
    START(startg2);
    g2.compute(inputs);
    g2.terminate();
    STOP(startg2, g2_elapsed);
    cout <<"total elapsed time:" <<g2_elapsed<<endl;

    printf("speedup = %f\n",double(seq_elapsed)/double(g2_elapsed));

    cout << "STARTING PARALLEL COMP 4 THREADS THREAD-POOL"<<endl;
    Graph<float> g4(4);
    g4.addNode(a);
    g4.addNode(b);
    g4.addNode(c);
    g4.addNode(d);
    g4.addNode(e);
    START(startg4);
    g4.compute(inputs);
    g4.terminate();
    STOP(startg4, g4_elapsed);
    cout <<"total elapsed time:" <<g4_elapsed<<endl;

    printf("speedup = %f\n",double(seq_elapsed)/double(g4_elapsed));

    cout << "STARTING PARALLEL COMP 8 THREADS THREAD-POOL"<<endl;
    Graph<float> g8(8);
    g8.addNode(a);
    g8.addNode(b);
    g8.addNode(c);
    g8.addNode(d);
    g8.addNode(e);
    START(startg8);
    g8.compute(inputs);
    g8.terminate();
    STOP(startg8, g8_elapsed);
    cout <<"total elapsed time:" <<g8_elapsed<<endl;

    printf("speedup = %f\n",double(seq_elapsed)/double(g8_elapsed));

    return 0;
}

//float main(float argc, char *argv[]) {
//    vector<float> input;
//    for(float i= 0; i< 10; i++){
//        input.push_back(i);
//    }
//    input.push_back(NULL);
//
//    for(float i=0; i < input.size(); i++){
//        if(input[i] == NULL) cout <<"null"<<endl;
//        cout << input[i]<<endl;
//    }
//}