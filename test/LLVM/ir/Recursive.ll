; ModuleID = 'src/Recursive.c'
source_filename = "src/Recursive.c"

@0 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@1 = private unnamed_addr constant [16 x i8] c"Invalid Order: \00", align 1
@2 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@3 = private unnamed_addr constant [12 x i8] c"Expect -1: \00", align 1
@4 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@5 = private unnamed_addr constant [11 x i8] c"Expect 1: \00", align 1
@6 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@7 = private unnamed_addr constant [11 x i8] c"Expect 1: \00", align 1
@8 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@9 = private unnamed_addr constant [11 x i8] c"Expect 2: \00", align 1
@10 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@11 = private unnamed_addr constant [11 x i8] c"Expect 6: \00", align 1
@12 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@13 = private unnamed_addr constant [12 x i8] c"Expect 24: \00", align 1
@14 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@15 = private unnamed_addr constant [13 x i8] c"Expect 120: \00", align 1
@16 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@17 = private unnamed_addr constant [13 x i8] c"Expect 720: \00", align 1

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define i32 @recursivefactoral(i32 %order) {
entry:
  %order1 = alloca i32
  store i32 %order, i32* %order1
  %order2 = load i32, i32* %order1
  %0 = icmp slt i32 %order2, 0
  br i1 %0, label %then, label %else

then:                                             ; preds = %entry
  %order3 = load i32, i32* %order1
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @0, i32 0, i32 0), i8* getelementptr inbounds ([16 x i8], [16 x i8]* @1, i32 0, i32 0), i32 %order3)
  ret i32 -1

else:                                             ; preds = %entry
  br label %end

end:                                              ; preds = %else
  %order4 = load i32, i32* %order1
  %2 = icmp eq i32 %order4, 0
  br i1 %2, label %then5, label %else6

then5:                                            ; preds = %end
  ret i32 1

else6:                                            ; preds = %end
  br label %end7

end7:                                             ; preds = %else6
  %order8 = load i32, i32* %order1
  %order9 = load i32, i32* %order1
  %3 = sub i32 %order9, 1
  %4 = call i32 @recursivefactoral(i32 %3)
  %5 = mul i32 %order8, %4
  ret i32 %5
}

define i32 @main() {
entry:
  %0 = call i32 @recursivefactoral(i32 -1)
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @2, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @3, i32 0, i32 0), i32 %0)
  %2 = call i32 @recursivefactoral(i32 0)
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @4, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @5, i32 0, i32 0), i32 %2)
  %4 = call i32 @recursivefactoral(i32 1)
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @6, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @7, i32 0, i32 0), i32 %4)
  %6 = call i32 @recursivefactoral(i32 2)
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @8, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @9, i32 0, i32 0), i32 %6)
  %8 = call i32 @recursivefactoral(i32 3)
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @10, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @11, i32 0, i32 0), i32 %8)
  %10 = call i32 @recursivefactoral(i32 4)
  %11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @12, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @13, i32 0, i32 0), i32 %10)
  %12 = call i32 @recursivefactoral(i32 5)
  %13 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @14, i32 0, i32 0), i8* getelementptr inbounds ([13 x i8], [13 x i8]* @15, i32 0, i32 0), i32 %12)
  %14 = call i32 @recursivefactoral(i32 6)
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @16, i32 0, i32 0), i8* getelementptr inbounds ([13 x i8], [13 x i8]* @17, i32 0, i32 0), i32 %14)
  ret i32 0
}
