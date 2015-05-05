#include<mpi.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<list>
#include "facedetect.h"
#include <sys/stat.h>
//mpic++ -o imagedetect imagedetect.cpp facedetect.h facedetect-test.cpp `pkg-config --libs opencv`
//mpirun -np 10 ./imagedetect
using namespace std;

int main(int argc, char*argv[]){
	MPI_Init(&argc, &argv);
	int nprocs,rank;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int trainingCompleted=0;
    if (rank==0)
    {
        int k=0;
        for(k=0;k<nprocs;k++)
        {
            char namebuf[10]="";
            char a[2];
            //printf("File name is1\n");
            snprintf(a,2,"%d",k);
            //printf("File name is2\n");
            strcat(namebuf,a);
            //printf("File name after a=%s is %s\n",a,namebuf);
            strcat(namebuf,".csv");
            //printf("File name is %s\n",namebuf);
            FILE *fptry=fopen(namebuf,"w+");
            fclose(fptry);
        }

    }

	if (rank==0)
	{	FILE *fp=fopen("allinputs.csv","r");
		char * buf=(char*)malloc(sizeof(char)*1024);
		int i=0;
		int sendtorank=1;
		while(fgets(buf,1024,fp)!=NULL)
		{	strtok(buf,"\n");
			strcat(buf,"\0");
			//printf("buf is %s and strlen is %d\n",buf,(int)strlen(buf));
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
        //printf("Test1\n");
        struct stat st = {0};
        char tempdirname[10];
        char combinedDir[10]="./";
        snprintf(tempdirname, sizeof(tempdirname), "%d", rank);
//        strcat(combinedDir,"./");
        strcat(combinedDir,tempdirname);
        if (stat(combinedDir, &st) == -1) {
            mkdir(combinedDir, 0700);
        }
        //printf("Test2\n");
		char stopMsg[]="STOP";
		MPI_Status status;
		int count=0;
		//list<string> fileList;
        strcat(tempdirname,".csv");
        //printf("Tempdirname=%s\n",tempdirname);
        FILE *fp=fopen(tempdirname,"w+");
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
                fputs(buf,fp);
                //fileList.push_back(buf);
				//printf("buf at rank %d = %s end of buf with strlen %d but count= %d\n",rank, buf,(int)strlen(buf),count);
            //printf("Test3\n");
		}
		//printf("Test4\n");
//        memset(tempdirname,0,sizeof(tempdirname));
//        strcpy(tempdirname,combinedDir);

//        for (list<string>::iterator it = fileList.begin(); it != fileList.end(); it++)
//        {
//                const char *passString=(*it).c_str();
////				printf("iter value %s\n",passString);
//                fputs(passString,fp);
//
//        }
//        printf("Test5\n");
        fclose(fp);
        char passFile[10]="./";
        strcat(passFile,tempdirname);

        facedetect(passFile,combinedDir);
        //printf("Test6\n");

	}

    MPI_Bcast(&trainingCompleted, 1, MPI_INT, 0, MPI_COMM_WORLD );
    //printf("training completed by rank %d=%d and extern value is %d\n",rank,trainingCompleted,facedetect(rank,NULL));

    //Face recognition code here

	MPI_Finalize();
return 0;
}
