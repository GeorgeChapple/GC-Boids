// Fill out your copyright notice in the Description page of Project Settings.


#include "BoidManager.h"
#include "Boid.h"

// Sets default values
ABoidManager::ABoidManager() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	transform = CreateDefaultSubobject<USceneComponent>("Root Scene Component");
	this -> SetRootComponent(transform);
}

// Called when the game starts or when spawned
void ABoidManager::BeginPlay() {
	Super::BeginPlay();
	// For boid managers placed in the level in editor
	CalculateConstraints();
	if (spawnOnStart) SpawnBoids();
}

// Called every frame
void ABoidManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	UpdateChooseBehaviour(DeltaTime);
}

// Waits set time after a new boid is tagged
// Prevents boids from getting confused and constantly tagging
void ABoidManager::TimeOut(float DeltaTime) {
	if (timeOut > 0) {
		timeOut -= DeltaTime;
	} else {
		tagged = tempTagged;
	} 
}

// Decides which update function run depending on the chosen behaviour in the enum
void ABoidManager::UpdateChooseBehaviour(float DeltaTime) {
	if (simulate) {
		switch (behaviour) {
		case 0:
			break;
		case 1:
			TagUpdateBoids(DeltaTime);
			break;
		case 2:
			FlockUpdateBoids(DeltaTime);
			break;
		default:
			break;
		}
	}
}

// Runs tag behaviour update for all boids in a boid array
void ABoidManager::TagUpdateBoids(float DeltaTime) {
	TimeOut(DeltaTime);
	for (ABoid* boid : boidsArray) {
		boid->TagUpdate(DeltaTime);
	}
}

// Runs flock behaviour update for all boids in a boid array
void ABoidManager::FlockUpdateBoids(float DeltaTime) {
	for (ABoid* boid : boidsArray) {
		boid->FlockUpdate(DeltaTime);
	}
}

// Find nearest boid to the input boid
FVector ABoidManager::NearestBoid(ABoid* thisBoid) {
	float nearestDistance = maxFleeDistance;
	FVector nearestPosition = FVector::ZeroVector;
	// Searches array for the nearest boid, returns it's position
	for (ABoid* boid : boidsArray) {
		if (boid == thisBoid || !boid || boid == lastTagged) continue;
		float aDistance = (boid->GetActorLocation() - thisBoid->GetActorLocation()).Size();
		if (aDistance < nearestDistance) {
			nearestDistance = aDistance;
			nearestPosition = boid->GetActorLocation();
		}
		// If tagged, check to see if within range of nearest boid
		if (thisBoid == tagged) {
			// Tags nearest boid, removes tag from current tagged boid
			if (nearestDistance < catchRange) {
				lastTagged = tagged;
				tempTagged = boid;
				tagged = NULL;
				timeOut = timeOutDuration;
			}
		}
	}
	return nearestPosition;
}

// Returns an array of all the boids within a given radius of the input boid
TArray<class ABoid*> ABoidManager::GetBoidNeighbours(class ABoid* thisBoid) {
	TArray<class ABoid*> returnBoids;
	for (ABoid* boid : boidsArray) {
		if (boid == thisBoid || !boid) continue;
		float aDistance = (boid->GetActorLocation() - thisBoid->GetActorLocation()).Size();
		// If distance is within radius, add current boid to array
		if (aDistance < neighbourRadius) returnBoids.Add(boid);
	}
	return returnBoids;
}

// Sets the material of input mesh to input material
void ABoidManager::ChangeMeshMaterial(FString meshLocation, FString materialLocation) {
	UMaterialInterface* material = Cast<UMaterialInterface>(StaticLoadObject(UObject::StaticClass(), nullptr, *materialLocation));
	UStaticMesh* mesh = Cast<UStaticMesh>(StaticLoadObject(UObject::StaticClass(), nullptr, *meshLocation));
	mesh->SetMaterial(0, material);
	mesh->PostEditChange();
}

// Spawns boids within given radius
void ABoidManager::SpawnBoids() {
	for (int i = 0; i < spawnCount; i++) {
		FVector spawnLocation = (FMath::VRand() * FMath::RandRange(0.f, spawnRadius)) + GetActorLocation();
		FRotator spawnRotation = GetActorRotation();
		ABoid* newBoid = GetWorld()->SpawnActor<ABoid>(spawnLocation, spawnRotation);
		newBoid->manager = this;
		boidsArray.Add(newBoid);
	}
	// Sets first boid to be tagged for tag behaviour
	if (boidsArray[0] != NULL) tempTagged = boidsArray[0];
	simulate = true;
}

// Destroys all boids under this manager 
void ABoidManager::DeleteBoids() {
	simulate = false;
	for (ABoid* boid : boidsArray) {
		boid->Destroy();
	}
	boidsArray.Empty();
}

// Destroys all boids under this manager, then destroys this manager
void ABoidManager::DeleteManager() {
	DeleteBoids();
	Destroy();
}

// Destroys all boids under this manager, then makes new ones
void ABoidManager::ResetBoids() {
	DeleteBoids();
	SpawnBoids();
}

// Calculates boid constraints
void ABoidManager::CalculateConstraints() {
	minConstraints = FVector(constraints / -2);
	maxConstraints = minConstraints * -1;
}