#include<set>
#include<vector>
#include<iostream>
#include<omp.h>

typedef double dataType;

//each thread maintains one tree
//each thread goes through every element and decided whether
//it should be inserted or deleted into its tree

class SetParallel{
private:
    std::vector<std::set<dataType> > allData;
    std::vector<dataType> rightEndPoints;

    int numTrees;
    int numThreads;
public:
    SetParallel(const dataType & =-1000000.0,
                const dataType & =1000000.0);

    void insertParallel(const std::vector<dataType> & );
    void eraseParallel(const std::vector<dataType> & );
    void printAllTrees() const;
};

void SetParallel::printAllTrees() const{
  std::cout<<"Printing all data stored: " <<std::endl;
  std::set<dataType>::iterator iter;
  for(int i=0; i<allData.size(); ++i){
    for(iter=allData[i].begin(); iter!=allData[i].end(); ++iter){
      std::cout<<*iter <<" ";
    }
  }
  std::cout<<std::endl;
}

SetParallel::SetParallel(const dataType& predictionMin,
                         const dataType& predictionMax){

    #pragma omp parallel
    {
      if(omp_get_thread_num()==0){
        numThreads=omp_get_num_threads();
      }
    }
    numTrees=numThreads;

    rightEndPoints.resize(numTrees-1);
    allData.resize(numTrees);

    dataType intervalSize=(predictionMax-predictionMin) / ( static_cast<dataType>(numTrees) );

    for(int i=0; i<numTrees-2; ++i){
        rightEndPoints[i] = predictionMin + ((i+1)*intervalSize);
    }
    rightEndPoints[numTrees-2] = predictionMax - intervalSize;
}

void SetParallel::insertParallel(const std::vector<dataType> & vectToInsert ){
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

void SetParallel::eraseParallel(const std::vector<dataType> & vectToErase ){
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
