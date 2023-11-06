; ModuleID = 'main'
source_filename = "fibonacci.dao"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

define internal double @fibonacci(double %x) {
entry:
  %lttmp = fcmp ult double %x, 3.000000e+00
  br i1 %lttmp, label %then, label %else

then:                                             ; preds = %entry
  br label %ifcont

else:                                             ; preds = %entry
  %subtmp = fsub double %x, 1.000000e+00
  %calltmp = call double @fibonacci(double %subtmp)
  %subtmp1 = fsub double %x, 2.000000e+00
  %calltmp2 = call double @fibonacci(double %subtmp1)
  %addtmp = fadd double %calltmp, %calltmp2
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %iftmp = phi double [ 1.000000e+00, %then ], [ %addtmp, %else ]
  ret double %iftmp
}

define i32 @main() {
entry:
  %calltmp = call double @fibonacci(double 4.000000e+01)
  ret i32 0
}

