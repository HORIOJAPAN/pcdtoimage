#include "pcimage.h"

#include <fstream>
#include <iostream>
#include <direct.h>
#include <windows.h>

using namespace cv;
using namespace std;

/*
*�@�R���X�g���N�^(�����L) 
*����:
*	string name �ۑ����̃t�@�C����
*	int width �@�c
*	int height�@��
*	int resolution�@1pix��cm�l���ɂ��邩
*/
PCImage::PCImage(int width, int height, int resolution)
{
	//-----�����o�̏�����-----
	img_name = "0_0";
	img_width = width;
	img_height = height;
	coefficient = 100 / resolution;
	imgval_increment = 80;
	limit = 10;
	limitpix = limit * coefficient;

	pcimage = Mat(Size(width, height), CV_8U, Scalar::all(0));

	//�N���������b�Ŗ��������f�B���N�g�����쐬
	getNowTime(dirname);
	if (_mkdir(dirname.c_str()) == 0){
		cout << "Made a directory named:" << dirname << endl << endl;
	}
	else{
		cout << "Failed to make the directory" << endl;
	}


	cout << "Image name : " << img_name << endl;
	cout << "Width:" << pcimage.cols
		<< "\nHeight:" << pcimage.rows << endl;
	Sleep(2000);

}

//�R���X�g���N�^�̃I�[�o�[���[�h
PCImage::PCImage()
{
	this->PCImage::PCImage( 1000, 1000, 5);

}
PCImage::PCImage(int resolution )
{
	this->PCImage::PCImage( 1000, 1000, resolution);

}

PCImage::~PCImage()
{
}

/*
*�@�T�v�F�w����W�ɓ_����������
*�@����:
*	float x_val x���W(m)
*	float y_val y���W(m)
*�@�Ԃ�l:
*	�Ȃ�
*/
void PCImage::writePoint(float x_val, float y_val)
{
	//x,y�̒l���w�肵���𑜓x�ɍ��킹��
	x_val *= coefficient;
	y_val *= -coefficient;

	//�擾����[x,y]�̉�f�l�𑝉�������.�I�[�o�[�t���[���Ȃ��悤�ɕ���.
	if (pcimage.data[(pcimage.rows / 2 + (int)y_val) * pcimage.cols + (int)x_val + limitpix] < (imgval_increment * (255 / imgval_increment))){
		pcimage.data[(pcimage.rows / 2 + (int)y_val) * pcimage.cols + (int)x_val + limitpix] += imgval_increment;
	}
	else pcimage.data[(pcimage.rows / 2 + (int)y_val) * pcimage.cols + (int)x_val + limitpix] = 255;
	
}

void PCImage::writeLine(float x_val, float y_val, float pos_x, float pos_y)
{
	//x,y�̒l���w�肵���𑜓x�ɍ��킹��
	x_val *= coefficient;
	y_val *= -coefficient;
	x_val = (int)x_val + limitpix;
	y_val = pcimage.rows / 2 + (int)y_val;


	pos_x *= coefficient;
	pos_y *= -coefficient;
	pos_x = (int)pos_x + limitpix;
	pos_y = pcimage.rows / 2 + (int)pos_y;

	//�擾����[x,y]�ƌ��ݒn����Ō���
	line(pcimage, Point(x_val, y_val), Point(pos_x, pos_y), 100);

}

/*
*�@�T�v�F�w����W(��΍��W)�ɓ_����������
*�@����:
*	float x_val �������ޓ_��x���W(m)
*	float y_val �������ޓ_��y���W(m)
*	float pos_x	���Ȉʒu��x���W(m)
*	float pos_y	���Ȉʒu��y���W(m)
*�@�Ԃ�l:
*	�Ȃ�
*/
void PCImage::writePoint(float x_val, float y_val, float pos_x, float pos_y)
{
	
	this->writeLine(x_val, y_val , pos_x ,pos_y );
	this->writePoint(x_val, y_val);

}

int PCImage::readPoint(int x_val, int y_val)
{
	//�w�肵��[x,y]�̉�f�l��Ԃ�
	return pcimage.data[y_val * pcimage.cols + x_val];
}

/*
*�@�T�v�F�摜��ۑ�
*�@����:
*�@	�Ȃ�
*�@�Ԃ�l:
*	0
*/
int PCImage::savePCImage()
{
	imwrite(dirname + "/" + dirname + ".jpg", pcimage);
	//resize(*pcimage_ptr, *pcimage_ptr, Size(1000, 1000));
	//imshow(*img_name_ptr, *pcimage_ptr);
	//waitKey(1);
	return 0;
}

std::string PCImage::getDirname()
{
	return dirname;
}

//���݂̎����𕶎���Ŏ擾����
void PCImage::getNowTime(string& nowstr){
	SYSTEMTIME st;
	GetSystemTime(&st);
	char szTime[25] = { 0 };
	// wHour���X���ԑ����āA���{���Ԃɂ���
	wsprintf(szTime, "%04d%02d%02d%02d%02d%02d",
		st.wYear, st.wMonth, st.wDay,
		st.wHour + 9, st.wMinute, st.wSecond);
	nowstr = szTime;
}
