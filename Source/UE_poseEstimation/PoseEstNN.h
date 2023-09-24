// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NeuralNetwork.h"

#include "PreOpenCVHeaders.h"
#include "OpenCVHelper.h"
#include <ThirdParty/OpenCV/include/opencv2/imgproc.hpp>
#include <ThirdParty/OpenCV/include/opencv2/highgui/highgui.hpp>
#include <ThirdParty/OpenCV/include/opencv2/core.hpp>
#include "PostOpenCVHeaders.h"


#include "PoseEstNN.generated.h"

/**
 * 
 */


UCLASS()
class UE_POSEESTIMATION_API UPoseEstNN : public UNeuralNetwork
{
	GENERATED_BODY()

public:

	UPROPERTY(Transient)
		UNeuralNetwork* Network = nullptr;
	UPoseEstNN();
	cv::Mat URunModel(cv::Mat image);
	TArray<float> UPreProcessImage2(cv::Mat image);

	TArray<float> UPreProcessImage(cv::Mat image);


};