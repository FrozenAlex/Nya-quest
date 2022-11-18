#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Physics.hpp"
#include "UnityEngine/RaycastHit.hpp"
#include "UnityEngine/Collider.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UI/HoverClickHelper.hpp"
#include "GlobalNamespace/VRController.hpp"
#include "HMUI/ModalView.hpp"
#include "VRUIControls/VRPointer.hpp"
#include "UnityEngine/Shader.hpp"
#include "NyaFloatingUI.hpp"
#include "Utils/Utils.hpp"
#include "UnityEngine/Space.hpp"
#include "main.hpp"

using namespace Nya;

DEFINE_TYPE(Nya, HoverClickHelper);

namespace Nya {
    
    void HoverClickHelper::Awake(){
        isHit = false;
        hintController = QuestUI::ArrayUtil::First(UnityEngine::Resources::FindObjectsOfTypeAll<HMUI::HoverHintController*>());
        panelUI = nullptr;
        notClickedModal = true;
        outOfRange = false;
        currentCollider = nullptr;
        grabbingHandle = false;
        hoveringHandle = false;
        mover = nullptr;
        hoverHandleMat = UnityEngine::Material::New_ctor(UnityEngine::Shader::Find("Hidden/Internal-DepthNormalsTexture"));
    }

    void HoverClickHelper::Init(VRUIControls::VRPointer* pointer, UnityEngine::GameObject* handle){
        vrPointer = pointer;
        handleTransform = handle;
        origHandleMat = handle->GetComponent<UnityEngine::MeshRenderer*>()->get_material();
        this->targetPosition = UnityEngine::Vector3::get_zero();
        this->targetRotation = UnityEngine::Quaternion::_get_identityQuaternion();
    }

    void HoverClickHelper::Update(){
        // Lerping
        auto dt = UnityEngine::Time::get_deltaTime();

        auto screenTransform = this->handleTransform->get_transform()->get_parent()->get_transform();
        auto previousRotation = screenTransform->get_rotation();
        auto previousPosition = screenTransform->get_position();
        
        if(!grabbingHandle) {
            screenTransform->set_rotation(UnityEngine::Quaternion::Slerp(previousRotation, this->targetRotation, dt*7));
            screenTransform->set_position(UnityEngine::Vector3::Lerp(previousPosition, this->targetPosition, dt*7));
        } else {
            targetPosition = previousPosition;
            targetRotation = previousRotation;
        }
        
        // VR conroller is sometimes null after leaving multiplayer?
        if (!vrPointer  || !vrPointer->m_CachedPtr.m_value) {
            return;
        }
        auto vrController = vrPointer->get_vrController();
        if (!vrController  || !vrController->m_CachedPtr.m_value) {
            return;
        }

        // Ray cast from the controller forward
        if(UnityEngine::Physics::Raycast(vrController->get_position(), vrController->get_forward(), hit, 100)){
            // If the needed collider is found and not grabbing handle
            if(static_cast<std::string>(hit.get_collider()->get_name()).substr(0, 12).compare("gridcollider") == 0 && !grabbingHandle){
                if (isHit && currentCollider && currentCollider != hit.get_collider()->get_transform()) {
                    panelUI->image->GetComponent<UnityEngine::UI::Image *>()->set_color(UnityEngine::Color::get_gray());
                    hintController->hoverHintPanel->Hide();
                    panelUI = nullptr;
                    isHit = false;
                }
            }
            else if (isHit){
                panelUI->image->set_color(UnityEngine::Color::get_gray());
                hintController->hoverHintPanel->Hide();
                panelUI = nullptr;
                isHit = false;
            }
            if(hit.get_collider()->get_transform() == handleTransform->get_transform()){
                if(!hoveringHandle){
                    hoveringHandle = true;
                    handleTransform->GetComponent<UnityEngine::MeshRenderer*>()->set_material(hoverHandleMat);
                }
            }
            else if(hoveringHandle && !grabbingHandle){
                hoveringHandle = false;
                handleTransform->GetComponent<UnityEngine::MeshRenderer*>()->set_material(origHandleMat);
            }
        }
        else {
            if (isHit && Main::NyaFloatingUI != nullptr && Main::NyaFloatingUI->settingsMenu != nullptr && !Main::NyaFloatingUI->settingsMenu->isShown()){
                panelUI->image->GetComponent<UnityEngine::UI::Image*>()->set_color(UnityEngine::Color::get_gray());
                hintController->hoverHintPanel->Hide();
                panelUI = nullptr;
                isHit = false;
            }
            if(hoveringHandle && !grabbingHandle){
                hoveringHandle = false;
                handleTransform->GetComponent<UnityEngine::MeshRenderer*>()->set_material(origHandleMat);
            }
        }
        if (isHit && grabbingController && !modalLocked && notClickedModal && !justClosedModal && !outOfRange){
            grabbingController = vrController;
            triggerPressed = true;
            notClickedModal = false;
            panelUI->image->set_color(UnityEngine::Color::get_gray());
            // Main::NyaFloatingUI->updateGridNotesInfo(panelUI->index);
            Main::NyaFloatingUI->settingsMenu->Show();
            hintController->hoverHintPanel->Hide();
            panelUI = nullptr;
            isHit = false;
            modalLocked = true;
        }
        if (!Main::NyaFloatingUI->settingsMenu->isShown() && !modalLocked && !notClickedModal) notClickedModal = true;
        if (justClosedModal) modalLocked = false;
        if (grabbingController) outOfRange = true;
    }

    void HoverClickHelper::LateUpdate(){
        if (vrPointer == nullptr) {
            this->UpdatePointer();
            return;
        }

        // VR conroller is sometimes null after leaving multiplayer?
        auto vrController = vrPointer->get_vrController();
        if (!vrController || !vrController->m_CachedPtr.m_value) {
            DEBUG("VR controller is null, trying to get a new one");
            this->UpdatePointer();
            return;
        }
        
        if(!triggerPressed && !isHit && vrController->get_triggerValue() > 0.9f && !(hoveringHandle || grabbingHandle)){
            // TODO: Fix modal hiding
            //    Main::NyaFloatingUI->settingsMenu->Hide();
            //    justClosedModal = true;
        }
        if (vrController->get_triggerValue() > 0.9f && !triggerPressed){
            grabbingController = vrController;
            triggerPressed = true;
            if ((hit.get_collider() && hit.get_collider()->get_transform() == handleTransform->get_transform())){
                handleTransform->GetComponent<UnityEngine::MeshRenderer*>()->set_material(hoverHandleMat);
                grabbingHandle = true;
                hoveringHandle = true;
            }
        }

        // If trigger is released
        if (triggerPressed && grabbingController != nullptr && grabbingController->get_triggerValue() < 0.1f){
            // If we were dragging the handle, release and save
            if (grabbingHandle){
                auto* screenTransform = handleTransform->get_transform()->get_parent();
                // Save coordinates to config
                Main::NyaFloatingUI->updateCoordinates(screenTransform);

                this->targetPosition = screenTransform->get_position();
                this->targetRotation = screenTransform->get_rotation();
            }
            
            grabbingController = nullptr;
            triggerPressed = false;
            justClosedModal = false;
            outOfRange = false;
            grabbingHandle = false;
        } 

        // If is hit and the thing is not shown then not hit
        if (isHit && Main::NyaFloatingUI != nullptr && Main::NyaFloatingUI->settingsMenu->isShown()) isHit = false;
        
    }

    void HoverClickHelper::resetBools(){
        justClosedModal = false;
        notClickedModal = true;
        triggerPressed = false;
        modalLocked = false;
        grabbingController = nullptr;
        outOfRange = false;
    }

    HoverClickHelper* addHoverClickHelper(VRUIControls::VRPointer* pointer, UnityEngine::GameObject* handle, QuestUI::FloatingScreen* screen){
        auto* helper = screen->get_gameObject()->AddComponent<HoverClickHelper*>();
        helper->Init(pointer, handle);
        return helper;
    }

    void HoverClickHelper::UpdatePointer(){
        auto* pointer = Utils::getAnyPointerWithController();
        vrPointer = pointer;
        // Get the last mover
        if (this->mover && this->mover->m_CachedPtr.m_value) {
            UnityEngine::Object::DestroyImmediate(this->mover);
        }
        if (vrPointer != nullptr) {
            this->mover = pointer->get_gameObject()->AddComponent<QuestUI::FloatingScreenMoverPointer*>();
            // UIScreen
            this->mover->Init(this->get_gameObject()->GetComponent<QuestUI::FloatingScreen*>(), pointer);
        }
    }

    void HoverClickHelper::LookAtCamera(){
        auto mainCamera = UnityEngine::Camera::get_main();
        auto screenTransform = this->handleTransform->get_transform()->get_parent()->get_transform();
        auto newRotation = UnityEngine::Quaternion::LookRotation(screenTransform->get_position() - mainCamera->get_transform()->get_position());
        this->targetRotation = newRotation;
        Main::NyaFloatingUI->updateCoordinates(screenTransform->get_position(), this->targetRotation.get_eulerAngles());
    }

    void HoverClickHelper::SetUpRight (){
        auto mainCamera = UnityEngine::Camera::get_main();
        auto screenTransform = this->handleTransform->get_transform()->get_parent()->get_transform();
        auto newRotation = UnityEngine::Quaternion::Euler(0.0, screenTransform->get_rotation().get_eulerAngles().y, 0.0);
       
        this->targetRotation = newRotation;
        Main::NyaFloatingUI->updateCoordinates(screenTransform->get_position(), this->targetRotation.get_eulerAngles());
    }

    void HoverClickHelper::SetPosition(UnityEngine::Vector3 position, UnityEngine::Quaternion rotation, bool lerp){
        auto screenTransform = this->handleTransform->get_transform()->get_parent()->get_transform();
        if (!lerp) {
            screenTransform->set_rotation(rotation);
            screenTransform->set_position(position);
        }

        this->targetRotation = rotation;
        this->targetPosition = position;
    }
}
