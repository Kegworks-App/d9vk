#include "dxso_analysis.h"

namespace dxvk {

  DxsoAnalyzer::DxsoAnalyzer(
    DxsoAnalysisInfo& analysis)
    : m_analysis(&analysis) { }

  void DxsoAnalyzer::processInstruction(
    const DxsoInstructionContext& ctx) {
    DxsoOpcode opcode = ctx.instruction.opcode;

    // Co-issued CNDs are issued before their parents,
    // except when the parent is a CND.
    if (opcode == DxsoOpcode::Cnd &&
        m_parentOpcode != DxsoOpcode::Cnd &&
        ctx.instruction.coissue) {
      m_analysis->coissues.push_back(ctx);
    }

    if (opcode == DxsoOpcode::TexKill)
      m_analysis->usesKill = true;

    if (opcode == DxsoOpcode::DsX
     || opcode == DxsoOpcode::DsY

     || opcode == DxsoOpcode::Tex
     || opcode == DxsoOpcode::TexCoord
     || opcode == DxsoOpcode::TexBem
     || opcode == DxsoOpcode::TexBemL
     || opcode == DxsoOpcode::TexReg2Ar
     || opcode == DxsoOpcode::TexReg2Gb
     || opcode == DxsoOpcode::TexM3x2Pad
     || opcode == DxsoOpcode::TexM3x2Tex
     || opcode == DxsoOpcode::TexM3x3Pad
     || opcode == DxsoOpcode::TexM3x3Tex
     || opcode == DxsoOpcode::TexM3x3Spec
     || opcode == DxsoOpcode::TexM3x3VSpec
     || opcode == DxsoOpcode::TexReg2Rgb
     || opcode == DxsoOpcode::TexDp3Tex
     || opcode == DxsoOpcode::TexM3x2Depth
     || opcode == DxsoOpcode::TexDp3
     || opcode == DxsoOpcode::TexM3x3
     //  Explicit LOD.
     //|| opcode == DxsoOpcode::TexLdd
     //|| opcode == DxsoOpcode::TexLdl
     || opcode == DxsoOpcode::TexDepth)
      m_analysis->usesDerivatives = true;

    m_parentOpcode = ctx.instruction.opcode;

    for (uint32_t i = 0; i < ctx.src.size(); i++) {
      DxsoRegisterId regId = ctx.src[i].id;

      m_analysis->usesRelativeAddressing |= ctx.src[i].hasRelative;
      if (m_analysis->usesRelativeAddressing)
        break;

      switch (regId.type) {
        case DxsoRegisterType::Const:
        case DxsoRegisterType::Const2:
        case DxsoRegisterType::Const3:
        case DxsoRegisterType::Const4:
          usedFloatConstants.insert(regId.num);
          break;
        case DxsoRegisterType::ConstInt:
          usedIntConstants.insert(regId.num);
          break;
      }
    }


    if (!ctx.dst.hasRelative && !m_analysis->usesRelativeAddressing) {
      DxsoRegisterId regId = ctx.dst.id;
      switch (opcode) {
        case DxsoOpcode::Def:
          usedFloatConstants.insert(regId.num);
          break;

        case DxsoOpcode::DefI:
          usedIntConstants.insert(regId.num);
          break;
      }
    }

    for (uint32_t i = 0; i < ctx.src.size() && !m_analysis->usesRelativeAddressing; i++) {
      DxsoRegisterId regId = ctx.src[i].id;
      uint32_t num = regId.num;

      m_analysis->usesRelativeAddressing |= ctx.src[i].hasRelative;
      if (m_analysis->usesRelativeAddressing)
        break;

      uint32_t iterCount = 1;
      switch (ctx.instruction.opcode) {
      case DxsoOpcode::M3x2:
        iterCount = 2;
        break;
      case DxsoOpcode::M4x3:
      case DxsoOpcode::M3x3:
        iterCount = 3;
        break;
      case DxsoOpcode::M4x4:
      case DxsoOpcode::M3x4:
        iterCount = 4;
        break;
      }

      for (uint32_t j = 0; j < iterCount; j++) {
        switch (regId.type) {
          case DxsoRegisterType::Const:
          case DxsoRegisterType::Const2:
          case DxsoRegisterType::Const3:
          case DxsoRegisterType::Const4:
            usedFloatConstants.insert(num);
            break;
          case DxsoRegisterType::ConstInt:
            usedIntConstants.insert(num);
            break;
        }
        num++;
      }
    }
  }

  void DxsoAnalyzer::finalize(size_t tokenCount) {
    for (auto itr = usedFloatConstants.begin(); itr != usedFloatConstants.end(); itr++) {
      m_analysis->floatConstantMap[*itr] = m_analysis->floatConstantMap.size();
    }
    for (auto itr = usedIntConstants.begin(); itr != usedIntConstants.end(); itr++) {
      m_analysis->intConstantMap[*itr] = m_analysis->intConstantMap.size();
    }

    m_analysis->bytecodeByteLength = tokenCount * sizeof(uint32_t);
  }

}