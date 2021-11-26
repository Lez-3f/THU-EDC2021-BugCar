#include"strategy.h"
#include<math.h>
#include"value.h"
#include"uppercom.h"
#include"ctrl.h"

# define T_trun 1 //1°
# define T_straight 1 //1cm

Pos curPos;

int best_time;
int best_iw;

float abs_arctan_dif(int t1[2],int t2[2])
{
	//返回0—180
	int x=t1[1]*t2[1]-t1[0]*t2[0],
		y=t1[0]*t2[1]-t1[1]*t2[0];
	float ax,ay,t,r;
	ax=x>0?x:-x;
	ay=y>0?y:-y;
	if (ax>ay) {
		t = ay/ax;
		r = t*(45-15.66*(t-1));}
	else if (ay==0)
		r=0;
	else {
		t = ax/ay;
		r = 90-t*(45-15.66*(t-1));}
	if (x<0) r = 180-r;
	//printf("%f\n",r);
	return r;
}

int time_cpw(Object* points,int i_w)
{
	//num个中间点，1个始car，1个末warehouse
    //Pos curPos = getCurPos();
    int num = 5;
    int vec[6][2];
    int i=0;
	//vec=(int(*)[2])malloc(sizeof(int)*num*2);
	vec[0][0] = points[0].x - curPos.x;
	vec[0][1] = points[0].y - curPos.y;
	for (i=1;i<num;i++)
	{
		vec[i][0] = points[i].x - points[i-1].x;
		vec[i][1] = points[i].y - points[i-1].y;
	}
	vec[num][0]=wareHouse[i_w].x-points[num-1].x;
	vec[num][1]=wareHouse[i_w].y-points[num-1].y;

    float car_angle = getRealAngle(), vec1_angle = atan2(vec[0][0], vec[0][1]);
    int angle = ANGLE_NORM(car_angle - vec1_angle); //获取总角度,len=num+1
    angle = 90 - fabs(90 - fabs(angle));
    for ( i = 1; i < num+1; i++) 
	{
		if(vec[i-1][0]*vec[i][0]+vec[i-1][1]*vec[i][1]>0)
			angle += abs_arctan_dif(vec[i-1],vec[i]);
		else 
			angle += 180 - abs_arctan_dif(vec[i-1],vec[i]);
	}

	int distance = 0;//获取总路程,len=num+1
	for ( i = 0; i < num+1; i++) distance += sqrt(vec[i][0]*vec[i][0]+vec[i][1]*vec[i][1]);

    return T_trun * angle + T_straight * distance;

}

void permutation_cpw(int k, int num, Object* points, Pos* b_p)  
{  
    //递归到底层  
    if(k == num-1){
        int i;
        for (i = 0; i < 8; i++)
        {
            int time = time_cpw(points, i);
			// printf("time:%d\n",time);
			if (time < best_time){
				// printf("best time:%d\n",best_time);
				best_time = time;
				best_iw = i;

                for (int i = 0; i < num; ++i)
                {
                    b_p[i].x = points[i].x;
                    b_p[i].y = points[i].y;
                }
            }
        }
    }
    else {
        for (int i = k; i < num; i++)
        {
            //换序排列
            Object temp = points[k];
            points[k] = points[i];
            points[i] = temp;

            //递归下一层
            permutation_cpw(k + 1, num, points, b_p);

            //每层递归后恢复上层的顺序
            temp = points[k];
            points[k] = points[i];
            points[i] = temp;
        }
    }
}  

void getBestPlan( Object* points, Pos* b_p)
{
    curPos = getCurPos();
    best_time = 2540, best_iw = 0;
    permutation_cpw(0, 5, points, b_p);
    b_p[5].x = wareHouse[best_iw].x, b_p[5].y = wareHouse[best_iw].y;
}