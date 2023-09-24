// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "PoseEstNN.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "NNCharacter.generated.h"

UCLASS()
class UE_POSEESTIMATION_API ANNCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANNCharacter();

	//UMeshComponent* CharMesh;

	//BP skeletal mesh component
	//UPROPERTY(VisibleAnywhere)
	//USkeletalMeshComponent* SkeletalMeshCompt;

	UPROPERTY(VisibleAnywhere)
	UPoseableMeshComponent* PoseableMeshComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:
	UPoseEstNN* PoseEstimationNN;

};
