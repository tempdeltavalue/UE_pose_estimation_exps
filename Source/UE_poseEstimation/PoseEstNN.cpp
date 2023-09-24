#include "PoseEstNN.h"

#include <fstream>
#include <iostream>
#include <array>
#include <string>
#include <filesystem>


#include "PreOpenCVHeaders.h"
#include "OpenCVHelper.h"
#include <ThirdParty/OpenCV/include/opencv2/imgproc.hpp>
#include <ThirdParty/OpenCV/include/opencv2/highgui/highgui.hpp>
#include <ThirdParty/OpenCV/include/opencv2/core.hpp>
#include "PostOpenCVHeaders.h"

//#include "MediaBundle.h"
using namespace std;
using namespace cv;


UPoseEstNN::UPoseEstNN()
{
	Network = nullptr;
}

/// https://stackoverflow.com/questions/75754985/how-to-load-an-image-as-an-input-to-an-onnx-model-in-c-using-directml-provider

cv::Mat UPoseEstNN::URunModel(cv::Mat image)
{
	// Create result array
	TArray<FColor> results;

	// Create Network object if null
	if (Network == nullptr) {
		//Running inference
		Network = NewObject<UNeuralNetwork>((UObject*)GetTransientPackage(), UNeuralNetwork::StaticClass());
		//create array of the correct pixel values from results


		// Load model from file.
		const FString& ONNXModelFilePath = TEXT("C:/Users/tempdelta/Desktop/uelessons/UE_poseEstimation/Content/Models/s_pose_estimation.onnx");
		// Set Device
		Network->SetDeviceType(ENeuralDeviceType::CPU);
		// Check that the network was successfully loaded
		if (Network->Load(ONNXModelFilePath))
		{
			UE_LOG(LogTemp, Log, TEXT("Neural Network loaded successfully."))
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UNeuralNetwork could not loaded from %s."), *ONNXModelFilePath);
			//return results;
		}
	}

	//// Fill input neural tensor
	const TArray<float> InArray = UPreProcessImage2(image); // Assumed initialized with data and that InArray.Num() == Network->Num()
	Network->SetInputFromArrayCopy(InArray); // Equivalent: Network->SetInputFromVoidPointerCopy(InArray.GetData());
	//UE_LOG(LogTemp, Display, TEXT("Input tensor: %s."), *Network->GetInputTensor().ToString());
	//// Run UNeuralNetwork
	Network->Run();
	//UE_LOG(LogTemp, Log, TEXT("Neural Network Inference complete."))
	//	//Read and print OutputTensor
	//	//const FNeuralTensor& OutputTensor = Network->GetOutputTensor();
	TArray<float> OutputTensor = Network->GetOutputTensor().GetArrayCopy<float>();
	UE_LOG(LogTemp, Warning, TEXT("Output tensor len: %d ."), OutputTensor.Num());

	//for (int i = 0; i < OutputTensor.Num(); i += 3) {
		//UE_LOG(LogTemp, Display, TEXT("Output tensor item: %f ."), OutputTensor[i]);
	//}



	UE_LOG(LogTemp, Display, TEXT("Output tensor slicing"));
	int step = 64 * 64;
	std::vector<std::vector<float>> Keypoints; // Assuming you have Keypoints filled

	for (int i = 0; i < OutputTensor.Num(); i += step) {
		TArray<float> SlicedArray = TArray<float>(OutputTensor.GetData() + i, step);

		float MaxValue = 0.0;
		int MaxIndex = 0;
		//auto MaxValue = std::max_element(SlicedArray.GetData(), SlicedArray.GetData() + SlicedArray.Num());
		//int32 MaxIndex = MaxElementIt - SlicedArray.GetData();

		for (int32 j = 1; j < SlicedArray.Num(); ++j)
		{
			if (SlicedArray[j] > MaxValue)
			{
				MaxValue = SlicedArray[j];
				MaxIndex = j;
			}
		}

		UE_LOG(LogTemp, Display, TEXT("sl array num %d ."), SlicedArray.Num());
		UE_LOG(LogTemp, Display, TEXT("sl max %f index %d ."), MaxValue, MaxIndex);

		int Row = MaxIndex % 64;
		int Column = MaxIndex / 64;

		std::vector<float> keypoint = { MaxValue, static_cast<float>(Row), static_cast<float>(Column) };

		Keypoints.push_back(keypoint);
	}


	std::vector<bool> is_joint_plotted(Keypoints.size(), false);
	const float THRESHOLD = 0.8;
	const int OUT_SHAPE_WIDTH = 64;
	const int OUT_SHAPE_HEIGHT = 64;
	std::vector<std::vector<int>> POSE_PAIRS2 = {
		{9, 8} , // upper body
		{8, 7},
		{7, 6},
		
		{6, 2} , // lower body
		{2, 1},
		{1, 0},

		{6, 3},
		{3, 4},
		{4, 5},


		{7, 12}, // arms
		{12, 11},
		{11, 10},
		{7, 13},
		{13, 14},
		{14, 15}
	};

	int IMG_HEIGHT = image.rows;
	int IMG_WIDTH = image.cols;

	for (const auto& pose_pair : POSE_PAIRS2) {
		int from_j = pose_pair[0];
		int to_j = pose_pair[1];
		std::cout << "\n";
		std::cout << from_j << " " << to_j << std::endl;

		float from_thr = Keypoints[from_j][0]; // The first value in the inner vector
		int from_x_j = static_cast<int>(Keypoints[from_j][1]); // The second value in the inner vector
		int from_y_j = static_cast<int>(Keypoints[from_j][2]); // The third value in the inner vector

		float to_thr = Keypoints[to_j][0]; // The first value in the inner vector
		int to_x_j = static_cast<int>(Keypoints[to_j][1]); // The second value in the inner vector
		int to_y_j = static_cast<int>(Keypoints[to_j][2]); // The third value in the inner vector

		from_x_j = from_x_j * IMG_WIDTH / OUT_SHAPE_WIDTH;
		to_x_j = to_x_j * IMG_WIDTH / OUT_SHAPE_WIDTH;
		from_y_j = from_y_j * IMG_HEIGHT / OUT_SHAPE_HEIGHT;
		to_y_j = to_y_j * IMG_HEIGHT / OUT_SHAPE_HEIGHT;

		UE_LOG(LogTemp, Warning, TEXT("from_x_j %d %s"), from_x_j, is_joint_plotted[from_j] ? "true" : "false");
		UE_LOG(LogTemp, Warning, TEXT("to_x_j %d %s"), to_x_j, is_joint_plotted[to_j] ? "true" : "false");
		UE_LOG(LogTemp, Warning, TEXT("from_thr %f to_thr %f"), from_thr, to_thr);

		if (from_thr > THRESHOLD && !is_joint_plotted[from_j]) {
			UE_LOG(LogTemp, Warning, TEXT("HERE"));
			cv::ellipse(image, cv::Point(from_x_j, from_y_j), cv::Size(4, 4), 0, 0, 360, cv::Scalar(255, 255, 255), cv::FILLED);
			is_joint_plotted[from_j] = true;
		}

		if (to_thr > THRESHOLD && !is_joint_plotted[to_j]) {
			UE_LOG(LogTemp, Warning, TEXT("HERE2"));
			cv::ellipse(image, cv::Point(to_x_j, to_y_j), cv::Size(4, 4), 0, 0, 360, cv::Scalar(255, 255, 255), cv::FILLED);
			is_joint_plotted[to_j] = true;
		}

		if (from_thr > THRESHOLD && to_thr > THRESHOLD) {
			UE_LOG(LogTemp, Warning, TEXT("HERE3"));
			cv::line(image, cv::Point(from_x_j, from_y_j), cv::Point(to_x_j, to_y_j), cv::Scalar(255, 74, 0), 3);
		}
	}

	cv::Mat modifiedImage = image.clone(); // Create a copy of the original image
	return modifiedImage;
}




TArray<float> UPoseEstNN::UPreProcessImage2(cv::Mat image)
{

	if (image.empty()) {
		return {};
	}


	// resize
	resize(image, image, Size(256, 256));

	// reshape to 1D
	//cv::Mat output;
	//cv::normalize(image, image, 0, 1, cv::NORM_MINMAX);
	//image.convertTo(image, CV_32F, 1.0 / 255.0, 0);//(1)
	Mat normImg;
	cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
	image = image.reshape(1, 1);

	vector<float> vec;
	image.convertTo(vec, CV_32FC1, 1. / 255);

	//TArray<float> result;

	//for (int row = 0; row < normImg.rows; ++row) {
	//	for (int col = 0; col < normImg.cols; ++col) {
	//		Vec3f &pixel = normImg.at<Vec3f>(row, col);  // For a 3-channel image (BGR)
	//		float p0 = ((static_cast<float>(pixel[0]) / 255.0) - 0.485) / 0.226; // blue channel
	//		float p1 = ((static_cast<float>(pixel[1]) / 255.0) - 0.456) / 0.224; // green channel
	//		float p2 = ((static_cast<float>(pixel[2]) / 255.0) - 0.406) / 0.225; // red channel

	//		result.Add(p0);
	//		result.Add(p1);
	//		result.Add(p2);

	//		//UE_LOG(LogTemp, Display, TEXT("after norm pixel: %f  %f   %f ."), pixel[0], pixel[1], pixel[2]);
	//	}
	//}

	// https://github.com/cassiebreviu/unreal-onnxruntime-nni-example/blob/main/MyNeuralNetwork.cpp
	// HWC -> CHW
	TArray<float> output;

	std::vector<float> means = { 0.485 , 0.456 ,0.406 };
	std::vector<float> stds = { 0.226 , 0.224 , 0.225 };

	for (size_t ch = 0; ch < 3; ++ch) {
		for (size_t i = ch; i < vec.size(); i += 3) {
			output.Emplace((vec[i] - means[ch]) / stds[ch]);
		}
	}

	return output;
}