//
// Created by haibao637 on 17-11-4.
//

#ifndef I23DSFM_MY_MPI_H
#define I23DSFM_MY_MPI_H
#include<mpi.h>
#include<sstream>
#include"../i23dMVS/apps/MVSAllSteps/MVSAllSteps.h"
//#include"../i23dSFM/i23dSFM/SFMAllSteps/SFMAllSteps.h"
#include"../i23dSFM/i23dSFM/mpi/sfmmodel.h"
#include<deque>
#include <memory>

#define SERVER_RANK 0
namespace i23dSFM{
    namespace mpi{
        std::string int2string(int n){
		stringstream ss;
		ss<<n;
		return ss.str();
	}
        enum Task{FINISH,START};
        enum {TASK,CLUSTER,THREAD_ID,PAIR_SIZE,PAIRS,STATUS};
        class Server{
        public:
            unique_ptr<MPI_Request> recv_thread_requests;
            unique_ptr<MPI_Status> recv_thread_statuses;
            const int server_id=0;
            deque<int> thread_pool;
            Server(){
            }
	    void init(int World_size){
		world_size=World_size;
		resetThreadPool();
                recv_thread_requests.reset(new MPI_Request[thread_pool.size()]);
                recv_thread_statuses.reset(new MPI_Status[thread_pool.size()]);
	   }
		
            void resetThreadPool(){
                thread_pool.clear();
                for (int i = 1; i < world_size; ++i) {
                    thread_pool.push_back(i);
                }
            }
            void Finishall(){
                for (int i = 1; i < world_size; ++i) {
                     int task=0;
                     MPI_Send(&task, 1, MPI_INT, i, TASK, MPI_COMM_WORLD);

                }
            }
            void Finish(int finished,int current_thread){

                MPI_Send(&finished, 1, MPI_INT, current_thread, TASK, MPI_COMM_WORLD);

            }

            int doTask(int cluster_id,Task task){
               
                if (thread_pool.empty()) {
                    //cout << "等待空闲线程" << endl;
//
                    int response = 0;
                    MPI_Waitany(world_size - 1, recv_thread_requests.get(), &response,
                                recv_thread_statuses.get());

                    //unsigned  recover=recv_thread_statuses.get()[response].MPI_SOURCE;
                    //cout << response << " 回收的线�?" << (response + 1) << endl;

                    thread_pool.push_back((response + 1));


                }
                int current_thread;
                if (!thread_pool.empty()) {
                    cout << "线程池状态：";
                    for (auto x: thread_pool) {
                        cout << x << " ";
                    }
                    //cout << endl;
                    current_thread= thread_pool.front();
                    thread_pool.pop_front();
                     MPI_Send(&task,1,MPI_INT,current_thread,TASK,MPI_COMM_WORLD);
                   // void (*func)(Content,int)=(void (Content,int))funcs.at(task_id);
                    if(task!=FINISH){
                        MPI_Send(&cluster_id,1,MPI_INT,current_thread,CLUSTER,MPI_COMM_WORLD);
                   
					
                		    	MPI_Irecv(NULL, 0, MPI_UNSIGNED, current_thread, THREAD_ID, MPI_COMM_WORLD,
                              			&(recv_thread_requests.get()[current_thread - 1]));

 		  }

                }
                //cout<<"当前空闲进程:";
                //for (auto x: thread_pool) {
                //    cout << x << " ";
                //}

                return current_thread;
            }
        private :
            int world_size;
        };
        class Client{
        public:
	    int cluster_id;
	    std::string ImageDirBase;
	        std::string OutputDir;
	        std::string sensorFileDatabase;
	   
	   Client(){
		
	      }
	   void init(std::string ImageDirBase_,std::string OutputDir_,std::string sensorFileDatabase_){
		ImageDirBase=ImageDirBase_;
		sensorFileDatabase=sensorFileDatabase_;
		OutputDir=OutputDir_;
	   }
/*
            Pair_Set loadPair(string path){
                Pair_Set pairs;
                if (!stlplus::file_exists(path))
                {

                    std::cerr << "Cannot open  directory:"<<path << std::endl;
                    return pairs;

                }
                string initial_pairs=path+"/points.json";
                ifstream ifs(initial_pairs);
                cereal::JSONInputArchive oar(ifs);
                oar(cereal::make_nvp("pairs",pairs));
                return pairs;

            }
*/
            bool doTask(){
                MPI_Status status;
                Task task;
                MPI_Recv(&task, 1, MPI_INT, SERVER_RANK, TASK, MPI_COMM_WORLD, &status);
                if(task==FINISH){
                    return false;
                }

                MPI_Recv(&cluster_id, 1, MPI_INT, SERVER_RANK, CLUSTER, MPI_COMM_WORLD, &status);
                
				//pipeline.loadPairs();
				//pipeline.ComputeFeatures();
				//pipeline.ComputeMatches();
				//pipeline.IncrementalSfM();
				SFMRun(ImageDirBase+int2string(cluster_id),OutputDir+"/"+int2string(cluster_id),sensorFileDatabase);
				printf("cluster %d SFM is finished,start MVS\n",cluster_id);
				//MVSRun(pipeline.partial_reconstrucionDir,pipeline.MVS_folder,pipeline.images_);
				
				MPI_Send( NULL,0, MPI_UNSIGNED, SERVER_RANK, THREAD_ID, MPI_COMM_WORLD);		
				return true;
            }
        };
    }
}

#endif //I23DSFM_MY_MPI_H
