#ifndef _INC_URG_UNKO
#define _INC_URG_UNKO

#include "urg_sensor.h"
#include "urg_utils.h"
#include "urg_open.h"
#include "pcimage.h"

#include <Windows.h>
#include <fstream>

#define URGCOUNT 2

//�w�肵��COM�|�[�g�����
int CommClose(HANDLE hComm);
//Arduino�̃n���h�����擾
void getArduinoHandle(HANDLE& hComm);
//urg_unko��main���[�v
int getDataUNKO(int URG_COM[], int ARDUINO_COM );

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

	float urgpos[3];	//NCWC�̉�]���S���猩��URG�̈ʒu�D�Z���T�̒n�ʂ���̍����C�Z���T�̊�ʒu����̋����C����ѐ����ʂ���̘�p
	float startpos[2];	//����J�n�ʒu���猩�����݂̈ʒu
	float startpos_old[2] ;

	urg_t urg;			//URG�I�u�W�F�N�g
	long *data = NULL;	
	long time_stamp;

	PCImage pcimage;	//�}�b�v�摜�쐬�p�N���X

	/*
	*	private�ȃ��\�b�h
	*/

	//URG�Ƃ̐ڑ���ؒf
	int disconnectURG();
	//URG�Ɛڑ�
	int connectURG();

	void set_3D_surface(int data_n);

	//pcd�t�@�C�����쐬���ď������ޏ������s��
	void pcdinit();
	//pcd�t�@�C���֓_����������
	void pcdWrite(float x, float y);
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

	//���g�̏������������s��
	void init(int COM , float pos[]);
	//URG����f�[�^���擾���郁�\�b�h
	int getData4URG(float& dist, float& rad);

};


#endif