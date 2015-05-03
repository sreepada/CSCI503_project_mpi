#include<mpi.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<list>

using namespace std;

int main(int argc, char*argv[]){
	MPI_Init(&argc, &argv);
	int nprocs,rank;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int trainingCompleted=0;
	if (rank==0)
	{	FILE *fp=fopen("allinputs.csv","r");
		char * buf=(char*)malloc(sizeof(char)*1024);
		int i=0;
		int sendtorank=1;
		while(fgets(buf,1024,fp)!=NULL)
		{	strtok(buf,"\n");
			strcat(buf,"\0");
			printf("buf is %s and strlen is %d\n",buf,(int)strlen(buf));
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

		//Training code and writing trained model to file
		trainingCompleted=1;
	}
	else{
		char stopMsg[]="STOP";
		MPI_Status status;
		int count=0;
		list<string> fileList;
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
//				printf("buf at rank %d = %s end of buf with strlen %d but count= %d\n",rank, buf,(int)strlen(buf),count);
		}
        for (list<string>::iterator it = fileList.begin(); it != fileList.end(); it++)
        {
                //Face Detection and cropping code
        }
	}

    MPI_Bcast(&trainingCompleted, 1, MPI_INT, 0, MPI_COMM_WORLD );
    //printf("training completed by rank %d=%d\n",rank,trainingCompleted);

    //Face recognition code here

	MPI_Finalize();
return 0;
}
