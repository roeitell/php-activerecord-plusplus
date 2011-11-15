PHP-ActiveRecord++
==================

PHP-ActiveRecord++ is an ORM extension for PHP written in C. It provides conventionalized and rapid development in the Active Record Pattern, 
while minimalizing the entitled performance penalty.

This project is a translation of the [php-activerecord](http://www.phpactiverecord.org/) project into a C extension for PHP. It is 
mainly intended for existing php-activerecord users, who can migrate and boost performance at no cost; and for developers of new applications,
looking for a good ORM.

The project is still under development. Contributors are most welcome.

ORM as a C Extension
--------------------

PHP is maturing, and conventionalized components for web applications are a standard. Yet, the advantages of rapid and conventionalized
development are sometimes moderated by performance penalties for using standard libraries and abstraction layers. This disadvantage stands 
out specifically with PHP ORMs: Heavily-conventionalized data access layers require thousands of intermediate method calls, objects and 
variables in a single request lifecycle. The heavier the data access in each request, the higher the performance penalty.

PHP-ActiveRecord++ is an ORM library which takes a different approach: We migrate the library into a C extension, and therefore reduce the performance
penalty to a minimum. We kept the entire API of a well-known ORM library available in PHP, while moving all the internal logic of the library to 
optimized C code. The project is based on the [php-activerecord](http://www.phpactiverecord.org/) project; which is, in turn, based on 
[Ruby on Rails' ActiveRecord library](http://ar.rubyonrails.org/).

Installation
------------

This project is specifically intended for existing users of php-activerecord. All that is required is to drop the inclusion of the PHP
library files, and instead, include the extension itself. The extension will provide all the existing, needed php-activerecord API.

1. Download to php's `ext/` folder, and compile (either use `phpize`, or re-compile with php `--enable-activerecord`)
2. Modify `php.ini` to refer to the extension binary
3. (For existing users - ) Drop the explicit inclusion of the PHP library from your code