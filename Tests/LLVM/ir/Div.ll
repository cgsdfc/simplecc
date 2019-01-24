; ModuleID = 'src/Div.c'
source_filename = "src/Div.c"

@0 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@1 = private unnamed_addr constant [13 x i8] c"Expected 1: \00", align 1
@2 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@3 = private unnamed_addr constant [14 x i8] c"Expected -1: \00", align 1

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define i32 @main() {
entry:
  %dividend = alloca i32
  store i32 2, i32* %dividend
  %dividend1 = load i32, i32* %dividend
  %0 = sdiv i32 %dividend1, 2
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @0, i32 0, i32 0), i8* getelementptr inbounds ([13 x i8], [13 x i8]* @1, i32 0, i32 0), i32 %0)
  store i32 -2, i32* %dividend
  %dividend2 = load i32, i32* %dividend
  %2 = sdiv i32 %dividend2, 2
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @2, i32 0, i32 0), i8* getelementptr inbounds ([14 x i8], [14 x i8]* @3, i32 0, i32 0), i32 %2)
  ret i32 0
}
