; ModuleID = 'Tests/IR/IRPrinter/Test.c'
source_filename = "Tests/IR/IRPrinter/Test.c"

@charvar = global i8 0
@0 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@1 = private unnamed_addr constant [11 x i8] c"IntVar > 0\00", align 1

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define i32 @main() {
entry:
  %intvar = alloca i32
  %intvar1 = load i32, i32* %intvar
  %gt = icmp sgt i32 %intvar1, 0
  br i1 %gt, label %then, label %else

then:                                             ; preds = %entry
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @1, i32 0, i32 0))
  br label %end

else:                                             ; preds = %entry
  br label %end

end:                                              ; preds = %else, %then
  ret i32 0
}
