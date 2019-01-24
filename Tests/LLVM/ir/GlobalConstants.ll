; ModuleID = 'src/GlobalConstants.c'
source_filename = "src/GlobalConstants.c"

@ci = constant i32 1
@cc = constant i8 97
@0 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@1 = private unnamed_addr constant [11 x i8] c"Expect 1: \00", align 1
@2 = private unnamed_addr constant [6 x i8] c"%s%c\0A\00", align 1
@3 = private unnamed_addr constant [11 x i8] c"Expect a: \00", align 1

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define i32 @main() {
entry:
  %ci = load i32, i32* @ci
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @0, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @1, i32 0, i32 0), i32 %ci)
  %cc = load i8, i8* @cc
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @2, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @3, i32 0, i32 0), i8 %cc)
  ret i32 0
}
