; ModuleID = 'src/GlobalStorage.c'
source_filename = "src/GlobalStorage.c"

@intvar_1 = global i32 0
@chararray = global [2 x i8] zeroinitializer
@0 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@1 = private unnamed_addr constant [22 x i8] c"Dump Global Variable:\00", align 1
@2 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@3 = private unnamed_addr constant [11 x i8] c"Expect 1: \00", align 1
@4 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@5 = private unnamed_addr constant [19 x i8] c"Dump Global Array:\00", align 1
@6 = private unnamed_addr constant [6 x i8] c"%s%c\0A\00", align 1
@7 = private unnamed_addr constant [11 x i8] c"Expect a: \00", align 1
@8 = private unnamed_addr constant [6 x i8] c"%s%c\0A\00", align 1
@9 = private unnamed_addr constant [11 x i8] c"Expect b: \00", align 1

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define i32 @main() {
entry:
  store i32 1, i32* @intvar_1
  store i8 97, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @chararray, i32 0, i32 0)
  store i8 98, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @chararray, i32 0, i32 1)
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i8* getelementptr inbounds ([22 x i8], [22 x i8]* @1, i32 0, i32 0))
  %intvar_1 = load i32, i32* @intvar_1
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @2, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @3, i32 0, i32 0), i32 %intvar_1)
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @4, i32 0, i32 0), i8* getelementptr inbounds ([19 x i8], [19 x i8]* @5, i32 0, i32 0))
  %3 = load i8, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @chararray, i32 0, i32 0)
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @6, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @7, i32 0, i32 0), i8 %3)
  %5 = load i8, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @chararray, i32 0, i32 1)
  %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @8, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @9, i32 0, i32 0), i8 %5)
  ret i32 0
}
