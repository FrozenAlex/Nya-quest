#include "main.hpp"
#include "ModifiersMenu.hpp"

#include "SettingsViewController.hpp"
#include "questui/shared/QuestUI.hpp"
#include "GlobalNamespace/ResultsViewController.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "GlobalNamespace/PauseMenuManager.hpp"
#include "GlobalNamespace/GamePause.hpp"
#include "GlobalNamespace/BeatmapObjectManager.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/ISaberSwingRatingCounter.hpp"
#include "GlobalNamespace/ScoreModel.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/NoteCutDirection.hpp"
#include "GlobalNamespace/CutScoreBuffer.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/MultiplayerResultsViewController.hpp"
#include "GlobalNamespace/GameServerLobbyFlowCoordinator.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "GlobalNamespace/MenuTransitionsHelper.hpp"
#include "GlobalNamespace/MainSettingsModelSO.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "NyaConfig.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "GlobalNamespace/OVRInput.hpp"
#include "GlobalNamespace/OVRInput_Button.hpp"
#include "GlobalNamespace/OculusVRHelper.hpp"
#include "Events.hpp"

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace Nya;

DEFINE_CONFIG(NyaConfig);

Nya::NyaFloatingUI* Nya::Main::NyaFloatingUI = nullptr;

MAKE_HOOK_MATCH(Pause, &GamePause::Pause, void, GamePause* self) {
    Pause(self);
    DEBUG("Pause");
    if (Main::NyaFloatingUI && Main::NyaFloatingUI->m_CachedPtr.m_value){
        Nya::Main::NyaFloatingUI->onSceneChange(Nya::FloatingUIScene::Pause);
    }
    
}

MAKE_HOOK_MATCH(Unpause, &GamePause::Resume, void, GlobalNamespace::GamePause* self) {
    Unpause(self);
    DEBUG("Unpause");
    if (Main::NyaFloatingUI && Main::NyaFloatingUI->m_CachedPtr.m_value){
        Nya::Main::NyaFloatingUI->onSceneChange(Nya::FloatingUIScene::Disabled);
    }
    
}

MAKE_HOOK_MATCH(Restartbutton, &PauseMenuManager::RestartButtonPressed, void, PauseMenuManager* self) {
    Restartbutton(self);
    DEBUG("Restartbutton");
    if (Main::NyaFloatingUI && Main::NyaFloatingUI->m_CachedPtr.m_value){
        Nya::Main::NyaFloatingUI->onSceneChange(Nya::FloatingUIScene::Disabled);
    }
}

MAKE_HOOK_MATCH(Results, &ResultsViewController::Init, void, ResultsViewController* self, LevelCompletionResults* levelCompletionResults, IReadonlyBeatmapData* transformedBeatmapData, IDifficultyBeatmap* difficultyBeatmap, bool practice, bool newHighScore) {
    DEBUG("Results");
    Results(self, levelCompletionResults, transformedBeatmapData, difficultyBeatmap, practice, newHighScore);
}

MAKE_HOOK_MATCH(MultiResults, &MultiplayerResultsViewController::DidActivate, void, MultiplayerResultsViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    MultiResults(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    DEBUG("MultiResults");
    if (Main::NyaFloatingUI && Main::NyaFloatingUI->m_CachedPtr.m_value){
        Nya::Main::NyaFloatingUI->onSceneChange(Nya::FloatingUIScene::MainMenu);
    }
    
}

// Soft restart in settings
MAKE_HOOK_MATCH(MenuTransitionsHelper_RestartGame, &MenuTransitionsHelper::RestartGame, void, MenuTransitionsHelper* self, System::Action_1<Zenject::DiContainer*>* finishCallback)
{
    DEBUG("MenuTransitionsHelper_RestartGame");
    // Destroy the floating UI on soft restart
    if (Main::NyaFloatingUI && Main::NyaFloatingUI->m_CachedPtr.m_value){
        GameObject::DestroyImmediate(Main::NyaFloatingUI->UIScreen->get_gameObject());

        Nya::NyaFloatingUI::delete_instance();
        Main::NyaFloatingUI = nullptr;
    }
    MenuTransitionsHelper_RestartGame(self, finishCallback);
}

MAKE_HOOK_MATCH(MainFlowCoordinator_DidActivate, &GlobalNamespace::MainFlowCoordinator::DidActivate, void, GlobalNamespace::MainFlowCoordinator* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    MainFlowCoordinator_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    DEBUG("MainFlowCoordinator_DidActivate");
    
    if (!Main::NyaFloatingUI || !Main::NyaFloatingUI->m_CachedPtr.m_value) {
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

        if (Nya::Main::NyaFloatingUI != nullptr) {
            QuestUI::MainThreadScheduler::Schedule([prevScene, nextScene]
            {
                Nya::Main::NyaFloatingUI->OnActiveSceneChanged(prevScene, nextScene);
            });
        }
    }   
}

void makeFolder() 
{    
    if (!direxists(NyaGlobals::nyaPath.c_str()))
    {
        int makePath = mkpath(NyaGlobals::nyaPath.c_str());
        if (makePath == -1)
        {
            ERROR("Failed to make Nya Folder path!");
        }
    }

    if (!direxists(NyaGlobals::imagesPath.c_str()))
    {
        int makePath = mkpath(NyaGlobals::imagesPath.c_str());
        if (makePath == -1)
        {
            ERROR("Failed to make Images folder!");
        }
    }
    if (!direxists(NyaGlobals::tempPath.c_str()))
    {
        int makePath = mkpath(NyaGlobals::tempPath.c_str());
        if (makePath == -1)
        {
            ERROR("Failed to make Temp folder!");
        }
    }

    if (!direxists(NyaGlobals::imagesPathSFW.c_str()))
    {
        int makePath = mkpath(NyaGlobals::imagesPathSFW.c_str());
        if (makePath == -1)
        {
            ERROR("Failed to make images folder!");
        }
    }

    if (getNyaConfig().NSFWUI.GetValue()) {
        if (!direxists(NyaGlobals::imagesPathNSFW.c_str()))
        {
            int makePath = mkpath(NyaGlobals::imagesPathNSFW.c_str());
            if (makePath == -1)
            {
                ERROR("Failed to make images2 folder!");
            }
        }
    }
}

Logger& Nya::getLoggerOld() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

// Returns a logger, useful for printing debug messages
Paper::ConstLoggerContext<4UL> Nya::getLogger() {
    static auto fastContext = Paper::Logger::WithContext<"Nya">();
    return fastContext;
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;

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

MAKE_HOOK_MATCH(FixedUpdateHook, &GlobalNamespace::OculusVRHelper::FixedUpdate, void, GlobalNamespace::OculusVRHelper* self){
    FixedUpdateHook(self);


    static bool pressedEventAllreadyRun = false;
    
    // 0 Means nothing is assigned, and we dont need to do anything
    int useButtonValue = getNyaConfig().UseButton.GetValue();
    if(useButtonValue > 0){
        // Determine if we need the Right or Left Controller (Right is 2 Left is One)
        // Definition from: GlobalNamespace::OVRInput::Controller::RTouch
        int controllerIndex = useButtonValue > 2 ? 1 : 2;

        // Here we correct the Index for direct Usage as Input for OVRInput.Get
        // After this line the Primary Button A/X (1/3 in Config) is 0 and the Secondary Button (2/4 in Config) is 1
        // Source: https://developer.oculus.com/documentation/unity/unity-ovrinput/
        useButtonValue = ((useButtonValue - 1) % 2) + 1;
        
        bool buttonPressed = GlobalNamespace::OVRInput::Get(useButtonValue, controllerIndex);
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
extern "C" void load() {
    il2cpp_functions::Init();

    // Load the config - make sure this is after il2cpp_functions::Init();
    getNyaConfig().Init(modInfo);

    // Init our custom config branch
    EndpointConfig::migrate(getNyaConfig().config);

    // Do config validation and modifications on start
    InitConfigOnStart();

    // Make local folders if they do not exist
    makeFolder();
    QuestUI::Init();

    QuestUI::Register::RegisterGameplaySetupMenu<Nya::ModifiersMenu*>(modInfo, "Nya");
    QuestUI::Register::RegisterModSettingsViewController<Nya::SettingsViewController*>(modInfo, "Nya");

    custom_types::Register::AutoRegister();

    Nya::getLoggerOld().info("Installing hooks...");
    // Install our hooks
    INSTALL_HOOK(Nya::getLoggerOld(), Pause);
    INSTALL_HOOK(Nya::getLoggerOld(), FixedUpdateHook);
    INSTALL_HOOK(Nya::getLoggerOld(), Results);
    INSTALL_HOOK(Nya::getLoggerOld(), Unpause);
    INSTALL_HOOK(Nya::getLoggerOld(), Restartbutton);
    INSTALL_HOOK(Nya::getLoggerOld(), MultiResults);
    INSTALL_HOOK(Nya::getLoggerOld(), MenuTransitionsHelper_RestartGame);
    INSTALL_HOOK(Nya::getLoggerOld(), SceneManager_Internal_ActiveSceneChanged);
    INSTALL_HOOK(Nya::getLoggerOld(), MainFlowCoordinator_DidActivate);

    Nya::getLoggerOld().info("Installed all hooks!");
}
