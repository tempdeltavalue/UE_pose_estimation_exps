// Fill out your copyright notice in the Description page of Project Settings.


#include "NNCharacter.h"

#include "PreOpenCVHeaders.h"
#include "OpenCVHelper.h"
#include <ThirdParty/OpenCV/include/opencv2/imgproc.hpp>
#include <ThirdParty/OpenCV/include/opencv2/highgui/highgui.hpp>
#include <ThirdParty/OpenCV/include/opencv2/core.hpp>
#include "PostOpenCVHeaders.h"
#include "Components/PoseableMeshComponent.h"

// Sets default values
ANNCharacter::ANNCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	FString MeshPath = TEXT("C:/Users/tempdelta/Desktop/uelessons/UE_poseEstimation/Content/Characters/Mannequins/Meshes/SKM_Quinn.uasset"); 
	
	if (FPaths::FileExists(MeshPath)) {
		UE_LOG(LogTemp, Warning, TEXT("Path is exists"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Non existence of path"));
	}

	UObject* BuffCharMesh = StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, *MeshPath);
	if (BuffCharMesh == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("BuffCharMesh is null"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("BuffCharMesh is inited properly"));
	}

	USkeletalMesh* CharMesh = Cast<USkeletalMesh>(BuffCharMesh);
	if (CharMesh == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("Char mesh is null"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Char mesh is inited properly"));
	}


	PoseEstimationNN = NewObject<UPoseEstNN>();
	USkeletalMeshComponent* SkeletalMeshCompt = GetMesh();
	if (SkeletalMeshCompt != NULL) {
		UE_LOG(LogTemp, Warning, TEXT("horray!"));

		PoseableMeshComp = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("PoseableMeshComponent"));

		PoseableMeshComp->AttachToComponent(SkeletalMeshCompt, FAttachmentTransformRules::KeepRelativeTransform);
		PoseableMeshComp->SetSkeletalMesh(SkeletalMeshCompt->SkeletalMesh);
		PoseableMeshComp->SetWorldLocation(SkeletalMeshCompt->GetComponentLocation());
		PoseableMeshComp->SetWorldRotation(SkeletalMeshCompt->GetComponentRotation());
	}
}

// Called when the game starts or when spawned
void ANNCharacter::BeginPlay()
{
	Super::BeginPlay();


		if (PoseableMeshComp) {
			UE_LOG(LogTemp, Warning, TEXT("Pose mesh inited"));


			FName boneName = "spine_01";
			//UAnimInstance* animInstance = SkeletalMeshCompt2->GetAnimInstance();

			FTransform boneTransform = PoseableMeshComp->GetBoneTransformByName(boneName, EBoneSpaces::ComponentSpace);
			boneTransform.SetRotation(FQuat::MakeFromEuler(FVector(45.f, 45.f, 45.f)));
			PoseableMeshComp->SetBoneTransformByName(boneName, boneTransform, EBoneSpaces::ComponentSpace);
			//PoseableMeshComponent->UpdateChildTransforms();
			//animInstance->UpdateAnimation(0.f, false);
	}

	UE_LOG(LogTemp, Warning, TEXT("NN char begin play"));

	// ABSOLUTE PATHS BE CAREFUL !
	cv::Mat TestImage = cv::imread("C:/Users/tempdelta/Desktop/uelessons/UE_poseEstimation/Content/Models/Cristo_crucificado.png", cv::IMREAD_COLOR);
	cv::Mat ResImage = PoseEstimationNN->URunModel(TestImage);
	cv::imwrite("C:/Users/tempdelta/Desktop/uelessons/UE_poseEstimation/Content/Models/output_image.png", ResImage); // Save the modified image

	UE_LOG(LogTemp, Warning, TEXT("After preds"));
}

// Called every frame
void ANNCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANNCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

