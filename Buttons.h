#pragma once

//按钮结构体
struct Button
{
	int x;
	int y;
	int width;
	int height;
	LPCSTR txt;
};

class BUTTONS
{
private:
	Button *butArr;
	int butnumber;

public:
	BUTTONS();
	int getnumber();
	void SetButtxt(int index, LPCSTR txt);
	void ButArrCreate(int number, Button* Arr);
	void Butborder(int index);
	void showBut();
	int butTop(int x, int y);
	void butDown(int x, int y);
};

//按钮数组 初始化
BUTTONS::BUTTONS()
{
	butArr = NULL;
}

//获取按钮数
int BUTTONS::getnumber()
{
	return butnumber;
}

//设置按钮的文本
void BUTTONS::SetButtxt(int index, LPCSTR txt)
{
	butArr[index].txt = txt;
	showBut();
}

//新建 number 按钮数
void BUTTONS::ButArrCreate(int number, Button* Arr)
{
	//声请空间
	butArr = Arr;
	butnumber = number;
}

//边框 index 要设置边框的butArr下标索引
void BUTTONS::Butborder(int index)
{

}

//显示所有按钮
void BUTTONS::showBut()
{
	setcolor(BLACK);//设置边框颜色
	setfillcolor(getbkcolor());//设置填充颜色
	//画出所有按钮
	for (int i = 0; i < butnumber; i++)
	{
		RECT c = { butArr[i].x, butArr[i].y, butArr[i].x + butArr[i].width, butArr[i].y + butArr[i].height };
		//默认边框
		bar(c.left, c.top, c.right, c.bottom);
		rectangle(c.left, c.top, c.right, c.bottom);
		rectangle(c.left + 2, c.top + 2, c.right - 2, c.bottom - 2);
		drawtext(butArr[i].txt, &c, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
}

//某个按钮抬起  画动作 返回按钮索引
int BUTTONS::butTop(int x,int y)
{
	setcolor(getbkcolor());
	for (int i = 0; i < butnumber; i++)
	{
		RECT c = { butArr[i].x, butArr[i].y, butArr[i].x + butArr[i].width, butArr[i].y + butArr[i].height };
		rectangle(c.left + 3, c.top + 3, c.right - 3, c.bottom - 3);
		if (x >= c.left&&y > c.top&&x <= c.right - 3 &&y < c.bottom)
		{
			//返回对应的索引
			return i;
		}
	}
	//没找到  无效点击
	return -1;
}

//某个按钮按下  画动作
void BUTTONS::butDown(int x, int y)
{
	setcolor(BLACK);
	for (int i = 0; i < butnumber; i++)
	{
		RECT c = { butArr[i].x, butArr[i].y, butArr[i].x + butArr[i].width, butArr[i].y + butArr[i].height };
		if (x >= c.left&&y > c.top&&x <= c.right - 3 && y < c.bottom)
		{
			rectangle(c.left + 3, c.top + 3, c.right - 3, c.bottom - 3);
		}
	}
}
