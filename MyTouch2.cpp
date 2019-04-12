#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
//#include "c++/complete/TouchAccomodations.h"
#include "c++/json.hpp"
#include "ParallelCompute.h"
#include <time.h>

using namespace std;
using json = nlohmann::json;


extern "C" void MyMain();
extern "C" int MyMain2();
extern "C" void ParallelComputeMainFuction(ofstream* ofs, vector<char*> *tasks);

int main(int argc, char *argv[]){
	clock_t start_time, end_time;
	start_time = clock(); 
    
    float total_time = 0;
	//cout<<"Implement Mytouch ,Start Tome: "<<start_time <<endl;
	string fileName="../2002_2018-09-03_10-29-36.json";

    json data;
    ifstream ifs(fileName, ifstream::in);
    ifs >> data;
	//int Xlocation=int(data["tapTask"]["trials"][0]["rawTouches"][0]["location"][0]);
	double x = double((data)["tapTask"]["trials"][0]["rawTouchTracks"][0]["rawTouches"][0]["location"][0]);
	cout<<x<<endl;
	TapTaskStruct.TrialNum=float(x);


	//////////////////Data to Vector/////////////////////////
	int trialNum = (data)["tapTask"]["trials"].size(); 
	TapTaskStruct.RawTouchDataArray.resize(trialNum);   //the number of trials from user ,fixed
	TapTaskStruct.TaskTargetDataArray.resize(trialNum);
	for (int trialIndex = 0 ; trialIndex < trialNum ; trialIndex++){		
		int trackNum = (data)["tapTask"]["trials"][trialIndex]["rawTouchTracks"].size();   //the number of finger
		TapTaskStruct.RawTouchDataArray[trialIndex].resize(trackNum);
		if (trackNum >= 1){
			bool noTouchPoint = true;            
			for (int trackIndex = 0 ; trackIndex < trackNum ; trackIndex++){
							
				int touchPointNum = (data)["tapTask"]["trials"][trialIndex]["rawTouchTracks"][trackIndex]["rawTouches"].size();//the number of data frames
				TapTaskStruct.RawTouchDataArray[trialIndex][trackIndex].resize(touchPointNum);        		
				for (int touchPointIndex = 0 ; touchPointIndex < touchPointNum ; touchPointIndex++){
									double touchpoint_location_x = double((data)["tapTask"]["trials"][trialIndex]["rawTouchTracks"][trackIndex]["rawTouches"][touchPointIndex]["location"][0]) ;
									double touchpoint_location_y = double((data)["tapTask"]["trials"][trialIndex]["rawTouchTracks"][trackIndex]["rawTouches"][touchPointIndex]["location"][1]) ;
									double touchpoint_timestamp = double((data)["tapTask"]["trials"][trialIndex]["rawTouchTracks"][trackIndex]["rawTouches"][touchPointIndex]["timestamp"]) ;

									TapTaskStruct.RawTouchDataArray[trialIndex][trackIndex][touchPointIndex].push_back(touchpoint_timestamp);
									TapTaskStruct.RawTouchDataArray[trialIndex][trackIndex][touchPointIndex].push_back(touchpoint_location_x);
									TapTaskStruct.RawTouchDataArray[trialIndex][trackIndex][touchPointIndex].push_back(touchpoint_location_y);	
				}
			}
		}
		//TapTaskStruct.RawTouchDataArray[trialIndex].resize(trackNum);
		//TapTaskStruct.TaskTargetDataArray[trialIndex].resize(2);  //0 for target location 1 for target size
		//cout<<(data)["tapTask"]["trials"][trialIndex]["targetFrame"]<<endl;

		double Target_location_x = double((data)["tapTask"]["trials"][trialIndex]["targetFrame"][0][0]) ;
		double Target_location_y = double((data)["tapTask"]["trials"][trialIndex]["targetFrame"][0][1]) ;
		double Target_Size_x=double((data)["tapTask"]["trials"][trialIndex]["targetFrame"][1][0]) ;
		double Target_Size_y=double((data)["tapTask"]["trials"][trialIndex]["targetFrame"][1][1]) ;
		
		//TapTaskStruct.TaskTargetDataArray[trialIndex].push_back(Target_location_x).push_back(Target_location_y).push_back(Target_Size_x).push_back(Target_Size_y);
		TapTaskStruct.TaskTargetDataArray[trialIndex].push_back(Target_location_x);
		TapTaskStruct.TaskTargetDataArray[trialIndex].push_back(Target_location_y);
		TapTaskStruct.TaskTargetDataArray[trialIndex].push_back(Target_Size_x);
		TapTaskStruct.TaskTargetDataArray[trialIndex].push_back(Target_Size_y);
			
		//	cout<<TapTaskStruct.TaskTargetDataArray[trialIndex][0]<<" "<<TapTaskStruct.TaskTargetDataArray[trialIndex][1]<<" "<<TapTaskStruct.TaskTargetDataArray[trialIndex][2]<<" "<<TapTaskStruct.TaskTargetDataArray[trialIndex][3]<<endl;
			


	}
//////////////////Data to Vector/////////////////////////


//TapTaskStruct.TaskName=char((data)["tapTask"]["trials"][0]["rawTouchTracks"][0]["rawTouches"][0]["location"][1]);

//std::cout << ifs << std::endl;
//cout<<data<<endl;
               ifs.close();
    
//MyMain();
//MyMain2();
    ofstream ofs("Result.csv", ofstream::out);
  
    vector<char*> tasks;
    //ofs( "Result.csv", ofstream::out);
//     tasks.push_back("tapTask");
    
    printf("Call ParallelComputeMainFuction\n");
    ParallelComputeMainFuction(&ofs, &tasks);



//int rawdatacount=TapTaskStruct.RawTouchDataArray[0][0][0].size(); 
//printf("This is rawdatacount: %d",rawdatacount);
    ofs.close();
   
   


    end_time = clock();	   
    total_time = (float)(end_time - start_time)/CLOCKS_PER_SEC;
    printf("Time : %f sec \n", total_time);



        return 0;

}
