
#include "Characters/WeeboCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Items/Weapons/TwoHandedWeapon.h"
#include "Enemy/Enemy.h"
#include "WeeboSword/DebugMacros.h"
#include "HUD/WeeboHUD.h"
#include "HUD/WeeboOverlay.h"
#include "Components/AttributeComponent.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "MotionWarpingComponent.h" 

AWeeboCharacter::AWeeboCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");


	RangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RangeSphere->SetupAttachment(GetRootComponent());

	

}

void AWeeboCharacter::Tick(float DeltaTime)
{
	if (Attributes && WeeboOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		WeeboOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}

}

void AWeeboCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AWeeboCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AWeeboCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &AWeeboCharacter::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &AWeeboCharacter::LookUp);


	PlayerInputComponent->BindAction(FName("Sprint"), IE_Pressed, this, &AWeeboCharacter::ShiftKeyPressed);
	PlayerInputComponent->BindAction(FName("Sprint"), IE_Released, this, &AWeeboCharacter::ShiftKeyReleased);
	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &AWeeboCharacter::Jump);
	PlayerInputComponent->BindAction(FName("Equip"), IE_Pressed, this, &AWeeboCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &AWeeboCharacter::Attack);
	PlayerInputComponent->BindAction(FName("Dodge"), IE_Pressed, this, &AWeeboCharacter::Dodge);
}

void AWeeboCharacter::Jump()
{
	if (IsUnoccupied())
	{
		Super::Jump();
	}
}

float AWeeboCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);

	SetHUDHealth();
	
	return DamageAmount;
}

void AWeeboCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint,Hitter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
	
}

void AWeeboCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void AWeeboCharacter::AddSoul(ASoul* Soul)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlapping"));
	if (Attributes && WeeboOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		WeeboOverlay->SetSouls(Attributes->GetSouls());
	}

}

void AWeeboCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && WeeboOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		WeeboOverlay->SetGold(Attributes->GetGold());

	}
}

void AWeeboCharacter::UpdateEnemyLocaiton()
{
	GetWTranslation();
	GetWRotation();
}

bool AWeeboCharacter::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

bool AWeeboCharacter::CheckMotionWarpingRange()
{
	return InTargetRange(EnemyChar, WarpingRadius);
}

void AWeeboCharacter::BeginPlay()
{
	Super::BeginPlay();
	MovementSpeed = GetCharacterMovement()->GetMaxSpeed();

	Tags.Add(FName("EngageableTarget"));
	
	InitializeWeeboOverlay();
	RangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeeboCharacter::OnSphereOverlap);
}

void AWeeboCharacter::MoveForward(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if (Controller && (Value != 0.f))
	{
		// find out which way is forward
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}

}

void AWeeboCharacter::MoveRight(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if (Controller && (Value != 0.f))
	{
		// find out which way is right
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AWeeboCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AWeeboCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AWeeboCharacter::EKeyPressed()
{

	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		//void CallingEB();
		if (EquippedWeapon)
		{
			EquippedWeapon->Destroy();
		}
		EquipWeapon(OverlappingWeapon);
	}

	else 
	{
		if (EquippedWeapon)
		{

			if (CanDisarm())
			{
				Disarm();
			}
			else if (CanArm())
			{
				Arm();
			}

		}

		
	}
	
	
}

void AWeeboCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("WeaponSocket"), this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
}

void AWeeboCharacter::Attack()
{
	Super::Attack();
	if (CanAttack())
	{

		CheckNearestEnemy();
		if (EnemyChar != nullptr)
		{
			
			UE_LOG(LogTemp, Warning, TEXT("Enemy Location: %s"), *EnemyChar->GetActorLocation().ToString());
			UpdateEnemyLocaiton();
		}

		CallingMotionWarping();

		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void AWeeboCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool AWeeboCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool AWeeboCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool AWeeboCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped 
;
}

void AWeeboCharacter::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void AWeeboCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void AWeeboCharacter::Dodge()
{
	if (IsOccupied() || !HasEnoughStamina()) return;
	
	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;
	if (Attributes && WeeboOverlay)
	{
		//void CallingRMB();
		Attributes->UseStamina(Attributes->GetDodgeCost());
		WeeboOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

bool AWeeboCharacter::HasEnoughStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();
}

bool AWeeboCharacter::IsOccupied()
{
	return ActionState != EActionState::EAS_Unoccupied;
}

void AWeeboCharacter::DodgeEnd()
{
	Super::DodgeEnd();

	ActionState = EActionState::EAS_Unoccupied;
}

void AWeeboCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void AWeeboCharacter::Die_Implementation()
{
	Super::Die_Implementation();

	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
}

void AWeeboCharacter::AttachWeaponToThigh()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
	
}

void AWeeboCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("WeaponSocket"));
	}
	
}

void AWeeboCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AWeeboCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

FVector AWeeboCharacter::GetWTranslation()
{

	if (EnemyChar == nullptr && EnemyChar->ActorHasTag("Dead")) return FVector();
	
	const FVector CombatTargetLocation = EnemyChar->GetActorLocation();
	const FVector Location = GetActorLocation();
	FVector TargetToEnemy = (Location - CombatTargetLocation).GetSafeNormal();
	TargetToEnemy *= WrapTargetDistance;
	
	return CombatTargetLocation + TargetToEnemy;
	
}

FVector AWeeboCharacter::GetWRotation()
{
	if (EnemyChar == nullptr )return FVector();
	return EnemyChar->GetActorLocation();
}

void AWeeboCharacter::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (OtherActor->ActorHasTag(TEXT("Enemy")))
	{
		EnemyCharacters.AddUnique(OtherActor);
	}
}

void AWeeboCharacter::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == EnemyChar && EnemyChar)
	{
		EnemyChar = nullptr;
		EnemyCharacters.Remove(OtherActor);
	}
}

bool AWeeboCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void AWeeboCharacter::InitializeWeeboOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("1"));
		AWeeboHUD* WeeboHUD = Cast<AWeeboHUD>(PlayerController->GetHUD());

		if (WeeboHUD)
		{
			UE_LOG(LogTemp, Warning, TEXT("2"));
			WeeboOverlay = WeeboHUD->GetWeeboOverlay();

			if (WeeboOverlay && Attributes)
			{
				UE_LOG(LogTemp, Warning, TEXT("3"));
				WeeboOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				WeeboOverlay->SetStaminaBarPercent(1.f);
				WeeboOverlay->SetGold(0);
				WeeboOverlay->SetSouls(0);
			}
		}
	}
}

void AWeeboCharacter::SetHUDHealth()
{
	if (WeeboOverlay && Attributes)
	{
		WeeboOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void AWeeboCharacter::CheckNearestEnemy()
{
	for (AActor* Enemy : EnemyCharacters)
	{
		if (Enemy->ActorHasTag("Dead"))
		{
			EnemyCharacters.Remove(Enemy);
			return;
		}
		double DistanceSquared = (Enemy->GetActorLocation() - GetActorLocation()).Size();

		if (DistanceSquared < NearestDistanceSquared)
		{
			NearestDistanceSquared = DistanceSquared;
			EnemyChar = Enemy;
		}

	}
}

void AWeeboCharacter::ShiftKeyPressed()
{

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed *= 3;
	}

}

void AWeeboCharacter::ShiftKeyReleased()
{


	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	}

}
