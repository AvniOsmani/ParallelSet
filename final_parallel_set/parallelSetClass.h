#ifndef MY_PARALLELSETCLASS_H
#define MY_PARALLELSETCLASS_H

#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <string>
#include <omp.h>

template <class T>
class set_parallel{
private:
    std::vector<std::set<T>> allData;
    std::vector<T> rightEndPoints;

    int numTrees;
    int numThreads;

    void rebalance_trees();

public:
    set_parallel();  //Default Constructor for strings
    set_parallel(const T &, const T &);  //Constructor given min and max
    set_parallel(std::vector<T> &);  //Constructor given data or right end points
    set_parallel(const set_parallel &);
    const set_parallel& operator=(const set_parallel &);
    set_parallel(set_parallel &&);
    set_parallel& operator=(set_parallel &&);
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
    virtual ~set_parallel();
};

template <class T>
void set_parallel<T>::rebalance_trees(){
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
set_parallel<T>::set_parallel(){
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
set_parallel<T>::set_parallel(const T & _predictionMin, const T & _predictionMax){
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
set_parallel<T>::set_parallel(std::vector<T> & _sampleData){
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
set_parallel<T>::set_parallel(const set_parallel & _copyFrom){
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
const set_parallel<T>& set_parallel<T>::operator=(const set_parallel<T>& _copyAssignFrom){
  for(int i=0; i<rightEndPoints.size(); ++i){
    rightEndPoints[i] = _copyAssignFrom.rightEndPoints[i];
  }

  for(int i=0; i<numTrees; ++i){
    allData[i] = _copyAssignFrom.allData[i];
  }

  return *this;
}

template <class T>
set_parallel<T>::set_parallel(set_parallel && _moveFrom){
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
set_parallel<T>& set_parallel<T>::operator=(set_parallel<T>&& _moveAssignFrom){
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
void set_parallel<T>::insert_parallel(const std::vector<T> & _vectToInsert ){
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
void set_parallel<T>::insert_element(const T & _element){
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
void set_parallel<T>::erase_parallel(const std::vector<T> & _vectToErase ){
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
void set_parallel<T>::erase_element(const T & _element){
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
void set_parallel<T>::find_parallel(const std::vector<T> & _vectToLookUp, std::vector<int> & _results) const{
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
int set_parallel<T>::find_element(const T & _check) const{
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
void set_parallel<T>::print_all_data() const{
  typename std::set<T>::iterator iter;
  for(int i=0; i<allData.size(); ++i){
    for(iter=allData[i].begin(); iter!=allData[i].end(); ++iter){
      std::cout<<*iter <<" ";
    }
  }
}

template <class T>
int set_parallel<T>::size_p() const{
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
int set_parallel<T>::empty_p() const{
  if(size_p()==0){
    return 1;
  }
  else{
    return 0;
  }
}

template <class T>
void set_parallel<T>::clear_p(){
  #pragma omp parallel num_threads(numThreads)
  {
    int myID=omp_get_thread_num();
    allData[myID].clear();
  }
  #pragma omp barrier
}

template <class T>
T set_parallel<T>::get_right_end_point(const int & _i) const{
  return rightEndPoints[_i];
}

template <class T>
set_parallel<T>::~set_parallel(){}

#endif
