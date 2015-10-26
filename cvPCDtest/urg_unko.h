#ifndef _INC_URG_UNKO
#define _INC_URG_UNKO

#include "urg_sensor.h"
#include "urg_utils.h"
#include "urg_open.h"
#include "pcimage.h"

#include <Windows.h>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_lib.hpp>


#define DEBUG_WRITELINE


//�ڑ�����URG�̌��������Ŕ��f����悤�ɂ����}�N��
#define getDataUNKO(aURGCOM , aURGPOS , ARDUINOCOM) getDataUNKOOrigin( (aURGCOM),(aURGPOS),(ARDUINOCOM),sizeof((aURGCOM))/sizeof(aURGCOM[0])) 

//�w�肵��COM�|�[�g�����
int CommClose(HANDLE hComm);
//Arduino�̃n���h�����擾
void getArduinoHandle(int arduinoCOM, HANDLE& hComm);
//urg_unko��main���[�v
void getDataUNKOOrigin(int URG_COM[], float URGPOS[][3], int ARDUINO_COM, int NumOfURG);

/*
*
*�@URG�Ńf�[�^���擾���ă}�b�s���O���s���N���X
*
*/
class urg_unko{
private:
	/*
	*	�����o�ϐ�
	*/
	int COMport;	//URG��COM�|�[�g

	std::ofstream ofs;	//�t�@�C���X�g���[���I�u�W�F�N�g�Dpcd�t�@�C���쐬�ɗp����
	int pcdnum;			//pcd�t�@�C���̔ԍ����J�E���g����ϐ�
	int pcdcount;		//pcd�t�@�C���ɏ������ޓ_�̐����J�E���g����ϐ�

	//float urgpos[3];	
	//NCWC�̉�]���S���猩��URG�̈ʒu�D�Z���T�̒n�ʂ���̍����C�Z���T�̊�ʒu����̋����C����ѐ����ʂ���̘�p

	float urgpos[4];	
	//NCWC�̉�]���S���猩��URG�̈ʒu�D�Z���T�̒n�ʂ���̍����C�Z���T�̊�ʒu�����x�����̋����C�������̋����C����ѐ����ʂ���̘�p
	
	urg_t urg;			//URG�I�u�W�F�N�g
	long *data = NULL;	
	long time_stamp;

	PCImage pcimage;	//�}�b�v�摜�쐬�p�N���X

	float scaninterval = 0.0;//�v�������{����Œ�Ԋu[mm]

	enum {
		CAPTURE_TIMES = 1,
	};

	float currentCoord_x, currentCoord_y;
	float distance , distance_old;
	float radian;

	/***********************
	 *	private�ȃ��\�b�h  *
	 ***********************/

	//URG�Ƃ̐ڑ���ؒf
	int disconnectURG();
	//URG�Ɛڑ�
	int connectURG();

	//�擾�����f�[�^������ۂ̓񎟌������v�Z���ă}�b�v�Cpcd�t�@�C���ւ̏������݂��s��
	void set_3D_surface(int data_n);

	//pcd�t�@�C�����쐬���ď������ޏ������s��
	void pcdinit();
	//pcd�t�@�C���֓_����������
	void pcdWrite(float x, float y);
	void pcdWrite(float x, float y, float pos_x, float pos_y, float droidAngle[], float droidGPS[]);
	//pcd�t�@�C���ւ̏������݂��I�����ĕۑ�����
	void pcdSave();

public:
	/*
	*	public�ȃ��\�b�h
	*/

	//�R���X�g���N�^
	urg_unko();
	//�f�X�g���N�^
	~urg_unko();
	void save();

	//���g�̏������������s��
	void init(int COM, float pos[]);
	//URG����f�[�^���擾���郁�\�b�h
	int getData4URG(float dist, float rad);

	void setWriteLine(bool isLine);
	std::string	getDirName();

	void updateCurrentCoord(float coord_x, float coord_y);
	void updateCurrentCoord(float coordXY[]);
};


#endif