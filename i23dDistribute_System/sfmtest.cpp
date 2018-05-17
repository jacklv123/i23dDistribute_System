//#include"../i23dSFM/i23dSFM/SFMAllSteps/SFMAllSteps.h"
//#include"../i23dSFM/i23dSFM/mpi/sfmmodel.h"
#include"../i23dMVS/apps/MVSAllSteps/MVSAllSteps.h"
//using namespace i23dSFM;
int main(){
	//SFMRun("/edata/lyb/data3/0","/edata/lyb/data3/recon/0","/edata/lyb/i23dDistribute_System/i23dDistribute_System/../i23dSFM/i23dSFM/exif/sensor_width_database/sensor_width_camera_database.txt");
	MVSRun("/edata/lyb/data3/recon/0/reconstruction_sequential","/edata/lyb/data3/recon/0/mvs");
}
	