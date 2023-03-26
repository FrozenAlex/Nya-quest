#include "Helpers/extension.hpp"

#include "Polyglot/LocalizedTextMeshProUGUI.hpp"
#include "TMPro/TMP_Text.hpp"


using namespace TMPro;
using namespace UnityEngine;
using namespace UnityEngine::UI;

namespace FSML::Helpers {
    void SetButtonText(Button* button, StringW text) {
            auto localizer = button->GetComponentInChildren<Polyglot::LocalizedTextMeshProUGUI*>(true);
            if (localizer)
                Object::DestroyImmediate(localizer);

            auto tmpUgui = button->GetComponentInChildren<TMP_Text*>();
            if (tmpUgui)
                tmpUgui->SetText(text);
    }

}