quarq-make
==========

Features
     - no builtin rules
     - non recursive. it can automatically load make recipes on reference to source in another directory
     - parallel builds
     - one to one, one to many, many to many target support
     - accurate and robust out fo date detection
     - uses helper programs to automatically scan and update implicit dependencies
     - can detect changes in command line options and update targets appropriately
     - not dependent on the shell
     - uses aldvanced algorithms to detect circular dependencies and execute update operations in parallel
     - has full lexical scoping and namespacing derived from dylan
