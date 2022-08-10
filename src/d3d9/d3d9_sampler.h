#pragma once

#include "d3d9_include.h"

#include "d3d9_util.h"

#include "../dxvk/dxvk_hash.h"

#include "../util/util_math.h"

namespace dxvk {

  class D3D9MipBiasFixed {
    // AMD HW uses 8 bits but that still leads to too many samplers.
    static constexpr int frac_bits = 6;
  public:
    D3D9MipBiasFixed() {}

    D3D9MipBiasFixed(float value) {
      // Games also pass NAN here, this accounts for that.
      if (value != value)
        value = 0.0f;

      // Clamp between -15.0f and 15.0f, matching mip limits of d3d9.
      value = std::clamp(value, -15.0f, 15.0f);

      // Convert to signed fixed point.
      data = value * (1 << frac_bits);
    }

    operator float() const {
      return float(data) / (1 << frac_bits);
    }

    int data = 0;
  };

  struct D3D9SamplerKey {
    D3DTEXTUREADDRESS AddressU;
    D3DTEXTUREADDRESS AddressV;
    D3DTEXTUREADDRESS AddressW;
    D3DTEXTUREFILTERTYPE MagFilter;
    D3DTEXTUREFILTERTYPE MinFilter;
    D3DTEXTUREFILTERTYPE MipFilter;
    DWORD MaxAnisotropy;
    D3D9MipBiasFixed MipmapLodBias;
    DWORD MaxMipLevel;
    D3DCOLOR BorderColor;
    bool Depth;

    size_t hash() const;
    bool eq(const D3D9SamplerKey& other) const;
  };

  inline void NormalizeSamplerKey(D3D9SamplerKey& key) {
    key.AddressU = std::clamp(key.AddressU, D3DTADDRESS_WRAP, D3DTADDRESS_MIRRORONCE);
    key.AddressV = std::clamp(key.AddressV, D3DTADDRESS_WRAP, D3DTADDRESS_MIRRORONCE);
    key.AddressW = std::clamp(key.AddressW, D3DTADDRESS_WRAP, D3DTADDRESS_MIRRORONCE);

    bool hasAnisotropy = IsAnisotropic(key.MagFilter) || IsAnisotropic(key.MinFilter);

    key.MagFilter = std::clamp(key.MagFilter, D3DTEXF_NONE, D3DTEXF_LINEAR);
    key.MinFilter = std::clamp(key.MinFilter, D3DTEXF_NONE, D3DTEXF_LINEAR);
    key.MipFilter = std::clamp(key.MipFilter, D3DTEXF_NONE, D3DTEXF_LINEAR);

    key.MaxAnisotropy = hasAnisotropy
      ? std::clamp<DWORD>(key.MaxAnisotropy, 1, 16)
      : 1;

    if (key.MipFilter == D3DTEXF_NONE) {
      // May as well try and keep slots down.
      key.MipmapLodBias = 0;
    }

    if (key.AddressU != D3DTADDRESS_BORDER
     && key.AddressV != D3DTADDRESS_BORDER
     && key.AddressW != D3DTADDRESS_BORDER) {
      key.BorderColor = 0;
    }
  }

}