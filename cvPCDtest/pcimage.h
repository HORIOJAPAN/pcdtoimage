#ifndef _INC_PCIMAGE
#define _INC_PCIMAGE

#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/opencv_lib.hpp>

//pcd�t�@�C�������w�肷��Ɖ摜�ɕϊ����ĕۑ�
int save_pcdtoimg(std::string fileName, std::string imgName, int resolution = 1);

//�_�Q�摜�ɏ�������(������)
int save_floorimg(std::string src_imgName, std::string dst_imgName);

//PCImage�N���X�̃e�X�g������֐�
int PCIclasstest();

//�_�Q�摜���쐬����N���X
class PCImage
{
private:
	cv::Mat pcimage[4];				//�摜1
	cv::Mat* pcimage_ptr;			//���ݎQ�Ƃ��Ă���摜�ւ̃|�C���^

	std::string dirname;			//�f�B���N�g����
	std::string img_name[4];		//�ۑ����̉摜��
	std::string* img_name_ptr;

	int img_width;					//�p�ӂ���摜�̕�
	int img_height;					//�p�ӂ���摜�̍���
	int coefficient;				//�f�[�^���𑜓x�ɍ��킹��W��
	int imgval_increment;			//��f�l�̑�����
	int limit , limitpix;			//���̉摜��ǂݍ��ރ{�[�_�[���C��(m)(pix)

	enum Direction { NONE , TOP , RIGHT, BOTTOM, LEFT , CENTER  };
	enum NowNumber { ZERO , ONE, TWO, THREE };

	bool isPrepareTOP, isPrepareRIGHT, isPrepareBOTTOM, isPrepareLEFT;

	Direction imageCondition[4];
	NowNumber nowimage;					//���ݎQ�Ƃ��Ă���摜�̔ԍ�

public:
	PCImage();
	PCImage( int resolution );
	PCImage( int width, int height, int resolution);
	~PCImage();

	void writePoint(float x_val, float y_val);
	void writePoint(float x_val, float y_val , float pos_x , float pos_y);

	int readPoint(int x_val, int y_val);
	int checkPosition(float pos_x, float pos_y);
	int prepareImage(Direction direction);
	int getEmptyImage();
	void getImageNumber(int xy[] );

	int savePCImage();

	std::string getDirname();


};


#endif