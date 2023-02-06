#pragma once

#include "d3d9_device_child.h"
#include "d3d9_device.h"
#include "d3d9_state.h"

#include "../util/util_bit.h"

namespace dxvk {

  class D3D9DeviceCmd {

  public:
    virtual ~D3D9DeviceCmd() { }
    D3D9DeviceCmd* Next() const {
      return m_next;
    }
    void SetNext(D3D9DeviceCmd* Next) {
      m_next = Next;
    }
    virtual void Exec(D3D9DeviceEx* Device) = 0;

  private:
    D3D9DeviceCmd* m_next = nullptr;

  };


  template<typename T>
  class alignas(16) D3D9DeviceTypedCmd : public D3D9DeviceCmd {

  public:
    D3D9DeviceTypedCmd(T&& Cmd)
    : m_command(std::move(Cmd)) { }

    D3D9DeviceTypedCmd             (D3D9DeviceTypedCmd&&) = delete;
    D3D9DeviceTypedCmd& operator = (D3D9DeviceTypedCmd&&) = delete;

    void Exec(D3D9DeviceEx* Device) {
      m_command(Device);
    }

  private:
    T m_command;
  };

    /**
   * \brief Command chunk
   *
   * Stores a list of commands.
   */
  class D3D9DeviceCmdChunk : public RcObject {
    constexpr static size_t MaxBlockSize = 256;
  public:

    D3D9DeviceCmdChunk();
    ~D3D9DeviceCmdChunk();

    /**
     * \brief Checks whether the chunk is empty
     * \returns \c true if the chunk is empty
     */
    bool Empty() const {
      return m_commandOffset == 0;
    }

    /**
     * \brief Tries to add a command to the chunk
     * 
     * If the given command can be added to the chunk, it
     * will be consumed. Otherwise, a new chunk must be
     * created which is large enough to hold the command.
     * \param [in] command The command to add
     * \returns \c true on success, \c false if
     *          a new chunk needs to be allocated
     */
    template<typename T>
    bool Push(T& command) {
      using FuncType = D3D9DeviceTypedCmd<T>;

      if (unlikely(m_commandOffset > MaxBlockSize - sizeof(FuncType)))
        return false;

      D3D9DeviceCmd* tail = m_tail;

      m_tail = new (m_data + m_commandOffset)
        FuncType(std::move(command));

      if (likely(tail != nullptr))
        tail->SetNext(m_tail);
      else
        m_head = m_tail;

      m_commandOffset += sizeof(FuncType);
      return true;
    }

    /**
     * \brief Executes all commands
     * 
     * This will also reset the chunk
     * so that it can be reused.
     * \param [in] ctx The context
     */
    void ExecuteAll(D3D9DeviceEx* Device);

  private:

    size_t m_commandOffset = 0;

    D3D9DeviceCmd* m_head = nullptr;
    D3D9DeviceCmd* m_tail = nullptr;

    alignas(64)
    char m_data[MaxBlockSize];

  };

  enum class D3D9CapturedStateFlag : uint32_t {
    VertexDecl,
    Indices,
    RenderStates,
    SamplerStates,
    VertexBuffers,
    Textures,
    VertexShader,
    PixelShader,
    Viewport,
    ScissorRect,
    ClipPlanes,
    VsConstants,
    PsConstants,
    StreamFreq,
    Transforms,
    TextureStages,
    Material,
    Lights
  };

  using D3D9CapturedStateFlags = Flags<D3D9CapturedStateFlag>;

  struct D3D9StateCaptures {
    D3D9CapturedStateFlags flags;

    bit::bitset<RenderStateCount>                       renderStates;

    bit::bitset<SamplerCount>                           samplers;
    std::array<
      bit::bitset<SamplerStateCount>,
      SamplerCount>                                     samplerStates;

    bit::bitset<caps::MaxStreams>                       vertexBuffers;
    bit::bitset<SamplerCount>                           textures;
    bit::bitset<caps::MaxClipPlanes>                    clipPlanes;
    bit::bitset<caps::MaxStreams>                       streamFreq;
    bit::bitset<caps::MaxTransforms>                    transforms;
    bit::bitset<caps::TextureStageCount>                textureStages;
    std::array<
      bit::bitset<TextureStageStateCount>,
      caps::TextureStageCount>                          textureStageStates;

    struct {
      bit::bitset<caps::MaxFloatConstantsSoftware>      fConsts;
      bit::bitset<caps::MaxOtherConstantsSoftware>      iConsts;
      bit::bitset<caps::MaxOtherConstantsSoftware>      bConsts;
    } vsConsts;

    struct {
      bit::bitset<caps::MaxFloatConstantsPS>            fConsts;
      bit::bitset<caps::MaxOtherConstants>              iConsts;
      bit::bitset<caps::MaxOtherConstants>              bConsts;
    } psConsts;

    bit::bitvector                                      lightEnabledChanges;
  };

  struct D3D9CapturedState {
    typedef typename std::array<DWORD, RenderStateCount> RenderStatesArray;
    typedef typename std::array<std::array<DWORD, SamplerStateCount>, SamplerCount> SamplerStatesArray;
    typedef typename std::array<D3D9VBO, caps::MaxStreams> VertexBuffersArray;
    typedef typename std::array<IDirect3DBaseTexture9*, SamplerCount> TexturesArray;
    typedef typename std::array<std::array<DWORD, TextureStageStateCount>, caps::TextureStageCount> TextureStagesArray;
    typedef typename std::array<Matrix4, caps::MaxTransforms> TransformsArray;

    D3D9CapturedState();

    ~D3D9CapturedState();

    Com<D3D9VertexDecl,  false>                      vertexDecl;
    Com<D3D9IndexBuffer, false>                      indices;

    std::unique_ptr<RenderStatesArray>               renderStates = nullptr;

    std::unique_ptr<SamplerStatesArray>              samplerStates = nullptr;

    std::unique_ptr<VertexBuffersArray>              vertexBuffers = nullptr;

    std::unique_ptr<TexturesArray>                   textures = nullptr;

    Com<D3D9VertexShader, false>                     vertexShader;
    Com<D3D9PixelShader,  false>                     pixelShader;

    D3DVIEWPORT9                                     viewport = {};
    RECT                                             scissorRect = {};

    std::array<
      D3D9ClipPlane,
      caps::MaxClipPlanes>                           clipPlanes = {};

    std::unique_ptr<TextureStagesArray>              textureStages = nullptr;

    std::unique_ptr<D3D9Constants>                   consts = nullptr;

    std::array<UINT, caps::MaxStreams>               streamFreq = {};

    std::unique_ptr<TransformsArray>                 transforms = nullptr;

    std::unique_ptr<D3DMATERIAL9>                    material = nullptr;

    std::vector<std::optional<D3DLIGHT9>>            lights;
    std::array<DWORD, caps::MaxEnabledLights>        enabledLightIndices;

    bool IsLightEnabled(DWORD Index) {
      const auto& indices = enabledLightIndices;
      return std::find(indices.begin(), indices.end(), Index) != indices.end();
    }
  };

  // Uses different indices than D3DSTATEBLOCKTYPE to act as flags
  enum class D3D9StateBlockType : uint32_t {
    None,
    VertexState = 1,
    PixelState = 2,
    All = 3
  };

  inline D3D9StateBlockType ConvertStateBlockType(D3DSTATEBLOCKTYPE type) {
    switch (type) {
      case D3DSBT_PIXELSTATE:  return D3D9StateBlockType::PixelState;
      case D3DSBT_VERTEXSTATE: return D3D9StateBlockType::VertexState;
      default:
      case D3DSBT_ALL:         return D3D9StateBlockType::All;
    }
  }

  using D3D9StateBlockBase = D3D9DeviceChild<IDirect3DStateBlock9>;
  class D3D9StateBlock : public D3D9StateBlockBase {

  public:

    D3D9StateBlock(D3D9DeviceEx* pDevice, D3D9StateBlockType Type);

    template<typename Cmd>
    void Record(Cmd&& command) {

    }

    HRESULT STDMETHODCALLTYPE QueryInterface(
        REFIID  riid,
        void** ppvObject) final;

    HRESULT STDMETHODCALLTYPE Capture() final;
    HRESULT STDMETHODCALLTYPE Apply() final;

    HRESULT SetVertexDeclaration(D3D9VertexDecl* pDecl);

    HRESULT SetIndices(D3D9IndexBuffer* pIndexData);

    HRESULT SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);

    HRESULT SetStateSamplerState(
            DWORD               StateSampler,
            D3DSAMPLERSTATETYPE Type,
            DWORD               Value);

    HRESULT SetStreamSource(
            UINT               StreamNumber,
            D3D9VertexBuffer*  pStreamData,
            UINT               OffsetInBytes,
            UINT               Stride);

    HRESULT SetStreamSourceFreq(UINT StreamNumber, UINT Setting);

    HRESULT SetStateTexture(DWORD StateSampler, IDirect3DBaseTexture9* pTexture);

    HRESULT SetVertexShader(D3D9VertexShader* pShader);

    HRESULT SetPixelShader(D3D9PixelShader* pShader);

    HRESULT SetMaterial(const D3DMATERIAL9* pMaterial);

    HRESULT SetLight(DWORD Index, const D3DLIGHT9* pLight);

    HRESULT LightEnable(DWORD Index, BOOL Enable);

    HRESULT SetStateTransform(uint32_t idx, const D3DMATRIX* pMatrix);

    HRESULT SetStateTextureStageState(
            DWORD                      Stage,
            D3D9TextureStageStateTypes Type,
            DWORD                      Value);

    HRESULT MultiplyTransform(D3DTRANSFORMSTATETYPE TransformState, const D3DMATRIX* pMatrix);

    HRESULT SetViewport(const D3DVIEWPORT9* pViewport);

    HRESULT SetScissorRect(const RECT* pRect);

    HRESULT SetClipPlane(DWORD Index, const float* pPlane);


    HRESULT SetVertexShaderConstantF(
            UINT   StartRegister,
      const float* pConstantData,
            UINT   Vector4fCount);

    HRESULT SetVertexShaderConstantI(
            UINT StartRegister,
      const int* pConstantData,
            UINT Vector4iCount);

    HRESULT SetVertexShaderConstantB(
            UINT  StartRegister,
      const BOOL* pConstantData,
            UINT  BoolCount);


    HRESULT SetPixelShaderConstantF(
            UINT   StartRegister,
      const float* pConstantData,
            UINT   Vector4fCount);

    HRESULT SetPixelShaderConstantI(
            UINT StartRegister,
      const int* pConstantData,
            UINT Vector4iCount);

    HRESULT SetPixelShaderConstantB(
            UINT  StartRegister,
      const BOOL* pConstantData,
            UINT  BoolCount);

    enum class D3D9StateFunction {
      Apply,
      Capture
    };

    template <typename Dst, typename Src>
    void ApplyOrCapture(Dst* dst, const Src* src) {
      if (m_captures.flags.test(D3D9CapturedStateFlag::StreamFreq)) {
        for (uint32_t idx : bit::BitMask(m_captures.streamFreq.dword(0)))
          dst->SetStreamSourceFreq(idx, src->streamFreq[idx]);
      }

      if (m_captures.flags.test(D3D9CapturedStateFlag::Indices))
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
      }
    }

    template <D3D9StateFunction Func>
    void ApplyOrCapture() {
      if      constexpr (Func == D3D9StateFunction::Apply)
        ApplyOrCapture(m_parent, &m_state);
      else if constexpr (Func == D3D9StateFunction::Capture)
        ApplyOrCapture(this, m_deviceState);
    }

    template <
      DxsoProgramType  ProgramType,
      D3D9ConstantType ConstantType,
      typename         T>
    HRESULT SetShaderConstants(
            UINT  StartRegister,
      const T*    pConstantData,
            UINT  Count) {
      if (unlikely(!m_state.consts))
        m_state.consts = std::make_unique<D3D9Constants>();


      auto SetHelper = [&](auto& setCaptures) {
        if constexpr (ProgramType == DxsoProgramTypes::VertexShader)
          m_captures.flags.set(D3D9CapturedStateFlag::VsConstants);
        else
          m_captures.flags.set(D3D9CapturedStateFlag::PsConstants);

        for (uint32_t i = 0; i < Count; i++) {
          uint32_t reg = StartRegister + i;
          if      constexpr (ConstantType == D3D9ConstantType::Float)
            setCaptures.fConsts.set(reg, true);
          else if constexpr (ConstantType == D3D9ConstantType::Int)
            setCaptures.iConsts.set(reg, true);
          else if constexpr (ConstantType == D3D9ConstantType::Bool)
            setCaptures.bConsts.set(reg, true);
        }

        UpdateStateConstants<
          ProgramType,
          ConstantType,
          T>(
            m_state.consts.get(),
            StartRegister,
            pConstantData,
            Count,
            false);

        return D3D_OK;
      };

      return ProgramType == DxsoProgramTypes::VertexShader
        ? SetHelper(m_captures.vsConsts)
        : SetHelper(m_captures.psConsts);
    }

    HRESULT SetVertexBoolBitfield(uint32_t idx, uint32_t mask, uint32_t bits);
    HRESULT SetPixelBoolBitfield (uint32_t idx, uint32_t mask, uint32_t bits);

    inline bool IsApplying() {
      return m_applying;
    }

  private:

    void CapturePixelRenderStates();
    void CapturePixelSamplerStates();
    void CapturePixelShaderStates();

    void CaptureVertexRenderStates();
    void CaptureVertexSamplerStates();
    void CaptureVertexShaderStates();

    void CaptureType(D3D9StateBlockType State);

    template <typename T>
    void Record(T& Command)
    {
      if (likely(m_chunks.back().Push(Command)))
        return;

      m_chunks.emplace_back();
      m_chunks.back().Push(Command);
    }

    D3D9CapturedState    m_state;
    D3D9StateCaptures    m_captures;

    D3D9DeviceState*     m_deviceState;

    bool                 m_applying = false;

    std::vector<D3D9DeviceCmdChunk> m_chunks;

  };

}