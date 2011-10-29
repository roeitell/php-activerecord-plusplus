PHP_ARG_ENABLE(activerecord, enable activerecord support,
[  --enable-activerecord           Enable activerecord support])

if test "$PHP_ACTIVERECORD" != "no"; then
  PHP_NEW_EXTENSION(activerecord, activerecord.c activerecord_sql.c activerecord_utils.c activerecord_config.c, $ext_shared)
fi
