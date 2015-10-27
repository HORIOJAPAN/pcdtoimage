#include "urg_unko.h"
using namespace std;

int imgWidth, imgHeight, imgResolution;

int main(int argc, char* argv[])
{
	//URG��COM�|�[�g���w��
	int URG_COM[] = { 19, 20 };

	//URG�̈ʒu���w��
	float urgPOS[][3] = { 1100.0, 285.0, 0.0,
		20.0, 443.0, 0.0 };

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
		//Arduino��COM�|�[�g���w��
		ARDUINO_COM = atoi(argv[3]);

		//URG�̈ʒu���w��
		urgPOS[0][0] = atof(argv[4]);
		urgPOS[0][1] = atof(argv[5]);
		urgPOS[1][0] = atof(argv[6]);
		urgPOS[1][1] = atof(argv[7]);

		//pcimage�̈���
		imgWidth = atoi(argv[8]);
		imgHeight = atoi(argv[9]);
		imgResolution = atoi(argv[10]);

		cout << "csForm����N��" << endl;
	}


	cout << "\n�@�ڑ�����URG�̌��F" << sizeof(URG_COM) / sizeof(URG_COM[0]) << endl << endl;

	getDataUNKO(URG_COM, urgPOS, ARDUINO_COM);

	//z = getchar();

	return 0;
}