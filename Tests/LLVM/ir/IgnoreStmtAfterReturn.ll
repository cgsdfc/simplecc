; ModuleID = 'src/IgnoreStmtAfterReturn.c'
source_filename = "src/IgnoreStmtAfterReturn.c"

@0 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@1 = private unnamed_addr constant [11 x i8] c"Expect 1: \00", align 1

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define i32 @test() {
entry:
  ret i32 1
}

define i32 @main() {
entry:
  %0 = call i32 @test()
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @0, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @1, i32 0, i32 0), i32 %0)
  ret i32 0
}
