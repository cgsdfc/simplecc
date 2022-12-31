; ModuleID = 'src/ReturnProperly.c'
source_filename = "src/ReturnProperly.c"

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define void @test1() {
entry:
  ret void
}

define i32 @test2() {
entry:
  br i1 true, label %then, label %else

then:                                             ; preds = %entry
  ret i32 1

else:                                             ; preds = %entry
  ret i32 2

end:                                              ; No predecessors!
  ret i32 0
}

define i32 @test3() {
entry:
  br label %loop

loop:                                             ; preds = %entry
  br i1 true, label %body, label %end

body:                                             ; preds = %loop
  ret i32 1

end:                                              ; preds = %loop
  ret i32 0
}

define i32 @main() {
entry:
  ret i32 0
}
