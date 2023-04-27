// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"
#include "Components/TextBlock.h"

void UOverheadWidget::SetDisplayText(FString text)
{
	displayText->SetText(FText::FromString(text));
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole remoteRole = InPawn->GetRemoteRole();
	FString roleString;
	switch (remoteRole)
	{
	case ROLE_None:
		roleString = FString("None");
		break;
	case ROLE_SimulatedProxy:
		roleString = FString("Simulated Proxy");
		break;
	case ROLE_AutonomousProxy:
		roleString = FString("Autonomus Proxy");
		break;
	case ROLE_Authority:
		roleString = FString("Authority");
		break;
	default:
		break;
	}
	SetDisplayText(roleString);
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}
