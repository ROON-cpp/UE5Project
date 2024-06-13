#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class UPawnSensingComponent;
class ASoul;
UCLASS()
class WEEBOSWORD_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()



public:

	AEnemy();
	/* <AActor>  */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	/* <AActor>  */

	/* <IHitInterface>  */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	/* <IHitInterface>  */


	

protected:

	/* <AActor>  */
	virtual void BeginPlay() override;
	/* <AActor>  */

	/* <BaseCharacter>  */
	virtual void Die_Implementation() override;
	void SpawnSoul();
	virtual void Attack()override;
	virtual bool CanAttack() override;
	virtual void AttackEnd() override;
	virtual void HandleDamage(float DamageAmount) override;

	/* <BaseCharacter>  */

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;
	/// <summary>
	/// testing
	/// </summary>

	UFUNCTION(BlueprintCallable)
	bool CheckMotionWarpingRange();

	USkeletalMeshComponent* SkeletalMesh;
private:

	/* AI Behavior  */
	void InitializeEnemy();
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void PatrolTimerFinished();
	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsAttacking();
	
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsEngaged();
	void ClearPatrolTimer();
	void StartAttackTimer();
	void ClearAttackTimer();
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();
	void SpawnDefaultWeapon();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn); // callback for OnPawnSeen in UPawnSensing Component

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, Category = Combat)
	double CombatRadius = 1000.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	double AttackRadius = 200.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	double AcceptanceRadius = 50;

	/// <summary>
	/// testing
	/// </summary>
	UPROPERTY(EditAnywhere)
	double MotionWarpingRadius = 250.f;


	//  Navigation   //

	UPROPERTY()
	class AAIController* EnemyController;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*>PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 10.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float PatrollingSpeed = 125.f;

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMin = 0.5f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMax = 1.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ChasingSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float DeathLifeSpan = 8.f;


	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<class ASoul> SoulClass;

public:

	float EnemyMovementSpeed;

	FTimerHandle SpawnDelayTimerHandle;

	bool IsDead();
};