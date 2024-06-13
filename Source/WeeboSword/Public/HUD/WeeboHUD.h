#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "WeeboHUD.generated.h"

class UWeeboOverlay;

UCLASS()
class WEEBOSWORD_API AWeeboHUD : public AHUD
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	virtual  void PreInitializeComponents() override;
private:
	UPROPERTY(EditDefaultsOnly, Category = Weebo)
		TSubclassOf<UWeeboOverlay> WeeboOverlayClass;

	UPROPERTY()
		UWeeboOverlay* WeeboOverlay;
public:
	FORCEINLINE UWeeboOverlay* GetWeeboOverlay()  { return WeeboOverlay; }
};