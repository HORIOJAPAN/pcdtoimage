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
	cv::Mat pcimage;				//�摜

	std::string dirname;			//�f�B���N�g����
	std::string img_name;			//�ۑ����̉摜��

	int img_width;					//�p�ӂ���摜�̕�
	int img_height;					//�p�ӂ���摜�̍���
	int coefficient;				//�f�[�^���𑜓x�ɍ��킹��W��
	int imgval_increment;			//��f�l�̑�����
	int limit , limitpix;			//���̉摜��ǂݍ��ރ{�[�_�[���C��(m)(pix)

	const int lineVal = 200;		// ����`�悷��Ƃ��̉�f�l

public:
	PCImage();
	PCImage( int resolution );
	PCImage( int width, int height, int resolution);
	~PCImage();

	// �摜�ɓ_����������
	void writePoint(float x_val, float y_val);
	// �_���������񂾌�ɐ���`�悷��
	void writePoint(float x_val, float y_val , float pos_x , float pos_y);

	// �w�肵����_�����Ԓ�����`��
	void writeLine(float x_val, float y_val, float pos_x, float pos_y);

	// ��f�l���擾����
	int readPoint(int x_val, int y_val);

	// �摜��ۑ�
	int savePCImage();

	// �ۑ������f�B���N�g�������擾
	std::string getDirname();

	//���݂̎����𕶎���Ŏ擾����
	static void getNowTime(std::string& nowstr);

};


#endif