; ModuleID = 'src/visitChar.c'
source_filename = "src/visitChar.c"

@0 = private unnamed_addr constant [4 x i8] c"%c\0A\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"%c\0A\00", align 1
@2 = private unnamed_addr constant [4 x i8] c"%c\0A\00", align 1

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define i32 @main() {
entry:
  %cvar = alloca i8
  store i8 97, i8* %cvar
  store i8 98, i8* %cvar
  store i8 42, i8* %cvar
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i8 97)
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @1, i32 0, i32 0), i8 98)
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @2, i32 0, i32 0), i8 42)
  ret i32 0
}
