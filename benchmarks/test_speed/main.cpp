// compile with
// c++ -o main main.cpp timer.cpp -std=c++14 -fno-elide-constructors -fopenmp
// run with
// ./main
// compile and run with
// c++ -o main main.cpp timer.cpp -std=c++14 -fno-elide-constructors -fopenmp && ./main

#include "testParallelSetClass.h"
#include "timer.h"
#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <string>
#include <omp.h>

typedef double dataType;

long long int numOfTestSubjects = 1000000;
dataType x1 = -1000.0, x2 = 1000.0, x;

int main(){

  std::cout<<std::endl;
  //generate random doubles for insertion
  std::vector<dataType> testData;

  const long max_rand = 1000000L;

  srandom(time(NULL));
  for(int i=0; i<numOfTestSubjects; ++i){
    x = x1 + ( x2 - x1) * (random() % max_rand) / max_rand;
    testData.push_back(x);
  }

  Timer tm;

  //testing insertion speed in c++ standard set
  std::set<dataType> singularSet;
  std::cout<<"Inserting into c++ standard set"<<std::endl;
  tm.start();
  for(int i=0; i<testData.size(); ++i){
    singularSet.insert(testData[i]);
  }
  tm.end();
  std::cout<<"Standard insertion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  //testing insertion speed in parallel set
  set_parallel<dataType> pSet(-1000.0, 1000.0);
  std::cout<<"Inserting into parallel set"<<std::endl;
  tm.start();
  pSet.insert_parallel(testData);
  tm.end();
  std::cout<<"Parallel insertion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  //use data from testData and generate random doubles to lookup
  std::vector<dataType> checkData;
  for(int i=0; i<numOfTestSubjects; ++i){
    x = x1 + ( x2 - x1) * (random() % max_rand) / max_rand;
    checkData.push_back(x);
    if(i%4 == 0){
      checkData.push_back(testData[i]);
    }
  }

  //testing lookup speed in c++ standard set
  std::set<dataType>::iterator it;
  std::cout<<"Lookup started in c++ standard set"<<std::endl;
  tm.start();
  for(int i=0; i<checkData.size(); ++i){
    it = singularSet.begin();
    it = singularSet.find(checkData[i]);
    if(it == singularSet.end()){}
  }
  tm.end();
  std::cout<<"Standard lookup finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  //testing lookup speed in parallel set
  std::vector<int> results;
  results.resize(checkData.size());
  for(int i=0; i<results.size(); ++i){
    results[i] = 0;
  }
  std::cout<<"Lookup started in parallel set"<<std::endl;
  tm.start();
  pSet.find_parallel(checkData, results);
  for(int i=0; i<results.size(); ++i){
    if(results[i] == 1){}
  }
  tm.end();
  std::cout<<"Parallel lookup finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  //use data from testData and generate random doulbes for deletion
  std::vector<dataType> deleteData;
  srandom(time(NULL));
  for(int i=0; i<numOfTestSubjects; ++i){
    x = x1 + ( x2 - x1) * (random() % max_rand) / max_rand;
    deleteData.push_back(x);
    if(i%4 == 0){
      deleteData.push_back(testData[i]);
    }
  }

  //testing deletion speed in c++ standard set
  /*std::cout<<"Deleting from c++ standard set"<<std::endl;
  tm.start();
  for(int i=0; i<deleteData.size(); ++i){
    singularSet.erase(deleteData[i]);
  }
  tm.end();
  std::cout<<"Standard deletion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  //testing deletion speed in parallel set
  std::cout<<"Deleting from parallel set"<<std::endl;
  tm.start();
  pSet.erase_parallel(deleteData);
  tm.end();
  std::cout<<"Parallel deletion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;*/

  std::cout<<"****************Testing Speed of Insertion with tree rebalancing****************" <<std::endl <<std::endl;
  std::cout<<"****************ROUND 1****************" <<std::endl <<std::endl;

  std::vector<dataType> moreData;

  for(int i=0; i<750000; ++i){
    x = x1 + ( x2 - x1) * (random() % max_rand) / max_rand;
    moreData.push_back(x);
  }

  //testing insertion speed in c++ standard set
  std::cout<<"Inserting into c++ standard set"<<std::endl;
  tm.start();
  for(int i=0; i<moreData.size(); ++i){
    singularSet.insert(moreData[i]);
  }
  tm.end();
  std::cout<<"Standard insertion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  //testing insertion speed in parallel set
  std::cout<<"Inserting into parallel set"<<std::endl;
  tm.start();
  pSet.insert_parallel(moreData);
  tm.end();
  std::cout<<"Parallel insertion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::cout<<"Lookup started in c++ standard set"<<std::endl;
  tm.start();
  for(int i=0; i<checkData.size(); ++i){
    it = singularSet.begin();
    it = singularSet.find(checkData[i]);
    if(it == singularSet.end()){}
  }
  tm.end();
  std::cout<<"Standard lookup finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::cout<<"Lookup started in parallel set"<<std::endl;
  tm.start();
  pSet.find_parallel(checkData, results);
  for(int i=0; i<results.size(); ++i){
    if(results[i] == 1){}
  }
  tm.end();
  std::cout<<"Parallel lookup finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  //deleting
  srandom(time(NULL));
  for(int i=0; i<(numOfTestSubjects/4); ++i){
    x = x1 + ( x2 - x1) * (random() % max_rand) / max_rand;
    deleteData.push_back(x);
    if(i%4 == 0){
      deleteData.push_back(moreData[i]);
    }
  }

  //testing deletion speed in c++ standard set
  /*std::cout<<"Deleting from c++ standard set"<<std::endl;
  tm.start();
  for(int i=0; i<deleteData.size(); ++i){
    singularSet.erase(deleteData[i]);
  }
  tm.end();
  std::cout<<"Standard deletion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  //testing deletion speed in parallel set
  std::cout<<"Deleting from parallel set"<<std::endl;
  tm.start();
  pSet.erase_parallel(deleteData);
  tm.end();
  std::cout<<"Parallel deletion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;*/

  std::cout<<"****************ROUND 2****************" <<std::endl <<std::endl;

  std::vector<dataType> moreData2;

  for(int i=0; i<500000; ++i){
    x = x1 + ( x2 - x1) * (random() % max_rand) / max_rand;
    moreData2.push_back(x);
  }

  //testing insertion speed in c++ standard set
  std::cout<<"Inserting into c++ standard set"<<std::endl;
  tm.start();
  for(int i=0; i<moreData2.size(); ++i){
    singularSet.insert(moreData2[i]);
  }
  tm.end();
  std::cout<<"Standard insertion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  //testing insertion speed in parallel set
  std::cout<<"Inserting into parallel set"<<std::endl;
  tm.start();
  pSet.insert_parallel(moreData2);
  tm.end();
  std::cout<<"Parallel insertion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::cout<<"Lookup started in c++ standard set"<<std::endl;
  tm.start();
  for(int i=0; i<checkData.size(); ++i){
    it = singularSet.begin();
    it = singularSet.find(checkData[i]);
    if(it == singularSet.end()){}
  }
  tm.end();
  std::cout<<"Standard lookup finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::cout<<"Lookup started in parallel set"<<std::endl;
  tm.start();
  pSet.find_parallel(checkData, results);
  for(int i=0; i<results.size(); ++i){
    if(results[i] == 1){}
  }
  tm.end();
  std::cout<<"Parallel lookup finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  //deleting
  srandom(time(NULL));
  for(int i=0; i<(numOfTestSubjects/4); ++i){
    x = x1 + ( x2 - x1) * (random() % max_rand) / max_rand;
    deleteData.push_back(x);
    if(i%4 == 0){
      deleteData.push_back(moreData2[i]);
    }
  }

  //testing deletion speed in c++ standard set
  /*std::cout<<"Deleting from c++ standard set"<<std::endl;
  tm.start();
  for(int i=0; i<deleteData.size(); ++i){
    singularSet.erase(deleteData[i]);
  }
  tm.end();
  std::cout<<"Standard deletion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  //testing deletion speed in parallel set
  std::cout<<"Deleting from parallel set"<<std::endl;
  tm.start();
  pSet.erase_parallel(deleteData);
  tm.end();
  std::cout<<"Parallel deletion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;*/

  std::cout<<"****************ROUND 3****************" <<std::endl <<std::endl;

  std::vector<dataType> moreData3;

  for(int i=0; i<250000; ++i){
    x = x1 + ( x2 - x1) * (random() % max_rand) / max_rand;
    moreData3.push_back(x);
  }

  //testing insertion speed in c++ standard set
  std::cout<<"Inserting into c++ standard set"<<std::endl;
  tm.start();
  for(int i=0; i<moreData3.size(); ++i){
    singularSet.insert(moreData3[i]);
  }
  tm.end();
  std::cout<<"Standard insertion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  //testing insertion speed in parallel set
  std::cout<<"Inserting into parallel set"<<std::endl;
  tm.start();
  pSet.insert_parallel(moreData3);
  tm.end();
  std::cout<<"Parallel insertion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::cout<<"Lookup started in c++ standard set"<<std::endl;
  tm.start();
  for(int i=0; i<checkData.size(); ++i){
    it = singularSet.begin();
    it = singularSet.find(checkData[i]);
    if(it == singularSet.end()){}
  }
  tm.end();
  std::cout<<"Standard lookup finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::cout<<"Lookup started in parallel set"<<std::endl;
  tm.start();
  pSet.find_parallel(checkData, results);
  for(int i=0; i<results.size(); ++i){
    if(results[i] == 1){}
  }
  tm.end();
  std::cout<<"Parallel lookup finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  //deleting
  srandom(time(NULL));
  for(int i=0; i<(numOfTestSubjects/4); ++i){
    x = x1 + ( x2 - x1) * (random() % max_rand) / max_rand;
    deleteData.push_back(x);
    if(i%4 == 0){
      deleteData.push_back(moreData3[i]);
    }
  }

  //testing deletion speed in c++ standard set
  std::cout<<"Deleting from c++ standard set"<<std::endl;
  tm.start();
  for(int i=0; i<deleteData.size(); ++i){
    singularSet.erase(deleteData[i]);
  }
  tm.end();
  std::cout<<"Standard deletion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  //testing deletion speed in parallel set
  std::cout<<"Deleting from parallel set"<<std::endl;
  tm.start();
  pSet.erase_parallel(deleteData);
  tm.end();
  std::cout<<"Parallel deletion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  return 0;
}
