; ModuleID = 'main'
source_filename = "examples/hello_world.dao"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@0 = private unnamed_addr constant [22 x i8] c"Hello world from dao!\00", align 1

declare i32 @puts(i8*)

define double @triple(double %x) {
entry:
  %multmp = fmul double %x, 3.000000e+00
  ret double %multmp
}

define i32 @main() {
entry:
  %calltmp = call i32 @puts([22 x i8]* @0)
  %calltmp1 = call double @triple(double 7.000000e+00)
  %calltmp2 = call i32 @puts(double %calltmp1)
  ret i32 0
}

