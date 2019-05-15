// compile with
// c++ -o main main.cpp -std=c++14 -fno-elide-constructors -fopenmp
// run with
// ./main
// compile and run with
// c++ -o main main.cpp -std=c++14 -fno-elide-constructors -fopenmp && ./main

#include "testParallelSetClass.h"
#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <string>
#include <omp.h>

int main(){
  std::vector<double> rep;
  rep.resize(3);
  rep[0]=-5;
  rep[1]=0;
  rep[2]=5;
  set_parallel<double> testRebalance(rep);
  std::vector<double> data;
  data.resize(12);
  data[0]=-9;
  data[1]=-8;
  data[2]=-7;
  data[3]=-4;
  data[4]=-3;
  data[5]=-2;
  data[6]=1;
  data[7]=2;
  data[8]=3;
  data[9]=7;
  data[10]=8;
  data[11]=9;

  testRebalance.insert_parallel(data);
  testRebalance.print_all_data();

  std::cout<<std::endl <<std::endl;
  std::cout<<"rightEndPoints are " <<testRebalance.get_right_end_point(0) <<" " <<testRebalance.get_right_end_point(1) <<" " <<testRebalance.get_right_end_point(2) <<" " <<std::endl;

  std::cout<<std::endl <<std::endl;

  std::vector<double> data2;
  data2.resize(6);
  data2[0]=-3.2;
  data2[1]=-3.5;
  data2[2]=-2.4;
  data2[3]=-3.7;
  data2[4]=-2.2;
  data2[5]=-3.54;

  testRebalance.insert_parallel(data2);
  testRebalance.print_all_data();

  std::cout<<std::endl <<std::endl;
  std::cout<<"rightEndPoints are " <<testRebalance.get_right_end_point(0) <<" " <<testRebalance.get_right_end_point(1) <<" " <<testRebalance.get_right_end_point(2) <<" " <<std::endl;

  return 0;
}
