#pragma once

#include "reflectcpp/include/rfl.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Vector4.hpp"

namespace rfl {
    template <>
    struct Reflector<UnityEngine::Color> {
        struct ReflType {
            float r;
            float g;
            float b;
            float a;
        };

        static UnityEngine::Color to(ReflType const& value) noexcept {
            return {value.r, value.g, value.b, value.a};
        }

        static ReflType from(UnityEngine::Color const& value) noexcept {
            return {value.r, value.g, value.b, value.a};
        }
    };

    template <>
    struct Reflector<UnityEngine::Vector2> {
        struct ReflType {
            float x;
            float y;
        };

        static UnityEngine::Vector2 to(ReflType const& value) noexcept {
            return {value.x, value.y};
        }

        static ReflType from(UnityEngine::Vector2 const& value) noexcept {
            return {value.x, value.y};
        }
    };

    template <>
    struct Reflector<UnityEngine::Vector3> {
        struct ReflType {
            float x;
            float y;
            float z;
        };

        static UnityEngine::Vector3 to(ReflType const& value) noexcept {
            return {value.x, value.y, value.z};
        }

        static ReflType from(UnityEngine::Vector3 const& value) noexcept {
            return {value.x, value.y, value.z};
        }
    };

    template <>
    struct Reflector<UnityEngine::Vector4> {
        struct ReflType {
            float x;
            float y;
            float z;
            float w;
        };

        static UnityEngine::Vector4 to(ReflType const& value) noexcept {
            return {value.x, value.y, value.z, value.w};
        }

        static ReflType from(UnityEngine::Vector4 const& value) noexcept {
            return {value.x, value.y, value.z, value.w};
        }
    };
}
