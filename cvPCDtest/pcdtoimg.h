#ifndef _INC_PCDTOIMG
#define _INC_PCDTOIMG

#include <fstream>
#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/opencv_lib.hpp>

//pcd�t�@�C�������w�肷��Ɖ摜�ɕϊ����ĕۑ�
int save_pcdtoimg(std::string fileName, std::string imgName, int resolution = 1);

//�_�Q�摜�ɏ�������(������)
int save_floorimg(std::string src_imgName, std::string dst_imgName);

int PCIclasstest();

//�_�Q�摜���쐬����N���X
class PCImage
{
private:
	cv::Mat pcimage;
	std::string img_name;
	int img_width;					//�p�ӂ���摜�̕�
	int img_height;				//�p�ӂ���摜�̍���
	int coefficient;	//�f�[�^���𑜓x�ɍ��킹��W��
	int imgval_increment;			//��f�l�̑�����

public:
	PCImage(std::string name, int width, int height, int resolution);
	~PCImage();
	void writePoint(float x_val, float y_val);
	int readPoint(int x_val, int y_val);
	
	int savePCImage();

};




#endif