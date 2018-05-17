#include"mpi.h"
//#include"util.h"
#include "../i23dSFM/third_party/cmdLine/cmdLine.h"
#include"../i23dSFM/i23dSFM/system/timer.hpp"
#include"my_mpi.h"
#include <sys/stat.h>
using namespace std;
#include<fstream>
using namespace i23dSFM;
using namespace i23dSFM::mpi;
int boundary_points;
int spectral_initialization;
int memory_saving; // forbid using local search or empty cluster removing
int cutType; //cut type, default is normalized cut
Server server;
Client client;
int main(int argc,char* argv[]){
     umask(0);

    CmdLine cmdLine;
    string image_dir;
    string sensorFileDatabase;
    string output_dir;
    int land_height=180;
    int nparts=1;
    system::Timer timer;
    ofstream Log;
    //MapPairs mapPairs;

    cmdLine.add( make_option('i', image_dir, "imageDirectory") );
    cmdLine.add( make_option('d', sensorFileDatabase, "sensorWidthDatabase") );
    cmdLine.add( make_option('o', output_dir, "outputDirectory") );
    cmdLine.add( make_option('n', nparts, "cluster nums") );
    cmdLine.add(make_option('h', land_height, "local altitude."));
    try {
        if (argc == 1) throw std::string("Invalid command line parameter.");
        cmdLine.process(argc, argv);
    } catch(const std::string& s) {
        std::cerr << "Usage: " << argv[0] << '\n'
                  << "[-i|--imageDirectory]\n"
                  << "[-d|--sensorWidthDatabase]\n"
                  << "[-o|--outputDirectory]\n"
                  << "[-f|--focal] (pixels)\n"
                  << "[-k|--intrinsics] Kmatrix: \"f;0;ppx;0;f;ppy;0;0;1\"\n"
                  << "[-c|--camera_model] Camera model type:\n"
                  << "\t 1: Pinhole\n"
                  << "\t 2: Pinhole radial 1\n"
                  << "\t 3: Pinhole radial 3 (default)\n"
                  << "\t 4: Pinhole brown 2\n"
                  << "[-g|--group_camera_model]\n"
                  << "\t 0-> each view have it's own camera intrinsic parameters,\n"
                  << "\t 1-> (default) view can share some camera intrinsic parameters\n"
                  << "[-e|--exif_gps_info]\n"
                  << std::endl;

        std::cerr << s << std::endl;
        return EXIT_FAILURE;
    }

    MPI_Init(NULL, NULL);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);//得到所有参加运算的进程的个数
    int world_rank=0;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);//得到当前正在运行的进程的标识号

    //SequentialPipeline pipeline(image_dir,sensorFileDatabase,output_dir,world_rank);
    //pipeline.LAND_HEIGHT=land_height;
    server.init(world_size);
    client.init(image_dir,output_dir,sensorFileDatabase);
    //if(world_rank==SERVER_RANK){
    //    pipeline.InitImageListing();
    //    cout<<"root image nums:"<<pipeline.sfm_data.views.size()<<endl;
    //}
    //MPI_Barrier(MPI_COMM_WORLD);

    //if(world_rank!=server.server_id){
    //    if(!Load(pipeline.sfm_data, pipeline.sSfM_Data_Filename, ESfM_Data(VIEWS|INTRINSICS))){
    //        std::cerr << std::endl
    //                  << "The input file \""<< pipeline.sSfM_Data_Filename << "\" cannot be read" << std::endl;
    //    }
    //    cout<<"image nums:"<<pipeline.sfm_data.views.size()<<endl;
   // }
    if(world_rank==server.server_id){
            Log.open(output_dir+"/log.txt");
           timer.reset();
	/*
        {

            mapPairs=pipeline.images_cluster(pipeline.global_graph,nparts);
            pipeline.saveGlobalPairs();
            cout<<"map pairs size:"<<mapPairs.size()<<endl;
        }
	*/
        //for(auto m:mapPairs){
          //  Pair_Set pairs=mapPairs[index];
        //        cout<<"mapPair:"<<m.first<<endl;
        //        server.doTask(m.first,START);
		for (int i=0;i<nparts;i++)
			server.doTask(i,START);

        //}
        server.Finishall();

    }
    if(world_rank!=SERVER_RANK){
        //only start after receiving start sign
        while(client.doTask());
            
        // 新建输出进程，减少io时间

    }
    MPI_Barrier(MPI_COMM_WORLD);
  
    MPI_Finalize();


    return EXIT_SUCCESS;
}
