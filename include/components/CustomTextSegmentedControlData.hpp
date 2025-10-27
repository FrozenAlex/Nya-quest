#pragma once

#include "custom-types/shared/macros.hpp"
#include "UnityEngine/GameObject.hpp"

#include "HMUI/SegmentedControl.hpp"
#include "HMUI/SegmentedControlCell.hpp"
#include "HMUI/TextSegmentedControlCell.hpp"

___DECLARE_TYPE_WRAPPER_INHERITANCE(NyaUI, CustomTextSegmentedControlData, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, UnityEngine::MonoBehaviour, "NyaUI", { classof(HMUI::SegmentedControl::IDataSource*) }, 0, nullptr,
    DECLARE_INSTANCE_FIELD(HMUI::SegmentedControl*, segmentedControl);
    DECLARE_INSTANCE_FIELD(HMUI::SegmentedControlCell*, firstCellPrefab);
    DECLARE_INSTANCE_FIELD(HMUI::SegmentedControlCell*, lastCellPrefab);
    DECLARE_INSTANCE_FIELD(HMUI::SegmentedControlCell*, singleCellPrefab);
    DECLARE_INSTANCE_FIELD(HMUI::SegmentedControlCell*, middleCellPrefab);
    DECLARE_INSTANCE_FIELD(ArrayW<StringW>, texts);
    
    DECLARE_OVERRIDE_METHOD_MATCH(int, NumberOfCells, &HMUI::SegmentedControl::IDataSource::NumberOfCells);
    DECLARE_OVERRIDE_METHOD_MATCH(HMUI::SegmentedControlCell*, CellForCellNumber, &HMUI::SegmentedControl::IDataSource::CellForCellNumber, int idx);
    DECLARE_CTOR(ctor);
    DECLARE_DTOR(dtor);
    public:
        void set_texts(ArrayW<StringW> list);
        void add_text(StringW addedText);
        
        float fontSize = 4.0f;
        bool hideCellBackground = false;
        bool overrideCellSize = false;
        float padding = 2.0f;
    private:
        HMUI::TextSegmentedControlCell* InstantiateCell(UnityEngine::GameObject* prefab);
);