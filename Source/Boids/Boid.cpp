// Fill out your copyright notice in the Description page of Project Settings.


#include "Boid.h"
#include "BoidManager.h"

// Sets default values
ABoid::ABoid() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Object Mesh
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	UStaticMesh* newMesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Mesh/Boid_Cone.Boid_Cone'")).Object;

	mesh->SetStaticMesh(newMesh);
	this->SetRootComponent(mesh);
}

// Called when the game starts or when spawned
void ABoid::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ABoid::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

// Adds wander, rotates boid, moves boid
void ABoid::MoveBoid(FVector inputVelocity, float DeltaTime) {
	ConstrainBoid();
	ScaleBoid();
	inputVelocity = Wander(inputVelocity);
	ApplyRotation();
	ApplyForces(inputVelocity, DeltaTime);
}

// Applies forces for target velocity
void ABoid::ApplyForces(FVector inputVelocity, float DeltaTime) {
	FVector newForce = inputVelocity - currentVelocity;
	FVector location = GetActorLocation();
	currentVelocity += newForce * DeltaTime;
	location += currentVelocity * manager->speed * DeltaTime;
	SetActorLocation(location);
}

// Rotates boids based on direction of input velocity
void ABoid::ApplyRotation() {
	FVector direction = currentVelocity.GetSafeNormal();
	FRotator rotation = direction.Rotation();
	rotation += FRotator(-90, 0, 0);
	SetActorRotation(rotation);
}

// Teleports boid to other side of contraints box
void ABoid::ConstrainBoid() {
	FVector location = GetActorLocation();
	FVector min = manager->minConstraints;
	FVector max = manager->maxConstraints;
	// Minimum constraint
	if (location.X <= min.X) TeleportBoid(FVector(max.X, location.Y, location.Z));
	if (location.Y <= min.Y) TeleportBoid(FVector(location.X, max.Y, location.Z));
	if (location.Z <= min.Z) TeleportBoid(FVector(location.X, location.Y, max.Z));
	// Maximum constraint
	if (location.X >= max.X) TeleportBoid(FVector(min.X, location.Y, location.Z));
	if (location.Y >= max.Y) TeleportBoid(FVector(location.X, min.Y, location.Z));
	if (location.Z >= max.Z) TeleportBoid(FVector(location.X, location.Y, min.Z));
}

// Teleports boid to input location
void ABoid::TeleportBoid(FVector location) {
	SetActorLocation(location);
}

// Scales boid size based on movement
void ABoid::ScaleBoid() {
	FVector scale = FVector(1 , 1, 2* currentVelocity.GetAbs().Length());
	SetActorScale3D(scale + 0.5);
}

// Wandering movement (if enabled)
FVector ABoid::Wander(FVector inputVelocity) {
	if (manager->wander && inputVelocity.Size() < manager->wanderThreshold) {
		FVector location = GetActorLocation();
		if (FVector::Dist(location, wanderDesintation) < manager->wanderDestinationThreshold) {
			// Get new destination in front of boid
			FVector projectedPosition = location + (GetActorForwardVector() * manager->wanderDistance);
			wanderDesintation = projectedPosition + (FMath::VRand() * FMath::RandRange(0.f, manager->wanderJitter));
		}
		FVector jitterDestination = Seek(wanderDesintation) + (FMath::VRand() * FMath::RandRange(0.f, manager->wanderJitter));
		inputVelocity += jitterDestination;
		inputVelocity.Normalize();
	}
	return inputVelocity;
}

// Move towards input position
FVector ABoid::Seek(FVector position) {
	FVector newVelocity = position - GetActorLocation();
	newVelocity.Normalize();
	return newVelocity;
}

// Move away from input position
FVector ABoid::Flee(FVector position) {
	FVector newVelocity = GetActorLocation() - position;
	newVelocity.Normalize();
	return newVelocity;
}

// Aligns boid with other boids
FVector ABoid::Alignment(TArray<ABoid*> neighbours) {
	if (neighbours.Num() == 0) return FVector::ZeroVector;
	FVector newVelocity;
	for (ABoid* boid : neighbours) {
		newVelocity += boid->currentVelocity;
	}
	newVelocity /= neighbours.Num();
	newVelocity.Normalize();
	return newVelocity;
}

// Brings boids together
FVector ABoid::Cohesion(TArray<ABoid*> neighbours) {
	if (neighbours.Num() == 0) return FVector::ZeroVector;
	FVector averageLocation;
	for (ABoid* boid : neighbours) {
		averageLocation += boid->GetActorLocation() ;
	}

	averageLocation /= neighbours.Num();

	return Seek(averageLocation);
}

// Pushes boids apart
FVector ABoid::Seperation(TArray<ABoid*> neighbours) {
	if (neighbours.Num() == 0) return FVector::ZeroVector;
	FVector averageFleeVelocity;
	for (ABoid* boid : neighbours) {
		averageFleeVelocity += Flee(boid->GetActorLocation());
	}
	averageFleeVelocity.Normalize();
	return averageFleeVelocity;
}

// Updates boid for chase and flee behaviour;
void ABoid::TagUpdate(float DeltaTime) {
	FVector targetVelocity = FVector::ZeroVector;
	FVector nearestBoidPosition = manager->NearestBoid(this);
	// Calculates velocity to move boid towards/away from nearest boid
	if (manager->tagged == this) {
		// Chase
		targetVelocity = Seek(nearestBoidPosition);
		targetVelocity *= manager->chaserMultiplier;
	}
	else {
		// Flee
		targetVelocity = Flee(nearestBoidPosition);
	}
	MoveBoid(targetVelocity, DeltaTime);
}

// Updates boid for flocking behaviour;
void ABoid::FlockUpdate(float DeltaTime) {
	FVector targetVelocity = FVector::ZeroVector;
	TArray<ABoid*> neighbours = manager->GetBoidNeighbours(this);
	// Add steering behaviours and truncate
	targetVelocity += Seperation(neighbours) * manager->separationWeight;
	if (targetVelocity.Length() < manager->alignmentThreshold) targetVelocity += Alignment(neighbours) * manager->alignmentWeight;
	if (targetVelocity.Length() < manager->cohesionThreshold) targetVelocity += Cohesion(neighbours) * manager->cohesionWeight;
	targetVelocity.Normalize();
	MoveBoid(targetVelocity, DeltaTime);
}