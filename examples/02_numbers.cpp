#include <iostream>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

int main () {
  // Define our context
  LLVMContext mContext;

  // Create int (size: 32)
  Value *one = ConstantInt::get(mContext, APInt(32, 1));

  // Create int (size: 128)
  Value *two = ConstantInt::get(mContext, APInt(128, 2));

  // Create double 
  Value *oneD = ConstantFP::get(mContext, APFloat(1.0));

  // Types
  Type *dType = Type::getDoubleTy(mContext);
  Type *i32Type = IntegerType::get(mContext, 32);

  Type *i32PointerType = PointerType::getUnqual(i32Type); // pointer to i32

  // Print them all out
  std::cout << "Values: \n";
  one->print(errs());   std::cout << std::endl;
  two->print(errs());   std::cout << std::endl; 
  oneD->print(errs());  std::cout << std::endl; 
  std::cout << "Types: \n";
  dType->print(errs()); std::cout << std::endl; 
  i32Type->print(errs()); std::cout << std::endl; 
  i32PointerType->print(errs()); std::cout << std::endl; 

  return 0;
}
