
#include <algorithm>
#include "cuda.h"
#include <limits>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include "ParallelCompute.h"

#include<stdio.h>
#include<cuda.h>
#include<cuda_runtime.h>
#include<device_launch_parameters.h>

using namespace std;


// __global__ TouchAccommodations::TouchAccommodations(double _fromTime, double _toTime){
// 	fromTime = _fromTime;
// 	toTime = _toTime;
// 	_MyTouchTask = MyTouchTask();
// }

DataStruct TapTaskStruct{};  // three dimension: track , time ,differentdata(0:x 1:y 2:pressure)
__device__ int MyFind(bool Array[],int ArrayLen,bool Target ){
	for(int i=0;i<ArrayLen;i++){
		if(Array[i]==Target){return i;}
	}
	return -1;
 }       
 __device__ int MyFind2(bool Array[],int ArrayLen,int Target ){
	for(int i=0;i<ArrayLen;i++){
		if(Array[i]==Target){return i;}
	}
	return -1;
 }  
__device__   bool ElementExist(double* Array,int i){
	if(Array[i]==-1){return false;}  //No element exist
	else{return true;}
}
__device__ int IndexTo1D(int trackindex,int pointindex,int MaxPoint_InTracks){
	return trackindex*MaxPoint_InTracks+pointindex;
}

__device__ int NumPointsInEachTrack(double* Array,int trackindex,int MaxPoint_InTracks){
	for(int i=0;i<MaxPoint_InTracks;i++){
		if(!ElementExist(Array,IndexTo1D(trackindex,i,MaxPoint_InTracks))){return i;};
	}
	return MaxPoint_InTracks;
}

__device__ int StartPointIndexInEachTrack(double* Array,int trackindex,int MaxPoint_InTracks){
	for(int i=0;i<MaxPoint_InTracks;i++){
		if(!ElementExist(Array,IndexTo1D(trackindex,i,MaxPoint_InTracks))){continue;}
		else{return i;}
	}
return -1;
}

__device__ int EndPointIndexInEachTrack(double* Array,int trackindex,int MaxPoint_InTracks){
	for(int i=MaxPoint_InTracks-1;i>=0;i--){
		if(!ElementExist(Array,IndexTo1D(trackindex,i,MaxPoint_InTracks))){continue;}
		else{return i;}
	}
	return -1;
}

__device__ int MyTest(){
	printf("MyTest!");
	return 1;

}
__device__ int TapRecognizer(double* Filtered_TimeStamp_AllTracks_InEachTrial,double* Filtered_PositionX_AllTracks_InEachTrial ,
	double* Filtered_PositionY_AllTracks_InEachTrial,int MaxPoint_InTracks,int MaxTrack_InTrials,double*Device_TrialTargetInfo_InEachTrial,int* Filtered_Index_AllTracks_InEachTrial){

	double Target_location_x=Device_TrialTargetInfo_InEachTrial[0];
	double Target_location_y=Device_TrialTargetInfo_InEachTrial[1];
	double Target_size_x=Device_TrialTargetInfo_InEachTrial[2];
	double Target_size_y=Device_TrialTargetInfo_InEachTrial[3];
	double TapMaximumDuration = 1.5;
	double TapAllowableMovement = 10;
	for(int j=0;j<8;j++){
if(j==threadIdx.x){
	for (int i=0;i<MaxPoint_InTracks*MaxTrack_InTrials;i++){


	printf("FilteredData: track: %d point %d : (%f,%f,%f) choose(%d) by (%d,%d,%d)(%d,%d,%d) \n",i/MaxPoint_InTracks,i%MaxPoint_InTracks,Filtered_TimeStamp_AllTracks_InEachTrial[IndexTo1D(i/MaxPoint_InTracks,i%MaxPoint_InTracks,MaxPoint_InTracks)],Filtered_PositionX_AllTracks_InEachTrial[IndexTo1D(i/MaxPoint_InTracks,i%MaxPoint_InTracks,MaxPoint_InTracks)],Filtered_PositionY_AllTracks_InEachTrial[IndexTo1D(i/MaxPoint_InTracks,i%MaxPoint_InTracks,MaxPoint_InTracks)],Filtered_Index_AllTracks_InEachTrial[IndexTo1D(i/MaxPoint_InTracks,i%MaxPoint_InTracks,MaxPoint_InTracks)],blockIdx.x,blockDim.x,threadIdx.x, blockIdx.y , blockDim.y , threadIdx.y);
		}}		
	}
//	printf("Test FilterData: %f by (%d,%d,%d)(%d,%d,%d)\n",Filtered_TimeStamp_AllTracks_InEachTrial[0],blockIdx.x,blockDim.x,threadIdx.x, blockIdx.y , blockDim.y , threadIdx.y);	
//	printf("TapRecognizer: (%f,%f,%f,%f)",Target_location_x,Target_location_y,Target_size_x,Target_size_y);
     
	if (MaxTrack_InTrials >= 1){
			bool noTouchPoint = true;
			int trackIndex, touchPointNum;
			int * StartPointIndexInEachTrackArray=new int(MaxTrack_InTrials);
			int * EndPointIndexInEachTrackArray=new int(MaxTrack_InTrials);


			for (trackIndex = 0 ; trackIndex < MaxTrack_InTrials ; trackIndex++){

				//touchPointNum = (*tapTaskData)["trials"][trialIndex]["rawTouchTracks"][trackIndex]["rawTouches"].size();
				//touchPointNum =NumPointsInEachTrack(Filtered_TimeStamp_AllTracks_InEachTrial,trackIndex,MaxPoint_InTracks);
				StartPointIndexInEachTrackArray[trackIndex]=StartPointIndexInEachTrack(Filtered_TimeStamp_AllTracks_InEachTrial,trackIndex,MaxPoint_InTracks);
				EndPointIndexInEachTrackArray[trackIndex]=EndPointIndexInEachTrack(Filtered_TimeStamp_AllTracks_InEachTrial,trackIndex,MaxPoint_InTracks);
				if(StartPointIndexInEachTrackArray[trackIndex]==-1&&EndPointIndexInEachTrackArray[trackIndex]==-1){touchPointNum=0;}
				else{
				noTouchPoint=false;
						//break;
				}

    printf("Track: %d , StartTouch: %d , EndTouch:%d  by X(%d,%d,%d) Y(%d,%d,%d)\n",trackIndex,StartPointIndexInEachTrackArray[trackIndex],EndPointIndexInEachTrackArray[trackIndex],blockIdx.x,blockDim.x,threadIdx.x, blockIdx.y , blockDim.y , threadIdx.y);
			
			}
			if (noTouchPoint){
				//cout << "No touch point" << endl;
				printf("Fail No Touch Point by X(%d,%d,%d) Y(%d,%d,%d)\n",blockIdx.x,blockDim.x,threadIdx.x, blockIdx.y , blockDim.y , threadIdx.y);
				//continue;
				return 0;
			}
			
			//bug?? 本來沒有
			for (trackIndex = 0 ; trackIndex < MaxTrack_InTrials ; trackIndex++){
			
			//About Timeout//
			/*
			if (double((*tapTaskData)["trials"][trialIndex]["rawTouchTracks"][trackIndex]["rawTouches"][touchPointNum-1]["timestamp"]) - 
				double((*tapTaskData)["trials"][trialIndex]["rawTouchTracks"][trackIndex]["rawTouches"][0]["timestamp"]) > TapMaximumDuration){
				//cout << "Timeout" << endl;
				//continue;
				return 0;
			}
			*/

				if(StartPointIndexInEachTrackArray[trackIndex]==-1&&EndPointIndexInEachTrackArray[trackIndex]==-1){ }//this track No touch point
				else{
						if(Filtered_TimeStamp_AllTracks_InEachTrial[IndexTo1D(trackIndex,EndPointIndexInEachTrackArray[trackIndex],MaxPoint_InTracks)]
							-Filtered_TimeStamp_AllTracks_InEachTrial[IndexTo1D(trackIndex,StartPointIndexInEachTrackArray[trackIndex],MaxPoint_InTracks)]> TapMaximumDuration){
					printf("Fail TimeOut by X(%d,%d,%d) Y(%d,%d,%d)",blockIdx.x,blockDim.x,threadIdx.x, blockIdx.y , blockDim.y , threadIdx.y);
						return 0;
				}
			}



			//About First Touch point corresponse to our test trial target//
			/*
			if (abs(double((*tapTaskData)["trials"][trialIndex]["rawTouchTracks"][trackIndex]["rawTouches"][0]["location"][0]) - double((*tapTaskData)["trials"][trialIndex]["targetFrame"][0][0]) - double((*tapTaskData)["trials"][trialIndex]["targetFrame"][1][0])*0.5) > double((*tapTaskData)["trials"][trialIndex]["targetFrame"][1][0])*0.5 ||
				abs(double((*tapTaskData)["trials"][trialIndex]["rawTouchTracks"][trackIndex]["rawTouches"][0]["location"][1]) - double((*tapTaskData)["trials"][trialIndex]["targetFrame"][0][1]) - double((*tapTaskData)["trials"][trialIndex]["targetFrame"][1][1])*0.5) > double((*tapTaskData)["trials"][trialIndex]["targetFrame"][1][1])*0.5){
				//cout << "First touch point is not correct" << endl;
				//continue;
				return 0;
			}
			*/
			if (abs(Filtered_PositionX_AllTracks_InEachTrial[IndexTo1D(trackIndex,StartPointIndexInEachTrackArray[trackIndex],MaxPoint_InTracks)] - Target_location_x- Target_size_x*0.5) > Target_size_x*0.5 ||
				abs(Filtered_PositionY_AllTracks_InEachTrial[IndexTo1D(trackIndex,StartPointIndexInEachTrackArray[trackIndex],MaxPoint_InTracks)] - Target_location_y- Target_size_y*0.5) > Target_size_y*0.5){
				//cout << "First touch point is not correct" << endl;
				//continue;
				printf("Fail! First Touch point not correct by X(%d,%d,%d) Y(%d,%d,%d)",blockIdx.x,blockDim.x,threadIdx.x, blockIdx.y , blockDim.y , threadIdx.y);
				return 0;
			}


			//About Sliding on the screen//
			bool isInAllowableMovement = true;
			for (int touchPointIndex = StartPointIndexInEachTrackArray[trackIndex] ; touchPointIndex < EndPointIndexInEachTrackArray[trackIndex] ; touchPointIndex++){

				//double dx = double((*tapTaskData)["trials"][trialIndex]["rawTouchTracks"][trackIndex]["rawTouches"][touchPointIndex]["location"][0]) - double((*tapTaskData)["trials"][trialIndex]["rawTouchTracks"][trackIndex]["rawTouches"][0]["location"][0]);
				//double dy = double((*tapTaskData)["trials"][trialIndex]["rawTouchTracks"][trackIndex]["rawTouches"][touchPointIndex]["location"][1]) - double((*tapTaskData)["trials"][trialIndex]["rawTouchTracks"][trackIndex]["rawTouches"][0]["location"][1]);
				
				double dx = Filtered_PositionX_AllTracks_InEachTrial[IndexTo1D(trackIndex,touchPointIndex,MaxPoint_InTracks)]  - Filtered_PositionX_AllTracks_InEachTrial[IndexTo1D(trackIndex,StartPointIndexInEachTrackArray[trackIndex],MaxPoint_InTracks)];
				double dy = Filtered_PositionY_AllTracks_InEachTrial[IndexTo1D(trackIndex,touchPointIndex,MaxPoint_InTracks)]  - Filtered_PositionY_AllTracks_InEachTrial[IndexTo1D(trackIndex,StartPointIndexInEachTrackArray[trackIndex],MaxPoint_InTracks)];

				if (dx * dx + dy * dy >= TapAllowableMovement * TapAllowableMovement){
					isInAllowableMovement = false;
					break;
				}
			}
			if (!isInAllowableMovement){
				printf("Fail Move larger than allowable move by X(%d,%d,%d) Y(%d,%d,%d)",blockIdx.x,blockDim.x,threadIdx.x, blockIdx.y , blockDim.y , threadIdx.y);
				//cout << "Move larger than allowable movement" << endl;
				//continue;
				return 0;

			}

						
				//cout << "Tap!!" << endl;
			}
			printf("Tap by X(%d,%d,%d) Y(%d,%d,%d)",blockIdx.x,blockDim.x,threadIdx.x, blockIdx.y , blockDim.y , threadIdx.y);
			return 1;
			
		} else {
			printf("Fail No Track by X(%d,%d,%d) Y(%d,%d,%d)",blockIdx.x,blockDim.x,threadIdx.x, blockIdx.y , blockDim.y , threadIdx.y);
			//cout << "No track" << endl;
			return 0;
		}
		//return 0;
}





__global__ void FilterOneTask(double* Device_HoldDurationArray,double* Device_IgnoreRepeatArray,double*Device_TapAssistanceArray,double* Device_TimeStamp_AllTracks_InEachTrial,double* Device_PositionX_AllTracks_InEachTrial,double* Device_PositionY_AllTracks_InEachTrial,double* Device_SuccessRateArray,int MaxPoint_InTracks,int MaxTrack_InTrials,double*Device_TrialTargetInfo_InEachTrial){

	printf("This is Thread: X(%d,%d,%d) Y(%d,%d,%d) \n ",blockIdx.x,blockDim.x,threadIdx.x, blockIdx.y , blockDim.y , threadIdx.y);
	for(int i=0;i<2;i++){
		for(int j=0;j<20;j++){
			printf("MyTest ElementExist(%d,%d,%d)\n",i,j,ElementExist(Device_TimeStamp_AllTracks_InEachTrial, IndexTo1D(i,j,MaxPoint_InTracks)));	
		}
	}
	if(threadIdx.x==2){	
		for(int i =0;i<MaxTrack_InTrials*MaxPoint_InTracks;i++){
		printf("InputData:Track: %d ,point: %d (%f,%f,%f)\n",i/MaxPoint_InTracks,i%MaxPoint_InTracks,Device_PositionX_AllTracks_InEachTrial[i],Device_PositionY_AllTracks_InEachTrial[i],Device_TimeStamp_AllTracks_InEachTrial[i]);
	}
	}
	
	//MyTest();
	int* Filtered_Index_AllTracks_InEachTrial = new int (MaxPoint_InTracks*MaxTrack_InTrials);
	int ThreadNum=8;
	int* Filtered_Index_AllTracks_InEachTrial_AllThread = new int (MaxPoint_InTracks*MaxTrack_InTrials*ThreadNum);
	


	int* Filtered_TouchPointIndex_InEachTrack=new int(MaxTrack_InTrials);



	int* trackIndexArr = new int (MaxTrack_InTrials);
	bool* trackEndedArr = new bool (MaxTrack_InTrials);
	bool* trackTouchedArr = new bool (MaxTrack_InTrials);
		
	for(int i=0;i<MaxPoint_InTracks*MaxTrack_InTrials;i++){
		//aaaa[i]=-1;
		Filtered_Index_AllTracks_InEachTrial[i]=0;
		if(Device_TimeStamp_AllTracks_InEachTrial[i]==-1){
			Filtered_Index_AllTracks_InEachTrial[i]=-1;
		}
		

	}


	for(int i=0;i<MaxTrack_InTrials;i++){
		Filtered_TouchPointIndex_InEachTrack[i]=0;
		trackIndexArr[i]=0;
		trackEndedArr[i]=false;
		trackTouchedArr [i]=false;	
	}


	int threadId=threadIdx.x;
	int IgnoreRepeatSize=5,HoldDurationSize=5;	

	int TA_index=threadId/(IgnoreRepeatSize*HoldDurationSize);
	int IG_HD_index=threadId%(IgnoreRepeatSize*HoldDurationSize);
	int HD_index=IG_HD_index/(IgnoreRepeatSize);
	int IG_index=IG_HD_index%(IgnoreRepeatSize);

	

	double ignoreRepeatTimer=Device_IgnoreRepeatArray[IG_index];
	double holdDurationTimer=Device_HoldDurationArray[HD_index];
	double tapAssistanceTimer=Device_TapAssistanceArray[TA_index];
	//printf("Thread:(%d) Parameter:(%f,%f,%f)  \n",threadId,ignoreRepeatTimer,holdDurationTimer,tapAssistanceTimer);
	int index=2+0*MaxPoint_InTracks;
	
	double rawdata=Device_PositionX_AllTracks_InEachTrial[index];
	double modData=Device_PositionX_AllTracks_InEachTrial[index]+holdDurationTimer;

		//	printf("Thread:(%d) Parameter:(%f,%f,%f)   Raw: %f Mod: %f\n",threadId,ignoreRepeatTimer,holdDurationTimer,tapAssistanceTimer,rawdata,modData);

		Device_SuccessRateArray[threadId]=modData;

		// initialization
		//vector<int> trackIndexArr(MaxTrack_InTrials, 0);         //記錄每個track 的touch index (filter 後 哪個touch index開始算) [track1.touchindex,track2.touchindex,...]
		//vector<bool> trackEndedArr(MaxTrack_InTrials, false);     //記錄每個track 的touch EndedState  [track1.touchied,track2.touched,...]
		//vector<bool> trackTouchedArr(MaxTrack_InTrials, false);
		
		//int trackIndexArr[MaxTrack_InTrials]={0};         //記錄每個track 的touch index (filter 後 哪個touch index開始算) [track1.touchindex,track2.touchindex,...]
		//bool trackEndedArr[MaxTrack_InTrials]={false};     //記錄每個track 的touch EndedState  [track1.touchied,track2.touched,...]
		//bool trackTouchedArr[MaxTrack_InTrials]={false};
			
		//cout << trackIndexArr.size() << endl;

		// Constant
		double TapAssistanceThreshold=42.0;
		double TapAssistanceFinalLocationThreshold = 42.0; // unit: pt

                int tapAssistance=2;
		// Initialization
		bool isTouched = false;
		double timestampReferenceForHoldDuration = 0.0;
		bool flagInitForTapAssistance = false;
		double timestampReferenceForTapAssistance = 0.0;
		double locationReferenceForInitTapAssistnace[2] = {0, 0};
		double locationReferenceForFinalTapAssistance[2] = {0, 0};
		double timestampReferenceForIgnoreRepeat =-999999999999999;//std::numeric_limits<double>::max();//-limits.numeric_limits<double>::infinity();
		bool PointPass_IG=false;
		bool PointPass_HD=false;

		struct TouchPoint{
				int tracknum;
				int pointnum_in_track;
				double timestamp;
				double positionX;
				double positionY;
		}ReferenceTouchPointInEachTrack;




		while (MyFind(Filtered_Index_AllTracks_InEachTrial,MaxPoint_InTracks*MaxTrack_InTrials, 0) !=-1){   //如果有任一track 還沒ended 繼續跑
			printf("=========================\n");
			printf("如果有任一track 還沒ended 繼續跑, MyFind: %d by Thread %d\n",MyFind(trackEndedArr,MaxTrack_InTrials, false),threadIdx.x);

			//找出最早觸摸銀幕的track,最小time stamp{
			double timestampNext = 99999999999999999999;//std::numeric_limits<double>::max;//limits.numeric_limits<double>::infinity();
			int trackNext;
			for (int trackIndex = 0 ; trackIndex < MaxTrack_InTrials ; trackIndex++){
				for (int pointIndex=0;pointIndex<MaxPoint_InTracks;pointIndex++){
					if(Filtered_Index_AllTracks_InEachTrial[IndexTo1D(trackIndex,pointIndex,MaxPoint_InTracks)]==0){
					//還沒經過檢查的點才可以成為下一個參考點 （0:表沒檢查。１表示要保存。-1 要清除的點）
						if(ElementExist(Device_TimeStamp_AllTracks_InEachTrial, IndexTo1D(trackIndex,pointIndex,MaxPoint_InTracks))){
							
							if (!trackEndedArr[trackIndex] && Device_TimeStamp_AllTracks_InEachTrial[IndexTo1D(trackIndex,pointIndex,MaxPoint_InTracks)] < timestampNext){
								timestampNext = Device_TimeStamp_AllTracks_InEachTrial[IndexTo1D(trackIndex,pointIndex,MaxPoint_InTracks)];
								trackNext = trackIndex;
								trackIndexArr[trackIndex]=pointIndex;

							}
						}
					}
				}
			}

			printf("找出最早觸摸銀幕的track,最小time stamp (time,track): (%f , %d)\n",timestampNext,trackNext);
			
			//}找出最早觸摸銀幕的track,最小time stamp

            for(int i=0;i<MaxTrack_InTrials*MaxPoint_InTracks;i++){
			//printf("Data:(%f,%f,%f)\n",Device_PositionX_AllTracks_InEachTrial[i],Device_PositionY_AllTracks_InEachTrial[i],Device_TimeStamp_AllTracks_InEachTrial[i]);
			}

			if(ElementExist(Device_TimeStamp_AllTracks_InEachTrial, IndexTo1D(trackNext,trackIndexArr[trackNext],MaxPoint_InTracks))){
			ReferenceTouchPointInEachTrack.tracknum=trackNext;
			ReferenceTouchPointInEachTrack.pointnum_in_track=trackIndexArr[trackNext];
			ReferenceTouchPointInEachTrack.positionX=Device_PositionX_AllTracks_InEachTrial[IndexTo1D(trackNext,trackIndexArr[trackNext],MaxPoint_InTracks)];
			ReferenceTouchPointInEachTrack.positionY=Device_PositionY_AllTracks_InEachTrial[IndexTo1D(trackNext,trackIndexArr[trackNext],MaxPoint_InTracks)];
			ReferenceTouchPointInEachTrack.timestamp=Device_TimeStamp_AllTracks_InEachTrial[IndexTo1D(trackNext,trackIndexArr[trackNext],MaxPoint_InTracks)];

			timestampReferenceForIgnoreRepeat = ReferenceTouchPointInEachTrack.timestamp;
			timestampReferenceForHoldDuration = ReferenceTouchPointInEachTrack.timestamp;
			locationReferenceForInitTapAssistnace[0] = ReferenceTouchPointInEachTrack.positionX;
			locationReferenceForInitTapAssistnace[1] = ReferenceTouchPointInEachTrack.positionY;

			}

		


			////// IMPORTANT //////
			// IGNORE REPEAT //
		
				//如果現在這個要測量的點時間小於參考點時間加上ignore repeattimer則忽略掉這個測量的點
				for (int i=ReferenceTouchPointInEachTrack.pointnum_in_track;i<MaxPoint_InTracks;i++){
					if(Device_TimeStamp_AllTracks_InEachTrial[IndexTo1D(trackNext,i,MaxPoint_InTracks)]<timestampReferenceForIgnoreRepeat + ignoreRepeatTimer)
					{	
					printf("忽略掉 在track %d 中的至第 %d 個點\n",trackNext,trackIndexArr[trackNext]);	
					Filtered_Index_AllTracks_InEachTrial[IndexTo1D(trackNext,trackIndexArr[trackNext],MaxPoint_InTracks)]=false;			
					}
					else
					{
						Filtered_Index_AllTracks_InEachTrial[IndexTo1D(trackNext,trackIndexArr[trackNext],MaxPoint_InTracks)]=true;
					}

				}
			



			// HOLD DURATION //  與// TapAssistence// 必須一起看
			
			


			//Hold duration : 如果這個track的最後一個timestamp沒有超出 hold duration 持續時間 整個track清掉
			
	//TapAssistance:在時間threshold內 (沒超出一定範圍)：點變成是“最初”或“最後”的點 (若超出一定範圍 重新計算) 
					      //在時間threahold外(沒超出一定範圍)： 前半變成一個點 後半就是各個點都要算
						//與hold duration 衝突： 1 time<HD 內：什麼都沒有 2.HD<time<TA+HD :點變成是“最初”或“最後”的點 (若超出一定範圍 重新計算) 3.TA+HD<time:前半變成一個點 後半就是各個點都要算

			int TapAssistance_ReferenceIndex=-1;
			for(int i=ReferenceTouchPointInEachTrack.pointnum_in_track;i<MaxPoint_InTracks;i++){
				double checkpoint_timestamp=Device_TimeStamp_AllTracks_InEachTrial[IndexTo1D(trackNext,i,MaxPoint_InTracks)];
		  		if (checkpoint_timestamp<timestampReferenceForHoldDuration + holdDurationTimer){
		  		
		  			Filtered_Index_AllTracks_InEachTrial[IndexTo1D(trackNext,i,MaxPoint_InTracks)]=false;	
		  			printf("忽略掉 在track %d 中的至第 %d 個點\n",trackNext,i);			
				
				}
				else if(checkpoint_timestamp>timestampReferenceForHoldDuration + holdDurationTimer&&checkpoint_timestamp<timestampReferenceForHoldDuration + holdDurationTimer+tapAssistanceTimer){
					if(tapAssistance=1){ //使用最初觸摸點

						Filtered_Index_AllTracks_InEachTrial[IndexTo1D(trackNext,i,MaxPoint_InTracks)]=false;

						if(TapAssistance_ReferenceIndex=-1){
							TapAssistance_ReferenceIndex=i;
							Filtered_Index_AllTracks_InEachTrial[IndexTo1D(trackNext,i,MaxPoint_InTracks)]=true;

						}		
					}
					else if(tapAssistance=2){//使用最後觸摸點
						Filtered_Index_AllTracks_InEachTrial[IndexTo1D(trackNext,i,MaxPoint_InTracks)]=false;
						TapAssistance_ReferenceIndex=i;

					}
					else{
						TapAssistance_ReferenceIndex=ReferenceTouchPointInEachTrack.pointnum_in_track
						Filtered_Index_AllTracks_InEachTrial[IndexTo1D(trackNext,i,MaxPoint_InTracks)]=true;

					}


				}
				else{
					Filtered_Index_AllTracks_InEachTrial[IndexTo1D(trackNext,i,MaxPoint_InTracks)]=true;
				}


			}
			if (TapAssistance_ReferenceIndex!=-1||TapAssistance_ReferenceIndex!=ReferenceTouchPointInEachTrack.pointnum_in_track){Filtered_Index_AllTracks_InEachTrial[IndexTo1D(trackNext,TapAssistance_ReferenceIndex,MaxPoint_InTracks)]=true;}
		} //while end

		//ignore repeated //
		//ignore repeated: 一但有point 成功true了過後 當每個個成功的track point "完全離開螢幕" 後才會開始忽略在其他 track的touch point
		double LastTrueTime=0;

		///sort Device timestamp

		for (int pointIndex=0;pointIndex<MaxPoint_InTracks;pointIndex++){
			for (int trackIndex = 0 ; trackIndex < MaxTrack_InTrials ; trackIndex++){

					//這個track 從 1變-1 表 這個track已離開 看看這個時間點有沒有其他track 狀態是1 有的話:系統會繼續追蹤那個還在狀態1的track 沒有的話：啟動ignore repeat
					if(Filtered_Index_AllTracks_InEachTrial[IndexTo1D(trackIndex,pointIndex,MaxPoint_InTracks)]==1){
						if(Filtered_Index_AllTracks_InEachTrial[IndexTo1D(trackIndex,pointIndex+1,MaxPoint_InTracks)]==-1){

							if(LastTrueTime<Device_TimeStamp_AllTracks_InEachTrial[IndexTo1D(trackIndex,pointIndex,MaxPoint_InTracks)]){
								LastTrueTime=Device_TimeStamp_AllTracks_InEachTrial[IndexTo1D(trackIndex,pointIndex,MaxPoint_InTracks)]


							}
						}
					}


			}

		}










       
	//Filtered_TimeStamp_AllTracks_InEachTrial[0]=threadIdx.x+100;
	int Success=TapRecognizer(Device_TimeStamp_AllTracks_InEachTrial,Device_PositionX_AllTracks_InEachTrial ,Device_PositionY_AllTracks_InEachTrial,MaxPoint_InTracks,MaxTrack_InTrials,Device_TrialTargetInfo_InEachTrial,Filtered_Index_AllTracks_InEachTrial);



	//	int Success=-1;

		//printf("Thread:(%d) Parameter:(%f,%f,%f)  Success %d\n",threadId,ignoreRepeatTimer,holdDurationTimer,tapAssistanceTimer,Success);


}









extern "C" void ParallelComputeMainFuction(ofstream* ofs, vector<char*> *tasks){
 
int rawdatatypecount = TapTaskStruct.RawTouchDataArray[0][0][0].size();
int TargetInfoSize_InEachTrial=TapTaskStruct.TaskTargetDataArray[0].size();
int Trials_Num=TapTaskStruct.RawTouchDataArray.size();
int parametersize=3;
int   data_size = parametersize*parametersize*parametersize;

	double *HoldDurationArray = new double[data_size],
	      *IgnoreRepeatArray = new double[data_size],
	      *TapAssistanceArray = new double[data_size],
	      *SuccessRateArray = new double[data_size];

	 double *TargetInfoInTrial = new double[TargetInfoSize_InEachTrial]; //Tap: location,size  Swipe: Angle


 HoldDurationArray[0] = 0;
 IgnoreRepeatArray[0] = 0;
 TapAssistanceArray[0]=0;
	for( int i = 1; i < parametersize; i++ )
	{
	    HoldDurationArray[i] = HoldDurationArray[i-1]+1;
	    IgnoreRepeatArray[i] = IgnoreRepeatArray[i-1]+1;
	    TapAssistanceArray[i]=TapAssistanceArray[i-1]+1;
	}







    //parallel program initialize	
	int Parameter_Size_Float = data_size *sizeof(double);
	double *Device_HoldDurationArray,*Device_IgnoreRepeatArray,*Device_TapAssistanceArray,*Device_SuccessRateArray;

    cudaMalloc((void**)&Device_HoldDurationArray, Parameter_Size_Float);
	cudaMalloc((void**)&Device_IgnoreRepeatArray, Parameter_Size_Float);
	cudaMalloc((void**)&Device_TapAssistanceArray, Parameter_Size_Float);
	cudaMalloc((void**)&Device_SuccessRateArray, Parameter_Size_Float);
	
	
	cudaMemcpy(Device_HoldDurationArray,HoldDurationArray,Parameter_Size_Float ,cudaMemcpyHostToDevice);
	cudaMemcpy(Device_IgnoreRepeatArray,IgnoreRepeatArray,Parameter_Size_Float ,cudaMemcpyHostToDevice);
	cudaMemcpy(Device_TapAssistanceArray,TapAssistanceArray,Parameter_Size_Float ,cudaMemcpyHostToDevice);
	cudaMemcpy(Device_SuccessRateArray,SuccessRateArray,Parameter_Size_Float ,cudaMemcpyHostToDevice);

  



    /////Device Data Array/////
  
  //Find Max Length in each dimeansion
 

  int MaxTrack_InTrials=1;
  int MaxPoint_InTracks=1;
/*  
for(int i=0; i<Trials_Num;i++){
		if(TapTaskStruct.RawTouchDataArray[i].size()>MaxTrack_InTrials){MaxTrack_InTrials=TapTaskStruct.RawTouchDataArray[i].size();}
		for(int j=0;j<TapTaskStruct.RawTouchDataArray[i].size();j++){
			if(TapTaskStruct.RawTouchDataArray[i][j].size()>MaxPoint_InTracks){MaxPoint_InTracks=TapTaskStruct.RawTouchDataArray[i][j].size();}

		}

  }
*/
  
  // Transfer data to 1-D array per each trial: 
  // Ex: Time:[track1(point1.time,point2.time,...,pointN.time),track2(point1.time,point2.time,...,pointN.time),...,trackN(point1.time,point2.time,...,pointN.time)]  
int Trial_Index=0; ///

for(int Trial_Index=15;Trial_Index<16;Trial_Index++){
	 
	 
	MaxTrack_InTrials=TapTaskStruct.RawTouchDataArray[Trial_Index].size();
	MaxPoint_InTracks=1;
		for(int j=0;j<TapTaskStruct.RawTouchDataArray[Trial_Index].size();j++){
			if(TapTaskStruct.RawTouchDataArray[Trial_Index][j].size()>MaxPoint_InTracks){MaxPoint_InTracks=TapTaskStruct.RawTouchDataArray[Trial_Index][j].size();}

		}




	 for(int info=0;info<TapTaskStruct.TaskTargetDataArray[Trial_Index].size();info++){
	 	TargetInfoInTrial[info]= TapTaskStruct.TaskTargetDataArray[Trial_Index][info];
	//TargetInfoInTrial[info]=20.21;	 
}



	int RawDataSize = MaxTrack_InTrials*MaxPoint_InTracks;

	double *Host_TimeStamp_AllTracks_InEachTrial = new double[ RawDataSize],
	      *Host_PositionX_AllTracks_InEachTrial = new double[ RawDataSize],
	      *Host_PositionY_AllTracks_InEachTrial = new double[ RawDataSize];
	    

	for( int i = 0; i < RawDataSize; i++ )
	{
	   	  int WhichTracks=i/MaxPoint_InTracks;
	   	  int WhichPoints=i%MaxPoint_InTracks;
	   	  Host_TimeStamp_AllTracks_InEachTrial[i] = -1,
	      Host_PositionX_AllTracks_InEachTrial[i]= -1,
	      Host_PositionY_AllTracks_InEachTrial[i] = -1;
		 //cout<<"MaxPoint:"<<MaxPoint_InTracks<<"Track: "<<WhichTracks<<"Point: "<<WhichPoints<<"TrialSize: "<<TapTaskStruct.RawTouchDataArray[Trial_Index].size()<<"TrackSize: "<<TapTaskStruct.RawTouchDataArray[Trial_Index][WhichTracks].size()<<endl;
	     
	      if (WhichTracks<TapTaskStruct.RawTouchDataArray[Trial_Index].size()){
	      	if(WhichPoints<TapTaskStruct.RawTouchDataArray[Trial_Index][WhichTracks].size()){
	      		 Host_TimeStamp_AllTracks_InEachTrial[i] = TapTaskStruct.RawTouchDataArray[Trial_Index][WhichTracks][WhichPoints][0];
	      		Host_PositionX_AllTracks_InEachTrial[i]= TapTaskStruct.RawTouchDataArray[Trial_Index][WhichTracks][WhichPoints][1];
	      		Host_PositionY_AllTracks_InEachTrial[i] = TapTaskStruct.RawTouchDataArray[Trial_Index][WhichTracks][WhichPoints][2];

	      	}


	      }

		   cout<<"Host Data ("<<WhichTracks<<","<<WhichPoints<<"): "<<Host_PositionX_AllTracks_InEachTrial[i]<<" "<<Host_PositionY_AllTracks_InEachTrial[i]<< " "<<Host_TimeStamp_AllTracks_InEachTrial[i] <<endl;


	}


	int RawDataSize_Double = RawDataSize *sizeof(double);
	double *Device_TimeStamp_AllTracks_InEachTrial ,
	      *Device_PositionX_AllTracks_InEachTrial ,
	      *Device_PositionY_AllTracks_InEachTrial ,
	      *Device_TrialTargetInfo_InEachTrial ;

    cudaMalloc((void**)&Device_TimeStamp_AllTracks_InEachTrial, RawDataSize_Double);
	cudaMalloc((void**)&Device_PositionX_AllTracks_InEachTrial, RawDataSize_Double);
	cudaMalloc((void**)&Device_PositionY_AllTracks_InEachTrial, RawDataSize_Double);
	cudaMalloc((void**)&Device_TrialTargetInfo_InEachTrial, TargetInfoSize_InEachTrial*sizeof(double));
	
	
	cudaMemcpy(Device_TimeStamp_AllTracks_InEachTrial,Host_TimeStamp_AllTracks_InEachTrial,RawDataSize_Double  ,cudaMemcpyHostToDevice);
	cudaMemcpy(Device_PositionX_AllTracks_InEachTrial,Host_PositionX_AllTracks_InEachTrial,RawDataSize_Double  ,cudaMemcpyHostToDevice);
	cudaMemcpy(Device_PositionY_AllTracks_InEachTrial,Host_PositionY_AllTracks_InEachTrial,RawDataSize_Double ,cudaMemcpyHostToDevice);
	cudaMemcpy(Device_TrialTargetInfo_InEachTrial,TargetInfoInTrial,TargetInfoSize_InEachTrial*sizeof(double),cudaMemcpyHostToDevice);



  
	//parallel program initialize

	//dim3 dimBlock(data_size,data_size);
	//dim3 dimGrid((data_size+dimBlock.x-1)/(dimBlock.x), (data_size+dimBlock.y-1)/(dimBlock.y));
   
	//FilterOneTask<<<dimGrid, dimBlock>>>(Device_HoldDurationArray,Device_IgnoreRepeatArray,Device_TapAssistanceArray,Device_TimeStamp_AllTracks_InEachTrial,Device_PositionX_AllTracks_InEachTrial,Device_PositionY_AllTracks_InEachTrial,Device_SuccessRateArray,MaxPoint_InTracks,MaxTrack_InTrials);

   	//FilterOneTask<<<1,data_size>>>(Device_HoldDurationArray,Device_IgnoreRepeatArray,Device_TapAssistanceArray,Device_TimeStamp_AllTracks_InEachTrial,Device_PositionX_AllTracks_InEachTrial,Device_PositionY_AllTracks_InEachTrial,Device_SuccessRateArray);
	FilterOneTask<<<1,data_size>>>(Device_HoldDurationArray,Device_IgnoreRepeatArray,Device_TapAssistanceArray,Device_TimeStamp_AllTracks_InEachTrial,Device_PositionX_AllTracks_InEachTrial,Device_PositionY_AllTracks_InEachTrial,Device_SuccessRateArray,MaxPoint_InTracks,MaxTrack_InTrials,Device_TrialTargetInfo_InEachTrial);

	
	cudaMemcpy(SuccessRateArray,Device_SuccessRateArray, Parameter_Size_Float, cudaMemcpyDeviceToHost);
//	cudaDeviceSynchronize();    
    for( int i = 0; i < data_size; i++ )
	{
        int IgnoreRepeatSize=5;
		int HoldDurationSize=5;
		int threadId=i;

		int	TA_index=threadId/(IgnoreRepeatSize*HoldDurationSize);
		int IG_HD_index=threadId%(IgnoreRepeatSize*HoldDurationSize);
		int HD_index=IG_HD_index/(IgnoreRepeatSize);
		int IG_index=IG_HD_index%(IgnoreRepeatSize);
		//cout<<"Host: "<<SuccessRateArray[i]<<"  ("<< HoldDurationArray[HD_index]<<","<<IgnoreRepeatArray[IG_index] <<","<<TapAssistanceArray[TA_index]<<")"<<endl;
	   
	}
 

	cudaFree(Device_HoldDurationArray);
    cudaFree(Device_IgnoreRepeatArray);
    cudaFree(Device_TapAssistanceArray);
    cudaFree(Device_TimeStamp_AllTracks_InEachTrial);
    cudaFree(Device_PositionX_AllTracks_InEachTrial);
    cudaFree(Device_PositionY_AllTracks_InEachTrial);
    cudaFree(Device_TrialTargetInfo_InEachTrial);
	
	//cudaDeviceSychronize();
}

}



