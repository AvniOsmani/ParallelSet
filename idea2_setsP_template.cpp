// compile with
// c++ -o i2spTemplate idea2_setsP_template.cpp -std=c++14 -fopenmp
// run with
// ./i2spTemplate
// compile and run with
// c++ -o i2spTemplate idea2_setsP_template.cpp -std=c++14 -fopenmp && ./i2spTemplate

//each thread maintains one tree
//each thread goes through every element and decides whether
//it should be inserted or deleted into its tree

#include<set>
#include<vector>
#include <algorithm>
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
    SetParallel(const T &, const T &);  //Default Constructor given min and max
    SetParallel(const std::vector<T> &, const int &);  //Default Constructor given break points
    SetParallel(std::vector<T> &, const char &);  //Default Constructor given sample data
    void insertParallel(const std::vector<T> & );
    void eraseParallel(const std::vector<T> & );
    void lookUpParallel(const std::vector<T> &, std::vector<int> &);
    void printAllTrees() const;
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
SetParallel<T>::SetParallel(const T & predictionMin, const T & predictionMax){

  #pragma omp parallel
  {
    if(omp_get_thread_num()==0){
      numThreads=omp_get_num_threads();
    }
  }
  numTrees=numThreads;

  rightEndPoints.resize(numTrees-1);
  allData.resize(numTrees);

  T intervalSize=(predictionMax-predictionMin) / ( static_cast<T>(numTrees) );

  for(int i=0; i<numTrees-2; ++i){
      rightEndPoints[i] = predictionMin + ((i+1)*intervalSize);
  }
  rightEndPoints[numTrees-2] = predictionMax - intervalSize;
}

template <class T>
SetParallel<T>::SetParallel(const std::vector<T> & breakPoints, const int & bP){
  #pragma omp parallel
  {
    if(omp_get_thread_num()==0){
      numThreads=omp_get_num_threads();
    }
  }
  numTrees=numThreads;

  rightEndPoints.resize(numTrees-1);
  allData.resize(numTrees);

  for(int i=0; i<numTrees-1; ++i){
    rightEndPoints[i]=breakPoints[i];
  }

  //printing data to make sure it works
  std::cout<<"right end points: ";
  for(int i=0; i<rightEndPoints.size(); ++i){
    std::cout<<rightEndPoints[i] <<" ";
  }
  std::cout<<std::endl;
}

template <class T>
SetParallel<T>::SetParallel(std::vector<T> & sampleData, const char & s){
  #pragma omp parallel
  {
    if(omp_get_thread_num()==0){
      numThreads=omp_get_num_threads();
    }
  }
  numTrees=numThreads;

  rightEndPoints.resize(numTrees-1);
  allData.resize(numTrees);

  std::sort (sampleData.begin(), sampleData.end());
  int intervalSize = sampleData.size() / numThreads;
  for(int i=0; i<numTrees-1; ++i){
    rightEndPoints[i]=sampleData[(i+1)*intervalSize];
  }

  //printing data to make sure it works
  std::cout<<"right end points: ";
  for(int i=0; i<rightEndPoints.size(); ++i){
    std::cout<<rightEndPoints[i] <<" ";
  }
  std::cout<<std::endl;

  std::cout<<"sample data: ";
  for(int i=0; i<sampleData.size(); ++i){
    std::cout<<sampleData[i] <<" ";
  }
  std::cout<<std::endl;
}

template <class T>
void SetParallel<T>::insertParallel(const std::vector<T> & vectToInsert ){
  #pragma omp parallel num_threads(numThreads)
  {
    int myID=omp_get_thread_num();
    for(int vectToInsertPosition=0; vectToInsertPosition<vectToInsert.size(); ++vectToInsertPosition){
      if(myID==0){
        if(vectToInsert[vectToInsertPosition] <= rightEndPoints[myID]){
          allData[myID].insert(vectToInsert[vectToInsertPosition]);
        }
      }
      else if(myID==numThreads-1){
        if(vectToInsert[vectToInsertPosition] > rightEndPoints[myID-1]){
          allData[myID].insert(vectToInsert[vectToInsertPosition]);
        }
      }
      else{
        if(vectToInsert[vectToInsertPosition] > rightEndPoints[myID-1] && vectToInsert[vectToInsertPosition] <= rightEndPoints[myID]){
          allData[myID].insert(vectToInsert[vectToInsertPosition]);
        }
      }
    }
  }
  #pragma omp barrier
}

template <class T>
void SetParallel<T>::eraseParallel(const std::vector<T> & vectToErase ){
  #pragma omp parallel num_threads(numThreads)
  {
    int myID=omp_get_thread_num();
    for(int vectToErasePosition=0; vectToErasePosition<vectToErase.size(); ++vectToErasePosition){
      if(myID==0){
        if(vectToErase[vectToErasePosition] <= rightEndPoints[myID]){
          allData[myID].erase(vectToErase[vectToErasePosition]);
        }
      }
      else if(myID==numThreads-1){
        if(vectToErase[vectToErasePosition] > rightEndPoints[myID-1]){
          allData[myID].erase(vectToErase[vectToErasePosition]);
        }
      }
      else{
        if(vectToErase[vectToErasePosition] > rightEndPoints[myID-1] && vectToErase[vectToErasePosition] <= rightEndPoints[myID]){
          allData[myID].erase(vectToErase[vectToErasePosition]);
        }
      }
    }
  }
  #pragma omp barrier
}

template <class T>
void SetParallel<T>::lookUpParallel(const std::vector<T> & vectToLookUp, std::vector<int> & results){
  #pragma omp parallel num_threads(numThreads)
  {
    int myID=omp_get_thread_num();
    typename std::set<T>::iterator iter;
    for(int vectToLookUpPosition=0; vectToLookUpPosition<vectToLookUp.size(); ++vectToLookUpPosition){
      if(myID==0){
        if(vectToLookUp[vectToLookUpPosition] <= rightEndPoints[myID]){
          iter=allData[myID].begin();
          iter=allData[myID].find(vectToLookUp[vectToLookUpPosition]);
          if(iter != allData[myID].end()){
            results[vectToLookUpPosition]=1;
          }
        }
      }
      else if(myID==numThreads-1){
        if(vectToLookUp[vectToLookUpPosition] > rightEndPoints[myID-1]){
          iter=allData[myID].begin();
          iter=allData[myID].find(vectToLookUp[vectToLookUpPosition]);
          if(iter != allData[myID].end()){
            results[vectToLookUpPosition]=1;
          }
        }
      }
      else{
        if(vectToLookUp[vectToLookUpPosition] > rightEndPoints[myID-1] && vectToLookUp[vectToLookUpPosition] <= rightEndPoints[myID]){
          iter=allData[myID].begin();
          iter=allData[myID].find(vectToLookUp[vectToLookUpPosition]);
          if(iter != allData[myID].end()){
            results[vectToLookUpPosition]=1;
          }
        }
      }
    }
  }
}

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

int main(){
  std::vector<dataType> vectRE;
  vectRE.resize(3);
  std::cout<<"enter 3 rightEndPoints:" <<std::endl;
  for(int i=0; i<3; ++i){
    std::cin>>vectRE[i];
  }
  SetParallel<dataType> givenRE(vectRE, 1);

  std::vector<dataType> sample;
  std::cout<<"how much sample data do you want to enter: ";
  int numSample;
  std::cin>>numSample;
  sample.resize(numSample);
  std::cout<<"enter " <<numSample <<" numbers: " <<std::endl;
  for(int i=0; i<numSample; ++i){
    std::cin>>sample[i];
  }
  SetParallel<dataType> givenSample(sample, 's');

  return 0;
}
