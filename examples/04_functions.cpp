#include <iostream>
#include <vector>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instructions.h"

using namespace llvm;

int main () {
  // Define our context
  LLVMContext mContext;
  IRBuilder<> mBuilder(mContext);
  std::unique_ptr<Module> mModule = llvm::make_unique<Module>("foo", mContext);

  // Create function prototype
  Type *returnType = IntegerType::get(mContext, 32);
  std::vector<Type *> arguments = { IntegerType::get(mContext, 32) };

  FunctionType *FT = FunctionType::get(returnType, arguments, false);
  Function *func = Function::Create(FT, Function::ExternalLinkage, "inc", mModule.get());

  Argument *argument = &(*func->arg_begin());
  argument->setName("foo");

  // Create the function
  BasicBlock *bb = BasicBlock::Create(mContext, "entry", func);
  mBuilder.SetInsertPoint(bb);

  // This is the body of the function
  Value *L = dyn_cast<Value>(argument);
  Value *R = ConstantInt::get(mContext, APInt(32, 1));

  Value *returnValue = mBuilder.CreateAdd(L, R, "addtmp");
  mBuilder.CreateRet(returnValue);

  //Print the function
  func->print(errs());

  return 0;
}
