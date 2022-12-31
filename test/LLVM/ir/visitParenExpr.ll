; ModuleID = 'src/visitParenExpr.c'
source_filename = "src/visitParenExpr.c"

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define i32 @main() {
entry:
  %ivar = alloca i32
  store i32 2, i32* %ivar
  store i32 97, i32* %ivar
  ret i32 0
}
