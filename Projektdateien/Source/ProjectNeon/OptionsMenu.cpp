// Fill out your copyright notice in the Description page of Project Settings.


#include "OptionsMenu.h"
#include "MainMenu.h"
#include "Kismet/KismetSystemLibrary.h"

void UOptionsMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UserSettings = UGameUserSettings::GetGameUserSettings();

	UKismetSystemLibrary::GetSupportedFullscreenResolutions(ScreenResolutions);

	InitContainers();
	UserSettings->ApplySettings(false);
}

void UOptionsMenu::FillWindowModes()
{
	WindowModeComboBox->ClearOptions();

	WindowModeComboBox->AddOption("Fullscreen");
	WindowModeComboBox->AddOption("Windowed Fullscreen");
	WindowModeComboBox->AddOption("Windowed");
}

void UOptionsMenu::FillResolutions()
{
	ResolutionComboBox->ClearOptions();

	for (FIntPoint Resolution : ScreenResolutions)
	{
		FString Option = FString();
		Option.AppendInt(Resolution.X);
		Option.Append(TEXT(" x "));
		Option.AppendInt(Resolution.Y);
		ResolutionComboBox->AddOption(Option);
	}
}

void UOptionsMenu::FillTextureQualities()
{
	TextureQualityComboBox->ClearOptions();

	TextureQualityComboBox->AddOption("Low");
	TextureQualityComboBox->AddOption("Medium");
	TextureQualityComboBox->AddOption("High");
	TextureQualityComboBox->AddOption("Ultra");
}

void UOptionsMenu::FillAntiAliasingQualities()
{
	AntiAliasingQualityComboBox->ClearOptions();

	AntiAliasingQualityComboBox->AddOption("Low");
	AntiAliasingQualityComboBox->AddOption("Medium");
	AntiAliasingQualityComboBox->AddOption("High");
	AntiAliasingQualityComboBox->AddOption("Ultra");
}

void UOptionsMenu::FillPostProcessingQualities()
{
	PostProcessingQualityComboBox->ClearOptions();

	PostProcessingQualityComboBox->AddOption("Low");
	PostProcessingQualityComboBox->AddOption("Medium");
	PostProcessingQualityComboBox->AddOption("High");
	PostProcessingQualityComboBox->AddOption("Ultra");
}

void UOptionsMenu::FillShadowQualities()
{
	ShadowQualityComboBox->ClearOptions();

	ShadowQualityComboBox->AddOption("Low");
	ShadowQualityComboBox->AddOption("Medium");
	ShadowQualityComboBox->AddOption("High");
	ShadowQualityComboBox->AddOption("Ultra");
}

void UOptionsMenu::InitContainers()
{
	FillWindowModes();
	FillResolutions();
	FillAntiAliasingQualities();
	FillPostProcessingQualities();
	FillShadowQualities();
	FillTextureQualities();
	InitVolumeSliders();
	SetCurrentSettings();
}

void UOptionsMenu::InitVolumeSliders() 
{
	MasterVolumeSlider->SetMinValue(0.0);
	MasterVolumeSlider->SetMaxValue(1.0);
	MasterVolumeSlider->SetValue(MasterClass->Properties.Volume);

	MusicVolumeSlider->SetMinValue(0.0);
	MusicVolumeSlider->SetMaxValue(1.0);
	MusicVolumeSlider->SetValue(MusicClass->Properties.Volume);

	UIEffectsVolumeSlider->SetMinValue(0.0);
	UIEffectsVolumeSlider->SetMaxValue(1.0);
	UIEffectsVolumeSlider->SetValue(UIEffectsClass->Properties.Volume);
}

void UOptionsMenu::SetMasterVolume()
{
	MasterClass->Properties.Volume = MasterVolumeSlider->GetValue();
}

void UOptionsMenu::SetMusicVolume()
{
	MusicClass->Properties.Volume = MusicVolumeSlider->GetValue();
}

void UOptionsMenu::SetUIEffectsVolume()
{
	UIEffectsClass->Properties.Volume = UIEffectsVolumeSlider->GetValue();
}

void UOptionsMenu::SetWindowMode()
{
	UserSettings->SetFullscreenMode(EWindowMode::ConvertIntToWindowMode(WindowModeComboBox->GetSelectedIndex()));
	UpdateComponentAvailability();
}

void UOptionsMenu::SetVSync()
{
	UserSettings->SetVSyncEnabled(VSyncCheckBox->IsChecked());
}

void UOptionsMenu::SetResolution()
{
	UserSettings->SetScreenResolution(ScreenResolutions[ResolutionComboBox->GetSelectedIndex()]);
}

void UOptionsMenu::SetTextureQuality()
{
	UserSettings->SetTextureQuality(TextureQualityComboBox->GetSelectedIndex());
}

void UOptionsMenu::SetAntiAliasingQuality()
{
	UserSettings->SetAntiAliasingQuality(AntiAliasingQualityComboBox->GetSelectedIndex());
}

void UOptionsMenu::SetShadowQuality()
{
	UserSettings->SetShadowQuality(ShadowQualityComboBox->GetSelectedIndex());
}

void UOptionsMenu::SetPostProcessingQuality()
{
	UserSettings->SetPostProcessingQuality(PostProcessingQualityComboBox->GetSelectedIndex());
}

void UOptionsMenu::SaveSettings()
{
	UserSettings->ApplySettings(false);
    UserSettings->SaveSettings();
	SetCurrentSettings();
}

void UOptionsMenu::SetCurrentSettings()
{
	WindowModeComboBox->SetSelectedIndex(UserSettings->GetFullscreenMode());
	ResolutionComboBox->SetSelectedIndex(ScreenResolutions.IndexOfByKey(UserSettings->GetScreenResolution()));
	TextureQualityComboBox->SetSelectedIndex(UserSettings->GetTextureQuality());
	AntiAliasingQualityComboBox->SetSelectedIndex(UserSettings->GetAntiAliasingQuality());
	PostProcessingQualityComboBox->SetSelectedIndex(UserSettings->GetPostProcessingQuality());
	ShadowQualityComboBox->SetSelectedIndex(UserSettings->GetShadowQuality());
	VSyncCheckBox->SetIsChecked(UserSettings->IsVSyncEnabled());
	UpdateComponentAvailability();
}

void UOptionsMenu::DiscardSettings()
{
	UserSettings->LoadSettings(true);
	SetCurrentSettings();
}

void UOptionsMenu::ReturnToMenu()
{
	if (UserSettings->IsDirty())
	{
		if (PopUpClass)
		{
			PopUpWidget = CreateWidget<UPopUp>(this, PopUpClass);

			if (PopUpWidget)
			{
				PopUpWidget->AddToViewport(9999);
				PopUpWidget->OnDiscardSettings.BindUObject(this, &UOptionsMenu::DiscardAndClose);
			}
		}

	}
	else
	{
		CloseMenu();
	}
}

void UOptionsMenu::CloseMenu()
{
	RemoveFromParent();
	RemoveFromRoot();
}

void UOptionsMenu::UpdateComponentAvailability()
{
	if (UserSettings->GetFullscreenMode() == EWindowMode::WindowedFullscreen)
	{
		int Index = ScreenResolutions.IndexOfByKey(UserSettings->GetDesktopResolution());
		ResolutionComboBox->SetSelectedIndex(Index);
		ResolutionComboBox->SetIsEnabled(false);
	}
	else
	{
		ResolutionComboBox->SetIsEnabled(true);
	}
}

void UOptionsMenu::DiscardAndClose()
{
	DiscardSettings();
	CloseMenu();
}
