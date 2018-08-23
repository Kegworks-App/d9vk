#pragma once

#include <iostream>
#include <mutex>

#include "dxvk_binding.h"
#include "dxvk_constant_state.h"
#include "dxvk_pipecache.h"
#include "dxvk_pipecompiler.h"
#include "dxvk_pipelayout.h"
#include "dxvk_renderpass.h"
#include "dxvk_resource.h"
#include "dxvk_shader.h"
#include "dxvk_stats.h"

namespace dxvk {
  
  class DxvkDevice;
  class DxvkGraphicsPipelineStateKey;
  class DxvkStateCache;
  
  /**
   * \brief Graphics pipeline state info
   * 
   * Stores all information that is required to create
   * a graphics pipeline, except the shader objects
   * themselves. Also used to identify pipelines using
   * the current pipeline state vector.
   */
  struct DxvkGraphicsPipelineStateInfo {
    DxvkGraphicsPipelineStateInfo();
    DxvkGraphicsPipelineStateInfo(
      const DxvkGraphicsPipelineStateInfo& other);
    
    DxvkGraphicsPipelineStateInfo& operator = (
      const DxvkGraphicsPipelineStateInfo& other);
    
    bool operator == (const DxvkGraphicsPipelineStateInfo& other) const;
    bool operator != (const DxvkGraphicsPipelineStateInfo& other) const;
    
    DxvkBindingState                    bsBindingState;
    
    VkPrimitiveTopology                 iaPrimitiveTopology;
    VkBool32                            iaPrimitiveRestart;
    uint32_t                            iaPatchVertexCount;
    
    uint32_t                            ilAttributeCount;
    uint32_t                            ilBindingCount;
    VkVertexInputAttributeDescription   ilAttributes[DxvkLimits::MaxNumVertexAttributes];
    VkVertexInputBindingDescription     ilBindings[DxvkLimits::MaxNumVertexBindings];
    uint32_t                            ilDivisors[DxvkLimits::MaxNumVertexBindings];
    
    VkBool32                            rsDepthClampEnable;
    VkBool32                            rsDepthBiasEnable;
    VkPolygonMode                       rsPolygonMode;
    VkCullModeFlags                     rsCullMode;
    VkFrontFace                         rsFrontFace;
    uint32_t                            rsViewportCount;
    
    VkSampleCountFlagBits               msSampleCount;
    uint32_t                            msSampleMask;
    VkBool32                            msEnableAlphaToCoverage;
    VkBool32                            msEnableAlphaToOne;
    
    VkBool32                            dsEnableDepthTest;
    VkBool32                            dsEnableDepthWrite;
    VkBool32                            dsEnableStencilTest;
    VkCompareOp                         dsDepthCompareOp;
    VkStencilOpState                    dsStencilOpFront;
    VkStencilOpState                    dsStencilOpBack;
    
    VkBool32                            omEnableLogicOp;
    VkLogicOp                           omLogicOp;
    VkPipelineColorBlendAttachmentState omBlendAttachments[MaxNumRenderTargets];
  };
  
  
  /**
   * \brief Common graphics pipeline state
   * 
   * Non-dynamic pipeline state that cannot
   * be changed dynamically.
   */
  struct DxvkGraphicsCommonPipelineStateInfo {
    bool                                msSampleShadingEnable;
    float                               msSampleShadingFactor;
  };
  
  
  /**
   * \brief Graphics pipeline instance
   * 
   * Stores a state vector and the corresponding
   * unoptimized and optimized pipeline handles.
   */
  class DxvkGraphicsPipelineInstance : public RcObject {
    friend class DxvkGraphicsPipeline;
  public:
    
    DxvkGraphicsPipelineInstance(
      const Rc<vk::DeviceFn>&               vkd,
      const DxvkGraphicsPipelineStateInfo&  stateVector,
      const DxvkRenderPass&                 renderPass,
            VkPipeline                      basePipeline);
    
    ~DxvkGraphicsPipelineInstance();
    
    /**
     * \brief Checks for matching pipeline state
     * 
     * \param [in] stateVector Graphics pipeline state
     * \param [in] renderPass Render pass handle
     * \returns \c true if the specialization is compatible
     */
    bool isCompatible(
      const DxvkGraphicsPipelineStateInfo&  stateVector,
            VkRenderPass                    renderPass) const {
      return m_renderPass  == renderPass
          && m_stateVector == stateVector;
    }
    
    /**
     * \brief Sets the optimized pipeline handle
     * 
     * If an optimized pipeline handle has already been
     * set up, this method will fail and the new pipeline
     * handle should be destroyed.
     * \param [in] pipeline The optimized pipeline
     */
    bool setFastPipeline(VkPipeline pipeline) {
      VkPipeline expected = VK_NULL_HANDLE;
      return m_fastPipeline.compare_exchange_strong(expected, pipeline);
    }
    
    /**
     * \brief Retrieves pipeline
     * 
     * Returns the optimized version of the pipeline if
     * if has been set, or the base pipeline if not.
     * \returns The pipeline handle
     */
    VkPipeline getPipeline() const {
      VkPipeline basePipeline = m_basePipeline.load();
      VkPipeline fastPipeline = m_fastPipeline.load();
      
      return fastPipeline != VK_NULL_HANDLE
        ? fastPipeline : basePipeline;
    }
    
  private:
    
    const Rc<vk::DeviceFn> m_vkd;
    
    DxvkGraphicsPipelineStateInfo m_stateVector;
    DxvkRenderPassFormat          m_renderPassFormat;
    VkRenderPass                  m_renderPass;
    
    std::atomic<VkPipeline> m_basePipeline;
    std::atomic<VkPipeline> m_fastPipeline;
    
  };
  
  
  /**
   * \brief Graphics pipeline
   * 
   * Stores the pipeline layout as well as methods to
   * recompile the graphics pipeline against a given
   * pipeline state vector.
   */
  class DxvkGraphicsPipeline : public DxvkResource {
    
  public:
    
    DxvkGraphicsPipeline(
      const DxvkDevice*               device,
      const Rc<DxvkPipelineCache>&    cache,
      const Rc<DxvkPipelineCompiler>& compiler,
      const Rc<DxvkStateCache>&       stateCache,
      const Rc<DxvkShader>&           vs,
      const Rc<DxvkShader>&           tcs,
      const Rc<DxvkShader>&           tes,
      const Rc<DxvkShader>&           gs,
      const Rc<DxvkShader>&           fs);
    
    ~DxvkGraphicsPipeline();
    
    /**
     * \brief Pipeline layout
     * 
     * Stores the pipeline layout and the descriptor set
     * layout, as well as information on the resource
     * slots used by the pipeline.
     * \returns Pipeline layout
     */
    DxvkPipelineLayout* layout() const {
      return m_layout.ptr();
    }
    
    /**
     * \brief Pipeline handle
     * 
     * Retrieves a pipeline handle for the given pipeline
     * state. If necessary, a new pipeline will be created.
     * \param [in] state Pipeline state vector
     * \param [in] renderPass The render pass
     * \param [in,out] stats Stat counter
     * \returns Pipeline handle
     */
    VkPipeline getPipelineHandle(
      const DxvkGraphicsPipelineStateInfo&    state,
      const DxvkRenderPass&                   renderPass,
            DxvkStatCounters&                 stats);
    
    /**
     * \brief Compiles optimized pipeline
     * 
     * Compiles an optimized version of a pipeline and makes
     * it available to the system. This method can fail if
     * another thread finished compiling an optimized pipeline
     * for the same instance before the calling thread.
     * \param [in] instance The pipeline instance
     * \returns \c true on success
     */
    bool compileInstance(
      const Rc<DxvkGraphicsPipelineInstance>& instance);
    
    /**
     * \brief Computes the state key for a given instance
     * 
     * The pipeline state key contains the shader keys and
     * a copy of the full state vector of the instance. It
     * is used for the pipeline state cache.
     * \param [in] instance The pipeline instance
     * \returns The pipeline state key
     */
    DxvkGraphicsPipelineStateKey getInstanceKey(
      const Rc<DxvkGraphicsPipelineInstance>& instance);
    
  private:
    
    struct PipelineStruct {
      DxvkGraphicsPipelineStateInfo stateVector;
      VkRenderPass                  renderPass;
      VkPipeline                    pipeline;
    };
    
    const DxvkDevice* const m_device;
    const Rc<vk::DeviceFn>  m_vkd;
    
    Rc<DxvkPipelineCache>     m_cache;
    Rc<DxvkPipelineCompiler>  m_compiler;
    Rc<DxvkStateCache>        m_stateCache;
    Rc<DxvkPipelineLayout>    m_layout;
    
    Rc<DxvkShaderModule>  m_vs;
    Rc<DxvkShaderModule>  m_tcs;
    Rc<DxvkShaderModule>  m_tes;
    Rc<DxvkShaderModule>  m_gs;
    Rc<DxvkShaderModule>  m_fs;
    
    uint32_t m_vsIn  = 0;
    uint32_t m_fsOut = 0;
    
    DxvkGraphicsCommonPipelineStateInfo m_common;
    
    // List of pipeline instances, shared between threads
    alignas(CACHE_LINE_SIZE) sync::Spinlock       m_mutex;
    std::vector<Rc<DxvkGraphicsPipelineInstance>> m_pipelines;
    
    // Pipeline handles used for derivative pipelines
    std::atomic<VkPipeline> m_basePipelineBase = { VK_NULL_HANDLE };
    std::atomic<VkPipeline> m_fastPipelineBase = { VK_NULL_HANDLE };
    
    DxvkShaderKey getShaderKey(
      const Rc<DxvkShaderModule>&          module) const;
    
    DxvkGraphicsPipelineInstance* findInstance(
      const DxvkGraphicsPipelineStateInfo& state,
            VkRenderPass                   renderPass) const;
    
    VkPipeline compilePipeline(
      const DxvkGraphicsPipelineStateInfo& state,
            VkRenderPass                   renderPass,
            VkPipelineCreateFlags          createFlags,
            VkPipeline                     baseHandle) const;
    
    bool validatePipelineState(
      const DxvkGraphicsPipelineStateInfo& state) const;
    
    void logPipelineState(
            LogLevel                       level,
      const DxvkGraphicsPipelineStateInfo& state) const;
    
  };
  
  
  /**
   * \brief Graphics pipeline key
   * 
   * Stores keys for all shaders that are part
   * of a graphics pipeline, as well as the full
   * state vector.
   */
  class DxvkGraphicsPipelineStateKey {
    
  public:
    
    DxvkGraphicsPipelineStateKey() { }
    DxvkGraphicsPipelineStateKey(
      const DxvkShaderKey&                  vsKey,
      const DxvkShaderKey&                  tcsKey,
      const DxvkShaderKey&                  tesKey,
      const DxvkShaderKey&                  gsKey,
      const DxvkShaderKey&                  psKey,
      const DxvkGraphicsPipelineStateInfo&  state,
      const DxvkRenderPassFormat&           format)
    : m_vs(vsKey), m_tcs(tcsKey), m_tes(tesKey),
      m_gs(gsKey), m_ps(psKey), m_state(state),
      m_format(format) { }
    
    DxvkShaderKey vsKey () const { return m_vs;  }
    DxvkShaderKey tcsKey() const { return m_tcs; }
    DxvkShaderKey tesKey() const { return m_tes; }
    DxvkShaderKey gsKey () const { return m_gs;  }
    DxvkShaderKey psKey () const { return m_ps;  }
    
    const DxvkRenderPassFormat& renderPassFormat() const {
      return m_format;
    }
    
    const DxvkGraphicsPipelineStateInfo& stateVector() const {
      return m_state;
    }
    
    bool operator == (const DxvkGraphicsPipelineStateKey& other) const;
    bool operator != (const DxvkGraphicsPipelineStateKey& other) const;
    
    size_t hash() const;
    
    std::istream& read (std::istream& stream);
    std::ostream& write(std::ostream& stream) const;
    
  private:
    
    DxvkShaderKey m_vs;
    DxvkShaderKey m_tcs;
    DxvkShaderKey m_tes;
    DxvkShaderKey m_gs;
    DxvkShaderKey m_ps;
    
    DxvkGraphicsPipelineStateInfo m_state;
    DxvkRenderPassFormat          m_format;
    
  };
  
}