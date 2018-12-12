#include <stdio.h>

int aim=1600424+2; 
//注意data temp大小趋势只能这样 
//data 大-->小
//temp 小-->大
int data[]={1633462,1625131,1616848,1608612,1600424,1592282,1584187,1576138,1568136,1560178,1552266,1544400,1536578}; //res
int temp[]={0,100,200,300,400,500,600,700,800,900,1000,1100,1200};
	
int search(int aim,int data[],int size,int* from,int* to)
{
	//二分法搜索 
	int left = 0;//定义left整数变量
	int right = size-1;//定义right
	
	if((aim > data[left]) || (aim < data[right])) //不在查找范围内
		return -1;
	
	while(left<=right) //在while循环中直到有一个条件结束搜索 
	{
		int mid = (left+right)/2;
		if(data[mid]>aim)
		{
			 *to = left = mid+1;	
		}
		else if(data[mid]<aim)
		{
			*from = right = mid-1;
		}
		else if(data[mid]==aim)
		{
			*from = *to = mid;
			break;//一定要break跳出循环 
		}
	}
	printf("left:%d,right:%d\n",left,right);
	return 1;
}

int main()
{

    int from,to;
	int t;
	printf("%d\n",search(aim,data,sizeof(data)/sizeof(data[0]),&from,&to));//希望输出14所对应的索引号 
	
	printf("%d is in data arr from:%d to:%d\n",aim,from,to);
	if(from == to)
		printf("temp is %d\n",temp[from]);
	else
	{
		t = temp[to] + (temp[to] - temp[from])*1.0*(data[to] - aim)/(data[from] - data[to]);
		printf("cal temp is %d\n",t);
	}
	return 0;
}






















