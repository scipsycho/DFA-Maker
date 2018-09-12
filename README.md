
# DFA-MAKER
## Description
The following script makeDFA.sh will take input a regular expression and output a DFA in form of a PNG.
#### Notes:

1. '#' , '.' and '$'  are special symbols (without quotes) and should not be
   used in the expression.

2. Order of Precedence of the operators

	( )  ------> HIGHEST  
	 \*    
  / , . -----> LOWEST  

   ( . means concatenation )

3. Ties are broken by first come first serve i.e. operation on the left has more
   precedance than on the right
4. States will not show transition for input symbol which leads to a trap state. This is done to reduce clutter.

5. Output is given in form of PNG file named 'DFA.png' (without quotes) in the
   current directory.                       
6. For bugs/contribution go to www.github.com/scipsycho or
   mail to dsingh1664@outlook.com

#### Prerequisites
1. C++ 11 or higher compiler
2. Graphviz 'dot' installed on the system

#### Future Scope
1. Minimization of DFA
2. Incorporation of '$' as epsilon in regex.
