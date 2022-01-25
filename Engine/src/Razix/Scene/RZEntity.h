#pragma once

#include "Razix/Core/RZUUID.h"

#include <entt.hpp>

namespace Razix {
    
    // Forward declaration
    class RZScene;

    /* An entity represents the basic building block of a scene, they are the objects the exist in the game world */
    class RAZIX_API RZEntity
    {
    public:
        RZEntity() = default;
        RZEntity(entt::entity handle, RZScene* scene);
        ~RZEntity() {}

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            RAZIX_CORE_ASSERT((!HasComponent<T>()), "Entity already has component!");
            return m_Scene->m_Registry.emplace<T>(m_Entity, std::forward<Args>(args)...);
            // TODO: callback to the scene when a component is added with the type of the component that was added
            //m_Scene->OnComponentAdded<T>(*this, component);
        }

        template <typename T, typename... Args>
        T& GetOrAddComponent(Args&&... args)
        {
            return m_Scene->m_Registry.get_or_emplace<T>(m_Entity, std::forward<Args>(args)...);
        }

        template <typename T, typename... Args>
        void AddOrReplaceComponent(Args&&... args)
        {
            m_Scene->m_Registry.emplace_or_replace<T>(m_Entity, std::forward<Args>(args)...);
        }

        template<typename T>
        T& GetComponent()
        {
            RAZIX_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            return m_Scene->m_Registry.get<T>(m_Entity);
        }

        template <typename T>
        T* TryGetComponent()
        {
            return m_Scene->m_Registry.try_get<T>(m_Entity);
        }

        template<typename T>
        bool HasComponent()
        {
            return m_Scene->m_Registry.has<T>(m_Entity);
        }

        template<typename T>
        void RemoveComponent()
        {
            RAZIX_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            m_Scene->m_Registry.remove<T>(m_Entity);
        }

        // TODO: Add active component to check if the entity is active or not in the editor/world/runtime etc.
        // TODO: Add Hierarchy model
        // TODO: Add operator overloads and getter/setters

        operator bool() const { return m_Entity != entt::null; }
        operator entt::entity() const { return m_Entity; }
        operator uint32_t() const { return (uint32_t) m_Entity; }

    private:
        entt::entity    m_Entity{entt::null};
        RZUUID*         m_UUID;
        RZScene*        m_Scene = nullptr;
    };
}

