#!/bin/bash

echo "
+------------Special Characters-----------+ 
|    *  : zero or more previous characters| 
|    $  : Epsillon or No character        | 
|    /  : or operation                    | 
|   ()  : used to group characters        | 
| #  .  : used for calc purposes          | 
+-----------------------------------------+ "

echo "Enter the regular expression: "
read input

echo $input | ./test  > .temp
dot .temp -Tpng -o diag.png
open diag.png
