#define _USE_MATH_DEFINES

#include "urg_unko.h"
#include "open_urg_sensor.c"


using namespace std;

// pcimage�̈����D�摜�̃T�C�Y�Ɖ𑜓x
// main.cpp�ŃR�}���h���C����������󂯎��
extern int imgWidth, imgHeight, imgResolution;

PCImage urg_unko::pcimage;

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
urg_unko::urg_unko() //:pcimage(::imgWidth, ::imgHeight, ::imgResolution)
	:shMem(SharedMemory<int>("unko"))
{
	COMport = 0;
	pcdnum = 0;

	shMem.reset();
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
int urg_unko::getData4URG(float& dist,float& old, float& rad){
	//�f�[�^�擾
#if 1
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

		//�ώZ��������,��]�p���i�[
		distance_old = old;
		distance = dist;
		radian = rad;

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
void urg_unko::set_3D_surface(int data_n)
{
	//printf(" set_3D_surface data_n = %d \n", data_n);

	(void)time_stamp;

	long min_distance;
	long max_distance;

	if (abs(distance - distance_old) >= scaninterval){//�ʒu���ݒ�l�ȏ㓮�����Ƃ�

		// �S�Ẵf�[�^�� X-Y �̈ʒu���擾
		//����Ɏ擾�����f�[�^�̍ő�l�C�ŏ��l���擾
		urg_distance_min_max(&urg, &min_distance, &max_distance);

		//pcd�t�@�C���̏�����
		pcdinit();

		float droidOrientation[3] = {};
		float droidGPS[3] = {};
		//rcvDroid.getOrientationData(droidOrientation);
		//rcvDroid.getGPSData(droidGPS);

		//�f�[�^�̐��������ۂ̍��W���v�Z���ă}�b�v�Cpcd�t�@�C���ɏ�������
		for (int i = 0; i < data_n; ++i) {
			long l = data[i];	//�擾�����_�܂ł̋���
			double radian;
			float x, y, z;
			float pointpos[3];

			//�ُ�l�Ȃ�Ƃ΂�
			if ((l <= min_distance) || (l >= max_distance)) {
				pcdWrite(0, 0, currentCoord_x / 1000, currentCoord_y / 1000, droidOrientation, droidGPS);
				continue;
				l = max_distance;
			}

			//�_�܂ł̊p�x���擾����xy�ɕϊ�
			//(�ɍ��W�Ŏ擾�����f�[�^���f�J���g���W�ɕϊ�)
			radian = urg_index2rad(&urg, i);
			x = (float)(l * cos(radian));
			y = (float)(l * sin(radian));
			z = 120.0;

			if (x < 1000 && abs(y) < 1000)
			{
				shMem.setShMemData(true, EMARGENCY);
			}

			//2�������ʂ̍��W�ϊ�
			//pointpos[0] = +cos(radian + urgpos[3]) * (x + distance - distance_old + urgpos[1]) + sin(radian + urgpos[3]) * (y + urgpos[2]) + currentCoord_x;
			//pointpos[1] = -sin(radian + urgpos[3]) * (x + distance - distance_old + urgpos[1]) + cos(radian + urgpos[3]) * (y + urgpos[2]) + currentCoord_y;

			pointpos[0] = +cos(this->radian) * x + sin(this->radian) * y + cos(this->radian) * (distance - distance_old + urgpos[1]) + currentCoord_x;
			pointpos[1] = -sin(this->radian) * x + cos(this->radian) * y - sin(this->radian) * (distance - distance_old + urgpos[1]) + currentCoord_y;

			pointpos[2] = z;

			

			// ���W��ۑ�
#ifndef DEBUG_WRITELINE
			// �_�̂ݏ�������
			pcimage.writePoint(pointpos[0] / 1000, pointpos[1] / 1000);
			pcdWrite(pointpos[0] / 1000, pointpos[1] / 1000);
#else
			// �_����������Ō��ݒn����̒���������
			pcimage.writePoint(pointpos[0] / 1000, pointpos[1] / 1000, currentCoord_x / 1000, currentCoord_y / 1000);
			pcdWrite(pointpos[0] / 1000, pointpos[1] / 1000, currentCoord_x / 1000, currentCoord_y / 1000, droidOrientation, droidGPS);
#endif

		}
		//�P�X�L��������pcd�t�@�C����ۑ�
		pcdSave();
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
	ofs.open("./" + pcimage.getDirname() + "/pointcloud_" + std::to_string(pcdnum) + ".pcd");

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

void urg_unko::pcdWrite(float x, float y, float pos_x, float pos_y, float droidAngle[], float droidGPS[])
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
void urg_unko::updateCurrentCoord(float coord_x, float coord_y)
{
	currentCoord_x = coord_x;
	currentCoord_y = coord_y;
}
void urg_unko::updateCurrentCoord(float coordXY[])
{
	currentCoord_x = coordXY[0];
	currentCoord_y = coordXY[1];
}

void urg_unko::initPCImage(PCImage& pci)
{
	pcimage = pci;
}

void urg_unko::setPCImageColor(PCImage::BGR bgr)
{
	pcimage.setColor(bgr);
}