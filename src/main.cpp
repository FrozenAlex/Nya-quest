#include "main.hpp"
#include "ModifiersMenu.hpp"
#include "bsml/shared/BSML.hpp"
#include "GlobalNamespace/ResultsViewController.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "GlobalNamespace/PauseMenuManager.hpp"
#include "GlobalNamespace/GamePause.hpp"
#include "GlobalNamespace/BeatmapObjectManager.hpp"
#include "GlobalNamespace/ScoreModel.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/MultiplayerResultsViewController.hpp"
#include "GlobalNamespace/GameServerLobbyFlowCoordinator.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/MenuTransitionsHelper.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "NyaConfig.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "GlobalNamespace/OVRInput.hpp"
#include "GlobalNamespace/OculusVRHelper.hpp"
#include "GlobalNamespace/MainFlowCoordinator.hpp"
#include "Events.hpp"
#include "UI/FlowCoordinators/NyaSettingsFlowCoordinator.hpp"
#include <fstream>
#include <string>
#include "logging.hpp"
#include "Utils/FileUtils.hpp"

// beatsaber-hook is a modding framework that lets us call functions and fetch field values from in the game
// It also allows creating objects, configuration, and importantly, hooking methods to modify their values
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"


using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace Nya;


SafePtrUnity<Nya::NyaFloatingUI> Nya::Main::NyaFloatingUI = nullptr;

MAKE_HOOK_MATCH(Pause, &GamePause::Pause, void, GamePause* self) {
    Pause(self);
    DEBUG("Pause");
    if (Main::NyaFloatingUI){
        Nya::Main::NyaFloatingUI->onSceneChange(Nya::FloatingUIScene::Pause);
    }
    
}

MAKE_HOOK_MATCH(Unpause, &GamePause::Resume, void, GlobalNamespace::GamePause* self) {
    Unpause(self);
    DEBUG("Unpause");
    if (Main::NyaFloatingUI){
        Nya::Main::NyaFloatingUI->onSceneChange(Nya::FloatingUIScene::Disabled);
    }
    
}

MAKE_HOOK_MATCH(Restartbutton, &PauseMenuManager::RestartButtonPressed, void, PauseMenuManager* self) {
    Restartbutton(self);
    DEBUG("Restartbutton");
    if (Main::NyaFloatingUI){
        Nya::Main::NyaFloatingUI->onSceneChange(Nya::FloatingUIScene::Disabled);
    }
}

MAKE_HOOK_MATCH(Results, &ResultsViewController::Init, void, ResultsViewController* self, LevelCompletionResults* levelCompletionResults, IReadonlyBeatmapData* transformedBeatmapData, ByRef<::GlobalNamespace::BeatmapKey> beatmapKey, ::GlobalNamespace::BeatmapLevel* beatmapLevel, bool practice, bool newHighScore) {
    DEBUG("Results");
    Results(self, levelCompletionResults, transformedBeatmapData, beatmapKey, beatmapLevel, practice, newHighScore);
}

MAKE_HOOK_MATCH(MultiResults, &MultiplayerResultsViewController::DidActivate, void, MultiplayerResultsViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    MultiResults(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    DEBUG("MultiResults");
    if (Main::NyaFloatingUI){
        Nya::Main::NyaFloatingUI->onSceneChange(Nya::FloatingUIScene::MainMenu);
    }
    
}

// Soft restart in settings
MAKE_HOOK_MATCH(MenuTransitionsHelper_RestartGame, &MenuTransitionsHelper::RestartGame, void, MenuTransitionsHelper* self, System::Action_1<Zenject::DiContainer*>* finishCallback)
{
    DEBUG("MenuTransitionsHelper_RestartGame");
    // Destroy the floating UI on soft restart
    if (Main::NyaFloatingUI && Main::NyaFloatingUI){
        // Destroy the floating UI screen
        if (Main::NyaFloatingUI->floatingScreen && Main::NyaFloatingUI->floatingScreen->get_gameObject()){
            GameObject::DestroyImmediate(Main::NyaFloatingUI->floatingScreen->get_gameObject());
        }
        Nya::NyaFloatingUI::delete_instance();
        Main::NyaFloatingUI = nullptr;
    }
    MenuTransitionsHelper_RestartGame(self, finishCallback);
}

MAKE_HOOK_MATCH(MainFlowCoordinator_DidActivate, &GlobalNamespace::MainFlowCoordinator::DidActivate, void, GlobalNamespace::MainFlowCoordinator* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    MainFlowCoordinator_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    DEBUG("MainFlowCoordinator_DidActivate");
    
    if (!Main::NyaFloatingUI) {
        Nya::Main::NyaFloatingUI = Nya::NyaFloatingUI::get_instance();
        Nya::Main::NyaFloatingUI->onSceneChange(Nya::FloatingUIScene::MainMenu);
    } else {
        // It runs after going to main menu from multi to show the window if it's needed
        Nya::Main::NyaFloatingUI->onSceneChange(Nya::FloatingUIScene::MainMenu);
    }
}

// On scene change
MAKE_HOOK_MATCH(SceneManager_Internal_ActiveSceneChanged, &UnityEngine::SceneManagement::SceneManager::Internal_ActiveSceneChanged, void, UnityEngine::SceneManagement::Scene prevScene, UnityEngine::SceneManagement::Scene nextScene) {
    SceneManager_Internal_ActiveSceneChanged(prevScene, nextScene);
    
    // If floating ui exists
    if(prevScene.IsValid() && nextScene.IsValid()) {
        std::string prevSceneName(prevScene.get_name());
        std::string nextSceneName(nextScene.get_name());

        if (Nya::Main::NyaFloatingUI) {
            BSML::MainThreadScheduler::Schedule([prevScene, nextScene]
            {
                Nya::Main::NyaFloatingUI->OnActiveSceneChanged(prevScene, nextScene);
            });
        }
    }   
}

void makeTempFolder() 
{    
    // Make the temp path
    if (!direxists(NyaGlobals::tempPath))
    {
        int makePath = mkpath(NyaGlobals::tempPath);
        if (makePath == -1)
        {
            ERROR("Failed to make the Temp folder!");
        }
    }
}


void Nya::CleanTempFolder(){
    if (direxists(NyaGlobals::tempPath))
    {
        std::vector<std::string> files = FileUtils::getAllFilesInFolder(NyaGlobals::tempPath);
        for (const std::string& file : files) {
            FileUtils::deleteFile(file);
        }
    }
}

void Nya::ApplyIndexingRules() 
{    
    if (!direxists(NyaGlobals::nyaPath))
    {
        ERROR("Nya folder not found, no reason to proceed!");
        return;
    }
    // Temp folder should not be indexed
    if (direxists(NyaGlobals::tempPath))
    {
        std::string tempNomedia = NyaGlobals::tempPath + ".nomedia";
        if (!fileexists(tempNomedia)) {
            // Create .nomedia file
            std::ofstream f(tempNomedia);
            f.close();
        }
    }

    bool indexSFW = getNyaConfig().IndexSFW.GetValue();
    bool indexNSFW = getNyaConfig().IndexNSFW.GetValue();


    if (direxists(NyaGlobals::imagesPathSFW)) {
        // Indexing rules for sfw
        std::string imagesSFWNomedia = NyaGlobals::imagesPathSFW + ".nomedia";
        if (indexSFW) {
            // If index SFW, delete .nomedia file if it exists
            if (fileexists(imagesSFWNomedia)) {
                // Delete .nomedia file
                remove(imagesSFWNomedia.c_str());
            }
        } else {
            // If dont index SFW, create .nomedia file if it doesnt exist
            if (!fileexists(imagesSFWNomedia)) {
                // Create .nomedia file
                std::ofstream f(imagesSFWNomedia);
                f.close();
            }
        }
    }
    

    if (direxists(NyaGlobals::imagesPathNSFW)) {
        std::string imagesNSFWNomedia = NyaGlobals::imagesPathNSFW + ".nomedia";
        // Indexing rules for nsfw
        if (indexNSFW) {
            // If index NSFW, delete .nomedia file if it exists
            if (fileexists(imagesNSFWNomedia)) {
                // Delete .nomedia file
                remove(imagesNSFWNomedia.c_str());
            }
        } else {
            // If dont index NSFW, create .nomedia file if it doesnt exist
            if (!fileexists(imagesNSFWNomedia)) {
                // Create .nomedia file
                std::ofstream f(imagesNSFWNomedia);
                f.close();
            }
        }
    }
    
}

// Called at the early stages of game loading
extern "C" __attribute__((visibility("default"))) void setup(CModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    info.version_long = GIT_COMMIT;

    INFO("Completed setup!");
}

 // Config manipulations on start
void InitConfigOnStart(){
    // No ui is present, we do some preflight things
    bool rememberNSFW = getNyaConfig().RememberNSFW.GetValue();
    if (!rememberNSFW) {
        getNyaConfig().NSFWEnabled.SetValue(false);
    }
}

// Used to migrate old
void MigrateOldImages() {
    try {
        std::string oldNyaPath = fmt::format(NYA_MOD_PATH_FORMAT, modloader::get_application_id().c_str());
        std::string oldSFWPath = oldNyaPath + "Images/sfw/";
        std::string oldNSFWPath = oldNyaPath + "Images/nsfw/";

        if (direxists(oldSFWPath)) {
            DEBUG("Migrating old SFW images from {}", oldSFWPath);
            std::filesystem::path sourcePath(oldSFWPath);
            std::filesystem::path destPath(NyaGlobals::imagesPathSFW);
            FileUtils::MoveDirectoriesRecursively(sourcePath, destPath);
        }

        if (direxists(oldNSFWPath)) {
            DEBUG("Migrating old NSFW images from {}", oldNSFWPath);
            std::filesystem::path sourcePath(oldNSFWPath);
            std::filesystem::path destPath(NyaGlobals::imagesPathNSFW);
            FileUtils::MoveDirectoriesRecursively(sourcePath, destPath);
        }

        
    } catch (...) {
        INFO("Failed to migrate old images, skipping...");
    }
}

// Handling buttons
MAKE_HOOK_MATCH(FixedUpdateHook, &GlobalNamespace::OculusVRHelper::FixedUpdate, void, GlobalNamespace::OculusVRHelper* self){
    FixedUpdateHook(self);


     static bool pressedEventAllreadyRun = false;
    
    // // 0 Means nothing is assigned, and we dont need to do anything
     int useButtonValue = getNyaConfig().UseButton.GetValue();
     if(useButtonValue > 0){
         // Determine if we need the Right or Left Controller (Right is 2 Left is One)
         // Definition from: GlobalNamespace::OVRInput::Controller::RTouch
         int controllerIndex = useButtonValue > 2 ? 1 : 2;

         // Here we correct the Index for direct Usage as Input for OVRInput.Get
         // After this line the Primary Button A/X (1/3 in Config) is 0 and the Secondary Button (2/4 in Config) is 1
         // Source: https://developer.oculus.com/documentation/unity/unity-ovrinput/
         useButtonValue = ((useButtonValue - 1) % 2) + 1;

         bool buttonPressed = GlobalNamespace::OVRInput::Get(GlobalNamespace::__OVRInput__Button(useButtonValue), controllerIndex);
         if(buttonPressed){
             if(!pressedEventAllreadyRun) {
                 if (Nya::GlobalEvents::onControllerNya.size() > 0) {
                     Nya::GlobalEvents::onControllerNya.invoke();
                     pressedEventAllreadyRun = true;
                 }
             }
         }
         else {
             pressedEventAllreadyRun = false;
         }
     }
}

// Called later on in the game loading - a good time to install function hooks
extern "C" __attribute__((visibility("default"))) void late_load() {
    il2cpp_functions::Init();
    
    BSML::Init();

    // Should always be before any custom types can possibly be used
    custom_types::Register::AutoRegister();

    // Load the config - make sure this is after il2cpp_functions::Init();
    getNyaConfig().Init(modInfo);

    // Do config validation and modifications on start
    InitConfigOnStart();

    try {
        // Make local folders if they do not exist
        makeTempFolder();
        // Sometimes when crashing, the temp folder is not deleted, so we do it here on start
        Nya::CleanTempFolder();
        Nya::ApplyIndexingRules();
        MigrateOldImages();
    } catch (std::exception& e) {
        ERROR("Error making folders and applying indexing rules: {}", e.what());
    }
    

    BSML::Register::RegisterGameplaySetupTab<Nya::ModifiersMenu*>("Nya");
    BSML::Register::RegisterSettingsMenu<Nya::UI::FlowCoordinators::NyaSettingsFlowCoordinator*>("Nya");

    INFO("Installing hooks...");
    INSTALL_HOOK(Logger, Pause);
    INSTALL_HOOK(Logger, FixedUpdateHook);
    INSTALL_HOOK(Logger, Results);
    INSTALL_HOOK(Logger, Unpause);
    INSTALL_HOOK(Logger, Restartbutton);
    INSTALL_HOOK(Logger, MultiResults);
    INSTALL_HOOK(Logger, MenuTransitionsHelper_RestartGame);
    INSTALL_HOOK(Logger, SceneManager_Internal_ActiveSceneChanged);
    INSTALL_HOOK(Logger, MainFlowCoordinator_DidActivate);
    INFO("Installed all hooks!");
}
