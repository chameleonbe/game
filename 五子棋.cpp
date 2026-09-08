#include<graphics.h>
#include<time.h>
#include<iostream>
using namespace std;
int a = 50;//y轴偏移
int b = 125;//x轴偏移
int black = 1;
int white = 2;
int Board_Data[15][15] = {0};//初始化，不然为垃圾数
int current_chequer = 1;
int Checkwin(int a)
{//横向判定
	int n=1;
for(int i=0;i<11;i++)
	for (int j = 0; j < 15; j++)
	{
		for (n = 1; n < 5; n++)
		{
			if (Board_Data[j][i] != a)
				break;
			if (Board_Data[j][i] != Board_Data[j][i + n])
				break;
		}
	        if (n == 5)
				return a;
		
	}
//纵向判定
for (int i = 0; i < 11; i++)
	for (int j = 0; j < 15; j++)
	{
		for ( n = 1; n < 5; n++)
		{
			if (Board_Data[i][j] != a)
				break;
			if (Board_Data[i][j] != Board_Data[i+n][j])
				break;
		}
		if (n == 5)
			return a;

	}
//右斜判定
for (int i = 0; i < 10; i++)
	for (int j = 0; j < 10; j++)
	{
		for (n = 1; n < 5; n++)
		{
			if (Board_Data[i][j] != a)
				break;
			if (Board_Data[i][j] != Board_Data[i + n][j + n])
				break;
		}
		if (n == 5)
			return a;
	}
 
//左斜判定
/*for (int i = 0; i < 10; i++)
	for (int j = 4; j < 15; j++)
	{
		for (int n = 1; n < 5; n++)//局部定义覆盖了全局定义
		{
			if (Board_Data[i][j] == 0)
				break;
			if (Board_Data[i][j] != Board_Data[i + n][j - n])
				break;
		}
		if (n == 5)
			
			return a;
	}*/
for (int i = 0; i < 10; i++)
	for (int j = 4; j < 15; j++)
	{
		for (n = 1; n < 5; n++)
		{
			if (Board_Data[i][j] != a)
				break;
			if (Board_Data[i][j] != Board_Data[i + n][j - n])
				break;
		}
		if (n == 5)
			return a;
	}
return 0;
}

void DrawBoard()
{
	for (int i = a; i < 750+a; i += 50)//横线
		line(b, i, 700+b, i);
	for (int i = b; i < 750+b; i += 50)//竖线
		line(i, a, i, 700+a);
}

 void DrawChequer()//蛤
 {
	for (int i = 0; i <= 14; i++)
		for (int j = 0; j <= 14; j++)
		{
			if (Board_Data[j][i] == 2)
				solidcircle(i * 50 + b, j * 50 + a, 20);
			if (Board_Data[j][i] == 1)
				circle(i * 50 + b, j * 50 + a, 20);
		}
	
}
int main()
{
	initgraph(1000,1000);
	ExMessage msg;
	bool r = true;
	BeginBatchDraw();
	while (r)
	{
		while (peekmessage(&msg))
		{//按下
			if (msg.message == WM_LBUTTONDOWN)
			{//落点
				int x = msg.x;
				int y = msg.y;
				int index_x=-1;
				int index_y=-1;
				//if ((y - a) % 50 > 20 ||  (x - b) % 50 > 20 )//按键判定范围
					//continue; 
				if ((y - a) % 50 > 30)//|| (y - a) % 50 <10
					index_x = (y - a) / 50 + 1;
				else if ((y - a) % 50 < 10)
					index_x = (y - a) / 50;
				else break;
				if ((x - b) % 50 > 30)//|| (x - b) % 50 <10
					index_y = (x - b) / 50 + 1;
				else if((x - b) % 50 < 10)
				     index_y = (x - b) / 50 ;
				else break;
				//落子
				if (Board_Data[index_x][index_y] == 0)//判空
				{
					Board_Data[index_x][index_y] = current_chequer;
					//换子
					if (current_chequer == 1)
					{
						current_chequer = 2;
					}
					else
					{
						current_chequer = 1;
					}
				}
				cout << x << ' ' << y <<' ' << index_x << ' ' << index_y<< " " << (x - b) % 50 << " " << (y - a) % 50 << endl;
				for (int i = 0; i <= 14; i++)
				{
					for (int j = 0; j <= 14; j++)
						cout << Board_Data[i][j];
					cout << endl;
				}
				cout << endl;
			}
			
			
		}
		DrawBoard();
		DrawChequer();
		FlushBatchDraw();
		if (Checkwin(black)==1)
		{
			Sleep(100);
			MessageBox(GetHWnd(), _T("黑 胜"), _T("GAME OVER"), MB_OK);
			r = false;
		}
		else if (Checkwin(white)==2)
		{
			Sleep(100);
			MessageBox(GetHWnd(), _T("白 胜"), _T("GAME OVER"), MB_OK);
			r = false;
		}
		Sleep(100);
		
	}
	EndBatchDraw();
}