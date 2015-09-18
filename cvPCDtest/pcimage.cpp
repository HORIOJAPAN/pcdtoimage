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

int PCIclasstest(){
	
	int img_width = 1000;
	int	img_height = 1000;
	float x_val, y_val;

	PCImage pcimage;

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
		pcimage.writePoint( x_val, y_val );

	}

	cout << "complete" << endl;
	pcimage.~PCImage();

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
PCImage::PCImage(int width, int height, int resolution) : pcimage(imageNum , *this)
{
	//-----�����o�̏�����-----
	img_width = width;
	img_height = height;
	coefficient = 100 / resolution;
	imgval_increment = 80;
	limit = 10;
	limitpix = limit * coefficient;

	//�N���������b�Ŗ��������f�B���N�g�����쐬
	this->getNowTime(dirname);
	if (_mkdir(dirname.c_str()) == 0){
		cout << "Made a directory named:" << dirname << endl << endl;
	}
	else{
		cout << "Failed to make the directory" << endl;
	}

	//pcimage[0]����������
	nowimage = 0;
	pcimage[nowimage].setPCI(0, 0);
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

//�f�X�g���N�^
//�������Ă��Ȃ��摜�̈悪����ΔO�̂��ߕۑ����Ă���
PCImage::~PCImage()
{
	for (int i = 0; i < imageNum; i++)
		if (!pcimage[i].empty())
		{
			pcimage[i].savePCImage();
		}
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
	int ret;
	ret = pcimage[nowimage].writePoint(x_val, y_val);
	if (ret)
	{
		pcimage[ret - 1].writePoint(x_val, y_val);
	}

	this->checkPosition(pos_x, pos_y);

}

/*
*
*  ������i �O�ցO�j��     �摜�̈�Ǘ��̒���     ����i�O�ցO �j����
*
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
	//x,y�̒l���w�肵���𑜓x�ɍ��킹��
	int xi = int(pos_x * coefficient);
	int yi = int(pos_y * -coefficient);

	//���݂̉摜��XY
	int xy[2];

	pcimage[nowimage].getImageNumber(xy);		//���S�摜��X,Y�ԍ����擾

	//8�ߖT�̃��~�b�g�`�F�b�N
	//�摜�[�܂�limitpix�ȉ��Ȃ玟�̉摜��p�ӂ��C���ꂽ��ߖT�摜��ۑ�����

	//��������̃��~�b�g�`�F�b�N��
	if (!checkPrepare(xy[0]+1,xy[1]) && yi < limitpix)
	{
		prepareImage(xy[0] + 1, xy[1]);
	}
	else if (checkPrepare( xy[0] + 1, xy[1] ) && yi > limitpix)
	{
		savePCImage( xy[0] + 1, xy[1] );
	}
	//���E�����̃��~�b�g�`�F�b�N��
	if (!checkPrepare(xy[0], xy[1] + 1) && xi > pcimage[nowimage].cols - limitpix)
	{
		prepareImage(xy[0], xy[1] + 1);
	}
	else if ( checkPrepare(xy[0], xy[1] + 1) && xi < pcimage[nowimage].cols - limitpix)
	{
		savePCImage(xy[0], xy[1] + 1);
	}
	//���������̃��~�b�g�`�F�b�N��
	if (!checkPrepare(xy[0] - 1, xy[1]) && yi >  pcimage[nowimage].rows - limitpix)
	{
		prepareImage(xy[0] - 1, xy[1]);
	}
	else if (checkPrepare(xy[0] - 1, xy[1]) && yi <  pcimage[nowimage].rows - limitpix)
	{
		savePCImage(xy[0] - 1, xy[1]);
	}
	//���������̃��~�b�g�`�F�b�N��
	if (!checkPrepare(xy[0], xy[1] - 1) && xi < limitpix)
	{
		prepareImage(xy[0], xy[1] - 1);
	}
	else if (checkPrepare(xy[0], xy[1] - 1) && xi > limitpix)
	{
		savePCImage(xy[0], xy[1] - 1);
	}

	//�����E������̃��~�b�g�`�F�b�N����
	if (!checkPrepare(xy[0] + 1, xy[1] + 1) && yi < limitpix && xi > pcimage[nowimage].cols - limitpix)
	{
		prepareImage(xy[0] + 1, xy[1] + 1);
	}
	else if (checkPrepare(xy[0] + 1, xy[1] + 1) && yi > limitpix && xi < pcimage[nowimage].cols - limitpix)
	{
		savePCImage(xy[0] + 1, xy[1] + 1);
	}
	//�����E�������̃��~�b�g�`�F�b�N����
	if (!checkPrepare(xy[0] + 1, xy[1] - 1) && xi > pcimage[nowimage].cols - limitpix && yi >  pcimage[nowimage].rows - limitpix)
	{
		prepareImage(xy[0] + 1, xy[1] - 1);
	}
	else if (checkPrepare(xy[0] + 1, xy[1] - 1) && xi < pcimage[nowimage].cols - limitpix && yi <  pcimage[nowimage].rows - limitpix)
	{
		savePCImage(xy[0] + 1, xy[1] - 1);
	}
	//�������������̃��~�b�g�`�F�b�N����
	if (!checkPrepare(xy[0] - 1, xy[1] - 1) && yi >  pcimage[nowimage].rows - limitpix && xi < limitpix)
	{
		prepareImage(xy[0] - 1, xy[1] - 1);

	}
	else if (checkPrepare(xy[0] - 1, xy[1] - 1) && yi <  pcimage[nowimage].rows - limitpix && xi > limitpix)
	{
		savePCImage(xy[0] - 1, xy[1] - 1);
	}
	//������������̃��~�b�g�`�F�b�N����
	if (!checkPrepare(xy[0] - 1, xy[1] + 1) && xi < limitpix && yi < limitpix)
	{
		prepareImage(xy[0] - 1, xy[1] + 1);
	}
	else if (checkPrepare(xy[0] - 1, xy[1] + 1) && xi > limitpix && yi < limitpix)
	{
		savePCImage(xy[0] - 1, xy[1] + 1);
	}

	int xshift = 0;
	int yshift = 0;

	/************************************
	*>>>>>>���̉摜�ɃV�t�g���鏈��<<<<<*
	************************************/
	//�摜�[�܂ł̋�����limitpix��1/3�ȉ��ɂȂ�����V�t�g
	//������̃��~�b�g�`�F�b�N
	if ( yi < limitpix/3)
	{
		yshift = 1;
	}
	//�E�����̃��~�b�g�`�F�b�N
	if (xi > pcimage[nowimage].cols - limitpix/3)
	{
		xshift = 1;
	}
	//�������̃��~�b�g�`�F�b�N
	if (!checkPrepare(xy[0] - 1, xy[1]) && yi >  pcimage[nowimage].rows - limitpix/3)
	{
		yshift = -1;
	}
	//�������̃��~�b�g�`�F�b�N
	if (!checkPrepare(xy[0], xy[1] - 1) && xi < limitpix/3)
	{
		xshift = -1;
	}
	//xshift��������yshift��0�ȊO�Ȃ�摜�V�t�g�����s
	if (xshift || yshift ) shiftCenterImage(xshift, yshift);

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
*�@	Direction direction�@�ۑ��������摜�̏��
*�@�Ԃ�l:
*	�Ȃ�
*/
void PCImage::savePCImage(int x , int y)
{
	for (int i = 0; i < imageNum; i++)
	{
		if (pcimage[i].isCoordinates(x, y)) pcimage[i].savePCImage();
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
*	�����@0
*	���s�@-1
*/
int PCImage::prepareImage(int x, int y)
{
	int emptyImageNum;
	int xy[2];

	pcimage[nowimage].getImageNumber(xy);		//���S�摜��x,y�ԍ����擾

	emptyImageNum = getEmptyImage();						//�󂢂Ă���摜�̔ԍ����擾
	pcimage[emptyImageNum].setPCI(xy[0] + x, xy[1] + y);	//�摜��p��
	loadPCImage(emptyImageNum);								//���ɍ쐬����Ă���ꍇ�͓ǂݍ���
	return 0;

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
	for (int i = 0; i < imageNum; i++)
		if (pcimage[i].empty()) return i;
	return -1;
}

/*
*�@�T�v�F���S�摜���w������ɃV�t�g����
*�@����:
*	int x�@�摜�̈���W�ɂ����錻�݉摜�����x�����ψ�(-1~1)
*	int y�@�摜�̈���W�ɂ����錻�݉摜�����y�����ψ�(-1~1)
*�@�Ԃ�l:
*	�Ȃ�
*/
int PCImage::shiftCenterImage(int x,int y)
{
	int nowXY[2];
	pcimage[nowimage].getImageNumber(nowXY);	//���݉摜�̍��W���擾

	//�w�肵�����W�̉摜������Ή摜�ԍ���nowimage�ɑ��
	for (int i = 0; i < imageNum; i++)
	{
		if (pcimage[i].isCoordinates(nowXY[0] + x, nowXY[1] + y))
		{
			nowimage = i;
			return 0;
		}
	}
	return -1;
}

/*
*�@�T�v�F�w�肵���摜�̈���W�̉摜���p�ӂ���Ă���Ɛ^��Ԃ�
*�@����:
*	int x�@�摜�̈���W�ɂ����錻�݉摜�����x�����ψ�(-1~1)
*	int y�@�摜�̈���W�ɂ����錻�݉摜�����y�����ψ�(-1~1)
*�@�Ԃ�l:
*	True or False
*/
bool PCImage::checkPrepare(int x, int y)
{
	for (int i = 0; i < imageNum; i++)
	{
		if (pcimage[i].isCoordinates(x, y)) return true;
	}
	return false;
}


//������i �O�ցO�j�� �ް�
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

/*------------------------------
*----��--PCI�N���X�̒�`--��----
*-------------------------------*/
/*
*�@�T�v�F=���Z�q�̃I�[�o�[���[�h
*		 Mat�N���X�̂��̂��Ē�`
*�@����:
*	cv::Mat& mat�@Mat�N���X�̎Q��
*�@�Ԃ�l:
*	*this�@���g�̎Q��
*/
PCImage::PCI& PCImage::PCI::operator=(cv::Mat& mat)
{
	Mat::operator=(mat);
	return *this;
}

PCImage::PCI::PCI(PCImage& pcimage_outer) : pciOut(pcimage_outer)
{
	//�O�̂��ߗ̈��������Ă���
	Mat::release();
}

/*
*�@�T�v�F�����o�̏�����
*�@����:
*	int x	�摜�ʒu��x���W
*	int y	�摜�ʒu��y���W
*	PCImage::Direction dir	�摜�̏��(CENTER����݂Ăǂ̕�����)
*�@�Ԃ�l:
*	�Ȃ�
*/
void PCImage::PCI::setPCI(int x, int y)
{
	imageNumXY[0] = x;
	imageNumXY[1] = y;
	name = "./" + pciOut.dirname + "/" +  to_string(imageNumXY[0]) + "_" + to_string(imageNumXY[1]) + ".jpg";
}

/*
*�@�T�v�F�摜�̏�Ԃ̎擾
*�@����:
*	�Ȃ�
*�@�Ԃ�l:
*	Direction�@imageCondition�@�摜�̏��(���S���猩�Ăǂ̕������C�������͋�)
*/


/*
*�@�T�v�F�摜�̈ʒu(x,y)��Ԃ�
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

/*
*�@�T�v�F�摜����Ԃ�[./(directoryname)/(filename).jpg]
*�@����:
*	�Ȃ�
*�@�Ԃ�l:
*	string�@name�@�摜��
*/
string PCImage::PCI::getName()
{
	return name;
}

/*
*�@�T�v�F�w����W�ɓ_����������
*�@����:
*	float x_val x���W(m)
*	float y_val y���W(m)
*�@�Ԃ�l:
*	�Ȃ�
*/
int PCImage::PCI::writePoint(float x_val, float y_val)
{
	//x,y�̒l���w�肵���𑜓x�ɍ��킹��
	x_val *= pciOut.coefficient;
	y_val *= -pciOut.coefficient;

	//x,y�̒l���摜�̈ʒu�ɍ��킹��(������) 
	x_val -= (imageNumXY[0] * pciOut.img_width + pciOut.limitpix);
	y_val -= (imageNumXY[1] * pciOut.img_height - rows / 2);

	//���摜�̈���̓_���m�F���ē��摜�̈�O�̏ꍇ�͊Y���̈��ID��Ԃ�
	int x_coord = 0;
	int y_coord = 0;
	if (x_val < 0)
	{
		x_coord = -1;
	}
	else if (x_val >= cols)
	{
		x_coord = 1;
	}
	if (y_val < 0)
	{
		y_coord = 1;
	}
	else if (y_val >= rows)
	{
		y_coord = -1;
	}
	if (x_coord*x_coord + y_coord*y_coord > 0)
	{
		for (int i = 0; i < imageNum; i++)
		{
			if (pciOut.pcimage[i].isCoordinates(imageNumXY[0] + x_coord, imageNumXY[1] + y_coord)) return i+1;
		}
		return -1;
	}

	//�擾����[x,y]�̉�f�l�𑝉�������
	if (data[(int)y_val * cols + (int)x_val] < (pciOut.imgval_increment * (255 / pciOut.imgval_increment))){
		data[(int)y_val * cols + (int)x_val] += pciOut.imgval_increment;
	}
	else data[(int)y_val * cols + (int)x_val] = 255;

	imshow("show", *this);

	return 0;
}

/*
*�@�T�v�F�摜��ۑ����ĉ摜�̈�����
*�@����:
*	�Ȃ�
*�@�Ԃ�l:
*	�Ȃ�
*/
void PCImage::PCI::savePCImage()
{
	imwrite( name, *this );
}
void PCImage::PCI::release()
{
	imwrite(name, *this);
	this->Mat::release();
	name = "";
}

bool PCImage::PCI::isCoordinates(int x, int y)
{
	if (imageNumXY[0] == x && imageNumXY[1] == y) return true;
	return false;
}
bool PCImage::PCI::isCoordinates(int xy[])
{
	if (imageNumXY[0] == xy[0] && imageNumXY[1] == xy[1]) return true;
	return false;
}
