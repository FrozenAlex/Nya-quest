#include "Helpers/creation.hpp"
#include "Helpers/getters.hpp"

#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Canvas.hpp"
#include "UnityEngine/CanvasGroup.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/AdditionalCanvasShaderChannels.hpp"
#include "UnityEngine/RenderMode.hpp"
#include "VRUIControls/VRGraphicRaycaster.hpp"

using namespace UnityEngine;
using namespace TMPro;

namespace FSML::Helpers {
    TMP_Text* CreateText(System::Type* type, UnityEngine::Transform* parent, StringW text, Vector2 anchoredPosition, Vector2 sizeDelta) {
        auto gameObj = GameObject::New_ctor("CustomUIText");
        gameObj->SetActive(false);

        auto textComponent = reinterpret_cast<TMP_Text*>(gameObj->AddComponent(type));
        textComponent->set_font(GetMainTextFont());
        textComponent->set_fontSharedMaterial(GetMainUIFontMaterial());
        textComponent->get_rectTransform()->SetParent(parent, false);
        textComponent->set_text(text);
        textComponent->set_fontSize(4);
        textComponent->set_color({1.0f, 1.0f, 1.0f, 1.0f});

        auto rectTransform = textComponent->get_rectTransform();
        rectTransform->set_anchorMin({0.5f, 0.5f});
        rectTransform->set_anchorMax({0.5f, 0.5f});
        rectTransform->set_sizeDelta(sizeDelta);
        rectTransform->set_anchoredPosition(anchoredPosition);

        gameObj->SetActive(true);
        return textComponent;
    }

    SafePtrUnity<Canvas> canvasTemplate;
    HMUI::ViewController* CreateViewController(System::Type* type) {
        if (!canvasTemplate)
            canvasTemplate = Resources::FindObjectsOfTypeAll<Canvas*>().FirstOrDefault([](auto x) { return x->get_name() == "DropdownTableView"; });
        
        auto go = GameObject::New_ctor(type->get_Name());
        auto cv = go->AddComponent<Canvas*>();

        cv->set_additionalShaderChannels(canvasTemplate->get_additionalShaderChannels());
        cv->set_overrideSorting(canvasTemplate->get_overrideSorting());
        cv->set_pixelPerfect(canvasTemplate->get_pixelPerfect());
        cv->set_referencePixelsPerUnit(canvasTemplate->get_referencePixelsPerUnit());
        cv->set_renderMode(canvasTemplate->get_renderMode());
        cv->set_scaleFactor(canvasTemplate->get_scaleFactor());
        cv->set_sortingLayerID(canvasTemplate->get_sortingLayerID());
        cv->set_sortingOrder(canvasTemplate->get_sortingOrder());
        cv->set_worldCamera(canvasTemplate->get_worldCamera());

        go->AddComponent<VRUIControls::VRGraphicRaycaster*>()->physicsRaycaster = GetPhysicsRaycasterWithCache();
        go->AddComponent<CanvasGroup*>();
        auto vc = reinterpret_cast<HMUI::ViewController*>(go->AddComponent(type));

        vc->get_rectTransform()->set_anchorMin({0, 0});
        vc->get_rectTransform()->set_anchorMax({1, 1});
        vc->get_rectTransform()->set_sizeDelta({0, 0});
        vc->get_rectTransform()->set_anchoredPosition({0, 0});
        go->SetActive(false);
        return vc;
    }

    HMUI::FlowCoordinator* CreateFlowCoordinator(System::Type* type) {
        auto flow = reinterpret_cast<HMUI::FlowCoordinator*>(GameObject::New_ctor(type->get_Name())->AddComponent(type));
        flow->baseInputModule = GetMainFlowCoordinator()->baseInputModule;
        return flow;
    }

    HMUI::HoverHint* AddHoverHint(UnityEngine::GameObject* object, StringW text) {
        auto hintText = object->AddComponent<HMUI::HoverHint*>();
        hintText->set_text(text);
        hintText->hoverHintController = GetHoverHintController();
        return hintText;
    }
}