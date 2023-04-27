// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONLINESHOOTER_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* displayText;

	void SetDisplayText(FString text);
	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(APawn* InPawn);


protected:
	virtual void NativeDestruct() override;

};
