// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TPSCharacter.generated.h"

UCLASS()
class TPSPROJECT_API ATPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* springArmComp;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* tpsCamComp;

	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class USkeletalMeshComponent* gunMeshComp;

	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	class UStaticMeshComponent* sniperGunComp;

	UPROPERTY(EditDefaultsOnly, Category = BulletFactory)
	TSubclassOf<class ABullet> bulletFactory;

	UPROPERTY(EditDefaultsOnly, Category = BulletEffect)
	class UParticleSystem* bulletEffectFactory;

	UPROPERTY(EditDefaultsOnly, Category = SniperUI)
	TSubclassOf<class UUserWidget> sniperUIFactory;

	UPROPERTY()
	class UUserWidget* sniperUI;

	UPROPERTY(EditDefaultsOnly, Category = SniperUI)
	TSubclassOf<class UUserWidget> crosshairUIFactory;

	UPROPERTY()
	class UUserWidget* crosshairUI;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputMappingContext* imcDefault;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputMappingContext* imcMouseLook;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* iaMove;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* iaMouseLook;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* iaJump;

	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* ia_Run;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* iaFire;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* iaGrenadeGun;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* iaRifleGun;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* iaSniper;

	bool bSniperAim = false;

	bool bUsingGrenadeGun = true;

	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float walkSpeed = 200;

	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float runSpeed = 600;

	UPROPERTY(EditDefaultsOnly, Category = CameraMotion)
	TSubclassOf<class UCameraShakeBase> cameraShake;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	class USoundBase* bulletSound;

	FVector direction;

	void Move(const struct FInputActionValue& inputValue);
	void Look(const struct FInputActionValue& inputValue);
	void PlayerMove();
	void InputFire(const struct FInputActionValue& inputValue);
	void ChangeToGrenadeGun(const struct FInputActionValue& inputValue);
	void ChangeToRifleGun(const struct FInputActionValue& inputValue);
	void SniperAim(const struct FInputActionValue& inputValue);
	void InputRun(const struct FInputActionValue& inputValue);
};
