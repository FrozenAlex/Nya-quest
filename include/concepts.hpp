#pragma once

#include <concepts>
#include <type_traits>

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"


namespace FSML::Concepts {
    template<typename T, typename U>
    concept FSMLConvertible = std::is_convertible_v<T, U>;

    template<typename T>
    concept HasGameObject = !FSMLConvertible<T, UnityEngine::GameObject*> && requires (T a) { {a->get_gameObject() } -> FSMLConvertible<UnityEngine::GameObject*>; };

    template<typename T>
    concept HasTransform = !FSMLConvertible<T, UnityEngine::Transform*> && requires (T a) { {a->get_transform() } -> FSMLConvertible<UnityEngine::Transform*>; };
}