; ModuleID = 'src/visitBoolOp.c'
source_filename = "src/visitBoolOp.c"

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define void @testcharinvolved() {
entry:
  %cv = alloca i8
  store i8 97, i8* %cv
  %cv1 = load i8, i8* %cv
  %0 = zext i8 %cv1 to i32
  %1 = icmp ne i32 %0, 0
  br i1 %1, label %then, label %else

then:                                             ; preds = %entry
  br label %end

else:                                             ; preds = %entry
  br label %end

end:                                              ; preds = %else, %then
  %cv2 = load i8, i8* %cv
  %2 = zext i8 %cv2 to i32
  %3 = add i32 %2, 1
  %4 = icmp ne i32 %3, 0
  br i1 %4, label %then3, label %else4

then3:                                            ; preds = %end
  br label %end5

else4:                                            ; preds = %end
  br label %end5

end5:                                             ; preds = %else4, %then3
  %cv6 = load i8, i8* %cv
  %5 = zext i8 %cv6 to i32
  %6 = icmp eq i32 %5, 97
  br i1 %6, label %then7, label %else8

then7:                                            ; preds = %end5
  br label %end9

else8:                                            ; preds = %end5
  br label %end9

end9:                                             ; preds = %else8, %then7
  ret void
}

define void @testexprascond() {
entry:
  %iv = alloca i32
  store i32 1, i32* %iv
  %iv1 = load i32, i32* %iv
  %0 = add i32 %iv1, 1
  %1 = icmp ne i32 %0, 0
  br i1 %1, label %then, label %else

then:                                             ; preds = %entry
  br label %end

else:                                             ; preds = %entry
  br label %end

end:                                              ; preds = %else, %then
  %iv2 = load i32, i32* %iv
  %2 = mul i32 %iv2, 1
  %3 = icmp ne i32 %2, 0
  br i1 %3, label %then3, label %else4

then3:                                            ; preds = %end
  br label %end5

else4:                                            ; preds = %end
  br label %end5

end5:                                             ; preds = %else4, %then3
  br i1 true, label %then6, label %else7

then6:                                            ; preds = %end5
  br label %end8

else7:                                            ; preds = %end5
  br label %end8

end8:                                             ; preds = %else7, %then6
  %iv9 = load i32, i32* %iv
  %neg = sub i32 0, %iv9
  %4 = icmp ne i32 %neg, 0
  br i1 %4, label %then10, label %else11

then10:                                           ; preds = %end8
  br label %end12

else11:                                           ; preds = %end8
  br label %end12

end12:                                            ; preds = %else11, %then10
  ret void
}

define void @testrichcompareop() {
entry:
  %iv = alloca i32
  store i32 1, i32* %iv
  %iv1 = load i32, i32* %iv
  %0 = icmp sgt i32 %iv1, 0
  br i1 %0, label %then, label %else

then:                                             ; preds = %entry
  br label %end

else:                                             ; preds = %entry
  br label %end

end:                                              ; preds = %else, %then
  %iv2 = load i32, i32* %iv
  %1 = icmp slt i32 %iv2, 0
  br i1 %1, label %then3, label %else4

then3:                                            ; preds = %end
  br label %end5

else4:                                            ; preds = %end
  br label %end5

end5:                                             ; preds = %else4, %then3
  %iv6 = load i32, i32* %iv
  %2 = icmp eq i32 %iv6, 0
  br i1 %2, label %then7, label %else8

then7:                                            ; preds = %end5
  br label %end9

else8:                                            ; preds = %end5
  br label %end9

end9:                                             ; preds = %else8, %then7
  %iv10 = load i32, i32* %iv
  %3 = icmp sge i32 %iv10, 0
  br i1 %3, label %then11, label %else12

then11:                                           ; preds = %end9
  br label %end13

else12:                                           ; preds = %end9
  br label %end13

end13:                                            ; preds = %else12, %then11
  %iv14 = load i32, i32* %iv
  %4 = icmp ne i32 %iv14, 0
  br i1 %4, label %then15, label %else16

then15:                                           ; preds = %end13
  br label %end17

else16:                                           ; preds = %end13
  br label %end17

end17:                                            ; preds = %else16, %then15
  %iv18 = load i32, i32* %iv
  %5 = icmp sle i32 %iv18, 0
  br i1 %5, label %then19, label %else20

then19:                                           ; preds = %end17
  br label %end21

else20:                                           ; preds = %end17
  br label %end21

end21:                                            ; preds = %else20, %then19
  ret void
}

define i32 @main() {
entry:
  call void @testrichcompareop()
  call void @testexprascond()
  call void @testcharinvolved()
  ret i32 0
}
