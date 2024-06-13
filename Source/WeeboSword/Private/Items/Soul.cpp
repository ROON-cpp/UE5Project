// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Soul.h"
#include "Interfaces/PickupInterface.h"
#include "Kismet/KismetSystemLibrary.h" 
#include "Characters/WeeboCharacter.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
void ASoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//WeeboLocation->GetActorLocation();
	//const double LocationZ = GetActorLocation().Z;
	//if (this->GetActorLocation() != WeeboLocation->GetActorLocation())
	//{
	//	const FVector DeltaLocation = FVector(0.f, 0.f, DriftRate * DeltaTime);
	//	AddActorWorldOffset(DeltaLocation);
		
	//}
		// Create a timer handle

	SoulMovingToWeebo();

	
}
void ASoul::BeginPlay()
{
	Super::BeginPlay();
	const FVector Start = GetActorLocation();
	const FVector End = Start - FVector(0.f,0.f,2000.f);
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectType;
	ObjectType.Add(EObjectTypeQuery::ObjectTypeQuery1);
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	FHitResult HitResult;
	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		Start,
		End,
		ObjectType,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true
	);
	
	//DesiredZ = HitResult.ImpactPoint.Z + 50.f;
}
void ASoul::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->AddSoul(this);
		SpawnPickupSystem();
		SpawnPickupSound();

		Destroy();

	}
	
}

void ASoul::SoulMovingToWeebo()
{

	UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	WeeboLocation = Cast<AWeeboCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (WeeboLocation)
	{
		FVector Direction = WeeboLocation->GetActorLocation() - GetActorLocation();

		Direction.Normalize();

		FVector NewLocation = GetActorLocation() + Direction * 100.f * GetWorld()->GetDeltaSeconds();
		SetActorLocation(NewLocation);
	}
}
