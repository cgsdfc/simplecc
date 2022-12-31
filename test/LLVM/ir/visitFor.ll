; ModuleID = 'src/visitFor.c'
source_filename = "src/visitFor.c"

@0 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"i: \00", align 1

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define i32 @main() {
entry:
  %i = alloca i32
  store i32 0, i32* %i
  br label %body

body:                                             ; preds = %loop, %entry
  %i3 = load i32, i32* %i
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @0, i32 0, i32 0), i8* getelementptr inbounds ([4 x i8], [4 x i8]* @1, i32 0, i32 0), i32 %i3)
  br label %loop

loop:                                             ; preds = %body
  %i1 = load i32, i32* %i
  %1 = add i32 %i1, 1
  store i32 %1, i32* %i
  %i2 = load i32, i32* %i
  %2 = icmp slt i32 %i2, 3
  br i1 %2, label %body, label %end

end:                                              ; preds = %loop
  ret i32 0
}
