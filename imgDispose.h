#pragma once
#include <opencv.hpp>
#include<string>

using namespace cv;
using namespace std;

#define AREA 300		//字符提取 最低面积

class Dispose
{
private:

public:
	static Mat Binaryzation(Mat img);
	static Mat Shape(Mat img, Size size);
	static Mat RegulateBrightness(Mat img, double beta);
	static int SearchTargetRect(Mat input, Rect* recs, double* an);
	static bool imagetostring(Mat input);
	static float GetRecognitionDegree(Mat input1, Mat input2);
	static Mat DotDelete(Mat input);
	static Mat DotDelete(Mat input,int  fa);
	static char MatchingChar(Mat input);
	static std::string MatchingSinogram(Mat input);
	static std::string Distill(Mat a);
	static Mat DeleteDown(Mat input, int fa);
};

//二值化
Mat Dispose::Binaryzation(Mat img)
{
	//保存灰度化
	Mat Resize = img.clone();

	Mat BinRGBImg = img.clone();  //二值化之后的图像
	//保存二值化图像
	Mat ER = BinRGBImg.clone();
	cvtColor(Resize, BinRGBImg, COLOR_BGR2HSV);	//hsv

	for (int i = 0; i < BinRGBImg.rows; i++)   //通过颜色分量将图片进行二值化处理
	{
		for (int j = 0; j < BinRGBImg.cols; j++)
		{
			int H = BinRGBImg.at<Vec3b>(i, j)[0] * 2;
			float S = (float)BinRGBImg.at<Vec3b>(i, j)[1] / 255;
			float V = (float)BinRGBImg.at<Vec3b>(i, j)[2] / 255;

			if ((H > 150 && H < 250) && (S > 0.45 && V > 0.4))
			{
				ER.at<Vec3b>(i, j) = 255;	//白
			}
			else
			{
				ER.at<Vec3b>(i, j) = 0;	//黑
			}
		}
	}
	//最后二值化
	cvtColor(ER, ER, COLOR_RGB2GRAY);
	ER = (ER > 2);

	return ER;
}

//形态学处理 （膨胀 and 腐蚀） 二值化图	size:处理大小
Mat Dispose::Shape(Mat img, Size size=Size(2,2))
{
	//形态学处理	
	Mat dilate_image, erode_image;
	Mat element = getStructuringElement(MORPH_RECT, size);
	Mat elementX = getStructuringElement(MORPH_RECT, Size(size.width, 1));
	Mat elementY = getStructuringElement(MORPH_RECT, Size(1, size.height));
	Point point(-1, -1);

	dilate(img, dilate_image, element, point, 2);			//膨胀 
	dilate(dilate_image, dilate_image, element, point, 2);

	//图像小了（车牌比较大） 多膨胀一次
	if (size.width > 2 || size.height > 2)
	{
		dilate(dilate_image, dilate_image, element, point, 2);
	}
	dilate(dilate_image, dilate_image, elementY, point, 2);
	erode(dilate_image, erode_image, elementY, point, 2);	//腐蚀
	erode(erode_image, erode_image, elementY, point, 2);
	erode(erode_image, erode_image, elementY, point, 2);
	return erode_image;
}

//调节亮度  beta亮度调节度(大于0 升高)
Mat Dispose::RegulateBrightness(Mat img,double beta)
{
	//输出图像
	Mat dst = Mat::zeros(img.size(), img.type());

	Mat m1;
	img.convertTo(m1, CV_32F);
	for (int row = 0; row < img.rows; row++) {
		for (int col = 0; col < img.cols; col++) {
			if (img.channels() == 3) {
				float b = m1.at<Vec3f>(row, col)[0];//读三通道像素，不能直接用Vec3f，要用Vec3b，因为是opencv是8UC的数据,除非提前src.convertTo转换类型
				float g = m1.at<Vec3f>(row, col)[1];//green
				float r = m1.at<Vec3f>(row, col)[2];//red
				dst.at<Vec3b>(row, col)[0] = saturate_cast<uchar>(b + beta); //写像素，修改亮度和对比度
				dst.at<Vec3b>(row, col)[1] = saturate_cast<uchar>(g + beta);
				dst.at<Vec3b>(row, col)[2] = saturate_cast<uchar>(r + beta);
			}
			else if (img.channels() == 1) {
				float v = img.at<uchar>(row, col);    //读一通道像素
				dst.at<uchar>(row, col) = saturate_cast<uchar>(v * 0.85 + beta);
			}
		}
	}
	return dst;
}

//搜索目标矩形
int Dispose::SearchTargetRect(Mat input,Rect* recs,double* an)
{
	int count = 0;	//保存arrs  的下标
	//定义储存变量
	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierarchy;
	findContours(input, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));

	//遍历目标对象数组 寻找符合条件的目标
	for (int i = 0; i < hierarchy.size(); i++)
	{
		//定义矩形
		Rect rec = boundingRect(contours[i]);		//正矩形
		RotatedRect r = minAreaRect(contours[i]);	//斜矩形  用来获取倾斜角度
		double area = rec.area();
		double width = rec.width;
		double height = rec.height;

		//初步筛选 宽高比 面积大小 轮廓面积与矩形的面积比
		if ((width / height > 2.5 && width / height < 4) && (area > 1000 && area < 100000) && (contourArea(contours[i])) / area > 0.6)
		{
			recs[count] = rec;
			an[count++] = r.angle;
		}
	}

	return count;
}

//将目标图上的字符转换为字符型数据
bool Dispose::imagetostring(Mat input)
{
	return false;
}

//获取两张图片的相似度
float Dispose::GetRecognitionDegree(Mat input1,Mat input2)
{
	//缩放
	resize(input1, input1, Size(20, 40));
	resize(input2, input2, Size(20, 40));
	int imcount = 0;
	for (int i = 0; i < input1.rows; i++)
	{
		uchar* datap = input1.ptr<uchar>(i);
		uchar* dataq = input2.ptr<uchar>(i);
		for (int j = 0; j < input1.cols; j++)
		{
			if (datap[j] == dataq[j])
			{
				imcount++;
			}
		}
	}
	return (float)imcount / (input1.rows*input1.cols);
}

//消除不符合要求的边线
Mat Dispose::DotDelete(Mat input)
{
	int acount = 0;
	for (int i = 0; i < 8; i++)//去左边线
	{

		acount = 0;
		for (int j = 3; j < input.rows - 3; j++)
		{
			uchar* dd = input.ptr<uchar>(j);
			if (dd[0] == 255)
			{
				acount++;
			}
			if (acount > 3)
			{
				input = input(Range::all(), Range(1, input.cols));
				break;
			}
		}
		if (acount == 0)
		{
			break;
		}
	}
	return input;
}

//消除不符合要求的边线
Mat Dispose::DotDelete(Mat input, int fa)
{
	int acount = 0;
	for (int i = 0; i < 8; i++)//去左边线
	{

		acount = 0;
		for (int j = 0; j < input.rows; j++)
		{
			uchar* dd = input.ptr<uchar>(j);
			if (dd[0] != fa)
			{
				acount++;
			}
		}
		if (acount == 0)
		{
			input = input(Range::all(), Range(1, input.cols));
		}
		else break;
	}
	return input;
}

//匹配车牌字符
char Dispose::MatchingChar(Mat input)
{
	Mat q;
	String s = "";
	float f = 0.5, f1;
	int i1 = 0, j1 = 0;
	char c;
	for (int i = 0; i <= 34; i++)
	{
		s = "plate/" + to_string(i) + ".jpg";  //图片地址
		q = imread(s, 0);
		//二值化
		q = (q > 9);
		f1 = GetRecognitionDegree(input, q);
		if (f1 > f)
		{
			f = f1;
			i1 = i;
		}
	}
	switch (i1)   //i相当于1 o相当于0 所以没有
	{
	case 0:	c = '0'; break;
	case 1:	c = '1'; break;
	case 2:	c = '2'; break;
	case 3:	c = '3'; break;
	case 4:	c = '4'; break;
	case 5:	c = '5'; break;
	case 6:	c = '6'; break;
	case 7:	c = '7'; break;
	case 8:	c = '8'; break;
	case 9:	c = '9'; break;
	case 10:	c = 'A'; break;
	case 11:	c = 'B'; break;
	case 12:	c = 'C'; break;
	case 13:	c = 'D'; break;
	case 14:	c = 'E'; break;
	case 15:	c = 'F'; break;
	case 16:	c = 'G'; break;
	case 17:	c = 'H'; break;
	case 18:	c = 'I'; break;
	case 19:	c = 'J'; break;
	case 20:	c = 'K'; break;
	case 21:	c = 'L'; break;
	case 22:	c = 'M'; break;
	case 23:	c = 'N'; break;
	case 24:	c = '0'; break;
	case 25:	c = 'P'; break;
	case 26:	c = 'Q'; break;
	case 27:	c = 'R'; break;
	case 28:	c = 'S'; break;
	case 29:	c = 'T'; break;
	case 30:	c = 'U'; break;
	case 31:	c = 'V'; break;
	case 32:	c = 'W'; break;
	case 33:	c = 'X'; break;
	case 34:	c = 'Y'; break;
	case 35:	c = 'Z'; break;
	//defaul9t: c = '*';  //为*号表示读取的字符不存在;
	//	break;
	}
	return c;
}

//匹配车牌汉字
std::string Dispose::MatchingSinogram(Mat input)
{
	Mat q;
	String s = ""; //存地址
	float f = 0.4, f1;
	int i1 = 99, j1 = 0;
	string c = ""; //存汉字
	for (int i = 0; i <= 30; i++)
	{
		s = "hanzi/" + to_string(i) + ".jpg";  //图片地址
		q = imread(s, 0);
		//二值化
		q = (q > 9);
		f1 = GetRecognitionDegree(input, q);
		if (f1 > f)
		{
			f = f1;
			i1 = i;
		}
	}
	switch (i1)
	{
	case 0:	c = "藏"; break;
	case 1:	c = "川"; break;
	case 2:	c = "鄂"; break;
	case 3:	c = "甘"; break;
	case 4:	c = "赣"; break;
	case 5:	c = "贵"; break;
	case 6:	c = "挂"; break;
	case 7:	c = "黑"; break;
	case 8:	c = "吉"; break;
	case 9:	c = "冀"; break;
	case 10:	c = "津"; break;
	case 11:	c = "晋"; break;
	case 12:	c = "京"; break;
	case 13:	c = "辽"; break;
	case 14:	c = "泸"; break;
	case 15:	c = "鲁"; break;
	case 16:	c = "蒙"; break;
	case 17:	c = "闽"; break;
	case 18:	c = "宁"; break;
	case 19:	c = "青"; break;
	case 20:	c = "琼"; break;
	case 21:	c = "陕"; break;
	case 22:	c = "苏"; break;
	case 23:	c = "皖"; break;
	case 24:	c = "湘"; break;
	case 25:	c = "新"; break;
	case 26:	c = "渝"; break;
	case 27:	c = "豫"; break;
	case 28:	c = "粤"; break;
	case 29:	c = "云"; break;
	case 30:	c = "浙"; break;
	default: c = '-';  //为*号表示读取的字符不存在;
	}
	return c;
}

//字符 and 汉字提取
std::string Dispose::Distill(Mat a)
{
	/*imshow("a", a);
	waitKey(0);*/
	string s = "-"; //	- 代表没有找到；
	char c1[10] = { '-','-', '-', '-', '-', '-', '-'};  //记录车牌上的字符
	int c2[10] = { 0,0,0,0,0,0,0 }; //记录车牌上字符的坐标
	int count = 0;
	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierarchy;
	findContours(a, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
	/*Mat dstImage = Mat::zeros(a.size(), CV_8UC3);*/
	for (int i = 0; i < hierarchy.size(); i++)
	{
		//设置颜色
		Scalar color = Scalar(255, 100, 255);
		//定义矩形
		Rect rec = boundingRect(contours[i]);
		double area = rec.area();
		double width = rec.width;
		double height = rec.height;

		if (area > AREA && area < 1700 && height > 20 && rec.x > 30 && rec.x < a.cols - 7)
		{
			//画矩形
			//rectangle(a, rec, color);
			//cout <<count<<"的坐标是："<< rec.x << endl;
			Mat m = a(rec);
			m = m(Range(1, m.rows - 1), Range(1, m.cols - 1));
			//imwrite("plate2/1-" + to_string(count) + ".jpg", m);
		/*	imshow("1-" + to_string(count), m);
			waitKey(0);*/
			/*	c1[count] = shibie_zifu(m);*/
			c1[count] = Dispose::MatchingChar(m);
			c2[count] = rec.x;
			//字符填黑
			rectangle(a, rec, Scalar(0, 0, 0), -1);
			count++;
		}
	}



	if (count != 7 && s == "-")
	{
		Mat test = a.clone();
		Mat element = getStructuringElement(MORPH_RECT, Size(5, 5)); //设置形态学处理窗的大小
		dilate(test, test, element);	//膨胀
	/*	imshow("汉字", test);
		waitKey(0);*/
		/***************************扫描汉字*******************************/
		findContours(test, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
		for (int i = 0; i < hierarchy.size(); i++)
		{
			//设置颜色
			Scalar color = Scalar(255, 100, 255);
			//定义矩形
			Rect rec = boundingRect(contours[i]);
			double area = rec.area();
			double width = rec.width;
			double height = rec.height;

			if (area > AREA + 300 && area < 2000 && height > 40 && rec.x < a.cols - 100)
			{
				//画矩形
				/*rectangle(a, rec, color);*/
				//cout <<count<<"的坐标是："<< rec.x << endl;
				Mat m = a(rec);
				m = m(Range(2, m.rows - 2), Range(2, m.cols - 2));
				//imwrite("plate2/1-" + to_string(count) + ".jpg", m);
				//imshow("1-" + to_string(count), m);
				//waitKey(0);
				/*s = shibie_hanzi(m);*/
				resize(m, m, Size(20, 40));
				m = DotDelete(m, 0);
				m = DeleteDown(m, 0);
				s = Dispose::MatchingSinogram(m);
				c2[count] = rec.x;
				/*rectangle(a, rec, Scalar(0, 0, 0), -1);*/
				count++;
			}
			//画轮廓
			//drawContours(dstImage, contours, i, color, 3, 8, hierarchy, 0);
		}
	}
	if (s=="-")
	{
		return "--";
	}
	/************************车牌排序********************************/
	int tmp = 0;
	char tmpc = '-';
	for (int i = 0; i < 6 - 1; i++)
	{
		for (int j = i + 1; j < 6; j++)
		{
			if (c2[i] > c2[j])
			{
				tmp = c2[i];
				c2[i] = c2[j];
				c2[j] = tmp;
				/*****字符交换****/
				tmpc = c1[i];
				c1[i] = c1[j];
				c1[j] = tmpc;
			}
		}
		s += c1[i];
	}
	s += c1[5];
	//cout << "车牌号是：" << s << endl;
	return s;
}

//删除下边线
Mat Dispose::DeleteDown(Mat input, int fa)
{
	int acount = 0;
	for (int i = input.rows-1; i > input.rows - 8; i--)//去下边线
	{
		acount = 0;
		for (int j = 0; j < input.cols; j++)
		{
			uchar* dd = input.ptr<uchar>(i);
			if (dd[j] != fa)
			{
				acount++;
			}
		}
		if (acount == 0)
		{
			input = input(Range(0,input.rows-1), Range::all());
		}
		else break;
	}
	return input;
}