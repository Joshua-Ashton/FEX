#include "Interface/IR/PassManager.h"
#include "Interface/Core/OpcodeDispatcher.h"

namespace FEXCore::IR {

class DeadFlagCalculationEliminination final : public FEXCore::IR::Pass {
public:
  bool Run(IREmitter *IREmit) override;
};

/**
 * @brief (UNSAFE) This pass removes flag calculations that will otherwise be unused INSIDE of that block
 *
 * Compilers don't really do any form of cross-block flag allocation like they do RA with GPRs.
 * This ends up with them recalculating flags across blocks regardless of if it is actually possible to reuse the flags.
 * This is an additional burden in x86 that most instructions change flags when called, so it is easier to recalculate anyway.
 *
 * This is unsafe since handwritten code can easily break this assumption.
 * This may be more interesting with full function level recompilation since flags definitely won't be used across function boundaries.
 *
 */
bool DeadFlagCalculationEliminination::Run(IREmitter *IREmit) {
  std::array<OrderedNode*, 32> LastValidFlagStores{};

  bool Changed = false;
  auto CurrentIR = IREmit->ViewIR();
  uintptr_t ListBegin = CurrentIR.GetListData();
  uintptr_t DataBegin = CurrentIR.GetData();

  for (auto Block : CurrentIR.getBlocks()) {
    auto BlockIROp = Block.GetNode(ListBegin)->Op(DataBegin)->CW<FEXCore::IR::IROp_CodeBlock>();
    LogMan::Throw::A(BlockIROp->Header.Op == OP_CODEBLOCK, "IR type failed to be a code block");

    // We grab these nodes this way so we can iterate easily
    auto CodeBegin = CurrentIR.at(BlockIROp->Begin);
    auto CodeLast = CurrentIR.at(BlockIROp->Last);
    while (1) {
      auto CodeOp = CodeBegin();
      OrderedNode *CodeNode = CodeOp->GetNode(ListBegin);
      auto IROp = CodeNode->Op(DataBegin);

      if (IROp->Op == OP_STOREFLAG) {
        auto Op = IROp->CW<IR::IROp_StoreFlag>();
        // Set this node as the last one valid for this flag
        LastValidFlagStores[Op->Flag] = CodeNode;
      }
      else if (IROp->Op == OP_LOADFLAG) {
        auto Op = IROp->CW<IR::IROp_LoadFlag>();
        LastValidFlagStores[Op->Flag] = nullptr;
      }

      // CodeLast is inclusive. So we still need to dump the CodeLast op as well
      if (CodeBegin == CodeLast) {
        break;
      }
      ++CodeBegin;
    }

    // If any flags are stored but not loaded by the end of the block, then erase them
    for (auto &Flag : LastValidFlagStores) {
      if (Flag != nullptr) {
        IREmit->Remove(Flag);
        Changed = true;
      }
    }

    LastValidFlagStores.fill(nullptr);
  }

  return Changed;
}

FEXCore::IR::Pass* CreateDeadFlagCalculationEliminination() {
  return new DeadFlagCalculationEliminination{};
}

}
