// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Public/DrawDebugHelpers.h"
#include "Components/PrimitiveComponent.h"
#define OUT
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
	FindPhysicsHandleComponent();
	SetupInputComponent();
}

void UGrabber::FindPhysicsHandleComponent() {
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle) {
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s is missing UPhysicsHandleComponent"),
			*(GetOwner()->GetName())
		)
	}
}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent) {
		UE_LOG(LogTemp, Warning, TEXT("%s has a UInputComponent"),
			*(GetOwner()->GetName())
		);
		///Bind an action to a method
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s is missing UInputComponent"),
			*(GetOwner()->GetName())
		)

	}
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation);
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());//No tag, No Complex Trace, Ignore the owner

	FHitResult Hit;
	

	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewPointLocation, //start 
		LineTraceEnd, //end
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	//FString LocationString = PlayerViewPointLocation.ToString();
	//FString RotationString = PlayerViewPointRotation.ToString();
	//UE_LOG(LogTemp, Warning, TEXT("L: %s, R: %s"),*LocationString,*RotationString);

	return Hit;
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation);
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (PhysicsHandle->GrabbedComponent) {
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}
}

void UGrabber::Grab() {
	UE_LOG(LogTemp, Warning, TEXT("Grabbed pressed"));
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	AActor* ActorHit = HitResult.GetActor();
	if (ActorHit){
		PhysicsHandle->GrabComponent(
			ComponentToGrab,
			NAME_None,//waht bone to grab
			ComponentToGrab->GetOwner()->GetActorLocation(), //where to grab
			true);//allow rotation
	}
}

void UGrabber::Release() {
	UE_LOG(LogTemp, Warning, TEXT("Grab Released"));
	PhysicsHandle->ReleaseComponent();
}

//DrawDebugLine(
//	GetWorld(),
//	PlayerViewPointLocation,
//	LineTraceEnd,
//	FColor(255, 0, 0),
//	false, //line persist
//	0.f, //lifetime
//	0.f, //priority
//	10.0f);//thickness

