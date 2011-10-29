PHP-ActiveRecord++
==================

This project is a translation of the [php-activerecord](https://github.com/kla/php-activerecord) project into a C extension for PHP.
The project is still under development. Contributors are most welcome.

ORM as a C Extension
--------------------

Standardized components for web development have become a necessity: Applications are developed using open-source, conventionalized
frameworks and libraries. Specifically, [ORMs](http://en.wikipedia.org/wiki/Object-relational_mapping), data access layers, are now included
in any web application framework.

In PHP specifically, this poses a challenge: Performance penalties are paid for using heavily-conventionalized libraries,
and specifically ORMs. In real-life PHP applications, data access layers produce thousands of intermediate objects, method 
calls, variables allocation etc. during a single requst lifecycle. This performance penalty is usually paid to enjoy the
conventionalized use of an ORM library.

PHP-ActiveRecord++ takes a different approach: We migrate the library into a C extension, and therefore reduce the performance
penalty to a minimum. We keep all the advantages: We kept the entire API of a well-known ORM library available in PHP, while moving 
all the internal logic of the library to optimized C. The project is based on the [php-activerecord](https://github.com/kla/php-activerecord) project; 
which, in turn, is based on Ruby on Rails' ActiveRecord library.

Installation
------------

This project is specifically intended for current users of php-activerecord. All that is required is to drop the inclusion of the PHP
library files, and instead, include the extension itself. The extension will provide all the existing, needed php-activerecord API.

1. Download and compile
2. Modify php.ini to refer to the extension binary
3. Drop the inclusion of the PHP library from your code
4. Continue to use as-is