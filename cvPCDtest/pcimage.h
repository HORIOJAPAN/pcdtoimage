#ifndef _INC_PCIMAGE
#define _INC_PCIMAGE

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_lib.hpp>


const int imageNum = 4;		//���O�ɗp�ӂ���摜�̈�̐�

int getURGdata();

//pcd�t�@�C�������w�肷��Ɖ摜�ɕϊ����ĕۑ�
int save_pcdtoimg(std::string fileName, std::string imgName, int resolution = 1);

//�_�Q�摜�ɏ�������(������)
int save_floorimg(std::string src_imgName, std::string dst_imgName);

//PCImage�N���X�̃e�X�g������֐�
int PCIclasstest();


//�_�Q�摜���쐬����N���X
class PCImage
{
public:
	//enum Direction { NONE, TOP, RIGHT, LEFT, BOTTOM, CENTER, UPPERRIGHT, LOWERRIGHT, UPPERLEFT, LOWERLEFT };		//������\���񋓌^
	//enum Number { ZERO, ONE, TWO, THREE };							//�摜�ԍ�

	//Mat�N���X���p�������_�Q�摜�N���X
	//�摜�ʒu���l�������������s��
	class PCI;
	std::string dirname;					//�쐬����f�B���N�g����

private:
	std::vector<PCI> pcimage;				//�摜�̈�̔z��


	int img_width;							//�p�ӂ���摜�̕�
	int img_height;							//�p�ӂ���摜�̍���
	int coefficient;						//�f�[�^���𑜓x�ɍ��킹��W��
	int imgval_increment;					//��f�l�̑�����
	int limit , limitpix;					//���̉摜��ǂݍ��ރ{�[�_�[���C��(m),(pix)

	//�㉺���E�ɂ��Ď��̉摜���p�ӂ���Ă��邩�ǂ����̃t���O
	bool isPrepareTOP = false, isPrepareRIGHT = false, isPrepareBOTTOM = false, isPrepareLEFT = false;

	int nowimage;						//���ݑ��s���Ă���摜�̔ԍ�

public:
	//�R���X�g���N�^
	PCImage();
	PCImage( int resolution );
	PCImage( int width, int height, int resolution);
	PCImage(PCImage& pcimage_ptr);
	//�f�X�g���N�^
	~PCImage();

	//�摜�ɓ_����������
	void writePoint(float x_val, float y_val);
	void writePoint(float x_val, float y_val , float pos_x , float pos_y);

	//�w�肵���_�̉�f��ǂݎ��
	int readPoint(int x_val, int y_val);

	//���Ȉʒu�ɉ����ĉ摜�̗p�ӂȂǂ̏���������
	int checkPosition(float pos_x, float pos_y);

	//�摜�̗̈�ԍ���₢���킹��Ɛ^�U��Ԃ�
	bool checkPrepare(int x, int y);

	//���̉摜��p�ӂ���
	//int prepareImage(Direction direction);
	int prepareImage(int x , int y);

	//�g���Ă��Ȃ��摜�̔ԍ���Ԃ�
	int getEmptyImage();

	//�摜��ۑ����ė̈���������
	//void savePCImage(Direction direction);
	void savePCImage(int x , int y);

	//�摜��ǂݍ���
	int loadPCImage(int emptyImageNum);

	//���S�摜���w������ɃV�t�g����
	//int shiftCenterImage(Direction direction);
	int shiftCenterImage(int x, int y);

	//���݂̎����𕶎���Ŏ擾����
	static void getNowTime(std::string& nowstr);

};

//Mat�N���X���p�������_�Q�摜�N���X
//�摜�ʒu���l�������������s��
class PCImage::PCI : public cv::Mat
{
private:
	PCImage& pciOut;	//PCImage�N���X�ւ̎Q��

	//PCImage::Direction	imageCondition;			//���݂̏��
	std::string			name;					//�ۑ����̖��O
	int					imageNumXY[2];			//�摜�̈ʒu

public:
	PCI(PCImage& pcimage_outer);
	PCI& operator=(cv::Mat& mat);


	//�摜�����Z�b�g����
	//void setPCI(int x, int y, PCImage::Direction dir);
	void setPCI(int x, int y);

	//�摜�̏�Ԃ�Ԃ�
	//PCImage::Direction getCondition();
	//�摜�̏�Ԃ�ύX����
	//void setCondition(PCImage::Direction dir);

	//�摜�̈ʒu��Ԃ�
	void getImageNumber(int xy[]);

	//�摜����Ԃ�
	// [./(directoryname)/(filename).jpg]
	std::string getName();

	//�摜�̗̈�ԍ���₢���킹��Ɛ^�U��Ԃ�
	bool isCoordinates( int x , int y);
	bool isCoordinates(int xy[]);

	//�摜�̗̈�ԍ��Z�b�g����
	void setCoordinates(int x, int y);
	void setCoordinates(int xy[]);

	//�摜�ɓ_����������
	int writePoint(float x_val, float y_val);

	//�摜��ۑ����ė̈���������
	void release();
	//�摜��ۑ�����
	void savePCImage();
};

#endif