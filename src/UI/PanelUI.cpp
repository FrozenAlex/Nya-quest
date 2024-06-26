#include "UI/PanelUI.hpp"
#include "main.hpp"
#include "bsml/shared/BSML-Lite/Creation/Misc.hpp"
#include "logging.hpp"
using namespace Nya;

namespace Nya {
    void PanelUI::set_panelText(std::string string){
        text->set_text(string);
    }

    PanelUI::PanelUI(UnityEngine::Transform* parent, UnityEngine::Sprite* sprite, int index){
        UnityEngine::GameObject* mainCanvas = BSML::Lite::CreateCanvas();
        mainCanvas->set_active(false);
        this->index = index;
        image = BSML::Lite::CreateCanvas()->AddComponent<UnityEngine::UI::Image*>();
        image->set_material(Main::NyaFloatingUI->UINoGlow);
        image->set_sprite(sprite);

        text = UnityEngine::GameObject::New_ctor("text")->AddComponent<TMPro::TextMeshProUGUI*>();
        text->get_gameObject()->set_layer(1);
        text->set_isOverlay(true);
        text->set_fontSize(5.0f);
        text->set_alignment(TMPro::TextAlignmentOptions::Center);

        auto* collider = mainCanvas->AddComponent<UnityEngine::BoxCollider*>();
        hoverHint = BSML::Lite::AddHoverHint(collider->get_gameObject(), "hewoo");
        collider->set_size({13.0f, 13.0f, 0.0f});
        collider->set_name("gridcollider " + std::to_string(index));
        hoverHint->_hoverHintController = nullptr;

        UnityEngine::RectTransform* rectTransform = image->get_transform().cast<UnityEngine::RectTransform>();
        rectTransform->set_anchorMin(UnityEngine::Vector2(0.5f, 0.5f));
        rectTransform->set_anchorMax(UnityEngine::Vector2(0.5f, 0.5f));
        rectTransform->set_anchoredPosition({0.0f, 0.0f});
        rectTransform->set_sizeDelta({4, 4});
        rectTransform->set_localScale({4.0f, 4.0f, 0.0f});
        image->set_color(UnityEngine::Color::get_gray());

        mainCanvas->get_transform()->set_localScale({0.57f, 0.57f, 0.0f});
        rectTransform->SetParent(mainCanvas->get_transform(), false);
        text->get_transform()->SetParent(mainCanvas->get_transform(), false);
        collider->get_transform()->SetParent(mainCanvas->get_transform(), false);
        hoverHint->get_transform()->SetParent(mainCanvas->get_transform(), true);
        mainCanvas->get_transform()->SetParent(parent, false);

        hoverHint->set_enabled(true);
        mainCanvas->set_active(true);
    }

}

