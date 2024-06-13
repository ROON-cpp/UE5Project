// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/WeeboHUD.h"
#include "HUD/WeeboOverlay.h"
void AWeeboHUD::BeginPlay()
{

	Super::BeginPlay();



	
}

void AWeeboHUD::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();

		if (Controller && WeeboOverlayClass)
		{
			WeeboOverlay = CreateWidget <UWeeboOverlay>(Controller, WeeboOverlayClass);
			WeeboOverlay->AddToViewport();

		}
	}
}

