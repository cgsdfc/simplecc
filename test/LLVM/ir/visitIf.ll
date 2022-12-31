; ModuleID = 'src/visitIf.c'
source_filename = "src/visitIf.c"

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define void @test() {
entry:
  br i1 true, label %then, label %else

then:                                             ; preds = %entry
  br label %end

else:                                             ; preds = %entry
  br label %end

end:                                              ; preds = %else, %then
  ret void
}

define void @test2() {
entry:
  br i1 true, label %then, label %else

then:                                             ; preds = %entry
  br label %end

else:                                             ; preds = %entry
  br label %end

end:                                              ; preds = %else, %then
  ret void
}

define i32 @main() {
entry:
  ret i32 0
}
