// compile with
// c++ -o i2sp2 idea2_setsP2.cpp -std=c++14 -fopenmp
// run with
// ./i2sp2
// compile and run with
// c++ -o i2sp2 idea2_setsP2.cpp -std=c++14 -fopenmp && ./i2sp2

//each thread maintains one tree
//each thread goes through every element and decided whether
//it should be inserted or deleted into its tree

#include<set>
#include<vector>
#include<iostream>
#include<omp.h>

typedef double dataType;

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
    void lookUpParallel(const std::vector<dataType> &, std::vector<int> &);
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

void SetParallel::lookUpParallel(const std::vector<dataType> & vectToLookUp, std::vector<int> & results){
  #pragma omp parallel num_threads(numThreads)
  {
    int myID=omp_get_thread_num();
    std::set<dataType>::iterator iter;
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
  SetParallel firstTest(-1000, 1000);
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

int main(){
  SetParallel firstTest(-1000, 1000);
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