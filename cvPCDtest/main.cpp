#include "urg_unko.h"
using namespace std;

int imgWidth, imgHeight, imgResolution;

int main(int argc, char* argv[])
{
	//URG��COM�|�[�g���w��
	int URG_COM[] = { 6, 27 };
	//int URG_COM[] = { 23 };

	//URG�̈ʒu���w��
	//�n�ʂ���̍���,���S����̉�,�c�����̋���,��]�p(���W�A��)
	float urgPOS[][4] = { 685.0, -330.0, 110.0, -1.5708,
		685.0, 340.0, 110.0, 1.5708,
	};

	//float urgPOS[][4] = { 430.0, 0.0, 310.0, 0.0 };

	//Arduino��COM�|�[�g���w��
	int ARDUINO_COM = 10;

	//pcimage�̈���
	imgWidth = 1000;
	imgHeight = 1000;
	imgResolution = 5;

	
	cout << argc << endl;
	for (int i = 0; i < argc; i++)
	{
		cout << argv[i] << endl;
	}
	

	if (argc == 11)
	{
		//URG��COM�|�[�g���w��
		URG_COM[0] = atoi(argv[1]);
		URG_COM[1] = atoi(argv[2]);
		URG_COM[2] = atoi(argv[3]);
		//Arduino��COM�|�[�g���w��
		ARDUINO_COM = atoi(argv[4]);

		//URG�̈ʒu���w��
		urgPOS[0][0] = atof(argv[5]);
		urgPOS[0][1] = atof(argv[6]);
		urgPOS[1][0] = atof(argv[7]);
		urgPOS[1][1] = atof(argv[8]);
		urgPOS[2][0] = atof(argv[9]);
		urgPOS[2][1] = atof(argv[10]);

		//pcimage�̈���
		imgWidth = atoi(argv[11]);
		imgHeight = atoi(argv[12]);
		imgResolution = atoi(argv[13]);

		cout << "csForm����N��" << endl;
	}


	cout << "\n�@�ڑ�����URG�̌��F" << sizeof(URG_COM) / sizeof(URG_COM[0]) << endl << endl;

	getDataUNKO(URG_COM, urgPOS, ARDUINO_COM);

	//z = getchar();

	return 0;
}