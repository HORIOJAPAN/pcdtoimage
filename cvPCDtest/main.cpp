#include "urg_unko.h"
using namespace std;

int main()
{
	char z;

	//URG��COM�|�[�g���w��
	int URG_COM[] = {7, 11 };

	//URG�̈ʒu���w��
	float urgPOS[][3] = { 1100.0, 285.0, 0.0,
							20.0, 443.0, 0.0 };

	//Arduino��COM�|�[�g���w��
	int ARDUINO_COM = 0;

	cout << "\n�@�ڑ�����URG�̌��F" << sizeof(URG_COM) / sizeof(URG_COM[0]) << endl << endl;

	getDataUNKO(URG_COM, urgPOS, ARDUINO_COM);

	z = getchar();

	return 0;
}