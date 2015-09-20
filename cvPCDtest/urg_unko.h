#ifndef _INC_URG_UNKO
#define _INC_URG_UNKO

#include "urg_sensor.h"
#include "urg_utils.h"
#include "urg_open.h"
#include "pcimage.h"

#include <Windows.h>
#include <fstream>

#define URGCOUNT 2

//指定したCOMポートを閉じる
int CommClose(HANDLE hComm);
//Arduinoのハンドルを取得
void getArduinoHandle(HANDLE& hComm);
//urg_unkoのmainループ
int getDataUNKO(int URG_COM[], int ARDUINO_COM );

/*
*
*　URGでデータを取得してマッピングを行うクラス
*
*/
class urg_unko{
private:
	/*
	*	メンバ変数
	*/
	int COMport;	//URGのCOMポート

	std::ofstream ofs;	//ファイルストリームオブジェクト．pcdファイル作成に用いる
	int pcdnum;			//pcdファイルの番号をカウントする変数
	int pcdcount;		//pcdファイルに書き込む点の数をカウントする変数

	float urgpos[3];	//NCWCの回転中心から見たURGの位置．センサの地面からの高さ，センサの基準位置からの距離，および水平面からの俯角
	float startpos[2];	//測定開始位置から見た現在の位置
	float startpos_old[2] ;

	urg_t urg;			//URGオブジェクト
	long *data = NULL;	
	long time_stamp;

	PCImage pcimage;	//マップ画像作成用クラス

	/*
	*	privateなメソッド
	*/

	//URGとの接続を切断
	int disconnectURG();
	//URGと接続
	int connectURG();

	void set_3D_surface(int data_n);

	//pcdファイルを作成して書き込む準備を行う
	void pcdinit();
	//pcdファイルへ点を書き込む
	void pcdWrite(float x, float y);
	//pcdファイルへの書き込みを終了して保存する
	void pcdSave();

public:
	/*
	*	publicなメソッド
	*/

	//コンストラクタ
	urg_unko();
	//デストラクタ
	~urg_unko();

	//自身の初期化処理を行う
	void init(int COM , float pos[]);
	//URGからデータを取得するメソッド
	int getData4URG(float& dist, float& rad);

};


#endif