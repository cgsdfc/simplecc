; ModuleID = 'src/GlobalVariable.c'
source_filename = "src/GlobalVariable.c"

@iv = global i32 0
@cv = global i8 0
@0 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@1 = private unnamed_addr constant [8 x i8] c"IV == 0\00", align 1
@2 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@3 = private unnamed_addr constant [8 x i8] c"CV == 0\00", align 1
@4 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@5 = private unnamed_addr constant [11 x i8] c"Expect 1: \00", align 1
@6 = private unnamed_addr constant [6 x i8] c"%s%c\0A\00", align 1
@7 = private unnamed_addr constant [11 x i8] c"Expect a: \00", align 1

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define i32 @main() {
entry:
  %iv = load i32, i32* @iv
  %0 = icmp eq i32 %iv, 0
  br i1 %0, label %then, label %else

then:                                             ; preds = %entry
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i8* getelementptr inbounds ([8 x i8], [8 x i8]* @1, i32 0, i32 0))
  br label %end

else:                                             ; preds = %entry
  br label %end

end:                                              ; preds = %else, %then
  %cv = load i8, i8* @cv
  %2 = zext i8 %cv to i32
  %3 = icmp eq i32 %2, 0
  br i1 %3, label %then1, label %else2

then1:                                            ; preds = %end
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @2, i32 0, i32 0), i8* getelementptr inbounds ([8 x i8], [8 x i8]* @3, i32 0, i32 0))
  br label %end3

else2:                                            ; preds = %end
  br label %end3

end3:                                             ; preds = %else2, %then1
  store i32 1, i32* @iv
  %iv4 = load i32, i32* @iv
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @4, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @5, i32 0, i32 0), i32 %iv4)
  store i8 97, i8* @cv
  %cv5 = load i8, i8* @cv
  %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @6, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @7, i32 0, i32 0), i8 %cv5)
  ret i32 0
}
