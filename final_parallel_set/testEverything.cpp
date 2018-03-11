// compile with
// c++ -o test testEverything.cpp -std=c++14 -fopenmp
// run with
// ./test
// compile and run with
// c++ -o test testEverything.cpp -std=c++14 -fno-elide-constructors -fopenmp && ./test

//each thread maintains one tree
//each thread goes through every element and decides whether
//it should be inserted or deleted into its tree

#include<set>
#include<vector>
#include<algorithm>
#include<string>
#include<iostream>
#include<omp.h>
#include"timer.cpp"
//#include <time.h>

typedef double dataType;

template <class T>
class SetParallel{
private:
    std::vector<std::set<T>> allData;
    std::vector<T> rightEndPoints;

    int numTrees;
    int numThreads;

    void rebalance_trees();

public:
    SetParallel();  //Default Constructor for strings
    SetParallel(const T &, const T &);  //Constructor given min and max
    SetParallel(std::vector<T> &);  //Constructor given data or right end points
    SetParallel(const SetParallel &);
    const SetParallel& operator=(const SetParallel &);
    SetParallel(SetParallel &&);
    SetParallel& operator=(SetParallel &&);
    void insert_parallel(const std::vector<T> &);
    void insert_element(const T &);
    void erase_parallel(const std::vector<T> &);
    void erase_element(const T &);
    void find_parallel(const std::vector<T> &, std::vector<int> &) const;
    int find_element(const T &) const;
    void print_all_data() const;
    int size_p() const;
    int empty_p() const;
    void clear_p();
    T get_right_end_point(const int &) const;
    virtual ~SetParallel();
};

template <class T>
void SetParallel<T>::rebalance_trees(){
  if(size_p() > numThreads){
    std::vector<T> original_data;
    original_data.resize(size_p());

    // copy all data to vector
    #pragma omp parallel num_threads(numThreads)
    {
      int myID=omp_get_thread_num();
      typename std::set<T>::iterator iter;
      int position = 0;
      for(int i=0; i<numThreads-1; ++i){
        if(myID > i){
          position += allData[i].size();
        }
      }
      for(iter=allData[myID].begin(); iter!=allData[myID].end(); ++iter){
        original_data[position] = (*iter);
        ++position;
      }
    }
    #pragma omp barrier

    // set new right endpoints
    if(original_data.size() == numThreads-1){
      for(int i=0; i<rightEndPoints.size(); ++i){
        rightEndPoints[i]=original_data[i];
      }
    }
    if(original_data.size() > numThreads-1){
      int intervalSize = original_data.size() / numThreads;
      if(original_data.size() % numThreads == 0){
        for(int i=0; i<rightEndPoints.size(); ++i){
          rightEndPoints[i]=original_data[(i+1)*intervalSize-1];
        }
      }
      else{
        for(int i=0; i<rightEndPoints.size(); ++i){
          rightEndPoints[i]=original_data[(i+1)*intervalSize];
        }
      }
    }

    /*// find and set new right end points
    int intervalSize = size_p() / numThreads;
    int rightEndPointsPos = 0;
    int allDataPos = 0;
    typename std::set<T>::iterator iter;
    for(int i=0; i<allData.size(); ++i){
      for(iter=allData[i].begin(); iter!=allData[i].end(); ++iter){
        if((allDataPos % intervalSize) == 0 && rightEndPointsPos < rightEndPoints.size()){
          rightEndPoints[rightEndPointsPos] = (*iter);
          ++rightEndPointsPos;
        }
        ++allDataPos;
      }
    }*/

    // move data to correct trees
    typename std::set<T>::iterator iter;
    for(iter=allData[0].begin(); iter!=allData[0].end(); ++iter){
      if((*iter) > rightEndPoints[0]){
        insert_element((*iter));
        allData[0].erase((*iter));
      }
    }

    for(int i=1; i<numThreads-1; ++i){
      for(iter=allData[i].begin(); iter!=allData[i].end(); ++iter){
        if((*iter) <= rightEndPoints[i-1] || (*iter) > rightEndPoints[i]){
          insert_element((*iter));
          allData[i].erase((*iter));
        }
      }
    }

    for(iter=allData[numThreads-1].begin(); iter!=allData[numThreads-1].end(); ++iter){
      if((*iter) <= rightEndPoints[numThreads-2]){
        insert_element((*iter));
        allData[numThreads-1].erase((*iter));
      }
    }
  }
}

template <class T>
SetParallel<T>::SetParallel(){
  //std::cout<<"DEFAULT CONSTRUCTOR" <<std::endl;
  #pragma omp parallel
  {
    if(omp_get_thread_num()==0){
      numThreads=omp_get_num_threads();
    }
  }
  numTrees=numThreads;

  rightEndPoints.resize(numTrees-1);
  allData.resize(numTrees);

  int intervalSize = 57/numThreads;
  for(int i=0; i<rightEndPoints.size(); ++i){
    char s = 57+((i+1)*intervalSize);
    std::string breakPoint(1, s);
    rightEndPoints[i]=breakPoint;
  }
}

template <class T>
SetParallel<T>::SetParallel(const T & _predictionMin, const T & _predictionMax){
  //std::cout<<"MIN/MAX CONSTRUCTOR" <<std::endl;
  #pragma omp parallel
  {
    if(omp_get_thread_num()==0){
      numThreads=omp_get_num_threads();
    }
  }
  numTrees=numThreads;

  rightEndPoints.resize(numTrees-1);
  allData.resize(numTrees);

  T intervalSize=(_predictionMax-_predictionMin) / ( static_cast<T>(numTrees) );

  for(int i=0; i<numTrees-2; ++i){
      rightEndPoints[i] = _predictionMin + ((i+1)*intervalSize);
  }
  rightEndPoints[numTrees-2] = _predictionMax - intervalSize;
}

template <class T>
SetParallel<T>::SetParallel(std::vector<T> & _sampleData){
  //std::cout<<"SAMPLE CONSTRUCTOR" <<std::endl;
  #pragma omp parallel
  {
    if(omp_get_thread_num()==0){
      numThreads=omp_get_num_threads();
    }
  }
  numTrees=numThreads;

  rightEndPoints.resize(numTrees-1);
  allData.resize(numTrees);

  std::sort (_sampleData.begin(), _sampleData.end());

  if(_sampleData.size() == numThreads-1){
    for(int i=0; i<rightEndPoints.size(); ++i){
      rightEndPoints[i]=_sampleData[i];
    }
  }
  else{
    int intervalSize = _sampleData.size() / numThreads;
    if(_sampleData.size() % numThreads == 0){
      for(int i=0; i<rightEndPoints.size(); ++i){
        rightEndPoints[i]=_sampleData[(i+1)*intervalSize-1];
      }
    }
    else{
      for(int i=0; i<rightEndPoints.size(); ++i){
        rightEndPoints[i]=_sampleData[(i+1)*intervalSize];
      }
    }
  }

  if(_sampleData.size() != numThreads-1){
    #pragma omp parallel num_threads(numThreads)
    {
      int myID=omp_get_thread_num();
      for(int vectToInsertPosition=0; vectToInsertPosition<_sampleData.size(); ++vectToInsertPosition){
        if(myID==0){
          if(_sampleData[vectToInsertPosition] <= rightEndPoints[myID]){
            allData[myID].insert(_sampleData[vectToInsertPosition]);
          }
        }
        else if(myID==numThreads-1){
          if(_sampleData[vectToInsertPosition] > rightEndPoints[myID-1]){
            allData[myID].insert(_sampleData[vectToInsertPosition]);
          }
        }
        else{
          if(_sampleData[vectToInsertPosition] > rightEndPoints[myID-1] && _sampleData[vectToInsertPosition] <= rightEndPoints[myID]){
            allData[myID].insert(_sampleData[vectToInsertPosition]);
          }
        }
      }
    }
    #pragma omp barrier
  }
}

template <class T>
SetParallel<T>::SetParallel(const SetParallel & _copyFrom){
  //std::cout<<"COPY CONSTRUCTOR" <<std::endl;
  numThreads = _copyFrom.numThreads;
  numTrees = _copyFrom.numTrees;

  rightEndPoints.resize(numTrees-1);
  for(int i=0; i<rightEndPoints.size(); ++i){
    rightEndPoints[i] = _copyFrom.rightEndPoints[i];
  }

  allData.resize(numTrees);
  for(int i=0; i<numTrees; ++i){
    allData[i] = _copyFrom.allData[i];
  }
}

template <class T>
const SetParallel<T>& SetParallel<T>::operator=(const SetParallel<T>& _copyAssignFrom){
  //std::cout<<"COPY ASSIGNMENT" <<std::endl;
  for(int i=0; i<rightEndPoints.size(); ++i){
    rightEndPoints[i] = _copyAssignFrom.rightEndPoints[i];
  }

  for(int i=0; i<numTrees; ++i){
    allData[i] = _copyAssignFrom.allData[i];
  }

  return *this;
}

template <class T>
SetParallel<T>::SetParallel(SetParallel && _moveFrom){
  //std::cout<<"MOVE CONSTRUCTOR" <<std::endl;
  numThreads = _moveFrom.numThreads;
  numTrees = _moveFrom.numTrees;

  rightEndPoints.resize(numTrees-1);
  for(int i=0; i<rightEndPoints.size(); ++i){
    rightEndPoints[i] = _moveFrom.rightEndPoints[i];
  }

  allData.resize(numTrees);
  for(int i=0; i<numTrees; ++i){
    allData[i] = _moveFrom.allData[i];
  }

  _moveFrom.numThreads=0;
  _moveFrom.numTrees=0;
  _moveFrom.rightEndPoints.clear();
  _moveFrom.allData.clear();
}

template <class T>
SetParallel<T>& SetParallel<T>::operator=(SetParallel<T>&& _moveAssignFrom){
  //std::cout<<"MOVE ASSIGNMENT" <<std::endl;
  for(int i=0; i<rightEndPoints.size(); ++i){
    rightEndPoints[i] = _moveAssignFrom.rightEndPoints[i];
  }

  for(int i=0; i<numTrees; ++i){
    allData[i] = _moveAssignFrom.allData[i];
  }

  _moveAssignFrom.numThreads=0;
  _moveAssignFrom.numTrees=0;
  _moveAssignFrom.rightEndPoints.clear();
  _moveAssignFrom.allData.clear();
}

template <class T>
void SetParallel<T>::insert_parallel(const std::vector<T> & _vectToInsert ){
  #pragma omp parallel num_threads(numThreads)
  {
    int myID=omp_get_thread_num();
    for(int vectToInsertPosition=0; vectToInsertPosition<_vectToInsert.size(); ++vectToInsertPosition){
      if(myID==0){
        if(_vectToInsert[vectToInsertPosition] <= rightEndPoints[myID]){
          allData[myID].insert(_vectToInsert[vectToInsertPosition]);
        }
      }
      else if(myID==numThreads-1){
        if(_vectToInsert[vectToInsertPosition] > rightEndPoints[myID-1]){
          allData[myID].insert(_vectToInsert[vectToInsertPosition]);
        }
      }
      else{
        if(_vectToInsert[vectToInsertPosition] > rightEndPoints[myID-1] && _vectToInsert[vectToInsertPosition] <= rightEndPoints[myID]){
          allData[myID].insert(_vectToInsert[vectToInsertPosition]);
        }
      }
    }
  }
  #pragma omp barrier
  rebalance_trees();
}

template <class T>
void SetParallel<T>::insert_element(const T & _element){
  if(_element <= rightEndPoints[0]){
    allData[0].insert(_element);
  }
  if(_element > rightEndPoints[numTrees-2]){
    allData[numTrees-1].insert(_element);
  }
  for(int i=0; i<numTrees-2; ++i){
    if(_element > rightEndPoints[i] && _element <= rightEndPoints[i+1]){
      allData[i+1].insert(_element);
    }
  }
}

template <class T>
void SetParallel<T>::erase_parallel(const std::vector<T> & _vectToErase ){
  #pragma omp parallel num_threads(numThreads)
  {
    int myID=omp_get_thread_num();
    for(int vectToErasePosition=0; vectToErasePosition<_vectToErase.size(); ++vectToErasePosition){
      if(myID==0){
        if(_vectToErase[vectToErasePosition] <= rightEndPoints[myID]){
          allData[myID].erase(_vectToErase[vectToErasePosition]);
        }
      }
      else if(myID==numThreads-1){
        if(_vectToErase[vectToErasePosition] > rightEndPoints[myID-1]){
          allData[myID].erase(_vectToErase[vectToErasePosition]);
        }
      }
      else{
        if(_vectToErase[vectToErasePosition] > rightEndPoints[myID-1] && _vectToErase[vectToErasePosition] <= rightEndPoints[myID]){
          allData[myID].erase(_vectToErase[vectToErasePosition]);
        }
      }
    }
  }
  #pragma omp barrier
  rebalance_trees();
}

template <class T>
void SetParallel<T>::erase_element(const T & _element){
  if(_element <= rightEndPoints[0]){
    allData[0].erase(_element);
  }
  if(_element > rightEndPoints[numTrees-2]){
    allData[numTrees-1].erase(_element);
  }
  for(int i=0; i<numTrees-2; ++i){
    if(_element > rightEndPoints[i] && _element <= rightEndPoints[i+1]){
      allData[i+1].erase(_element);
    }
  }
}

template <class T>
void SetParallel<T>::find_parallel(const std::vector<T> & _vectToLookUp, std::vector<int> & _results) const{
  #pragma omp parallel num_threads(numThreads)
  {
    int myID=omp_get_thread_num();
    typename std::set<T>::iterator iter;
    for(int vectToLookUpPosition=0; vectToLookUpPosition<_vectToLookUp.size(); ++vectToLookUpPosition){
      if(myID==0){
        if(_vectToLookUp[vectToLookUpPosition] <= rightEndPoints[myID]){
          iter=allData[myID].begin();
          iter=allData[myID].find(_vectToLookUp[vectToLookUpPosition]);
          if(iter != allData[myID].end()){
            _results[vectToLookUpPosition]=1;
          }
        }
      }
      else if(myID==numThreads-1){
        if(_vectToLookUp[vectToLookUpPosition] > rightEndPoints[myID-1]){
          iter=allData[myID].begin();
          iter=allData[myID].find(_vectToLookUp[vectToLookUpPosition]);
          if(iter != allData[myID].end()){
            _results[vectToLookUpPosition]=1;
          }
        }
      }
      else{
        if(_vectToLookUp[vectToLookUpPosition] > rightEndPoints[myID-1] && _vectToLookUp[vectToLookUpPosition] <= rightEndPoints[myID]){
          iter=allData[myID].begin();
          iter=allData[myID].find(_vectToLookUp[vectToLookUpPosition]);
          if(iter != allData[myID].end()){
            _results[vectToLookUpPosition]=1;
          }
        }
      }
    }
  }
  #pragma omp barrier
}

template <class T>
int SetParallel<T>::find_element(const T & _check) const{
  typename std::set<T>::iterator iter;
  if(_check <= rightEndPoints[0]){
    iter=allData[0].begin();
    iter=allData[0].find(_check);
    if(iter != allData[0].end()){
      return 1;
    }
  }
  else if(_check > rightEndPoints[numTrees-2]){
    iter=allData[numTrees-1].begin();
    iter=allData[numTrees-1].find(_check);
    if(iter != allData[numTrees-1].end()){
      return 1;
    }
  }
  else{
    for(int i=0; i<numTrees-2; ++i){
      if(_check > rightEndPoints[i] && _check <= rightEndPoints[i+1]){
        iter=allData[i+1].begin();
        iter=allData[i+1].find(_check);
        if(iter != allData[i+1].end()){
          return 1;
        }
      }
    }
  }

  return 0;
}

template <class T>
void SetParallel<T>::print_all_data() const{
  std::cout<<"Printing all data stored: " <<std::endl;
  typename std::set<T>::iterator iter;
  for(int i=0; i<allData.size(); ++i){
    for(iter=allData[i].begin(); iter!=allData[i].end(); ++iter){
      std::cout<<*iter <<" " <<"(tree " <<i <<")  " <<std::endl;
    }
  }
  std::cout<<std::endl;
}

template <class T>
int SetParallel<T>::size_p() const{
  std::vector<int> sizeOfTrees;
  sizeOfTrees.resize(numThreads);
  for(int i=0; i<numThreads; ++i){
    sizeOfTrees[i]=0;
  }

  #pragma omp parallel num_threads(numThreads)
  {
    int myID=omp_get_thread_num();
    typename std::set<T>::iterator iter;
    for(iter=allData[myID].begin(); iter!=allData[myID].end(); ++iter){
      ++sizeOfTrees[myID];
    }
  }
  #pragma omp barrier

  int totalSize = 0;
  for(int i=0; i<numThreads; ++i){
    totalSize+=sizeOfTrees[i];
  }

  return totalSize;
}

template <class T>
int SetParallel<T>::empty_p() const{
  if(size_p()==0){
    return 1;
  }
  else{
    return 0;
  }
}

template <class T>
void SetParallel<T>::clear_p(){
  #pragma omp parallel num_threads(numThreads)
  {
    int myID=omp_get_thread_num();
    allData[myID].clear();
  }
  #pragma omp barrier
}

template <class T>
T SetParallel<T>::get_right_end_point(const int & _i) const{
  return rightEndPoints[_i];
}

template <class T>
SetParallel<T>::~SetParallel(){}

long long int numOfTestSubjects = 1000000;
dataType x1 = -1000.0, x2 = 1000.0, x;

int main_test_rebalance(){
  std::vector<double> rep;
  rep.resize(3);
  rep[0]=-5;
  rep[1]=0;
  rep[2]=5;
  SetParallel<double> testRebalance(rep);
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
  SetParallel<dataType> pSet(-1000.0, 1000.0);
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

  return 0;
}

SetParallel<dataType> testmove(){
  SetParallel<dataType> t(1.2, 5.3);
  std::vector<dataType> sampleData;
  sampleData.resize(2);
  sampleData[0]=-422;
  sampleData[1]=9784;
  t.insert_parallel(sampleData);
  return t;
}

int main_test_everything(){

  SetParallel<std::string> testDC;
  SetParallel<double> testMinMax(-1000.0, 1000.0);
  std::vector<double> data;
  data.resize(10);
  for(int i=0; i<10; ++i){
    data[i]=-1500+(i*250);
  }
  SetParallel<double> testSample(data);
  testSample.print_all_data();

  SetParallel<double> testCC(testSample);
  testCC.print_all_data();
  testMinMax=testCC;
  testMinMax.print_all_data();

  SetParallel<dataType> testMOVE(-5.0, 5.0);
  testMOVE=testmove();
  testMOVE.print_all_data();

  SetParallel<dataType> testFunctions(-100.0, 100.0);
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
