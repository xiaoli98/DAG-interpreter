//
// Created by boy on 05/07/22.
//

#ifndef DAG_LIB_THREADPOOL_HPP
#define DAG_LIB_THREADPOOL_HPP

#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <cstdio>
#include <vector>

#include "ConcurrentQueue.hpp"

using namespace std;

class Thread_pool {
private:
    int n_worker;
    vector<thread> tids;
    mutex pool_mtx;
    condition_variable empty_cv;
    atomic<bool> terminate;
    ConcurrentQueue task_queue;

//    atomic<int> n_done;
    int n_done;
    int n_added;
    mutex mtx;

public:
    Thread_pool(int nw) : n_worker(nw), terminate(false) {
        n_done = n_added = 0;
    };

    void addTask(function<void()> f) {
        //add a task to be executed
        unique_lock<mutex> lck(pool_mtx);
//        cout <<"task received"<<endl;
        task_queue.push(f);
        n_added++;
        empty_cv.notify_one();
    };

    void run() {
        function<void()> task;
        bool dequeued;

        while (!terminate || !task_queue.empty()) {
            {
                unique_lock<mutex> lock(pool_mtx);
//                cout << this_thread::get_id()<<" running "<<endl;
                if (task_queue.empty() && !terminate) {
                    empty_cv.wait(lock);
                }
                dequeued = task_queue.pop(task);
            }
            if (dequeued) {
                task();
                {
                    unique_lock<mutex> lck(mtx);
                    n_done++;
                }
            }
        }
    }

    void start() {
        //starts all threads in the thread pool
        for (int i = 0; i < n_worker; i++)
            tids.emplace_back(&Thread_pool::run, this);
    };

    void shutdown() {
        //shutdown the thread pool
        //all remaining not started task are not executed
        terminate = true;
        empty_cv.notify_all();
        for (auto & tid : tids)
            tid.join();
    };

    int getN_done(){
        return n_done;
    }

    int getN_added(){
        return n_added;
    }

    int getTot(){
        return n_done + task_queue.size();
    }
};

#endif //DAG_LIB_THREADPOOL_HPP
