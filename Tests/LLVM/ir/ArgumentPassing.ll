; ModuleID = 'src/ArgumentPassing.c'
source_filename = "src/ArgumentPassing.c"

@0 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@1 = private unnamed_addr constant [16 x i8] c"Dump Arguments:\00", align 1
@2 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@3 = private unnamed_addr constant [11 x i8] c"Expect 1: \00", align 1
@4 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@5 = private unnamed_addr constant [11 x i8] c"Expect 2: \00", align 1
@6 = private unnamed_addr constant [6 x i8] c"%s%c\0A\00", align 1
@7 = private unnamed_addr constant [11 x i8] c"Expect a: \00", align 1
@8 = private unnamed_addr constant [6 x i8] c"%s%c\0A\00", align 1
@9 = private unnamed_addr constant [11 x i8] c"Expect b: \00", align 1

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define void @testargumnentpassing(i32 %i1, i32 %i2, i8 %c1, i8 %c2) {
entry:
  %i11 = alloca i32
  store i32 %i1, i32* %i11
  %i22 = alloca i32
  store i32 %i2, i32* %i22
  %c13 = alloca i8
  store i8 %c1, i8* %c13
  %c24 = alloca i8
  store i8 %c2, i8* %c24
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i8* getelementptr inbounds ([16 x i8], [16 x i8]* @1, i32 0, i32 0))
  %i15 = load i32, i32* %i11
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @2, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @3, i32 0, i32 0), i32 %i15)
  %i26 = load i32, i32* %i22
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @4, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @5, i32 0, i32 0), i32 %i26)
  %c17 = load i8, i8* %c13
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @6, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @7, i32 0, i32 0), i8 %c17)
  %c28 = load i8, i8* %c24
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @8, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @9, i32 0, i32 0), i8 %c28)
  ret void
}

define i32 @main() {
entry:
  call void @testargumnentpassing(i32 1, i32 2, i8 97, i8 98)
  ret i32 0
}
