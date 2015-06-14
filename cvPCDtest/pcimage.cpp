#include "pcimage.h"

#include <fstream>
#include <iostream>
#include <direct.h>
#include <windows.h>

using namespace cv;
using namespace std;

/*
*pcd�t�@�C�������w�肷��Ɖ摜�ɕϊ����ĕۑ�
*����:
*	string fileName �t�@�C����
*	string imgName �ۑ�����摜��
*	int resolution 1pix��cm�l���ɂ��邩
*�Ԃ�l:
*	���� 0  ���s�@1
*/
int save_pcdtoimg(string fileName, string imgName ,int resolution )
{
	const int img_width = 1000;					//�p�ӂ���摜�̕�
	const int img_height = 1000;				//�p�ӂ���摜�̍���
	const int coefficient = 100 / resolution;	//�f�[�^���𑜓x�ɍ��킹��W��
	const int imgval_increment = 80;			//��f�l�̑�����

	//�_�Q���W����摜�̍��W�ɕϊ������l
	int	x_val, y_val;

	string str ,x_str ,y_str;
	string searchLine("nan");
	string searchWord(" ");
	string::size_type x_pos , y_pos;

	Mat pcd_img( Size( img_width, img_height ) , CV_8U, Scalar::all(0) );

	//pcd�t�@�C����ǂݍ���
	ifstream ifs( fileName );
	if (ifs.fail())
	{
		cerr << "False" << endl;
		return EXIT_FAILURE;
	}

	//�w�b�_�������Ƃ΂����߂̃��[�v
	for (int i = 0 ; i <= 11; i++){
		getline(ifs, str);
	}

	while (getline(ifs, str))
	{
		//nan�̗�Ȃ�X���[
		if ( str.find(searchLine) != string::npos ) continue;

		//�擪���甼�p�X�y�[�X�܂ł̕�����ɌW�����|����int�^�Ŏ擾
		x_pos = str.find(searchWord);
		if (x_pos != string::npos){
			x_str = str.substr(0, x_pos);
			x_val = int(stof(x_str) * coefficient);
		}

		//x�̒l�̌�납�甼�p�X�y�[�X�܂ł̕�����ɌW�����|����int�^�Ŏ擾
		y_pos = str.find(searchWord, x_pos + 1);
		if (y_pos != string::npos){
			y_str = str.substr( x_pos + 1 , y_pos );
			y_val = int(stof(y_str) * -coefficient);
		}

		//�擾����[x,y]�̉�f�l�𑝉�������
		pcd_img.data[(pcd_img.rows / 2 + y_val) * pcd_img.cols + x_val + pcd_img.cols / 5] += imgval_increment;

	}

	cout << "complete" << endl;

	//jpg�ŕۑ�
	imwrite(imgName, pcd_img);

	return 0;
}

int save_floorimg(string src_imgName, string dst_imgName)
{
	string str;
	string searchLine("nan");
	string searchWord(" ");

	Mat src_img = imread( src_imgName , 0 );
	Mat dst_img = src_img.clone();

	for (int y = 0; y < src_img.rows - 1; y++){
		for (int x = 0; x < src_img.cols - 1; x++){
			if (src_img.data[y * src_img.cols + x] > 0){
				line(dst_img, Point(x, y), Point(dst_img.cols / 5, dst_img.rows / 2), 80);
			}
		}
	}
	for (int y = 0; y < src_img.rows - 1; y++){
		for (int x = 0; x < src_img.cols - 1; x++){
			if (src_img.data[y * src_img.cols + x] > 0){
				dst_img.data[y * dst_img.cols + x] = src_img.data[y * src_img.cols + x];
			}
		}
	}


	cout << "complete" << endl;

	//jpg�ŕۑ�
	imwrite(dst_imgName, dst_img);

	return 0;
}


//���݂̎����𕶎���Ŏ擾����
void getNowTime(string& nowstr){
	SYSTEMTIME st;
	GetSystemTime(&st);
	char szTime[25] = { 0 };
	// wHour���X���ԑ����āA���{���Ԃɂ���
	wsprintf(szTime, "%04d%02d%02d%02d%02d%02d",
		st.wYear, st.wMonth, st.wDay,
		st.wHour + 9, st.wMinute, st.wSecond);
	nowstr = szTime;
}

int PCIclasstest(){
	
	int img_width = 1000;
	int	img_height = 1000;
	float x_val, y_val;

	PCImage pcimage(3);

	string str, x_str, y_str;
	string searchLine("nan");
	string searchWord(" ");
	string::size_type x_pos, y_pos;
	string fileName = "./pointcloud.pcd";

	//pcd�t�@�C����ǂݍ���
	ifstream ifs(fileName);
	if (ifs.fail())
	{
		cerr << "False" << endl;
		return EXIT_FAILURE;
	}

	//�w�b�_�������Ƃ΂����߂̃��[�v
	for (int i = 0; i <= 11; i++){
		getline(ifs, str);
	}

	while (getline(ifs, str))
	{
		//nan�̗�Ȃ�X���[
		if (str.find(searchLine) != string::npos) continue;

		//�擪���甼�p�X�y�[�X�܂ł̕�����ɌW�����|����int�^�Ŏ擾
		x_pos = str.find(searchWord);
		if (x_pos != string::npos){
			x_str = str.substr(0, x_pos);
			x_val = stof(x_str);
		}

		//x�̒l�̌�납�甼�p�X�y�[�X�܂ł̕�����ɌW�����|����int�^�Ŏ擾
		y_pos = str.find(searchWord, x_pos + 1);
		if (y_pos != string::npos){
			y_str = str.substr(x_pos + 1, y_pos);
			y_val = stof(y_str);
		}
		pcimage.writePoint(x_val, y_val);

	}

	cout << "complete" << endl;
	pcimage.savePCImage();

	return 0;
}

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
	img_name[0] = "0_0";
	img_width = width;
	img_height = height;
	coefficient = 100 / resolution;
	imgval_increment = 80;
	limit = 10;
	limitpix = limit * coefficient;

	for (auto& image : pcimage)
	{
		image = Mat(Size(width, height), CV_8U, Scalar::all(0));
	}
	for (auto& icondition : imageCondition)
	{
		icondition = NONE;
	}

	//�܂�pci0���Q�Ƃ���
	pcimage_ptr = &pcimage[0];
	img_name_ptr = &img_name[0];
	nowimage = ZERO;
	imageCondition[0] = CENTER;

	//�N���������b�Ŗ��������f�B���N�g�����쐬
	getNowTime(dirname);
	if (_mkdir(dirname.c_str()) == 0){
		cout << "Made a directory named:" << dirname << endl << endl;
	}
	else{
		cout << "Failed to make the directory" << endl;
	}


	cout << "Image name : " << *img_name_ptr << endl;
	cout << "Width:" << pcimage_ptr->cols
		<< "\nHeight:" << pcimage_ptr->rows << endl;

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

	//�擾����[x,y]�̉�f�l�𑝉�������
	if (pcimage_ptr->data[(pcimage_ptr->rows / 2 + (int)y_val) * pcimage_ptr->cols + (int)x_val + limitpix] < (imgval_increment * (255 / imgval_increment))){
		pcimage_ptr->data[(pcimage_ptr->rows / 2 + (int)y_val) * pcimage_ptr->cols + (int)x_val + limitpix] += imgval_increment;
	}
	else pcimage_ptr->data[(pcimage_ptr->rows / 2 + (int)y_val) * pcimage_ptr->cols + (int)x_val + limitpix] = 255;
	

	//imshow("pci", pcimage);
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
	this->writePoint(x_val, y_val);
	this->checkPosition(pos_x, pos_y);

}

/*
*�@�T�v�F�摜���Œ[�ɋߕt���Ă��Ȃ����`�F�b�N
*�@����:
*	float pos_x	���Ȉʒu��x���W(m)
*	float pos_y	���Ȉʒu��y���W(m)
*�@�Ԃ�l:
*	0
*/
int PCImage::checkPosition(float pos_x, float pos_y)
{
	int xi = int(pos_x * coefficient);
	int yi = int(pos_y * -coefficient);

	//������̃��~�b�g�`�F�b�N
	if (isPrepareTOP && yi < limitpix)
	{
		prepareImage(TOP);
	}
	//�E�����̃��~�b�g�`�F�b�N
	if (isPrepareRIGHT && xi > pcimage_ptr->cols - limitpix)
	{
		prepareImage(RIGHT);
	}
	//�������̃��~�b�g�`�F�b�N
	if (isPrepareBOTTOM && yi >  pcimage_ptr->rows - limitpix)
	{
		prepareImage(BOTTOM);
	}
	//�������̃��~�b�g�`�F�b�N
	if (isPrepareLEFT && xi < limitpix)
	{
		prepareImage(LEFT);
	}
	return 0;
}

int PCImage::readPoint(int x_val, int y_val)
{
	//�w�肵��[x,y]�̉�f�l��Ԃ�
	return pcimage_ptr->data[y_val * pcimage_ptr->cols + x_val];
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
	imwrite(dirname + "/" + *img_name_ptr + ".jpg", *pcimage_ptr);
	return 0;
}

/*
*�@�T�v�F���̉摜��p�ӂ���
*�@����:
*	Direction direction�@�ߕt���Ă������
*�@�Ԃ�l:
*	�Ȃ�
*/
int PCImage::prepareImage(Direction direction)
{
	int emptyImageNum;
	int xy[2];

	getImageNumber(xy);		//���S�摜��x,y�ԍ����擾

	switch (direction)
	{
	case TOP:
		isPrepareTOP = true;														//�t���O��true�ɂ���
		emptyImageNum = getEmptyImage();											//�󂢂Ă���摜�̔ԍ����擾
		img_name[emptyImageNum] = to_string(xy[0]) + "_" + to_string(xy[1] + 1);	//�p�ӂ���摜�����쐬
		imageCondition[emptyImageNum] = TOP;										//�摜�̏�Ԃ�ύX
		pcimage[emptyImageNum] = imread(img_name[emptyImageNum]);					//���ɍ쐬����Ă���ꍇ��ǂݍ���
		return 0;

	case RIGHT:
		isPrepareRIGHT = true;
		emptyImageNum = getEmptyImage();
		img_name[emptyImageNum] = to_string(xy[0] + 1) + "_" + to_string(xy[1]);
		imageCondition[emptyImageNum] = RIGHT;
		pcimage[emptyImageNum] = imread(img_name[emptyImageNum]);
		return 0;

	case BOTTOM:
		isPrepareBOTTOM = true;
		emptyImageNum = getEmptyImage();
		img_name[emptyImageNum] = to_string(xy[0]) + "_" + to_string(xy[1] - 1);
		imageCondition[emptyImageNum] = BOTTOM;
		pcimage[emptyImageNum] = imread(img_name[emptyImageNum]);
		return 0;

	case LEFT:
		isPrepareLEFT = true;
		emptyImageNum = getEmptyImage();
		img_name[emptyImageNum] = to_string(xy[0] - 1) + "_" + to_string(xy[1]);
		imageCondition[emptyImageNum] = LEFT;
		pcimage[emptyImageNum] = imread(img_name[emptyImageNum]);
		return 0;

	default:
		return -1;
	}
}

/*
*�@�T�v�F�󂢂Ă���摜�̔ԍ���Ԃ�
*�@����:
*	�Ȃ�
*�@�Ԃ�l:
*	int	i�@�󂢂Ă���摜�̔ԍ�
*	-1		�󂢂Ă���摜����
*/
int PCImage::getEmptyImage()
{
	for (int i = 0; i < 4; i++)
		if (imageCondition[i] == NONE) return i;
	return -1;
}


/*
*�@�T�v�F���݂̒��S�摜�̔ԍ�(x,y)��Ԃ�
*�@����:
*	int xy[]�@x,y���W���i�[����z��
*�@�Ԃ�l:
*	�Ȃ�
*/
void PCImage::getImageNumber( int xy[] )
{
	string::size_type x_pos;
	
	x_pos = img_name[nowimage].find("_");
	if (x_pos != string::npos){
		xy[0] = stoi(img_name[nowimage].substr(0, x_pos));
		xy[1] = stoi(img_name[nowimage].substr(x_pos + 1));
	}
}