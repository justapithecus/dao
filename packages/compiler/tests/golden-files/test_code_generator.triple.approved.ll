; ModuleID = 'main'
source_filename = "examples/triple.dao"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

define double @triple(double %x) {
entry:
  %multmp = fmul double %x, 3.000000e+00
  ret double %multmp
}

define i32 @main() {
entry:
  %calltmp = call double @triple(double 7.000000e+00)
  ret i32 0
}

