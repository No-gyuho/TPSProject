// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Bullet.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyFSM.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterAnim.h"
// Sets default values
ATPSCharacter::ATPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*ConstructorHelpers::FObjectFinder<USkeletalMesh>
		TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple"));
	if (TempMesh.Succeeded()) {
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}*/

	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));
	springArmComp->TargetArmLength = 400;
	springArmComp->bUsePawnControlRotation = true;

	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));
	tpsCamComp->SetupAttachment(springArmComp);
	tpsCamComp->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	JumpMaxCount = 2;

	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	//gunMeshComp->SetupAttachment(GetMesh());
	gunMeshComp->SetupAttachment(GetMesh(), TEXT("HandGrip_R"));
	/*ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Weapons/GrenadeLauncher/Meshes/SKM_GrenadeLauncher.SKM_GrenadeLauncher'"));
	if (TempGunMesh.Succeeded()) {
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		gunMeshComp->SetRelativeLocation(FVector(-14, 52, 120));
	}*/

	sniperGunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SniperGunComp"));
	//sniperGunComp->SetupAttachment(GetMesh());
	sniperGunComp->SetupAttachment(GetMesh(), TEXT("HandGrip_R"));
	/*ConstructorHelpers::FObjectFinder<UStaticMesh>
		TempSniperMesh(TEXT("/Script/Engine.StaticMesh'/Game/Weapons/Rifle/Meshes/SM_Rifle.SM_Rifle'"));
	if (TempSniperMesh.Succeeded()) {
		sniperGunComp->SetStaticMesh(TempSniperMesh.Object);
		sniperGunComp->SetRelativeLocation(FVector(-22, 55, 120));
	}*/
}

// Called when the game starts or when spawned
void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;

	APlayerController* pc = Cast<APlayerController>(Controller);
	if (pc != nullptr) {
		UEnhancedInputLocalPlayerSubsystem* subsys =
			ULocalPlayer::GetSubsystem< UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsys != nullptr) {
			subsys->AddMappingContext(imcDefault, 0);
			subsys->AddMappingContext(imcMouseLook, 0);
		}
	}
	
	ChangeToRifleGun(FInputActionValue());

	sniperUI = CreateWidget(GetWorld(), sniperUIFactory);
	crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);

	crosshairUI->AddToViewport();
}

// Called every frame
void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PlayerMove();

}

// Called to bind functionality to input
void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (enhancedInputComponent != nullptr) {
		enhancedInputComponent->BindAction(iaMove, ETriggerEvent::Triggered, this, &ATPSCharacter::Move);
		enhancedInputComponent->BindAction(iaMouseLook, ETriggerEvent::Triggered, this, &ATPSCharacter::Look);
		enhancedInputComponent->BindAction(iaJump, ETriggerEvent::Started, this, &ATPSCharacter::Jump);
		enhancedInputComponent->BindAction(iaJump, ETriggerEvent::Completed, this, &ATPSCharacter::StopJumping);
		enhancedInputComponent->BindAction(iaFire, ETriggerEvent::Started, this, &ATPSCharacter::InputFire);
		enhancedInputComponent->BindAction(iaGrenadeGun, ETriggerEvent::Started, this, &ATPSCharacter::ChangeToGrenadeGun);
		enhancedInputComponent->BindAction(iaRifleGun, ETriggerEvent::Started, this, &ATPSCharacter::ChangeToRifleGun);
		enhancedInputComponent->BindAction(iaSniper, ETriggerEvent::Started, this, &ATPSCharacter::SniperAim);
		enhancedInputComponent->BindAction(iaSniper, ETriggerEvent::Completed, this, &ATPSCharacter::SniperAim);
		enhancedInputComponent->BindAction(ia_Run, ETriggerEvent::Started, this, &ATPSCharacter::InputRun);
		enhancedInputComponent->BindAction(ia_Run, ETriggerEvent::Completed, this, &ATPSCharacter::InputRun);

	}

}

void ATPSCharacter::Move(const struct FInputActionValue& inputValue) {

	FVector2D value = inputValue.Get< FVector2D>();
	direction.X = value.Y;
	direction.Y = value.X;
}

void ATPSCharacter::Look(const struct FInputActionValue& inputValue) {
	FVector2D value = inputValue.Get<FVector2D>();
	AddControllerYawInput(value.X);
	AddControllerPitchInput(value.Y);
}

void ATPSCharacter::PlayerMove() {
	direction = FTransform(GetControlRotation()).TransformVector(direction);
	/*FVector P0 = GetActorLocation();
	FVector vt = direction * walkSpeed * DeltaTime;
	FVector P = P0 + vt;
	SetActorLocation(P);*/
	AddMovementInput(direction);
	direction = FVector::ZeroVector;
}

void ATPSCharacter::InputFire(const struct FInputActionValue& inputValue) {

	UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);

	auto controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StartCameraShake(cameraShake);

	auto anim = Cast<UCharacterAnim>(GetMesh()->GetAnimInstance());
	anim->PlayAttackAnim();

	if (bUsingGrenadeGun) {
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	}
	else {
		FVector startPos = tpsCamComp->GetComponentLocation();
		FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp->GetForwardVector() * 5000;
		FHitResult hitInfo;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
		if (bHit) {
			FTransform bulletTrans;
			bulletTrans.SetLocation(hitInfo.ImpactPoint);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);

			auto hitComp = hitInfo.GetComponent();
			if (hitComp && hitComp->IsSimulatingPhysics()) {
				FVector dir = (endPos - startPos).GetSafeNormal();
				FVector force = dir * hitComp->GetMass() * 500000;
				hitComp->AddForceAtLocation(force, hitInfo.ImpactPoint);
			}

			auto enemy = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));
			if (enemy) {
				UEnemyFSM* enemyFSM = Cast<UEnemyFSM>(enemy);
				enemyFSM->OnDamageProcess();
			}
		}
	}
}

void ATPSCharacter::ChangeToGrenadeGun(const struct FInputActionValue& inputValue) {
	bUsingGrenadeGun = true;
	gunMeshComp->SetVisibility(true);
	sniperGunComp->SetVisibility(false);
}

void ATPSCharacter::ChangeToRifleGun(const struct FInputActionValue& inputValue) {
	bUsingGrenadeGun = false;
	gunMeshComp->SetVisibility(false);
	sniperGunComp->SetVisibility(true);
}

void ATPSCharacter::SniperAim(const struct FInputActionValue& inputValue) {
	if (bUsingGrenadeGun) {
		return;
	}
	if (bSniperAim) {
		bSniperAim = false;
		sniperUI->RemoveFromParent();
		tpsCamComp->SetFieldOfView(90.0f);
		crosshairUI->AddToViewport();
	}
	else {
		bSniperAim = true;
		sniperUI->AddToViewport();
		tpsCamComp->SetFieldOfView(45.0f);
		crosshairUI->RemoveFromParent();
	}
}

void ATPSCharacter::InputRun(const FInputActionValue& inputValue) {
	auto movement = GetCharacterMovement();
	if (movement->MaxWalkSpeed > walkSpeed) {
		movement->MaxWalkSpeed = walkSpeed;
	}
	else {
		movement->MaxWalkSpeed = runSpeed;
	}
}