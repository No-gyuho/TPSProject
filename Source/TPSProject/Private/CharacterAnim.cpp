// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnim.h"
#include "TPSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCharacterAnim::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto ownerPawn = TryGetPawnOwner();
	ATPSCharacter* player = Cast<ATPSCharacter>(ownerPawn);
	if (player) {
		FVector velocity = player->GetVelocity();
		//speed = velocity.Length();
		FVector forwardVector = player->GetActorForwardVector();
		speed = FVector::DotProduct(forwardVector, velocity);

		FVector rightVector = player->GetActorRightVector();
		direction = FVector::DotProduct(rightVector, velocity);

		auto movement = player->GetCharacterMovement();
		isInAir = movement->IsFalling();
	}
}

void UCharacterAnim::PlayAttackAnim() {
	Montage_Play(attackAnimMontage);
}

