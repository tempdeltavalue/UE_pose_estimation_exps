// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ControlRig.h"
#include "PoseControlRig.generated.h"

/**
 * 
 */
UCLASS()
class UE_POSEESTIMATION_API UPoseControlRig : public UControlRig
{
	GENERATED_BODY()

public:
		UPROPERTY(EditDefaultsOnly, BluePrintReadOnly)
		float PoseX;

};
