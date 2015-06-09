#include "pcdtoimg.h"

#include <opencv2/opencv.hpp>
#include <opencv2/opencv_lib.hpp>

using namespace cv;
using namespace std;

/*
*pcd�t�@�C�������w�肷��Ɖ摜�ɕϊ����ĕۑ�
*����:
*	string fileName �t�@�C����
*	string imgName �ۑ�����摜��
*	int resolution 1pix��cm�l���ɂ��邩
*�Ԃ�l:
*	���� 0  ���s�@1
*/
int save_pcdtoimg(string fileName, string imgName ,int resolution )
{
	const int img_width = 1000;					//�p�ӂ���摜�̕�
	const int img_heigth = 1000;				//�p�ӂ���摜�̍���
	const int coefficient = 100 / resolution;	//�f�[�^���𑜓x�ɍ��킹��W��
	const int imgval_increment = 80;			//��f�l�̑�����

	char	z;

	//�_�Q���W����摜�̍��W�ɕϊ������l
	int	x_val, y_val;

	string str ,x_str ,y_str;
	string searchLine("nan");
	string searchWord(" ");
	string::size_type x_pos , y_pos;

	Mat pcd_img( Size( img_width, img_heigth ) , CV_8U, Scalar::all(0) );

	//pcd�t�@�C����ǂݍ���
	ifstream ifs( fileName );
	if (ifs.fail())
	{
		cerr << "False" << endl;
		z = getchar();
		return EXIT_FAILURE;
	}

	//�w�b�_�������Ƃ΂����߂̃��[�v
	for (int i = 0 ; i <= 11; i++){
		getline(ifs, str);
	}

	while (getline(ifs, str))
	{
		//nan�̗�Ȃ�X���[
		if ( str.find(searchLine) != string::npos ) continue;

		//�擪���甼�p�X�y�[�X�܂ł̕�����ɌW�����|����int�^�Ŏ擾
		x_pos = str.find(searchWord);
		if (x_pos != string::npos){
			x_str = str.substr(0, x_pos);
			x_val = int(stof(x_str) * coefficient);
		}

		//x�̒l�̌�납�甼�p�X�y�[�X�܂ł̕�����ɌW�����|����int�^�Ŏ擾
		y_pos = str.find(searchWord, x_pos + 1);
		if (y_pos != string::npos){
			y_str = str.substr( x_pos + 1 , y_pos );
			y_val = int(stof(y_str) * -coefficient);
		}

		//�擾����[x,y]�̉�f�l�𑝉�������
		pcd_img.data[(pcd_img.rows / 2 + y_val) * pcd_img.cols + x_val + pcd_img.cols / 5] += imgval_increment;

	}

	cout << "complete" << endl;

	//jpg�ŕۑ�
	imwrite(imgName, pcd_img);

	return 0;
}