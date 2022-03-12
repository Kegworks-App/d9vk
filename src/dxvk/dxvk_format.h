#pragma once

#include <array>
#include <initializer_list>

#include "dxvk_include.h"

namespace dxvk {

  class DxvkAdapter;

  /**
   * \brief DXVK format enum
   */
  enum class DxvkFormat : uint32_t {
    Unknown         = 0,
    /* RGBA */
    RGBA4un         = 1,
    RGB5A1un        = 2,
    RGBA8un         = 3,
    RGBA8srgb       = 4,
    RGBA8ui         = 5,
    RGBA8us         = 6,
    RGBA8sn         = 7,
    RGBA8si         = 8,
    RGBA8ss         = 9,
    RGB10A2un       = 10,
    RGB10A2ui       = 11,
    RGB10A2us       = 12,
    RGB10A2sn       = 13,
    RGB10A2si       = 14,
    RGB10A2ss       = 15,
    RGBA16sf        = 16,
    RGBA16un        = 17,
    RGBA16ui        = 18,
    RGBA16us        = 19,
    RGBA16sn        = 20,
    RGBA16si        = 21,
    RGBA16ss        = 22,
    RGBA32sf        = 23,
    RGBA32ui        = 24,
    RGBA32si        = 25,
    /* RGBX */
    RGBX4un         = 26,
    RGB5X1un        = 27,
    RGBX8un         = 28,
    RGBX8srgb       = 29,
    /* BGRA */
    BGRA4un         = 30,
    BGR5A1un        = 31,
    BGRA8un         = 32,
    BGRA8srgb       = 33,
    BGRA8ui         = 34,
    BGR10A2un       = 35,
    BGR10A2ui       = 36,
    /* BGRX */
    BGRX4un         = 37,
    BGR5X1un        = 38,
    BGRX8un         = 39,
    BGRX8srgb       = 40,
    /* RGB */
    R5G6B5un        = 41,
    RGB9E5uf        = 42,
    RGB11uf         = 43,
    RGB32sf         = 44,
    RGB32ui         = 45,
    RGB32si         = 46,
    /* BGR */
    B5G6R5un        = 47,
    /* RG */
    RG8un           = 48,
    RG8ui           = 49,
    RG8us           = 50,
    RG8sn           = 51,
    RG8si           = 52,
    RG8ss           = 53,
    RG16sf          = 54,
    RG16un          = 55,
    RG16ui          = 56,
    RG16us          = 57,
    RG16sn          = 58,
    RG16si          = 59,
    RG16ss          = 60,
    RG32sf          = 61,
    RG32ui          = 62,
    RG32si          = 63,
    RG4un           = 89, // TODO
    /* R */
    R8un            = 64,
    R8ui            = 65,
    R8us            = 66,
    R8sn            = 67,
    R8si            = 68,
    R8ss            = 69,
    R16sf           = 70,
    R16un           = 71,
    R16ui           = 72,
    R16us           = 73,
    R16sn           = 74,
    R16si           = 75,
    R16ss           = 76,
    R32sf           = 77,
    R32ui           = 78,
    R32si           = 79,
    R8srgb          = 88, // TODO
    /* A */
    A8un            = 80,
    /* D / DS */
    S8              = 81,
    D16             = 82,
    D16S8           = 83,
    D24             = 84,
    D24S8           = 85,
    D32             = 86,
    D32S8           = 87,
    /* Block-compressed */
    BC1un           = 96,
    BC1srgb         = 97,
    BC2un           = 98,
    BC2srgb         = 99,
    BC3un           = 100,
    BC3srgb         = 101,
    BC4un           = 102,
    BC4sn           = 103,
    BC5un           = 104,
    BC5sn           = 105,
    BC6Huf          = 106,
    BC6Hsf          = 107,
    BC7un           = 108,
    BC7srgb         = 109,
    /* Subsampled */
    RGBG8un_422     = 112,
    GRGB8un_422     = 113,
    /* Multi-plane */
    NV12_420        = 120,

    /* Dummy value */
    Count
  };

  /**
   * \brief Format property flags
   */
  enum class DxvkFormatFlag {
    BlockCompressed = 0,  ///< Image format is block compressed
    SampledUInt     = 1,  ///< Sampled type is an unsigned integer type
    SampledSInt     = 2,  ///< Sampled type is a signed integer type
    ColorSpaceSrgb  = 3,  ///< Non-linear SRGB color format
    MultiPlane      = 4,  ///< Multi-plane format
  };
  
  using DxvkFormatFlags = Flags<DxvkFormatFlag>;

  /**
   * \brief Format type
   */
  enum class DxvkFormatType : uint32_t {
    None,     ///< Undefined
    Float,    ///< Float -> Float
    Srgb,     ///< Uint -> [0..1] non-linear
    Unorm,    ///< Uint -> [0..1]
    Uint,     ///< Uint -> Uint
    Uscaled,  ///< Uint -> Float
    Snorm,    ///< Sint -> [-1..1]
    Sint,     ///< Sint -> Sint
    Sscaled,  ///< Sint -> Float
  };

  /**
   * \brief Planar format info
   */
  struct DxvkPlaneFormatInfo {
    /// Byte size of a pixel in the current plane
    VkDeviceSize elementSize = 0;
    /// Number of image pixels covered by a
    /// single pixel in the current plane
    VkExtent2D blockSize = { 1, 1 };
  };

  static constexpr VkComponentMapping FormatSwizzleDefault = {
    VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
    VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
  };

  /**
   * \brief Format properties
   */
  struct DxvkFormatProperties {
    DxvkFormat          format      = DxvkFormat::Unknown;    ///< Format
    DxvkFormatType      type        = DxvkFormatType::None;   ///< Element type
    VkFormat            vkFormat    = VK_FORMAT_UNDEFINED;    ///< Vulkan format
    VkImageAspectFlags  aspectMask  = 0;                      ///< Image aspect mask
    VkDeviceSize        elementSize = 0;                      ///< Element size in bytes
    VkExtent3D          blockSize   = { 1, 1, 1 };            ///< Compressed block size
    VkComponentMapping  rSwizzle    = FormatSwizzleDefault;   ///< Read-only component mapping
    VkComponentMapping  wSwizzle    = FormatSwizzleDefault;   ///< Write-only component mapping
    DxvkPlaneFormatInfo planes[3];                            ///< Plane info
    VkFormatProperties  support;                              ///< Support flags

    bool isCompressed() const {
      // No compressed or subsampled formats known that
      // have blocks in only the y or z directions
      return blockSize.width > 1;
    }

    bool isMultiPlane() const {
      // PLANE_0 is present in all planar formats
      return !!(aspectMask & VK_IMAGE_ASPECT_PLANE_0_BIT);
    }
  };


  /**
   * \brief Format map
   *
   * Provides DXVK format to Vulkan format mappings, as
   * well as additional information for each format.
   */
  class DxvkFormatMap {

  public:

    DxvkFormatMap(DxvkAdapter* adapter);
    ~DxvkFormatMap();

    /**
     * \brief Looks up format
     *
     * \param [in] format Format to look up
     * \returns Reference to format property struct
     */
    const DxvkFormatProperties& lookup(DxvkFormat format) const {
      return m_formats[uint32_t(format)];
    }

  private:

    std::array<DxvkFormatProperties, uint32_t(DxvkFormat::Count)> m_formats;

  };


  /**
   * \brief Image format list
   *
   * Small container that stores unique formats of
   * the given kind. Supports arbitrary format types.
   */
  template<typename T>
  class DxvkTypedFormatList {

  public:

    DxvkTypedFormatList() { }
    DxvkTypedFormatList(const std::initializer_list<T>& formats) {
      for (auto f : formats)
        add(f);
    }

    bool add(T format) {
      if (contains(format))
        return true;

      if (m_count < m_formats.size()) {
        m_formats[m_count++] = format;
        return true;
      }

      return false;
    }

    uint32_t count() const {
      return m_count;
    }

    const T* formats() const {
      return m_formats.data();
    }

    bool contains(T format) const {
      for (uint32_t i = 0; i < m_count; i++) {
        if (m_formats[i] == format)
          return true;
      }

      return false;
    }

    T operator [] (uint32_t idx) const {
      return m_formats[idx];
    }

  private:

    uint32_t          m_count = 0;
    std::array<T, 15> m_formats;

  };

  using DxvkFormatList = DxvkTypedFormatList<DxvkFormat>;
  using DxvkVulkanFormatList = DxvkTypedFormatList<VkFormat>;

  /**
   * \brief Format info structure
   * 
   * Provides some useful information
   * about a Vulkan image format. 
   */
  struct DxvkFormatInfo {
    /// Size of an element in this format. For compressed
    /// formats, this is the size of a block, in bytes.
    VkDeviceSize elementSize = 0;
    
    /// Available image aspect flags
    VkImageAspectFlags aspectMask = 0;
    
    /// Some other format info flags
    DxvkFormatFlags flags = 0;
    
    /// Size, in pixels, of a compressed block. For
    /// non-block formats, all these values are 1.
    VkExtent3D blockSize = { 1, 1, 1 };

    /// Plane info for multi-planar formats
    std::array<DxvkPlaneFormatInfo, 3> planes;
  };
  
  
  
  const DxvkFormatInfo* imageFormatInfo(VkFormat format);
  
}