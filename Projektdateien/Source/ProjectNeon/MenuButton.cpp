// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuButton.h"

void UMenuButton::NativeOnInitialized() {
	Super::NativeOnInitialized();
}

UButton* UMenuButton::GetButton() {
	return Button;
}