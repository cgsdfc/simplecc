; ModuleID = 'src/EscapeSequence.c'
source_filename = "src/EscapeSequence.c"

@0 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@1 = private unnamed_addr constant [17 x i8] c"\5Cn\5Cr\5C\5C\5Ca\5Cb\5Cf\5Ct\5Cv\00", align 1

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define i32 @main() {
entry:
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i8* getelementptr inbounds ([17 x i8], [17 x i8]* @1, i32 0, i32 0))
  ret i32 0
}
