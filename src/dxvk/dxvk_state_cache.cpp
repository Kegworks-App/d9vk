#include "dxvk_device.h"
#include "dxvk_pipecompiler.h"
#include "dxvk_pipemanager.h"
#include "dxvk_state_cache.h"

#include "../util/sha1/sha1_util.h"

namespace dxvk {
  
  std::istream& DxvkStateCacheHeader::read(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&version), sizeof(version));
    stream.read(reinterpret_cast<char*>(&keySize), sizeof(keySize));
    return stream;
  }
  
  
  std::ostream& DxvkStateCacheHeader::write(std::ostream& stream) const {
    stream.write(reinterpret_cast<const char*>(&version), sizeof(version));
    stream.write(reinterpret_cast<const char*>(&keySize), sizeof(keySize));
    return stream;
  }
  
  
  DxvkStateCache::DxvkStateCache(
    DxvkDevice*           device,
    DxvkPipelineCompiler* pipeCompiler,
    DxvkPipelineManager*  pipeManager)
  : m_device      (device),
    m_pipeCompiler(pipeCompiler),
    m_pipeManager (pipeManager),
    m_writerThread([this] { runWriterThread(); }),
    m_lookupThread([this] { runLookupThread(); }) {
    if (!this->readCacheFile())
      this->initCacheFile();
  }
  
  
  DxvkStateCache::~DxvkStateCache() {
    { std::unique_lock<std::mutex> writerLock(m_writerLock);
      std::unique_lock<std::mutex> lookupLock(m_lookupLock);
      
      m_stop.store(true);
      
      m_writerCond.notify_all();
      m_lookupCond.notify_all();
    }
    
    m_writerThread.join();
    m_lookupThread.join();
  }
  
  
  void DxvkStateCache::cachePipelineInstance(
    const Rc<DxvkGraphicsPipeline>&         pipeline,
    const Rc<DxvkGraphicsPipelineInstance>& instance) {
    { std::unique_lock<std::mutex> writerLock(m_writerLock);
      
      m_writerQueue.push({ pipeline, instance });
      m_writerCond.notify_one();
    }
  }
  
  
  void DxvkStateCache::provideShader(
    const Rc<DxvkShader>&                   shader) {
    { std::unique_lock<std::mutex> lookupLock(m_lookupLock);
      
      m_lookupQueue.push({ shader });
      m_lookupCond.notify_one();
    }
  }
  
  
  void DxvkStateCache::insertShaderEntry(
    const DxvkShaderKey&                key,
    const DxvkGraphicsPipelineStateKey* ptr) {
    if (key.isDefined())
      m_shaderEntries.insert({ key, ptr });
  }
  
  
  const DxvkGraphicsPipelineStateKey* DxvkStateCache::insertEntry(
    const DxvkGraphicsPipelineStateKey& key) {
    auto entry = m_cacheEntries.insert(key);
    return entry.second ? &(*entry.first) : nullptr;
  }
  
  
  bool DxvkStateCache::lookupShader(
    const DxvkShaderKey&  key,
          Rc<DxvkShader>& shader) const {
    if (key.isDefined()) {
      auto entry = m_shaderObjects.find(key);
      
      if (entry != m_shaderObjects.end()) {
        shader = entry->second;
        return true;
      } return false;
    } else {
      shader = nullptr;
      return true;
    }
  }
  
  
  void DxvkStateCache::runWriterThread() {
    std::ofstream fileStream(getFilePath(),
      std::ios_base::binary | std::ios_base::app);
    
    while (!m_stop.load()) {
      WriterEntry entry;
      
      { std::unique_lock<std::mutex> lock(m_writerLock);
        
        m_writerCond.wait(lock, [this] {
          return m_stop.load()
              || m_writerQueue.size() > 0;
        });
        
        if (m_writerQueue.size() > 0) {
          entry = std::move(m_writerQueue.front());
          m_writerQueue.pop();
        }
      }
      
      if (entry.pipeline == nullptr)
        continue;
      
      // Write pipeline key to the cache file if not already cached
      DxvkGraphicsPipelineStateKey key = entry.pipeline->getInstanceKey(entry.instance);
      
      if (m_cacheEntries.find(key) == m_cacheEntries.end()) {
        Sha1Hash hash = Sha1Hash::compute(
        reinterpret_cast<const uint8_t*>(&key),
        sizeof(key));

        fileStream.write(reinterpret_cast<const char*>(&hash), sizeof(hash));

        key.write(fileStream);
      }
    }
  }
  
  
  void DxvkStateCache::runLookupThread() {
    while (!m_stop.load()) {
      LookupEntry entry;
      
      { std::unique_lock<std::mutex> lock(m_lookupLock);
        
        m_lookupCond.wait(lock, [this] {
          return m_stop.load()
              || m_lookupQueue.size() > 0;
        });
        
        if (m_lookupQueue.size() > 0) {
          entry = std::move(m_lookupQueue.front());
          m_lookupQueue.pop();
        }
      }
      
      if (entry.shader == nullptr)
        continue;
      
      // Make the shader object visible for future lookups
      m_shaderObjects.insert({ entry.shader->key(), entry.shader });
      
      // Start compiling pipelines for which all shaders
      // became available with the newly added shader
      auto eq = m_shaderEntries.equal_range(entry.shader->key());
      
      for (auto i = eq.first; i != eq.second; i++) {
        const DxvkGraphicsPipelineStateKey* curr = i->second;
        
        // Look up shaders. If one fails, skip the pipeline.
        Rc<DxvkShader> vs, tcs, tes, gs, ps;
        
        if (!this->lookupShader(curr->vsKey(),  vs)
         || !this->lookupShader(curr->tcsKey(), tcs)
         || !this->lookupShader(curr->tesKey(), tes)
         || !this->lookupShader(curr->gsKey(),  gs)
         || !this->lookupShader(curr->psKey(),  ps))
          continue;
        
        // Create the pipeline object with the given set of shaders,
        // and then compile a pipeline instance with the given state
        // vector asynchronously.
        auto pipeline = m_pipeManager->createGraphicsPipeline(vs, tcs, tes, gs, ps);
        auto renderPass = m_device->createRenderPass(curr->renderPassFormat());
        
        // Create and compile an optimized pipeline instance
        Rc<DxvkGraphicsPipelineInstance> instance = new DxvkGraphicsPipelineInstance(
          m_device->vkd(), curr->stateVector(), *renderPass, VK_NULL_HANDLE);
        
        Logger::info("Compiling from state cache");
        m_pipeCompiler->queueCompilation(pipeline, instance);
      }
    }
  }
  
  
  bool DxvkStateCache::readCacheFile() {
    std::ifstream fileStream(getFilePath(), std::ios_base::binary);
    
    if (!fileStream)
      return false;
    
    DxvkStateCacheHeader header = { };
    header.read(fileStream);
    
    if (header.version != DxvkStateCacheHeader::CurrVersion
     || header.keySize != sizeof(DxvkGraphicsPipelineStateKey))
      return false;
    
    // Read as many entries from the file as
    // possible and fill the data structures
    while (fileStream) {
      Sha1Hash expectedHash;
      fileStream.read(reinterpret_cast<char*>(&expectedHash), sizeof(expectedHash));

      DxvkGraphicsPipelineStateKey key;
      key.read(fileStream);

      Sha1Hash actualHash = Sha1Hash::compute(
      reinterpret_cast<const uint8_t*>(&key),
      sizeof(key));
      
      if (!fileStream)
        break;

      if (!(actualHash == expectedHash)) {
        Logger::debug("Skipping shader due to hash");
        continue;
      }
      
      const DxvkGraphicsPipelineStateKey* ptr = this->insertEntry(key);
      
      if (ptr != nullptr) {
        this->insertShaderEntry(key.vsKey(),  ptr);
        this->insertShaderEntry(key.tcsKey(), ptr);
        this->insertShaderEntry(key.tesKey(), ptr);
        this->insertShaderEntry(key.gsKey(),  ptr);
        this->insertShaderEntry(key.psKey(),  ptr);
      }
    }
    
    Logger::info(str::format(
      "DxvkStateCache: Read ", m_cacheEntries.size(), " entries"));
    return true;
  }
  
  
  void DxvkStateCache::initCacheFile() {
    Logger::info("DxvkStateCache: Initializing cache file");
    
    std::ofstream fileStream(getFilePath(),
      std::ios_base::binary | std::ios_base::trunc);
    
    DxvkStateCacheHeader header;
    header.version = DxvkStateCacheHeader::CurrVersion;
    header.keySize = sizeof(DxvkGraphicsPipelineStateKey);
    header.write(fileStream);
  }
  
  
  std::string DxvkStateCache::getFilePath() {
    std::string tempDir = env::getTempDirectory();
    
    if (tempDir.size() == 0)
      return std::string();
    
    std::string exeName = env::getExeName();
    std::string exeHash = Sha1Hash::compute(
      reinterpret_cast<const uint8_t*>(exeName.c_str()),
      exeName.size()).toString();
    return str::format(tempDir, exeHash, ".scache");
  }
  
}
