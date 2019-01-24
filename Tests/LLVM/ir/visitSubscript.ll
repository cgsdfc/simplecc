; ModuleID = 'src/visitSubscript.c'
source_filename = "src/visitSubscript.c"

@0 = private unnamed_addr constant [4 x i8] c"%c\0A\00", align 1

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define i32 @main() {
entry:
  %iarr = alloca [2 x i32]
  %ivar = alloca i32
  %carr = alloca [2 x i8]
  %0 = getelementptr inbounds [2 x i32], [2 x i32]* %iarr, i32 0, i32 0
  store i32 1, i32* %0
  %1 = getelementptr inbounds [2 x i32], [2 x i32]* %iarr, i32 0, i32 0
  %2 = load i32, i32* %1
  store i32 %2, i32* %ivar
  %3 = getelementptr inbounds [2 x i32], [2 x i32]* %iarr, i32 0, i32 1
  %4 = load i32, i32* %3
  %5 = getelementptr inbounds [2 x i32], [2 x i32]* %iarr, i32 0, i32 0
  store i32 %4, i32* %5
  %6 = getelementptr inbounds [2 x i8], [2 x i8]* %carr, i32 0, i32 1
  %7 = load i8, i8* %6
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i8 %7)
  ret i32 0
}
