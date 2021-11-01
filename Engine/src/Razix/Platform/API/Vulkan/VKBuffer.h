#pragma once

#ifdef RAZIX_RENDER_API_VULKAN 

#include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {

        /* Vulkan buffer to hold and transfer data */
        class VKBuffer
        {
        public: 
            /**
             * Creates a buffer with the usage type and of required size
             * 
             * @param usage What will the buffer used for? Storage, transfer staging or something else
             * @param size The size of the buffer
             * @param data The data with which the buffer will be filled with
             */
            VKBuffer(VkBufferUsageFlags usage, uint32_t size, const void* data);
            /* Creates an empty buffer to be mapped with data later */
            VKBuffer();
            ~VKBuffer() {}

            /* Destroy the buffer and it's memory */
            void destroy();
            
            /* Maps the buffer to an external location on the HOST to copy data to it */
            void map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
            /* Unmaps the memory to which the buffer was mapped to */
            void unMap();
            /* Clears out the buffer allocation */
            void flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
            
            /* Sets the buffer with new given data */
            void setData(uint32_t size, const void* data);         

            /* Gets the reference to the Vulkan buffer object */
            inline const VkBuffer& getBuffer() const { return m_Buffer; }
            /* Tells how the buffer is being used currently */
            inline const VkBufferUsageFlags& getUsage() const { return m_UsageFlags; }
            /* Sets the usage of the buffer */
            inline void setUsage(VkBufferUsageFlags flags) { m_UsageFlags = flags; }

        private:
            VkBuffer                m_Buffer;               /* handle to the Vulkan buffer          */
            VkDeviceMemory          m_Memory;               /* Handle to the buffer memory          */
            VkDeviceSize            m_Size;                 /* The size of the buffer               */
            VkDescriptorBufferInfo  m_DesciptorBufferInfo;  /* The buffer description info          */
            VkBufferUsageFlags      m_UsageFlags;           /* Buffer usage description             */
            void*                   m_Mapped = nullptr;     /* The HOST mapped region of the buffer */

        private:
            /**
             * Initializes the buffer with the given size, usage and data
             * 
             * @param data The data with which the buffer will be filled with
             */
            void init(const void* data);
        };

    }
}
#endif