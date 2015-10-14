#include "../cvPCDtest/pcimage.h"

#include <fstream>
#include <iostream>
#include <filesystem> 
#include <algorithm>
#include <vector>

using namespace std;

#define WRITELINE

const string DIRPATH = "C:\\Users\\user\\Documents\\�Ȃ��ނ�\\���΃`�������W2015\\����f�[�^\\20151014224236";

PCImage pcimage(5000, 5000, 5);

void getAllFileName(vector<string>& fileNames)
{
	namespace sys = std::tr2::sys;
	sys::path p(DIRPATH); // �񋓂̋N�_
	ifstream ifs;
	string fname;

	for_each(sys::directory_iterator(p), sys::directory_iterator(),
		[&](const sys::path& p) {
		if (sys::is_regular_file(p)) { // �t�@�C���Ȃ�...
			cout << p.filename() << endl;
			fname = p.filename();
			if (fname.find("pcd") != string::npos)	fileNames.push_back(p.filename());
		}
		else if (sys::is_directory(p)) { // �f�B���N�g���Ȃ�...
			cout << "dir.: " << p.string() << endl;
		}
	});
}

void makePCImageFromPCD(string filename )
{
	float	x_cood, y_cood;
	float	x_pos, y_pos;

	string str, data_str;
	string searchWord(",");
	string::size_type begin, end;

	//pcd�t�@�C����ǂݍ���
	ifstream ifs(DIRPATH + "\\" + filename);
	if (ifs.fail())
	{
		cerr << "False" << endl;
		return ;
	}

	//�w�b�_�������Ƃ΂����߂̃��[�v
	for (int i = 0; i <= 11; i++){
		getline(ifs, str);
	}

	while (getline(ifs, str))
	{
		//x
		begin = str.find(searchWord);
		if (begin != string::npos){
			data_str = str.substr(0, begin);
			x_cood = stof(data_str);

			//cout << x_cood << endl;
		}

		//y
		end = str.find(searchWord, begin + 1);
		if (end != string::npos){
			data_str = str.substr(begin + 1, end);
			y_cood = stof(data_str);

			//cout << y_cood << endl;
		}

		//x_pos
		begin = end;
		end = str.find(searchWord, begin + 1);
		if (end != string::npos){
			data_str = str.substr(begin + 1, end);
			x_pos = stof(data_str);

			//cout << x_pos << endl;
		}

		//y_pos
		begin = end;
		end = str.find(searchWord, begin + 1);
		if (end != string::npos){
			data_str = str.substr(begin + 1, end);
			y_pos = stof(data_str);

			//cout << y_pos << endl;
		}

		//�擾����[x,y]�̉�f�l�𑝉�������
#ifdef WRITELINE
		pcimage.writePoint(x_cood, y_cood,x_pos/1000,y_pos/1000);
#else
		pcimage.writePoint(x_cood,y_cood);
#endif

	}

	//cout << "complete" << endl;
}

void main()
{
	vector<string> allFileNames;

	getAllFileName(allFileNames);

	for (auto& name : allFileNames)
	{
		makePCImageFromPCD(name);
	}
	pcimage.savePCImage();
}