#define _USE_MATH_DEFINES

#include "urg_unko.h"
#include "SharedMemory.h"
#include "open_urg_sensor.c"
#include "Timer.h"
#include "receiveAndroidSensors.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#define PI 3.14159265359

#define DEBUG_WRITELINE

#define KAISUU 10

using namespace std;
using namespace cv;

float scaninterval = 0.0;//�v�������{����Œ�Ԋu[mm]

//URG�֌W�̃O���[�o���ϐ�
enum {
	CAPTURE_TIMES = 1,
};

rcvAndroidSensors rcvDroid(24);
float defaultOrientation[3];

float chairpos_old = 0.0;//�Ԃ����̎ԗւ̒��O�̐i�s 
float chairpos = 0.0;//�Ԃ����̌��݂̐i�s 
float DIS_old = 0.0;

float startpos[2] = {};	//����J�n�ʒu���猩�����݂̈ʒu
float startpos_old[2] = {};	//���O�̈ʒu��ۑ�����ϐ�

//���l�\���p
Mat picture;
Mat arrowpic;
Mat rotatepic;
Mat affine_mat;

bool isInitialized = false;


//���E�ւ̃G���R�[�_���f�[�^�ώZ�p
int data_L = 0, data_R = 0;

// pcimage�̈����D�摜�̃T�C�Y�Ɖ𑜓x
// main.cpp�ŃR�}���h���C��
extern int imgWidth, imgHeight, imgResolution;


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
void showDirection(float radian , string showName)
{
	affine_mat = getRotationMatrix2D(Point(arrowpic.cols / 2, arrowpic.rows / 2), -radian / PI * 180, 1);
	warpAffine(arrowpic, rotatepic, affine_mat, arrowpic.size());
	putText(rotatepic, to_string((int)(-radian / (2 * PI))), cv::Point(20, 50), FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(100, 0, 230), 2, CV_AA);
	imshow("direction" + showName, rotatepic);
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

	float			droidOrientation[3];

	// �n���h���`�F�b�N
	if( !hComm )	return -1;
	// �o�b�t�@�N���A
	memset(sendbuf, 0x00, sizeof(sendbuf));
	// �p�P�b�g�쐬
	sendbuf[0] = (unsigned char)1;
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

	//�擾�����l�𕄍����ɑ��
	signed char receive_char1, receive_char2;
	receive_char1 = receive_data[0];
	receive_char2 = receive_data[1];

	//cout << "\n\n\ndata1:" << data1 << " ,  data2:" << data2 << endl << endl;

	//char�^�𐮐��l�Ƃ��ĕ\��
	//cout << "\n\n\ndata1:" << std::showbase << std::dec << static_cast<int>(receive_char1) << " ,  data2:" << std::showbase << std::dec << static_cast<int>(receive_char2) << endl << endl;

	// �f�[�^��ώZ
	data_L += static_cast<int>(receive_char1);
	data_R += static_cast<int>(receive_char2);
	//cout << "\n\n\ndata_L:" << data_L << " ,  data_R:" << data_R << endl << endl;

	//���E�ւ̉�]�ʂ���ړ��ʂ��v�Z
	DL = receive_char1 * 24.0086517664 / 1.005 ;
	DR = receive_char2 * 23.751783167;

	//�ړ������C��]�ʂ��v�Z
	DIS = (DL + DR) / 2;
	ANG = -(DL - DR) / 530;	//�E��]����

	//�ړ��ʁC��]�ʂ�ώZ�p�ϐ��֐ώZ
	dist += DIS;
	rad += ANG;
	//rcvDroid.getOrientationData(droidOrientation);
	//rad = droidOrientation[0] - defaultOrientation[0];

	//printf("Distance = %d , Angle = %f \n", (int)dist, rad);

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
void getArduinoHandle(int arduinoCOM , HANDLE& hComm)
{
	//�V���A���|�[�g���J���ăn���h�����擾
	string com = "\\\\.\\COM" + to_string(arduinoCOM);
	hComm = CreateFile( com.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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

	Timer	timer; //���[�v�̊Ԋu�����p�^�C�}�[
	int		interval;
	timer.Start();

	float dist = 0.0;	//�ړ������̐ώZ�p�ϐ�
	float rad = 0.0;	//��]�ʂ̐ώZ�p�ϐ�

	// ���l�\���p�̕ϐ��B
	string meterName[] = {"dataL","dataR", "Difference of encoder value(L-R)", "Ratio of encoder value(L/R[%])", 
							"Current coordinates X", "Current coordinates Y", "Moving distance[mm]", "Angle variation[deg]",
							"Interval[millisec]"};
	float		meterData[9] = {};

	// csForm�Ƃ̌�����
	// ���[�v������^�C�~���O�Ƃ��̂����p
	SharedMemory<int> shMemInt("MappingFormInt");
	enum {ISEND , INTERVALTIME};
	shMemInt.setShMemData(false, ISEND);

	// �p���\���p���̓ǂݍ���
	arrowpic = imread("arrow.jpg");
	if (arrowpic.empty()) cout << "No arrow image" << endl;
	arrowpic = ~arrowpic;

	//rcvDroid.getOrientationData(defaultOrientation);

	//Arduino�ƃV���A���ʐM���s�����߂̃n���h�����擾
	getArduinoHandle(ARDUINO_COM,handle_ARDUINO);
	//�G���R�[�_�̏�����
	Encoder(handle_ARDUINO, dist, rad);

	//�ڑ�����URG�̐�����urg_unko�^�I�u�W�F�N�g��������
	for (int i = 0; i < NumOfURG; i++)
	{
		unkoArray[i].init(URG_COM[i], URGPOS[i]);
		unkoArray[i].setWriteLine(false);
	}

	//�}�b�v�쐬���s�����[�v
	//'q'����͂���ƃ��[�v�𔲂���
#ifndef KAISUU
	while (true){
#else
	for (int i = 0; i < 10; i++){
#endif
		// �����̊Ԋu���w�莞�Ԃ�����
		if (timer.getLapTime(1, Timer::millisec, false) < shMemInt.getShMemData(INTERVALTIME)) continue;
		interval = timer.getLapTime();

		//�G���R�[�_����ړ��ʁC��]�ʂ��擾
		Encoder(handle_ARDUINO, dist, rad);
		//cout << "\n\n dist = " << dist << ", rad = " << rad << endl << endl;

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
		//printf("startpos[0] = %f , startpos[1] = %f\n", startpos[0], startpos[1]);

		//���݂̈ړ��ʂ�ۑ�
		DIS_old = chairpos;

		//'q'�����͂��ꂽ�烋�[�v�𔲂���
		// �������͋��L��������0�Ԓn��0�����͂�(ry
		if (cv::waitKey(1) == 'q' || shMemInt.getShMemData(0)) break;

		// ���[�^�[�̕\����ݒ�
		{
			meterData[0] = data_L;
			meterData[1] = data_R;
			meterData[2] = data_L - data_R;
			if (data_R)	meterData[3] = (float)data_L / (float)data_R * 100;
			else meterData[3] = 0;
			meterData[4] = startpos[0];
			meterData[5] = startpos[1];
			meterData[6] = dist;
			meterData[7] = rad / PI *180;
			meterData[8] = interval;

			meter(picture, meterData, meterName, 9);
			showDirection( -rad , ":Encoder" );
		}

	}


	//New�Ŋm�ۂ����z��̉��
	delete[] unkoArray;
	// �\�����Ă���摜�����
	destroyAllWindows();
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
// urg_unko��z��Ő錾�����Ƃ��Ɉ�����n���Ȃ��̂�pcimage�̈����͂Ƃ肠�����O���[�o���ϐ�����󂯎��
urg_unko::urg_unko() :pcimage(::imgWidth,::imgHeight,::imgResolution)
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
void urg_unko::init(int COM, float pos[])
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
	pcimage.savePCImage();
	disconnectURG();
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
	//printf(" set_3D_surface data_n = %d \n", data_n);

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

			float droidOrientation[3];
			float droidGPS[3];
			rcvDroid.getOrientationData(droidOrientation);
			rcvDroid.getGPSData(droidGPS);

			//�f�[�^�̐��������ۂ̍��W���v�Z���ă}�b�v�Cpcd�t�@�C���ɏ�������
			for (int i = 0; i < data_n; ++i) {
				long l = data[i];	//�擾�����_�܂ł̋���
				double radian;
				float x, y, z;
				float pointpos[3];

				//�ُ�l�Ȃ�Ƃ΂�
				if ((l <= min_distance) || (l >= max_distance)) {
					pcdWrite(0, 0, startpos[0] / 1000, startpos[1] / 1000, droidOrientation, droidGPS);
					continue;
					l = max_distance;
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

				// ���W��ۑ�
#ifndef DEBUG_WRITELINE
				// �_�̂ݏ�������
				pcimage.writePoint(pointpos[0] / 1000, pointpos[1] / 1000);
				pcdWrite(pointpos[0] / 1000, pointpos[1] / 1000);
#else
				// �_����������Ō��ݒn����̒���������
				pcimage.writePoint(pointpos[0] / 1000, pointpos[1] / 1000, startpos[0] / 1000, startpos[1] / 1000);
				pcdWrite(pointpos[0] / 1000, pointpos[1] / 1000, startpos[0] / 1000, startpos[1] / 1000 ,droidOrientation , droidGPS);
#endif

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

void urg_unko::pcdWrite(float x, float y , float pos_x , float pos_y , float droidAngle[] , float droidGPS[])
{
	//�f�[�^����������Ńf�[�^�����J�E���g
	ofs << x << ", " << y << ", " << pos_x << ", " << pos_y << ", " << droidAngle[0] << ", " << droidAngle[1] << ", " << droidAngle[2] << ", " << droidGPS[0] << ", " << droidGPS[1] << ", " << droidGPS[2] << ", " << endl;
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

void urg_unko::setWriteLine(bool isLine)
{
	pcimage.isWriteLine = isLine;
}

std::string	urg_unko::getDirName()
{
	return pcimage.getDirname();
}
void urg_unko::save()
{
	pcimage.savePCImage();
}