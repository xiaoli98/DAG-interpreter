//
// Created by boy on 09/07/22.
//

#ifndef DAG_LIB_UTILS_HPP
#define DAG_LIB_UTILS_HPP

#define START(timename) auto timename = std::chrono::system_clock::now();
#define STOP(timename,elapsed)  auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - timename).count();


#endif //DAG_LIB_UTILS_HPP
