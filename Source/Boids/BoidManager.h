// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoidManager.generated.h"

UENUM()
enum Behaviour {
	none UMETA(DisplayName = "None"),
	tag UMETA(DisplayName = "Tag"),
	flock UMETA(DisplayName = "Flock")
};

UCLASS()
class BOIDS_API ABoidManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoidManager();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TEnumAsByte<Behaviour> behaviour = none;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int spawnCount = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float spawnRadius = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float speed = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int constraints = 3000;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	bool spawnOnStart = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool simulate = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wander")
	bool wander = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wander")
	float wanderThreshold = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wander")
	float wanderRadius = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wander")
	float wanderDistance = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wander")
	float wanderJitter = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wander")
	float wanderDestinationThreshold = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tag")
	float maxFleeDistance = 9999999.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tag")
	float chaserMultiplier = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tag")
	float catchRange = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tag")
	float timeOutDuration = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flock")
	float neighbourRadius = 900.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flock")
	float separationWeight = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flock")
	float cohesionWeight = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flock")
	float alignmentWeight = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flock")
	float cohesionThreshold = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flock")
	float alignmentThreshold = 100.f;
	
	FString meshPath;
	FString materialPath;
	FVector minConstraints;
	FVector maxConstraints;
	float timeOut = 0;
	USceneComponent* transform;
	TArray<class ABoid*> boidsArray;
	class ABoid* lastTagged;
	class ABoid* tagged;
	class ABoid* tempTagged;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void TimeOut(float DeltaTime);
	void UpdateChooseBehaviour(float DeltaTime);
	void TagUpdateBoids(float DeltaTime);
	void FlockUpdateBoids(float DeltaTime);
	FVector NearestBoid(ABoid* thisBoid);
	TArray<class ABoid*> GetBoidNeighbours(class ABoid* thisBoid);

	UFUNCTION(BlueprintCallable, Category = "Mesh")
	static void ChangeMeshMaterial(FString Mesh, FString Material);
	UFUNCTION(BlueprintCallable, Category = "Boids")
	void SpawnBoids();
	UFUNCTION(BlueprintCallable, Category = "Boids")
	void DeleteBoids();
	UFUNCTION(BlueprintCallable, Category = "Boids")
	void DeleteManager();
	UFUNCTION(BlueprintCallable, Category = "Boids")
	void ResetBoids();
	UFUNCTION(BlueprintCallable, Category = "Boids")
	void CalculateConstraints();
};
