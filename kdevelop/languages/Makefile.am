##
## Do NOT remove the comments that start with "kdevelop:"
## They are actually directives to the kdevelop plugin system
##
## The include_xxxx variables are controlled by configure.in.in
##

if include_ada
ADA_LANGUAGE = ada
endif

if include_bash
BASH_LANGUAGE = bash
endif

if include_cpp
CPP_LANGUAGE = cpp
endif

if include_fortran
FORTRAN_LANGUAGE = fortran
endif

if include_java
JAVA_LANGUAGE = java
endif

if include_csharp
CSHARP_LANGUAGE = csharp
endif

if include_pascal
PASCAL_LANGUAGE = pascal
endif

if include_perl
PERL_LANGUAGE = perl
endif

if include_php
PHP_LANGUAGE = php
endif

if include_python
PYTHON_LANGUAGE = python
endif

if include_ruby
RUBY_LANGUAGE = ruby
endif

if include_sql
SQL_LANGUAGE = sql
endif

#kdevelop: ADA_LANGUAGE = ada
#kdevelop: BASH_LANGUAGE = bash
#kdevelop: CPP_LANGUAGE = cpp
#kdevelop: FORTRAN_LANGUAGE = fortran
#kdevelop: JAVA_LANGUAGE = java
#kdevelop: CSHARP_LANGUAGE = csharp
#kdevelop: PASCAL_LANGUAGE = pascal
#kdevelop: PERL_LANGUAGE = perl
#kdevelop: PHP_LANGUAGE = php
#kdevelop: PYTHON_LANGUAGE = python
#kdevelop: RUBY_LANGUAGE = ruby
#kdevelop: SQL_LANGUAGE = sql

SUBDIRS = lib $(ADA_LANGUAGE) $(BASH_LANGUAGE) $(CPP_LANGUAGE) $(CSHARP_LANGUAGE) \
	$(FORTRAN_LANGUAGE) $(JAVA_LANGUAGE) $(PASCAL_LANGUAGE) \
	$(PERL_LANGUAGE) $(PHP_LANGUAGE) $(PYTHON_LANGUAGE) $(RUBY_LANGUAGE) $(SQL_LANGUAGE)

DOXYGEN_EMPTY = YES
include ../Doxyfile.am
