// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Boid.generated.h"

UCLASS()
class BOIDS_API ABoid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoid();
	UStaticMeshComponent* mesh;

	FVector currentVelocity;
	class ABoidManager* manager;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector wanderDesintation;

	void MoveBoid(FVector inputVelocity, float DeltaTime);
	void ApplyForces(FVector inputVelocity, float DeltaTime);
	void ApplyRotation();
	void ConstrainBoid();
	void TeleportBoid(FVector location);
	void ScaleBoid();
	FVector Wander(FVector inputVelocity);
	FVector Seek(FVector position);
	FVector Flee(FVector position);
	FVector Alignment(TArray<ABoid*> neighbours);
	FVector Cohesion(TArray<ABoid*> neighbours);
	FVector Seperation(TArray<ABoid*> neighbours);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void TagUpdate(float DeltaTime);
	void FlockUpdate(float DeltaTime);
};
