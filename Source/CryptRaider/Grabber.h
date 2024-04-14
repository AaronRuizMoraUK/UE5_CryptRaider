// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <optional>

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Grabber.generated.h"

class UPhysicsHandleComponent;
class UPrimitiveComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRYPTRAIDER_API UGrabber : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

	inline static const FName GrabbedTag = FName("Grabbed");

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void Grab();

	UFUNCTION(BlueprintCallable)
	void Release();

	UFUNCTION(BlueprintCallable)
	bool IsGrabbing() const;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere)
	float MaxGrabDistance = 330.0f;

	UPROPERTY(EditAnywhere)
	float GrabSphereRadius = 100.0f;

	UPROPERTY(EditAnywhere)
	float HoldDistance = 100.0f;

	UPROPERTY(EditAnywhere)
	float DropDistance = 500.0f;

	UPROPERTY(EditAnywhere)
	bool Debug = false;

	UPhysicsHandleComponent* GetPhysicsHandle() const;

	UPrimitiveComponent* GetGrabbedComponent() const;

	std::optional<FHitResult> CheckHit() const;
};
