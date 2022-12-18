#pragma once

#include "Razix/Graphics/FrameGraph/RZPassNode.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            // http://www.cplusplus.com/articles/oz18T05o/
            // https://www.modernescpp.com/index.php/c-core-guidelines-type-erasure-with-templates

            /**
         * Wrapper around a virtual resource and it's entry point for creation and execution
         */
            class RZResourceEntry final
            {
                friend class RZFrameGraph;

                // Define custom types here

                /**
             * Concept is used to enforce certain rules on the Resources 
             */
                struct RZConcept
                {
                    virtual ~RZConcept() = default;

                    virtual void create(void *)  = 0;
                    virtual void destroy(void *) = 0;

                    virtual std::string toString() const = 0;
                };

                template<typename T>
                struct RZTypeResource : RZConcept
                {
                    RZTypeResource(typename T::CreateDesc &&, T &&);

                    void create(void *allocator) final;
                    void destroy(void *allocator) final;

                    std::string toString() const final;

                    // Resource creation descriptor
                    typename const T::CreateDesc descriptor;

                    // Resource handle
                    T resource;
                };

            public:
                RZResourceEntry(RZResourceEntry &&) noexcept            = default;
                RZResourceEntry &operator=(RZResourceEntry &&) noexcept = default;

                RZResourceEntry()                                   = delete;
                RZResourceEntry(const RZResourceEntry &)            = delete;
                RZResourceEntry &operator=(const RZResourceEntry &) = delete;

                void create(void *allocator);
                void destroy(void *allocator);

                std::string toString() const;

                uint32_t getVersion() const { return m_Version; }
                bool     isImported() const { return m_Imported; }
                bool     isTransient() const { return !m_Imported; }

                template<typename T>
                T &get();

                template<typename T>
                typename const T::CreateDesc &getDescriptor() const;

            private:
                const uint32_t             m_ID;
                std::unique_ptr<RZConcept> m_Concept;
                const bool                 m_Imported = false;
                uint32_t                   m_Version;
                RZPassNode                *m_Producer = nullptr;
                RZPassNode                *m_Last     = nullptr;

            private:
                template<typename T>
                RZResourceEntry(uint32_t id, typename T::CreateDesc &&, T &&, uint32_t version, bool imported = false);

                template<typename T>
                auto *getTypeResource() const;
            };

        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix