#!/bin/bash

cat logo
echo "
+---------------Operations----------------+ 
|    a*  : zero or more a                 |
|   a/b  : either a or b                  |
|   (ab) : grouping of characters         |
+-----------------------------------------+
"
echo ""
echo "Enter the regular expression: "
read input

echo $input | ./test  > .temp
dot .temp -Tpng -o DFA.png
rm .temp
