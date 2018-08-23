#pragma once

#include <atomic>
#include <condition_variable>
#include <fstream>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <unordered_set>

#include "dxvk_graphics.h"

namespace dxvk {
  
  class DxvkDevice;
  class DxvkPipelineCompiler;
  class DxvkPipelineManager;
  
  /**
   * \brief State cache header
   * 
   * The header of the state cache file. Stores
   * the cache version and the size, in bytes,
   * of the data structures.
   */
  struct DxvkStateCacheHeader {
    static constexpr uint32_t CurrVersion = 2;
    
    uint32_t version;
    uint32_t keySize;
    
    std::istream& read (std::istream& stream);
    std::ostream& write(std::ostream& stream) const;
  };
  
  /**
   * \brief Pipeline state cache
   * 
   * Stores combinations of shaders which are used together
   * in a pipeline, as well as the full state vectors to
   * create the actual Vulkan pipelines as soon as the
   * SPIR-V shaders are compiled by the game.
   */
  class DxvkStateCache : public RcObject {
    
  public:
    
    DxvkStateCache(
      DxvkDevice*           device,
      DxvkPipelineCompiler* pipeCompiler,
      DxvkPipelineManager*  pipeManager);
    
    ~DxvkStateCache();
    
    /**
     * \brief Caches a pipeline instance
     * 
     * If the pipeline instance is not cached, the new
     * pipeline instance will be written to the cache.
     * \param [in] pipeline The pipeline object
     * \param [in] instance The pipeline instance
     */
    void cachePipelineInstance(
      const Rc<DxvkGraphicsPipeline>&         pipeline,
      const Rc<DxvkGraphicsPipelineInstance>& instance);
    
    /**
     * \brief Provides a shader
     * 
     * Starts asynchronous compilation of Vulkan
     * pipelines for which all shaders are available.
     * \param [in] shader The shader
     */
    void provideShader(
      const Rc<DxvkShader>&                   shader);
    
  private:
    
    struct WriterEntry {
      Rc<DxvkGraphicsPipeline>         pipeline;
      Rc<DxvkGraphicsPipelineInstance> instance;
    };
    
    struct LookupEntry {
      Rc<DxvkShader> shader;
    };
    
    // Pipeline manager and compiler objects
    DxvkDevice*           m_device;
    DxvkPipelineCompiler* m_pipeCompiler;
    DxvkPipelineManager*  m_pipeManager;
    
    // Stores all cached pipeline states. This is read-only
    // after being populated with the cache file's contents
    // upon initialization.
    std::unordered_set<DxvkGraphicsPipelineStateKey, DxvkHash> m_cacheEntries;
    
    // Lookup table which stores pointers to all cached pipeline
    // states that a given shader is used in. This is read-only
    // after being populated upon initialization.
    std::unordered_multimap<DxvkShaderKey, const DxvkGraphicsPipelineStateKey*, DxvkHash> m_shaderEntries;
    
    // Lookup table which maps shader keys to shader objects.
    // Accessed by the lookup thread for reading and writing.
    std::unordered_map<DxvkShaderKey, Rc<DxvkShader>, DxvkHash> m_shaderObjects;
    
    std::atomic<bool> m_stop = { false };
    
    std::mutex              m_writerLock;
    std::condition_variable m_writerCond;
    std::queue<WriterEntry> m_writerQueue;
    std::thread             m_writerThread;
    
    std::mutex              m_lookupLock;
    std::condition_variable m_lookupCond;
    std::queue<LookupEntry> m_lookupQueue;
    std::thread             m_lookupThread;
    
    void insertShaderEntry(
      const DxvkShaderKey&                key,
      const DxvkGraphicsPipelineStateKey* ptr);
    
    const DxvkGraphicsPipelineStateKey* insertEntry(
      const DxvkGraphicsPipelineStateKey& key);
    
    bool lookupShader(
      const DxvkShaderKey&  key,
            Rc<DxvkShader>& shader) const;
    
    void runWriterThread();
    void runLookupThread();
    
    bool readCacheFile();
    
    void initCacheFile();
    
    static std::string getFilePath();
    
  };
  
}
