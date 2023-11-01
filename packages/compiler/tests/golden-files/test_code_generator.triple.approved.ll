; ModuleID = 'main'
source_filename = "examples/triple.dao"

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

