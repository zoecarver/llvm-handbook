# An LLVM handbook
# Disclamer!
This ~~tutorial~~ handbook is under development and **Not done!**
## Intro
This is not meant to be a guid to building a toy language, or a comprehensive documentation of the llvm API. It is meant to be an explanation (with examples) of how to use some llvm api's that I find I use often. 

Also, I do not pretend to be an expert at compiler construction or llvm. I simply thought I would document some thing that would have helped me when I was building my toy language. 

All the code snippets below can be found in the examples folder. All you have to do is run cmake and make to make whatever examples you want to run. 

## Setup
There are a couple of global variables that will be used here:
### LLVMContext
```c++
LLVMContext mContext;
```
This is the context for our compiler. It holds and manages important global data for the compiler such as types and constant uniquing tables.

### IRBuilder
```c++
IRBuilder<> mBuilder(mContext);
```
The IRBuilder handles creating instructions and inserting them in the right place.

### Module
```c++
std::unique_ptr<Module> mModule = llvm::make_unique<Module>("foo", mContext);
```
The Module can be used to print all of the generated code so we don't have to print each value individually.

## Values and Printing
A common type that you will see in LLVM is the `Value *` type. It is a base type that a lot of other types inherit from. Basically it just holds a value. It has lots of properties but, the first one I will bring up is `print`. `print` accepts a `raw_ostream` - we will use `errs()`. 

Here is an example:
```c++
// Create a simpel value
Value *one = ConstantInt::get(mContext, APInt(32, 1));
one->print(errs());
```
Output:
```assembly
i32 1
```

## Numbers
Here are some common numbers that you might use. 
```c++
// Create int (size: 32)
Value *one = ConstantInt::get(mContext, APInt(32, 1));

// Create int (size: 128)
Value *two = ConstantInt::get(mContext, APInt(128, 2));

// Create double 
Value *oneD = ConstantFP::get(mContext, APFloat(1.0));
```

Another property of the `Value` class is `getType()` this returns a `Type *`. Types are, well, types. Here are some examples:

```c++
Type *dType = Type::getDoubleTy(mContext);
Type *i32Type = IntegerType::get(mContext, 32);

Type *i32PointerType = PointerType::getUnqual(i32Type); // pointer to i32
```

As you can see above,  this creates a double type, an int type and a pointer type to the int type. 

Here is what the output of both look like:
```assembly
Values:
  i32 1
  i128 2
  double 1.000000e+00
Types:
  double
  i32
  i32*
```
 
## Basic Computation
Now that we have some numbers, lets say we want to do some computation. 

The basic setup will be a `Value` an operator and another `Value` which will look something like this:

```
Left + Right
```

For this we will use some instructions from the IRBuilder. In the following examples, assume that both `L` and `R` are `Value`s of type `i32`. 
### Adding
```c++
mBuilder.CreateAdd(L, R, "addtmp");
```

### Subtracting
```c++
mBuilder.CreateSub(L, R, "subtmp");
```

### Multiplying
```c++
mBuilder.CreateMul(L, R, "multmp");
```

### Dividing
```c++
mBuilder.CreateSDiv(L, R, "divtmp");
```

### Output
The above examples will output the following:
```assembly
i32 4
i32 0
i32 4
i32 1
```

### Some notes:
* All of the above examples will return a `Value` containing the result.
* The last argument in the above examples can be whatever you want, it is the name that the return value will be assigned to.
* For doubles, simply add an `F` after `Create` for example `CreateAdd` -> `CreateFAdd`

## Functions
Here is how to make a function:
```c++
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
```

There are a few different things going on here:
1. A `FunctionType` type is created with a return type and arguments
2. The `FunctionType` is used to make the function. If the function was printed at this point it would just be a prototype. **This is how external function should be declared** (but more on that later).
3. The first argument's name is set.
4. A basic block entry is created. Basic blocks are a way of keeping track of where the builder should be inserting code, this will be important for allocas as well.
5. The body of the function is created. Any values you create here will be part of the function body. 
6. A return value is created.

When compiled, here is what the function looks like:
```assembly
define i32 @inc(i32 %foo) {
entry:
  %addtmp = add i32 %foo, 1
  ret i32 %addtmp
}
```

## Variables and alloca
Allocas can be used to store values. An alloca is basically just a pointer to a type that can be set. Here are some examples:

### Create Alloca Function
For this we will actually use a function to keep the code a bit cleaner:
```c++
static AllocaInst *CreateBlockAlloca(Function *func, std::string name, Type* type) {
  IRBuilder<> tmpBuilder(&func->getEntryBlock(), func->getEntryBlock().begin());
  return tmpBuilder.CreateAlloca(type, nullptr, name);
}
```
There are a few things about this code:
1. An `AllocaInst` is returned from this function. That is an instruction to create an alloca. 
2. A temporary IR builder is used instead of our typical builder.
3. To actually create the alloca the function `CreateAlloca` on the temporary builder is used.

The temporary builder is given a basic block entry point so that it knows where to create the alloca, this is because _allocas can only exist in exactly one function_. 

### Create/Load/Store
Here is an example of how to create load and store from allocas. You can create an alloca in the `main` function created using code from the functions section.

#### Create

```c++
  AllocaInst *alloca = CreateBlockAlloca(func, "one", one->getType());
```

Arguments:
1. Insert point
2. Name of alloca
3. Type

#### Store
```c++
  Value *storeInst = mBuilder.CreateStore(one, alloca);
```
This will store the value (one) in the alloca created above.

#### Load
```c++
  Value *loadedValue = mBuilder.CreateLoad(alloca);
```
This will simply load the value in the alloca. There is also an optional second parameter that will load it into a named instruction (eg. `mBuilder.CreateLoad(alloca, "foo")`).

- - - -
Putting all the above code together (including the main function) gets us the following:

```c++
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
```

Which will output:
```assembly
alloca:
  %one = alloca i32
store instruction:
  store i32 1, i32* %one
load instruction:
  %0 = load i32, i32* %one
entire function:

define i32 @main() {
entry:
  %one = alloca i32
  store i32 1, i32* %one
  %0 = load i32, i32* %one
  ret i32 %0
}
```
