// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "BaseCharacter.generated.h"
class UAnimMontage;
class AWeapon;
class UAttributeComponent;
UCLASS()
class WEEBOSWORD_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:

	ABaseCharacter();

	virtual void Tick(float DeltaTime) override;
	
	
protected:

	virtual void BeginPlay() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void Attack();
	UFUNCTION(BlueprintNativeEvent)
	void Die();
	virtual void HandleDamage(float DamageAmount);
	virtual bool CanAttack();
	virtual bool IsAlive();
	void DisableMeshCollision();
	void DisableCapsule();
	void DirectionalHitReact(const FVector& ImpactPoint);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	void PlayHitReactMontage(const FName& SectionName);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	virtual void PlayDodgeMontage();
	void StopAttackMontage();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();
	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd();

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte <EDeathPose> DeathPose;
	
	UPROPERTY(BlueprintReadOnly)
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = Combat)
	double WrapTargetDistance = 75.f;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* Attributes;


private:

	void PlayMontagSection(UAnimMontage* Montage, const FName& SectionName);
	virtual int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);

	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* HitParticles;

	/**
	 Animation montages
	**/

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	TArray<FName> AttackMontageSections;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	TArray<FName> DeathMontageSections;

public:
	FORCEINLINE TEnumAsByte <EDeathPose> GetDeathPose() const { return DeathPose; }
};
