; ModuleID = 'src/LocalArray.c'
source_filename = "src/LocalArray.c"

@0 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@1 = private unnamed_addr constant [13 x i8] c"Test_IArray:\00", align 1
@2 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@3 = private unnamed_addr constant [11 x i8] c"Expect 1: \00", align 1
@4 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@5 = private unnamed_addr constant [11 x i8] c"Expect 2: \00", align 1
@6 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@7 = private unnamed_addr constant [13 x i8] c"Test_CArray:\00", align 1
@8 = private unnamed_addr constant [6 x i8] c"%s%c\0A\00", align 1
@9 = private unnamed_addr constant [11 x i8] c"Expect a: \00", align 1
@10 = private unnamed_addr constant [6 x i8] c"%s%c\0A\00", align 1
@11 = private unnamed_addr constant [11 x i8] c"Expect b: \00", align 1

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define void @test_iarray() {
entry:
  %iarray = alloca [2 x i32]
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i8* getelementptr inbounds ([13 x i8], [13 x i8]* @1, i32 0, i32 0))
  %1 = getelementptr inbounds [2 x i32], [2 x i32]* %iarray, i32 0, i32 0
  store i32 1, i32* %1
  %2 = getelementptr inbounds [2 x i32], [2 x i32]* %iarray, i32 0, i32 0
  %3 = load i32, i32* %2
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @2, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @3, i32 0, i32 0), i32 %3)
  %5 = getelementptr inbounds [2 x i32], [2 x i32]* %iarray, i32 0, i32 1
  store i32 2, i32* %5
  %6 = getelementptr inbounds [2 x i32], [2 x i32]* %iarray, i32 0, i32 1
  %7 = load i32, i32* %6
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @4, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @5, i32 0, i32 0), i32 %7)
  ret void
}

define void @test_carray() {
entry:
  %carray = alloca [2 x i8]
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @6, i32 0, i32 0), i8* getelementptr inbounds ([13 x i8], [13 x i8]* @7, i32 0, i32 0))
  %1 = getelementptr inbounds [2 x i8], [2 x i8]* %carray, i32 0, i32 0
  store i8 97, i8* %1
  %2 = getelementptr inbounds [2 x i8], [2 x i8]* %carray, i32 0, i32 0
  %3 = load i8, i8* %2
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @8, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @9, i32 0, i32 0), i8 %3)
  %5 = getelementptr inbounds [2 x i8], [2 x i8]* %carray, i32 0, i32 1
  store i8 98, i8* %5
  %6 = getelementptr inbounds [2 x i8], [2 x i8]* %carray, i32 0, i32 1
  %7 = load i8, i8* %6
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @10, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @11, i32 0, i32 0), i8 %7)
  ret void
}

define i32 @main() {
entry:
  call void @test_iarray()
  call void @test_carray()
  ret i32 0
}
