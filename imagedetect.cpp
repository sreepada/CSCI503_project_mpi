#include<mpi.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<list>
#include <sys/stat.h>
#include "detectAndRecog.cpp"
#include <opencv2/core/core.hpp>
#include <opencv2/contrib/contrib.hpp>
#include "trainer.cpp"
//mpic++ -o imagedetect imagedetect.cpp facedetect.h facedetect-test.cpp `pkg-config --libs opencv`
//mpirun -np 10 ./imagedetect
using namespace std;

int main(int argc, char*argv[]){
	MPI_Init(&argc, &argv);
	double start_time,end_time;
	int nprocs,rank;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int trainingCompleted=0;
	if (rank==0)
	{	
		start_time=MPI_Wtime();
		someMain("train.csv", "test.csv", "att.csv");
		FILE *fp=fopen("test.csv","r");
		char * buf=(char*)malloc(sizeof(char)*1024);
		int i=0;
		int sendtorank=1;
		while(fgets(buf,1024,fp)!=NULL)
		{	strtok(buf,"\n");
			strcat(buf,"\0");
			MPI_Send(buf, strlen(buf), MPI_CHAR, sendtorank, sendtorank,MPI_COMM_WORLD);
			sendtorank++;
			if(sendtorank==nprocs)
				sendtorank=1;
		}
		char stopMsg[]="STOP";
		for(i=1;i<nprocs;i++){
			MPI_Send(stopMsg, strlen(stopMsg), MPI_CHAR, i, i,MPI_COMM_WORLD);
		}
		fclose(fp);
	/*int k=0;
	list<string>
	for(k=0;k<nprocs;k++)
	{
			MPI_Status status;
			MPI_Probe(MPI_ANY_SOURCE,MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			MPI_Get_count(&status, MPI_CHAR, &count);
			char buf[count+1];
			memset(buf,'\0',sizeof(char)*(count+1));
			MPI_Recv(buf, count, MPI_CHAR, 0, rank,MPI_COMM_WORLD, &status);
	}*/
  }
	else{

		char stopMsg[]="STOP";
		MPI_Status status;
		int count=0;
        list<string> fileList;
		vector<string> suspectImages;
		while(1){
			MPI_Probe(0, rank, MPI_COMM_WORLD, &status);
			MPI_Get_count(&status, MPI_CHAR, &count);
			char buf[count+1];
			memset(buf,'\0',sizeof(char)*(count+1));
			MPI_Recv(buf, count, MPI_CHAR, 0, rank,MPI_COMM_WORLD, &status);
			strcat(buf,"\0");
			if (strcmp(buf,stopMsg)==0)
				break;
			else
				fileList.push_back(buf);
        }
		int sum=0;
	
		for (list<string>::iterator it = fileList.begin(); it != fileList.end(); it++)
        {
                const char *temppassString=(*it).c_str();
				char *passString=strdup(temppassString);
				char* path=strtok(passString,";");
				vector<int> croppedLabel;
		        std::vector<Mat> croppedImages=cropFaces(path);
				path=strtok(NULL,";");
				for(int i=0; i < croppedImages.size(); i++) {
					croppedLabel.push_back(atoi(path));
				}  
				int found=recognizeSuspect(croppedImages, croppedLabel, "eigenfaces_at.yml");
				if(found==1)
				{
					suspectImages.push_back(passString);
					printf("Suspect found in image %s\n",passString);
				}
        }
	/*	for ( int i= 0; i<suspectImages.size(); i++)
		{
			sum += suspectImages[i].size();
		}
		MPI_Send(&suspectImages,sum,MPI_CHAR,0,rank,MPI_COMM_WORLD);*/
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if(rank==0)
	{
		end_time=MPI_Wtime();
		printf("Time elapsed %f\n",end_time-start_time);
	}
	MPI_Finalize();
return 0;
}
