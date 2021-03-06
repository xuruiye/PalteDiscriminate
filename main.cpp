#include<graphics.h>
#include <opencv.hpp>
#include "core/core.hpp"    
#include "imgproc/imgproc.hpp"    
#include<windows.h>
#include <sys/timeb.h>
#include<string>
#include"Buttons.h"
#include"imgDispose.h"


using namespace std;
using namespace cv;
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) 

HWND init;			//文件对话框 句柄
#define N3 300		//车牌字符识别面积阀值

//函数声明处
void InitInterface();
void LoadSourcePlate(LPCTSTR str);
void LoadPlate(LPCTSTR str);
void OutPlate(string number);
void OutTime(string time);
void AddClick(Mat *);
//以上为界面设计函数 以下为图片识别 提取函数
void opencvdispose(Mat *input);
string Imagetostring(Mat a, int Er = 109);
int Gettimeconsuming(SYSTEMTIME t1, SYSTEMTIME t2);
bool Boolstring(string str);

//主函数
int main()
{
	InitInterface();

	Button arr[2] = {
		{550,200,100,40,"添加图片"} ,
		{550,300,100,40,"开始识别"}
	};
	
	BUTTONS buts;
	buts.ButArrCreate(2, arr);
	buts.showBut();

	MOUSEMSG m;
	Mat input;
	while (1)
	{
		//检测是否有鼠标消息
		while (MouseHit())
		{
			m = GetMouseMsg();
			if (m.uMsg == WM_LBUTTONUP)		//左键释放
			{
				
				switch (buts.butTop(m.x, m.y))
				{
				case 0: AddClick(&input); ; break;
				case 1: buts.SetButtxt(1,"正在识别"); opencvdispose(&input); buts.SetButtxt(1, "开始识别"); break;
				default: break;
				}
			}
			else if (m.uMsg == WM_LBUTTONDOWN)	//左键按下
			{
				buts.butDown(m.x, m.y);
			}
		}
	}
	return 0;
}

//初始化界面
void InitInterface()
{
	//新建绘图窗口
	init=initgraph(700, 450);

	//设置刷子颜色
	setbkcolor(RGB(227, 231, 216));//BLUE
	cleardevice();//刷子

	setbkmode(TRANSPARENT);
	setcolor(RGB(10, 10, 10));
	settextcolor(RGB(200, 200, 200));
	setfillcolor(RGB(255, 255, 255));

	bar(10, 10, 500, 400);
	rectangle(10, 10, 500, 400);
	//输出文字
	RECT source = { 10, 10, 500, 400 };		//字符串输出位置
	drawtext("原图像", &source, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	
	bar(510, 10, 690, 75);
	rectangle(510, 10, 690, 75);
	RECT plate = { 510, 10, 690, 75 };
	drawtext("车牌区域", &plate, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	bar(510, 120, 635, 150);
	RECT number = { 510,120,635,150 };
	rectangle(number.left, number.top, number.right, number.bottom);
	drawtext("例：陕K88888", &number, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT time = { 640,120,690,150 };
	bar(640, 120, 690, 150);
	rectangle(time.left, time.top, time.right, time.bottom);
	drawtext("0.000", &time, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	setcolor(BLACK);
	RECT numbertitle = { 510,100,645,120 };
	drawtext("车牌号:", &numbertitle, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	RECT timetitle = { 640,100,690,120 };
	drawtext("耗时(s)", &timetitle, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

}

//加载源图片
void LoadSourcePlate(LPCTSTR str)
{
	IMAGE yuan;
	loadimage(&yuan, str, 487, 387);
	putimage(12,12,&yuan);
}

//加载车牌区域图
void LoadPlate(LPCTSTR str)
{
	IMAGE plate;
	loadimage(&plate, str, 177, 62);
	putimage(512,12,&plate);
}

//输出车牌号码
void OutPlate(string numbers)
{
	setcolor(BLACK);
	RECT number = { 510,120,635,150 };
	bar(511, 121, 634, 149);
	drawtext(numbers.c_str(), &number, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

//输出耗时
void OutTime(string times)
{
	setcolor(BLACK);
	RECT time = { 640,120,690,150 };
	bar(641, 121, 689, 149);
	drawtext(times.c_str(), &time, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

//添加图片单击事件
void AddClick(Mat *input)
{
	//新建一个对话窗口，选择文件
	OPENFILENAME ofn = { 0 };
	char szFile[MAX_PATH];
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = init;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nFilterIndex = 1;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "*.jpg\0*.jpg\0*.jpeg\0*.jpeg\0\0";
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	if (GetOpenFileName(&ofn))
	{
		LoadSourcePlate(ofn.lpstrFile);
		*input = imread(ofn.lpstrFile);
		return ;
	}
	else
	{
		MessageBoxA(NULL, "未选择任何文件！", "提示", MB_OK | MB_SETFOREGROUND);
		return ;
	}
}

//开始是识别单击事件
//opencv 处理
void opencvdispose(Mat *input)
{
	//获取识别开始时 时间
	SYSTEMTIME t1;
	GetLocalTime(&t1);

	Mat Originalimg = (*input).clone();
	if (Originalimg.empty())
	{
		//对话框
		MessageBox(NULL, "请先添加图片！！！", "错误提示", MB_OK | MB_SETFOREGROUND);
		return ;
	}

	//尺寸变换图 变化到规定大小
	Mat ResizeImg;
	resize(Originalimg, ResizeImg, Size(680, 680 * Originalimg.rows / Originalimg.cols));

	//定义保存二值化之后的图像
	Mat BinRGBImg = ResizeImg.clone();
	BinRGBImg=Dispose::Binaryzation(BinRGBImg);	//二值化处理 针对蓝色

	//定义保存心态学处理图像 （腐蚀 And 膨胀）
	Mat BinOriImg= BinRGBImg;
	BinOriImg = Dispose::Shape(BinRGBImg, Size(Originalimg.cols < 300 ? 8 : 4, Originalimg.rows < 300 ? 2 : 1));

	//初步寻找目标矩形对象 保存在数组中
	Rect recs[5] = {};	//保存符合条件的所有矩形
	double an[5] = {};	//对应矩形的旋转角度
	int count = 0;		//保存数组个数
	count = Dispose::SearchTargetRect(BinOriImg, recs, an);
	
	//如果 对象数组为0 失识别失败
	if (count == 0)
	{
		MessageBox(NULL, "识别失败", "错误提示", MB_OK | MB_SETFOREGROUND);
		return;
	}

	bool dispose = false;
	for (int i = 0; i < count; i++)
	{
		//截取对象
		Mat outputimag = ResizeImg(recs[i]);
		//保存目标为图像文件 IMWRITE_JPEG_QUALITY
		imwrite("plate.jpg", outputimag);

		//字符切割  图像对象对比识别 输出结果 
		string platestr = "";
		platestr = Imagetostring(outputimag);

		//判断是否识别成功
		dispose = Boolstring(platestr);
		if (dispose)
		{
			LoadPlate("plate.jpg");
			OutPlate(platestr);
			break;
		}
		else
		{
			for (int j = 101; j < 240; j += 5)
			{
				outputimag = Dispose::RegulateBrightness(outputimag,-3.1);
				platestr = Imagetostring(outputimag, j);
				dispose = Boolstring(platestr);
			
				if (dispose)
				{
					LoadPlate("plate.jpg");
					OutPlate(platestr);
					break;
				}
			}
			if (dispose)break;
			else continue;
		}
	}

	if (!dispose)
	{
		MessageBox(NULL, "识别失败", "错误提示", MB_OK | MB_SETFOREGROUND);
				return;
	}
	//end 识别

	//获取识别结束时 时间
	SYSTEMTIME t2;
	GetLocalTime(&t2);

	//问题代码  获取时间差
	double timecount = (double)Gettimeconsuming(t1, t2) / 1000;

	//将整形时间差 转换为字符串输出
	string str;
	str = to_string(timecount);
	//输出时间 保存三位小数 0.000
	OutTime(str.substr(0,5));
}

//车牌图片 转 车牌号
string Imagetostring(Mat a,int Er)
{
	//Mat a = imread("outputimag.jpg",0);
	cvtColor(a, a, COLOR_RGB2GRAY);
	resize(a, a, Size(240, 70));	//二值化
	a = (a > Er);
	a = Dispose::DotDelete(a);	//消除小点
	a = a(Range(3, a.rows - 3), Range(0, a.cols - 4));//删除上下边线										  
	flip(a, a, 1);				//翻转 1 180°
	a = Dispose::DotDelete(a);	//消除小点
	flip(a, a, 1);				//翻转 回来
	string plate = Dispose::Distill(a);
	return plate;
}

//获取时间差 返回毫秒
int Gettimeconsuming(SYSTEMTIME t1, SYSTEMTIME t2)
{
	//获取时间差
	int H = t2.wHour - t1.wHour;	//时
	int M = t2.wMinute - t1.wMinute;	//分
	int S = t2.wSecond - t1.wSecond;	//秒
	int SS = t2.wMilliseconds - t1.wMilliseconds;	//豪秒

	//返回时间差值（毫秒）
	return H*3600000 + M*60000 + S*1000 + SS;
}

//判断字符串是否符合要求
bool Boolstring(string str)
{
	for (int j = 0; j < 7; j++)
	{
		string ch = str.substr(j, 1);
		if (ch == "-")
		{
			return false;
		}
	}
	return true;
}