; ModuleID = 'src/visitBinOp.c'
source_filename = "src/visitBinOp.c"

@0 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@1 = private unnamed_addr constant [12 x i8] c"Expect 97: \00", align 1
@2 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@3 = private unnamed_addr constant [13 x i8] c"Expect 194: \00", align 1
@4 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@5 = private unnamed_addr constant [11 x i8] c"Expect 2: \00", align 1
@6 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@7 = private unnamed_addr constant [11 x i8] c"Expect 2: \00", align 1

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define void @testcharinvolved() {
entry:
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @0, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @1, i32 0, i32 0), i32 97)
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @2, i32 0, i32 0), i8* getelementptr inbounds ([13 x i8], [13 x i8]* @3, i32 0, i32 0), i32 194)
  ret void
}

define void @testbinop() {
entry:
  %i = alloca i32
  store i32 1, i32* %i
  %i1 = load i32, i32* %i
  %0 = mul i32 %i1, 2
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @4, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @5, i32 0, i32 0), i32 %0)
  %i2 = load i32, i32* %i
  %i3 = load i32, i32* %i
  %2 = add i32 %i2, %i3
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @6, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @7, i32 0, i32 0), i32 %2)
  ret void
}

define i32 @main() {
entry:
  call void @testbinop()
  call void @testcharinvolved()
  ret i32 0
}
