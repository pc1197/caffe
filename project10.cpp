#include<iostream>
#include<vector>
#include<stdio.h>
#include<assert.h>
#include<cmath>
#include<math.h>
#include<opencv2/opencv.hpp>
#include<stdlib.h>

using namespace std;
using namespace cv;	

int activation(const double sigma)
{
	return ( 1 / (1 + exp(-1*sigma)));
}

Mat applyFilter(Mat image, double **filter ,int filterHeight, int filterwidth,int padding, int stride)
{	
	int height = image.rows;
	int width = image.cols;
	int newImageHeight = ((height-filterHeight+2*padding)/stride)+1;
	int newImagewidth = ((width-filterwidth+2*padding)/stride)+1;
	int d,i,j,h,w;

	
	Mat img_result(newImageHeight,newImagewidth,image.type());
	
	double ***p;
	p = (double***)malloc(image.channels()*sizeof(double**));
		
	for(int b=0; b<image.channels(); b++)
	{	
		*(p+b) = (double**)malloc((image.rows+2*padding)*sizeof(double*));
		for(int n=0; n<image.rows+2*padding; n++)
		{
			*(*(p+b)+n) = (double*)malloc((image.cols+2*padding)*sizeof(double));
		}
	}

	for(int l=0; l<image.channels(); l++)
	{
		for(int k=0; k<image.rows+2*padding; k++)
		{	
			for(int o=0; o<image.cols+2*padding; o++)
			{
				*(*(*(p+l)+k)+o) = 0;
				
			}
		}
	}

	for(int l=0; l<image.channels(); l++)
	{
		for(int k=padding; k<image.rows+padding; k++)
		{	
			for(int o=padding; o<image.cols+padding; o++)
			{
				*(*(*(p+l)+k)+o) = image.at<Vec3b>(k-padding,o-padding)[l];
			}
		}
	}
	
	

	for(d=0; d<img_result.channels(); d++)
	{
		for(i=0; i<newImageHeight; i++)
		{
			for(j=0; j<newImagewidth; j++)
			{
				for(h=i; h<i+filterHeight; h++)
				{
					for(w=j; w<j+filterwidth; w++)
					{
							
						img_result.at<Vec3b>(i,j)[d] += filter[h-i][w-j]*p[d][h][w];
					}
				}
			}			
		}
	}
	return img_result;
}	

Mat pooling(Mat image, int FilterHeight1, int Filterwidth1, int stride)
{	
	int max=0;
		
	Mat img_result1(((image.rows-FilterHeight1)/stride)+1,((image.cols-Filterwidth1)/stride)+1 ,image.type());
	//printf("%d %d ", ((image.rows-FilterHeight1)/stride)+1, ((image.cols-Filterwidth1)/stride)+1); 432 771

	for(int k=0; k<image.channels(); k++)
	{
		for(int c=0; c<image.rows-FilterHeight1; c=c+stride)
		{
			for(int v=0; v<image.cols-Filterwidth1; v=v+stride)
			{
				for(int i=0; i<FilterHeight1; i++)
				{
					for(int j=0; j<Filterwidth1; j++)
					{
						if((image.at<Vec3b>(c+i,v+j)[k]) > max)
						max = image.at<Vec3b>(c+i,v+j)[k] ;
					}
				}
			
				img_result1.at<Vec3b>(c/stride,v/stride)[k] = max;
				//printf("%d %d ", c/stride, v/stride);
				max = 0;
			}
		}
	}
	return img_result1;
}
	

int main()
{
	Mat img_input;
	int n;
	int stride;
	int padding;	
	
	printf("fliter size : ");
	scanf("%d",&n);		

	double **filter;
	
	filter = (double**)malloc(n*sizeof(double*));
	for(int i=0; i<n; i++)
	{
		 *(filter+i) = (double*)malloc(n*sizeof(double*));
	}
	for(int x=0; x<n; x++)
	{
		for(int m=0; m<n; m++)
		{
			scanf("%lf",&filter[x][m]);
			filter[x][m] /=16.0;
			
		}
	}
	
	//이미지 파일을 읽어와서 img_input에 저장
	img_input = imread("input5.jpg", IMREAD_COLOR);
	if(img_input.empty())
	{
		cout << "파일을 읽어올 수 없습니다." << endl;
		exit(1);
	}
	
	printf("stride : ");
	scanf("%d", &stride);
	printf("padding : ");
	scanf("%d", &padding);

	Mat img_result1(((img_input.rows-n)/stride) +1,((img_input.cols-n)/stride) +1 ,img_input.type());
	Mat img_result( (((img_input.rows-n+2*padding)/stride)+1), (((img_input.cols-n+2*padding)/stride)+1),img_input.type());
	
	img_result1 = pooling(img_input,n,n,stride);	
	img_result = applyFilter(img_input,filter,n,n,padding,stride);
	
	//결과를 파일로 저장
	imwrite("img_result.jpg", img_result);	

	Mat edge_filter = Mat::ones(3,3,CV_32F);

	int newImageHeight = ((img_input.rows-n+2*padding)/stride)+1;
	int newImagewidth = ((img_input.cols-n+2*padding)/stride)+1;



	Mat test(newImageHeight,newImagewidth,img_input.type());
	filter2D(img_input,test,-1,edge_filter);
	
	Mat result(newImageHeight,newImagewidth,img_input.type());
	result = img_result - test;
	namedWindow("result",WINDOW_AUTOSIZE);
	imshow("result",result);
	imwrite("result.jpg", result);
	waitKey(0);	

for(int l=0; l<result.channels(); l++)
{
	for(int o=0; o<result.rows; o++)
	{
		for(int p=0; p<result.cols; p++)
		{
			printf("%lf",result.at<Vec3b>(o,p)[l]);
		}
	}
}
	for(int i=0; i<n; i++) free(*(filter+i));
	free(filter);	

	
	
	
	return 0; 
	
}
