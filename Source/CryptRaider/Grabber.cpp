// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "Engine/World.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Early return if there si no physics handle
	if (!GetPhysicsHandle())
	{
		return;
	}

	if (auto GrabbedComponent = GetGrabbedComponent())
	{
		if (FVector::Distance(GrabbedComponent->GetComponentLocation(), GetComponentLocation()) > DropDistance)
		{
			// Release the object if it gets too far away
			Release();
		}
		else
		{
			// Update grabbed component location and rotation
			const FVector TargetLocation = GetComponentLocation() + GetForwardVector() * HoldDistance;
			GetPhysicsHandle()->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
		}
	}
	else
	{
		if (Debug)
		{
			// Draw sphere at max grab distance. Red if it's colliding, blue if it's not.
			DrawDebugSphere(
				GetWorld(),
				GetComponentLocation() + GetForwardVector() * MaxGrabDistance,
				GrabSphereRadius,
				20,
				CheckHit().has_value() ? FColor::Red : FColor::Blue
			);
		}
	}
}


void UGrabber::Grab()
{
	// Early return if there si no physics handle
	if (!GetPhysicsHandle())
	{
		return;
	}

	// Do nothing if it's grabbing an object already
	if (IsGrabbing())
	{
		return;
	}

	if (auto HitResult = CheckHit();
		HitResult.has_value())
	{
		if (Debug)
		{
			UE_LOG(LogTemp, Display, TEXT("Grabbing actor: %s"), *HitResult->GetActor()->GetActorNameOrLabel());
		}

		auto HitComponent = HitResult->GetComponent();

		// Wake hit component's physics so it's ready to be grabbed.
		HitComponent->WakeAllRigidBodies();

		GetPhysicsHandle()->GrabComponentAtLocationWithRotation(
			HitComponent,
			NAME_None, 
			HitResult->ImpactPoint, 
			GetComponentRotation()
		);
	}
	else
	{
		if (Debug)
		{
			UE_LOG(LogTemp, Display, TEXT("No actor grabbed!"));
		}
	}
}


void UGrabber::Release()
{
	// Early return if there si no physics handle
	if (!GetPhysicsHandle())
	{
		return;
	}

	if (auto GrabbedComponent = GetGrabbedComponent())
	{
		if (Debug)
		{
			UE_LOG(LogTemp, Display, TEXT("Releasing actor: %s"), *GrabbedComponent->GetOwner()->GetActorNameOrLabel());
		}

		// Wake up the grabbed component so it's physics are active when it's released
		GrabbedComponent->WakeAllRigidBodies();

		GetPhysicsHandle()->ReleaseComponent();
	}
	else
	{
		if (Debug)
		{
			UE_LOG(LogTemp, Display, TEXT("No actor to release!"));
		}
	}
}


bool UGrabber::IsGrabbing() const
{
	return GetGrabbedComponent() != nullptr;
}


UPhysicsHandleComponent* UGrabber::GetPhysicsHandle() const
{
	return GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
}


UPrimitiveComponent* UGrabber::GetGrabbedComponent() const
{
	auto PhysicsHandle = GetPhysicsHandle();
	return PhysicsHandle
		? PhysicsHandle->GetGrabbedComponent()
		: nullptr;
}


std::optional<FHitResult> UGrabber::CheckHit() const
{
	const FVector StartLocation = GetComponentLocation();
	const FVector EndLocation = StartLocation + GetForwardVector() * MaxGrabDistance;

	FHitResult HitResult;
	bool Hit = GetWorld()->SweepSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		FQuat::Identity,
		ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(GrabSphereRadius)
	);

	return Hit 
		? std::optional<FHitResult>(std::move(HitResult)) 
		: std::nullopt;
}
