#pragma once

#include "dxso_modinfo.h"
#include "dxso_decoder.h"
#include <set>
#include <map>

namespace dxvk {

  struct DxsoAnalysisInfo {
    uint32_t bytecodeByteLength;

    bool usesDerivatives        = false;
    bool usesKill               = false;
    bool usesRelativeAddressing = false;

    std::vector<DxsoInstructionContext> coissues;
    std::map<uint32_t, uint32_t> floatConstantMap;
    std::map<uint32_t, uint32_t> intConstantMap;
  };

  class DxsoAnalyzer {

  public:

    DxsoAnalyzer(
            DxsoAnalysisInfo& analysis);

    /**
     * \brief Processes a single instruction
     * \param [in] ins The instruction
     */
    void processInstruction(
      const DxsoInstructionContext& ctx);

    void finalize(size_t tokenCount);

  private:

    DxsoAnalysisInfo* m_analysis = nullptr;

    DxsoOpcode m_parentOpcode;

    std::set<uint32_t> usedFloatConstants;
    std::set<uint32_t> usedIntConstants;

  };

}