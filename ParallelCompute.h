//#include "c++/json.hpp"
using namespace std;
//using json = nlohmann::json;
#include <iostream>
#include <string>

extern struct DataStruct{
       char TaskName[20];
       int TrialNum;

       vector<vector<vector<vector<double>>>>RawTouchDataArray;  // three dimension: track , time ,differentdata(0:x 1:y 2:pressure)
       vector<vector<double>>TaskTargetDataArray;       


    }TapTaskStruct;

class ParallelCompute{
public:
	//extern "C" void ParallelComputeMainFuction(ofstream* ofs, vector<string> *tasks, json *data){
    //string *task;
    //json *data;
  // json *data;
    ofstream* ofs;
 	//ofstream ofs(filePath + "/Result_" + userName + ".csv", ofstream::out);

  static vector<string> tasks;
/*
 static  struct DataStruct{
       char TaskName[20];
       int TrialNum;

       vector<vector<vector<float>>>RawTouchDataArray;  // three dimension: track , time ,differentdata(0:x 1:y 2:pressure)
       


    
}TapTaskStruct ;
*/
};
