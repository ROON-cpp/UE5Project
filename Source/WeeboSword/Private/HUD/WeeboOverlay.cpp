// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/WeeboOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UWeeboOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void UWeeboOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
	
}

void UWeeboOverlay::SetGold(int32 Gold)
{
	if (GoldText)
	{
		const FString String = FString::Printf(TEXT("%d"), Gold);
		const FText FText = FText::FromString(String);
		GoldText->SetText(FText);
	}
}

void UWeeboOverlay::SetSouls(int32 Souls)
{
	if (SoulsText)
	{
		const FString String = FString::Printf(TEXT("%d"), Souls);
		const FText FText = FText::FromString(String);
		SoulsText->SetText(FText);
	}
}
