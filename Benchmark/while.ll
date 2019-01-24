; ModuleID = 'while.lc'
source_filename = "../../Benchmark/Sample4Opt-1-while.c"

@max_num = constant i32 1024
@0 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@1 = private unnamed_addr constant [6 x i8] c" x = \00", align 1
@2 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@3 = private unnamed_addr constant [6 x i8] c" y = \00", align 1
@4 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@5 = private unnamed_addr constant [11 x i8] c" SWAP x = \00", align 1
@6 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@7 = private unnamed_addr constant [11 x i8] c" SWAP y = \00", align 1
@8 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@9 = private unnamed_addr constant [21 x i8] c" OVERFLOW!          \00", align 1
@10 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@11 = private unnamed_addr constant [19 x i8] c" complete number: \00", align 1
@12 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@13 = private unnamed_addr constant [3 x i8] c"  \00", align 1
@14 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@15 = private unnamed_addr constant [11 x i8] c"          \00", align 1
@16 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@17 = private unnamed_addr constant [75 x i8] c" ---------------------------------------------------------------          \00", align 1
@18 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@19 = private unnamed_addr constant [6 x i8] c" b = \00", align 1
@20 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@21 = private unnamed_addr constant [75 x i8] c" ---------------------------------------------------------------          \00", align 1
@22 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@23 = private unnamed_addr constant [2 x i8] c" \00", align 1
@24 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@25 = private unnamed_addr constant [11 x i8] c"          \00", align 1
@26 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@27 = private unnamed_addr constant [24 x i8] c"          The total is \00", align 1
@28 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@29 = private unnamed_addr constant [8 x i8] c" 10! = \00", align 1

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define i32 @factorial(i32 %n) {
entry:
  %n1 = alloca i32
  store i32 %n, i32* %n1
  %n2 = load i32, i32* %n1
  %0 = icmp sle i32 %n2, 1
  br i1 %0, label %then, label %else

then:                                             ; preds = %entry
  ret i32 1

else:                                             ; preds = %entry
  br label %end

end:                                              ; preds = %else
  %n3 = load i32, i32* %n1
  %n4 = load i32, i32* %n1
  %1 = sub i32 %n4, 1
  %2 = call i32 @factorial(i32 %1)
  %3 = mul i32 %n3, %2
  ret i32 %3
}

define i32 @mod(i32 %x, i32 %y) {
entry:
  %x1 = alloca i32
  store i32 %x, i32* %x1
  %y2 = alloca i32
  store i32 %y, i32* %y2
  %x3 = load i32, i32* %x1
  %x4 = load i32, i32* %x1
  %y5 = load i32, i32* %y2
  %0 = sdiv i32 %x4, %y5
  %y6 = load i32, i32* %y2
  %1 = mul i32 %0, %y6
  %2 = sub i32 %x3, %1
  store i32 %2, i32* %x1
  %x7 = load i32, i32* %x1
  ret i32 %x7
}

define void @swap(i32 %x, i32 %y) {
entry:
  %x1 = alloca i32
  store i32 %x, i32* %x1
  %y2 = alloca i32
  store i32 %y, i32* %y2
  %temp = alloca i32
  %x3 = load i32, i32* %x1
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @0, i32 0, i32 0), i8* getelementptr inbounds ([6 x i8], [6 x i8]* @1, i32 0, i32 0), i32 %x3)
  %y4 = load i32, i32* %y2
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @2, i32 0, i32 0), i8* getelementptr inbounds ([6 x i8], [6 x i8]* @3, i32 0, i32 0), i32 %y4)
  %x5 = load i32, i32* %x1
  store i32 %x5, i32* %temp
  %y6 = load i32, i32* %y2
  store i32 %y6, i32* %x1
  %temp7 = load i32, i32* %temp
  store i32 %temp7, i32* %y2
  %x8 = load i32, i32* %x1
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @4, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @5, i32 0, i32 0), i32 %x8)
  %y9 = load i32, i32* %y2
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @6, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @7, i32 0, i32 0), i32 %y9)
  ret void
}

define void @complete_num(i32 %val) {
entry:
  %val1 = alloca i32
  store i32 %val, i32* %val1
  %k = alloca [128 x i32]
  %i = alloca i32
  %j = alloca i32
  %n = alloca i32
  %s = alloca i32
  %x1 = alloca i32
  %m = alloca i32
  %k2 = alloca i32
  %h = alloca i32
  %leap = alloca i32
  %x2 = alloca i32
  %a = alloca i32
  %b = alloca i32
  %c = alloca i32
  %val2 = load i32, i32* %val1
  store i32 %val2, i32* %j
  br label %loop

loop:                                             ; preds = %end33, %entry
  %j3 = load i32, i32* %j
  %max_num = load i32, i32* @max_num
  %0 = icmp slt i32 %j3, %max_num
  br i1 %0, label %body, label %end

body:                                             ; preds = %loop
  store i32 -1, i32* %n
  %j4 = load i32, i32* %j
  store i32 %j4, i32* %s
  store i32 1, i32* %i
  br label %loop5

end:                                              ; preds = %loop
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @16, i32 0, i32 0), i8* getelementptr inbounds ([75 x i8], [75 x i8]* @17, i32 0, i32 0))
  store i32 0, i32* %a
  store i32 0, i32* %i
  br label %loop43

loop5:                                            ; preds = %end15, %body
  %i6 = load i32, i32* %i
  %j7 = load i32, i32* %j
  %2 = icmp slt i32 %i6, %j7
  br i1 %2, label %body8, label %end9

body8:                                            ; preds = %loop5
  %j10 = load i32, i32* %j
  %i11 = load i32, i32* %i
  %3 = sdiv i32 %j10, %i11
  %i12 = load i32, i32* %i
  %4 = mul i32 %3, %i12
  store i32 %4, i32* %x1
  %j13 = load i32, i32* %j
  %i14 = load i32, i32* %i
  %5 = call i32 @mod(i32 %j13, i32 %i14)
  %6 = icmp eq i32 %5, 0
  br i1 %6, label %then, label %else

end9:                                             ; preds = %loop5
  %s30 = load i32, i32* %s
  %7 = icmp eq i32 %s30, 0
  br i1 %7, label %then31, label %else32

then:                                             ; preds = %body8
  %n16 = load i32, i32* %n
  %8 = add i32 %n16, 1
  store i32 %8, i32* %n
  %s17 = load i32, i32* %s
  %i18 = load i32, i32* %i
  %9 = sub i32 %s17, %i18
  store i32 %9, i32* %s
  %n19 = load i32, i32* %n
  %10 = icmp sge i32 %n19, 128
  br i1 %10, label %then20, label %else21

else:                                             ; preds = %body8
  br label %end15

end15:                                            ; preds = %end26, %else
  %i29 = load i32, i32* %i
  %11 = add i32 %i29, 1
  store i32 %11, i32* %i
  br label %loop5

then20:                                           ; preds = %then
  %12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @8, i32 0, i32 0), i8* getelementptr inbounds ([21 x i8], [21 x i8]* @9, i32 0, i32 0))
  br label %end22

else21:                                           ; preds = %then
  br label %end22

end22:                                            ; preds = %else21, %then20
  %n23 = load i32, i32* %n
  %13 = icmp slt i32 %n23, 128
  br i1 %13, label %then24, label %else25

then24:                                           ; preds = %end22
  %i27 = load i32, i32* %i
  %n28 = load i32, i32* %n
  %14 = getelementptr inbounds [128 x i32], [128 x i32]* %k, i32 0, i32 %n28
  store i32 %i27, i32* %14
  br label %end26

else25:                                           ; preds = %end22
  br label %end26

end26:                                            ; preds = %else25, %then24
  br label %end15

then31:                                           ; preds = %end9
  %j34 = load i32, i32* %j
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @10, i32 0, i32 0), i8* getelementptr inbounds ([19 x i8], [19 x i8]* @11, i32 0, i32 0), i32 %j34)
  store i32 0, i32* %i
  br label %loop35

else32:                                           ; preds = %end9
  br label %end33

end33:                                            ; preds = %end39, %else32
  %j42 = load i32, i32* %j
  %16 = add i32 %j42, 1
  store i32 %16, i32* %j
  br label %loop

loop35:                                           ; preds = %body38, %then31
  %i36 = load i32, i32* %i
  %n37 = load i32, i32* %n
  %17 = icmp sle i32 %i36, %n37
  br i1 %17, label %body38, label %end39

body38:                                           ; preds = %loop35
  %i40 = load i32, i32* %i
  %18 = getelementptr inbounds [128 x i32], [128 x i32]* %k, i32 0, i32 %i40
  %19 = load i32, i32* %18
  %20 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @12, i32 0, i32 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @13, i32 0, i32 0), i32 %19)
  %i41 = load i32, i32* %i
  %21 = add i32 %i41, 1
  store i32 %21, i32* %i
  br label %loop35

end39:                                            ; preds = %loop35
  %22 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @14, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @15, i32 0, i32 0))
  br label %end33

loop43:                                           ; preds = %body46, %end
  %i44 = load i32, i32* %i
  %max_num45 = load i32, i32* @max_num
  %23 = icmp slt i32 %i44, %max_num45
  br i1 %23, label %body46, label %end47

body46:                                           ; preds = %loop43
  %a48 = load i32, i32* %a
  %i49 = load i32, i32* %i
  %24 = mul i32 %a48, %i49
  store i32 %24, i32* %b
  %j50 = load i32, i32* %j
  %n51 = load i32, i32* %n
  %25 = mul i32 %j50, %n51
  store i32 %25, i32* %c
  %a52 = load i32, i32* %a
  %i53 = load i32, i32* %i
  %26 = mul i32 %a52, %i53
  store i32 %26, i32* %a
  %a54 = load i32, i32* %a
  %b55 = load i32, i32* %b
  %27 = add i32 %a54, %b55
  %c56 = load i32, i32* %c
  %28 = add i32 %27, %c56
  store i32 %28, i32* %a
  %i57 = load i32, i32* %i
  %29 = add i32 %i57, 1
  store i32 %29, i32* %i
  br label %loop43

end47:                                            ; preds = %loop43
  %a58 = load i32, i32* %a
  store i32 %a58, i32* %b
  store i32 0, i32* %i
  br label %loop59

loop59:                                           ; preds = %body62, %end47
  %i60 = load i32, i32* %i
  %max_num61 = load i32, i32* @max_num
  %30 = icmp slt i32 %i60, %max_num61
  br i1 %30, label %body62, label %end63

body62:                                           ; preds = %loop59
  %b64 = load i32, i32* %b
  %i65 = load i32, i32* %i
  %31 = add i32 %b64, %i65
  store i32 %31, i32* %b
  %i66 = load i32, i32* %i
  %32 = add i32 %i66, 1
  store i32 %32, i32* %i
  br label %loop59

end63:                                            ; preds = %loop59
  %b67 = load i32, i32* %b
  %33 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @18, i32 0, i32 0), i8* getelementptr inbounds ([6 x i8], [6 x i8]* @19, i32 0, i32 0), i32 %b67)
  %34 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @20, i32 0, i32 0), i8* getelementptr inbounds ([75 x i8], [75 x i8]* @21, i32 0, i32 0))
  store i32 0, i32* %h
  store i32 1, i32* %leap
  store i32 2, i32* %m
  br label %loop68

loop68:                                           ; preds = %end91, %end63
  %m69 = load i32, i32* %m
  %max_num70 = load i32, i32* @max_num
  %35 = icmp sle i32 %m69, %max_num70
  br i1 %35, label %body71, label %end72

body71:                                           ; preds = %loop68
  %m73 = load i32, i32* %m
  %36 = sdiv i32 %m73, 2
  store i32 %36, i32* %k2
  store i32 2, i32* %i
  br label %loop74

end72:                                            ; preds = %loop68
  %h101 = load i32, i32* %h
  %37 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @26, i32 0, i32 0), i8* getelementptr inbounds ([24 x i8], [24 x i8]* @27, i32 0, i32 0), i32 %h101)
  ret void

loop74:                                           ; preds = %end86, %body71
  %i75 = load i32, i32* %i
  %k276 = load i32, i32* %k2
  %38 = icmp sle i32 %i75, %k276
  br i1 %38, label %body77, label %end78

body77:                                           ; preds = %loop74
  %m79 = load i32, i32* %m
  %i80 = load i32, i32* %i
  %39 = sdiv i32 %m79, %i80
  %i81 = load i32, i32* %i
  %40 = mul i32 %39, %i81
  store i32 %40, i32* %x2
  %m82 = load i32, i32* %m
  %i83 = load i32, i32* %i
  %41 = call i32 @mod(i32 %m82, i32 %i83)
  %42 = icmp eq i32 %41, 0
  br i1 %42, label %then84, label %else85

end78:                                            ; preds = %loop74
  %leap88 = load i32, i32* %leap
  %43 = icmp eq i32 %leap88, 1
  br i1 %43, label %then89, label %else90

then84:                                           ; preds = %body77
  store i32 0, i32* %leap
  br label %end86

else85:                                           ; preds = %body77
  br label %end86

end86:                                            ; preds = %else85, %then84
  %i87 = load i32, i32* %i
  %44 = add i32 %i87, 1
  store i32 %44, i32* %i
  br label %loop74

then89:                                           ; preds = %end78
  %m92 = load i32, i32* %m
  %45 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @22, i32 0, i32 0), i8* getelementptr inbounds ([2 x i8], [2 x i8]* @23, i32 0, i32 0), i32 %m92)
  %h93 = load i32, i32* %h
  %46 = add i32 %h93, 1
  store i32 %46, i32* %h
  %h94 = load i32, i32* %h
  %47 = sdiv i32 %h94, 10
  %48 = mul i32 %47, 10
  store i32 %48, i32* %x2
  %x295 = load i32, i32* %x2
  %h96 = load i32, i32* %h
  %49 = icmp eq i32 %x295, %h96
  br i1 %49, label %then97, label %else98

else90:                                           ; preds = %end78
  br label %end91

end91:                                            ; preds = %end99, %else90
  store i32 1, i32* %leap
  %m100 = load i32, i32* %m
  %50 = add i32 %m100, 1
  store i32 %50, i32* %m
  br label %loop68

then97:                                           ; preds = %then89
  %51 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @24, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @25, i32 0, i32 0))
  br label %end99

else98:                                           ; preds = %then89
  br label %end99

end99:                                            ; preds = %else98, %then97
  br label %end91
}

define i32 @main() {
entry:
  %n = alloca i32
  %0 = call i32 @factorial(i32 10)
  store i32 %0, i32* %n
  %n1 = load i32, i32* %n
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @28, i32 0, i32 0), i8* getelementptr inbounds ([8 x i8], [8 x i8]* @29, i32 0, i32 0), i32 %n1)
  call void @swap(i32 5, i32 10)
  call void @complete_num(i32 2)
  ret i32 0
}
