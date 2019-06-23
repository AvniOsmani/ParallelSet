// compile with
// c++ -o main main.cpp timer.cpp -std=c++14 -fno-elide-constructors -fopenmp
// run with
// ./main
// compile and run with
// c++ -o main main.cpp timer.cpp -std=c++14 -fno-elide-constructors -fopenmp && ./main

#include "testParallelSetClass2.h"
#include "timer.h"
#include <iostream>
#include <set>
#include <vector>

typedef double dataType;

long long int numOfTestSubjects = 1000000;
dataType xMin = -1000.0, xMax = 1000.0;

dataType frand();

void addRandoms(std::vector<dataType> &_testData);

void SingleInsertion(std::set<dataType> &_singularSet, const std::vector<dataType> &_testData);

void ParallelInsertion(set_parallel<dataType> &_singularSet, const std::vector<dataType> &_testData);

void SingleLookup(const std::set<dataType> &_singularSet, const std::vector<dataType> &_checkData);

void ParallelLookup(const set_parallel<dataType> &_pSet, const std::vector<dataType> &_checkData, std::vector<int> &_results);

void SingleDeletion(std::set<dataType> &_singularSet, const std::vector<dataType> &_deleteData);

void ParallelDeletion(set_parallel<dataType> &_pSet, const std::vector<dataType> &_deleteData);

int main(){

  std::cout<<std::endl;

  std::vector<dataType> testData;
  srand(time(NULL));
  addRandoms(testData);

  std::set<dataType> singularSet;
  std::cout<<"Inserting into c++ standard set"<<std::endl;
  Timer tm;
  tm.start();
  SingleInsertion(singularSet, testData);
  tm.end();
  std::cout<<"Standard insertion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  set_parallel<dataType> pSet(-1000.0, 1000.0);
  std::cout<<"Inserting into parallel set"<<std::endl;
  tm.start();
  ParallelInsertion(pSet, testData);
  tm.end();
  std::cout<<"Parallel insertion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::vector<dataType> checkData;
  addRandoms(checkData);

  std::cout<<"Lookup started in c++ standard set"<<std::endl;
  tm.start();
  SingleLookup(singularSet, checkData);
  tm.end();
  std::cout<<"Standard lookup finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::vector<int> results;
  results.resize(checkData.size());
  for(int i=0; i<results.size(); ++i){
    results[i] = 0;
  }

  std::cout<<"Lookup started in parallel set"<<std::endl;
  tm.start();
  ParallelLookup(pSet, checkData, results);
  tm.end();
  std::cout<<"Parallel lookup finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::vector<dataType> deleteData;
  addRandoms(deleteData);
  for(int i=0; i<numOfTestSubjects; ++i){
    if(i%4 == 0){
      deleteData.push_back(testData[i]);
    }
  }

  std::cout<<"Deleting from c++ standard set"<<std::endl;
  tm.start();
  SingleDeletion(singularSet, deleteData);
  tm.end();
  std::cout<<"Standard deletion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::cout<<"Deleting from parallel set"<<std::endl;
  tm.start();
  ParallelDeletion(pSet, deleteData);
  tm.end();
  std::cout<<"Parallel deletion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::cout<<"****************Testing Speed of Insertion with tree rebalancing****************" <<std::endl <<std::endl;
  std::cout<<"****************ROUND 1****************" <<std::endl <<std::endl;

  std::vector<dataType> moreData;
  addRandoms(moreData);

  std::cout<<"Inserting into c++ standard set"<<std::endl;
  tm.start();
  SingleInsertion(singularSet, moreData);
  tm.end();
  std::cout<<"Standard insertion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::cout<<"Inserting into parallel set"<<std::endl;
  tm.start();
  ParallelInsertion(pSet, moreData);
  tm.end();
  std::cout<<"Parallel insertion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::vector<dataType> checkMoreData;
  addRandoms(checkMoreData);

  std::cout<<"Lookup started in c++ standard set"<<std::endl;
  tm.start();
  SingleLookup(singularSet, checkMoreData);
  tm.end();
  std::cout<<"Standard lookup finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  results.resize(checkMoreData.size());
  for(int i=0; i<results.size(); ++i){
    results[i] = 0;
  }

  std::cout<<"Lookup started in parallel set"<<std::endl;
  tm.start();
  ParallelLookup(pSet, checkMoreData, results);
  tm.end();
  std::cout<<"Parallel lookup finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::vector<dataType> deleteMoreData;
  addRandoms(deleteMoreData);
  for(int i=0; i<numOfTestSubjects; ++i){
    if(i%4 == 0){
      deleteMoreData.push_back(moreData[i]);
    }
  }

  std::cout<<"Deleting from c++ standard set"<<std::endl;
  tm.start();
  SingleDeletion(singularSet, deleteMoreData);
  tm.end();
  std::cout<<"Standard deletion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::cout<<"Deleting from parallel set"<<std::endl;
  tm.start();
  ParallelDeletion(pSet, deleteMoreData);
  tm.end();
  std::cout<<"Parallel deletion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::cout<<"****************ROUND 2****************" <<std::endl <<std::endl;

  std::vector<dataType> moreData2;
  addRandoms(moreData2);

  std::cout<<"Inserting into c++ standard set"<<std::endl;
  tm.start();
  SingleInsertion(singularSet, moreData2);
  tm.end();
  std::cout<<"Standard insertion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::cout<<"Inserting into parallel set"<<std::endl;
  tm.start();
  ParallelInsertion(pSet, moreData2);
  tm.end();
  std::cout<<"Parallel insertion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::vector<dataType> checkMoreData2;
  addRandoms(checkMoreData2);

  std::cout<<"Lookup started in c++ standard set"<<std::endl;
  tm.start();
  SingleLookup(singularSet, checkMoreData2);
  tm.end();
  std::cout<<"Standard lookup finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  results.resize(checkMoreData2.size());
  for(int i=0; i<results.size(); ++i){
    results[i] = 0;
  }

  std::cout<<"Lookup started in parallel set"<<std::endl;
  tm.start();
  ParallelLookup(pSet, checkMoreData2, results);
  tm.end();
  std::cout<<"Parallel lookup finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::vector<dataType> deleteMoreData2;
  addRandoms(deleteMoreData2);
  for(int i=0; i<numOfTestSubjects; ++i){
    if(i%4 == 0){
      deleteMoreData2.push_back(moreData2[i]);
    }
  }

  std::cout<<"Deleting from c++ standard set"<<std::endl;
  tm.start();
  SingleDeletion(singularSet, deleteMoreData2);
  tm.end();
  std::cout<<"Standard deletion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::cout<<"Deleting from parallel set"<<std::endl;
  tm.start();
  ParallelDeletion(pSet, deleteMoreData2);
  tm.end();
  std::cout<<"Parallel deletion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::cout<<"****************ROUND 3****************" <<std::endl <<std::endl;

  std::vector<dataType> moreData3;
  addRandoms(moreData3);

  std::cout<<"Inserting into c++ standard set"<<std::endl;
  tm.start();
  SingleInsertion(singularSet, moreData3);
  tm.end();
  std::cout<<"Standard insertion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::cout<<"Inserting into parallel set"<<std::endl;
  tm.start();
  ParallelInsertion(pSet, moreData3);
  tm.end();
  std::cout<<"Parallel insertion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::vector<dataType> checkMoreData3;
  addRandoms(checkMoreData3);

  std::cout<<"Lookup started in c++ standard set"<<std::endl;
  tm.start();
  SingleLookup(singularSet, checkMoreData3);
  tm.end();
  std::cout<<"Standard lookup finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  results.resize(checkMoreData3.size());
  for(int i=0; i<results.size(); ++i){
    results[i] = 0;
  }

  std::cout<<"Lookup started in parallel set"<<std::endl;
  tm.start();
  ParallelLookup(pSet, checkMoreData3, results);
  tm.end();
  std::cout<<"Parallel lookup finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::vector<dataType> deleteMoreData3;
  addRandoms(deleteMoreData3);
  for(int i=0; i<numOfTestSubjects; ++i){
    if(i%4 == 0){
      deleteMoreData3.push_back(moreData3[i]);
    }
  }

  std::cout<<"Deleting from c++ standard set"<<std::endl;
  tm.start();
  SingleDeletion(singularSet, deleteMoreData3);
  tm.end();
  std::cout<<"Standard deletion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  std::cout<<"Deleting from parallel set"<<std::endl;
  tm.start();
  ParallelDeletion(pSet, deleteMoreData3);
  tm.end();
  std::cout<<"Parallel deletion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  return 0;
}

dataType frand(){
  dataType f = (double)rand() / RAND_MAX;
  return xMin + f * (xMax - xMin);
}

void addRandoms(std::vector<dataType> &_testData){
  for(int i=0; i<numOfTestSubjects; ++i){
    _testData.push_back(frand());
    //std::cout<<_testData[i] <<std::endl;
  }
}

void SingleInsertion(std::set<dataType> &_singularSet, const std::vector<dataType> &_testData){
  for(int i=0; i<_testData.size(); ++i){
    _singularSet.insert(_testData[i]);
  }
  dataType sum = 0;
  typename std::set<dataType>::iterator iter;
  for(iter=_singularSet.begin(); iter!=_singularSet.end(); ++iter){
    sum += (*iter);
  }
  std::cout<<"The sum of all the elements in the set is " <<sum <<std::endl;
}

void ParallelInsertion(set_parallel<dataType> &_pSet, const std::vector<dataType> &_testData){
  _pSet.insert_parallel(_testData);
  _pSet.find_sum();
}

void SingleLookup(const std::set<dataType> &_singularSet, const std::vector<dataType> &_checkData){
  std::set<dataType>::iterator it;
  for(int i=0; i<_checkData.size(); ++i){
    it = _singularSet.begin();
    it = _singularSet.find(_checkData[i]);
    if(it == _singularSet.end()){}
  }
  dataType sum = 0;
  typename std::set<dataType>::iterator iter;
  for(iter=_singularSet.begin(); iter!=_singularSet.end(); ++iter){
    sum += (*iter);
  }
  std::cout<<"The sum of all the elements in the set is " <<sum <<std::endl;
}

void ParallelLookup(const set_parallel<dataType> &_pSet, const std::vector<dataType> &_checkData, std::vector<int> &_results){
  _pSet.find_parallel(_checkData, _results);
  for(int i=0; i<_results.size(); ++i){
    if(_results[i] == 1){}
  }
  _pSet.find_sum();
}

void SingleDeletion(std::set<dataType> &_singularSet, const std::vector<dataType> &_deleteData){
  for(int i=0; i<_deleteData.size(); ++i){
    _singularSet.erase(_deleteData[i]);
  }
  dataType sum = 0;
  typename std::set<dataType>::iterator iter;
  for(iter=_singularSet.begin(); iter!=_singularSet.end(); ++iter){
    sum += (*iter);
  }
  std::cout<<"The sum of all the elements in the set is " <<sum <<std::endl;
}

void ParallelDeletion(set_parallel<dataType> &_pSet, const std::vector<dataType> &_deleteData){
  _pSet.erase_parallel(_deleteData);
  _pSet.find_sum();
}
