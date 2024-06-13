// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Characters/CharacterTypes.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/PickupInterface.h"
#include "WeeboCharacter.generated.h"

class USphereComponent;
class UCharacterMovementComponent;
class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class UAnimMontage;
class ATwoHandedWeapon;
class AEnemy;
class UWeeboOverlay;
class ASoul;
class ATreasure;

UCLASS()
class WEEBOSWORD_API AWeeboCharacter : public ABaseCharacter ,public IPickupInterface
{
	GENERATED_BODY()

public:
	AWeeboCharacter();
	virtual void Tick(float DeltaTime)override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void SetOverlappingItem( AItem* Item) override;
	virtual void AddSoul(ASoul* Soul)override;
	virtual void AddGold(class ATreasure* Treasure)override;



	/// </My Shit>
	
	UFUNCTION(BlueprintCallable)
	FVector GetWTranslation();

	UFUNCTION(BlueprintCallable)
	FVector GetWRotation();

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void CallingMotionWarping();

	TArray <AActor*> AllOverlappedActors;




	void UpdateEnemyLocaiton();

	bool InTargetRange(AActor* Target, double Radius);

	UFUNCTION(BlueprintCallable)
	bool CheckMotionWarpingRange();

	double WarpingRadius = 400.f;

	UPROPERTY(VisibleAnyWhere)
	USphereComponent* RangeSphere;

	UPROPERTY(BlueprintReadWrite)
	FVector EnemyLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Select the Second Character");
	class AActor* EnemyChar;


protected:

	virtual void BeginPlay() override;
	/** Callbacks for input **/
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EKeyPressed();
	void ShiftKeyPressed();
	void ShiftKeyReleased();
	/** Combat **/
	void EquipWeapon(AWeapon* Weapon);
	virtual void Attack() override;
	virtual void AttackEnd()override;
	virtual bool CanAttack() override;
	virtual void DodgeEnd()override;
	void PlayEquipMontage(const FName& SectionName);
	virtual void Die_Implementation() override;
	bool CanDisarm();
	bool CanArm();
	void Disarm();
	void Arm();
	bool HasEnoughStamina();
	bool IsOccupied();
	void Dodge();
	
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToThigh();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();
	
	UFUNCTION(BlueprintCallable)
	void HitReactEnd();



	
	
	/// <summary>
	/// My shit
	/// </summary>
	/// 
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Select the Second Character");
	AEnemy* EnemyCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Select Your Character");
	AWeeboCharacter* WeeboCharacter;

private:

	bool IsUnoccupied();
	void InitializeWeeboOverlay();
	void SetHUDHealth();



	/** Character Components **/

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;


	UWeeboOverlay* WeeboOverlay;

public:

	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }

	//Testign 
	TArray<AActor*> EnemyCharacters;

	float NearestDistanceSquared = MAX_FLT;

	void CheckNearestEnemy();
	
};