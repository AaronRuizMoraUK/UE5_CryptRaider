// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerComponent.h"
#include "Grabber.h"

// Sets default values for this component's properties
UTriggerComponent::UTriggerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UTriggerComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Do nothing if the mover hasnt' been setup
	if (!Mover)
	{
		return;
	}

	AActor* AcceptableActor = GetAcceptableActor();

	// While there is an acceptable actor in the trigger, move the mover.
	if (AcceptableActor)
	{
		if (TriggersOnce)
		{
			const FName Accepted("Accepted");
			if (!AcceptableActor->ActorHasTag(Accepted))
			{
				// Disable acceptable actor's physics and grabber collisions.
				if (auto PrimitiveComponent = Cast<UPrimitiveComponent>(AcceptableActor->GetRootComponent()))
				{
					PrimitiveComponent->SetSimulatePhysics(false);
					PrimitiveComponent->SetCollisionResponseToChannel(UGrabber::CollisionChannel, ECollisionResponse::ECR_Ignore);
				}

				// Attach the acceptable actor to the trigger's root component.
				// This way the actor moves with the trigger and won't release the trigger.
				AcceptableActor->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);

				// Use "Accepted" tag to mark it as used and avoid reattaching every frame.
				AcceptableActor->Tags.Add(Accepted);
			}
		}

		// Start moving the mover.
		Mover->SetShouldMove(true);
	}
	else
	{
		// Stop moving the mover.
		Mover->SetShouldMove(false);
	}
}

void UTriggerComponent::SetMover(UMover* NewMover)
{
	Mover = NewMover;
}

AActor* UTriggerComponent::GetAcceptableActor() const
{
	TArray<AActor*> Actors;
	GetOverlappingActors(Actors);

	for (AActor* Actor : Actors)
	{
		if (Actor->ActorHasTag(AcceptableActorTag) &&
			!Actor->ActorHasTag(UGrabber::GrabbedTag))
		{
			return Actor;
		}
	}

	return nullptr;
}
