#pragma once

#include "Razix/Core/RZLog.h"

#include "Razix/Graphics/RHI/API/RZDescriptorSet.h"
#include "Razix/Graphics/RHI/API/RZTextureData.h"

namespace Razix {
    namespace Graphics {

        /**
         * A class that manages Textures/Image resources for the Engine
         * @brief It manages creation and conversion of Image resources, also stores in a custom Engine Format depending on how it's being used
         */
        // TODO: Derive Texture from a RazixResource/Asset class this way it gets a resource/asset UUID + serialization by default
        // TODO: Calculate size properly for manually set texture data
        // TODO: Add support and Utility functions for sRGB textures
        class RAZIX_API RZTexture : public RZRoot
        {
        public:
            /* Default constructor, texture resource is done on demand */
            RZTexture() = default;
            /* Virtual destructor enables the API implementation to delete it's resources */
            virtual ~RZTexture() {}

            /**
             * Calculates the Mip Map count based on the Width and Height of the texture
             *
             * @param width     The width of the Texture
             * @param height    The height of the texture
             */
            static u32                         calculateMipMapCount(u32 width, u32 height);
            static RZTextureProperties::Format bitsToTextureFormat(u32 bits);

            // TODO: Move this to the RXBaseAsset class in later designs
            /* Releases the texture data and it's underlying resources */
            virtual void Release(bool deleteImage = true) = 0;

            /* Returns the name of the texture resource */
            std::string getName() const { return m_Name; }
            /* returns the width of the texture */
            u32 getWidth() const { return m_Width; }
            /* returns the height of the texture */
            u32 getHeight() const { return m_Height; }
            /* Gets the size of the texture resource */
            uint64_t getSize() const { return m_Size; }
            /* Returns the type of the texture */
            RZTextureProperties::Type getType() const { return m_TextureType; }
            void                      setType(RZTextureProperties::Type type) { m_TextureType = type; }
            /* Returns the internal format of the texture */
            RZTextureProperties::Format getFormat() const { return m_Format; }
            /* Returns the virtual path of the texture resource */
            std::string                    getPath() const { return m_VirtualPath; }
            RZTextureProperties::Filtering getFilterMode() { return m_FilterMode; }
            RZTextureProperties::Wrapping  getWrapMode() { return m_WrapMode; }

            /* Binds the Texture resource to the Pipeline */
            virtual void Bind(u32 slot) = 0;
            /* Unbinds the Texture resource from the pipeline */
            virtual void Unbind(u32 slot) = 0;

            /* Resize the texture */
            virtual void Resize(u32 width, u32 height RZ_DEBUG_NAME_TAG_E_ARG) {}

            /* Gets the handle to the underlying API texture instance */
            virtual void* GetHandle() const = 0;

            /* Generates the descriptor set for the texture */
            void             generateDescriptorSet();
            RZDescriptorSet* getDescriptorSet() { return m_DescriptorSet; }

        protected:
            std::string                    m_Name;          /* The name of the texture resource         */
            std::string                    m_VirtualPath;   /* The virtual path of the texture          */
            u32                            m_Width;         /* The width of the texture                 */
            u32                            m_Height;        /* The height of the texture                */
            uint64_t                       m_Size;          /* The size of the texture resource         */
            RZTextureProperties::Type      m_TextureType;   /* The type of this texture                 */
            RZTextureProperties::Format    m_Format;        /* The internal format of the texture data  */
            RZTextureProperties::Wrapping  m_WrapMode;      /* Wrap mode of the texture                 */
            RZTextureProperties::Filtering m_FilterMode;    /* Filtering mode of the texture data       */
            bool                           m_FlipX;         /* Flip the texture on X-axis during load   */
            bool                           m_FlipY;         /* Flip the texture on Y-axis during load   */
            RZDescriptorSet*               m_DescriptorSet; /* Descriptor set for the image             */
        };

        //-----------------------------------------------------------------------------------
        // Texture 2D
        //-----------------------------------------------------------------------------------

        /* 2D Texture interface */
        class RAZIX_API RZTexture2D : public RZTexture
        {
        public:
            /**
             * Create a 2D texture resource from the given data
             * 
             * @param name          The name of the texture resource 
             * @param width         The Width of the texture
             * @param height        The height of the texture
             * @param data          The pixel data to fill the texture with
             * @param format        The internal format of the texture
             * @param wrapMode      The wrapping mode of the texture
             * @param filterMode    The filtering mode of the texture
             * @returns Pointer to Texture object of the underlying API
             */
            static RZTexture2D* Create(RZ_DEBUG_NAME_TAG_F_ARG const std::string& name, u32 width, u32 height, void* data, RZTextureProperties::Format format, RZTextureProperties::Wrapping wrapMode = RZTextureProperties::Wrapping::CLAMP_TO_EDGE, RZTextureProperties::Filtering filterMode = RZTextureProperties::Filtering{});

            /**
             * Creates an Empty 2D array texture that can be used as an Render Target
             */
            static RZTexture2D* CreateArray(RZ_DEBUG_NAME_TAG_F_ARG const std::string& name, u32 width, u32 height, u32 numLayers, RZTextureProperties::Format format, RZTextureProperties::Wrapping wrapMode = RZTextureProperties::Wrapping::CLAMP_TO_EDGE, RZTextureProperties::Filtering filterMode = RZTextureProperties::Filtering{});

            /**
             * Create a 2D texture resource from the given file
             * 
             * @param filePath      The filepath to load the texture resource from
             * @param name          The name of the texture resource 
             * @param format        The internal format of the texture
             * @param wrapMode      The wrapping mode of the texture
             * @param filterMode    The filtering mode of the texture
             * @returns Pointer to Texture object of the underlying API
             */
            static RZTexture2D* CreateFromFile(
                RZ_DEBUG_NAME_TAG_F_ARG const std::string& filePath, const std::string& name, RZTextureProperties::Wrapping wrapMode = RZTextureProperties::Wrapping::CLAMP_TO_EDGE, RZTextureProperties::Filtering filterMode = RZTextureProperties::Filtering{});

            /**
             * Sets the pixel data for the 2D Texture 
             * 
             * @param pixels The pixel data to set
             */
            virtual void SetData(const void* pixels) = 0;
        };

        //-----------------------------------------------------------------------------------
        // Texture 3D
        //-----------------------------------------------------------------------------------
        /* 3D Texture interface */
        class RAZIX_API RZTexture3D : public RZTexture
        {
        public:
            /**
             * Create a 3D texture resource from the given data
             * 
             * @param name          The name of the texture resource 
             * @param width         The Width of the texture
             * @param height        The height of the texture
             * @param data          The pixel data to fill the texture with
             * @param format        The internal format of the texture
             * @param wrapMode      The wrapping mode of the texture
             * @param filterMode    The filtering mode of the texture
             * @returns Pointer to Texture object of the underlying API
             */
            static RZTexture3D* Create(RZ_DEBUG_NAME_TAG_F_ARG const std::string& name, u32 width, u32 height, u32 depth, RZTextureProperties::Format format, RZTextureProperties::Wrapping wrapMode = RZTextureProperties::Wrapping::CLAMP_TO_EDGE, RZTextureProperties::Filtering filterMode = RZTextureProperties::Filtering{});
        };

        //-----------------------------------------------------------------------------------
        // Cube Map Texture
        //-----------------------------------------------------------------------------------
        class RAZIX_API RZCubeMap : public RZTexture
        {
        public:
            static RZCubeMap* Create(RZ_DEBUG_NAME_TAG_F_ARG const std::string& name, u32 width, u32 height, bool enableMipsGeneration = false, RZTextureProperties::Wrapping wrapMode = RZTextureProperties::Wrapping::CLAMP_TO_EDGE, RZTextureProperties::Filtering filterMode = RZTextureProperties::Filtering{});

            void setMipLevel(u32 idx) { m_CurrentMipRenderingLevel = idx; }

        protected:
            u32  m_TotalMipLevels           = 5;
            u32  m_CurrentMipRenderingLevel = 0;
            bool m_GenerateMips             = false;
        };

        //-----------------------------------------------------------------------------------
        // Depth Texture
        //-----------------------------------------------------------------------------------

        class RAZIX_API RZDepthTexture : public RZTexture
        {
        public:
            static RZDepthTexture* Create(u32 width, u32 height);
        };

        //-----------------------------------------------------------------------------------
        // Render Texture
        //-----------------------------------------------------------------------------------

        class RAZIX_API RZRenderTexture : public RZTexture
        {
        public:
            static RZRenderTexture* Create(RZ_DEBUG_NAME_TAG_F_ARG
                                               u32 width,
                u32 height, RZTextureProperties::Format format = RZTextureProperties::Format::SCREEN, RZTextureProperties::Wrapping wrapMode = RZTextureProperties::Wrapping::REPEAT, RZTextureProperties::Filtering filterMode = RZTextureProperties::Filtering{});

            virtual int32_t ReadPixels(u32 x, u32 y) = 0;
        };
    }    // namespace Graphics
}    // namespace Razix