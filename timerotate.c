#include<stdio.h>
#include<time.h>
#include<unistd.h>
#include<ctype.h>
#include<sys/types.h>
#include<pthread.h>
#include<stdlib.h>
#define MAX_THREAD 16
#define MAX_NO 16
#define dim 256
int index1[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
int filter[3][3] = {{0,1,0},{1,-4,1},{0,1,0}};	
unsigned char buffer[1024][1024];					//to store the image data
unsigned char out_buffer[1024][1024];
int height,width;	
void *noise(void *arg){
	printf("hello");
	int i = *(int*)arg;
	int x,y,j,k,l,p,q,temp;
	int filSum=0;
	x=(i%4)*dim;
	y=(i/4)*dim;
	printf("<THREAD %d> ",i);
	
	for(i=x;i<x+dim;i++)
	{
		for(j=y;j<y+dim;j++)
		{
			
			filSum=0;
			if(i == 0 || j == 0 || i == width-1 || j == height-1){
				out_buffer[i][j] = buffer[i][j];
			}
			else{
				for(k=i-1;k<i+2;k++){
					for(l=j-1;l<j+2;l++){
						temp=buffer[k][l];
						filSum=filSum+(temp*filter[k-i+1][l-j+1]);
					}
				}
				out_buffer[i][j]=(unsigned char)filSum;
			} 
		}
	}
	return(NULL);
}
unsigned char selection(unsigned char a[],int n)
{
	int i,j,min,t;
	for(i=0;i<n;i++)
	{
		min=i;
		for(j=i+i;j<n;j++)
		{
			if(a[min]>a[j])
			{
				min=j;
			}
		}
		if(min!=i)
		{
			t=a[min];
			a[min]=a[i];
			a[i]=t;
		}
	}
	return a[4];
}
	
int main()
{
	time_t t1,t2;
	clock_t start, end; 
	pthread_t threads[MAX_THREAD];
	FILE *fIn = fopen("man.bmp","r");					//Input File name
	FILE *fOut = fopen("lena_rotate.bmp","w+");		    	//Output File name

	int i,j,choice,k,l,x;
	unsigned char byte[54],colorTable[1024];

	if(fIn==NULL)											// check if the input file has not been opened succesfully.
	{
		printf("File does not exist.\n");
	}

	for(i=0;i<54;i++)										//read the 54 byte header from fIn
	{
		byte[i] = getc(fIn);
	}

	fwrite(byte,sizeof(unsigned char),54,fOut);				//write the header back

	// extract image height, width and bitDepth from imageHeader
	height = *(int*)&byte[18];
	width = *(int*)&byte[22];
	int bitDepth = *(int*)&byte[28];
//	unsigned char buffer[width][height];					//to store the image data
	//unsigned char out_buffer[width][height];
	printf("width: %d\n",width);
	printf("height: %d\n",height );

	long int size = height*width;
	int filSum=0;
	unsigned char a[9]={0};		
	//int filter[3][3] = {{1,1,1},{1,-8,1},{1,1,1}};
						//calculate image size
	int diagfilter[3][3] = {{1,1,0},{1,0,-1},{0,-1,-1}};
	//int filter[3][3] = {{1,1,1},{0,0,0},{-1,-1,-1}};
	
	if(bitDepth <= 8)										//if ColorTable present, extract it.
	{
		fread(colorTable,sizeof(unsigned char),1024,fIn);
		fwrite(colorTable,sizeof(unsigned char),1024,fOut);
	}

	

	fread(buffer,sizeof(unsigned char),size,fIn);			//read the image data

	printf("Enter your choice :\n");
	printf("1. Rotate right\n");
	printf("2. Rotate left\n");
	printf("3. Rotate 180\n");
	printf("4. Smooth Image\n");
	printf("5. Sharp Image\n");
	(void) time(&t1);
	start = clock(); 
	for(i=0;i<MAX_THREAD;++i){
		printf("Heloo\n");
		pthread_create(&threads[i],NULL,noise,(void*)&index1[i]);
	}
	for(i=0;i<MAX_THREAD;++i){
		pthread_join(threads[i], NULL);
	}
	(void) time(&t2);
	end = clock(); 
	double time_taken = (double)(end - start) / (double)(CLOCKS_PER_SEC); 
	printf("\ntime taken:%lf\n",time_taken);
	fwrite(out_buffer,sizeof(unsigned char),size,fOut);		//write back to the output image

	fclose(fIn);
	fclose(fOut);

	return 0;
}
