#define _USE_MATH_DEFINES

#include "urg_unko.h"
#include "open_urg_sensor.c"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#define PI 3.14

using namespace std;
using namespace cv;

float scaninterval = 0.0;//�v�������{����Œ�Ԋu[mm]

//URG�֌W�̃O���[�o���ϐ�
enum {
	CAPTURE_TIMES = 1,
};

float chairpos_old = 0.0;//�Ԃ����̎ԗւ̒��O�̐i�s 
float chairpos = 0.0;//�Ԃ����̌��݂̐i�s 
float DIS_old = 0.0;

float startpos[2] = {};	//����J�n�ʒu���猩�����݂̈ʒu
float startpos_old[2] = {};	//���O�̈ʒu��ۑ�����ϐ�

//���l�\���p
Mat picture;
Mat arroypic;
Mat rotatepic;
Mat affine_mat;

bool isInitialized = false;


//���E�ւ̃G���R�[�_���f�[�^�ώZ�p
int data_L = 0, data_R = 0;

//���l�̕\��
void meter(Mat pic, float data[] , string name[], int NumOfData)
{
	int baseline = 0;
	pic = Mat::zeros(500, 1000, CV_8UC3);
	Size textSize = getTextSize("OpenCV ", FONT_HERSHEY_SIMPLEX, 2, 2, &baseline);
	for (int i = 0; i < NumOfData; i++){
		putText(pic, name[i] + " : " + to_string(data[i]), cv::Point(50, 50 + textSize.height*i), FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(0, 200,0 ), 2, CV_AA);
	}
	
	imshow("meter", pic);
}

//��]�p����ŕ\��
void showDirection(float radian)
{
	//rotatepic = Mat::zeros(arroypic.cols, arroypic.rows, CV_8UC3);
	affine_mat = getRotationMatrix2D(Point(arroypic.cols / 2, arroypic.rows / 2), -radian / PI * 180, 1);
	warpAffine(arroypic, rotatepic, affine_mat, arroypic.size());
	imshow("direction", rotatepic);
}

int CommClose(HANDLE hComm)
{
	if (hComm){
		CloseHandle(hComm);
	}

	return 1;
}

/*
*	�T�v:
*		Arduino���獶�E�ւ̕ω��ʂ��擾���C�ړ��ʁC��]�ʂ�ώZ����
*	�����F
*		HANDLE hComm	Arduino�̃n���h��
*		float& dist		�ړ��ʂ�ώZ����ϐ��ւ̎Q��
*		float& rad		��]�ʂ�ώZ����ϐ��ւ̎Q��
*	�Ԃ�l:
*		int ret	�����������ǂ���
*/
int Encoder(HANDLE hComm, float& dist, float& rad)
{
	unsigned char	sendbuf[1];
	unsigned char	receive_data[2];
	int				ret;
	float			DL, DR, DIS, ANG;
	unsigned long	len;

	// �n���h���`�F�b�N
	if( !hComm )	return -1;

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

	//����������Ă��Ȃ���Ώ�����(���߂̃f�[�^���̂Ă�)
	if (!isInitialized)
	{
		isInitialized = true;
		return 0;
	}

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

	//�擾�����l�𕄍����ɑ��
	signed char receive_char1, receive_char2;
	receive_char1 = receive_data[0];
	receive_char2 = receive_data[1];

	//cout << "\n\n\ndata1:" << data1 << " ,  data2:" << data2 << endl << endl;

	//char�^�𐮐��l�Ƃ��ĕ\��
	//cout << "\n\n\ndata1:" << std::showbase << std::dec << static_cast<int>(receive_char1) << " ,  data2:" << std::showbase << std::dec << static_cast<int>(receive_char2) << endl << endl;
	//int data[] = { static_cast<int>(receive_char1), static_cast<int>(receive_char2) };
	//meter(picture, data, 2);
	data_L += static_cast<int>(receive_char1);
	data_R += static_cast<int>(receive_char2);
	cout << "\n\n\ndata_L:" << data_L << " ,  data_R:" << data_R << endl << endl;


	//DL = receive_data[0] * 2.5;
	//DR = receive_data[1] * 2.5;
	/*
	DL = (signed int)data1 * 24.78367538;
	DR = (signed int)data2 * 24.78367538;
	*/

	//���E�ւ̉�]�ʂ���ړ��ʂ��v�Z
	DL = receive_char1 * 24.87094184;
	DR = receive_char2 * 24.87094184;

	//�ړ������C��]�ʂ��v�Z
	DIS = (DL + DR) / 2;
	ANG = (DL - DR) / 530 ;	//�E��]����

	//printf("Distance = %d , Angle = %f \n", (int)DIS, ANG);

	//�ړ��ʁC��]�ʂ�ώZ�p�ϐ��֐ώZ
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
	//�V���A���|�[�g���J���ăn���h�����擾
	hComm = CreateFile("\\\\.\\COM10", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hComm == INVALID_HANDLE_VALUE){
		printf("�V���A���|�[�g���J�����Ƃ��ł��܂���ł����B");
		char z;
		z = getchar();
		return;
	}
	//�|�[�g���J���Ă���ΒʐM�ݒ���s��
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
void getDataUNKOOrigin(int URG_COM[], float URGPOS[][3], int ARDUINO_COM, int NumOfURG)
{
	HANDLE handle_ARDUINO;	//Arduino�p�n���h��

	urg_unko *unkoArray = new urg_unko[NumOfURG];	//urg_unko�^�ϐ��̔z��

	float dist = 0.0;	//�ړ������̐ώZ�p�ϐ�
	float rad = 0.0;	//��]�ʂ̐ώZ�p�ϐ�

	string meterName[] = {"dataL","dataR", "Difference of encoder value(L-R)", "Ratio of encoder value(L/R[%])", 
							"Current coordinates X", "Current coordinates Y", "Moving distance[mm]", "Angle variation[rad]" };
	float		meterData[8] = {};

	arroypic = imread("arrow.jpg");

	//Arduino�ƃV���A���ʐM���s�����߂̃n���h�����擾
	getArduinoHandle(handle_ARDUINO);
	//�G���R�[�_�̏�����
	Encoder(handle_ARDUINO, dist, rad);

	//�ڑ�����URG�̐�����urg_unko�^�I�u�W�F�N�g��������
	for (int i = 0; i < NumOfURG; i++)
	{
		unkoArray[i].init(URG_COM[i], URGPOS[i]);

	}

	//���[�v�𔲂��邽�߂̃L�[���͂�҂E�B���h�E���쐬
	//cv::namedWindow("q");

	//�}�b�v�쐬���s�����[�v
	//'q'����͂���ƃ��[�v�𔲂���
	while (true){

		//�G���R�[�_����ړ��ʁC��]�ʂ��擾
		Encoder(handle_ARDUINO, dist, rad);
		cout << "\n\n dist = " << dist << ", rad = " << rad << endl << endl;

		//�ώZ�����������i�[
		chairpos = dist;

		//URG����f�[�^���擾���C�G���R�[�_�̒l����Ƀ}�b�v�Cpcd�t�@�C�����쐬
		for (int i = 0; i < NumOfURG; i++)
		{
			unkoArray[i].getData4URG(dist, rad);
		}

		//���݂̈ʒu��ۑ�
		startpos_old[0] = startpos[0];
		startpos_old[1] = startpos[1];

		//���݂̈ʒu���X�V
		//����J�n���_�����
		//		x�̐��F�O
		//		y�̐��F��
		startpos[0] += cos(rad) * (chairpos - DIS_old);
		startpos[1] -= sin(rad) * (chairpos - DIS_old);
		printf("startpos[0] = %f , startpos[1] = %f\n", startpos[0], startpos[1]);

		//���݂̈ړ��ʂ�ۑ�
		DIS_old = chairpos;

		//'q'�����͂��ꂽ�烋�[�v�𔲂���
		if (cv::waitKey(1) == 'q')
		{
			delete[] unkoArray;
			break;
		}

		{
			meterData[0] = data_L;
			meterData[1] = data_R;
			meterData[2] = data_L - data_R;
			if (data_R)	meterData[3] = (float)data_L / (float)data_R * 100;
			else meterData[3] = 0;
			meterData[4] = startpos[0];
			meterData[5] = startpos[1];
			meterData[6] = dist;
			meterData[7] = rad;

			meter(picture, meterData, meterName, 6);
			showDirection(rad);
		}


	}

	//Arduino�̃n���h�������
	CommClose(handle_ARDUINO);

	return;
}

/********************************************
*
*	��������urg_unko�̎�������
*
*********************************************/

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
	// ������COM�������o��COMport�Ɋi�[
	COMport = COM;

	//�w�肳�ꂽCOM�|�[�g��URG�Ɛڑ�
	connectURG();

	//�ȉ��C�����o�̏�����
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

	//urg�I�u�W�F�N�g�̎擾
	if (open_urg_sensor(&urg, COMport) < 0) {
		return 1;
	}

	//�f�[�^�擾�p�̃��������m��
	data = (long *)malloc(urg_max_data_size(&urg) * sizeof(data[0]));
	if (!data) {
		perror("urg_max_index()");
		return 1;
	}

	printf("URG connected : urg_max_data_size =  %d \n", urg_max_data_size(&urg));
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
	//�f�[�^�擾
#if 0
	//�f�[�^�̎擾�͈͂�ύX����ꍇ
	urg_set_scanning_parameter(&urg,
		urg_deg2step(&urg, -90),
		urg_deg2step(&urg, +90), 0);
#endif

	//����̊J�n
	urg_start_measurement(&urg, URG_DISTANCE, 1, 0);

	for (int i = 0; i < CAPTURE_TIMES; ++i) {
		int n;

		//����f�[�^�̎擾
		n = urg_get_distance(&urg, data, &time_stamp);
		if (n <= 0) {
			printf("urg_get_distance: %s\n", urg_error(&urg));
			free(data);
			urg_close(&urg);
			return 1;
		}

		//�ώZ������]�p���i�[
		urgpos[2] = rad;

		//����f�[�^����}�b�v�Cpcd�t�@�C�����쐬
		set_3D_surface(n);
	}

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

	long min_distance;
	long max_distance;

	if (chairpos >= 0){//�ʒu�̎擾�����������Ƃ�

		if (abs(chairpos - chairpos_old) >= scaninterval){//�ʒu���ݒ�l�ȏ㓮�����Ƃ�

			// �S�Ẵf�[�^�� X-Y �̈ʒu���擾
			//����Ɏ擾�����f�[�^�̍ő�l�C�ŏ��l���擾
			urg_distance_min_max(&urg, &min_distance, &max_distance);

			//pcd�t�@�C���̏�����
			pcdinit();

			//�f�[�^�̐��������ۂ̍��W���v�Z���ă}�b�v�Cpcd�t�@�C���ɏ�������
			for (int i = 0; i < data_n; ++i) {
				long l = data[i];	//�擾�����_�܂ł̋���
				double radian;
				float x, y, z;
				float pointpos[3];

				//�ُ�l�Ȃ�Ƃ΂�
				if ((l <= min_distance) || (l >= max_distance)) {
					continue;
				}

				//�_�܂ł̊p�x���擾����xy�ɕϊ�
				//(�ɍ��W�Ŏ擾�����f�[�^���f�J���g���W�ɕϊ�)
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
				pcdWrite(pointpos[0] / 1000, pointpos[1] / 1000);

			}
			//�P�X�L��������pcd�t�@�C����ۑ�
			pcdSave();
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
	//�t�@�C�������w�肵�ăt�@�C���X�g���[�����J��
	ofs.open( "./" + pcimage.getDirname() + "/pointcloud_" + std::to_string(pcdnum) + ".pcd");

	//pcd�t�@�C���ԍ���i�߂ăf�[�^���J�E���g�p�ϐ���������
	pcdnum++;
	pcdcount = 0;

	//�w�b�_���L��
	ofs << "# .PCD v.7 - Point Cloud Data file format\n"
		<< "VERSION .7\n"
		<< "FIELDS x y z\n"
		<< "SIZE 4 4 4\n"
		<< "TYPE F F F\n"
		<< "COUNT 1 1 1\n"
		<< "WIDTH 400\n"
		<< "HEIGHT 1\n"
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
	//�f�[�^����������Ńf�[�^�����J�E���g
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
	//�ŏI�I�ȃf�[�^����ǋL
	ofs.seekp(0, ios_base::beg);

	ofs << "# .PCD v.7 - Point Cloud Data file format\n"
		<< "VERSION .7\n"
		<< "FIELDS x y z\n"
		<< "SIZE 4 4 4\n"
		<< "TYPE F F F\n"
		<< "COUNT 1 1 1\n"
		<< "WIDTH " + std::to_string(pcdcount) + "\n"
		<< "HEIGHT 1\n"
		<< "VIEWPOINT 0 0 0 1 0 0 0\n"
		<< "POINTS " + std::to_string(pcdcount) + "\n"
		<< "DATA ascii" << endl;

	//�t�@�C���X�g���[�����g
	ofs.close();
	ofs.flush();
}
