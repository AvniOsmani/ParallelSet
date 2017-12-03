#include<set>
#include<vector>
#include<iostream>
#include<omp.h>

typedef double dataType;

void printSwitchBoardInfo(const std::vector<int> & requestedInfo,
                          const std::vector<int> & grantedInfo,
                          const int & totalNumOfThreads){
  std::cout<<std::endl <<"Printing the permissions requested:" <<std::endl;
  std::cout<<"thread number:  ";
  for(int i=0; i<totalNumOfThreads; ++i){
    std::cout<<i <<" ";
  }
  std::cout<<std::endl;
  std::cout<<"tree requested: ";
  for(int i=0; i<requestedInfo.size(); ++i){
    std::cout<<requestedInfo[i] <<" ";
  }
  std::cout<<std::endl;

  std::cout<<"Printing the permissions granted:" <<std::endl;
  std::cout<<"thread number: ";
  for(int i=0; i<totalNumOfThreads; ++i){
    std::cout<<i <<" ";
  }
  std::cout<<std::endl;
  std::cout<<"tree granted:  ";
  for(int i=0; i<grantedInfo.size(); ++i){
    std::cout<<grantedInfo[i] <<" ";
  }
  std::cout<<std::endl <<std::endl;
}

class SetParallel{
private:
    std::vector<std::set<dataType> > allData;
    std::vector<dataType> rightEndPoints;
    //std::vector<dataType> rightEndPoints;

    std::vector<int> permissionsRequested; // permissionsRequested[i]
                                        // can be accessed only by thread
                                        // i
    std::vector<int> permissionsGranted; // permissionsGranted[i]
                                         // is 0/1 depending on whether
                                         // the permission is granted
                                         // 1 = granted
                                         // 0 = not granted

    int numTrees;
    int numThreads;
public:
    SetParallel(const int& =0, const int& =0,
                const dataType & =-1000000.0,
                const dataType & =1000000.0); // construct all the vectors

    void insertParallel(const std::vector<dataType> & );
    void eraseParallel(const std::vector<dataType> & );
    void printAllTrees() const;
    void checkElements(const std::vector<dataType> &, std::vector<int> & );
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

SetParallel::SetParallel(const int& desiredNumThreads, //won't need later
                         const int& desiredNumTrees,   //won't need later
                         const dataType& predictionMin,
                         const dataType& predictionMax){
    numThreads=desiredNumThreads; //won't need later
    numTrees=desiredNumTrees;    //won't need later
    if(numThreads==0){
        numThreads=4;//TERRIBLE, DEFINITELY CANNOT STAY
    }
    if(numTrees==0){
        numTrees=numThreads* 4;
    }
    permissionsRequested.resize(numThreads);
    permissionsGranted.resize(numThreads);

    rightEndPoints.resize(numTrees-1);
    allData.resize(numTrees);

    dataType intervalSize=(predictionMax-predictionMin) / ( static_cast<dataType>(numTrees) );

    for(int i=0; i<numTrees-2; ++i){
        rightEndPoints[i] = predictionMin + ((i+1)*intervalSize);
    }
    rightEndPoints[numTrees-2] = predictionMax - intervalSize;
}

void SetParallel::insertParallel(const std::vector<dataType> & vectToInsert ){
  int numberOfThreads;
  #pragma omp parallel
  {
    if(omp_get_thread_num()==0){
      numberOfThreads=omp_get_num_threads();
    }
  }

  int numberOfJobs = (vectToInsert.size() / numberOfThreads);
  if(vectToInsert.size() % numberOfThreads != 0){
    ++numberOfJobs;
  }
  std::set<dataType> checkPermissions;
  std::set<dataType>::iterator it;
  std::vector<dataType> leftovers;
  leftovers.resize(numberOfThreads);

  for(int counter=0; counter<numberOfJobs; ++counter){ //each loop, thread inserts one datatype to allData
    //determine which tree each thread needs to insert to
    #pragma omp parallel num_threads(numberOfThreads)
    {
      int myID=omp_get_thread_num();
      int vectToInsertPosition = counter * numberOfThreads + myID;
      int insertToTree;

      if(vectToInsertPosition < vectToInsert.size()){
        if(vectToInsert[vectToInsertPosition] < rightEndPoints[0]){
          insertToTree=0;
        }
        if(vectToInsert[vectToInsertPosition] >= rightEndPoints[numTrees-2]){
          insertToTree=numTrees-1;
        }
        for(int i=0; i<numTrees-2; ++i){
          if(vectToInsert[vectToInsertPosition] >= rightEndPoints[i]
              && vectToInsert[vectToInsertPosition] < rightEndPoints[i+1]){
                insertToTree=i+1;
              }
        }
        permissionsRequested[myID]=insertToTree;
      }
    }
    #pragma omp barrier

    //cpu grants permissions
    checkPermissions.clear();
    checkPermissions.insert(permissionsRequested[0]);
    permissionsGranted[0]=1;
    for(int i=1; i<numberOfThreads; ++i){
      it=checkPermissions.begin();
      it=checkPermissions.find(permissionsRequested[i]);
      if(it==checkPermissions.end()){
        checkPermissions.insert(permissionsRequested[i]);
        permissionsGranted[i]=1;
      }
      else{
        permissionsGranted[i]=0;
      }
    }

    //printSwitchBoardInfo(permissionsRequested, permissionsGranted, numberOfThreads);
    //threads that were granted permissions insert into allData
    //threads that don't have permission will insert elements into leftovers
    //cpu will insert every element from leftovers into allData
    #pragma omp parallel num_threads(numberOfThreads)
    {
      int myID=omp_get_thread_num();
      int vectToInsertPosition = counter * numberOfThreads + myID;
      if(permissionsGranted[myID] == 1 && vectToInsertPosition < vectToInsert.size()){
        allData[permissionsRequested[myID]].insert(vectToInsert[vectToInsertPosition]);
        leftovers[myID]=0;
      }
      if(permissionsGranted[myID] == 0 && vectToInsertPosition < vectToInsert.size()){
        leftovers[myID]=vectToInsert[vectToInsertPosition];
      }
      if(vectToInsertPosition > vectToInsert.size()-1){
        leftovers[myID]=0;
      }
    }
    #pragma omp barrier

    for(int i=0; i<numberOfThreads; ++i){
      if(leftovers[i] != 0){
        allData[permissionsRequested[i]].insert(leftovers[i]);
      }
    }
  }
}

void SetParallel::eraseParallel(const std::vector<dataType> & vectToDelete ){
  int numberOfThreads;
  #pragma omp parallel
  {
    if(omp_get_thread_num()==0){
      numberOfThreads=omp_get_num_threads();
    }
  }

  int numberOfJobs = (vectToDelete.size() / numberOfThreads);
  if(vectToDelete.size() % numberOfThreads != 0){
    ++numberOfJobs;
  }
  std::set<dataType> checkPermissions;
  std::set<dataType>::iterator it;
  std::vector<dataType> leftovers;
  leftovers.resize(numberOfThreads);

  for(int counter=0; counter<numberOfJobs; ++counter){ //each loop, thread deletes one datatype to allData
    //determine which tree each thread needs to delete from
    #pragma omp parallel num_threads(numberOfThreads)
    {
      int myID=omp_get_thread_num();
      int vectToDeletePosition = counter * numberOfThreads + myID;
      int deleteFromTree;

      if(vectToDeletePosition < vectToDelete.size()){
        if(vectToDelete[vectToDeletePosition] < rightEndPoints[0]){
          deleteFromTree=0;
        }
        if(vectToDelete[vectToDeletePosition] >= rightEndPoints[numTrees-2]){
          deleteFromTree=numTrees-1;
        }
        for(int i=0; i<numTrees-2; ++i){
          if(vectToDelete[vectToDeletePosition] >= rightEndPoints[i]
              && vectToDelete[vectToDeletePosition] < rightEndPoints[i+1]){
                deleteFromTree=i+1;
              }
        }
        permissionsRequested[myID]=deleteFromTree;
      }
    }
    #pragma omp barrier

    //cpu grants permissions
    checkPermissions.clear();
    checkPermissions.insert(permissionsRequested[0]);
    permissionsGranted[0]=1;
    for(int i=1; i<numberOfThreads; ++i){
      it=checkPermissions.begin();
      it=checkPermissions.find(permissionsRequested[i]);
      if(it==checkPermissions.end()){
        checkPermissions.insert(permissionsRequested[i]);
        permissionsGranted[i]=1;
      }
      else{
        permissionsGranted[i]=0;
      }
    }

    //printSwitchBoardInfo(permissionsRequested, permissionsGranted, numberOfThreads);
    //threads that were granted permissions delete from allData
    //threads that don't have permission will place their elements into leftovers
    //cpu will delete everything in leftovers
    #pragma omp parallel num_threads(numberOfThreads)
    {
      int myID=omp_get_thread_num();
      int vectToDeletePosition = counter * numberOfThreads + myID;
      if(permissionsGranted[myID] == 1 && vectToDeletePosition < vectToDelete.size()){
        allData[permissionsRequested[myID]].erase(vectToDelete[vectToDeletePosition]);
        leftovers[myID]=0;
      }
      if(permissionsGranted[myID] == 0 && vectToDeletePosition < vectToDelete.size()){
        leftovers[myID]=vectToDelete[vectToDeletePosition];
      }
      if(vectToDeletePosition > vectToDelete.size()-1){
        leftovers[myID]=0;
      }
    }
    #pragma omp barrier

    for(int i=0; i<numberOfThreads; ++i){
      if(leftovers[i] != 0){
        allData[permissionsRequested[i]].erase(leftovers[i]);
      }
    }
  }
}

void SetParallel::checkElements(const std::vector<dataType> & vectToCheck, std::vector<int> & _results){
  _results.resize(vectToCheck.size());
  for(int i=0; i<_results.size(); ++i){
    _results[i] = 0;
  }

  int numberOfThreads;
  #pragma omp parallel
  {
    if(omp_get_thread_num()==0){
      numberOfThreads=omp_get_num_threads();
    }
  }

  int numberOfJobs = (vectToCheck.size() / numberOfThreads);
  if(vectToCheck.size() % numberOfThreads != 0){
    ++numberOfJobs;
  }

  for(int counter=0; counter<numberOfJobs; ++counter){
    //std::cout<<"counter=" <<counter <<" numberOfJobs=" <<numberOfJobs <<std::endl;
    #pragma omp parallel num_threads(numberOfThreads)
    {
      int myID=omp_get_thread_num();
      int vectToCheckPosition = counter * numberOfThreads + myID;
      int checkTree;

      if(vectToCheckPosition < vectToCheck.size()){
        if(vectToCheck[vectToCheckPosition] < rightEndPoints[0]){
          checkTree=0;
        }
        if(vectToCheck[vectToCheckPosition] >= rightEndPoints[numTrees-2]){
          checkTree=numTrees-1;
        }
        for(int i=0; i<numTrees-2; ++i){
          if(vectToCheck[vectToCheckPosition] >= rightEndPoints[i]
              && vectToCheck[vectToCheckPosition] < rightEndPoints[i+1]){
                checkTree=i+1;
              }
        }
      }
      std::set<dataType>::iterator checkIt;
      for(checkIt=allData[checkTree].begin(); checkIt!=allData[checkTree].end(); ++checkIt){
        if(vectToCheck[vectToCheckPosition] == *checkIt){
          _results[vectToCheckPosition] = 1;
        }
      }
    }
    #pragma omp barrier
  }
}
