; ModuleID = 'src/GlobalArray.c'
source_filename = "src/GlobalArray.c"

@iarray = global [2 x i32] zeroinitializer
@carray = global [2 x i8] zeroinitializer
@0 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@1 = private unnamed_addr constant [13 x i8] c"Test_IArray:\00", align 1
@2 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@3 = private unnamed_addr constant [11 x i8] c"Expect 0: \00", align 1
@4 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@5 = private unnamed_addr constant [11 x i8] c"Expect 0: \00", align 1
@6 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@7 = private unnamed_addr constant [11 x i8] c"Expect 1: \00", align 1
@8 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@9 = private unnamed_addr constant [11 x i8] c"Expect 2: \00", align 1
@10 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@11 = private unnamed_addr constant [13 x i8] c"Test_CArray:\00", align 1
@12 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@13 = private unnamed_addr constant [11 x i8] c"Expect 0: \00", align 1
@14 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@15 = private unnamed_addr constant [11 x i8] c"Expect 0: \00", align 1
@16 = private unnamed_addr constant [6 x i8] c"%s%c\0A\00", align 1
@17 = private unnamed_addr constant [11 x i8] c"Expect a: \00", align 1
@18 = private unnamed_addr constant [6 x i8] c"%s%c\0A\00", align 1
@19 = private unnamed_addr constant [11 x i8] c"Expect b: \00", align 1

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define void @test_iarray() {
entry:
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i8* getelementptr inbounds ([13 x i8], [13 x i8]* @1, i32 0, i32 0))
  %1 = load i32, i32* getelementptr inbounds ([2 x i32], [2 x i32]* @iarray, i32 0, i32 0)
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @2, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @3, i32 0, i32 0), i32 %1)
  %3 = load i32, i32* getelementptr inbounds ([2 x i32], [2 x i32]* @iarray, i32 0, i32 1)
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @4, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @5, i32 0, i32 0), i32 %3)
  store i32 1, i32* getelementptr inbounds ([2 x i32], [2 x i32]* @iarray, i32 0, i32 0)
  %5 = load i32, i32* getelementptr inbounds ([2 x i32], [2 x i32]* @iarray, i32 0, i32 0)
  %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @6, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @7, i32 0, i32 0), i32 %5)
  store i32 2, i32* getelementptr inbounds ([2 x i32], [2 x i32]* @iarray, i32 0, i32 1)
  %7 = load i32, i32* getelementptr inbounds ([2 x i32], [2 x i32]* @iarray, i32 0, i32 1)
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @8, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @9, i32 0, i32 0), i32 %7)
  ret void
}

define void @test_carray() {
entry:
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @10, i32 0, i32 0), i8* getelementptr inbounds ([13 x i8], [13 x i8]* @11, i32 0, i32 0))
  %1 = load i8, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @carray, i32 0, i32 0)
  %2 = zext i8 %1 to i32
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @12, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @13, i32 0, i32 0), i32 %2)
  %4 = load i8, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @carray, i32 0, i32 1)
  %5 = zext i8 %4 to i32
  %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @14, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @15, i32 0, i32 0), i32 %5)
  store i8 97, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @carray, i32 0, i32 0)
  %7 = load i8, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @carray, i32 0, i32 0)
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @16, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @17, i32 0, i32 0), i8 %7)
  store i8 98, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @carray, i32 0, i32 1)
  %9 = load i8, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @carray, i32 0, i32 1)
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @18, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @19, i32 0, i32 0), i8 %9)
  ret void
}

define i32 @main() {
entry:
  call void @test_iarray()
  call void @test_carray()
  ret i32 0
}
