; ModuleID = 'main'
source_filename = "if_else.dao"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@0 = private unnamed_addr constant [21 x i8] c"10 is greater than 9\00", align 1
@1 = private unnamed_addr constant [12 x i8] c"Unreachable\00", align 1

declare i32 @puts(ptr)

define internal double @double(double %x) {
entry:
  %multmp = fmul double %x, 2.000000e+00
  ret double %multmp
}

define i32 @main() {
entry:
  %calltmp = call double @double(double 5.000000e+00)
  %gttmp = fcmp ugt double %calltmp, 9.000000e+00
  br i1 %gttmp, label %then, label %else

then:                                             ; preds = %entry
  %calltmp1 = call i32 @puts(ptr @0)
  br label %ifcont

else:                                             ; preds = %entry
  %calltmp2 = call i32 @puts(ptr @1)
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %iftmp = phi i32 [ %calltmp1, %then ], [ %calltmp2, %else ]
  ret i32 0
}

