// Fill out your copyright notice in the Description page of Project Settings.


#include "NGSkateCore/JumpObstacle.h"


AJumpObstacle::AJumpObstacle()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = StaticMeshComp;

}


void AJumpObstacle::BeginPlay()
{
	Super::BeginPlay();
	
}




