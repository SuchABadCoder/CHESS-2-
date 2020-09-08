#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

const int Size = 56; //размер одной клетки
const int Frame = 27; //  размер рамки 
const int board_size = 8; //размер массива доски
int number = 32; //кол-во фигур на доске
bool isWarn = false, endLine = false, mat = false, KingW_1 = false, KingW_2 = false, rewrite = false; //есть ли предупреждение
int turn = 1, px, py, kingStep;
int board[board_size][board_size] =
{ -1,-2,-3,-4,-5,-3,-2,-1,
 -6,-6,-6,-6,-6,-6,-6,-6,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  6, 6, 6, 6, 6, 6, 6, 6,
  1, 2, 3, 4, 5, 3, 2, 1 };
int way[board_size][board_size];
int stepBack[board_size][board_size]; //буфер где записан предыдущий ход
int rkrvk[6] = { 1,1,1,1,1,1 };
Sprite f[32];
Vector2i kingPos[8];

void SetPos() //выставляет спрайты фигур на позиции
{
	int k = 0;
	for (int i = 0; i < board_size; i++)
		for (int j = 0; j < board_size; j++)
		{
			int n = board[i][j];
			if (!n) continue;
			int x = abs(n) - 1;
			int y = n > 0 ? 1 : 0;
			f[k].setTextureRect(IntRect(Size*x, Size*y, Size, Size));
			f[k].setPosition(Size*j + Frame, Size*i + Frame);
			k++;
		}
}

void rewriter (int mas1[board_size][board_size], int mas2[board_size][board_size]) //перезаписывает текущую доску в буфер
{
	for (int i = 0; i < board_size; i++)
		for (int j = 0; j < board_size; j++)
			mas1[i][j] = mas2[i][j];
}

void spin()
{
	for(int i = 0; i < board_size / 2; i++)
		for (int j = 0; j < board_size; j++)
		{
			int t = board[i][j];
			board[i][j] = board[7 - i][j];
			board[7 - i][j] = t;
		}
}

bool F2(int m, int n)
{
	for (int q = 0; q < 9; q++)
		if (m == kingPos[q].x && n == kingPos[q].y)
		{
			kingPos[q].x = -1;
			kingPos[q].y = -1;
			break;
		}
	else if (board[n][m] != 0)
		return true;
	return false;
}

void F3(int n, int m) // knight
{
	int comb[8] = { 1, 1, -1, -1, -2, 2, 2, -2 };
	int m1, n1;
	for (int r = 0; r < 8; r++)
	{
		m1 = m + comb[r];
		n1 = n + comb[7 - r];
		for (int q = 0; q < 9; q++)
			if ((m1 == kingPos[q].x && n1 == kingPos[q].y) || abs(board[m1][n1]) == 5)
			{
				kingPos[q].x = -1;
				kingPos[q].y = -1;
				break;
			}
	}
}

bool F4(int n, int m, int type)
{
	if (board[m][n] == 0)
		way[m][n] = 1;
	else if (board[m][n] < 0 && type > 0)
	{
		way[m][n] = 2;
		return true;
	}
	else if (board[m][n] > 0 && type < 0)
	{
		way[m][n] = 2;
		return true;
	}
	else
		return true;
	return false;
}

void kingSquare(int x, int y)
{
	int c = 0;
	for (int i = y - 1; i <= y + 1; i++)
		for (int j = x - 1; j <= x + 1; j++)
			if ((turn % 2 == 0 && board[i][j] >= 0 || turn % 2 != 0 && board[i][j] <= 0) && i >= 0 && j >= 0 && i < board_size && j < board_size)
			{
				kingPos[c].x = j;
				kingPos[c].y = i;
				c++;
			}
			else
			{
				kingPos[c].x = -1;
				kingPos[c].y = -1;
				c++;
			}
}

void kingWay(int x, int y)
{
	int t = turn % 2 != 0 ? -1 : 1;
	for (int i = 0; i < board_size; i++)
		for (int j = 0; j < board_size; j++)
		{
			int tx = j;
			int ty = i;
			if (board[i][j] * t == 1)
			{
				for (int n = tx - 1; n >= 0; n--)//left
					if (F2(ty, n))
						break;
				for (int n = tx + 1; n < board_size; n++)//right
					if (F2(ty, n))
						break;
				for (int n = ty - 1; n >= 0; n--)//up
					if (F2(n, tx))
						break;
				for (int n = ty + 1; n < board_size; n++)//down
					if (F2(n, tx))
						break;
			}
			if (board[i][j] * t == 2)
				F3(ty, tx);
			if (board[i][j] * t == 3)
			{
				for (int n = tx + 1, m = ty - 1; n < board_size || m >= 0; n++, m--)//up right
					if (F2(n, m))
						break;
				for (int n = tx - 1, m = ty - 1; n < board_size || m >= 0; n--, m--)//up left
					if (F2(n, m))
						break;
				for (int n = tx - 1, m = ty + 1; n < board_size || m >= 0; n--, m++)//down left
					if (F2(n, m))
						break;
				for (int n = tx + 1, m = ty + 1; n < board_size || m >= 0; n++, m++)//down right
					if (F2(n, m))
						break;
			}
			if (board[i][j] * t == 4)
			{
				for (int n = tx + 1, m = ty - 1; n < board_size || m >= 0; n++, m--)//up right
					if (F2(n, m))
						break;
				for (int n = tx - 1, m = ty - 1; n < board_size || m >= 0; n--, m--)//up left
					if (F2(n, m))
						break;
				for (int n = tx - 1, m = ty + 1; n < board_size || m >= 0; n--, m++)//down left
					if (F2(n, m))
						break;
				for (int n = tx + 1, m = ty + 1; n < board_size || m >= 0; n++, m++)//down right
					if (F2(n, m))
						break;
				for (int n = tx - 1; n >= 0; n--)//left
					if (F2(ty, n))
						break;
				for (int n = tx + 1; n < board_size; n++)//right
					if (F2(ty, n))
						break;
				for (int n = ty - 1; n >= 0; n--)//up
					if (F2(n, tx))
						break;
				for (int n = ty + 1; n < board_size; n++)//down
					if (F2(n, tx))
						break;
			}
			if (board[i][j] * t == 5)
			{
				for (int n = i - 1; n <= i + 1; n++)
					for (int m = j - 1; m <= j + 1; m++)
						for (int q = 0; q < 9; q++)
							if(n == kingPos[q].y && m == kingPos[q].x)
							{
								kingPos[q].x = -1;
								kingPos[q].y = -1;
							}
			}
			if (board[i][j] * t == 6)
				for (int q = 0; q < 9; q++)
					if ((tx + 1 == kingPos[q].x || tx - 1 == kingPos[q].x) && ty + 1 == kingPos[q].y)
					{
						kingPos[q].x = -1;
						kingPos[q].y = -1;
					}
		}
}

void getStep(int ex, int ey, int pType, bool b)
{
	int t = turn % 2 != 0 ? -1 : 1;
	if (b && isWarn == false)
	{
		board[ey][ex] = pType;
		SetPos();
		spin();
		SetPos();
		turn++;
		rewrite = true;
		if (stepBack[ey][ex]*t == 5)
		{
			isWarn = true;
				mat = true;
		}
	}
	else
	{
		rewriter(board, stepBack);
		SetPos();
		isWarn = true;
	}
}

void stepper(int bx, int by, int ex, int ey) //логика фигур
{
	int pType = stepBack[by][bx];
	bool flag = false;
		switch (abs(pType))
		{
		case 1://ладья
			if (bx == ex || by == ey) //условие передвижения
			{
				if (by > ey)//up
					for (int i = by - 1; i > ey; i--)
						if (board[i][bx] != 0)
							flag = true;//blocked
				if (ey > by)//down
					for (int i = by + 1; i < ey; i++)
						if (board[i][bx] != 0)
							flag = true;
				if (bx > ex)//left
					for (int i = bx - 1; i > ex; i--)
						if (board[by][i] != 0)
							flag = true;
				if (ex > bx)//right
					for (int i = bx + 1; i < ex; i++)
						if (board[by][i] != 0)
							flag = true;
				if (ex == bx && ey == by)
					flag = true;
				if (flag) // если на пути есть фигуры
					getStep(ex, ey, pType, false);
				else
				{
					if (board[ey][ex] != 0)
						number--;
					if (turn % 2 != 0 && bx == 0 && by == 7)
						rkrvk[1] = 0;
					if (turn % 2 != 0 && bx == 7 && by == 7)
						rkrvk[2] = 0;
					if (turn % 2 == 0 && bx == 0 && by == 7)
						rkrvk[4] = 0;
					if (turn % 2 == 0 && bx == 7 && by == 7)
						rkrvk[5] = 0;
					getStep(ex, ey, pType, true);
				}
			}
			else
				getStep(ex, ey, pType, false);
			break;
		case 2://knight
			if ((abs(ex - bx) == 2 && abs(ey - by) == 1) || (abs(ey - by) == 2 && abs(ex - bx) == 1))
			{
				if (board[ey][ex] != 0)
					number--;
				getStep(ex, ey, pType, true);
			}
			else
				getStep(ex, ey, pType, false);
			break;
		case 3://bishop
			if (abs(bx - ex) == abs(by - ey))
			{
				if (by > ey && bx < ex) //up right
					for (int i = bx + 1, j = by - 1; i < ex; i++, j--)
						if (board[j][i] != 0)
							flag = true;
				if (by > ey && bx > ex)//up left
					for (int i = bx - 1, j = by - 1; i > ex; i--, j--)
						if (board[j][i] != 0)
							flag = true;
				if (by < ey && bx > ex)//down left
					for (int i = bx - 1, j = by + 1; i > ex; i--, j++)
						if (board[j][i] != 0)
							flag = true;
				if (by < ey && bx < ex)//down right
					for (int i = bx + 1, j = by + 1; i < ex; i++, j++)
						if (board[j][i] != 0)
							flag = true;
				if (ex == bx && ey == by)
					flag = true;
				if (flag) // если на пути есть фигуры
					getStep(ex, ey, pType, false);
				else
				{
					if (board[ey][ex] != 0)
						number--;
					getStep(ex, ey, pType, true);
				}
			}
			else
				getStep(ex, ey, pType, false);
			break;
		case 4: //queen
			if (bx == ex || by == ey || abs(bx - ex) == abs(by - ey))
			{
				if (by > ey && bx < ex) //up right
					for (int i = bx + 1, j = by - 1; i < ex; i++, j--)
						if (board[j][i] != 0)
							flag = true;
				if (by > ey && bx > ex)//up left
					for (int i = bx - 1, j = by - 1; i > ex; i--, j--)
						if (board[j][i] != 0)
							flag = true;
				if (by < ey && bx > ex)//down left
					for (int i = bx - 1, j = by + 1; i > ex; i--, j++)
						if (board[j][i] != 0)
							flag = true;
				if (by < ey && bx < ex)//down right
					for (int i = bx + 1, j = by + 1; i < ex; i++, j++)
						if (board[j][i] != 0)
							flag = true;
				if (by > ey && bx == ex)//up
					for (int i = by - 1; i > ey; i--)
						if (board[i][bx] != 0)
							flag = true;//blocked
				if (ey > by && bx == ex)//down
					for (int i = by + 1; i < ey; i++)
						if (board[i][bx] != 0)
							flag = true;
				if (bx > ex && by == ey)//left
					for (int i = bx - 1; i > ex; i--)
						if (board[by][i] != 0)
							flag = true;
				if (ex > bx  && by == ey)//right
					for (int i = bx + 1; i < ex; i++)
						if (board[by][i] != 0)
							flag = true;
				if (ex == bx && ey == by)
					flag = true;
				if (flag) // если на пути есть фигуры
					getStep(ex, ey, pType, false);
				else
				{
					if (board[ey][ex] != 0)
						number--;
					getStep(ex, ey, pType, true);
				}
			}
			else
				getStep(ex, ey, pType, false);
			break;
		case 5://king
			if (ey == by && ex == bx)
			{
				rewriter(board, stepBack);
				SetPos();
				break;
			}
			else
			{
				bool rakirovka = true;
				if (rkrvk[0] == 1 && turn % 2 != 0)
				{
					if (ex == 2 && ey == 7 && rkrvk[1] == 1)
					{
						for (int i = 3; i > 0; i--)
							if (board[7][i] != 0)
							{
								rakirovka = false;
								break;
							}
						if (rakirovka)
						{
							board[7][2] = 5;
							board[7][4] = 0;
							board[7][0] = 0;
							board[7][3] = 1;
							SetPos();
							spin();
							SetPos();
							turn++;
							rewrite = true;
							break;
						}
					}
				}
				if (rkrvk[3] == 1 && turn % 2 == 0)
				{
					if (ex == 2 && ey == 7 && rkrvk[4] == 1)
					{
						for (int i = 2; i > 0; i--)
							if (board[7][i] != 0)
							{
								rakirovka = false;
								break;
							}
						if (rakirovka)
						{
							board[7][2] = -5;
							board[7][4] = 0;
							board[7][0] = 0;
							board[7][3] = -1;
							SetPos();
							spin();
							SetPos();
							turn++;
							rewrite = true;
							break;
						}
					}
				}
				if (rkrvk[0] == 1 && turn % 2 != 0)
				{
					if (ex == 6 && ey == 7 && rkrvk[2] == 1)
					{
						for (int i = 5; i < 7; i++)
							if (board[7][i] != 0)
							{
								rakirovka = false;
								break;
							}
						if (rakirovka)
						{
							board[7][6] = 5;
							board[7][4] = 0;
							board[7][7] = 0;
							board[7][5] = 1;
							SetPos();
							spin();
							SetPos();
							turn++;
							rewrite = true;
							break;
						}
					}
				}
				if (rkrvk[3] == 1 && turn % 2 == 0)
				{
					if (ex == 6 && ey == 7 && rkrvk[4] == 1)
					{
						for (int i = 5; i < 7; i++)
							if (board[7][i] != 0)
							{
								rakirovka = false;
								break;
							}
						if (rakirovka)
						{
							board[7][6] = -5;
							board[7][4] = 0;
							board[7][7] = 0;
							board[7][5] = -1;
							SetPos();
							spin();
							SetPos();
							turn++;
							rewrite = true;
							break;
						}
					}
				}
				if (abs(ex - bx) <= 1 && abs(ey - by) <= 1)
				{
					kingSquare(bx, by);
					kingWay(bx, by);
					for (int i = 0; i < 9; i++)
						if (ex == kingPos[i].x && ey == kingPos[i].y)
						{
							flag = true;
							break;
						}
					if (!flag)
					{
						getStep(ex, ey, pType, false);
						KingW_2 = true;
					}
					else
					{
						if (board[ey][ex] != 0)
							number--;
						if (turn % 2 != 0)
							rkrvk[0] = 0;
						if (turn % 2 == 0)
							rkrvk[3] = 0;
						getStep(ex, ey, pType, true);
					}
				}
				else
				{
					KingW_1 = true;
					rewriter(board, stepBack);
					SetPos();
					isWarn = true;
				}
			}	
			break;
		case 6: //пешка
			if (abs(by - ey) == 1 && abs(bx - ex) == 1 && board[ey][ex] != 0) //бьет по диагонали, если в конечной клетке есть фигура
			{
				number--;
				getStep(ex, ey, pType, true);
			}
			else if (board[ey][ex] != 0) //ход невозможен если путь занят
				getStep(ex, ey, pType, false);
			else if (by - ey == 1 && bx == ex) //ходит на одну клетку вперед
				getStep(ex, ey, pType, true);
			else if (by == 6 && by - ey == 2 && board[by - 1][bx] == 0 && bx == ex) // ходит на две клетки с начальной позиции, если путь свободен
				getStep(ex, ey, pType, true);
			else
				getStep(ex, ey, pType, false);
			if (ey == 0)
			{
				px = ex;
				py = 7 - ey;
				endLine = true;
			}
			break;
	}
}

void draway(int x, int y, int type)
{
	int comb[8] = { 1, 1, -1, -1, -2, 2, 2, -2 };
	bool rakirovka;
	switch (abs(type))
	{
	case 1:
		for (int n = x - 1; n >= 0; n--)//left
			if (F4(n, y, type))
				break;
		for (int n = x + 1; n < board_size; n++)//right
			if (F4(n, y, type))
				break;
		for (int n = y - 1; n >= 0; n--)//up
			if (F4(x, n, type))
				break;
		for (int n = y + 1; n < board_size; n++)//down
			if (F4(x, n, type))
				break;
		break;
	case 2:
		int m1, n1;
		for (int i = 0; i < 8; i++)
		{
			m1 = x + comb[i];
			n1 = y + comb[7 - i];
			if(m1 >= 0 && m1 < board_size && n1 >= 0 && n1 < board_size)
				F4(m1, n1, type);
		}
		break;
	case 3:
		for (int n = x + 1, m = y - 1; n < board_size && m >= 0 && m < board_size && n >= 0; n++, m--)//up right
				if (F4(n, m, type))
					break;
		for (int n = x - 1, m = y - 1; n < board_size && m >= 0 && m < board_size && n >= 0; n--, m--)//up left
				if (F4(n, m, type))
					break;
		for (int n = x - 1, m = y + 1; n < board_size && m >= 0 && m < board_size && n >= 0; n--, m++)//down left
				if (F4(n, m, type))
					break;
		for (int n = x + 1, m = y + 1; n < board_size && m >= 0 && m < board_size && n >= 0; n++, m++)//down right
				if (F4(n, m, type))
					break;
		break;
	case 4:
		for (int n = x - 1; n >= 0; n--)//left
			if (F4(n, y, type))
				break;
		for (int n = x + 1; n < board_size; n++)//right
			if (F4(n, y, type))
				break;
		for (int n = y - 1; n >= 0; n--)//up
			if (F4(x, n, type))
				break;
		for (int n = y + 1; n < board_size; n++)//down
			if (F4(x, n, type))
				break;
		for (int n = x + 1, m = y - 1; n < board_size && m >= 0 && m < board_size && n >= 0; n++, m--)//up right
			if (F4(n, m, type))
				break;
		for (int n = x - 1, m = y - 1; n < board_size && m >= 0 && m < board_size && n >= 0; n--, m--)//up left
			if (F4(n, m, type))
				break;
		for (int n = x - 1, m = y + 1; n < board_size && m >= 0 && m < board_size && n >= 0; n--, m++)//down left
			if (F4(n, m, type))
				break;
		for (int n = x + 1, m = y + 1; n < board_size && m >= 0 && m < board_size && n >= 0; n++, m++)//down right
			if (F4(n, m, type))
				break;
		break;
	case 5:
		kingSquare(x, y);
		kingWay(x, y);
		for (int q = 0; q < 9; q++)
		{
			if (kingPos[q].x != -1 && kingPos[q].y != -1)
				if(board[kingPos[q].y][kingPos[q].x] == 0)
					way[kingPos[q].y][kingPos[q].x] = 1;
				else 
					way[kingPos[q].y][kingPos[q].x] = 2;
			way[y][x] = 0;
		}
		if (rkrvk[0] == 1 && turn % 2 != 0 && rkrvk[1] == 1)
		{
			rakirovka = true;
			for (int i = 3; i > 0; i--)
				if (board[7][i] != 0)
				{
					rakirovka = false;
					break;
				}
			if (rakirovka)
				way[7][2] = 1;
		}
		if (rkrvk[3] == 1 && turn % 2 == 0 && rkrvk[4] == 1)
		{
			rakirovka = true;
			for (int i = 2; i > 0; i--)
				if (board[7][i] != 0)
				{
					rakirovka = false;
					break;
				}
			if (rakirovka)
				way[7][2] = 1;
		}
		if (rkrvk[0] == 1 && turn % 2 != 0 && rkrvk[2] == 1)
		{
			rakirovka = true;
			for (int i = 5; i < 7; i++)
				if (board[7][i] != 0)
				{
					rakirovka = false;
					break;
				}
			if (rakirovka)
				way[7][6] = 1;
		}
		if (rkrvk[3] == 1 && turn % 2 == 0 && rkrvk[4] == 1)
		{
			rakirovka = true;
			for (int i = 5; i < 7; i++)
				if (board[7][i] != 0)
				{
					rakirovka = false;
					break;
				}
			if (rakirovka)
				way[7][6] = 1;
		}
		break;
	case 6:
		if (board[y - 1][x + 1] < 0 && type > 0)
			way[y - 1][x + 1] = 2;
		if (board[y - 1][x + 1] > 0 && type < 0)
			way[y - 1][x + 1] = 2;
		if (board[y - 1][x - 1] < 0 && type > 0)
			way[y - 1][x - 1] = 2;
		if (board[y - 1][x - 1] > 0 && type < 0)
			way[y - 1][x - 1] = 2;
		if (board[y-1][x] == 0)
			way[y - 1][x] = 1;
		if (y == 6 && board[y - 2][x] == 0)
			way[y - 2][x] = 1;
		break;
	}
}

int main()
{
	RenderWindow window(VideoMode(504, 504), "CHESS");
	Texture t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11;
	t0.loadFromFile("black.png");
	t1.loadFromFile("figures.png");
	t2.loadFromFile("board.png");
	t3.loadFromFile("pown.png");
	t4.loadFromFile("bishop.png");
	t5.loadFromFile("rook.png");
	t6.loadFromFile("knight.png");
	t7.loadFromFile("queen.png");
	t8.loadFromFile("king.png");
	t9.loadFromFile("white.png");
	t10.loadFromFile("back.jpg");
	t11.loadFromFile("king1.png");
	Sprite Board(t2), PawnW(t3), BishopW(t4), RookW(t5), KnightW(t6), QueenW(t7), KingW1(t11), Pieces(t1);
	Sprite KingW2(t8), WhiteW(t9), BlackW(t0), Back(t10), Rook(t1), Knight(t1), Bishop(t1), Queen(t1);
	RectangleShape rect(Vector2f(50, 50));
	rect.setFillColor(Color::Transparent);
	rect.setOutlineThickness(3);
	for (int i = 0; i < number; i++)
		f[i].setTexture(t1);
	bool isMove = false, released = false;
	float dx = 0, dy = 0;
	int n = 0, pType = 0;
	SetPos();
	Vector2i oldPos(0, 0);
	while (window.isOpen())
	{
		Vector2i pos = Mouse::getPosition(window);
		int x = (pos.x - Frame) / Size;
		int y = (pos.y - Frame) / Size;
		Event e;
		while (window.pollEvent(e))
		{
			if (e.type == Event::Closed)
				window.close();

			if (e.type == Event::KeyPressed)
				if (e.key.code == Keyboard::Escape)
					if (rewrite)
					{
						turn--;
						rewriter(board, stepBack);
						SetPos();
						rewrite = false;
					}

			if (e.type == Event::MouseMoved)
				if (endLine)
				{
					if (Rook.getGlobalBounds().contains(pos.x, pos.y))
						Rook.setScale(1.1, 1.1);
					else
						Rook.setScale(1, 1);
					if (Knight.getGlobalBounds().contains(pos.x, pos.y))
						Knight.setScale(1.1, 1.1);
					else
						Knight.setScale(1, 1);
					if (Bishop.getGlobalBounds().contains(pos.x, pos.y))
						Bishop.setScale(1.1, 1.1);
					else
						Bishop.setScale(1, 1);
					if (Queen.getGlobalBounds().contains(pos.x, pos.y))
						Queen.setScale(1.1, 1.1);
					else
						Queen.setScale(1, 1);
				}

			if (e.type == Event::MouseButtonPressed)
				if (e.key.code == Mouse::Left)
				{
					for (int i = 0; i < number; i++)
						if (f[i].getGlobalBounds().contains(pos.x, pos.y) && isWarn == false)
						{
							n = i;
							isMove = true;
							released = true;
							oldPos.x = (pos.x - Frame) / Size;
							oldPos.y = (pos.y - Frame) / Size;
							pType = board[oldPos.y][oldPos.x];
							rewriter(stepBack, board);
							board[oldPos.y][oldPos.x] = 0;
							dx = pos.x - f[i].getPosition().x;
							dy = pos.y - f[i].getPosition().y;
						}
					int i = pType > 0 ? 1 : -1;
					if (endLine && Rook.getGlobalBounds().contains(pos.x, pos.y))
					{
						endLine = false;
						board[py][px] = i * 1;
						SetPos();
					}
					if (endLine && Knight.getGlobalBounds().contains(pos.x, pos.y))
					{
						endLine = false;
						board[py][px] = i * 2;
						SetPos();
					}
					if (endLine && Bishop.getGlobalBounds().contains(pos.x, pos.y))
					{
						endLine = false;
						board[py][px] = i * 3;
						SetPos();
					}
					if (endLine && Queen.getGlobalBounds().contains(pos.x, pos.y))
					{
						endLine = false;
						board[py][px] = i * 4;
						SetPos();
					}
					draway(oldPos.x, oldPos.y, pType);
				}
			if (e.type == Event::MouseButtonReleased)
				if (e.key.code == Mouse::Left)
				{
					isMove = false;
					if (turn % 2 == 0 && pType > 0)
					{
						pType = 7;
						isWarn = true;
					}
					else if (turn % 2 != 0 && pType < 0)
					{
						pType = 8;
						isWarn = true;
					}
					if (board[y][x] * pType > 0)
					{
						rewriter(board, stepBack);
						SetPos();
					}
					else
					{
						if (released)
							stepper(oldPos.x, oldPos.y, x, y);
						released = false;
					}
					if (isWarn && PawnW.getGlobalBounds().contains(pos.x, pos.y))
						isWarn = false;
					if (isWarn && BishopW.getGlobalBounds().contains(pos.x, pos.y))
						isWarn = false;
					if (isWarn && RookW.getGlobalBounds().contains(pos.x, pos.y))
						isWarn = false;
					if (isWarn && KnightW.getGlobalBounds().contains(pos.x, pos.y))
						isWarn = false;
					if (isWarn && QueenW.getGlobalBounds().contains(pos.x, pos.y))
						isWarn = false;
					if (isWarn && KingW1.getGlobalBounds().contains(pos.x, pos.y))
					{
						isWarn = false;
						KingW_1 = false;
					}
					if (isWarn && KingW2.getGlobalBounds().contains(pos.x, pos.y))
					{
						isWarn = false;
						KingW_2 = false;
					}
					if (isWarn && BlackW.getGlobalBounds().contains(pos.x, pos.y))
						isWarn = false;
					if (isWarn && WhiteW.getGlobalBounds().contains(pos.x, pos.y))
						isWarn = false;
					for (int i = 0; i < board_size; i++)
						for (int j = 0; j < board_size; j++)
							way[i][j] = 0;
				}
			if (isMove)
				f[n].setPosition(pos.x - dx, pos.y - dy);
		}
		window.clear();
		window.draw(Back);
		if (turn % 2 == 0)
			Board.setTextureRect(IntRect(504, 0, -504, 504));
		else
			Board.setTextureRect(IntRect(0, 0, 504, 504));
		window.draw(Board);
		for (int i = 0; i < board_size; i++)
			for (int j = 0; j < board_size; j++)
				if (way[i][j] == 1)
				{
					rect.setOutlineColor(Color(0, 125, 125));
					rect.setPosition(Size*j + Frame + 4, Size*i + Frame + 4);
					window.draw(rect);
				}
				else if (way[i][j] == 2)
				{
					rect.setOutlineColor(Color(255, 0, 0));
					rect.setPosition(Size*j + Frame + 4, Size*i + Frame + 4);
					window.draw(rect);
				}	
		for (int i = 0; i < number; i++)
			window.draw(f[i]);
		if (isWarn)
		{
			switch (abs(pType))
			{
			case 1:
				RookW.setPosition(120, 220);
				window.draw(RookW);
				break;
			case 2:
				KnightW.setPosition(120, 205);
				window.draw(KnightW);
				break;
			case 3:
				BishopW.setPosition(120, 222);
				window.draw(BishopW);
				break;
			case 4:
				QueenW.setPosition(120, 220);
				window.draw(QueenW);
				break;
			case 5:
				if (KingW_2 == true)
				{
					KingW2.setPosition(120, 220);
					window.draw(KingW2);
				}
				if (KingW_1 == true)
				{
					KingW1.setPosition(120, 200);
					window.draw(KingW1);
				}
				break;
			case 6:
				PawnW.setPosition(120, 200);
				window.draw(PawnW);
				break;
			case 7:
				BlackW.setPosition(120, 220);
				window.draw(BlackW);
				break;
			case 8:
				WhiteW.setPosition(120, 220);
				window.draw(WhiteW);
				break;
			}
		}
		if (endLine)
		{
			int i = pType > 0 ? 1 : 0;
			Back.setTextureRect(IntRect(0, Size, 290, 100));
			Back.setPosition(109, 202);
			Rook.setTextureRect(IntRect(0, i*Size, Size, Size));
			Knight.setTextureRect(IntRect(56, i*Size, Size, Size));
			Bishop.setTextureRect(IntRect(110, i*Size, Size, Size));
			Queen.setTextureRect(IntRect(166, i*Size, Size, Size));
			Rook.setPosition(120, 224);
			Knight.setPosition(185, 224);
			Bishop.setPosition(250, 224);
			Queen.setPosition(315, 224);
			window.draw(Back);
			window.draw(Rook);
			window.draw(Knight);
			window.draw(Bishop);
			window.draw(Queen);
		}
		if (mat)
		{
			Back.setTextureRect(IntRect(0, 0, 504, 504));
			window.draw(Back);
		}
		window.display();
	}
	return 0;
}