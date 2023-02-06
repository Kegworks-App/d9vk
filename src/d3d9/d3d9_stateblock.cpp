#include "d3d9_stateblock.h"

#include "d3d9_caps.h"
#include "d3d9_device.h"
#include "d3d9_vertex_declaration.h"
#include "d3d9_buffer.h"
#include "d3d9_shader.h"
#include "d3d9_texture.h"
#include "../util/util_small_vector.h"

#include "d3d9_util.h"

namespace dxvk {

  D3D9CapturedState::D3D9CapturedState() {
    for (uint32_t i = 0; i < streamFreq.size(); i++)
      streamFreq[i] = 1;

    for (uint32_t i = 0; i < enabledLightIndices.size(); i++)
      enabledLightIndices[i] = UINT32_MAX;
  }

  D3D9CapturedState::~D3D9CapturedState() {
    if (unlikely(textures != nullptr)) {
      for (uint32_t i = 0; i < textures->size(); i++)
        TextureChangePrivate((*textures)[i], nullptr);
    }
  }

  D3D9StateBlock::D3D9StateBlock(D3D9DeviceEx* pDevice, D3D9StateBlockType Type)
    : D3D9StateBlockBase(pDevice)
    , m_deviceState     (pDevice->GetRawState()) {
    CaptureType(Type);
  }


  HRESULT STDMETHODCALLTYPE D3D9StateBlock::QueryInterface(
          REFIID  riid,
          void** ppvObject) {
    if (ppvObject == nullptr)
      return E_POINTER;

    *ppvObject = nullptr;

    if (riid == __uuidof(IUnknown)
     || riid == __uuidof(IDirect3DStateBlock9)) {
      *ppvObject = ref(this);
      return S_OK;
    }

    Logger::warn("D3D9StateBlock::QueryInterface: Unknown interface query");
    Logger::warn(str::format(riid));
    return E_NOINTERFACE;
  }


  void D3D9StateBlock::CaptureType(D3D9StateBlockType State) {
    if (State & D3D9StateBlockType::VertexState) {
      for (uint32_t i = 0; i < caps::MaxStreams; i++) {
        const D3D9VBO& vbo = m_deviceState->vertexBuffers[i];
        SetStreamSource(i, vbo.vertexBuffer.ptr(), vbo.offset, vbo.stride);
        SetStreamSourceFreq(i, m_deviceState->streamFreq[i]);
      }

      SetRenderState(D3DRENDERSTATE_CULLMODE, m_deviceState->rs[D3DRENDERSTATE_CULLMODE]);
    }



      /*if (m_captures.flags.test(D3D9CapturedStateFlag::Indices))
        dst->SetIndices(src->indices.ptr());

      if (m_captures.flags.test(D3D9CapturedStateFlag::RenderStates)) {
        for (uint32_t i = 0; i < m_captures.renderStates.dwordCount(); i++) {
          for (uint32_t rs : bit::BitMask(m_captures.renderStates.dword(i))) {
            uint32_t idx = i * 32 + rs;

            if constexpr (std::is_same_v<Src, D3D9DeviceState>)
              dst->SetRenderState(D3DRENDERSTATETYPE(idx), src->renderStates[idx]);
            else
              dst->SetRenderState(D3DRENDERSTATETYPE(idx), (*src->renderStates)[idx]);
          }
        }
      }

      if (m_captures.flags.test(D3D9CapturedStateFlag::SamplerStates)) {
        for (uint32_t samplerIdx : bit::BitMask(m_captures.samplers.dword(0))) {
          for (uint32_t stateIdx : bit::BitMask(m_captures.samplerStates[samplerIdx].dword(0)))
            if constexpr (std::is_same_v<Src, D3D9DeviceState>)
              dst->SetStateSamplerState(samplerIdx, D3DSAMPLERSTATETYPE(stateIdx), src->samplerStates[samplerIdx][stateIdx]);
            else
              dst->SetStateSamplerState(samplerIdx, D3DSAMPLERSTATETYPE(stateIdx), (*src->samplerStates)[samplerIdx][stateIdx]);
        }
      }

      if (m_captures.flags.test(D3D9CapturedStateFlag::VertexBuffers)) {
        for (uint32_t idx : bit::BitMask(m_captures.vertexBuffers.dword(0))) {
          const D3D9VBO* vbo;
          if constexpr (std::is_same_v<Src, D3D9DeviceState>)
            vbo = &src->vertexBuffers[idx];
          else
            vbo = &(*src->vertexBuffers)[idx];

          dst->SetStreamSource(
            idx,
            vbo->vertexBuffer.ptr(),
            vbo->offset,
            vbo->stride);
        }
      }

      if (m_captures.flags.test(D3D9CapturedStateFlag::Material)) {

        if constexpr (std::is_same_v<Src, D3D9DeviceState>)
          dst->SetMaterial(&src->material);
        else
          dst->SetMaterial(src->material.get());
      }

      if (m_captures.flags.test(D3D9CapturedStateFlag::Textures)) {
        for (uint32_t idx : bit::BitMask(m_captures.textures.dword(0))) {
          if constexpr (std::is_same_v<Src, D3D9DeviceState>)
            dst->SetStateTexture(idx, src->textures[idx]);
          else
            dst->SetStateTexture(idx, (*src->textures)[idx]);
        }
      }

      if (m_captures.flags.test(D3D9CapturedStateFlag::VertexShader))
        dst->SetVertexShader(src->vertexShader.ptr());

      if (m_captures.flags.test(D3D9CapturedStateFlag::PixelShader))
        dst->SetPixelShader(src->pixelShader.ptr());

      if (m_captures.flags.test(D3D9CapturedStateFlag::Transforms)) {
        for (uint32_t i = 0; i < m_captures.transforms.dwordCount(); i++) {
          for (uint32_t trans : bit::BitMask(m_captures.transforms.dword(i))) {
            uint32_t idx = i * 32 + trans;

            if constexpr (std::is_same_v<Src, D3D9DeviceState>)
              dst->SetStateTransform(idx, reinterpret_cast<const D3DMATRIX*>(&src->transforms[idx]));
            else
              dst->SetStateTransform(idx, reinterpret_cast<const D3DMATRIX*>(&(*src->transforms)[idx]));
          }
        }
      }

      if (m_captures.flags.test(D3D9CapturedStateFlag::TextureStages)) {
        for (uint32_t stageIdx : bit::BitMask(m_captures.textureStages.dword(0))) {
          for (uint32_t stateIdx : bit::BitMask(m_captures.textureStageStates[stageIdx].dword(0))) {
            if constexpr (std::is_same_v<Src, D3D9DeviceState>)
              dst->SetStateTextureStageState(stageIdx, D3D9TextureStageStateTypes(stateIdx), src->textureStages[stageIdx][stateIdx]);
            else
              dst->SetStateTextureStageState(stageIdx, D3D9TextureStageStateTypes(stateIdx), (*src->textureStages)[stageIdx][stateIdx]);
          }
        }
      }

      if (m_captures.flags.test(D3D9CapturedStateFlag::Viewport))
        dst->SetViewport(&src->viewport);

      if (m_captures.flags.test(D3D9CapturedStateFlag::ScissorRect))
        dst->SetScissorRect(&src->scissorRect);

      if (m_captures.flags.test(D3D9CapturedStateFlag::ClipPlanes)) {
        for (uint32_t idx : bit::BitMask(m_captures.clipPlanes.dword(0)))
          dst->SetClipPlane(idx, src->clipPlanes[idx].coeff);
      }

      if (m_captures.flags.test(D3D9CapturedStateFlag::VsConstants)) {
        if (unlikely(!m_state.consts)) {
          m_state.consts = std::make_unique<D3D9Constants>();
        }

        for (uint32_t i = 0; i < m_captures.vsConsts.fConsts.dwordCount(); i++) {
          for (uint32_t consts : bit::BitMask(m_captures.vsConsts.fConsts.dword(i))) {
            uint32_t idx = i * 32 + consts;

            if constexpr (std::is_same_v<Src, D3D9DeviceState>) {
              dst->SetVertexShaderConstantF(idx, (float*)&src->consts.vs.fConsts[idx], 1);
            } else {
              dst->SetVertexShaderConstantF(idx, (float*)&src->consts->vs.fConsts[idx], 1);
            }
          }
        }

        for (uint32_t i = 0; i < m_captures.vsConsts.iConsts.dwordCount(); i++) {
          for (uint32_t consts : bit::BitMask(m_captures.vsConsts.iConsts.dword(i))) {
            uint32_t idx = i * 32 + consts;

            if constexpr (std::is_same_v<Src, D3D9DeviceState>) {
              dst->SetVertexShaderConstantI(idx, (int*)&src->consts.vs.iConsts[idx], 1);
            } else {
              dst->SetVertexShaderConstantI(idx, (int*)&src->consts->vs.iConsts[idx], 1);
            }
          }
        }

        if (m_captures.vsConsts.bConsts.any()) {
          for (uint32_t i = 0; i < m_captures.vsConsts.bConsts.dwordCount(); i++)
            if constexpr (std::is_same_v<Src, D3D9DeviceState>) {
              dst->SetVertexBoolBitfield(i, m_captures.vsConsts.bConsts.dword(i), src->consts.vs.bConsts[i]);
            } else {
              dst->SetVertexBoolBitfield(i, m_captures.vsConsts.bConsts.dword(i), src->consts->vs.bConsts[i]);
            }
        }
      }

      if (m_captures.flags.test(D3D9CapturedStateFlag::PsConstants)) {
        if (unlikely(!m_state.consts)) {
          m_state.consts = std::make_unique<D3D9Constants>();
        }

        for (uint32_t i = 0; i < m_captures.psConsts.fConsts.dwordCount(); i++) {
          for (uint32_t consts : bit::BitMask(m_captures.psConsts.fConsts.dword(i))) {
            uint32_t idx = i * 32 + consts;

            if constexpr (std::is_same_v<Src, D3D9DeviceState>) {
              dst->SetPixelShaderConstantF(idx, (float*)&src->consts.ps.fConsts[idx], 1);
            } else {
              dst->SetPixelShaderConstantF(idx, (float*)&src->consts->ps.fConsts[idx], 1);
            }
          }
        }

        for (uint32_t i = 0; i < m_captures.psConsts.iConsts.dwordCount(); i++) {
          for (uint32_t consts : bit::BitMask(m_captures.psConsts.iConsts.dword(i))) {
            uint32_t idx = i * 32 + consts;

            if constexpr (std::is_same_v<Src, D3D9DeviceState>) {
              dst->SetPixelShaderConstantI(idx, (int*)&src->consts.ps.iConsts[idx], 1);
            } else {
              dst->SetPixelShaderConstantI(idx, (int*)&src->consts->ps.iConsts[idx], 1);
            }
          }
        }

        if (m_captures.psConsts.bConsts.any()) {
          for (uint32_t i = 0; i < m_captures.psConsts.bConsts.dwordCount(); i++)
            if constexpr (std::is_same_v<Src, D3D9DeviceState>) {
              dst->SetPixelBoolBitfield(i, m_captures.psConsts.bConsts.dword(i), src->consts.ps.bConsts[i]);
            } else {
              dst->SetPixelBoolBitfield(i, m_captures.psConsts.bConsts.dword(i), src->consts->ps.bConsts[i]);
            }
        }
      }

      if (m_captures.flags.test(D3D9CapturedStateFlag::Lights)) {
        for (uint32_t i = 0; i < m_state.lights.size(); i++) {
          if (!m_state.lights[i].has_value())
            continue;

          dst->SetLight(i, &m_state.lights[i].value());
        }
        for (uint32_t i = 0; i < m_captures.lightEnabledChanges.dwordCount(); i++) {
          for (uint32_t consts : bit::BitMask(m_captures.lightEnabledChanges.dword(i))) {
            uint32_t idx = i * 32 + consts;

            dst->LightEnable(idx, m_state.IsLightEnabled(idx));
          }
        }
      }*/

    return D3D_OK;
  }


  HRESULT STDMETHODCALLTYPE D3D9StateBlock::Apply() {
    for (D3D9DeviceCmdChunk& chunk : m_chunks) {
      chunk.ExecuteAll(m_parent);
    }
    return D3D_OK;
  }


  HRESULT D3D9StateBlock::SetVertexDeclaration(D3D9VertexDecl* pDecl) {
    Com<D3D9VertexDecl, false> decl = pDecl;
    Record([
      c_decl = std::move(decl)
    ] (D3D9DeviceEx* device) {
      device->SetVertexDeclaration(c_decl.ptr());
    });
    return D3D_OK;
  }


  HRESULT D3D9StateBlock::SetIndices(D3D9IndexBuffer* pIndexData) {
    Com<D3D9IndexBuffer, false> ibo = pIndexData;
    Record([
      c_ibo = std::move(ibo)
    ] (D3D9DeviceEx* device) {
      device->SetIndices(c_ibo.ptr());
    });
    return D3D_OK;
  }


  HRESULT D3D9StateBlock::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) {
    Record([
      State, Value
    ] (D3D9DeviceEx* device) {
      device->SetRenderState(State, Value);
    });
    return D3D_OK;
  }


  HRESULT D3D9StateBlock::SetStateSamplerState(
          DWORD               StateSampler,
          D3DSAMPLERSTATETYPE Type,
          DWORD               Value) {
    Record([
      StateSampler, Type, Value
    ] (D3D9DeviceEx* device) {
      device->SetStateSamplerState(StateSampler, Type, Value);
    });
    return D3D_OK;
  }


  HRESULT D3D9StateBlock::SetStreamSource(
          UINT                StreamNumber,
          D3D9VertexBuffer*   pStreamData,
          UINT                OffsetInBytes,
          UINT                Stride) {
    Com<D3D9VertexBuffer, false> vbo = pStreamData;
    Record([
      StreamNumber, c_vbo = std::move(vbo), OffsetInBytes, Stride
    ] (D3D9DeviceEx* device) {
      device->SetStreamSource(StreamNumber, c_vbo.ptr(), OffsetInBytes, Stride);
    });
    return D3D_OK;
  }


  HRESULT D3D9StateBlock::SetStreamSourceFreq(UINT StreamNumber, UINT Setting) {
    Record([
      StreamNumber, Setting
    ] (D3D9DeviceEx* device) {
      device->SetStreamSourceFreq(StreamNumber, Setting);
    });
    return D3D_OK;
  }


  HRESULT D3D9StateBlock::SetStateTexture(DWORD StateSampler, IDirect3DBaseTexture9* pTexture) {
    TextureRefPrivate(pTexture, true);
    Record([
      StateSampler, pTexture
    ] (D3D9DeviceEx* device) {
      device->SetStateTexture(StateSampler, pTexture);
      TextureRefPrivate(pTexture, false);
    });
    return D3D_OK;
  }


  HRESULT D3D9StateBlock::SetVertexShader(D3D9VertexShader* pShader) {
    Com<D3D9VertexShader, false> vs = pShader;
    Record([
      c_vs = std::move(vs)
    ] (D3D9DeviceEx* device) {
      device->SetVertexShader(c_vs.ptr());
    });
    return D3D_OK;
  }


  HRESULT D3D9StateBlock::SetPixelShader(D3D9PixelShader* pShader) {
    Com<D3D9PixelShader, false> ps = pShader;
    Record([
      c_ps = std::move(ps)
    ] (D3D9DeviceEx* device) {
      device->SetPixelShader(c_ps.ptr());
    });
    return D3D_OK;
  }


  HRESULT D3D9StateBlock::SetMaterial(const D3DMATERIAL9* pMaterial) {
    Record([
      c_material = *pMaterial
    ] (D3D9DeviceEx* device) {
      device->SetMaterial(&c_material);
    });
    return D3D_OK;
  }


  HRESULT D3D9StateBlock::SetLight(DWORD Index, const D3DLIGHT9* pLight) {
    Record([
      Index, c_light = *pLight
    ] (D3D9DeviceEx* device) {
      device->SetLight(Index, &c_light);
    });
    return D3D_OK;
  }


  HRESULT D3D9StateBlock::LightEnable(DWORD Index, BOOL Enable) {
    Record([
      Index, Enable
    ] (D3D9DeviceEx* device) {
      device->LightEnable(Index, Enable);
    });
    return D3D_OK;
  }


  HRESULT D3D9StateBlock::SetStateTransform(uint32_t idx, const D3DMATRIX* pMatrix) {
    Record([
      idx, c_matrix = *pMatrix
    ] (D3D9DeviceEx* device) {
      device->SetStateTransform(idx, &c_matrix);
    });
    return D3D_OK;
  }


  HRESULT D3D9StateBlock::SetStateTextureStageState(
          DWORD                      Stage,
          D3D9TextureStageStateTypes Type,
          DWORD                      Value) {
    Record([
      Stage, Type, Value
    ] (D3D9DeviceEx* device) {
      device->SetStateTextureStageState(Stage, Type, Value);
    });
    return D3D_OK;
  }


  HRESULT D3D9StateBlock::MultiplyTransform(D3DTRANSFORMSTATETYPE TransformState, const D3DMATRIX* pMatrix) {
    Record([
      TransformState, c_matrix = *pMatrix
    ] (D3D9DeviceEx* device) {
      device->MultiplyTransform(TransformState, &c_matrix);
    });
    return D3D_OK;
  }


  HRESULT D3D9StateBlock::SetViewport(const D3DVIEWPORT9* pViewport) {
    Record([
      c_viewport = *pViewport
    ] (D3D9DeviceEx* device) {
      device->SetViewport(&c_viewport);
    });
    return D3D_OK;
  }


  HRESULT D3D9StateBlock::SetScissorRect(const RECT* pRect) {
    Record([
      c_rect = *pRect
    ] (D3D9DeviceEx* device) {
      device->SetScissorRect(&c_rect);
    });
    return D3D_OK;
  }


  HRESULT D3D9StateBlock::SetClipPlane(DWORD Index, const float* pPlane) {
    std::array<float, 4> plane = { pPlane[0], pPlane[1], pPlane[2], pPlane[3] };
    Record([
      Index, plane
    ] (D3D9DeviceEx* device) {
      device->SetClipPlane(Index, plane.data());
    });
    return D3D_OK;
  }


  HRESULT D3D9StateBlock::SetVertexShaderConstantF(
          UINT   StartRegister,
    const float* pConstantData,
          UINT   Vector4fCount) {
    /*small_vector<float, 4> vec;
    vec.reserve(Vector4fCount * 4);
    std::memcpy(vec.data(), pConstantData, Vector4fCount * 4 * sizeof(float));
    Record([
      StartRegister, Vector4fCount, c_data = std::move(vec)
    ] (D3D9DeviceEx* device) {
      device->SetVertexShaderConstantF(StartRegister, c_data.data(), Vector4fCount);
    });*/

    return SetShaderConstants<
      DxsoProgramTypes::VertexShader,
      D3D9ConstantType::Float>(
        StartRegister,
        pConstantData,
        Vector4fCount);
  }


  HRESULT D3D9StateBlock::SetVertexShaderConstantI(
          UINT StartRegister,
    const int* pConstantData,
          UINT Vector4iCount) {
    return SetShaderConstants<
      DxsoProgramTypes::VertexShader,
      D3D9ConstantType::Int>(
        StartRegister,
        pConstantData,
        Vector4iCount);
  }


  HRESULT D3D9StateBlock::SetVertexShaderConstantB(
          UINT  StartRegister,
    const BOOL* pConstantData,
          UINT  BoolCount) {
    return SetShaderConstants<
      DxsoProgramTypes::VertexShader,
      D3D9ConstantType::Bool>(
        StartRegister,
        pConstantData,
        BoolCount);
  }


  HRESULT D3D9StateBlock::SetPixelShaderConstantF(
          UINT   StartRegister,
    const float* pConstantData,
          UINT   Vector4fCount) {
    return SetShaderConstants<
      DxsoProgramTypes::PixelShader,
      D3D9ConstantType::Float>(
        StartRegister,
        pConstantData,
        Vector4fCount);
  }


  HRESULT D3D9StateBlock::SetPixelShaderConstantI(
          UINT StartRegister,
    const int* pConstantData,
          UINT Vector4iCount) {
    return SetShaderConstants<
      DxsoProgramTypes::PixelShader,
      D3D9ConstantType::Int>(
        StartRegister,
        pConstantData,
        Vector4iCount);
  }


  HRESULT D3D9StateBlock::SetPixelShaderConstantB(
          UINT  StartRegister,
    const BOOL* pConstantData,
          UINT  BoolCount) {
    return SetShaderConstants<
      DxsoProgramTypes::PixelShader,
      D3D9ConstantType::Bool>(
        StartRegister,
        pConstantData,
        BoolCount);
  }


  HRESULT D3D9StateBlock::SetVertexBoolBitfield(uint32_t idx, uint32_t mask, uint32_t bits) {
    if (unlikely(!m_state.consts))
      m_state.consts = std::make_unique<D3D9Constants>();

    m_state.consts->vs.bConsts[idx] &= ~mask;
    m_state.consts->vs.bConsts[idx] |= bits & mask;
    return D3D_OK;
  }


  HRESULT D3D9StateBlock::SetPixelBoolBitfield(uint32_t idx, uint32_t mask, uint32_t bits) {
    Record([idx, mask, bits] (D3D9DeviceEx* Device) {
      Device->SetPixelBoolBitfield(idx, mask, bits);
    });
    return D3D_OK;
  }


  void D3D9StateBlock::CapturePixelRenderStates() {
    m_captures.flags.set(D3D9CapturedStateFlag::RenderStates);

    m_captures.renderStates.set(D3DRS_ZENABLE, true);
    m_captures.renderStates.set(D3DRS_FILLMODE, true);
    m_captures.renderStates.set(D3DRS_SHADEMODE, true);
    m_captures.renderStates.set(D3DRS_ZWRITEENABLE, true);
    m_captures.renderStates.set(D3DRS_ALPHATESTENABLE, true);
    m_captures.renderStates.set(D3DRS_LASTPIXEL, true);
    m_captures.renderStates.set(D3DRS_SRCBLEND, true);
    m_captures.renderStates.set(D3DRS_DESTBLEND, true);
    m_captures.renderStates.set(D3DRS_ZFUNC, true);
    m_captures.renderStates.set(D3DRS_ALPHAREF, true);
    m_captures.renderStates.set(D3DRS_ALPHAFUNC, true);
    m_captures.renderStates.set(D3DRS_DITHERENABLE, true);
    m_captures.renderStates.set(D3DRS_FOGSTART, true);
    m_captures.renderStates.set(D3DRS_FOGEND, true);
    m_captures.renderStates.set(D3DRS_FOGDENSITY, true);
    m_captures.renderStates.set(D3DRS_ALPHABLENDENABLE, true);
    m_captures.renderStates.set(D3DRS_DEPTHBIAS, true);
    m_captures.renderStates.set(D3DRS_STENCILENABLE, true);
    m_captures.renderStates.set(D3DRS_STENCILFAIL, true);
    m_captures.renderStates.set(D3DRS_STENCILZFAIL, true);
    m_captures.renderStates.set(D3DRS_STENCILPASS, true);
    m_captures.renderStates.set(D3DRS_STENCILFUNC, true);
    m_captures.renderStates.set(D3DRS_STENCILREF, true);
    m_captures.renderStates.set(D3DRS_STENCILMASK, true);
    m_captures.renderStates.set(D3DRS_STENCILWRITEMASK, true);
    m_captures.renderStates.set(D3DRS_TEXTUREFACTOR, true);
    m_captures.renderStates.set(D3DRS_WRAP0, true);
    m_captures.renderStates.set(D3DRS_WRAP1, true);
    m_captures.renderStates.set(D3DRS_WRAP2, true);
    m_captures.renderStates.set(D3DRS_WRAP3, true);
    m_captures.renderStates.set(D3DRS_WRAP4, true);
    m_captures.renderStates.set(D3DRS_WRAP5, true);
    m_captures.renderStates.set(D3DRS_WRAP6, true);
    m_captures.renderStates.set(D3DRS_WRAP7, true);
    m_captures.renderStates.set(D3DRS_WRAP8, true);
    m_captures.renderStates.set(D3DRS_WRAP9, true);
    m_captures.renderStates.set(D3DRS_WRAP10, true);
    m_captures.renderStates.set(D3DRS_WRAP11, true);
    m_captures.renderStates.set(D3DRS_WRAP12, true);
    m_captures.renderStates.set(D3DRS_WRAP13, true);
    m_captures.renderStates.set(D3DRS_WRAP14, true);
    m_captures.renderStates.set(D3DRS_WRAP15, true);
    m_captures.renderStates.set(D3DRS_COLORWRITEENABLE, true);
    m_captures.renderStates.set(D3DRS_BLENDOP, true);
    m_captures.renderStates.set(D3DRS_SCISSORTESTENABLE, true);
    m_captures.renderStates.set(D3DRS_SLOPESCALEDEPTHBIAS, true);
    m_captures.renderStates.set(D3DRS_ANTIALIASEDLINEENABLE, true);
    m_captures.renderStates.set(D3DRS_TWOSIDEDSTENCILMODE, true);
    m_captures.renderStates.set(D3DRS_CCW_STENCILFAIL, true);
    m_captures.renderStates.set(D3DRS_CCW_STENCILZFAIL, true);
    m_captures.renderStates.set(D3DRS_CCW_STENCILPASS, true);
    m_captures.renderStates.set(D3DRS_CCW_STENCILFUNC, true);
    m_captures.renderStates.set(D3DRS_COLORWRITEENABLE1, true);
    m_captures.renderStates.set(D3DRS_COLORWRITEENABLE2, true);
    m_captures.renderStates.set(D3DRS_COLORWRITEENABLE3, true);
    m_captures.renderStates.set(D3DRS_BLENDFACTOR, true);
    m_captures.renderStates.set(D3DRS_SRGBWRITEENABLE, true);
    m_captures.renderStates.set(D3DRS_SEPARATEALPHABLENDENABLE, true);
    m_captures.renderStates.set(D3DRS_SRCBLENDALPHA, true);
    m_captures.renderStates.set(D3DRS_DESTBLENDALPHA, true);
    m_captures.renderStates.set(D3DRS_BLENDOPALPHA, true);
  }


  void D3D9StateBlock::CapturePixelSamplerStates() {
    m_captures.flags.set(D3D9CapturedStateFlag::SamplerStates);

    for (uint32_t i = 0; i < caps::MaxTexturesPS + 1; i++) {
      m_captures.samplers.set(i, true);

      m_captures.samplerStates[i].set(D3DSAMP_ADDRESSU, true);
      m_captures.samplerStates[i].set(D3DSAMP_ADDRESSV, true);
      m_captures.samplerStates[i].set(D3DSAMP_ADDRESSW, true);
      m_captures.samplerStates[i].set(D3DSAMP_BORDERCOLOR, true);
      m_captures.samplerStates[i].set(D3DSAMP_MAGFILTER, true);
      m_captures.samplerStates[i].set(D3DSAMP_MINFILTER, true);
      m_captures.samplerStates[i].set(D3DSAMP_MIPFILTER, true);
      m_captures.samplerStates[i].set(D3DSAMP_MIPMAPLODBIAS, true);
      m_captures.samplerStates[i].set(D3DSAMP_MAXMIPLEVEL, true);
      m_captures.samplerStates[i].set(D3DSAMP_MAXANISOTROPY, true);
      m_captures.samplerStates[i].set(D3DSAMP_SRGBTEXTURE, true);
      m_captures.samplerStates[i].set(D3DSAMP_ELEMENTINDEX, true);
    }
  }


  void D3D9StateBlock::CapturePixelShaderStates() {
    m_captures.flags.set(D3D9CapturedStateFlag::PixelShader);
    m_captures.flags.set(D3D9CapturedStateFlag::PsConstants);

    m_captures.psConsts.fConsts.setAll();
    m_captures.psConsts.iConsts.setAll();
    m_captures.psConsts.bConsts.setAll();
  }


  void D3D9StateBlock::CaptureVertexRenderStates() {
    m_captures.flags.set(D3D9CapturedStateFlag::RenderStates);

    m_captures.renderStates.set(D3DRS_CULLMODE, true);
    m_captures.renderStates.set(D3DRS_FOGENABLE, true);
    m_captures.renderStates.set(D3DRS_FOGCOLOR, true);
    m_captures.renderStates.set(D3DRS_FOGTABLEMODE, true);
    m_captures.renderStates.set(D3DRS_FOGSTART, true);
    m_captures.renderStates.set(D3DRS_FOGEND, true);
    m_captures.renderStates.set(D3DRS_FOGDENSITY, true);
    m_captures.renderStates.set(D3DRS_RANGEFOGENABLE, true);
    m_captures.renderStates.set(D3DRS_AMBIENT, true);
    m_captures.renderStates.set(D3DRS_COLORVERTEX, true);
    m_captures.renderStates.set(D3DRS_FOGVERTEXMODE, true);
    m_captures.renderStates.set(D3DRS_CLIPPING, true);
    m_captures.renderStates.set(D3DRS_LIGHTING, true);
    m_captures.renderStates.set(D3DRS_LOCALVIEWER, true);
    m_captures.renderStates.set(D3DRS_EMISSIVEMATERIALSOURCE, true);
    m_captures.renderStates.set(D3DRS_AMBIENTMATERIALSOURCE, true);
    m_captures.renderStates.set(D3DRS_DIFFUSEMATERIALSOURCE, true);
    m_captures.renderStates.set(D3DRS_SPECULARMATERIALSOURCE, true);
    m_captures.renderStates.set(D3DRS_VERTEXBLEND, true);
    m_captures.renderStates.set(D3DRS_CLIPPLANEENABLE, true);
    m_captures.renderStates.set(D3DRS_POINTSIZE, true);
    m_captures.renderStates.set(D3DRS_POINTSIZE_MIN, true);
    m_captures.renderStates.set(D3DRS_POINTSPRITEENABLE, true);
    m_captures.renderStates.set(D3DRS_POINTSCALEENABLE, true);
    m_captures.renderStates.set(D3DRS_POINTSCALE_A, true);
    m_captures.renderStates.set(D3DRS_POINTSCALE_B, true);
    m_captures.renderStates.set(D3DRS_POINTSCALE_C, true);
    m_captures.renderStates.set(D3DRS_MULTISAMPLEANTIALIAS, true);
    m_captures.renderStates.set(D3DRS_MULTISAMPLEMASK, true);
    m_captures.renderStates.set(D3DRS_PATCHEDGESTYLE, true);
    m_captures.renderStates.set(D3DRS_POINTSIZE_MAX, true);
    m_captures.renderStates.set(D3DRS_INDEXEDVERTEXBLENDENABLE, true);
    m_captures.renderStates.set(D3DRS_TWEENFACTOR, true);
    m_captures.renderStates.set(D3DRS_POSITIONDEGREE, true);
    m_captures.renderStates.set(D3DRS_NORMALDEGREE, true);
    m_captures.renderStates.set(D3DRS_MINTESSELLATIONLEVEL, true);
    m_captures.renderStates.set(D3DRS_MAXTESSELLATIONLEVEL, true);
    m_captures.renderStates.set(D3DRS_ADAPTIVETESS_X, true);
    m_captures.renderStates.set(D3DRS_ADAPTIVETESS_Y, true);
    m_captures.renderStates.set(D3DRS_ADAPTIVETESS_Z, true);
    m_captures.renderStates.set(D3DRS_ADAPTIVETESS_W, true);
    m_captures.renderStates.set(D3DRS_ENABLEADAPTIVETESSELLATION, true);
    m_captures.renderStates.set(D3DRS_NORMALIZENORMALS, true);
    m_captures.renderStates.set(D3DRS_SPECULARENABLE, true);
    m_captures.renderStates.set(D3DRS_SHADEMODE, true);
  }


  void D3D9StateBlock::CaptureVertexSamplerStates() {
    m_captures.flags.set(D3D9CapturedStateFlag::SamplerStates);

    for (uint32_t i = caps::MaxTexturesPS + 1; i < SamplerCount; i++) {
      m_captures.samplers.set(i, true);
      m_captures.samplerStates[i].set(D3DSAMP_DMAPOFFSET, true);
    }
  }


  void D3D9StateBlock::CaptureVertexShaderStates() {
    m_captures.flags.set(D3D9CapturedStateFlag::VertexShader);
    m_captures.flags.set(D3D9CapturedStateFlag::VsConstants);

    m_captures.vsConsts.fConsts.setN(m_parent->GetVertexConstantLayout().floatCount);
    m_captures.vsConsts.iConsts.setN(m_parent->GetVertexConstantLayout().intCount);
    m_captures.vsConsts.bConsts.setN(m_parent->GetVertexConstantLayout().boolCount);
  }


  /*void D3D9StateBlock::CaptureType(D3D9StateBlockType Type) {
    if (Type == D3D9StateBlockType::PixelState || Type == D3D9StateBlockType::All) {
      CapturePixelRenderStates();
      CapturePixelSamplerStates();
      CapturePixelShaderStates();

      m_captures.flags.set(D3D9CapturedStateFlag::TextureStages);
      m_captures.textureStages.setAll();
      for (auto& stage : m_captures.textureStageStates)
        stage.setAll();
    }

    if (Type == D3D9StateBlockType::VertexState || Type == D3D9StateBlockType::All) {
      CaptureVertexRenderStates();
      CaptureVertexSamplerStates();
      CaptureVertexShaderStates();

      m_captures.flags.set(D3D9CapturedStateFlag::VertexDecl);
      m_captures.flags.set(D3D9CapturedStateFlag::StreamFreq);
      m_captures.flags.set(D3D9CapturedStateFlag::Lights);
      m_captures.lightEnabledChanges.setN(m_deviceState->lights.size());

      for (uint32_t i = 0; i < caps::MaxStreams; i++)
        m_captures.streamFreq.set(i, true);
    }

    if (Type == D3D9StateBlockType::All) {
      m_captures.flags.set(D3D9CapturedStateFlag::Textures);
      m_captures.textures.setAll();

      m_captures.flags.set(D3D9CapturedStateFlag::VertexBuffers);
      m_captures.vertexBuffers.setAll();

      m_captures.flags.set(D3D9CapturedStateFlag::Indices);
      m_captures.flags.set(D3D9CapturedStateFlag::Viewport);
      m_captures.flags.set(D3D9CapturedStateFlag::ScissorRect);

      m_captures.flags.set(D3D9CapturedStateFlag::ClipPlanes);
      m_captures.clipPlanes.setAll();

      m_captures.flags.set(D3D9CapturedStateFlag::Transforms);
      m_captures.transforms.setAll();

      m_captures.flags.set(D3D9CapturedStateFlag::Material);
    }

    if (Type != D3D9StateBlockType::None)
      this->Capture();
  }*/

  void D3D9DeviceCmdChunk::ExecuteAll(D3D9DeviceEx* Device) {
    auto command = m_head;
    while (command != nullptr)
    {
      auto next = command->Next();
      command->Exec(Device);
      command->~D3D9DeviceCmd();
      command = next;
    }
    m_commandOffset = 0;
    m_head = nullptr;
    m_tail = nullptr;
  }

  D3D9DeviceCmdChunk::~D3D9DeviceCmdChunk() {
    auto cmd = m_head;

    while (cmd != nullptr) {
      auto next = cmd->Next();
      cmd->~D3D9DeviceCmd();
      cmd = next;
    }

    m_head = nullptr;
    m_tail = nullptr;

    m_commandOffset = 0;
  }

}