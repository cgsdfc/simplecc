; ModuleID = 'src/LocalStorage.c'
source_filename = "src/LocalStorage.c"

@0 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@1 = private unnamed_addr constant [16 x i8] c"Dump Arguments:\00", align 1
@2 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@3 = private unnamed_addr constant [11 x i8] c"Expect 1: \00", align 1
@4 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@5 = private unnamed_addr constant [11 x i8] c"Expect 2: \00", align 1
@6 = private unnamed_addr constant [6 x i8] c"%s%c\0A\00", align 1
@7 = private unnamed_addr constant [11 x i8] c"Expect a: \00", align 1
@8 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@9 = private unnamed_addr constant [22 x i8] c"Dump Local Variables:\00", align 1
@10 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@11 = private unnamed_addr constant [13 x i8] c"Expect 100: \00", align 1
@12 = private unnamed_addr constant [6 x i8] c"%s%d\0A\00", align 1
@13 = private unnamed_addr constant [13 x i8] c"Expect 101: \00", align 1
@14 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@15 = private unnamed_addr constant [18 x i8] c"Dump Local Array:\00", align 1
@16 = private unnamed_addr constant [6 x i8] c"%s%c\0A\00", align 1
@17 = private unnamed_addr constant [11 x i8] c"Expect z: \00", align 1
@18 = private unnamed_addr constant [6 x i8] c"%s%c\0A\00", align 1
@19 = private unnamed_addr constant [11 x i8] c"Expect y: \00", align 1

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define void @function(i32 %i1, i32 %i2, i8 %c3) {
entry:
  %i11 = alloca i32
  store i32 %i1, i32* %i11
  %i22 = alloca i32
  store i32 %i2, i32* %i22
  %c33 = alloca i8
  store i8 %c3, i8* %c33
  %intvar_1 = alloca i32
  %intvar_2 = alloca i32
  %chararray = alloca [2 x i8]
  store i32 100, i32* %intvar_1
  store i32 101, i32* %intvar_2
  %0 = getelementptr inbounds [2 x i8], [2 x i8]* %chararray, i32 0, i32 0
  store i8 122, i8* %0
  %1 = getelementptr inbounds [2 x i8], [2 x i8]* %chararray, i32 0, i32 1
  store i8 121, i8* %1
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i8* getelementptr inbounds ([16 x i8], [16 x i8]* @1, i32 0, i32 0))
  %i14 = load i32, i32* %i11
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @2, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @3, i32 0, i32 0), i32 %i14)
  %i25 = load i32, i32* %i22
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @4, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @5, i32 0, i32 0), i32 %i25)
  %c36 = load i8, i8* %c33
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @6, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @7, i32 0, i32 0), i8 %c36)
  %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @8, i32 0, i32 0), i8* getelementptr inbounds ([22 x i8], [22 x i8]* @9, i32 0, i32 0))
  %intvar_17 = load i32, i32* %intvar_1
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @10, i32 0, i32 0), i8* getelementptr inbounds ([13 x i8], [13 x i8]* @11, i32 0, i32 0), i32 %intvar_17)
  %intvar_28 = load i32, i32* %intvar_2
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @12, i32 0, i32 0), i8* getelementptr inbounds ([13 x i8], [13 x i8]* @13, i32 0, i32 0), i32 %intvar_28)
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @14, i32 0, i32 0), i8* getelementptr inbounds ([18 x i8], [18 x i8]* @15, i32 0, i32 0))
  %10 = getelementptr inbounds [2 x i8], [2 x i8]* %chararray, i32 0, i32 0
  %11 = load i8, i8* %10
  %12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @16, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @17, i32 0, i32 0), i8 %11)
  %13 = getelementptr inbounds [2 x i8], [2 x i8]* %chararray, i32 0, i32 1
  %14 = load i8, i8* %13
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @18, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @19, i32 0, i32 0), i8 %14)
  ret void
}

define i32 @main() {
entry:
  call void @function(i32 1, i32 2, i8 97)
  ret i32 0
}
