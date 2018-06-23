#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

int main () {
  // Define our context
  LLVMContext mContext;

  // Create a simpel value
  Value *one = ConstantInt::get(mContext, APInt(32, 1));
  one->print(errs());
  return 0;
}
