#pragma once

#include "Razix/Graphics/API/RZTexture.h"
#include "Razix/Graphics/API/RZVertexBuffer.h"
#include "Razix/Graphics/API/RZIndexBuffer.h"
#include "Razix/Graphics/API/RZShader.h"

#include <glm/glm.hpp>

namespace Razix {
    namespace Graphics {

        struct RZVeretx2D
        {
            glm::vec4 Position;
            glm::vec4 Color;
            glm::vec2 UV;
        };
        
        /// We use a orthographic projection matrix for aspect ratio, this will be updated by the Renderer2D when resized, the layout(set = 0, binding = 0) will be shared by all the sprites in the engine to draw this

        /**
         * A Sprite is a 2D renderable that can be used to draw textures, particles effects, fonts and anything in 2D
         */
        class RAZIX_API RZSprite
        {
        public:
            /**
             * Creates an sprite with a given color
             * 
             * @param position The position of the sprite
             * @param rotation The rotation of the sprite
             * @param scale The dimensions of the sprite
             * @parma color The color of the sprite
             */
            RZSprite(const glm::vec2& position = glm::vec2(0.0f, 0.0f), const float& rotation = 0.0f, const glm::vec2& scale = glm::vec2(1.0f, 1.0f), const glm::vec4& color = glm::vec4(1.0f));
            /**
             * Crates a sprite with a given texture
             * 
             * @param texture The texture with which the sprite will drawn
             * @param position The position of the sprite
             * @param rotation The rotation of the sprite
             * @param scale The dimensions of the sprite
             */
            RZSprite(RZTexture2D* texture, const glm::vec2& position, const float& rotation, const glm::vec2& scale);
            virtual ~RZSprite() = default;

            void destroy();

            /* Gets the default UV values that is common for any sprite */
            static const std::array<glm::vec2, 4>& GetDefaultUVs();
            /* Gets the UV values given the minimum and maximum dimensions/range, useful while generating sprite sheet UVs */
            static const std::array<glm::vec2, 4>& RZSprite::GetUVs(const glm::vec2& min, const glm::vec2& max);

            void setSpriteSheet(const glm::vec2& cellIndex, const glm::vec2& sheetDimension);
            
            RZTexture2D* getTexture() const { return m_Texture; }
            RAZIX_INLINE void setTexture(RZTexture2D* texture) { m_Texture = texture; }
            RAZIX_INLINE glm::vec2 getPosition() const { return m_Position; }
            void setPosition(const glm::vec2& vector2) { m_Position = vector2; };
            RAZIX_INLINE glm::vec2 getScale() const { return m_Scale; }
            void setScale(const glm::vec2& scale) { m_Scale = scale; }
            RAZIX_INLINE const glm::vec4& getColour() const { return m_Color; }
            void setColour(const glm::vec4& color) { m_Color = color; updateVertexData(); }
            RAZIX_INLINE float getRotation() { return m_Rotation; }
            void setRotation(float rotation) { m_Rotation = rotation; updateVertexData(); }
            RAZIX_INLINE const std::array<glm::vec2, 4>& getUVs() const { return m_UVs; }

            // getter for shader, buffers and sets
            RZVertexBuffer* getVertexBuffer() { return m_VBO; }
            RZIndexBuffer* getIndexBuffer() { return m_IBO; }

            RZShader* getShader();
            RZDescriptorSet* getDescriptorSet(uint32_t index);

            //template<class Archive>
            //void load(Archive& archive)
            //{
            //   
            //}
            //
            //template<class Archive>
            //void save(Archive& archive) const
            //{
            //    archive(cereal::make_nvp("MeshName", Mesh->getName()));
            //}

        private:
            RZTexture2D*                    m_Texture;
            glm::vec2                       m_Position;// Do we really need them? Remove this, will be done via push_contants and updated via transform component
            float                           m_Rotation;// Do we really need them? Remove this, will be done via push_contants and updated via transform component
            glm::vec2                       m_Scale;   // Do we really need them? Remove this, will be done via push_contants and updated via transform component
            glm::vec4                       m_Color;
            std::array<glm::vec2, 4>        m_UVs;
            bool                            m_IsAnimated = false;
            bool                            m_IsTextured = false;

            RZShader*                       m_SpriteShader;
            RZShader*                       m_TexturedSpriteShader;
            //RZShader*                       m_SpriteSheetShader;

            RZVertexBuffer*                 m_VBO;
            RZIndexBuffer*                  m_IBO;
            std::vector<RZDescriptorSet*>   m_TexturedSpriteDescriptorSets;
            //std::vector<RZDescriptorSet*>   m_SpriteSheetDescriptorSets;

        private:
            void createBuffers();
            void updateVertexData();
            void updateDescriptorSets();
        };
    }
}
