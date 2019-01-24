; ModuleID = 'src/visitWhile.c'
source_filename = "src/visitWhile.c"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define void @test1() {
entry:
  %i = alloca i32
  store i32 0, i32* %i
  br label %loop

loop:                                             ; preds = %body, %entry
  %i1 = load i32, i32* %i
  %0 = icmp slt i32 %i1, 4
  br i1 %0, label %body, label %end

body:                                             ; preds = %loop
  %i2 = load i32, i32* %i
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i32 %i2)
  %i3 = load i32, i32* %i
  %2 = add i32 %i3, 1
  store i32 %2, i32* %i
  br label %loop

end:                                              ; preds = %loop
  ret void
}

define i32 @main() {
entry:
  call void @test1()
  ret i32 0
}
