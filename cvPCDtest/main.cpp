#include "pcimage.h"
#include <opencv2/nonfree/nonfree.hpp> // SIFT�ESURF���W���[���p

#include "Timer.h"

using namespace std;

void FeatureMatching(
	const std::string& filename1,			// �摜�P�̃t�@�C����
	const std::string& filename2,			// �摜�Q�̃t�@�C����
	const std::string& featureDetectorName,		// detectorType
	const std::string& descriptorExtractorName,	// descriptorExtractorType
	const std::string& descriptorMatcherName,	// descriptorMatcherType
	bool crossCheck = true)				// �}�b�`���O���ʂ��N���X�`�F�b�N���邩�ǂ���
{
	// �摜�̓ǂݍ���
	cv::Mat img1 = cv::imread(filename1);
	cv::Mat img2 = cv::imread(filename2);

	// SIFT�ESURF���W���[���̏�����
	cv::initModule_nonfree();

	// �����_���o
	cv::Ptr<cv::FeatureDetector> detector = cv::FeatureDetector::create(featureDetectorName);
	std::vector<cv::KeyPoint> keypoint1, keypoint2;
	detector->detect(img1, keypoint1);
	detector->detect(img2, keypoint2);

	// �����L�q
	cv::Ptr<cv::DescriptorExtractor> extractor = cv::DescriptorExtractor::create(descriptorExtractorName);
	cv::Mat descriptor1, descriptor2;
	extractor->compute(img1, keypoint1, descriptor1);
	extractor->compute(img2, keypoint2, descriptor2);

	// �}�b�`���O
	cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(descriptorMatcherName);
	std::vector<cv::DMatch> dmatch;
	if (crossCheck)
	{
		// �N���X�`�F�b�N����ꍇ
		std::vector<cv::DMatch> match12, match21;
		matcher->match(descriptor1, descriptor2, match12);
		matcher->match(descriptor2, descriptor1, match21);
		for (size_t i = 0; i < match12.size(); i++)
		{
			cv::DMatch forward = match12[i];
			cv::DMatch backward = match21[forward.trainIdx];
			if (backward.trainIdx == forward.queryIdx)
				dmatch.push_back(forward);
		}
	}
	else
	{
		// �N���X�`�F�b�N���Ȃ��ꍇ
		matcher->match(descriptor1, descriptor2, dmatch);
	}

	// �}�b�`���O���ʂ̕\��
	cv::Mat out;
	cv::drawMatches(img1, keypoint1, img2, keypoint2, dmatch, out);
	cv::imshow("matching", out);
	cv::imwrite("matching.jpg", out);
	while (cv::waitKey(1) == -1);
}





int main()
{
	string fileName = "./pointcloud.pcd";
	string imgName = "pcd.jpg";
	string floorimgName = "pcd_floor.jpg";

	Timer t;
	t.Start(0);
	FeatureMatching("0_0_1.jpg", "0_0_2.jpg", "HARRIS", "SURF","FlannBased");
	t.End(0,Timer::sec);

	char z;

	//save_pcdtoimg(fileName, imgName,1);
	//save_floorimg(imgName, floorimgName);

	//PCIclasstest();

	/*
	cv::Mat image(cv::Size(50, 50), CV_8U, cv::Scalar::all(0));
	for (int i = 0; i < 3; i++) image.data[20 * image.cols + 20] += 100;
	cv::imwrite("testimg.jpg", image);
	*/
	
	z = getchar();

	return 0;
}