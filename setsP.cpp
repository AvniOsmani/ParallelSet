#include<set>
#include<vector>
#include<iostream> 

typedef double dataType;

class SetParallel{
private:
    std::vector<std::set<dataType> > allData;
    std::vector<dataType> leftEndPoints;
    //std::vector<dataType> rightEndPoints;
    
    std::vector<int> askForPermissions; // askForPermissions[i]
                                        // can be accessed only by thread
                                        // i
    std::vector<int> permissionsGranted; // permissionsGranted[i]
                                         // is 0/1 depending on whether
                                         // the permission is granted
    
    int numTrees;
    int numThreads;
public:
    SetParallel(const int& =0, const int& =0,
                const dataType& =-1000000.0,
                const & dataType =1000000.0); // construct all the vectors
    
    void insert(const std::vector<dataType> & );
    
};

SetParallel::SetParallel(const int& desiredNumThreads,
                         const int& desiredNumTrees,
                         const dataType& predictionMin,
                         const dataType& predictionMax){
    numThreads=desiredNumThreads;
    numTrees=desiredNumTrees;
    if(numThreads==0){
        numThreads=4;//TERRIBLE, DEFINITELY CANNOT STAY
    }
    if(numTrees==0){
        numTrees=numThreads* 4;
    }
    askForPermissions.resize(numThreads);
    permissionsGranted.resize(numThreads);
    
    leftEndPoints.resize(numTrees);
    allData.resize(numTrees);
    
    dataType intervalSize=(predictionMax-predictionMin) / ( static_cast<dataType>(numTrees) );
    
    for(int i=0;i<numTrees;++i){
        leftEndPoints[i]=predictionMin + i*intervalSize;
    }
    
    
}

void SetParallel::insert(const std::vector<dataType> & vectToInsert ){
    
    
    
    
}
int main(){
    
}
