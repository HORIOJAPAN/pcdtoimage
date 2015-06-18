#ifndef _INC_PCIMAGE
#define _INC_PCIMAGE

#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_lib.hpp>


const int imageNum = 4;		//���O�ɗp�ӂ���摜�̈�̐�


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
	enum Direction { NONE, TOP, RIGHT, BOTTOM, LEFT, CENTER };		//������\���񋓌^
	enum NowNumber { ZERO, ONE, TWO, THREE };
	int coefficient;						//�f�[�^���𑜓x�ɍ��킹��W��

	//Mat�N���X���p�������_�Q�摜�N���X
	//�摜�ԍ����l�������������s��
	class PCI : public cv::Mat
	{
	private:
		Direction			imageCondition;
		std::string			name;
		int					imageNumXY[2];
		PCImage&			pciOut;

	public:
		PCI();
		PCI& operator=(cv::Mat& mat);
		//PCI& operator=(PCImage& pciOut);
		void setOuter(PCImage& pciOut);

		//�摜�ɓ_����������
		void writePoint(float x_val, float y_val);
	};

private:
	PCI pcimage[imageNum];					//�摜�̈�̔z��
	PCI* pcimage_ptr;						//���ݎQ�Ƃ��Ă���摜�ւ̃|�C���^

	std::string dirname;					//�쐬����f�B���N�g����
	std::string img_name[imageNum];			//�ۑ����̉摜���̔z��
	std::string* img_name_ptr;

	int img_width;							//�p�ӂ���摜�̕�
	int img_height;							//�p�ӂ���摜�̍���

	int imgval_increment;					//��f�l�̑�����
	int limit , limitpix;					//���̉摜��ǂݍ��ރ{�[�_�[���C��(m)(pix)

	bool isPrepareTOP = false, isPrepareRIGHT = false, isPrepareBOTTOM = false, isPrepareLEFT = false;		//�㉺���E�ɂ��Ď��̉摜���p�ӂ���Ă��邩�ǂ����̃t���O

	Direction imageCondition[imageNum];		//�摜�̈�̏��
	NowNumber nowimage;						//���ݎQ�Ƃ��Ă���摜�̔ԍ�

public:
	//�R���X�g���N�^
	PCImage();
	PCImage( int resolution );
	PCImage( int width, int height, int resolution);
	//�f�X�g���N�^
	~PCImage();
	PCImage& operator=(PCImage& pc);

	//�摜�ɓ_����������
	void writePoint(float x_val, float y_val);
	void writePoint(float x_val, float y_val , float pos_x , float pos_y);

	//�w�肵���_�̉�f��ǂݎ��
	int readPoint(int x_val, int y_val);

	//���Ȉʒu�ɉ����ĉ摜�̗p�ӂȂǂ̏���������
	int checkPosition(float pos_x, float pos_y);

	//���̉摜��p�ӂ���
	int prepareImage(Direction direction);

	//�g���Ă��Ȃ��摜�̔ԍ���Ԃ�
	int getEmptyImage();

	//�摜�̈ʒu��Ԃ�
	void getImageNumber(int xy[] );

	//�摜��ۑ����ė̈���������
	int savePCImage( int imgNum);
	void savePCImage(Direction direction);

	//�摜��ǂݍ���
	int loadPCImage(int emptyImageNum);

	//���S�摜���w������ɃV�t�g����
	int shiftCenterImage(Direction direction);

};



#endif