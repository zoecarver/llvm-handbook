#include <iostream>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"

using namespace llvm;

int main () {
  // Define our context
  LLVMContext mContext;
  IRBuilder<> mBuilder(mContext);

  // Create some values
  Value *L = ConstantInt::get(mContext, APInt(32, 2));
  Value *R = ConstantInt::get(mContext, APInt(32, 2));

  // compute them
  Value *added = mBuilder.CreateAdd(L, R, "addtmp");
  Value *subtracted = mBuilder.CreateSub(L, R, "subtmp");
  Value *multiplied = mBuilder.CreateMul(L, R, "multmp");
  Value *divided = mBuilder.CreateSDiv(L, R, "divtmp");

  // Print them out
  std::cout << "Individual Values: " << std::endl;
  added->print(errs());       std::cout << std::endl;
  subtracted->print(errs());  std::cout << std::endl;
  multiplied->print(errs());  std::cout << std::endl;
  divided->print(errs());     std::cout << std::endl;

  return 0;
}
