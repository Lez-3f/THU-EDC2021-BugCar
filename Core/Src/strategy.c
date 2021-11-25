#include"strategy.h"
#include"ctrl.h"
#include<math.h>
#include"uppercom.h"
#include"delay.h"
#include"pwm.h"

#define PW2(X) ((X)*(X))
#define A 1e8

bool isEnable()
{
    return !isGamePause() && HAL_GPIO_ReadPin(pinEnable_GPIO_Port, pinEnable_Pin) == GPIO_PIN_RESET;
}

void go2Point(Pos dest)
{
    Pos curPos = getCurPos();

    Pos relaPos;
    relaPos.x = dest.x - curPos.x;
    relaPos.y = dest.y - curPos.y;

    float angle0 = getRealAngle();  //获取当前车身角度

    double relaAngle = - 180 * atan2(relaPos.x, relaPos.y) / PI; //计算位移角度
    float distance = sqrt(PW2(relaPos.x) + PW2(relaPos.y)); //小车的距离

    float difAngle = ANGLE_NORM(relaAngle - angle0);

    if(fabs(difAngle) > 90)
    {
        setAngle(180+relaAngle);
        while(!isAngleCompleted())
        {
            if(!isEnable())
            {
                setSpeedStraight0();
                setAngle(getRealAngle());
                return;
            }
            delay_ms(10);
        }   //转角度

        setSpeedStraight(-SPEED, distance);

        while(!isDisCompleted())
        {
            if (!isEnable())
            {
                setSpeedStraight0();
                setAngle(getRealAngle());
                return;
            }
            delay_ms(10);
        }
    }

    else{
        setAngle(relaAngle);
        while(!isAngleCompleted())
        {
            if(!isEnable())
            {
                setSpeedStraight0();
                setAngle(getRealAngle());
                return;
            }
            delay_ms(10);
        }   //转角度

        setSpeedStraight(SPEED, distance);

        while(!isDisCompleted())
        {
            if (!isEnable())
            {
                setSpeedStraight0();
                setAngle(getRealAngle());
                return;
            }
            delay_ms(10);
        }
    }
}


Pos calMetPos(int S[3], Pos P[3])
{
	int aa[2], bb[2], cc[2], AA, BB, CC, x0, y0;
	int i = 0;
	for (i = 0; i < 2; i++)
	{
		aa[i] = 2 * P[i + 1].x - 2 * P[i].x;
		bb[i] = 2 * P[i + 1].y - 2 * P[i].y;
		cc[i] = A / S[i] - A / S[i + 1]+PW2(P[i+1].x)-PW2(P[i].x)+PW2(P[i+1].y)-PW2(P[i].y);
	}
	AA = aa[0] * bb[1] - aa[1] * bb[0];
	BB = aa[0] * cc[1] - aa[1] * cc[0];
	CC = bb[0] * cc[1] - bb[1] * cc[0];
	x0 = -CC / AA;
	y0 = BB / AA;
    Pos p = {x0, y0};
    return p;
}

Pos calCarPosByBeacon(int d[3])
{
	int aa[2], bb[2], cc[2], AA, BB, CC, x0, y0;
	int i = 0;
	for (i = 0; i < 2; ++i)
	{
        aa[i] = 2 * b[i].x - 2 * b[i + 1].x;
        bb[i] = 2 * b[i].x - 2 * b[i + 1].x;
        cc[i] = PW2(d[i + 1]) - PW2(d[i]) + PW2(b[i].x) + PW2(b[i].y) - PW2(b[i+1].x) - PW2(b[i+1].y);
    }
    AA = aa[0] * bb[1] - aa[1] * bb[0];
	BB = aa[0] * cc[1] - aa[1] * cc[0];
	CC = bb[0] * cc[1] - bb[1] * cc[0];
    x0 = -CC / AA;
    y0 = BB / AA;
    Pos p = {x0, y0};
    return p;
}
/*
# define T_trun 1 //1°
# define T_straight 1 //1cm
//# define wKidth 254

// struct Object{
// 	int x,y;
// 	char type;
// }m[2],//metal
// w[8]={{0,0},{0,127},{0,254},{127,0},{127,254},{254,0},{254,127},{254,254}},//warehouse
// b[3]={{127,112},{127,127},{127,142}};//beacon

// struct Car{
// 	int x,y;
// 	int vec[2];//速度方向={x坐标变化，y坐标变化}
// 	float angle;
// }car={111,0,{1,0}};

// void generate_metal(struct Object* m)
// {
// 	m->x=rand()%254+1;
// 	m->y=rand()%254+1;
// }

// float arctan(float x,float y)
// {
// 	//返回-180—180
// 	float ax,ay,t,r;
// 	ax=x>0?x:-x;
// 	ay=y>0?y:-y;
// 	if (ax>ay) {
// 		t = ay/ax;
// 		r = t*(45-15.66*(t-1));}
// 	else {
// 		t = ax/ay;
// 		r = 90-t*(45-15.66*(t-1));}
// 	if (x<0) r = 180-r;
// 	if (y<0) r = -r;
// 	//printf("%f\n",r);
// 	return r;
// }
float arctan_dif(int t1[2],int t2[2])
{
	//t1\t2 = {x，y}
	//返回-180—180
	int x=t1[1]*t2[1]-t1[0]*t2[0],
		y=t1[0]*t2[1]-t1[1]*t2[0],
		ax,ay;
	float t,r;
	ax=x>0?x:-x;
	ay=y>0?y:-y;
	if (ax>ay) {
		t = ay/ax;
		r = t*(45-15.66*(t-1));}
	else {
		t = ax/ay;
		r = 90-t*(45-15.66*(t-1));}
	if (x<0) r = 180-r;
	if (y<0) r = -r;
	//printf("%f\n",r);
	return r;
}
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
// unsigned int sqrt(unsigned long M)  
// {  
// 	//实现16位整数开根
//     unsigned int N, i;  
//     unsigned long tmp, ttp;   // 结果、循环计数  
//     if (M == 0)               // 被开方数，开方结果也为0  
//         return 0;  
//     N = 0;  
//     tmp = (M >> 30);          // 获取最高位：B[m-1]  
//     M <<= 2;  
//     if (tmp > 1)              // 最高位为1  
// 	{  
//         N ++;                 // 结果当前位为1，否则为默认的0  
//         tmp -= N;  
//     }  
//     for (i=15; i>0; i--)      // 求剩余的15位  
//     {  
//         N <<= 1;              // 左移一位  
//         tmp <<= 2;  
//         tmp += (M >> 30);     // 假设  
//         ttp = N;  
//         ttp = (ttp<<1)+1;  
//         M <<= 2;  
//         if (tmp >= ttp)       // 假设成立  
//         {  
//             tmp -= ttp;  
//             N ++;  
//         }  
//     }  
// 	//printf("%d\n",N);
//     return N;  
// } 
int time_cpw(Object* points,int num,int i_w)
{
	//num个中间点，1个始car，1个末warehouse
    Pos curPos = getCurPos();
    int (*vec)[2],i=0;//获取所有路径矢量,len=num+1
	vec=(int(*)[2])malloc(sizeof(int)*num*2);
	vec[0][0] = points[0].x - curPos.x;
	vec[0][1] = points[0].y - curPos.y;
	for (i=1;i<num;i++)
	{
		vec[i][0] = points[i].x - points[i-1].x;
		vec[i][1] = points[i].y - points[i-1].y;
	}
	vec[num][0]=w[i_w].x-points[num-1].x;
	vec[num][1]=w[i_w].y-points[num-1].y;

    float car_angle = getRealAngle(), vec1_angle = atan2(vec[0][0], vec[0][1]);
    int angle = ANGLE_NORM(car_angle - vec1_angle);//获取总角度,len=num+1
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

int best_time=254*10,best_iw;
Object* b_p;//best points
void permutation(int k, int num, Object* points)  
{  
    //递归到底层  
    if(k == num-1){  
		int i;
		for (i=0;i<8;i++){
			int time = time_cpw(points,num,i);
			//printf("time:%d\n",time);
			if (time < best_time){
				printf("best time:%d\n",best_time);
				best_time = time;
				best_iw = i;
				b_p = points;
				;}}}
    else {  
        for(int i = k; i < num; i ++){  
			//换序排列
			Object temp = points[k];  
			points[k] = points[i];  
			points[i] = temp;  
      
            //递归下一层  
			permutation(k+1, num, points);  
      
            //每层递归后恢复上层的顺序  
            temp = points[k];  
			points[k] = points[i];  
			points[i] = temp;}}  
}  
*/
// void main()
// {
// 	clock_t start,end;
// 	srand((unsigned)time( NULL )); 
// 	generate_metal(&m[0]);
// 	//printf("%d,%d\n",m[0].x,m[0].y);
// 	generate_metal(&m[1]);
// 	//printf("%d,%d\n",m[1].x,m[1].y);
// 	struct Object points[5]={m[0],m[1],b[0],b[1],b[2]};//第一回合需经过的点列

// 	start = clock();
// 	{
// 		//for (int i=0;i<1000000;i++) arctan(1,7);
// 		//for (int i=0;i<1000000;i++) sqrt(254*200);
// 		//printf("%d\n",sqrt(254*254+230*230));
// 		{
// 		permutation(0,5,points);
// 		printf("car:(%d,%d)\n",car.x,car.y);
// 		for(int i=0;i<5;i++)printf("(%d,%d)\n",b_p[i].x,b_p[i].y);
// 		printf("warehouse:(%d,%d)\n",w[best_iw].x,w[best_iw].y);
// 		}
// 		//int time = time_cpw(points,5,3);
// 	}
// 	end = clock();
	
// 	printf("计算用时 = %fs\n",double(end-start)/CLK_TCK);
// }

