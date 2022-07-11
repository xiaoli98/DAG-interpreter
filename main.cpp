#include "Graph.hpp"
#include "Node.hpp"
#include "Utils.hpp"

#include <cmath>
#include <chrono>
#include <thread>
#include <unistd.h>

#define TIME 500
#define CYCLE 10000
#define N_INPUT 10000
#define N_GRAPHS 8

#define GRAPH 2

#if (GRAPH == 1)
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


    cout << "STARTING PARALLEL COMP 16 THREADS THREAD-POOL"<<endl;
    Graph<float> g16(16);
    g16.addNode(a);
    g16.addNode(b);
    g16.addNode(c);
    g16.addNode(d);
    g16.addNode(e);
    START(startg16);
    g16.compute(inputs);
    g16.terminate();
    STOP(startg16, g16_elapsed);
    cout <<"total elapsed time:" <<g16_elapsed<<endl;

    printf("speedup = %f\n",double(seq_elapsed)/double(g16_elapsed));

    cout << "STARTING PARALLEL COMP 32 THREADS THREAD-POOL"<<endl;
    Graph<float> g32(32);
    g32.addNode(a);
    g32.addNode(b);
    g32.addNode(c);
    g32.addNode(d);
    g32.addNode(e);
    START(startg32);
    g32.compute(inputs);
    g32.terminate();
    STOP(startg32, g32_elapsed);
    cout <<"total elapsed time:" <<g32_elapsed<<endl;

    printf("speedup = %f\n",double(seq_elapsed)/double(g32_elapsed));

    cout << "STARTING PARALLEL COMP 64 THREADS THREAD-POOL"<<endl;
    Graph<float> g64(64);
    g64.addNode(a);
    g64.addNode(b);
    g64.addNode(c);
    g64.addNode(d);
    g64.addNode(e);
    START(startg64);
    g64.compute(inputs);
    g64.terminate();
    STOP(startg64, g64_elapsed);
    cout <<"total elapsed time:" <<g64_elapsed<<endl;

    printf("speedup = %f\n",double(seq_elapsed)/double(g64_elapsed));

    cout << "STARTING PARALLEL COMP 112 THREADS THREAD-POOL"<<endl;
    Graph<float> g112(112);
    g112.addNode(a);
    g112.addNode(b);
    g112.addNode(c);
    g112.addNode(d);
    g112.addNode(e);
    START(startg112);
    g112.compute(inputs);
    g112.terminate();
    STOP(startg112, g112_elapsed);
    cout <<"total elapsed time:" <<g112_elapsed<<endl;

    printf("speedup = %f\n",double(seq_elapsed)/double(g112_elapsed));
    return 0;
}
#endif

#if (GRAPH == 2)
int main(int argc, char *argv[]){
    auto* a = new Node<float>(1, 1, 1, IN);
    auto* b = new Node<float>(2, 1, 1, MIDDLE);
    auto* c = new Node<float>(3, 1, 1, MIDDLE);
    auto* d = new Node<float>(4, 1, 1, MIDDLE);
    auto* e = new Node<float>(5, 1, 1, MIDDLE);
    auto* f = new Node<float>(6, 1, 1, MIDDLE);
    auto* g = new Node<float>(7, 1, 1, MIDDLE);
    auto* h = new Node<float>(8, 1, 1, MIDDLE);
    auto* i = new Node<float>(9, 1, 1, MIDDLE);
    auto* l = new Node<float>(10, 1, 1, MIDDLE);
    auto* m = new Node<float>(10, 1, 1, MIDDLE);
    auto* n = new Node<float>(10, 1, 1, MIDDLE);
    auto* o = new Node<float>(10, 1, 1, OUT);

    a->addDep(b);
    b->addDep(c);
    c->addDep(d);
    d->addDep(e);
    e->addDep(f);
    f->addDep(g);
    g->addDep(h);
    h->addDep(i);
    i->addDep(l);
    l->addDep(m);
    m->addDep(n);
    n->addDep(o);

    a->addCompute([&a]() {
        float in;
        int id = a->binds_inputs({&in});
        float out1 = in;
        for(int i = 0; i< CYCLE; i++){
            out1 = sin(sin(sin(out1)));
        }
        a->send_out(out1, 0, id);
    });
    b->addCompute([&b]() {
        float in;
        int id = b->binds_inputs({&in});
        float out1 = in;
        for(int i = 0; i< CYCLE; i++){
            out1 = sin(log(cos(out1)));
        }
        b->send_out(out1, 0, id);
    });
    c->addCompute([&c]() {
        float in;
        int id = c->binds_inputs({&in});
        float out1 = in;
        for(int i = 0; i< CYCLE; i++){
            out1 = pow(sin(sin(out1)), 6);
        }
        c->send_out(out1, 0, id);
    });
    d->addCompute([&d]() {
        float in;
        int id = d->binds_inputs({&in});
        float out1 = in;
        for(int i = 0; i< CYCLE; i++){
            out1 = sin(sin(sin(out1)));
        }
        d->send_out(out1, 0, id);
    });
    e->addCompute([&e]() {
        float in;
        int id = e->binds_inputs({&in});
        float out1 = in;
        for(int i = 0; i< CYCLE; i++){
            out1 = cos(cos(cos(out1)));
        }
        e->send_out(out1, 0, id);
    });
    f->addCompute([&f]() {
        float in;
        int id = f->binds_inputs({&in});
        float out1 = in;
        for(int i = 0; i< CYCLE; i++){
            out1 = cos(sin(sin(out1)));
        }
        f->send_out(out1, 0, id);
    });
    g->addCompute([&g]() {
        float in;
        int id = g->binds_inputs({&in});
        float out1 = in;
        for(int i = 0; i< CYCLE; i++){
            out1 = log(log(log(out1)));
        }
        g->send_out(out1, 0, id);
    });
    h->addCompute([&h]() {
        float in;
        int id = h->binds_inputs({&in});
        float out1 = in;
        for(int i = 0; i< CYCLE; i++){
            out1 = asin(acos(tanh(out1)));
        }
        h->send_out(out1, 0, id);
    });
    i->addCompute([&i]() {
        float in;
        int id = i->binds_inputs({&in});
        float out1 = in;
        for(int z = 0; z< CYCLE; z++){
            out1 = sin(sin(sin(out1)));
        }
        i->send_out(out1, 0, id);
    });
    l->addCompute([&l]() {
        float in;
        int id = l->binds_inputs({&in});
        float out1 = in;
        for(int i = 0; i< CYCLE; i++){
            out1 = log(cos(pow(out1, 4)));
        }
        l->send_out(out1, 0, id);
    });
    m->addCompute([&m]() {
        float in;
        int id = m->binds_inputs({&in});
        float out1 = in;
        for(int i = 0; i< CYCLE; i++){
            out1 = sin(sin(sin(out1)));
        }
        m->send_out(out1, 0, id);
    });
    n->addCompute([&n]() {
        float in;
        int id = n->binds_inputs({&in});
        float out1 = in;
        for(int i = 0; i< CYCLE; i++){
            out1 = tanh(sin(sin(out1)));
        }
        n->send_out(out1, 0, id);
    });
    o->addCompute([&o]() {
        float in;
        int id = o->binds_inputs({&in});
        float out1 = in;
        for(int i = 0; i< CYCLE; i++){
            out1 = sin(sin(sin(out1)));
        }
        o->send_out(out1, 0, id);
    });

    vector<float> inputs;
    for (int input = 1; input < N_INPUT; input++) {
        inputs.push_back(float(input));
    }

    vector<Graph<float>*> my_graphs;
    Graph<float> graph_seq;

    graph_seq.addNode(a);
    graph_seq.addNode(b);
    graph_seq.addNode(c);
    graph_seq.addNode(d);
    graph_seq.addNode(e);
    graph_seq.addNode(f);
    graph_seq.addNode(g);
    graph_seq.addNode(h);
    graph_seq.addNode(i);
    graph_seq.addNode(l);
    graph_seq.addNode(m);
    graph_seq.addNode(n);
    graph_seq.addNode(o);
    cout << "---------------SEQUENTIAL----------------------"<<endl;
    START(start_seq);
    graph_seq.compute_sequential(inputs);
//    cout << "total task = "<<g.getCount()<<endl;
    STOP(start_seq, seq_elapsed);
    cout <<"total elapsed time:" <<seq_elapsed<<endl;

    int numbers[6] = {1, 2, 4, 8, 16, 32};

    for (int number : numbers){
        my_graphs.push_back(new Graph<float>(number, false));
    }

    int z = 0;
    for (auto graph : my_graphs){
        graph->addNode(a);
        graph->addNode(b);
        graph->addNode(c);
        graph->addNode(d);
        graph->addNode(e);
        graph->addNode(f);
        graph->addNode(g);
        graph->addNode(h);
        graph->addNode(i);
        graph->addNode(l);
        graph->addNode(m);
        graph->addNode(n);
        graph->addNode(o);
        cout << "-------------------------------------"<<endl;
        cout << "N_Thread: "<< numbers[z++] <<endl;
        graph->startExecutor();
        START(start);
        graph->compute(inputs);
//    cout << "total task = "<<g.getCount()<<endl;
        graph->terminate();
        STOP(start, elapsed);
        cout <<"total elapsed time:" <<elapsed<<endl;
        printf("speedup = %f\n",double(seq_elapsed)/double(elapsed));
    }
}
#endif

#if (GRAPH == 3)
#endif

#if (GRAPH == 4)

#endif
