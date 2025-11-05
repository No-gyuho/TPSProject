// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "TPSCharacter.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "TPSProject.h"
#include "Components/CapsuleComponent.h"
// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSCharacter::StaticClass());
	target = Cast<ATPSCharacter>(actor);
	me = Cast<AEnemy>(GetOwner());
	// ...
	
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FString logMsg = UEnum::GetValueAsString(mState);
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, logMsg);
	switch (mState) {
	case EEnemyState::Idle:
		IdleState();
		break;
	case EEnemyState::Move:
		MoveState();
		break;
	case EEnemyState::Attack:
		AttackState();
		break;
	case EEnemyState::Damage:
		DamageState();
		break;
	case EEnemyState::Die:
		DieState();
		break;
	}

}

void UEnemyFSM::IdleState() {
	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > idleDelayTime) {
		mState = EEnemyState::Move;
		currentTime = 0;
	}
}

void UEnemyFSM::MoveState() {
	FVector destination = target->GetActorLocation();
	FVector direction = destination - me->GetActorLocation();
	me->AddMovementInput(direction.GetSafeNormal());

	if (direction.Size() < attackRange) {
		mState = EEnemyState::Attack;
	}
}

void UEnemyFSM::AttackState() {
	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > attackDelayTime) {
		PRINT_LOG(TEXT("Attack!!!!!"));
		currentTime = 0;
	}

	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());
	if (distance > attackRange) {
		mState = EEnemyState::Move;
	}
}

void UEnemyFSM::DamageState() {
	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > damageDelayTime) {
		mState = EEnemyState::Idle;
		currentTime = 0;
	}
}

void UEnemyFSM::DieState() {
	FVector P0 = me->GetActorLocation();
	FVector vt = FVector::DownVector * dieSpeed * GetWorld()->DeltaTimeSeconds;
	FVector P = P0 + vt;
	me->SetActorLocation(P);

	if (P.Z < -200.0f) {
		me->Destroy();
	}
}

void UEnemyFSM::OnDamageProcess() {
	//me->Destroy();
	hp--;
	if (hp > 0) {
		mState = EEnemyState::Damage;
	}
	else {
		mState = EEnemyState::Die;
		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

