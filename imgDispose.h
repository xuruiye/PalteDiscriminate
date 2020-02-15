#pragma once
#include <opencv.hpp>
#include<string>

using namespace cv;
using namespace std;

#define AREA 300		//�ַ���ȡ ������

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

//��ֵ��
Mat Dispose::Binaryzation(Mat img)
{
	//����ҶȻ�
	Mat Resize = img.clone();

	Mat BinRGBImg = img.clone();  //��ֵ��֮���ͼ��
	//�����ֵ��ͼ��
	Mat ER = BinRGBImg.clone();
	cvtColor(Resize, BinRGBImg, COLOR_BGR2HSV);	//hsv

	for (int i = 0; i < BinRGBImg.rows; i++)   //ͨ����ɫ������ͼƬ���ж�ֵ������
	{
		for (int j = 0; j < BinRGBImg.cols; j++)
		{
			int H = BinRGBImg.at<Vec3b>(i, j)[0] * 2;
			float S = (float)BinRGBImg.at<Vec3b>(i, j)[1] / 255;
			float V = (float)BinRGBImg.at<Vec3b>(i, j)[2] / 255;

			if ((H > 150 && H < 250) && (S > 0.45 && V > 0.4))
			{
				ER.at<Vec3b>(i, j) = 255;	//��
			}
			else
			{
				ER.at<Vec3b>(i, j) = 0;	//��
			}
		}
	}
	//����ֵ��
	cvtColor(ER, ER, COLOR_RGB2GRAY);
	ER = (ER > 2);

	return ER;
}

//��̬ѧ���� ������ and ��ʴ�� ��ֵ��ͼ	size:�����С
Mat Dispose::Shape(Mat img, Size size=Size(2,2))
{
	//��̬ѧ����	
	Mat dilate_image, erode_image;
	Mat element = getStructuringElement(MORPH_RECT, size);
	Mat elementX = getStructuringElement(MORPH_RECT, Size(size.width, 1));
	Mat elementY = getStructuringElement(MORPH_RECT, Size(1, size.height));
	Point point(-1, -1);

	dilate(img, dilate_image, element, point, 2);			//���� 
	dilate(dilate_image, dilate_image, element, point, 2);

	//ͼ��С�ˣ����ƱȽϴ� ������һ��
	if (size.width > 2 || size.height > 2)
	{
		dilate(dilate_image, dilate_image, element, point, 2);
	}
	dilate(dilate_image, dilate_image, elementY, point, 2);
	erode(dilate_image, erode_image, elementY, point, 2);	//��ʴ
	erode(erode_image, erode_image, elementY, point, 2);
	erode(erode_image, erode_image, elementY, point, 2);
	return erode_image;
}

//��������  beta���ȵ��ڶ�(����0 ����)
Mat Dispose::RegulateBrightness(Mat img,double beta)
{
	//���ͼ��
	Mat dst = Mat::zeros(img.size(), img.type());

	Mat m1;
	img.convertTo(m1, CV_32F);
	for (int row = 0; row < img.rows; row++) {
		for (int col = 0; col < img.cols; col++) {
			if (img.channels() == 3) {
				float b = m1.at<Vec3f>(row, col)[0];//����ͨ�����أ�����ֱ����Vec3f��Ҫ��Vec3b����Ϊ��opencv��8UC������,������ǰsrc.convertToת������
				float g = m1.at<Vec3f>(row, col)[1];//green
				float r = m1.at<Vec3f>(row, col)[2];//red
				dst.at<Vec3b>(row, col)[0] = saturate_cast<uchar>(b + beta); //д���أ��޸����ȺͶԱȶ�
				dst.at<Vec3b>(row, col)[1] = saturate_cast<uchar>(g + beta);
				dst.at<Vec3b>(row, col)[2] = saturate_cast<uchar>(r + beta);
			}
			else if (img.channels() == 1) {
				float v = img.at<uchar>(row, col);    //��һͨ������
				dst.at<uchar>(row, col) = saturate_cast<uchar>(v * 0.85 + beta);
			}
		}
	}
	return dst;
}

//����Ŀ�����
int Dispose::SearchTargetRect(Mat input,Rect* recs,double* an)
{
	int count = 0;	//����arrs  ���±�
	//���崢�����
	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierarchy;
	findContours(input, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));

	//����Ŀ��������� Ѱ�ҷ���������Ŀ��
	for (int i = 0; i < hierarchy.size(); i++)
	{
		//�������
		Rect rec = boundingRect(contours[i]);		//������
		RotatedRect r = minAreaRect(contours[i]);	//б����  ������ȡ��б�Ƕ�
		double area = rec.area();
		double width = rec.width;
		double height = rec.height;

		//����ɸѡ ��߱� �����С �����������ε������
		if ((width / height > 2.5 && width / height < 4) && (area > 1000 && area < 100000) && (contourArea(contours[i])) / area > 0.6)
		{
			recs[count] = rec;
			an[count++] = r.angle;
		}
	}

	return count;
}

//��Ŀ��ͼ�ϵ��ַ�ת��Ϊ�ַ�������
bool Dispose::imagetostring(Mat input)
{
	return false;
}

//��ȡ����ͼƬ�����ƶ�
float Dispose::GetRecognitionDegree(Mat input1,Mat input2)
{
	//����
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

//����������Ҫ��ı���
Mat Dispose::DotDelete(Mat input)
{
	int acount = 0;
	for (int i = 0; i < 8; i++)//ȥ�����
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

//����������Ҫ��ı���
Mat Dispose::DotDelete(Mat input, int fa)
{
	int acount = 0;
	for (int i = 0; i < 8; i++)//ȥ�����
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

//ƥ�䳵���ַ�
char Dispose::MatchingChar(Mat input)
{
	Mat q;
	String s = "";
	float f = 0.5, f1;
	int i1 = 0, j1 = 0;
	char c;
	for (int i = 0; i <= 34; i++)
	{
		s = "plate/" + to_string(i) + ".jpg";  //ͼƬ��ַ
		q = imread(s, 0);
		//��ֵ��
		q = (q > 9);
		f1 = GetRecognitionDegree(input, q);
		if (f1 > f)
		{
			f = f1;
			i1 = i;
		}
	}
	switch (i1)   //i�൱��1 o�൱��0 ����û��
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
	//defaul9t: c = '*';  //Ϊ*�ű�ʾ��ȡ���ַ�������;
	//	break;
	}
	return c;
}

//ƥ�䳵�ƺ���
std::string Dispose::MatchingSinogram(Mat input)
{
	Mat q;
	String s = ""; //���ַ
	float f = 0.4, f1;
	int i1 = 99, j1 = 0;
	string c = ""; //�溺��
	for (int i = 0; i <= 30; i++)
	{
		s = "hanzi/" + to_string(i) + ".jpg";  //ͼƬ��ַ
		q = imread(s, 0);
		//��ֵ��
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
	case 0:	c = "��"; break;
	case 1:	c = "��"; break;
	case 2:	c = "��"; break;
	case 3:	c = "��"; break;
	case 4:	c = "��"; break;
	case 5:	c = "��"; break;
	case 6:	c = "��"; break;
	case 7:	c = "��"; break;
	case 8:	c = "��"; break;
	case 9:	c = "��"; break;
	case 10:	c = "��"; break;
	case 11:	c = "��"; break;
	case 12:	c = "��"; break;
	case 13:	c = "��"; break;
	case 14:	c = "��"; break;
	case 15:	c = "³"; break;
	case 16:	c = "��"; break;
	case 17:	c = "��"; break;
	case 18:	c = "��"; break;
	case 19:	c = "��"; break;
	case 20:	c = "��"; break;
	case 21:	c = "��"; break;
	case 22:	c = "��"; break;
	case 23:	c = "��"; break;
	case 24:	c = "��"; break;
	case 25:	c = "��"; break;
	case 26:	c = "��"; break;
	case 27:	c = "ԥ"; break;
	case 28:	c = "��"; break;
	case 29:	c = "��"; break;
	case 30:	c = "��"; break;
	default: c = '-';  //Ϊ*�ű�ʾ��ȡ���ַ�������;
	}
	return c;
}

//�ַ� and ������ȡ
std::string Dispose::Distill(Mat a)
{
	/*imshow("a", a);
	waitKey(0);*/
	string s = "-"; //	- ����û���ҵ���
	char c1[10] = { '-','-', '-', '-', '-', '-', '-'};  //��¼�����ϵ��ַ�
	int c2[10] = { 0,0,0,0,0,0,0 }; //��¼�������ַ�������
	int count = 0;
	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierarchy;
	findContours(a, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
	/*Mat dstImage = Mat::zeros(a.size(), CV_8UC3);*/
	for (int i = 0; i < hierarchy.size(); i++)
	{
		//������ɫ
		Scalar color = Scalar(255, 100, 255);
		//�������
		Rect rec = boundingRect(contours[i]);
		double area = rec.area();
		double width = rec.width;
		double height = rec.height;

		if (area > AREA && area < 1700 && height > 20 && rec.x > 30 && rec.x < a.cols - 7)
		{
			//������
			//rectangle(a, rec, color);
			//cout <<count<<"�������ǣ�"<< rec.x << endl;
			Mat m = a(rec);
			m = m(Range(1, m.rows - 1), Range(1, m.cols - 1));
			//imwrite("plate2/1-" + to_string(count) + ".jpg", m);
		/*	imshow("1-" + to_string(count), m);
			waitKey(0);*/
			/*	c1[count] = shibie_zifu(m);*/
			c1[count] = Dispose::MatchingChar(m);
			c2[count] = rec.x;
			//�ַ����
			rectangle(a, rec, Scalar(0, 0, 0), -1);
			count++;
		}
	}



	if (count != 7 && s == "-")
	{
		Mat test = a.clone();
		Mat element = getStructuringElement(MORPH_RECT, Size(5, 5)); //������̬ѧ�����Ĵ�С
		dilate(test, test, element);	//����
	/*	imshow("����", test);
		waitKey(0);*/
		/***************************ɨ�躺��*******************************/
		findContours(test, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
		for (int i = 0; i < hierarchy.size(); i++)
		{
			//������ɫ
			Scalar color = Scalar(255, 100, 255);
			//�������
			Rect rec = boundingRect(contours[i]);
			double area = rec.area();
			double width = rec.width;
			double height = rec.height;

			if (area > AREA + 300 && area < 2000 && height > 40 && rec.x < a.cols - 100)
			{
				//������
				/*rectangle(a, rec, color);*/
				//cout <<count<<"�������ǣ�"<< rec.x << endl;
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
			//������
			//drawContours(dstImage, contours, i, color, 3, 8, hierarchy, 0);
		}
	}
	if (s=="-")
	{
		return "--";
	}
	/************************��������********************************/
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
				/*****�ַ�����****/
				tmpc = c1[i];
				c1[i] = c1[j];
				c1[j] = tmpc;
			}
		}
		s += c1[i];
	}
	s += c1[5];
	//cout << "���ƺ��ǣ�" << s << endl;
	return s;
}

//ɾ���±���
Mat Dispose::DeleteDown(Mat input, int fa)
{
	int acount = 0;
	for (int i = input.rows-1; i > input.rows - 8; i--)//ȥ�±���
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