// compile with
// c++ -o i2spTemplate2 idea2_setsP_template2.cpp -std=c++14 -fopenmp
// run with
// ./i2spTemplate2
// compile and run with
// c++ -o i2spTemplate2 idea2_setsP_template2.cpp -std=c++14 -fopenmp && ./i2spTemplate2

//each thread maintains one tree
//each thread goes through every element and decides whether
//it should be inserted or deleted into its tree

#include<set>
#include<vector>
#include<algorithm>
#include<iostream>
#include<omp.h>

typedef double dataType;

template <class T>
class SetParallel{
private:
    std::vector<std::set<T>> allData;
    std::vector<T> rightEndPoints;

    int numTrees;
    int numThreads;
public:
    SetParallel(const T &, const T &);  //Constructor given min and max
    SetParallel(std::vector<T> &);  //Constructor given sample data or right end points
    SetParallel(const SetParallel &); //Copy Constructor
    const SetParallel& operator=(const SetParallel &); //Copy Assignment
    SetParallel(SetParallel &&); //Move Constructor
    SetParallel& operator=(SetParallel &&); //Move Assignment
    void insertParallel(const std::vector<T> & );
    void eraseParallel(const std::vector<T> & );
    void lookUpParallel(const std::vector<T> &, std::vector<int> &);
    void printAllTrees() const;
    int sizeP() const;
    int emptyP() const;
    void clearP();
    void setRightEndPoints(const std::vector<T> & );
    virtual ~SetParallel();
};

template <class T>
void SetParallel<T>::printAllTrees() const{
  std::cout<<"Printing all data stored: " <<std::endl;
  typename std::set<T>::iterator iter;
  for(int i=0; i<allData.size(); ++i){
    for(iter=allData[i].begin(); iter!=allData[i].end(); ++iter){
      std::cout<<*iter <<" ";
    }
  }
  std::cout<<std::endl;
}

template <class T>
SetParallel<T>::SetParallel(const T & _predictionMin, const T & _predictionMax){

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
    for(int i=0; i<numTrees-1; ++i){
      rightEndPoints[i]=_sampleData[i];
    }
  }
  else{
    int intervalSize = _sampleData.size() / numThreads;
    for(int i=0; i<numTrees-1; ++i){
      rightEndPoints[i]=_sampleData[(i+1)*intervalSize];
    }
  }

  //printing data to make sure it works
  std::cout<<"right end points: ";
  for(int i=0; i<rightEndPoints.size(); ++i){
    std::cout<<rightEndPoints[i] <<" ";
  }
  std::cout<<std::endl;

  std::cout<<"sample data: ";
  for(int i=0; i<_sampleData.size(); ++i){
    std::cout<<_sampleData[i] <<" ";
  }
  std::cout<<std::endl;
}

template <class T>
SetParallel<T>::SetParallel(const SetParallel & _copyFrom){
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
void SetParallel<T>::insertParallel(const std::vector<T> & _vectToInsert ){
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
}

template <class T>
void SetParallel<T>::eraseParallel(const std::vector<T> & _vectToErase ){
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
}

template <class T>
void SetParallel<T>::lookUpParallel(const std::vector<T> & _vectToLookUp, std::vector<int> & _results){
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
}

template <class T>
int SetParallel<T>::sizeP() const{
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

  int totalSize = 0;
  for(int i=0; i<numThreads; ++i){
    totalSize+=sizeOfTrees[i];
  }

  return totalSize;
}

template <class T>
int SetParallel<T>::emptyP() const{
  if(sizeP()==0){
    return 1;
  }
  else{
    return 0;
  }
}

template <class T>
void SetParallel<T>::clearP(){
  #pragma omp parallel num_threads(numThreads)
  {
    int myID=omp_get_thread_num();
    allData[myID].clear();
  }
}

template <class T>
void SetParallel<T>::setRightEndPoints(const std::vector<T> & _newRightEndPoints){
  for(int i=0; i<numThreads-1; ++i){
    rightEndPoints[i]=_newRightEndPoints[i];
  }

  //print to make sure it works properly
  std::cout<<"new right end points are: ";
  for(int i=0; i<numThreads-1; ++i){
    std::cout<<rightEndPoints[i] <<" ";
  }
  std::cout<<std::endl;
}

template <class T>
SetParallel<T>::~SetParallel(){}

int oldmain(){
  SetParallel<dataType> firstTest(-1000, 1000);
  std::vector<dataType> dataToEnter;
  std::cout<<"how many numbers do you want to insert? ";
  int total;
  std::cin>>total;
  dataToEnter.resize(total);
  std::cout<<"give me " <<total <<" numbers: ";
  for(int i=0; i<total; ++i){
    std::cin>>dataToEnter[i];
  }
  firstTest.insertParallel(dataToEnter);

  firstTest.printAllTrees();

  std::vector<dataType> dataToErase;
  std::cout<<"how many numbers do you want to erase? ";
  int eraseTotal;
  std::cin>>eraseTotal;
  dataToErase.resize(eraseTotal);
  std::cout<<"give me " <<eraseTotal <<" numbers: ";
  for(int i=0; i<eraseTotal; ++i){
    std::cin>>dataToErase[i];
  }
  firstTest.eraseParallel(dataToErase);

  firstTest.printAllTrees();

  return 0;
}

int oldoldmain(){
  SetParallel<dataType> firstTest(-1000.0, 1000.0);
  std::vector<dataType> dataToEnter;

  const long max_rand = 1000000L;
  dataType x1 = -1000.0, x2 = 1000.0, x;

  srandom(time(NULL));
  for(int i=0; i<1000; ++i){
    x = x1 + ( x2 - x1) * (random() % max_rand) / max_rand;
    dataToEnter.push_back(x);
  }

  firstTest.insertParallel(dataToEnter);

  std::vector<dataType> testDataToEnter;
  testDataToEnter.push_back(-333);
  testDataToEnter.push_back(354.65);
  testDataToEnter.push_back(18546);
  testDataToEnter.push_back(-754.76);
  testDataToEnter.push_back(5.433);

  firstTest.insertParallel(testDataToEnter);

  std::vector<dataType> dataToLookUp;
  dataToLookUp.push_back(-333);
  dataToLookUp.push_back(5.433);
  dataToLookUp.push_back(4.516);
  dataToLookUp.push_back(4.4515);
  dataToLookUp.push_back(-754.76);


  std::vector<int> results;
  results.resize(5);
  for(int i=0; i<results.size(); ++i){
    results[i] = 0;
  }

  firstTest.lookUpParallel(dataToLookUp, results);

  for(int i = 0; i<results.size(); ++i){
    std::cout <<results[i] <<" ";
  }
  std::cout<<std::endl;

  //firstTest.printAllTrees();

  return 0;
}

int oldoldoldmain(){
  std::vector<dataType> vectRE;
  vectRE.resize(3);
  std::cout<<"enter 3 rightEndPoints:" <<std::endl;
  for(int i=0; i<3; ++i){
    std::cin>>vectRE[i];
  }
  SetParallel<dataType> givenRE(vectRE);

  std::vector<dataType> sample;
  std::cout<<"how much sample data do you want to enter: ";
  int numSample;
  std::cin>>numSample;
  sample.resize(numSample);
  std::cout<<"enter " <<numSample <<" numbers: " <<std::endl;
  for(int i=0; i<numSample; ++i){
    std::cin>>sample[i];
  }
  SetParallel<dataType> givenSample(sample);

  return 0;
}

int old2main(){
  std::vector<dataType> rep;
  rep.resize(3);

  std::cout<<"enter 3 right end points: ";
  for(int i=0; i<3; ++i){
    std::cin>>rep[i];
  }

  SetParallel<dataType> testingFunctions(rep);

  std::vector<dataType> data;
  std::cout<<"how much data do you want to enter ";
  int num;
  std::cin>>num;
  data.resize(num);
  std::cout<<"enter " <<num <<" numbers: " <<std::endl;
  for(int i=0; i<num; ++i){
    std::cin>>data[i];
  }

  testingFunctions.insertParallel(data);
  std::cout<<"the size of the set is " <<testingFunctions.sizeP() <<std::endl;

  std::cout<<"is the set empty: " <<testingFunctions.emptyP() <<std::endl;

  testingFunctions.clearP();
  std::cout<<"cleared set" <<std::endl;
  std::cout<<"size of set is " <<testingFunctions.sizeP() <<std::endl;
  std::cout<<"is the set empty: " <<testingFunctions.emptyP() <<std::endl;

  std::vector<dataType> newrep;
  newrep.resize(3);
  std::cout<<"enter 3 new right end points: ";
  for(int i=0; i<3; ++i){
    std::cin>>newrep[i];
  }

  testingFunctions.setRightEndPoints(newrep);


  return 0;
}

int main(){
  SetParallel<dataType> testcopy(-5, 5);
  std::vector<dataType> sampleData;
  sampleData.resize(8);
  sampleData[0]=-8;
  sampleData[1]=-6;
  sampleData[2]=-4;
  sampleData[3]=1;
  sampleData[4]=3;
  sampleData[5]=5;
  sampleData[6]=9;
  sampleData[7]=43;

  testcopy.insertParallel(sampleData);
  testcopy.printAllTrees();

  SetParallel<dataType> test2(testcopy);
  test2.printAllTrees();

  SetParallel<dataType> testcopyassign(-10, 10);
  testcopyassign=testcopy;
  testcopyassign.printAllTrees();

  return 0;
}