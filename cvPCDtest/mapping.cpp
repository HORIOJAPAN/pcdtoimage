#define _USE_MATH_DEFINES

#include "urg_unko.h"
#include "open_urg_sensor.c"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

float scaninterval = 0.0;//�v�������{����Œ�Ԋu[mm]

//URG�֌W�̃O���[�o���ϐ�
enum {
	CAPTURE_TIMES = 1,
};

float chairpos_old = 0.0;//�Ԃ����̎ԗւ̒��O�̐i�s 
float chairpos = 0.0;//�Ԃ����̌��݂̐i�s 
float DIS_old = 0.0;


int CommClose(HANDLE hComm)
{
	if (hComm){
		CloseHandle(hComm);
	}

	return 1;
}

int Encoder(HANDLE hComm, float& dist, float& rad)
{
	unsigned char	sendbuf[1];
	unsigned char	receive_data[2];
	int				ret;
	float			DL, DR, DIS, ANG;
	unsigned long	len;

	// �n���h���`�F�b�N
	/*if( !hComm ){
	return -1;
	}*/


	// �o�b�t�@�N���A
	memset(sendbuf, 0x00, sizeof(sendbuf));

	// �p�P�b�g�쐬
	sendbuf[0] = (unsigned char)1;				// �w�b�_�[1

	// �ʐM�o�b�t�@�N���A
	PurgeComm(hComm, PURGE_RXCLEAR);

	// ���M
	ret = WriteFile(hComm, &sendbuf, 1, &len, NULL);

	// �o�b�t�@�N���A
	memset(receive_data, 0x00, sizeof(receive_data));

	// �ʐM�o�b�t�@�N���A
	PurgeComm(hComm, PURGE_RXCLEAR);

	// Arduino����f�[�^����M
	ret = ReadFile(hComm, &receive_data, 2, &len, NULL);
	//cout << static_cast<bitset<8>>(receive_data[0]) << "," << static_cast<bitset<8>>(receive_data[1] )<< endl;

	//int data1 = 0, data2 = 0;
	/*
	if ((int)receive_data[0] == 255)
	{
		data1 = -256 | receive_data[0];
	}
	else data1 = receive_data[0];
	if ((int)receive_data[1] == 255)
	{
		data2 = -256 | receive_data[1];
	}
	else data2 = receive_data[1];
	*/
	signed char receive_char1, receive_char2;
	receive_char1 = receive_data[0];
	receive_char2 = receive_data[1];

	//cout << "\n\n\ndata1:" << data1 << " ,  data2:" << data2 << endl << endl;
	cout << "\n\n\ndata1:" << std::showbase << std::dec << static_cast<int>(receive_char1) << " ,  data2:" << std::showbase << std::dec << static_cast<int>(receive_char2) << endl << endl;

	//DL = receive_data[0] * 2.5;
	//DR = receive_data[1] * 2.5;
	/*
	DL = (signed int)data1 * 24.78367538;
	DR = (signed int)data2 * 24.78367538;
	*/

	DL = receive_char1 * 24.78367538;
	DR = receive_char2 * 24.78367538;

	DIS = (DL + DR) / 2;
	ANG = (DL - DR) / 526 ;

	printf("Distance = %d , Angle = %f \n", (int)DIS, ANG);

	dist += DIS;
	rad += ANG;

	printf("Distance = %d , Angle = %f \n", (int)dist, rad);

	return ret;
}

/*
 *	�T�v:
 *		Arduino�ƃV���A���ʐM���s�����߂̃n���h�����擾����
 *	�����F
 *		HANDLE&	hComm	�n���h���ϐ��ւ̎Q��
 *	�Ԃ�l:
 *		�Ȃ�
 */
void getArduinoHandle(HANDLE& hComm)
{
	hComm = CreateFile("\\\\.\\COM10", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hComm == INVALID_HANDLE_VALUE){
		printf("�V���A���|�[�g���J�����Ƃ��ł��܂���ł����B");
		char z;
		z = getchar();
		return;
	}
	else
	{
		DCB lpTest;
		GetCommState(hComm, &lpTest);
		lpTest.BaudRate = 9600;
		lpTest.ByteSize = 8;
		lpTest.Parity = NOPARITY;
		lpTest.StopBits = ONESTOPBIT;
		SetCommState(hComm, &lpTest);
	}
	return;
}

/*
*	�T�v:
*		�}�b�v�쐬�p�̃��C�����[�v
*		'q'�ŏI��
*		�K�؂ɏI�����Ȃ��Ɖ摜���ۑ�����Ȃ�
*	�����F
*		int URG_COM[]	�ڑ�����URG��COM�|�[�g�̔z��
*		int ARDUINO_COM	�ڑ�����Arduino��COM�|�[�g

*	�Ԃ�l:
*		�Ȃ�
*/
void getDataUNKO(int URG_COM[], int ARDUINO_COM)
{
	HANDLE handle_ARDUINO;

	urg_unko unkoArray[URGCOUNT];

	float dist = 0.0;
	float rad = 0.0;

	float urgPOS[][3] = { 1100.0, 285.0, 0.0,
		20.0, 443.0, 0.0 };

	getArduinoHandle(handle_ARDUINO);

	Sleep(2000);

	cout << sizeof(URG_COM) / sizeof(int) << endl;

	for (int i = 0; i < URGCOUNT; i++)
	{
		unkoArray[i].init(URG_COM[i], urgPOS[i]);

	}

	cv::namedWindow("q");

	while (true){

		Encoder(handle_ARDUINO, dist, rad);
		cout << "\n\n dist = " << dist << ", rad = " << rad << endl << endl;

		for (int i = 0; i < URGCOUNT; i++)
		{
			unkoArray[i].getData4URG(dist, rad);
		}



		DIS_old = chairpos;

		if (cv::waitKey(1) == 'q')
		{
			break;
		}

	}

	CommClose(handle_ARDUINO);

	return;
}

/*
*
*	��������urg_unko�̎�������
*
*/

/*
*	�T�v:
*		�R���X�g���N�^
*		pcimage�CCOMport�Cpcdnum������������
*	�����F
*		�Ȃ�
*	�Ԃ�l:
*		�Ȃ�
*/
urg_unko::urg_unko() :pcimage(5000, 5000, 5)
{
	COMport = 0;
	pcdnum = 0;

}

/*
*	�T�v:
*		�I�u�W�F�N�g�̏���������
*	�����F
*		int COM		URG��COM�|�[�g�ԍ�
*		float pos[]	NCWC�̉�]���S���猩��URG�̈ʒu
*	�Ԃ�l:
*		�Ȃ�
*/
void urg_unko::init(int COM , float pos[])
{
	COMport = COM;
	urg_unko::connectURG();

	startpos[0] = 0.0;
	startpos[1] = 0.0;

	startpos_old[0] = 0.0;
	startpos_old[1] = 0.0;

	for (int i = 0; i < 3; i++)
	{
		urgpos[i] = pos[i];
	}

}

/*
*	�T�v:
*		URG�̐ڑ���ؒf
*	�����F
*		�Ȃ�
*	�Ԃ�l:
*		0
*/
int urg_unko::disconnectURG(){

	//�ؒf
	free(data);
	urg_close(&urg);

	printf("URG disconnected \n");
	return 0;

}

/*
*	�T�v:
*		�f�X�g���N�^
*		URG�̐ؒf�C�摜�̕ۑ����s��
*	�����F
*		�Ȃ�
*	�Ԃ�l:
*		�Ȃ�
*/
urg_unko::~urg_unko()
{
	disconnectURG();
	pcimage.savePCImage();
}

/*
*	�T�v:
*		URG�Ɛڑ�����
*		init(int COM , float pos[])�Ŏw�肵��URG�Ɛڑ�����
*	�����F
*		�Ȃ�
*	�Ԃ�l:
*		0
*/
int urg_unko::connectURG(){
	if (open_urg_sensor(&this->urg, COMport) < 0) {
		return 1;
	}

	this->data = (long *)malloc(urg_max_data_size(&this->urg) * sizeof(this->data[0]));
	if (!this->data) {
		perror("urg_max_index()");
		return 1;
	}

	printf("URG connected : urg_max_data_size =  %d \n", urg_max_data_size(&this->urg));
	return 0;
}

/*
*	�T�v:
*		URG����f�[�^���擾����
*	�����F
*		float& dist	�ώZ���鋗���f�[�^�p�ϐ��̎Q��
*		float& rad	�ώZ�����]�p�f�[�^�p�ϐ��̎Q��
*	�Ԃ�l:
*		0
*/
int urg_unko::getData4URG(float& dist, float& rad){

	int n;
	int i;

	//�f�[�^�擾
#if 0
	//�f�[�^�̎擾�͈͂�ύX����ꍇ
	urg_set_scanning_parameter(&this->urg,
		urg_deg2step(&this->urg, -90),
		urg_deg2step(&this->urg, +90), 0);
#endif

	startpos_old[0] = startpos[0];
	startpos_old[1] = startpos[1];

	urg_start_measurement(&this->urg, URG_DISTANCE, 1, 0);

	for (i = 0; i < CAPTURE_TIMES; ++i) {
		n = urg_get_distance(&this->urg, this->data, &this->time_stamp);
		if (n <= 0) {
			printf("urg_get_distance: %s\n", urg_error(&this->urg));
			free(this->data);
			urg_close(&this->urg);
			return 1;
		}

		set_3D_surface( n );

		chairpos = dist;
		urgpos[2] = rad;

	}
	startpos[0] = + cos(urgpos[2]) * (chairpos - DIS_old) + startpos_old[0];
	startpos[1] = - sin(urgpos[2]) * (chairpos - DIS_old) + startpos_old[1];
	printf("startpos[0] = %f , startpos[1] = %f\n", startpos[0], startpos[1]);

	return 0;

}

/*
*	�T�v:
*		�擾�����f�[�^������ۂ̓񎟌������v�Z���ă}�b�v�Cpcd�t�@�C���ւ̏������݂��s��
*	�����F
*		int data_n	�擾�����f�[�^�̐�
*	�Ԃ�l:
*		�Ȃ�
*/
void urg_unko::set_3D_surface( int data_n)
{
	printf(" set_3D_surface data_n = %d \n", data_n);

	(void)time_stamp;

	int i;
	long min_distance;
	long max_distance;

	if (chairpos >= 0){//�ʒu�̎擾�����������Ƃ�

		if (abs(chairpos - chairpos_old) >= scaninterval){//�֎q�̈ʒu���ݒ�l�ȏ㓮�����Ƃ�

			// �S�Ẵf�[�^�� X-Y �̈ʒu���擾
			urg_distance_min_max(&urg, &min_distance, &max_distance);

			pcdinit();

			for (i = 0; i < data_n; ++i) {
				long l = data[i];
				double radian;
				float x, y, z;
				float pointpos[3];

				if ((l <= min_distance) || (l >= max_distance)) {
					continue;
				}

				radian = urg_index2rad(&urg, i);
				x = (float)(l * cos(radian));
				y = (float)(l * sin(radian));
				z = 120.0;

				//2�������ʂ̍��W�ϊ�
				pointpos[0] = + cos(urgpos[2]) * x + sin(urgpos[2]) * y + cos(urgpos[2]) * (chairpos - DIS_old + urgpos[1]) + startpos[0];
				pointpos[1] = -sin(urgpos[2]) * x + cos(urgpos[2]) * y - sin(urgpos[2]) * (chairpos - DIS_old + urgpos[1]) + startpos[1];
				pointpos[2] = z;

				//���W��ۑ�
				pcimage.writePoint(pointpos[0] / 1000, pointpos[1] / 1000);

				this->pcdWrite(pointpos[0] / 1000, pointpos[1] / 1000);

			}
			this->pcdSave();
		}
	}
}

/*
*	�T�v:
*		pcd�t�@�C�����쐬���ď���������
*	�����F
*		�Ȃ�
*	�Ԃ�l:
*		�Ȃ�
*/
void urg_unko::pcdinit()
{
	ofs.open( "./" + pcimage.getDirname() + "/pointcloud_" + std::to_string(pcdnum) + ".pcd");

	pcdnum++;
	pcdcount = 0;
	ofs << "# .PCD v.7 - Point Cloud Data file format\n"
		<< "VERSION .7\n"
		<< "FIELDS x y z rgb\n"
		<< "SIZE 4 4 4 4\n"
		<< "TYPE F F F F\n"
		<< "COUNT 1 1 1 1\n"
		<< "WIDTH\n"
		<< "HEIGHT\n"
		<< "VIEWPOINT 0 0 0 1 0 0 0\n"
		<< "POINTS 400\n"
		<< "DATA ascii" << endl;
}

/*
*	�T�v:
*		pcd�t�@�C���Ƀf�[�^����������
*	�����F
*		float x	x���W�l
*		float y	y���W�l
*	�Ԃ�l:
*		�Ȃ�
*/
void urg_unko::pcdWrite(float x, float y)
{
	ofs << x << " " << y << " " << "0.0" << endl;
	pcdcount++;
}

/*
*	�T�v:
*		�t�@�C���X�g���[������ĕۑ�����
*	�����F
*		�Ȃ�
*	�Ԃ�l:
*		�Ȃ�
*/
void urg_unko::pcdSave()
{
	ofs.seekp(0, ios_base::beg);

	ofs << "# .PCD v.7 - Point Cloud Data file format\n"
		<< "VERSION .7\n"
		<< "FIELDS x y z rgb\n"
		<< "SIZE 4 4 4 4\n"
		<< "TYPE F F F F\n"
		<< "COUNT 1 1 1 1\n"
		<< "WIDTH\n"
		<< "HEIGHT\n"
		<< "VIEWPOINT 0 0 0 1 0 0 0\n"
		<< "POINTS " + std::to_string(pcdcount) + "\n"
		<< "DATA ascii" << endl;

	ofs.close();
	ofs.flush();
}
