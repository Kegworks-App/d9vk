#include "dxvk_format.h"
#include "dxvk_device.h"

namespace dxvk {

  static constexpr VkComponentMapping FormatSwizzleRGBX = { VK_COMPONENT_SWIZZLE_R,   VK_COMPONENT_SWIZZLE_G,   VK_COMPONENT_SWIZZLE_B,   VK_COMPONENT_SWIZZLE_ONE  };
  static constexpr VkComponentMapping FormatSwizzleBGRA = { VK_COMPONENT_SWIZZLE_B,   VK_COMPONENT_SWIZZLE_G,   VK_COMPONENT_SWIZZLE_R,   VK_COMPONENT_SWIZZLE_A    };
  static constexpr VkComponentMapping FormatSwizzleBGRX = { VK_COMPONENT_SWIZZLE_B,   VK_COMPONENT_SWIZZLE_G,   VK_COMPONENT_SWIZZLE_R,   VK_COMPONENT_SWIZZLE_ONE  };
  static constexpr VkComponentMapping FormatSwizzleAr   = { VK_COMPONENT_SWIZZLE_ZERO,VK_COMPONENT_SWIZZLE_ZERO,VK_COMPONENT_SWIZZLE_ZERO,VK_COMPONENT_SWIZZLE_R    };
  static constexpr VkComponentMapping FormatSwizzleAw   = { VK_COMPONENT_SWIZZLE_A,   VK_COMPONENT_SWIZZLE_ZERO,VK_COMPONENT_SWIZZLE_ZERO,VK_COMPONENT_SWIZZLE_ZERO };
  static constexpr VkComponentMapping FormatSwizzleL    = { VK_COMPONENT_SWIZZLE_R,   VK_COMPONENT_SWIZZLE_R,   VK_COMPONENT_SWIZZLE_R,   VK_COMPONENT_SWIZZLE_ONE  };
  static constexpr VkComponentMapping FormatSwizzleLA   = { VK_COMPONENT_SWIZZLE_R,   VK_COMPONENT_SWIZZLE_R,   VK_COMPONENT_SWIZZLE_R,   VK_COMPONENT_SWIZZLE_G    };
  static constexpr VkComponentMapping FormatSwizzleVU   = { VK_COMPONENT_SWIZZLE_R,   VK_COMPONENT_SWIZZLE_G,   VK_COMPONENT_SWIZZLE_ONE, VK_COMPONENT_SWIZZLE_ONE  };
  static constexpr VkComponentMapping FormatSwizzleAYUV = { VK_COMPONENT_SWIZZLE_R,   VK_COMPONENT_SWIZZLE_B,   VK_COMPONENT_SWIZZLE_G,   VK_COMPONENT_SWIZZLE_A    };

  static const std::array<DxvkFormatProperties, 104> g_formatProperties = {{
    /* RGBA */
    { DxvkFormat::RGBA4un,        DxvkFormatType::Unorm,    VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT,        VK_IMAGE_ASPECT_COLOR_BIT,                                  2 },
    { DxvkFormat::RGB5A1un,       DxvkFormatType::Unorm,    VK_FORMAT_A1R5G5B5_UNORM_PACK16,            VK_IMAGE_ASPECT_COLOR_BIT,                                  2,    { 1, 1, 1 },  FormatSwizzleBGRA,    FormatSwizzleBGRA },
    { DxvkFormat::RGBA8un,        DxvkFormatType::Unorm,    VK_FORMAT_R8G8B8A8_UNORM,                   VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::RGBA8srgb,      DxvkFormatType::Srgb,     VK_FORMAT_R8G8B8A8_SRGB,                    VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::RGBA8ui,        DxvkFormatType::Uint,     VK_FORMAT_R8G8B8A8_UINT,                    VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::RGBA8us,        DxvkFormatType::Uscaled,  VK_FORMAT_R8G8B8A8_USCALED,                 VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::RGBA8sn,        DxvkFormatType::Snorm,    VK_FORMAT_R8G8B8A8_SNORM,                   VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::RGBA8si,        DxvkFormatType::Sint,     VK_FORMAT_R8G8B8A8_SINT,                    VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::RGBA8ss,        DxvkFormatType::Sscaled,  VK_FORMAT_R8G8B8A8_SSCALED,                 VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::RGB10A2un,      DxvkFormatType::Unorm,    VK_FORMAT_A2B10G10R10_UNORM_PACK32,         VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::RGB10A2ui,      DxvkFormatType::Uint,     VK_FORMAT_A2B10G10R10_UINT_PACK32,          VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::RGB10A2us,      DxvkFormatType::Uscaled,  VK_FORMAT_A2B10G10R10_USCALED_PACK32,       VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::RGB10A2sn,      DxvkFormatType::Snorm,    VK_FORMAT_A2B10G10R10_SNORM_PACK32,         VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::RGB10A2si,      DxvkFormatType::Sint,     VK_FORMAT_A2B10G10R10_SINT_PACK32,          VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::RGB10A2ss,      DxvkFormatType::Sscaled,  VK_FORMAT_A2B10G10R10_SSCALED_PACK32,       VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::RGBA16sf,       DxvkFormatType::Float,    VK_FORMAT_R16G16B16A16_SFLOAT,              VK_IMAGE_ASPECT_COLOR_BIT,                                  8 },
    { DxvkFormat::RGBA16un,       DxvkFormatType::Unorm,    VK_FORMAT_R16G16B16A16_UNORM,               VK_IMAGE_ASPECT_COLOR_BIT,                                  8 },
    { DxvkFormat::RGBA16ui,       DxvkFormatType::Uint,     VK_FORMAT_R16G16B16A16_UINT,                VK_IMAGE_ASPECT_COLOR_BIT,                                  8 },
    { DxvkFormat::RGBA16us,       DxvkFormatType::Uscaled,  VK_FORMAT_R16G16B16A16_USCALED,             VK_IMAGE_ASPECT_COLOR_BIT,                                  8 },
    { DxvkFormat::RGBA16sn,       DxvkFormatType::Snorm,    VK_FORMAT_R16G16B16A16_SNORM,               VK_IMAGE_ASPECT_COLOR_BIT,                                  8 },
    { DxvkFormat::RGBA16si,       DxvkFormatType::Sint,     VK_FORMAT_R16G16B16A16_SINT,                VK_IMAGE_ASPECT_COLOR_BIT,                                  8 },
    { DxvkFormat::RGBA16ss,       DxvkFormatType::Sscaled,  VK_FORMAT_R16G16B16A16_SSCALED,             VK_IMAGE_ASPECT_COLOR_BIT,                                  8 },
    { DxvkFormat::RGBA32sf,       DxvkFormatType::Float,    VK_FORMAT_R32G32B32A32_SFLOAT,              VK_IMAGE_ASPECT_COLOR_BIT,                                  16 },
    { DxvkFormat::RGBA32ui,       DxvkFormatType::Uint,     VK_FORMAT_R32G32B32A32_UINT,                VK_IMAGE_ASPECT_COLOR_BIT,                                  16 },
    { DxvkFormat::RGBA32si,       DxvkFormatType::Sint,     VK_FORMAT_R32G32B32A32_SINT,                VK_IMAGE_ASPECT_COLOR_BIT,                                  16 },
    /* RGBX */
    { DxvkFormat::RGBX4un,        DxvkFormatType::Unorm,    VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT,        VK_IMAGE_ASPECT_COLOR_BIT,                                  2,    { 1, 1, 1 },  FormatSwizzleRGBX },
    { DxvkFormat::RGB5X1un,       DxvkFormatType::Unorm,    VK_FORMAT_A1R5G5B5_UNORM_PACK16,            VK_IMAGE_ASPECT_COLOR_BIT,                                  2,    { 1, 1, 1 },  FormatSwizzleBGRX,    FormatSwizzleBGRX },
    { DxvkFormat::RGBX8un,        DxvkFormatType::Unorm,    VK_FORMAT_R8G8B8A8_UNORM,                   VK_IMAGE_ASPECT_COLOR_BIT,                                  4,    { 1, 1, 1 },  FormatSwizzleRGBX },
    { DxvkFormat::RGBX8srgb,      DxvkFormatType::Srgb,     VK_FORMAT_R8G8B8A8_SRGB,                    VK_IMAGE_ASPECT_COLOR_BIT,                                  4,    { 1, 1, 1 },  FormatSwizzleRGBX },
    /* BGRA */
    { DxvkFormat::BGRA4un,        DxvkFormatType::Unorm,    VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT,        VK_IMAGE_ASPECT_COLOR_BIT,                                  2 },
    { DxvkFormat::BGR5A1un,       DxvkFormatType::Unorm,    VK_FORMAT_A1R5G5B5_UNORM_PACK16,            VK_IMAGE_ASPECT_COLOR_BIT,                                  2 },
    { DxvkFormat::BGRA8un,        DxvkFormatType::Unorm,    VK_FORMAT_B8G8R8A8_UNORM,                   VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::BGRA8srgb,      DxvkFormatType::Srgb,     VK_FORMAT_B8G8R8A8_SRGB,                    VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::BGRA8ui,        DxvkFormatType::Uint,     VK_FORMAT_B8G8R8A8_UINT,                    VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::BGR10A2un,      DxvkFormatType::Unorm,    VK_FORMAT_A2R10G10B10_UNORM_PACK32,         VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::BGR10A2ui,      DxvkFormatType::Uint,     VK_FORMAT_A2R10G10B10_UINT_PACK32,          VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    /* BGRX */
    { DxvkFormat::BGRX4un,        DxvkFormatType::Unorm,    VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT,        VK_IMAGE_ASPECT_COLOR_BIT,                                  2,    { 1, 1, 1 },  FormatSwizzleRGBX },
    { DxvkFormat::BGR5X1un,       DxvkFormatType::Unorm,    VK_FORMAT_A1R5G5B5_UNORM_PACK16,            VK_IMAGE_ASPECT_COLOR_BIT,                                  2,    { 1, 1, 1 },  FormatSwizzleRGBX },
    { DxvkFormat::BGRX8un,        DxvkFormatType::Unorm,    VK_FORMAT_B8G8R8A8_UNORM,                   VK_IMAGE_ASPECT_COLOR_BIT,                                  4,    { 1, 1, 1 },  FormatSwizzleRGBX },
    { DxvkFormat::BGRX8srgb,      DxvkFormatType::Srgb,     VK_FORMAT_B8G8R8A8_SRGB,                    VK_IMAGE_ASPECT_COLOR_BIT,                                  4,    { 1, 1, 1 },  FormatSwizzleRGBX },
    /* RGB */
    { DxvkFormat::R5G6B5un,       DxvkFormatType::Unorm,    VK_FORMAT_B5G6R5_UNORM_PACK16,              VK_IMAGE_ASPECT_COLOR_BIT,                                  2 },
    { DxvkFormat::RGB9E5uf,       DxvkFormatType::Float,    VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,           VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::RGB11uf,        DxvkFormatType::Float,    VK_FORMAT_B10G11R11_UFLOAT_PACK32,          VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::RGB32sf,        DxvkFormatType::Float,    VK_FORMAT_R32G32B32_SFLOAT,                 VK_IMAGE_ASPECT_COLOR_BIT,                                  12 },
    { DxvkFormat::RGB32ui,        DxvkFormatType::Uint,     VK_FORMAT_R32G32B32_UINT,                   VK_IMAGE_ASPECT_COLOR_BIT,                                  12 },
    { DxvkFormat::RGB32si,        DxvkFormatType::Sint,     VK_FORMAT_R32G32B32_SINT,                   VK_IMAGE_ASPECT_COLOR_BIT,                                  12 },
    /* BGR */
    { DxvkFormat::B5G6R5un,       DxvkFormatType::Unorm,    VK_FORMAT_R5G6B5_UNORM_PACK16,              VK_IMAGE_ASPECT_COLOR_BIT,                                  2 },
    /* RG */
    { DxvkFormat::RG8un,          DxvkFormatType::Unorm,    VK_FORMAT_R8G8_UNORM,                       VK_IMAGE_ASPECT_COLOR_BIT,                                  2 },
    { DxvkFormat::RG8ui,          DxvkFormatType::Uint,     VK_FORMAT_R8G8_UINT,                        VK_IMAGE_ASPECT_COLOR_BIT,                                  2 },
    { DxvkFormat::RG8us,          DxvkFormatType::Uscaled,  VK_FORMAT_R8G8_USCALED,                     VK_IMAGE_ASPECT_COLOR_BIT,                                  2 },
    { DxvkFormat::RG8sn,          DxvkFormatType::Snorm,    VK_FORMAT_R8G8_SNORM,                       VK_IMAGE_ASPECT_COLOR_BIT,                                  2 },
    { DxvkFormat::RG8si,          DxvkFormatType::Sint,     VK_FORMAT_R8G8_SINT,                        VK_IMAGE_ASPECT_COLOR_BIT,                                  2 },
    { DxvkFormat::RG8ss,          DxvkFormatType::Sscaled,  VK_FORMAT_R8G8_SSCALED,                     VK_IMAGE_ASPECT_COLOR_BIT,                                  2 },
    { DxvkFormat::RG16sf,         DxvkFormatType::Float,    VK_FORMAT_R16G16_SFLOAT,                    VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::RG16un,         DxvkFormatType::Unorm,    VK_FORMAT_R16G16_UNORM,                     VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::RG16ui,         DxvkFormatType::Uint,     VK_FORMAT_R16G16_UINT,                      VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::RG16us,         DxvkFormatType::Uscaled,  VK_FORMAT_R16G16_USCALED,                   VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::RG16sn,         DxvkFormatType::Snorm,    VK_FORMAT_R16G16_SNORM,                     VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::RG16si,         DxvkFormatType::Sint,     VK_FORMAT_R16G16_SINT,                      VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::RG16ss,         DxvkFormatType::Sscaled,  VK_FORMAT_R16G16_SSCALED,                   VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::RG32sf,         DxvkFormatType::Float,    VK_FORMAT_R32G32_SFLOAT,                    VK_IMAGE_ASPECT_COLOR_BIT,                                  8 },
    { DxvkFormat::RG32ui,         DxvkFormatType::Uint,     VK_FORMAT_R32G32_UINT,                      VK_IMAGE_ASPECT_COLOR_BIT,                                  8 },
    { DxvkFormat::RG32si,         DxvkFormatType::Sint,     VK_FORMAT_R32G32_SINT,                      VK_IMAGE_ASPECT_COLOR_BIT,                                  8 },
    /* R */
    { DxvkFormat::R8un,           DxvkFormatType::Unorm,    VK_FORMAT_R8_UNORM,                         VK_IMAGE_ASPECT_COLOR_BIT,                                  1 },
    { DxvkFormat::R8ui,           DxvkFormatType::Uint,     VK_FORMAT_R8_UINT,                          VK_IMAGE_ASPECT_COLOR_BIT,                                  1 },
    { DxvkFormat::R8us,           DxvkFormatType::Uscaled,  VK_FORMAT_R8_USCALED,                       VK_IMAGE_ASPECT_COLOR_BIT,                                  1 },
    { DxvkFormat::R8sn,           DxvkFormatType::Snorm,    VK_FORMAT_R8_SNORM,                         VK_IMAGE_ASPECT_COLOR_BIT,                                  1 },
    { DxvkFormat::R8si,           DxvkFormatType::Sint,     VK_FORMAT_R8_SINT,                          VK_IMAGE_ASPECT_COLOR_BIT,                                  1 },
    { DxvkFormat::R8ss,           DxvkFormatType::Sscaled,  VK_FORMAT_R8_SSCALED,                       VK_IMAGE_ASPECT_COLOR_BIT,                                  1 },
    { DxvkFormat::R16sf,          DxvkFormatType::Float,    VK_FORMAT_R16_SFLOAT,                       VK_IMAGE_ASPECT_COLOR_BIT,                                  2 },
    { DxvkFormat::R16un,          DxvkFormatType::Unorm,    VK_FORMAT_R16_UNORM,                        VK_IMAGE_ASPECT_COLOR_BIT,                                  2 },
    { DxvkFormat::R16ui,          DxvkFormatType::Uint,     VK_FORMAT_R16_UINT,                         VK_IMAGE_ASPECT_COLOR_BIT,                                  2 },
    { DxvkFormat::R16us,          DxvkFormatType::Uscaled,  VK_FORMAT_R16_USCALED,                      VK_IMAGE_ASPECT_COLOR_BIT,                                  2 },
    { DxvkFormat::R16sn,          DxvkFormatType::Snorm,    VK_FORMAT_R16_SNORM,                        VK_IMAGE_ASPECT_COLOR_BIT,                                  2 },
    { DxvkFormat::R16si,          DxvkFormatType::Sint,     VK_FORMAT_R16_SINT,                         VK_IMAGE_ASPECT_COLOR_BIT,                                  2 },
    { DxvkFormat::R16ss,          DxvkFormatType::Sscaled,  VK_FORMAT_R16_SSCALED,                      VK_IMAGE_ASPECT_COLOR_BIT,                                  2 },
    { DxvkFormat::R32sf,          DxvkFormatType::Float,    VK_FORMAT_R32_SFLOAT,                       VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::R32ui,          DxvkFormatType::Uint,     VK_FORMAT_R32_UINT,                         VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    { DxvkFormat::R32si,          DxvkFormatType::Sint,     VK_FORMAT_R32_SINT,                         VK_IMAGE_ASPECT_COLOR_BIT,                                  4 },
    /* A */
    { DxvkFormat::A8un,           DxvkFormatType::Unorm,    VK_FORMAT_R8_UNORM,                         VK_IMAGE_ASPECT_COLOR_BIT,                                  1,    { 1, 1, 1 },  FormatSwizzleAr,      FormatSwizzleAw },
    /* D / DS */
    { DxvkFormat::S8,             DxvkFormatType::Unorm,    VK_FORMAT_S8_UINT,                          VK_IMAGE_ASPECT_STENCIL_BIT,                                1 },
    { DxvkFormat::D16,            DxvkFormatType::Unorm,    VK_FORMAT_D16_UNORM,                        VK_IMAGE_ASPECT_DEPTH_BIT,                                  2 },
    { DxvkFormat::D16S8,          DxvkFormatType::Unorm,    VK_FORMAT_D16_UNORM_S8_UINT,                VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,    4 },
    { DxvkFormat::D24,            DxvkFormatType::Unorm,    VK_FORMAT_X8_D24_UNORM_PACK32,              VK_IMAGE_ASPECT_DEPTH_BIT,                                  4 },
    { DxvkFormat::D24S8,          DxvkFormatType::Unorm,    VK_FORMAT_D24_UNORM_S8_UINT,                VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,    4 },
    { DxvkFormat::D32,            DxvkFormatType::Float,    VK_FORMAT_D32_SFLOAT,                       VK_IMAGE_ASPECT_DEPTH_BIT,                                  4 },
    { DxvkFormat::D32S8,          DxvkFormatType::Float,    VK_FORMAT_D32_SFLOAT_S8_UINT,               VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,    8 },
    /* Block-compressed */
    { DxvkFormat::BC1un,          DxvkFormatType::Unorm,    VK_FORMAT_BC1_RGBA_UNORM_BLOCK,             VK_IMAGE_ASPECT_COLOR_BIT,                                  8,    { 4, 4, 1 } },
    { DxvkFormat::BC1srgb,        DxvkFormatType::Srgb,     VK_FORMAT_BC1_RGBA_SRGB_BLOCK,              VK_IMAGE_ASPECT_COLOR_BIT,                                  8,    { 4, 4, 1 } },
    { DxvkFormat::BC2un,          DxvkFormatType::Unorm,    VK_FORMAT_BC2_UNORM_BLOCK,                  VK_IMAGE_ASPECT_COLOR_BIT,                                  16,   { 4, 4, 1 } },
    { DxvkFormat::BC2srgb,        DxvkFormatType::Srgb,     VK_FORMAT_BC2_SRGB_BLOCK,                   VK_IMAGE_ASPECT_COLOR_BIT,                                  16,   { 4, 4, 1 } },
    { DxvkFormat::BC3un,          DxvkFormatType::Unorm,    VK_FORMAT_BC3_UNORM_BLOCK,                  VK_IMAGE_ASPECT_COLOR_BIT,                                  16,   { 4, 4, 1 } },
    { DxvkFormat::BC3srgb,        DxvkFormatType::Srgb,     VK_FORMAT_BC3_SRGB_BLOCK,                   VK_IMAGE_ASPECT_COLOR_BIT,                                  16,   { 4, 4, 1 } },
    { DxvkFormat::BC4un,          DxvkFormatType::Unorm,    VK_FORMAT_BC4_UNORM_BLOCK,                  VK_IMAGE_ASPECT_COLOR_BIT,                                  8,    { 4, 4, 1 } },
    { DxvkFormat::BC4sn,          DxvkFormatType::Snorm,    VK_FORMAT_BC4_SNORM_BLOCK,                  VK_IMAGE_ASPECT_COLOR_BIT,                                  8,    { 4, 4, 1 } },
    { DxvkFormat::BC5un,          DxvkFormatType::Unorm,    VK_FORMAT_BC5_UNORM_BLOCK,                  VK_IMAGE_ASPECT_COLOR_BIT,                                  16,   { 4, 4, 1 } },
    { DxvkFormat::BC5sn,          DxvkFormatType::Snorm,    VK_FORMAT_BC5_SNORM_BLOCK,                  VK_IMAGE_ASPECT_COLOR_BIT,                                  16,   { 4, 4, 1 } },
    { DxvkFormat::BC6Huf,         DxvkFormatType::Float,    VK_FORMAT_BC6H_UFLOAT_BLOCK,                VK_IMAGE_ASPECT_COLOR_BIT,                                  16,   { 4, 4, 1 } },
    { DxvkFormat::BC6Hsf,         DxvkFormatType::Float,    VK_FORMAT_BC6H_SFLOAT_BLOCK,                VK_IMAGE_ASPECT_COLOR_BIT,                                  16,   { 4, 4, 1 } },
    { DxvkFormat::BC7un,          DxvkFormatType::Unorm,    VK_FORMAT_BC7_UNORM_BLOCK,                  VK_IMAGE_ASPECT_COLOR_BIT,                                  16,   { 4, 4, 1 } },
    { DxvkFormat::BC7srgb,        DxvkFormatType::Srgb,     VK_FORMAT_BC7_SRGB_BLOCK,                   VK_IMAGE_ASPECT_COLOR_BIT,                                  16,   { 4, 4, 1 } },
    /* Subsampled */
    { DxvkFormat::RGBG8un_422,    DxvkFormatType::Unorm,    VK_FORMAT_G8B8G8R8_422_UNORM,               VK_IMAGE_ASPECT_COLOR_BIT,                                  4,    { 2, 1, 1 } },
    { DxvkFormat::GRGB8un_422,    DxvkFormatType::Unorm,    VK_FORMAT_B8G8R8G8_422_UNORM,               VK_IMAGE_ASPECT_COLOR_BIT,                                  4,    { 2, 1, 1 } },
    /* Multi-plane */
    { DxvkFormat::NV12_420,       DxvkFormatType::None,     VK_FORMAT_G8_B8R8_2PLANE_420_UNORM,         VK_IMAGE_ASPECT_PLANE_0_BIT | VK_IMAGE_ASPECT_PLANE_1_BIT,  6,    { 1, 1, 1 },  FormatSwizzleDefault, FormatSwizzleDefault,
      { DxvkPlaneFormatInfo { 1, { 1, 1 } }, DxvkPlaneFormatInfo { 2, { 2, 2 } } } },
  }};
  

  DxvkFormatMap::DxvkFormatMap(DxvkAdapter* adapter) {
    for (const auto& e : g_formatProperties)
      m_formats[uint32_t(e.format)] = e;

    // Handle 4-4-4-4 formats here since we need to support those
    if (!adapter->features().ext4444Formats.formatA4R4G4B4) {
      Logger::warn("Mapping DxvkFormat::BGRA4un to VK_FORMAT_B4G4R4A4_UNORM_PACK16");

      auto* format = &m_formats[uint32_t(DxvkFormat::BGRA4un)];
      format->vkFormat = VK_FORMAT_B4G4R4A4_UNORM_PACK16;
      format->rSwizzle = { VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_A, VK_COMPONENT_SWIZZLE_B };

      format = &m_formats[uint32_t(DxvkFormat::BGRX4un)];
      format->vkFormat = VK_FORMAT_B4G4R4A4_UNORM_PACK16;
      format->rSwizzle = { VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_A, VK_COMPONENT_SWIZZLE_ONE };
    }

    // Check for format support
    for (auto& format : m_formats) {
      if (format.vkFormat) {
        adapter->vki()->vkGetPhysicalDeviceFormatProperties(
          adapter->handle(), format.vkFormat, &format.support);
      }
    }

    // All client APIs require D24 formats
    VkFormatFeatureFlags depthStencilFeatures = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    auto& d24   = m_formats[uint32_t(DxvkFormat::D24)];
    auto& d24s8 = m_formats[uint32_t(DxvkFormat::D24S8)];

    if ((d24.support.optimalTilingFeatures & depthStencilFeatures) != depthStencilFeatures) {
      Logger::warn("Mapping DxvkFormat::D24 to VK_FORMAT_D32_SFLOAT");

      auto& d32 = m_formats[uint32_t(DxvkFormat::D32)];
      d24.vkFormat = d32.vkFormat;
      d24.support  = d32.support;
    }

    if ((d24s8.support.optimalTilingFeatures & depthStencilFeatures) != depthStencilFeatures) {
      Logger::warn("Mapping DxvkFormat::D24S8 to VK_FORMAT_D32_SFLOAT_S8_UINT");

      auto& d32s8 = m_formats[uint32_t(DxvkFormat::D32S8)];
      d24s8.vkFormat = d32s8.vkFormat;
      d24s8.support  = d32s8.support;
    }
  }


  DxvkFormatMap::~DxvkFormatMap() {

  }


  const std::array<DxvkFormatInfo, 152> g_formatInfos = {{
    // VK_FORMAT_UNDEFINED
    { },
    
    // VK_FORMAT_R4G4_UNORM_PACK8
    { 1, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R4G4B4A4_UNORM_PACK16
    { 2, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_B4G4R4A4_UNORM_PACK16
    { 2, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R5G6B5_UNORM_PACK16
    { 2, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_B5G6R5_UNORM_PACK16
    { 2, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R5G5B5A1_UNORM_PACK16
    { 2, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_B5G5R5A1_UNORM_PACK16
    { 2, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_A1R5G5B5_UNORM_PACK16
    { 2, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R8_UNORM
    { 1, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R8_SNORM
    { 1, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R8_USCALED
    { 1, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R8_SSCALED
    { 1, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R8_UINT
    { 1, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledUInt },
    
    // VK_FORMAT_R8_SINT
    { 1, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledSInt },
    
    // VK_FORMAT_R8_SRGB
    { 1, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::ColorSpaceSrgb },
    
    // VK_FORMAT_R8G8_UNORM
    { 2, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R8G8_SNORM
    { 2, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R8G8_USCALED
    { 2, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R8G8_SSCALED
    { 2, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R8G8_UINT
    { 2, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledUInt },
    
    // VK_FORMAT_R8G8_SINT
    { 2, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledSInt },
    
    // VK_FORMAT_R8G8_SRGB
    { 2, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::ColorSpaceSrgb },
    
    // VK_FORMAT_R8G8B8_UNORM
    { 3, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R8G8B8_SNORM
    { 3, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R8G8B8_USCALED
    { 3, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R8G8B8_SSCALED
    { 3, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R8G8B8_UINT
    { 3, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledUInt },
    
    // VK_FORMAT_R8G8B8_SINT
    { 3, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledSInt },
    
    // VK_FORMAT_R8G8B8_SRGB
    { 3, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::ColorSpaceSrgb },
    
    // VK_FORMAT_B8G8R8_UNORM
    { 3, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_B8G8R8_SNORM
    { 3, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_B8G8R8_USCALED
    { 3, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_B8G8R8_SSCALED
    { 3, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_B8G8R8_UINT
    { 3, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledUInt },
    
    // VK_FORMAT_B8G8R8_SINT
    { 3, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledSInt },
    
    // VK_FORMAT_B8G8R8_SRGB
    { 3, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::ColorSpaceSrgb },
    
    // VK_FORMAT_R8G8B8A8_UNORM
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R8G8B8A8_SNORM
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R8G8B8A8_USCALED
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R8G8B8A8_SSCALED
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R8G8B8A8_UINT
    { 4, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledUInt },
    
    // VK_FORMAT_R8G8B8A8_SINT
    { 4, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledSInt },
    
    // VK_FORMAT_R8G8B8A8_SRGB
    { 4, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::ColorSpaceSrgb },
    
    // VK_FORMAT_B8G8R8A8_UNORM
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_B8G8R8A8_SNORM
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_B8G8R8A8_USCALED
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_B8G8R8A8_SSCALED
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_B8G8R8A8_UINT
    { 4, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledUInt },
    
    // VK_FORMAT_B8G8R8A8_SINT
    { 4, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledSInt },
    
    // VK_FORMAT_B8G8R8A8_SRGB
    { 4, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::ColorSpaceSrgb },
    
    // VK_FORMAT_A8B8G8R8_UNORM_PACK32
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_A8B8G8R8_SNORM_PACK32
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_A8B8G8R8_USCALED_PACK32
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_A8B8G8R8_SSCALED_PACK32
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_A8B8G8R8_UINT_PACK32
    { 4, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledUInt },
    
    // VK_FORMAT_A8B8G8R8_SINT_PACK32
    { 4, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledSInt },
    
    // VK_FORMAT_A8B8G8R8_SRGB_PACK32
    { 4, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::ColorSpaceSrgb },
    
    // VK_FORMAT_A2R10G10B10_UNORM_PACK32
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_A2R10G10B10_SNORM_PACK32
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_A2R10G10B10_USCALED_PACK32
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_A2R10G10B10_SSCALED_PACK32
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_A2R10G10B10_UINT_PACK32
    { 4, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledUInt },
    
    // VK_FORMAT_A2R10G10B10_SINT_PACK32
    { 4, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledSInt },
    
    // VK_FORMAT_A2B10G10R10_UNORM_PACK32
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_A2B10G10R10_SNORM_PACK32
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_A2B10G10R10_USCALED_PACK32
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_A2B10G10R10_SSCALED_PACK32
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_A2B10G10R10_UINT_PACK32
    { 4, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledUInt },
    
    // VK_FORMAT_A2B10G10R10_SINT_PACK32
    { 4, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledSInt },
    
    // VK_FORMAT_R16_UNORM
    { 2, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R16_SNORM
    { 2, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R16_USCALED
    { 2, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R16_SSCALED
    { 2, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R16_UINT
    { 2, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledUInt },
    
    // VK_FORMAT_R16_SINT
    { 2, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledSInt },
    
    // VK_FORMAT_R16_SFLOAT
    { 2, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R16G16_UNORM
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R16G16_SNORM
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R16G16_USCALED
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R16G16_SSCALED
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R16G16_UINT
    { 4, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledUInt },
    
    // VK_FORMAT_R16G16_SINT
    { 4, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledSInt },
    
    // VK_FORMAT_R16G16_SFLOAT
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R16G16B16_UNORM
    { 6, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R16G16B16_SNORM
    { 6, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R16G16B16_USCALED
    { 6, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R16G16B16_SSCALED
    { 6, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R16G16B16_UINT
    { 6, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledUInt },
    
    // VK_FORMAT_R16G16B16_SINT
    { 6, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledSInt },
    
    // VK_FORMAT_R16G16B16_SFLOAT
    { 6, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R16G16B16A16_UNORM
    { 8, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R16G16B16A16_SNORM
    { 8, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R16G16B16A16_USCALED
    { 8, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R16G16B16A16_SSCALED
    { 8, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R16G16B16A16_UINT
    { 8, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledUInt },
    
    // VK_FORMAT_R16G16B16A16_SINT
    { 8, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledSInt },
    
    // VK_FORMAT_R16G16B16A16_SFLOAT
    { 8, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R32_UINT
    { 4, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledUInt },
    
    // VK_FORMAT_R32_SINT
    { 4, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledSInt },
    
    // VK_FORMAT_R32_SFLOAT
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R32G32_UINT
    { 8, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledUInt },
    
    // VK_FORMAT_R32G32_SINT
    { 8, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledSInt },
    
    // VK_FORMAT_R32G32_SFLOAT
    { 8, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R32G32B32_UINT
    { 12, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledUInt },
    
    // VK_FORMAT_R32G32B32_SINT
    { 12, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledSInt },
    
    // VK_FORMAT_R32G32B32_SFLOAT
    { 12, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R32G32B32A32_UINT
    { 16, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledUInt },
    
    // VK_FORMAT_R32G32B32A32_SINT
    { 16, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledSInt },
    
    // VK_FORMAT_R32G32B32A32_SFLOAT
    { 16, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R64_UINT
    { 8, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledUInt },
    
    // VK_FORMAT_R64_SINT
    { 8, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledSInt },
    
    // VK_FORMAT_R64_SFLOAT
    { 8, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R64G64_UINT
    { 16, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledUInt },
    
    // VK_FORMAT_R64G64_SINT
    { 16, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledSInt },
    
    // VK_FORMAT_R64G64_SFLOAT
    { 16, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R64G64B64_UINT
    { 24, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledUInt },
    
    // VK_FORMAT_R64G64B64_SINT
    { 24, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledSInt },
    
    // VK_FORMAT_R64G64B64_SFLOAT
    { 24, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_R64G64B64A64_UINT
    { 32, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledUInt },
    
    // VK_FORMAT_R64G64B64A64_SINT
    { 32, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::SampledSInt },
    
    // VK_FORMAT_R64G64B64A64_SFLOAT
    { 32, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_B10G11R11_UFLOAT_PACK32
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_E5B9G9R9_UFLOAT_PACK32
    { 4, VK_IMAGE_ASPECT_COLOR_BIT },
    
    // VK_FORMAT_D16_UNORM
    { 2, VK_IMAGE_ASPECT_DEPTH_BIT },
    
    // VK_FORMAT_X8_D24_UNORM_PACK32
    { 4, VK_IMAGE_ASPECT_DEPTH_BIT },
    
    // VK_FORMAT_D32_SFLOAT
    { 4, VK_IMAGE_ASPECT_DEPTH_BIT },
    
    // VK_FORMAT_S8_UINT
    { 1, VK_IMAGE_ASPECT_STENCIL_BIT },
    
    // VK_FORMAT_D16_UNORM_S8_UINT
    { 4, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT },
    
    // VK_FORMAT_D24_UNORM_S8_UINT
    { 4, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT },
    
    // VK_FORMAT_D32_SFLOAT_S8_UINT
    { 8, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT },
    
    // VK_FORMAT_BC1_RGB_UNORM_BLOCK
    { 8, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::BlockCompressed,
      VkExtent3D { 4, 4, 1 } },
    
    // VK_FORMAT_BC1_RGB_SRGB_BLOCK
    { 8, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlags(
        DxvkFormatFlag::BlockCompressed,
        DxvkFormatFlag::ColorSpaceSrgb),
      VkExtent3D { 4, 4, 1 } },
    
    // VK_FORMAT_BC1_RGBA_UNORM_BLOCK
    { 8, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::BlockCompressed,
      VkExtent3D { 4, 4, 1 } },
    
    // VK_FORMAT_BC1_RGBA_SRGB_BLOCK
    { 8, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlags(
        DxvkFormatFlag::BlockCompressed,
        DxvkFormatFlag::ColorSpaceSrgb),
      VkExtent3D { 4, 4, 1 } },
    
    // VK_FORMAT_BC2_UNORM_BLOCK
    { 16, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::BlockCompressed,
      VkExtent3D { 4, 4, 1 } },
    
    // VK_FORMAT_BC2_SRGB_BLOCK
    { 16, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlags(
        DxvkFormatFlag::BlockCompressed,
        DxvkFormatFlag::ColorSpaceSrgb),
      VkExtent3D { 4, 4, 1 } },
    
    // VK_FORMAT_BC3_UNORM_BLOCK
    { 16, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::BlockCompressed,
      VkExtent3D { 4, 4, 1 } },
    
    // VK_FORMAT_BC3_SRGB_BLOCK
    { 16, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlags(
        DxvkFormatFlag::BlockCompressed,
        DxvkFormatFlag::ColorSpaceSrgb),
      VkExtent3D { 4, 4, 1 } },
    
    // VK_FORMAT_BC4_UNORM_BLOCK
    { 8, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::BlockCompressed,
      VkExtent3D { 4, 4, 1 } },
    
    // VK_FORMAT_BC4_SNORM_BLOCK
    { 8, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::BlockCompressed,
      VkExtent3D { 4, 4, 1 } },
    
    // VK_FORMAT_BC5_UNORM_BLOCK
    { 16, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::BlockCompressed,
      VkExtent3D { 4, 4, 1 } },
    
    // VK_FORMAT_BC5_SNORM_BLOCK
    { 16, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::BlockCompressed,
      VkExtent3D { 4, 4, 1 } },
    
    // VK_FORMAT_BC6H_UFLOAT_BLOCK
    { 16, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::BlockCompressed,
      VkExtent3D { 4, 4, 1 } },
    
    // VK_FORMAT_BC6H_SFLOAT_BLOCK
    { 16, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::BlockCompressed,
      VkExtent3D { 4, 4, 1 } },
    
    // VK_FORMAT_BC7_UNORM_BLOCK
    { 16, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::BlockCompressed,
      VkExtent3D { 4, 4, 1 } },
    
    // VK_FORMAT_BC7_SRGB_BLOCK
    { 16, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlags(
        DxvkFormatFlag::BlockCompressed,
        DxvkFormatFlag::ColorSpaceSrgb),
      VkExtent3D { 4, 4, 1 } },
    
    // VK_FORMAT_G8B8G8R8_422_UNORM_KHR
    { 4, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::BlockCompressed,
      VkExtent3D { 2, 1, 1 } },
    
    // VK_FORMAT_B8G8R8G8_422_UNORM_KHR
    { 4, VK_IMAGE_ASPECT_COLOR_BIT,
      DxvkFormatFlag::BlockCompressed,
      VkExtent3D { 2, 1, 1 } },

    // VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT
    { 2, VK_IMAGE_ASPECT_COLOR_BIT },

    // VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT
    { 2, VK_IMAGE_ASPECT_COLOR_BIT },

    // VK_FORMAT_G8_B8R8_2PLANE_420_UNORM
    { 6, VK_IMAGE_ASPECT_PLANE_0_BIT | VK_IMAGE_ASPECT_PLANE_1_BIT,
      DxvkFormatFlag::MultiPlane, VkExtent3D { 1, 1, 1 },
      { DxvkPlaneFormatInfo { 1, { 1, 1 } },
        DxvkPlaneFormatInfo { 2, { 2, 2 } } } },
  }};
  
  
  const std::array<std::pair<VkFormat, VkFormat>, 4> g_formatGroups = {{
    { VK_FORMAT_UNDEFINED,                  VK_FORMAT_BC7_SRGB_BLOCK            },
    { VK_FORMAT_G8B8G8R8_422_UNORM_KHR,     VK_FORMAT_B8G8R8G8_422_UNORM_KHR    },
    { VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT,  VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT },
    { VK_FORMAT_G8_B8R8_2PLANE_420_UNORM,   VK_FORMAT_G8_B8R8_2PLANE_420_UNORM  },
  }};
  
  
  const DxvkFormatInfo* imageFormatInfo(VkFormat format) {
    uint32_t indexOffset = 0;
    
    for (const auto& group : g_formatGroups) {
      if (format >= group.first && format <= group.second) {
        uint32_t index = uint32_t(format) - uint32_t(group.first);
        return &g_formatInfos[indexOffset + index];
      } else {
        indexOffset += uint32_t(group.second)
                     - uint32_t(group.first) + 1;
      }
    }
    
    return nullptr;
  }
  
}