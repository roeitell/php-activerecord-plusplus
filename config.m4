dnl $Id$
dnl config.m4 for extension activerecord

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(activerecord, for activerecord support,
dnl Make sure that the comment is aligned:
dnl [  --with-activerecord             Include activerecord support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(activerecord, whether to enable activerecord support,
dnl Make sure that the comment is aligned:
dnl [  --enable-activerecord           Enable activerecord support])

if test "$PHP_ACTIVERECORD" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-activerecord -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/activerecord.h"  # you most likely want to change this
  dnl if test -r $PHP_ACTIVERECORD/$SEARCH_FOR; then # path given as parameter
  dnl   ACTIVERECORD_DIR=$PHP_ACTIVERECORD
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for activerecord files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       ACTIVERECORD_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$ACTIVERECORD_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the activerecord distribution])
  dnl fi

  dnl # --with-activerecord -> add include path
  dnl PHP_ADD_INCLUDE($ACTIVERECORD_DIR/include)

  dnl # --with-activerecord -> check for lib and symbol presence
  dnl LIBNAME=activerecord # you may want to change this
  dnl LIBSYMBOL=activerecord # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $ACTIVERECORD_DIR/lib, ACTIVERECORD_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_ACTIVERECORDLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong activerecord lib version or lib not found])
  dnl ],[
  dnl   -L$ACTIVERECORD_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(ACTIVERECORD_SHARED_LIBADD)

  PHP_NEW_EXTENSION(activerecord, activerecord.c, $ext_shared)
fi
