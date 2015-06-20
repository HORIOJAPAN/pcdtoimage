#include "pcimage.h"

#include <fstream>
#include <iostream>
#include <direct.h>
#include <windows.h>

using namespace cv;
using namespace std;

/*
*�@�T�v:pcd�t�@�C�������w�肷��Ɖ摜�ɕϊ����ĕۑ�
*�@����:
*	string fileName �t�@�C����
*	string imgName �ۑ�����摜��
*	int resolution 1pix��cm�l���ɂ��邩
*�@�Ԃ�l:
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

	PCImage pcimage(5);

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
	pcimage.savePCImage(PCImage::CENTER);

	return 0;
}

/*------------------------------
*--��--PCImage�N���X�̒�`--��--
*-------------------------------*/

/*
*�@�T�v:�R���X�g���N�^(�����L) 
*�@����:
*	int width �@�c
*	int height�@��
*	int resolution�@1pix��cm�l���ɂ��邩
*/
PCImage::PCImage(int width, int height, int resolution) :pcimage(imageNum , *this)
{
	//-----�����o�̏�����-----
	img_width = width;
	img_height = height;
	coefficient = 100 / resolution;
	imgval_increment = 80;
	limit = 10;
	limitpix = limit * coefficient;

	//�N���������b�Ŗ��������f�B���N�g�����쐬
	getNowTime(dirname);
	if (_mkdir(dirname.c_str()) == 0){
		cout << "Made a directory named:" << dirname << endl << endl;
	}
	else{
		cout << "Failed to make the directory" << endl;
	}

	//pcimage[0]����������
	nowimage = ZERO;
	pcimage[nowimage].setPCI(0, 0, CENTER);
	pcimage[nowimage] = Mat(Size(width, height), CV_8U, Scalar::all(0));


	cout << "Width:" << pcimage[nowimage].cols
		<< "\nHeight:" << pcimage[nowimage].rows << endl;

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
	pcimage[nowimage].writePoint(x_val, y_val);
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
	pcimage[nowimage].writePoint(x_val, y_val);
	this->checkPosition(pos_x, pos_y);

}

/*
*�@�T�v�F�摜���̎��Ȉʒu���`�F�b�N���ĕK�v�ȏ������s��
*		�@�摜�̗p�ӂƕۑ��C���S�摜�̈ڍs
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

	//�㉺���E�̃��~�b�g�`�F�b�N
	//�摜�̒[�ɋߕt�����玟�̉摜��p�ӂ��C���ꂽ��ۑ�����
	//������̃��~�b�g�`�F�b�N
	if (!isPrepareTOP && yi < limitpix)
	{
		prepareImage(TOP);
		isPrepareTOP = true;		//�t���O��true�ɂ���
	}
	else if (isPrepareTOP && yi > limitpix)
	{
		savePCImage(TOP);
		isPrepareTOP = false;		//�t���O��false�ɂ���
	}
	//�E�����̃��~�b�g�`�F�b�N
	if (!isPrepareRIGHT && xi > pcimage[nowimage].cols - limitpix)
	{
		prepareImage(RIGHT);
		isPrepareRIGHT = true;
	}
	else if (isPrepareRIGHT && xi < pcimage[nowimage].cols - limitpix)
	{
		savePCImage(RIGHT);
		isPrepareRIGHT = false;
	}
	//�������̃��~�b�g�`�F�b�N
	if (!isPrepareBOTTOM && yi >  pcimage[nowimage].rows - limitpix)
	{
		prepareImage(BOTTOM);
		isPrepareBOTTOM = true;

	}
	else if (isPrepareBOTTOM && yi <  pcimage[nowimage].rows - limitpix)
	{
		savePCImage(BOTTOM);
		isPrepareBOTTOM = false;
	}
	//�������̃��~�b�g�`�F�b�N
	if (!isPrepareLEFT && xi < limitpix)
	{
		prepareImage(LEFT);
		isPrepareLEFT = true;
	}
	else if (isPrepareLEFT && xi > limitpix)
	{
		savePCImage(LEFT);
		isPrepareLEFT = false;
	}

	
	return 0;
}

int PCImage::readPoint(int x_val, int y_val)
{
	//�w�肵��[x,y]�̉�f�l��Ԃ�
	return pcimage[nowimage].data[y_val * pcimage[nowimage].cols + x_val];
}

/*
*�@�T�v�F�摜��ۑ�
*�@����:
*�@	int imgNum�@�摜�ԍ�
*�@�Ԃ�l:
*	0
*/
void PCImage::savePCImage(Direction direction)
{
	for (int i = 0; i < imageNum ; i++)
	{
		if (pcimage[i].getCondition() == direction) pcimage[i].savePCImage();
	}

}

/*
*�@�T�v�F�摜��ǂݍ���
*�@����:
*�@	int emptyImageNum �摜�ԍ�
*�@�Ԃ�l:
*	0
*/
int PCImage::loadPCImage(int emptyImageNum)
{
	pcimage[emptyImageNum] = imread(pcimage[emptyImageNum].getName());
	if (pcimage[emptyImageNum].empty())
	{
		pcimage[emptyImageNum] = Mat(Size(img_width, img_height), CV_8U, Scalar::all(0));
	}
	return 0;
}

/*
*�@�T�v�F���̉摜��p�ӂ���
*�@����:
*	Direction direction�@�p�ӂ������
*�@�Ԃ�l:
*	�Ȃ�
*/
int PCImage::prepareImage(Direction direction)
{
	int emptyImageNum;
	int xy[2];

	pcimage[nowimage].getImageNumber(xy);		//���S�摜��x,y�ԍ����擾

	switch (direction)
	{
	case TOP:
		emptyImageNum = getEmptyImage();						//�󂢂Ă���摜�̔ԍ����擾
		pcimage[emptyImageNum].setPCI(xy[0], xy[1] + 1, TOP);	//�摜��p��
		loadPCImage(emptyImageNum);								//���ɍ쐬����Ă���ꍇ�͓ǂݍ���
		return 0;

	case RIGHT:
		emptyImageNum = getEmptyImage();
		pcimage[emptyImageNum].setPCI(xy[0] + 1, xy[1] , RIGHT);
		loadPCImage(emptyImageNum);
		return 0;

	case BOTTOM:
		emptyImageNum = getEmptyImage();
		pcimage[emptyImageNum].setPCI(xy[0], xy[1] - 1, BOTTOM);
		loadPCImage(emptyImageNum);
		return 0;

	case LEFT:
		emptyImageNum = getEmptyImage();
		pcimage[emptyImageNum].setPCI(xy[0] - 1 , xy[1] , LEFT);
		loadPCImage(emptyImageNum);
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
		if (pcimage[i].getCondition() == NONE) return i;
	return -1;
}

/*
*�@�T�v�F���S�摜���w������ɃV�t�g����
*�@����:
*	Direction direction �V�t�g�������
*�@�Ԃ�l:
*	�Ȃ�
*/
int PCImage::shiftCenterImage(Direction direction)
{

	return 0;
}

/*------------------------------
*----��--PCI�N���X�̒�`--��----
*-------------------------------*/
PCImage::PCI& PCImage::PCI::operator=(cv::Mat& mat)
{
	Mat::operator=(mat);
	return *this;
}
void PCImage::PCI::setPCI(int x, int y, PCImage::Direction dir)
{
	imageNumXY[0] = x;
	imageNumXY[1] = y;
	imageCondition = dir;
	name = "./" + pciOut.dirname + "/" +  to_string(imageNumXY[0]) + "_" + to_string(imageNumXY[1]) + ".jpg";
}
PCImage::Direction PCImage::PCI::getCondition()
{
	return imageCondition;
}
/*
*�@�T�v�F���݂̒��S�摜�̈ʒu(x,y)��Ԃ�
*�@����:
*	int xy[]�@x,y���W���i�[����z��
*�@�Ԃ�l:
*	�Ȃ�
*/
void PCImage::PCI::getImageNumber(int xy[])
{
	xy[0] = imageNumXY[0];
	xy[1] = imageNumXY[1];
}

string PCImage::PCI::getName()
{
	return name;
}
void PCImage::PCI::writePoint(float x_val, float y_val)
{
	//x,y�̒l���w�肵���𑜓x�ɍ��킹��
	x_val *= pciOut.coefficient;
	y_val *= -pciOut.coefficient;

	//x,y�̒l���摜�̈ʒu�ɍ��킹��
	x_val -= imageNumXY[0] * pciOut.img_width - pciOut.limitpix;
	y_val -= imageNumXY[1] * pciOut.img_height - rows / 2;

	//�擾����[x,y]�̉�f�l�𑝉�������
	if (data[(int)y_val * cols + (int)x_val] < (pciOut.imgval_increment * (255 / pciOut.imgval_increment))){
		data[(int)y_val * cols + (int)x_val] += pciOut.imgval_increment;
	}
	else data[(int)y_val * cols + (int)x_val] = 255;
}
void PCImage::PCI::savePCImage()
{
	imwrite( name, *this );
	this->release();
	name = "";
	imageCondition = NONE;
}

