// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ComboBoxString.h"
#include "Components/CheckBox.h"
#include "GameFramework/GameUserSettings.h"
#include "PopUp.h"
#include "Components/Slider.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "OptionsMenu.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNEON_API UOptionsMenu : public UUserWidget
{
	GENERATED_BODY()

protected:

	/**
	* Initializes the main menu.
	* All click events for the buttons are set.
	* Disables the options menu on start.
	*/
	virtual void NativeOnInitialized() override;

	/* settings of the user */
	UPROPERTY(Transient)
	UGameUserSettings* UserSettings;

	/* combo box containing the possible window modes */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UComboBoxString* WindowModeComboBox;

	/* combo box containing all possible resolutions */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UComboBoxString* ResolutionComboBox;

	/* combo box containing all possible texture qualities */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UComboBoxString* TextureQualityComboBox;

	/* combo box containing all possible anti aliasing qualities */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UComboBoxString* AntiAliasingQualityComboBox;

	/* combo box containing all possible post processing qualities */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UComboBoxString* PostProcessingQualityComboBox;

	/* combo box containing all possible shadow qualities */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UComboBoxString* ShadowQualityComboBox;

	/* check box for enableing vsync */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCheckBox* VSyncCheckBox;

	/* slider to change the master volume */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USlider* MasterVolumeSlider;

	/* slider to change the music volume */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USlider* MusicVolumeSlider;

	/* slider to change the UI-effects volume */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USlider* UIEffectsVolumeSlider;

	/* master sound mix */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	USoundMix* SoundMix;

	/* master sound class, have all other sound classes as children */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	USoundClass* MasterClass;

	/* music sound class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	USoundClass* MusicClass;

	/* UI-effects sound class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	USoundClass* UIEffectsClass;

	/* class of pop up widget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> PopUpClass;

	/* widget containing pop up */
	UPopUp* PopUpWidget;

	/* array of all supported resolutions */
	TArray<FIntPoint> ScreenResolutions;

protected:

	/**
	 * @brief Fills the combo box containing
	 * the window modes. Automatically called
	 * when initializing all containers.
	 */
	void FillWindowModes();

	/**
	 * @brief Fills the combo box containing
	 * the window modes. Automatically called
	 * when initializing all containers.
	 */
	void FillResolutions();

	void FillTextureQualities();

	void FillAntiAliasingQualities();

	void FillPostProcessingQualities();

	void FillShadowQualities();

	/**
	 * @brief Initializes all containers of the
	 * options menu.
	 */
	void InitContainers();

	/**
	 * @brief Initializes all volume sliders of the obtions menu.
	 */
	void InitVolumeSliders();

	/**
	 * @brief Sets the master volume.
	 */
	UFUNCTION(BlueprintCallable)
	void SetMasterVolume();

	/**
	 * @brief Sets the music volume.
	 */
	UFUNCTION(BlueprintCallable)
	void SetMusicVolume();

	/**
	 * @brief Sets the UI-effects volume.
	 */
	UFUNCTION(BlueprintCallable)
	void SetUIEffectsVolume();

	/**
	 * @brief Sets the fullscreen mode in
	 * user settings based on the chosen
	 * combo box option.
	 */
	UFUNCTION(BlueprintCallable)
	void SetWindowMode();

	/**
	 * @brief Sets the vsync mode in
	 * user settings based on the check
	 * box state.
	 */
	UFUNCTION(BlueprintCallable)
	void SetVSync();

	/**
	 * @brief Sets the fullscreen mode in
	 * user settings based on the chosen
	 * combo box option.
	 */
	UFUNCTION(BlueprintCallable)
	void SetResolution();

	UFUNCTION(BlueprintCallable)
	void SetTextureQuality();

	UFUNCTION(BlueprintCallable)
	void SetAntiAliasingQuality();

	UFUNCTION(BlueprintCallable)
	void SetPostProcessingQuality();

	UFUNCTION(BlueprintCallable)
	void SetShadowQuality();

	/**
	 * @brief Saves and applies the
	 * chosen settings.
	 */
	UFUNCTION(BlueprintCallable)
	void SaveSettings();

	/**
	 * @brief Sets the containers to
	 * match the currently chosen options.
	 */
	UFUNCTION(BlueprintCallable)
	void SetCurrentSettings();

	/**
	 * @brief Discards all currently chosen
	 * settings and loads back the saved ones
	 * from persistent storage.
	 */
	UFUNCTION(BlueprintCallable)
	void DiscardSettings();

	/**
	 * @brief Discards the currently changed settings
	 * and returns back to main menu.
	 */
	void DiscardAndClose();

	/**
	 * @brief Checks if the settings have unsaved changes
	 * Creates a confirmation popup if so or exits the menu
	 * otherwise.
	 */
	UFUNCTION(BlueprintCallable)
	void ReturnToMenu();

	/**
	 * @brief Closes this widget.
	 */
	UFUNCTION(BlueprintCallable)
	void CloseMenu();

	/**
	 * @brief Checks if the resolution combo box
	 * should be available.
	 */
	void UpdateComponentAvailability();
};