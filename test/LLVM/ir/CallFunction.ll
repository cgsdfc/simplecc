; ModuleID = 'src/CallFunction.c'
source_filename = "src/CallFunction.c"

@0 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@1 = private unnamed_addr constant [11 x i8] c"Expect 1: \00", align 1
@2 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@3 = private unnamed_addr constant [11 x i8] c"Expect 3: \00", align 1
@4 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@5 = private unnamed_addr constant [11 x i8] c"Expect 6: \00", align 1
@6 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@7 = private unnamed_addr constant [12 x i8] c"Expect 10: \00", align 1

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define i32 @function_1(i32 %i1) {
entry:
  %i11 = alloca i32
  store i32 %i1, i32* %i11
  %i12 = load i32, i32* %i11
  ret i32 %i12
}

define i32 @function_2(i32 %i1, i32 %i2) {
entry:
  %i11 = alloca i32
  store i32 %i1, i32* %i11
  %i22 = alloca i32
  store i32 %i2, i32* %i22
  %i13 = load i32, i32* %i11
  %0 = call i32 @function_1(i32 %i13)
  %i24 = load i32, i32* %i22
  %1 = add i32 %0, %i24
  ret i32 %1
}

define i32 @function_3(i32 %i1, i32 %i2, i32 %i3) {
entry:
  %i11 = alloca i32
  store i32 %i1, i32* %i11
  %i22 = alloca i32
  store i32 %i2, i32* %i22
  %i33 = alloca i32
  store i32 %i3, i32* %i33
  %i14 = load i32, i32* %i11
  %i25 = load i32, i32* %i22
  %0 = call i32 @function_2(i32 %i14, i32 %i25)
  %i36 = load i32, i32* %i33
  %1 = add i32 %0, %i36
  ret i32 %1
}

define i32 @function_4(i32 %i1, i32 %i2, i32 %i3, i32 %i4) {
entry:
  %i11 = alloca i32
  store i32 %i1, i32* %i11
  %i22 = alloca i32
  store i32 %i2, i32* %i22
  %i33 = alloca i32
  store i32 %i3, i32* %i33
  %i44 = alloca i32
  store i32 %i4, i32* %i44
  %i15 = load i32, i32* %i11
  %i26 = load i32, i32* %i22
  %i37 = load i32, i32* %i33
  %0 = call i32 @function_3(i32 %i15, i32 %i26, i32 %i37)
  %i48 = load i32, i32* %i44
  %1 = add i32 %0, %i48
  ret i32 %1
}

define i32 @main() {
entry:
  %0 = call i32 @function_1(i32 1)
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @0, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @1, i32 0, i32 0), i32 %0)
  %2 = call i32 @function_2(i32 1, i32 2)
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @2, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @3, i32 0, i32 0), i32 %2)
  %4 = call i32 @function_3(i32 1, i32 2, i32 3)
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @4, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @5, i32 0, i32 0), i32 %4)
  %6 = call i32 @function_4(i32 1, i32 2, i32 3, i32 4)
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @6, i32 0, i32 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @7, i32 0, i32 0), i32 %6)
  ret i32 0
}
