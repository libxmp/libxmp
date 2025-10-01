AC_INIT
0>confdefs.h
AC_PROG_CC
AC_C_BIGENDIAN

dnl XMP_TRY_COMPILE(<message>,<cache-var>,<flags>,<program>,<ifyes>,<ifno>)
AC_DEFUN([XMP_TRY_COMPILE],[
  AC_CACHE_CHECK([$1],[$2],[
    oldcflags="${CFLAGS}"
    CFLAGS="${CFLAGS} $3"
    AC_COMPILE_IFELSE([AC_LANG_SOURCE([[$4]])],[$2=yes],[$2=no],[true])
    CFLAGS="${oldcflags}"])
  AS_IF([test "x$$2" = xyes], [$5], [$6])])

AC_DEFUN([AC_CHECK_DEFINED],[
  AS_VAR_PUSHDEF([ac_var],[ac_cv_defined_$1])dnl
  AC_CACHE_CHECK([for $1 defined], ac_var,
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [[
    #ifdef $1
    int ok;
    #else
    choke me
    #endif
  ]])],[AS_VAR_SET(ac_var, yes)],[AS_VAR_SET(ac_var, no)]))
  AS_IF([test AS_VAR_GET(ac_var) != "no"], [$2], [$3])dnl
  AS_VAR_POPDEF([ac_var])dnl
])

AC_CHECK_DEFINED(__cplusplus)

XMP_TRY_COMPILE(whether compiler understands -Wall,
  ac_cv_c_flag_w_all,
  -Wall,[
  int main(void){return 0;}],
  CFLAGS="${CFLAGS} -Wall")

XMP_TRY_COMPILE(whether compiler understands -Wextra,
  ac_cv_c_flag_w_extra,
  -Wextra,[
  int main(void){return 0;}],
  CFLAGS="${CFLAGS} -Wextra")

XMP_TRY_COMPILE(whether compiler understands -Wunused-parameter,
  ac_cv_c_flag_w_unused_parameter,
  -Wunused-parameter,[
  int main(void){return 0;}],
  CFLAGS="${CFLAGS} -Wno-unused-parameter")

XMP_TRY_COMPILE(whether compiler understands -Wsign-compare,
  ac_cv_c_flag_w_sign_compare,
  -Wsign-compare,[
  int main(void){return 0;}],
  CFLAGS="${CFLAGS} -Wno-sign-compare")

XMP_TRY_COMPILE(whether compiler understands -Wwrite-strings,
  ac_cv_c_flag_w_write_strings,
  -Wwrite-strings,[
  int main(void){return 0;}],
  CFLAGS="${CFLAGS} -Wwrite-strings")

if test "$ac_cv_defined___cplusplus" = "no"; then
  XMP_TRY_COMPILE(whether compiler understands -Wdeclaration-after-statement,
    ac_cv_c_flag_w_declaration_after_statement,
    -Wdeclaration-after-statement,[
    int main(void){return 0;}],
    CFLAGS="${CFLAGS} -Wdeclaration-after-statement")

  XMP_TRY_COMPILE(whether compiler understands -Wmissing-prototypes,
    ac_cv_c_flag_w_missing_prototypes,
    -Wmissing-prototypes,[
    int main(void){return 0;}],
    CFLAGS="${CFLAGS} -Wmissing-prototypes")

  XMP_TRY_COMPILE(whether compiler understands -Wstrict-prototypes,
    ac_cv_c_flag_w_strict_prototypes,
    -Wstrict-prototypes,[
    int main(void){return 0;}],
    CFLAGS="${CFLAGS} -Wstrict-prototypes")
fi

AC_CHECK_LIB(m,pow)
AC_CHECK_FUNCS(pipe popen mkstemp fnmatch fmemopen)
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
