#include "PickupBase1.h"
#include "ItemDefinition.h"
#include "Data/ItemData.h"
#include "Engine/DataTable.h"
#include "TimerManager.h"

APickupBase1::APickupBase1()
{
	PrimaryActorTick.bCanEverTick = true;

	PickupMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	check(PickupMeshComponent != nullptr);
	SetRootComponent(PickupMeshComponent);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	check(SphereComponent != nullptr);
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
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APickupBase1::OnSphereBeginOverlap);

	InitializePickup();
}

void APickupBase1::InitializePickup()
{
	const FSoftObjectPath TablePath = PickupDataTable.ToSoftObjectPath();
	if (!TablePath.IsNull() && !PickupItemID.IsNone())
	{
		UDataTable* LoadedDataTable = PickupDataTable.IsValid()
			? PickupDataTable.Get()
			: PickupDataTable.LoadSynchronous();

		if (!LoadedDataTable) return;

		const FItemData* ItemDataRow = LoadedDataTable->FindRow<FItemData>(PickupItemID, TEXT("PickupBase1::InitializePickup"));
		if (!ItemDataRow) return;

		UItemDefinition* TempItemDefinition = ItemDataRow->ItemBase.IsValid()
			? ItemDataRow->ItemBase.Get()
			: ItemDataRow->ItemBase.LoadSynchronous();

		if (!TempItemDefinition) return;

		ReferenceItem = NewObject<UItemDefinition>(this);
		ReferenceItem->ID = ItemDataRow->ID;
		ReferenceItem->ItemType = ItemDataRow->ItemType;
		ReferenceItem->ItemText = ItemDataRow->ItemText;
		ReferenceItem->WorldMesh = TempItemDefinition->WorldMesh;

		UStaticMesh* LoadedMesh = TempItemDefinition->WorldMesh.IsValid()
			? TempItemDefinition->WorldMesh.Get()
			: TempItemDefinition->WorldMesh.LoadSynchronous();

		if (LoadedMesh)
		{
			PickupMeshComponent->SetStaticMesh(LoadedMesh);
		}

		PickupMeshComponent->SetVisibility(true);
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void APickupBase1::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Attempting a pickup collision"));

	AAdventureCharacter* Character = Cast<AAdventureCharacter>(OtherActor);
	if (Character != nullptr)
	{
		PickupMeshComponent->SetVisibility(false);
		PickupMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		if (bShouldRespawn)
		{
			GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &APickupBase1::InitializePickup, RespawnTime, false);
		}
	}
}

#if WITH_EDITOR
void APickupBase1::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.Property
		? PropertyChangedEvent.Property->GetFName()
		: NAME_None;

	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(APickupBase1, PickupItemID) ||
		ChangedPropertyName == GET_MEMBER_NAME_CHECKED(APickupBase1, PickupDataTable))
	{
		InitializePickup();
	}
}
#endif