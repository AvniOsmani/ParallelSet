// c++ -o testSpeed main.cpp -std=c++14 -fopenmp
// ./testSpeed
// c++ -o testSpeed main.cpp -std=c++14 -fopenmp && ./testSpeed

#include"idea2ParallelSetClass.cpp"
#include"timer4300.cpp"
#include <time.h>

int main(){
  //generate random doubles for insertion
  std::vector<dataType> testData;

  const long max_rand = 1000000L;
  dataType x1 = -1000.0, x2 = 1000.0, x;

  srandom(time(NULL));
  for(int i=0; i<1000000; ++i){
    x = x1 + ( x2 - x1) * (random() % max_rand) / max_rand;
    testData.push_back(x);
  }

  Timer4300 tm;

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
  SetParallel pSet(-1000.0, 1000.0);
  std::cout<<"Inserting into parallel set"<<std::endl;
  tm.start();
  pSet.insertParallel(testData);
  tm.end();
  std::cout<<"Parallel insertion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  //generate random doulbes for deletion
  std::vector<dataType> deleteData;
  srandom(time(NULL));
  for(int i=0; i<1000000; ++i){
    x = x1 + ( x2 - x1) * (random() % max_rand) / max_rand;
    deleteData.push_back(x);
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
  pSet.eraseParallel(deleteData);
  tm.end();
  std::cout<<"Parallel deletion finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;

  /*std::vector<dataType> checkData;
  for(int i=0; i<1000000; ++i){
    x = x1 + ( x2 - x1) * (random() % max_rand) / max_rand;
    checkData.push_back(x);
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
  std::cout<<"Lookup started in parallel set"<<std::endl;
  tm.start();
  pSet.checkElements(checkData, results);
  for(int i=0; i<results.size(); ++i){
    if(results[i] == 1){}
  }
  tm.end();
  std::cout<<"Parallel lookup finished in "<<tm.getTime()<<" miliseconds."<<std::endl <<std::endl;*/

  return 0;
}
