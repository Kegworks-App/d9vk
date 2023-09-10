#pragma once

#include <array>

#include <cstdint>

#include "../spirv/spirv_module.h"

class D3D9DeviceEx;

namespace dxvk {

  enum D3D9SpecConstantId : uint32_t {
    SpecAlphaCompareOp  = 0,
    SpecSamplerType     = 1,
    SpecFogEnabled      = 2,
    SpecVertexFogMode   = 3,
    SpecPixelFogMode    = 4,

    SpecPointMode       = 5,
    SpecProjectionType  = 6,

    SpecVertexShaderBools = 7,
    SpecPixelShaderBools  = 8,
    SpecFetch4            = 9,

    SpecSamplerDepthMode  = 10,
    
    SpecConstantCount,
  };

  struct D3D9SpecializationInfo {
    template <D3D9SpecConstantId Id>
    uint32_t set(uint32_t value) {
      if (get<Id>() == value)
        return false;

      data[Id] = value;
      return true;
    }

    template <D3D9SpecConstantId Id>
    uint32_t get() const {
      return data[Id];
    }

    std::array<uint32_t, SpecConstantCount> data = {};
  };

}