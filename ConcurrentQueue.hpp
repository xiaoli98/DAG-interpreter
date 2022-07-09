//
// Created by boy on 05/07/22.
//

#ifndef DAG_LIB_CONCURRENTQUEUE_HPP
#define DAG_LIB_CONCURRENTQUEUE_HPP

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>

using namespace std;

class ConcurrentQueue {
private:
    queue<function<void()>> m_queue;
    mutex m_mutex;
public:
    ConcurrentQueue() = default;

    bool empty() {
        unique_lock<mutex> lock(m_mutex);
        return m_queue.empty();
    }

    int size() {
        unique_lock<mutex> lock(m_mutex);
        return m_queue.size();
    }

    void push(function<void()> t) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(t);
    }

    bool pop(function<void()>& t) {
        std::unique_lock<std::mutex> lock(m_mutex);

        if (m_queue.empty()) {
            return false;
        }
        t = move(m_queue.front());

        m_queue.pop();
        return true;
    }
};


#endif //DAG_LIB_CONCURRENTQUEUE_HPP
