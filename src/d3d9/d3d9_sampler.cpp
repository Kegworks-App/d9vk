#include "d3d9_sampler.h"

namespace dxvk {

  size_t D3D9SamplerKey::hash() const {
    DxvkHashState state;

    std::hash<DWORD>                dhash;
    std::hash<D3DTEXTUREADDRESS>    tahash;
    std::hash<D3DTEXTUREFILTERTYPE> tfhash;
    std::hash<int>                  ihash;
    std::hash<bool>                 bhash;

    state.add(tahash(AddressU));
    state.add(tahash(AddressV));
    state.add(tahash(AddressW));
    state.add(tfhash(MagFilter));
    state.add(tfhash(MinFilter));
    state.add(tfhash(MipFilter));
    state.add(dhash (MaxAnisotropy));
    state.add(ihash (MipmapLodBias.data));
    state.add(dhash (MaxMipLevel));
    state.add(dhash (BorderColor));
    state.add(bhash (Depth));

    return state;
  }


  bool D3D9SamplerKey::eq(const D3D9SamplerKey& other) const {
    return AddressU            == other.AddressU
        && AddressV            == other.AddressV
        && AddressW            == other.AddressW
        && MagFilter           == other.MagFilter
        && MinFilter           == other.MinFilter
        && MipFilter           == other.MipFilter
        && MaxAnisotropy       == other.MaxAnisotropy
        && MipmapLodBias.data  == other.MipmapLodBias.data
        && MaxMipLevel         == other.MaxMipLevel
        && BorderColor         == other.BorderColor
        && Depth               == other.Depth;
  }

}
