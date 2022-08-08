#pragma once

#include "d3d9_resource.h"
#include "../dxso/dxso_module.h"
#include "d3d9_shader_permutations.h"
#include "d3d9_util.h"
#include "d3d9_mem.h"

#include <array>

namespace dxvk {


  /**
   * \brief Common shader object
   * 
   * Stores the compiled SPIR-V shader and the SHA-1
   * hash of the original DXBC shader, which can be
   * used to identify the shader.
   */
  class D3D9CommonShader {

  public:

    D3D9CommonShader();

    D3D9CommonShader(
            D3D9DeviceEx*         pDevice,
            VkShaderStageFlagBits ShaderStage,
      const DxvkShaderKey&        Key,
      const DxsoModuleInfo*       pDxbcModuleInfo,
      const void*                 pShaderBytecode,
      const DxsoAnalysisInfo&     AnalysisInfo,
            DxsoModule*           pModule,
            D3D9MemoryAllocator*  pAllocator);

    Rc<DxvkShader> GetShader(D3D9ShaderPermutation Permutation) const {
      return m_shaders[Permutation];
    }

    std::string GetName() const {
      return m_shaders[D3D9ShaderPermutations::None]->debugName();
    }

    uint32_t GetBytecodeLength() {
      return m_bytecodeLength;
    }

    const void* GetBytecode() {
      m_memory.Map();
      return m_memory.Ptr();
    }

    void UnmapBytecode() {
      m_memory.Unmap();
    }

    const DxsoIsgn& GetIsgn() const {
      return m_isgn;
    }

    const DxsoShaderMetaInfo& GetMeta() const { return m_meta; }
    const DxsoDefinedConstants& GetConstants() const { return m_constants; }

    D3D9ShaderMasks GetShaderMask() const { return D3D9ShaderMasks{ m_usedSamplers, m_usedRTs }; }

    const DxsoProgramInfo& GetInfo() const { return m_info; }

    uint32_t GetMaxDefinedConstant() const { return m_maxDefinedConst; }

  private:

    DxsoIsgn              m_isgn;
    uint32_t              m_usedSamplers;
    uint32_t              m_usedRTs;

    DxsoProgramInfo       m_info;
    DxsoShaderMetaInfo    m_meta;
    DxsoDefinedConstants  m_constants;
    uint32_t              m_maxDefinedConst;

    DxsoPermutations      m_shaders;

    D3D9MemoryAllocator*  m_allocator;
    D3D9Memory            m_memory;
    uint32_t              m_bytecodeLength;

  };

  /**
   * \brief Common shader interface
   * 
   * Implements methods for all D3D11*Shader
   * interfaces and stores the actual shader
   * module object.
   */
  template <typename Base>
  class D3D9Shader : public D3D9DeviceChild<Base> {

  public:

    D3D9Shader(
            D3D9DeviceEx*      pDevice,
            D3D9CommonShader*  pCommonShader)
      : D3D9DeviceChild<Base>( pDevice )
      , m_shader             ( pCommonShader ) { }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) {
      if (ppvObject == nullptr)
        return E_POINTER;

      *ppvObject = nullptr;

      if (riid == __uuidof(IUnknown)
       || riid == __uuidof(Base)) {
        *ppvObject = ref(this);
        return S_OK;
      }

      Logger::warn("D3D9Shader::QueryInterface: Unknown interface query");
      Logger::warn(str::format(riid));
      return E_NOINTERFACE;
    }

    HRESULT STDMETHODCALLTYPE GetFunction(void* pOut, UINT* pSizeOfData) {
      if (pSizeOfData == nullptr)
        return D3DERR_INVALIDCALL;

      uint32_t length = m_shader->GetBytecodeLength();
      if (pOut == nullptr) {
        *pSizeOfData = length;
        return D3D_OK;
      }

      const void* bytecode = m_shader->GetBytecode();

      uint32_t copyAmount = std::min(*pSizeOfData, length);
      std::memcpy(pOut, bytecode, copyAmount);

      m_shader->UnmapBytecode();

      return D3D_OK;
    }

    const D3D9CommonShader* GetCommonShader() const {
      return m_shader;
    }

  private:

    D3D9CommonShader* m_shader;

  };

  // Needs their own classes and not usings for forward decl.

  class D3D9VertexShader final : public D3D9Shader<IDirect3DVertexShader9> {

  public:

    D3D9VertexShader(
            D3D9DeviceEx*      pDevice,
            D3D9CommonShader*  pCommonShader)
      : D3D9Shader<IDirect3DVertexShader9>( pDevice, pCommonShader ) { }

  };

  class D3D9PixelShader final : public D3D9Shader<IDirect3DPixelShader9> {

  public:

    D3D9PixelShader(
            D3D9DeviceEx*      pDevice,
            D3D9CommonShader*  pCommonShader)
      : D3D9Shader<IDirect3DPixelShader9>( pDevice, pCommonShader ) { }

  };

  /**
   * \brief Shader module set
   * 
   * Some applications may compile the same shader multiple
   * times, so we should cache the resulting shader modules
   * and reuse them rather than creating new ones. This
   * class is thread-safe.
   */
  class D3D9ShaderModuleSet : public RcObject {
    
  public:

    D3D9ShaderModuleSet()
      : m_allocator() {}
    
    void GetShaderModule(
            D3D9DeviceEx*         pDevice,
            D3D9CommonShader**    ppShaderModule,
            VkShaderStageFlagBits ShaderStage,
      const DxsoModuleInfo*       pDxbcModuleInfo,
      const void*                 pShaderBytecode);
    
  private:

    D3D9MemoryAllocator m_allocator;
    
    dxvk::mutex m_mutex;
    
    std::unordered_map<
      DxvkShaderKey,
      D3D9CommonShader,
      DxvkHash, DxvkEq> m_modules;
    
  };

  template<typename T>
  const D3D9CommonShader* GetCommonShader(const T& pShader) {
    return pShader != nullptr ? pShader->GetCommonShader() : nullptr;
  }

}