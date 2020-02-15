#pragma once

//��ť�ṹ��
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

//��ť���� ��ʼ��
BUTTONS::BUTTONS()
{
	butArr = NULL;
}

//��ȡ��ť��
int BUTTONS::getnumber()
{
	return butnumber;
}

//���ð�ť���ı�
void BUTTONS::SetButtxt(int index, LPCSTR txt)
{
	butArr[index].txt = txt;
	showBut();
}

//�½� number ��ť��
void BUTTONS::ButArrCreate(int number, Button* Arr)
{
	//����ռ�
	butArr = Arr;
	butnumber = number;
}

//�߿� index Ҫ���ñ߿��butArr�±�����
void BUTTONS::Butborder(int index)
{

}

//��ʾ���а�ť
void BUTTONS::showBut()
{
	setcolor(BLACK);//���ñ߿���ɫ
	setfillcolor(getbkcolor());//���������ɫ
	//�������а�ť
	for (int i = 0; i < butnumber; i++)
	{
		RECT c = { butArr[i].x, butArr[i].y, butArr[i].x + butArr[i].width, butArr[i].y + butArr[i].height };
		//Ĭ�ϱ߿�
		bar(c.left, c.top, c.right, c.bottom);
		rectangle(c.left, c.top, c.right, c.bottom);
		rectangle(c.left + 2, c.top + 2, c.right - 2, c.bottom - 2);
		drawtext(butArr[i].txt, &c, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
}

//ĳ����ţ̌��  ������ ���ذ�ť����
int BUTTONS::butTop(int x,int y)
{
	setcolor(getbkcolor());
	for (int i = 0; i < butnumber; i++)
	{
		RECT c = { butArr[i].x, butArr[i].y, butArr[i].x + butArr[i].width, butArr[i].y + butArr[i].height };
		rectangle(c.left + 3, c.top + 3, c.right - 3, c.bottom - 3);
		if (x >= c.left&&y > c.top&&x <= c.right - 3 &&y < c.bottom)
		{
			//���ض�Ӧ������
			return i;
		}
	}
	//û�ҵ�  ��Ч���
	return -1;
}

//ĳ����ť����  ������
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
