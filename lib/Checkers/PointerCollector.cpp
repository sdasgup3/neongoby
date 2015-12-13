#include "PointerCollector.h"

using namespace llvm;

namespace neongoby {

PointerCollector::PointerCollector() : llvmModule(0) { }

void PointerCollector::visitModule(Module &M) {
  Pointers.clear();
  llvmModule = &M;

  Module::global_iterator it = M.global_begin(), itEnd = M.global_end();
  for (; it != itEnd; ++it) {
    GlobalVariable *gv = &(*it);
    assert(gv->getType()->isPointerTy());
    GlobalPointers.insert(gv);
  }
}

void PointerCollector::visitFunction(Function &F) {
  Function::arg_iterator it = F.arg_begin(), itEnd = F.arg_end();
  for (; it != itEnd; ++it) {
    Argument *arg = &(*it);
    if (arg->getType()->isPointerTy())
      Pointers[&F].insert(arg);
  }
}

void PointerCollector::visitInstruction(Instruction &I) {
  Function *f = I.getParent()->getParent();

  if (I.getType()->isPointerTy())
    Pointers[f].insert(&I);

}

unsigned long long PointerCollector::getTotalNumberOfPointers() {
  unsigned long long total = GlobalPointers.size();
  PointerMap::iterator it = Pointers.begin(), itEnd = Pointers.end();
  for (; it != itEnd; ++it) {
    total += it->second.size();
  }

  return total;
}

void PointerCollector::printPointers(raw_ostream &O) {
  O << "\n======================================================\n";
  O << "                   Global Pointers\n";
  O << "------------------------------------------------------\n\n";

  PointerSet::iterator itG = GlobalPointers.begin(),
                       itGEnd = GlobalPointers.end();
  for (; itG != itGEnd; ++itG) {
    const Value *ptr = *itG;
    O << "\t";
    ptr->print(O);
    O << "\n";
  }

  O << "======================================================\n\n";

  O << "\n======================================================\n";
  O << "                   Local Pointers\n";
  O << "------------------------------------------------------\n\n";

  PointerMap::iterator it = Pointers.begin(), itEnd = Pointers.end();
  for (; it != itEnd; ++it) {
    const Function *f = it->first;
    PointerSet &LocalPtrs = it->second;
    O << "Function: " << f->getName() << "\n";
    PointerSet::iterator it2 = LocalPtrs.begin(), it2End = LocalPtrs.end();
    for (; it2 != it2End; ++it2) {
      const Value *ptr = *it2;
      O << "\t";
      ptr->print(O);
      O << "\n";
    }
    O << "\n";
  }

  O << "======================================================\n\n";
}

}
