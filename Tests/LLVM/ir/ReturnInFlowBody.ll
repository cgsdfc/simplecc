; ModuleID = 'src/ReturnInFlowBody.c'
source_filename = "src/ReturnInFlowBody.c"

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define void @test() {
entry:
  br i1 true, label %then, label %else

then:                                             ; preds = %entry
  ret void

else:                                             ; preds = %entry
  br label %end

end:                                              ; preds = %else
  ret void
}

define void @test2() {
entry:
  br label %loop

loop:                                             ; preds = %entry
  br i1 true, label %body, label %end

body:                                             ; preds = %loop
  ret void

end:                                              ; preds = %loop
  ret void
}

define void @test3() {
entry:
  %i = alloca i32
  store i32 0, i32* %i
  br label %body

body:                                             ; preds = %loop, %entry
  ret void

loop:                                             ; No predecessors!
  %i1 = load i32, i32* %i
  %0 = add i32 %i1, 1
  store i32 %0, i32* %i
  %i2 = load i32, i32* %i
  %1 = icmp slt i32 %i2, 3
  br i1 %1, label %body, label %end

end:                                              ; preds = %loop
  ret void
}

define i32 @main() {
entry:
  ret i32 0
}
