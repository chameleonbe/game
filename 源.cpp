#include<graphics.h>
#include<time.h>
#include<iostream>

char board_data[3][3] =
{ 
'-','-','-',
'-', '-', '-', 
'-', '-', '-',
};//棋盘

char current_piece = 'O';//初始落子

bool CheckWin(char c)//检测胜利
{
	for (int i = 0; i < 3; i++)
		if (board_data[i][0] == c && board_data[i][1] == c && board_data[i][2] == c)
		{
			line(0, 0, 600, 600);
			return true;
		}
	for (int i = 0; i < 3; i++)
		if (board_data[0][i] == c && board_data[1][i] == c && board_data[2][i] == c)
		{
			line(0, 600, 600, 0);
			return true;
		}
	if (board_data[0][0] == c && board_data[1][1] == c && board_data[2][2] == c)
	{
		
		return true;
	}
	if (board_data[0][2] == c && board_data[1][1] == c && board_data[2][0] == c)
	{
		
		return true;
	}

	return false;
}

bool CheckDraw()//检测平局
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			if (board_data[i][j] == '-')
				return false;
	return true;
}
void DrawBoard()//棋盘
{
	line(0, 200, 600, 200);
	line(0, 400, 600, 400);
	line(200, 0, 200, 600);
	line(400, 0, 400, 600);
}
void Drawpiece()//棋子
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			if (board_data[i][j] == 'O')
				circle(200 * i + 100, 200 * j + 100, 100);
			else if (board_data[i][j] == 'X')
			{
				line(200 * i, 200 * j, 200 * i + 200, 200 * j + 200);
				line(200 * i + 200, 200 * j, 200 * i, 200 * j + 200);
			}
}
void DrawTipText()//提示信息
{
	static TCHAR str[64];
	_stprintf_s(str,_T("当前棋子类型：%c"),current_piece);
	settextcolor(RGB(225, 175, 45));
	outtextxy(0, 0, str);
}
int main()
{
	initgraph(600, 600);
	ExMessage msg;
	bool r = true;
	BeginBatchDraw();

	while (r)
	{
		DWORD s = GetTickCount();
		while(peekmessage(&msg))
		{//按下
			if (msg.message == WM_LBUTTONDOWN)
			{//落点
				int x = msg.x;
				int y = msg.y;

				int index_x = x / 200;
				int index_y = y / 200;
				//落子
				if (board_data[index_x][index_y]=='-')//判空
				{
					board_data[index_x][index_y] = current_piece;
					//换子
					if (current_piece == 'O')
					{
						current_piece = 'X';
					}
					else
					{
						current_piece = 'O';
					}
				}
			}
		
		
		}
		cleardevice();
		DrawBoard();
		Drawpiece();
		DrawTipText();
		FlushBatchDraw();
		if (CheckWin('X'))
		{
			Sleep(100);
			MessageBox(GetHWnd(), _T("X 胜"), _T("GAME OVER"), MB_OK);
			r = false;
		}
		else if (CheckWin('O'))
		{
			Sleep(100);
			MessageBox(GetHWnd(), _T("O 胜"), _T("GAME OVER"), MB_OK);
			r = false;
		}
		else if (CheckDraw())
		{
			Sleep(100);
			MessageBox(GetHWnd(), _T("平局"), _T("GAME OVER"), MB_OK);
			r = false;
		}
		DWORD e = GetTickCount();
		DWORD a = s - e;
		if (a < 1000 / 60)
			Sleep(1000 / 60 - a);
		//Sleep(15);//粗略cpu优化
	
	}
	EndBatchDraw();
}