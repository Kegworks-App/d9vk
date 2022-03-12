#include "d3d9_format.h"

namespace dxvk {

  // It is also worth noting that the msb/lsb-ness is flipped between VK and D3D9.
  D3D9_VK_FORMAT_MAPPING ConvertFormatUnfixed(D3D9Format Format) {
    switch (Format) {
      case D3D9Format::Unknown: return {};

      case D3D9Format::R8G8B8: return {}; // Unsupported

      case D3D9Format::A8R8G8B8: return {
        DxvkFormat::BGRA8un,
        DxvkFormat::BGRA8srgb,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::X8R8G8B8: return {
        DxvkFormat::BGRX8un,
        DxvkFormat::BGRX8srgb,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::R5G6B5: return {
        DxvkFormat::B5G6R5un,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT};

      case D3D9Format::X1R5G5B5: return {
        DxvkFormat::BGR5X1un,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::A1R5G5B5: return {
        DxvkFormat::BGR5A1un,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::A4R4G4B4: return {
        DxvkFormat::BGRA4un,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::R3G3B2: return {}; // Unsupported

      case D3D9Format::A8: return {
        DxvkFormat::A8un, // TODO: zero swizzle?
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::A8R3G3B2: return {}; // Unsupported

      case D3D9Format::X4R4G4B4: return {
        DxvkFormat::BGRX4un,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::A2B10G10R10: return {
        DxvkFormat::RGB10A2un, // The A2 is out of place here. This should be investigated.
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::A8B8G8R8: return {
        DxvkFormat::RGBA8un,
        DxvkFormat::RGBA8srgb,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::X8B8G8R8: return {
        DxvkFormat::RGBX8un,
        DxvkFormat::RGBX8srgb,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::G16R16: return {
        DxvkFormat::RG16un, // TODO: one swizzles?
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::A2R10G10B10: return {
        DxvkFormat::BGR10A2un,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::A16B16G16R16: return {
        DxvkFormat::RGBA16un,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::A8P8: return {}; // Unsupported

      case D3D9Format::P8: return {}; // Unsupported

      case D3D9Format::L8: return {
        DxvkFormat::R8un, // TODO swizzle
        DxvkFormat::R8srgb,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::A8L8: return {
        DxvkFormat::RG8un, // TODO swizzle
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::A4L4: return {
        DxvkFormat::RG4un, // TODO swizzles
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::V8U8: return {
        DxvkFormat::RG8sn, // TODO swizzles
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::L6V5U5: return {
        // Any PACK16 format will do...
        DxvkFormat::B5G6R5un, // TODO swizzle
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT,
        { D3D9ConversionFormat_L6V5U5, 1u,
        // Convert -> float (this is a mixed snorm and unorm type)
          DxvkFormat::RGBA16sf } };

      case D3D9Format::X8L8V8U8: return {
        DxvkFormat::BGRA8un, // TODO swizzles
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT,
        { D3D9ConversionFormat_X8L8V8U8, 1u,
        // Convert -> float (this is a mixed snorm and unorm type)
          DxvkFormat::RGBA16sf } };

      case D3D9Format::Q8W8V8U8: return {
        DxvkFormat::RGBA8sn,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::V16U16: return {
        DxvkFormat::RG16sn, // TODO swizzle
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::A2W10V10U10: return {
        DxvkFormat::RGB10A2un, // TODO: check swizzles
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT,
        { D3D9ConversionFormat_A2W10V10U10, 1u,
        // Convert -> float (this is a mixed snorm and unorm type)
          DxvkFormat::RGBA16sf } };

      case D3D9Format::UYVY: return {
        DxvkFormat::BGRA8un,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT,
        { D3D9ConversionFormat_UYVY, 1u }
      };

      case D3D9Format::R8G8_B8G8: return {
        DxvkFormat::RGBG8un_422, // This format may have been _SCALED in DX9.
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::YUY2: return {
        DxvkFormat::BGRA8un,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT,
        { D3D9ConversionFormat_YUY2, 1u }
      };

      case D3D9Format::G8R8_G8B8: return {
        DxvkFormat::GRGB8un_422, // This format may have been _SCALED in DX9.
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::DXT1: return {
        DxvkFormat::BC1un,
        DxvkFormat::BC1srgb,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::DXT2: return {
        DxvkFormat::BC2un,
        DxvkFormat::BC2srgb,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::DXT3: return {
        DxvkFormat::BC2un,
        DxvkFormat::BC2srgb,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::DXT4: return {
        DxvkFormat::BC3un,
        DxvkFormat::BC3srgb,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::DXT5: return {
        DxvkFormat::BC3un,
        DxvkFormat::BC3srgb,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::D16_LOCKABLE: return {
        DxvkFormat::D16,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_DEPTH_BIT };

      case D3D9Format::D32: return {
        DxvkFormat::D32,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_DEPTH_BIT };

      case D3D9Format::D15S1: return {}; // Unsupported (everywhere)

      case D3D9Format::D24S8: return {
        DxvkFormat::D24S8,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT };

      case D3D9Format::D24X8: return {
        DxvkFormat::D24S8,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_DEPTH_BIT };

      case D3D9Format::D24X4S4: return {}; // Unsupported (everywhere)

      case D3D9Format::D16: return {
        DxvkFormat::D16,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_DEPTH_BIT };

      case D3D9Format::D32F_LOCKABLE: return {
        DxvkFormat::D32,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_DEPTH_BIT };

      case D3D9Format::D24FS8: return {
        DxvkFormat::D24S8,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT };

      case D3D9Format::D32_LOCKABLE: return {
        DxvkFormat::D32,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_DEPTH_BIT };

      case D3D9Format::S8_LOCKABLE: return {
        DxvkFormat::S8,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_STENCIL_BIT };

      case D3D9Format::L16: return {
        DxvkFormat::R16un, // TODO: swizzles
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::VERTEXDATA: return {
        DxvkFormat::R8ui,
        DxvkFormat::Unknown,
        0 };

      case D3D9Format::INDEX16: return {
        DxvkFormat::R16ui,
        DxvkFormat::Unknown,
        0 };

      case D3D9Format::INDEX32: return {
        DxvkFormat::R32ui,
        DxvkFormat::Unknown,
        0 };

      case D3D9Format::Q16W16V16U16: return {
        DxvkFormat::RGBA16sn,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::MULTI2_ARGB8: return {}; // Unsupported

      case D3D9Format::R16F: return {
        DxvkFormat::R16sf, // TODO: one swizzle?
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::G16R16F: return {
        DxvkFormat::RG16sf, // TODO: check if swizzles are ok (ONE for missing components)
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::A16B16G16R16F: return {
        DxvkFormat::RGBA16sf,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::R32F: return {
        DxvkFormat::R32sf,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::G32R32F: return {
        DxvkFormat::RG32sf,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::A32B32G32R32F: return {
        DxvkFormat::RGBA32sf,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::CxV8U8: return {}; // Unsupported

      case D3D9Format::A1: return {}; // Unsupported

      case D3D9Format::A2B10G10R10_XR_BIAS: return {
        DxvkFormat::RGB10A2un,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::BINARYBUFFER: return {
        DxvkFormat::R8ui,
        DxvkFormat::Unknown,
        0 };

      case D3D9Format::ATI1: return {
        DxvkFormat::BC4un, // TODO: swizzles?
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::ATI2: return {
        DxvkFormat::BC5un, // TODO: swizzles?
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT };

      case D3D9Format::INST: return {}; // Driver hack, handled elsewhere

      case D3D9Format::DF24: return {
        DxvkFormat::D24, // TODO: swizzles? // TODO: D24S8?
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_DEPTH_BIT};

      case D3D9Format::DF16: return {
        DxvkFormat::D16, // TODO: swizzles?
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_DEPTH_BIT };

      case D3D9Format::NULL_FORMAT: return {}; // Driver hack, handled elsewhere

      case D3D9Format::GET4: return {}; // Unsupported

      case D3D9Format::GET1: return {}; // Unsupported

      case D3D9Format::NVDB: return {}; // Driver hack, handled elsewhere

      case D3D9Format::A2M1: return {}; // Driver hack, handled elsewhere

      case D3D9Format::A2M0: return {}; // Driver hack, handled elsewhere

      case D3D9Format::ATOC: return {}; // Driver hack, handled elsewhere

      case D3D9Format::INTZ: return {
        DxvkFormat::D24S8, // TODO: swizzles?
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT };

      case D3D9Format::NV12: return {
        DxvkFormat::R8un,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT,
        { D3D9ConversionFormat_NV12, 2u, DxvkFormat::BGRA8un }
      };

      case D3D9Format::YV12: return {
        DxvkFormat::R8un,
        DxvkFormat::Unknown,
        VK_IMAGE_ASPECT_COLOR_BIT,
        { D3D9ConversionFormat_YV12, 3u, DxvkFormat::BGRA8un }
      };

      case D3D9Format::RAWZ: return {}; // Unsupported

      default:
        Logger::warn(str::format("ConvertFormat: Unknown format encountered: ", Format));
        return {}; // Unsupported
    }
  }

  D3D9VkFormatTable::D3D9VkFormatTable(
    const Rc<DxvkAdapter>& adapter,
    const D3D9Options&     options)
    : m_adapter(adapter) {
    m_dfSupport = options.supportDFFormats;
    m_x4r4g4b4Support = options.supportX4R4G4B4;
    m_d32supportFinal = options.supportD32;

    // AMD do not support 24-bit depth buffers on Vulkan,
    // so we have to fall back to a 32-bit depth format.
    m_d24s8Support = CheckImageFormatSupport(adapter, VK_FORMAT_D24_UNORM_S8_UINT,
      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
      VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);

    // NVIDIA do not support 16-bit depth buffers with stencil on Vulkan,
    // so we have to fall back to a 32-bit depth format.
    m_d16s8Support = CheckImageFormatSupport(adapter, VK_FORMAT_D16_UNORM_S8_UINT,
      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
      VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);

    // VK_EXT_4444_formats
    m_a4r4g4b4Support = CheckImageFormatSupport(adapter, VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT,
      VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);

    if (!m_d24s8Support)
      Logger::info("D3D9: VK_FORMAT_D24_UNORM_S8_UINT -> VK_FORMAT_D32_SFLOAT_S8_UINT");

    if (!m_d16s8Support) {
      if (m_d24s8Support)
        Logger::info("D3D9: VK_FORMAT_D16_UNORM_S8_UINT -> VK_FORMAT_D24_UNORM_S8_UINT");
      else
        Logger::info("D3D9: VK_FORMAT_D16_UNORM_S8_UINT -> VK_FORMAT_D32_SFLOAT_S8_UINT");
    }

    if (!m_a4r4g4b4Support)
      Logger::warn("D3D9: VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT -> VK_FORMAT_B4G4R4A4_UNORM_PACK16");
  }

  D3D9_VK_FORMAT_MAPPING D3D9VkFormatTable::GetFormatMapping(
          D3D9Format          Format) const {
    D3D9_VK_FORMAT_MAPPING mapping = ConvertFormatUnfixed(Format);

    if (Format == D3D9Format::DF16 && !m_dfSupport)
      return D3D9_VK_FORMAT_MAPPING();

    if (Format == D3D9Format::DF24 && !m_dfSupport)
      return D3D9_VK_FORMAT_MAPPING();

    if (Format == D3D9Format::D32 && !m_d32supportFinal)
      return D3D9_VK_FORMAT_MAPPING();

    return mapping;
  }

  const DxvkFormatProperties* D3D9VkFormatTable::GetUnsupportedFormatInfo(
    D3D9Format            Format) const {
    static const DxvkFormatProperties r8b8g8      = { DxvkFormat::Unknown, DxvkFormatType::None, VK_FORMAT_UNDEFINED, VK_IMAGE_ASPECT_COLOR_BIT, 3 };
    static const DxvkFormatProperties r3g3b2      = { DxvkFormat::Unknown, DxvkFormatType::None, VK_FORMAT_UNDEFINED, VK_IMAGE_ASPECT_COLOR_BIT, 1 };
    static const DxvkFormatProperties a8r3g3b2    = { DxvkFormat::Unknown, DxvkFormatType::None, VK_FORMAT_UNDEFINED, VK_IMAGE_ASPECT_COLOR_BIT, 2 };
    static const DxvkFormatProperties a8p8        = { DxvkFormat::Unknown, DxvkFormatType::None, VK_FORMAT_UNDEFINED, VK_IMAGE_ASPECT_COLOR_BIT, 2 };
    static const DxvkFormatProperties p8          = { DxvkFormat::Unknown, DxvkFormatType::None, VK_FORMAT_UNDEFINED, VK_IMAGE_ASPECT_COLOR_BIT, 1 };
    static const DxvkFormatProperties l6v5u5      = { DxvkFormat::Unknown, DxvkFormatType::None, VK_FORMAT_UNDEFINED, VK_IMAGE_ASPECT_COLOR_BIT, 2 };
    static const DxvkFormatProperties x8l8v8u8    = { DxvkFormat::Unknown, DxvkFormatType::None, VK_FORMAT_UNDEFINED, VK_IMAGE_ASPECT_COLOR_BIT, 4 };
    static const DxvkFormatProperties a2w10v10u10 = { DxvkFormat::Unknown, DxvkFormatType::None, VK_FORMAT_UNDEFINED, VK_IMAGE_ASPECT_COLOR_BIT, 4 };
    static const DxvkFormatProperties cxv8u8      = { DxvkFormat::Unknown, DxvkFormatType::None, VK_FORMAT_UNDEFINED, VK_IMAGE_ASPECT_COLOR_BIT, 2 };
    static const DxvkFormatProperties unknown     = {};

    switch (Format) {
      case D3D9Format::R8G8B8:
        return &r8b8g8;

      case D3D9Format::R3G3B2:
        return &r3g3b2;

      case D3D9Format::A8R3G3B2:
        return &a8r3g3b2;

      case D3D9Format::A8P8:
        return &a8p8;

      case D3D9Format::P8:
        return &p8;

      case D3D9Format::L6V5U5:
        return &l6v5u5;

      case D3D9Format::X8L8V8U8:
        return &x8l8v8u8;

      case D3D9Format::A2W10V10U10:
        return &a2w10v10u10;

      // MULTI2_ARGB8 -> Don't have a clue what this is.

      case D3D9Format::CxV8U8:
        return &cxv8u8;

      // A1 -> Doesn't map nicely here cause it's not byte aligned.
      // Gonna just pretend that doesn't exist until something
      // depends on that.

      default:
        return &unknown;
    }
  }
  

  bool D3D9VkFormatTable::CheckImageFormatSupport(
    const Rc<DxvkAdapter>&      Adapter,
          VkFormat              Format,
          VkFormatFeatureFlags  Features) const {
    VkFormatProperties supported = Adapter->formatProperties(Format);
    
    return (supported.linearTilingFeatures  & Features) == Features
        || (supported.optimalTilingFeatures & Features) == Features;
  }

}