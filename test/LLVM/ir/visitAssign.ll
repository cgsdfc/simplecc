; ModuleID = 'src/visitAssign.c'
source_filename = "src/visitAssign.c"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"%c\0A\00", align 1

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define i32 @main() {
entry:
  %intvar = alloca i32
  %charvar = alloca i8
  store i32 1, i32* %intvar
  %intvar1 = load i32, i32* %intvar
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i32 %intvar1)
  store i8 97, i8* %charvar
  %charvar2 = load i8, i8* %charvar
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @1, i32 0, i32 0), i8 %charvar2)
  ret i32 0
}
