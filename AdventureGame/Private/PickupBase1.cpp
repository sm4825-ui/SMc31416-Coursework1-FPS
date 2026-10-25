#include "PickupBase1.h"

#include "ItemDefinition.h"
#include "Data/ItemData.h"
#include "AdventureCharacter.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "UObject/Package.h"

APickupBase1::APickupBase1()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupMeshComponent =
		CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));

	SetRootComponent(PickupMeshComponent);

	SphereComponent =
		CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));

	SphereComponent->SetupAttachment(GetRootComponent());
	SphereComponent->SetSphereRadius(50.0f);
	SphereComponent->SetGenerateOverlapEvents(true);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void APickupBase1::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.RemoveAll(this);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(
		this, &APickupBase1::OnSphereBeginOverlap);

	InitializePickup();
}

void APickupBase1::InitializePickup()
{
	if (PickupItemID.IsNone())
	{
		return;
	}

	UDataTable* LoadedDataTable = PickupDataTable.IsValid()
		? PickupDataTable.Get()
		: PickupDataTable.LoadSynchronous();

	if (!LoadedDataTable)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("PickupBase1: Could not load PickupDataTable."));
		return;
	}

	const FItemData* ItemDataRow =
		LoadedDataTable->FindRow<FItemData>(
			PickupItemID,
			TEXT("PickupBase1::InitializePickup"));

	if (!ItemDataRow)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("PickupBase1: Item row not found."));
		return;
	}

	UItemDefinition* SourceDefinition = ItemDataRow->ItemBase.IsValid()
		? ItemDataRow->ItemBase.Get()
		: ItemDataRow->ItemBase.LoadSynchronous();

	if (!SourceDefinition)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("PickupBase1: Could not load item definition."));
		return;
	}

	ReferenceItem = SourceDefinition->CreateItemCopy(GetTransientPackage());

	if (!ReferenceItem)
	{
		return;
	}

	ReferenceItem->ID = ItemDataRow->ID;
	ReferenceItem->ItemType = ItemDataRow->ItemType;
	ReferenceItem->ItemText = ItemDataRow->ItemText;

	UStaticMesh* LoadedMesh = ReferenceItem->WorldMesh.IsValid()
		? ReferenceItem->WorldMesh.Get()
		: ReferenceItem->WorldMesh.LoadSynchronous();

	if (LoadedMesh)
	{
		PickupMeshComponent->SetStaticMesh(LoadedMesh);
	}

	PickupMeshComponent->SetVisibility(true);
	PickupMeshComponent->SetCollisionEnabled(
		ECollisionEnabled::QueryOnly);

	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void APickupBase1::OnSphereBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	AAdventureCharacter* Character = Cast<AAdventureCharacter>(OtherActor);

	if (!Character || !ReferenceItem)
	{
		return;
	}

	Character->GiveItem(ReferenceItem);

	PickupMeshComponent->SetVisibility(false);
	PickupMeshComponent->SetCollisionEnabled(
		ECollisionEnabled::NoCollision);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (bShouldRespawn)
	{
		GetWorldTimerManager().SetTimer(
			RespawnTimerHandle,
			this,
			&APickupBase1::InitializePickup,
			RespawnTime,
			false);
	}
}

#if WITH_EDITOR
void APickupBase1::PostEditChangeProperty(
	FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.Property
		? PropertyChangedEvent.Property->GetFName()
		: NAME_None;

	if (ChangedPropertyName ==
		GET_MEMBER_NAME_CHECKED(APickupBase1, PickupItemID) ||
		ChangedPropertyName ==
		GET_MEMBER_NAME_CHECKED(APickupBase1, PickupDataTable))
	{
		InitializePickup();
	}
}
#endif
