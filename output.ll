; ModuleID = 'pascal_module'
source_filename = "pascal_module"

define i32 @main() {
entry:
  %test_block = alloca [8 x double], align 8
  %test_dct = alloca [8 x double], align 8
  %test_idct = alloca [8 x double], align 8
  %dim_x = alloca i64, align 8
  %dim_y = alloca i64, align 8
  %i = alloca i64, align 8
  %j = alloca i64, align 8
}

define double @my_fabs(double %x) {
entry:
  %x_local = alloca double, align 8
  store double %x, ptr %x_local, align 8
  %return_value = alloca double, align 8
  %return_value_final = load double, ptr %return_value, align 8
  ret double %return_value_final
}

define double @p(double %x) {
entry:
  %x_local = alloca double, align 8
  store double %x, ptr %x_local, align 8
  %return_value = alloca double, align 8
  %return_value_final = load double, ptr %return_value, align 8
  ret double %return_value_final
}

define double @my_sin_impl(double %x) {
entry:
  %x_local = alloca double, align 8
  store double %x, ptr %x_local, align 8
  %return_value = alloca double, align 8
  %return_value_final = load double, ptr %return_value, align 8
  ret double %return_value_final
}

define double @my_sin(double %x) {
entry:
  %x_local = alloca double, align 8
  store double %x, ptr %x_local, align 8
  %return_value = alloca double, align 8
  %xx = alloca i64, align 8
  %return_value_final = load double, ptr %return_value, align 8
  ret double %return_value_final
}

define double @my_cos(double %x) {
entry:
  %x_local = alloca double, align 8
  store double %x, ptr %x_local, align 8
  %return_value = alloca double, align 8
  %return_value_final = load double, ptr %return_value, align 8
  ret double %return_value_final
}
