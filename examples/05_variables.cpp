#include <iostream>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instructions.h"

using namespace llvm;

static AllocaInst *CreateBlockAlloca(Function *func, std::string name, Type* type) {
  IRBuilder<> tmpBuilder(&func->getEntryBlock(), func->getEntryBlock().begin());
  return tmpBuilder.CreateAlloca(type, nullptr, name);
}

int main () {
  // Define our context
  LLVMContext mContext;
  IRBuilder<> mBuilder(mContext);
  std::unique_ptr<Module> mModule = llvm::make_unique<Module>("foo", mContext);

  // Create and enter the main function
  Type *returnType = IntegerType::get(mContext, 32);
  std::vector<Type *> arguments;

	FunctionType *FT = FunctionType::get(returnType, arguments, false);
  Function *func = Function::Create(FT, Function::ExternalLinkage, "main", mModule.get());

	BasicBlock *bb = BasicBlock::Create(mContext, "entry", func);
	mBuilder.SetInsertPoint(bb);

  // Create a simpel value
  Value *one = ConstantInt::get(mContext, APInt(32, 1));

  // Create an alloca for that value
  AllocaInst *alloca = CreateBlockAlloca(func, "one", one->getType());
  std::cout << "alloca: " << std::endl;
  alloca->print(errs()); std::cout << std::endl;

  // Store the value
  Value *storeInst = mBuilder.CreateStore(one, alloca);
  std::cout << "store instruction: " << std::endl;
  storeInst->print(errs()); std::cout << std::endl;

  // Load the value
  Value *loadedValue = mBuilder.CreateLoad(alloca);
  std::cout << "load instruction: " << std::endl;
  loadedValue->print(errs()); std::cout << std::endl;

  // Create return for function
  mBuilder.CreateRet(loadedValue);

  // Print whole function
  std::cout << "entire function: " << std::endl;
  func->print(errs());

  return 0;
}
