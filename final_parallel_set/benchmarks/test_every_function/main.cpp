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

typedef double dataType;

set_parallel<dataType> testmove(){
  set_parallel<dataType> t(1.2, 5.3);
  std::vector<dataType> sampleData;
  sampleData.resize(2);
  sampleData[0]=-422;
  sampleData[1]=9784;
  t.insert_parallel(sampleData);
  return t;
}

int main(){

  set_parallel<std::string> testDC;
  set_parallel<double> testMinMax(-1000.0, 1000.0);
  std::vector<double> data;
  data.resize(10);
  for(int i=0; i<10; ++i){
    data[i]=-1500+(i*250);
  }
  set_parallel<double> testSample(data);
  testSample.print_all_data();

  set_parallel<double> testCC(testSample);
  testCC.print_all_data();
  testMinMax=testCC;
  testMinMax.print_all_data();

  set_parallel<dataType> testMOVE(-5.0, 5.0);
  testMOVE=testmove();
  testMOVE.print_all_data();

  set_parallel<dataType> testFunctions(-100.0, 100.0);
  std::cout<<"is empty " <<testFunctions.empty_p() <<std::endl;
  std::vector<dataType> data2;
  data2.resize(10);
  data2[0]=432.54;
  data2[1]=-332.54;
  data2[2]=-132.54;
  data2[3]=32.54;
  data2[4]=42.54;
  data2[5]=-2.54;
  data2[6]=-97.54;
  data2[7]=67.54;
  data2[8]=-77.54;
  data2[9]=-13.54;
  testFunctions.insert_parallel(data2);
  testFunctions.print_all_data();

  std::vector<dataType> dataDelete;
  dataDelete.resize(6);
  dataDelete[0]=345;
  dataDelete[1]=-332.54;
  dataDelete[2]=-97.54;
  dataDelete[3]=67.54;
  dataDelete[4]=2734;
  dataDelete[5]=-2.54;
  testFunctions.erase_parallel(dataDelete);
  testFunctions.print_all_data();
  std::cout<<"size is " <<testFunctions.size_p() <<std::endl;
  std::cout<<"is empty " <<testFunctions.empty_p() <<std::endl;

  std::vector<dataType> lookup;
  lookup.resize(6);
  lookup[0]=42.54;
  lookup[1]=233;
  lookup[2]=-13.54;
  lookup[3]=-77.54;
  lookup[4]=67.54;
  lookup[5]=-2.54;
  std::vector<int> results;
  results.resize(6);
  testFunctions.find_parallel(lookup, results);
  for(int i=0; i<6; ++i){
    std::cout<<results[i] <<" ";
  }
  std::cout<<std::endl;

  std::cout<<testFunctions.find_element(432.54) <<std::endl;
  std::cout<<testFunctions.find_element(-77.54) <<std::endl;
  std::cout<<testFunctions.find_element(32.54) <<std::endl;
  testFunctions.insert_element(-200.3);
  testFunctions.insert_element(1.5);
  testFunctions.insert_element(500.5);
  testFunctions.print_all_data();
  std::cout<<std::endl;
  testFunctions.erase_element(-132.54);
  testFunctions.erase_element(32.54);
  testFunctions.erase_element(432.54);
  testFunctions.print_all_data();

  for(int i=0; i<3; ++i){
    std::cout<<testFunctions.get_right_end_point(i) <<" ";
  }
  std::cout<<std::endl;

  testFunctions.clear_p();
  std::cout<<"is empty " <<testFunctions.empty_p() <<std::endl;
  std::cout<<"size is " <<testFunctions.size_p() <<std::endl;

  return 0;
}
