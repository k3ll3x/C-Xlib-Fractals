#!/usr/bin/python3

import os, sys, re

function = ""
r = ""
g = ""
b = ""

if len(sys.argv) == 5:
	function = sys.argv[1]
	r = sys.argv[2]
	g = sys.argv[3]
	b = sys.argv[4]
elif len(sys.argv) == 4:
	function = sys.argv[1]
	r = sys.argv[2]
	g = sys.argv[3]
	b = "0"
elif len(sys.argv) == 3:
	function = sys.argv[1]
	r = sys.argv[2]
	g = "0"
	b = "0"
elif len(sys.argv) == 2:
	function = sys.argv[1]
#	r = "cargl(z*z+t)*iter"
#	g = "cimagl(z*z+t)*iter"
#	b = "cos(z+t)*iter"
	r = "z*iter*z*z + cos(t*z*iter)"
	g = "z*z*iter + tan(t*iter*iter)"
	b = "z*iter*iter + sin(t*z*iter*z)"
else:
	function = input("compĺex function: ")
	r = "t*iter"
	g = "t"
	b = "t*t*iter*iter"

f = open("ftmplt.c","r")
code = f.read()
f.close()

code = code.replace("PFUNCTION", function)

cf = re.findall("c\w+", function)
for i in range(len(cf)):
	function = function.replace(cf[i], "$" + str(i))
function = function.replace("z", "*z").replace("c", "*c")
for i in range(len(cf)):
	function = function.replace("$" + str(i), cf[i])

code = code.replace("FUNCTION", function)
code = code.replace("RED", r)
code = code.replace("GREEN", g)
code = code.replace("BLUE", b)

f = open("fuser.c","w")
f.write(code)
f.close()

os.popen("make fuser").read()
