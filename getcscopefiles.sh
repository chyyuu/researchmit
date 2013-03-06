find . -type f -name "*.cc" >cscope.files
find . -type f -name "*.hh" >>cscope.files
find . -type f -name "*.[ch]" >>cscope.files
cscope -bq 
